/***********************************************
 *      VBE graphics Drivers                   *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "mem_map.h"
#include "driver.h"

#if defined(USE_VBE)

#ifdef __DJGPP__
#include <crt0.h>
int _crt0_startup_flags = _CRT0_FLAG_NEARPTR | _CRT0_FLAG_NONMOVE_SBRK;
#endif

/***************************************************************/

EXTERN VBE_MODE *Get_Mode_Info( USHORT Mode, VBE_MODE *_VBE_Info_ )
{
   USHORT Error;
   __dpmi_regs r;

   if ( Allocate_Dos_Buffer( 512 ) == 0 ) return( NULL );

   Mem_Clear( &r );
   r.x.ax = 0x4f01;
   r.x.bx = r.x.cx = Mode;
   r.x.es = Dos_Segment;
   r.x.di = 0x00;
   __dpmi_int( 0x10, &r );
   Error = r.x.ax;

   if ( Error != 0x004f )
   {
      SET_DRV_ERR2( "Could not get VESA info about mode 0x%x. ", (STRING)(UINT)Mode );
      return( NULL );
   }

   dosmemget( Dos_Data, sizeof( VBE_MODE ), (void *)_VBE_Info_ );
   return( _VBE_Info_ );
}

/***************************************************************/

EXTERN PIXEL *Get_VBE_Info( PIXEL **VInfo, INT Vbe20 )
{
   USHORT Error;
   __dpmi_regs r;

   if ( Allocate_Dos_Buffer( 512 ) == 0 ) return( NULL );

   if ( *VInfo==NULL ) *VInfo = New_Object( 512, BYTE );
   if ( *VInfo==NULL ) Exit_Upon_Mem_Error( "VBE info buffer", 512 );

   bzero( (void *)(*VInfo), 512 );

   if ( Vbe20 )
   {
      (*VInfo)[0] = 'V'; (*VInfo)[1] = 'B';
      (*VInfo)[2] = 'E'; (*VInfo)[3] = '2';
   }

   dosmemput( (void *)(*VInfo), 512, Dos_Data );
   Mem_Clear( &r );
   r.x.ax = 0x4f00;
   r.x.es = Dos_Segment;
   r.x.di = 0x00;
   __dpmi_int( 0x10, &r );
   Error = r.x.ax;

   if ( Error == 0x014F )
   {
      SET_DRV_ERR( "Could not get VBE infos" );
      return( NULL );
   }

   dosmemget( Dos_Data, 512, (void *)(*VInfo) );

   if ( (*VInfo)[0] != 'V' || (*VInfo)[1] != 'E' ||
        (*VInfo)[2] != 'S' || (*VInfo)[3] != 'A' )
   {
       SET_DRV_ERR( " no 'VESA' signature !" );
       return( NULL );
   }
   return( *VInfo );
}

/***************************************************************/

EXTERN void Vesa_Info_To_Mem_Img( MEM_IMG *M, VBE_MODE *Info )
{
   INT Depth, Quantum;
   FORMAT Format;

   IMG_Width(M) = Info->XRes;
   Quantum = Info->bytePerLine/Info->XRes;
   IMG_Height(M) = Info->YRes;

   Format = ( Info->RedMaskSize<<8 ) | ( Info->GreenMaskSize<<4 ) | ( Info->BlueMaskSize );
   Depth = Format_Depth( Format );
   if ( Depth!=Quantum ) Format = (Format & ~0xF000) | (Quantum<<12);
   IMG_Format(M) = Format;

/* TODO: Fix that. Field pos may not be == 0 ...
   M->Field_Pos = 
      ( Info->RedFieldPos<<24 ) | 
      ( Info->GreenFieldPos<<16 ) | 
      ( Info->BlueFieldPos );
*/
   if ( Info->WinFuncPtr == NULL ) IMG_BpS(M) &= ~_RM_BNK_SW_;
   if ( !(Info->ModeAttributes&0x80) ) IMG_BpS(M) &= ~_LFB_;
}

/***************************************************************/

EXTERN MEM_IMG *VBE_Detect_Available_Modes( MEM_ZONE_VBE *VBE, USHORT *Ptr )
{
   MEM_IMG *Cur;
   USHORT i, Nb_Modes, Nb_VESA;
   VBE_MODE *Info, VBE_Mode_Buffer;

      // we should be _disable()'d when getting here

   for( Nb_VESA=0; Ptr[ Nb_VESA ] != 0xFFFF; ++Nb_VESA );

   Nb_Modes = Nb_VESA;
   VBE->Nb_Modes = 0;
   Cur = VBE->Modes = New_Object( Nb_Modes, MEM_IMG );
   if ( Cur==NULL ) goto End;    // <geee!!!>
   for( i=0; i<Nb_VESA; ++i )
   {
      Info = Get_Mode_Info( Ptr[i], &VBE_Mode_Buffer );
         /*  ^^^^^^^^^^^^^^^^^^^^^ this is brain-damaged
          * with S3VBE20!!!! Geeeee :( 
          */
      if ( Info==NULL ) { Nb_Modes--; continue; }
      if ( Info->BitsPerPixel<8 ) { Nb_Modes--; continue; }
      Cur->BpS = VBE->Refresh;           // Global mask
      Vesa_Info_To_Mem_Img( Cur, Info );
      Cur->Quantum = Ptr[i]|(i<<16);     // Store Mode number in .Quantum
      Cur++;
   }

   VBE->Nb_Modes = Nb_Modes;

      // well, got some junk... Just clear that...
   VBE->Modes = My_Realloc( VBE->Modes, Nb_Modes*sizeof( MEM_IMG ) );
End:
   return( VBE->Modes );
}

/***************************************************************/
/***************************************************************/

EXTERN void VBE_Print_Mode_Info( STRING S, MEM_IMG *M )
{
#ifndef SKL_LIGHT_CODE

   Out_Message( "%s: %d x %d (F=0x%.4x)(Mode #0x%x)(#%d)",
      S, IMG_Width(M), IMG_Height(M), IMG_Format(M),
      IMG_Quantum(M)&0x7FFF, (IMG_Quantum(M)>>16)+1 );
   if ( IMG_BpS(M) & _PM_BNK_SW_ ) Out_String( "[PM BNK SW]" );
   if ( IMG_BpS(M) & _RM_BNK_SW_ ) Out_String( "[RM BNK SW]" );
   if ( IMG_BpS(M) & _INT_10h_ ) Out_String( "[int10h]" );
   if ( IMG_BpS(M) & _VGA_MODE_ ) Out_String( "[VGA]" );
   if ( IMG_BpS(M) & _SPEC_BNK_SW_ ) Out_String( "[Spc BNK SW]" );
   if ( IMG_BpS(M) & _LFB_ ) Out_String( "[LFB]" );
   if ( IMG_BpS(M) & _SPEC_LFB_ ) Out_String( "[Spc LFB]" );
   Out_String( "\n" );

#endif   // SKL_LIGHT_CODE
}

static void VBE_Print_Info( MEM_ZONE_DRIVER *M )
{
#ifndef SKL_LIGHT_CODE

   MEM_ZONE_VBE *VBE;
   INT i;

   VBE = (MEM_ZONE_VBE *)M;
   if ( VBE==NULL ) return;

   if ( VBE->Video_Card != NULL )
      Out_Message( "      --- %s ---", VBE->Video_Card->Name );

   if ( VBE->Video_Card != NULL && VBE->Video_Card->Print_Info!=NULL )
      (*VBE->Video_Card->Print_Info)( M );

   if ( !VBE->Nb_Modes ) goto Req_Modes;
   Out_Message( " * Available mode(s):" );
   for( i=0; i<VBE->Nb_Modes; ++i )
   {
      Out_String( "[%dx%d:0x%4x] \t",
         VBE->Modes[i].Width, VBE->Modes[i].Height,
         VBE->Modes[i].Format );
      if ( (i%3) == 0x02 ) Out_String( "\n" );
   }

Req_Modes:
   Drv_Print_Req_Infos( M, VBE_Print_Mode_Info );

#endif   // SKL_LIGHT_CODE
}

/***************************************************************/
/***************************************************************/

static INT VBE_Adapt( MEM_ZONE_DRIVER *M, void *Dummy )
{
   MEM_ZONE_VBE *VBE;
   INT i;
   DRIVER_ARG *Args;

   Args = (DRIVER_ARG *)Dummy;
   VBE = (MEM_ZONE_VBE *)M;
   if ( VBE==NULL ) return( 0 );

   for( i=0; i<Args->Nb_Modes; ++i )
   {
      MEM_IMG *Req;
      INT Width, Height;
      FORMAT Format, Format2;
      INT Mode_Number;
      REFRESH_MTHD Refresh_Method;
      INT j, Best;

      Req = &Args->Modes[i];
      Width = IMG_Width( Req );
      Height = IMG_Height( Req );
      if ( Width == -1 || Height == -1 ) { Width = 640; Height = 480; }
      Format = IMG_Format( Req );
      if ( Format == 0xFFFFFFFF ) Format = FMT_CMAP;

      Format2 = Format;    // A priori...

         // we want a VGA mode (W<640 and H<480)

      if ( Args->Refresh_Method & _VGA_MODE_ )
         if ( Format_Depth(Format)==1 && Width<640 && Height<480 )
         {
            Mode_Number = VBE_Search_Best_X_Mode( Req, &VBE->The_Display );
            if ( Mode_Number<0 ) goto SVGA_Modes;
            Mode_Number |= 0xFFFF<<16;
            Refresh_Method = _VGA_MODE_;
            goto Mode_Ok;
         }

SVGA_Modes:
      Best = -1;
      for( j=VBE->Nb_Modes-1; j>=0; --j )
      {
         if ( VBE->Modes[j].Format != Format ) continue;
         if ( VBE->Modes[j].Width < Width ) continue;
         if ( VBE->Modes[j].Height < Height ) continue;
         if ( Best!=-1 ) 
         {
            if ( VBE->Modes[Best].Width<VBE->Modes[j].Width ) continue;
            if ( VBE->Modes[Best].Height<VBE->Modes[j].Height ) continue;
         }
         Best = j;
      }
      if ( Best!=-1 )
      {
            // retreive temporaly stored info
         Format2 = Format;
         Refresh_Method = (REFRESH_MTHD)VBE->Modes[Best].BpS & ~_VGA_MODE_;
         Mode_Number = VBE->Modes[Best].Quantum;
         goto Mode_Ok;
      }

      if ( !Args->Convert ) 
      {
         SET_DRV_ERR2( "Convertion needed for requested mode #%d", (STRING)(INT)i+1 );
         return( 0 );
      }
      SET_DRV_ERR( "VBE2.0, Help !" );
      return( 0 );

Mode_Ok:
         // Store additional info in *Req
      Req->Pad = Format2;
      Req->BpS = Refresh_Method;
      Req->Quantum = Mode_Number; /* VESA mode number | (position in VBE->Modes[])<<16 */
   }

   return( i );
}

/***************************************************************/

static MEM_ZONE_DRIVER *VBE_Setup( _G_DRIVER_ *Drv, void *Dummy )
{
   MEM_ZONE_VBE *VBE;
   MEM_ZONE_DRIVER *M;
   PIXEL *VInfo;
   USHORT Ptr[ 512 ], *Src_Ptr;
   __dpmi_regs r;
   USHORT Error, cx, es, di;
   CARD_METHODS *Video_Card;
   INT i;
   DRIVER_ARG *Args;

   Args = (DRIVER_ARG *)Dummy;
   VInfo = NULL;
   if ( Get_VBE_Info( &VInfo, TRUE ) == NULL ) return( NULL );

   for( i=0; ; ++i )
   {   
      Video_Card = _Registered_Card_Drivers_[i];
      if ( Video_Card==_VBE_GENERIC_ ) break;      // Default
      if ( Video_Card==NULL || Video_Card->Detect==NULL ) continue;
      if ( (*Video_Card->Detect)( (VBE_INFO *)VInfo ) == 0 ) continue;
      break;
   }
   if ( Video_Card==NULL )
   {
      SET_DRV_ERR( "Video detection failure" );
      return( NULL );
   }

   M = (MEM_ZONE_DRIVER *)New_MEM_Zone( MEM_ZONE_VBE, DEFAULT_METHODS );
   if ( M==NULL ) Exit_Upon_Mem_Error( "driver MEM_ZONE", sizeof( MEM_ZONE_VBE ) );

   VBE = (MEM_ZONE_VBE *)M;
   M->Type |= MEM_IS_DISPLAY; 
   VBE->VInfo = (VBE_INFO *)VInfo;   
   VBE->Refresh = Video_Card->Refresh & Args->Refresh_Method;  // Global mask
   VBE->Video_Card = Video_Card;

         // First, Detect if protected mode func call is available

   Mem_Clear( &r );
   r.x.ax = 0x4f0a;
   __dpmi_int( 0x10, &r );
   Error = r.x.ax;
   es = r.x.es;
   di = r.x.di;
   cx = r.x.cx;

#if defined(__DJGPP__)

      // disabled with WATCOM. That suxx...

   if ( Error == 0x004F )    // It's ok...
   {
      unsigned Tab, Src_Offset;
      SHORT Len;
      BYTE *Dst;

         // Recopy code from real-mode zone to our space...

      Tab = (unsigned)SEG_OFF_FP( es, di );
      Len = cx;
      Dst = New_Object( Len, BYTE );
      if ( Dst == NULL ) goto PM_Func_Failed;

      Src_Offset = _farpeekw( _dos_ds, Tab );   // Offset is in Tab[0]
      movedata( _dos_ds, Tab+Src_Offset,
                _my_ds(), (unsigned)Dst,   Len );

      VBE->PM_WinFuncPtr = Dst;
      goto PM_Func_Done;
   }

PM_Func_Failed:

   VBE->Refresh &= ~_PM_BNK_SW_;  // Not available
   VBE->PM_WinFuncPtr = NULL;

PM_Func_Done:

#else

   VBE->Refresh &= ~_PM_BNK_SW_;  // Not available
   VBE->PM_WinFuncPtr = NULL;

#endif

      // fill RAMDAC_Width and Total_Mem...

   VBE->RAMDAC_Width = VBE->RAMDAC_Cur_Width = 6;

   if ( VInfo[10] & 0x01 )    // 8bits switchable DAC
   {
      SHORT W;

      Mem_Clear( &r );
      r.x.ax = 0x4F08;
      r.x.bx = 0x0800;        // Set to 8 bit width
      __dpmi_int( 0x10, &r );
      Error = r.x.ax;
      W = (USHORT)r.h.bh;

      if ( Error != 0x014F ) VBE->RAMDAC_Width = W;
   }
       // size in Megs...
   VBE->Total_Mem = ( *( USHORT *)( VInfo+18 ) )>>4;

      // Now, get modes infos

   {
      UINT Seg, Off;
      Seg = (UINT)( *(USHORT *)( VInfo+16 ) );
      Off = (UINT)( *(USHORT *)( VInfo+14 ) );
      Src_Ptr = ( USHORT *)SEG_OFF_FP( Seg, Off );
   }

#if defined(__DJGPP__)
   for( i=0; i<512; ++i )
   {
      USHORT Mode;

      Mode = _farpeekw( _dos_ds, (unsigned)Src_Ptr+2*i );
      if ( Mode == 0xFFFF || i==511 )
      {
         Ptr[i] = 0xFFFF;
         break;
      }
      if ( Mode == 0x00 ) // <= S3VBE20 bug workaround...grrrr...
         Ptr[i] = i + 0x100;
      else Ptr[i] = Mode;
   }

#else

   for( i=0x000; i<0x1ff; ++i ) Ptr[i] = i+0x100;   // scan all modes.
   Ptr[511] = 0xFFFF;

#endif

   if ( VBE_Detect_Available_Modes( VBE, Ptr ) == NULL ) goto Failed;

      // TODO: Complete infos about The_Display.

   VBE->The_Display.Width = 320;
   VBE->The_Display.Height = 200;
   VBE->The_Display.Format = 0x00;
   VBE->The_Display.Quantum = 5 | (0xFFFF<<16); /* _320x200_ */
   VBE->The_Display.BpS = _VGA_MODE_ | _LFB_;

   Free_Dos_Buffer( );
   M_Free( VBE->VInfo );
   /* _Disable( ); */
   return( M );

Failed:
   M_Free( VBE->VInfo );
   if ( M!=NULL ) M_Free( M );
   Free_Dos_Buffer( );
   /* _Disable( ); */
   return( NULL );
}

/***************************************************************/

static MEM_ZONE_DRIVER *Open_Mode( MEM_ZONE_DRIVER *Dummy, REFRESH_MTHD Refresh )
{
   MEM_ZONE_VBE *VBE;

   VBE = (MEM_ZONE_VBE *)Dummy;

      // Install method corresponding to special video card

   if ( Refresh & _VGA_MODE_ )
   {
      VBE->Refresh = _VGA_MODE_;
      if ( (*VBE->Video_Card->Init_VGA)(Dummy) !=NULL ) goto Ok;
   }
   if ( Refresh & _ALL_LFB_ )
   {
      VBE->Refresh = Refresh & _ALL_LFB_;
      if ( (*VBE->Video_Card->Init_LFB)(Dummy) !=NULL ) goto Ok;
   }
   if ( Refresh & _BNK_SW_ )
   {
      VBE->Refresh = _BNK_SW_ & Refresh;
      if ( (*VBE->Video_Card->Init_BNK_SW)(Dummy) !=NULL ) goto Ok;
   }
   SET_DRV_ERR( "No suitable refresh method" );
   return( NULL );

Ok:
   Format_To_Zone_Type( (MEM_ZONE *)Dummy );

   return( Dummy );
}

static MEM_ZONE *VBE_Open( MEM_ZONE_DRIVER *M, INT Nb )
{
   MEM_IMG *Req;
   FORMAT Format, Format2;
   INT Width, Height;
   USHORT Mode_Number;
   MEM_ZONE_VBE *VBE;
   REFRESH_MTHD Refresh;

   if ( M==NULL || !(M->Type&MEM_IS_DISPLAY) ) return( NULL );
   VBE = (MEM_ZONE_VBE *)M;

   if ( Nb<1 )      // Clean all
   {
      if ( VBE->Cur_Req_Mode )
      {
         if ( VBE->Refresh & _VGA_MODE_ )
            VGA_Mode_Set( 0x03 );      // back to text mode
         else 	// on my S3, switching back with VGA call jamms the display. so...
         {
            __dpmi_regs r;
            Mem_Clear( &r );
            r.x.ax = 0x4f02;
            r.x.bx = (USHORT)0x03;
            __dpmi_int( 0x10, &r );
         }
      }
      ZONE_DESTROY( M );      
      return( (MEM_ZONE *)M );
   }

   if ( Nb>VBE->Nb_Req_Modes )
   {
      SET_DRV_ERR2( "Invalid mode #%d request", (STRING)(INT)Nb );
      return( NULL );
   }

   Req = &VBE->Req_Modes[Nb-1];

   Format = IMG_Format( Req );
   Format2 = (FORMAT)IMG_Pad( Req );
   VBE->Refresh = Refresh = IMG_BpS( Req );
   Mode_Number = IMG_Quantum( Req )>>16;
   if ( Mode_Number != 0xFFFF )  /* VESA mode */
      VBE->The_Display = *( VBE->Modes + Mode_Number );
   /* else: will Display will be set in VBE_Init_VGA_Mode() */
   VBE->Mode_Number = IMG_Quantum( Req ) & 0xFFFF;

   MEM_Width(M) = IMG_Width( Req );
   MEM_Height(M) = IMG_Height( Req );
   MEM_Base_Ptr( M ) = NULL;
   MEM_Quantum(M) = Format_Depth( Format2 );
   MEM_Format(M) = Format2;

   if ( Open_Mode( M, Refresh ) == NULL ) goto Failed;
   Format_To_Zone_Type( (MEM_ZONE *)M );

#ifndef SKL_NO_CVRT

   if ( Format2!=Format )
   {
      M_Free( MEM_Base_Ptr(M) );
      if ( Mem_Img_Set( MEM_IMG(M), IMG_OWNS_PTR,
         MEM_Width(M), MEM_Height(M), 0, 0, Format, NULL )==NULL )
      {
         SET_DRV_ERR( "convertion setup failed" );
         return( NULL );
      }
      Format_To_Zone_Type( (MEM_ZONE *)M );
      // M->Methods->Propagate = VBE_Set_Position;
      if ( Format_Depth( Format ) == 1 )
      {
         Install_Index_To_Any( (MEM_ZONE *)M, Format2, NULL );
      }
      else Install_RGB_To_Any( (MEM_ZONE *)M, Format2, NULL );
   }
   else
#endif   // SKL_NO_CVRT
   {
      if ( Format_Depth( Format ) == 1 )
      {
         Install_Index_To_Any( (MEM_ZONE *)M, 0xFFFFFFFF, NULL );
      }
   }
   M->Type |= MEM_IS_DISPLAY;

   MEM_Xo(VBE) = ( VBE->The_Display.Width - MEM_Width(VBE) )/2;
   MEM_Yo(VBE) = ( VBE->The_Display.Height - MEM_Height(VBE) )/2;

   Free_Dos_Buffer( );
   _Enable( VBE->Base_Ptr, VBE->The_Display.Size );

#if defined(__DJGPP__)
   if ( Enabled==3 )     // DOSemu mode: ON. :)
   {
      if ( VBE->Refresh & _BNK_SW_ )
         Exit_Upon_Error( " Fatal: bank switch not available with DOSemu. Sorry." );
      VBE->Base_Ptr = (PIXEL *)( (unsigned)VBE->Base_Ptr - _The_Segment_ );
   }
#endif

      // Clear screen

   bzero( ZONE_SCANLINE(VBE,0), MEM_Size(VBE) );
   if ( VBE->Set_Bank==NULL )
      Safe_Bzero( VBE->Base_Ptr, VBE->Bank_Size );	// LFB.
   else
   {
      int i, Nb, Left_Over;
      Nb = VBE->The_Display.Size / VBE->Bank_Size;
      Left_Over = VBE->The_Display.Size - Nb*VBE->Bank_Size;
      for ( i=0; i<Nb; ++i )
      {
         (*VBE->Set_Bank)( VBE, i );
         Safe_Bzero( VBE->Base_Ptr, VBE->Bank_Size );
      }
      if ( Left_Over )
      {
      	(*VBE->Set_Bank)( VBE, i );
        Safe_Bzero( VBE->Base_Ptr, Left_Over );
      }
   }
   return( (MEM_ZONE *)M );

Failed:
   Free_Dos_Buffer( );
   return( NULL );
}

/***************************************************************/

static MEM_ZONE_DRIVER *VBE_Change_Size( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_VBE *VBE;

   VBE = (MEM_ZONE_VBE *)M;

   return( M );
}

/***************************************************************/

EXTERN EVENT_TYPE Get_Event_VBE( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_VBE *VBE;
   INT x, y, Buttons;
   BYTE al;
   INT cx, dx;
   __dpmi_regs Regs;

   if ( !(M->Type & MEM_IS_DISPLAY) ) return( DRV_NULL_EVENT );

   VBE = (MEM_ZONE_VBE *)M;

   VBE->Event.Event = DRV_NULL_EVENT;

      // Keys

   VBE->Event.Modifier = DRV_NO_MODIFIER;

   Mem_Clear( &Regs );
   Regs.x.ax = 0x1200;
   __dpmi_int( 0x16, &Regs );
   al = Regs.h.al;

   if ( al & DOS_SHIFT_KEY )      // Left or Right SHIFT, or SHIFT_LOCK
      VBE->Event.Modifier |= DRV_SHIFT_MODIFIER;
   if ( al & DOS_CTRL_KEY )
      VBE->Event.Modifier |= DRV_CTRL_MODIFIER;
   if ( al & DOS_ALT_KEY )
      VBE->Event.Modifier |= DRV_ALT_MODIFIER;

/*
   Mem_Clear( &Regs );   
   Regs.x.ax = 0x1100;
   __dpmi_int( 0x16, &Regs );
*/
   if ( kbhit( ) )
   {
      INT Key;
      Key = getch( );
#if 0
/*      
      Mem_Clear( &Regs );
      Regs.x.ax = 0x1000;
      __dpmi_regs( 0x16, &Regs );
 */

// fprintf( stderr, "Key: %d %c 0x%x Mod:%d\n", Key, Key, Key, VBE->Event.Modifier );

      if ( ( Key & 0x40 == 0 ) &&
             Key != '\r' && Key != '\b' &&
             Key != 8 && Key != 1 && Key != 27 )
      {
          Key |= 0x40;
      }
#endif
      VBE->Event.Event |= DRV_KEY_PRESS;
      VBE->Event.Key = ( unsigned char )( Key & 0xFF );
      Key = 0;
   }

         // Mouse

   Mem_Clear( &Regs );
   Regs.x.ax = 0x000b;
   __dpmi_int( 0x33, &Regs );
   cx = (INT)Regs.x.cx;
   dx = (INT)Regs.x.dx;

   x = VBE->Last_Event.x + cx;
   y = VBE->Last_Event.y + dx;

   if ( x<0 ) x=0;
   else if (x>=MEM_Width(M)-1 ) x = MEM_Width(M)-1;
   if ( y<0 ) y=0;
   else if (y>=MEM_Height(M)-1 ) x = MEM_Height(M)-1;

   Mem_Clear( &Regs );
   Regs.x.ax = 0x0003;
   __dpmi_int( 0x33, &Regs );
   Buttons = (INT)Regs.x.bx;

         // Moves

   if ( x != VBE->Last_Event.x || y != VBE->Last_Event.y )
   {
#if 0
      if ( Buttons & 0x01 ) VBE->Event.Event |= DRV_CLICK1;
      if ( Buttons & 0x02 ) VBE->Event.Event |= DRV_CLICK2;
      if ( Buttons & 0x04 ) VBE->Event.Event |= DRV_CLICK3;
#endif      
      VBE->Event.Event |= DRV_MOVE;
   }

         // Clicks

   if ( ( Buttons & 0x01 ) && !( VBE->Last_Event.Event & DRV_CLICK1 ) )
      VBE->Event.Event |= DRV_CLICK1;
   if ( ( Buttons & 0x02 ) && !( VBE->Last_Event.Event & DRV_CLICK2 ) )
      VBE->Event.Event |= DRV_CLICK2;
   if ( ( Buttons & 0x04 ) && !( VBE->Last_Event.Event & DRV_CLICK3 ) )
      VBE->Event.Event |= DRV_CLICK3;

         // Releases

   if ( !( Buttons & 0x01 ) && ( VBE->Last_Event.Event & DRV_CLICK1 ) )
      VBE->Event.Event |= DRV_RELEASE1;
   if ( !( Buttons & 0x02 ) && ( VBE->Last_Event.Event & DRV_CLICK2 ) )
      VBE->Event.Event |= DRV_RELEASE2;
   if ( !( Buttons & 0x04 ) && ( VBE->Last_Event.Event & DRV_CLICK3 ) )
      VBE->Event.Event |= DRV_RELEASE3;

   VBE->Event.x = x;
   VBE->Event.y = y;

   return( VBE->Event.Event );
}

static INT VBE_Change_CMap( MEM_ZONE_DRIVER *M, INT Nb, COLOR_ENTRY *CMap )
{
   MEM_ZONE_VBE *VBE;
   int RAMDAC_Shift, i;
   BYTE Pal[768], *Ptr;
   __dpmi_regs r;

   VBE = ( MEM_ZONE_VBE *)M;

   RAMDAC_Shift = 8 - VBE->RAMDAC_Cur_Width;

   if ( RAMDAC_Shift!=2 )
   {
      Mem_Clear( &r );
      r.x.ax = 0x4f09;
      r.x.bx = 0x0000;  // set palette
      r.x.cx = Nb;
      r.x.dx = 0x00;
      r.x.es = (UINT)CMap >>4;
      r.x.di = (UINT)CMap & 0x0F;
      __dpmi_int( 0x10, &r );
      if (r.x.ax != 0x004F ) RAMDAC_Shift = 2;
   }

   if ( RAMDAC_Shift==2 )
   {
#if defined(__DJGPP__)
      Ptr = Pal;
      for( i=0; i<Nb; ++i )
      {
         *Ptr++ = CMap[i][RED_F]>>RAMDAC_Shift;
         *Ptr++ = CMap[i][GREEN_F]>>RAMDAC_Shift;
         *Ptr++ = CMap[i][BLUE_F]>>RAMDAC_Shift;
      }

      asm( 
          "\tmovw $0x3c8,%%dx \n"
          "\txorl %%eax,%%eax  \n"
          "\toutb %%al,%%dx    \n"
          "\tincw %%dx        \n"
          "\tmovl $768,%%ecx   \n"
          "\tmovl %0,%%esi     \n"
          "\tcld               \n"
          "\trep               \n"
          "\toutsb             \n"
          :
          : "g" (Pal)
          : "eax", "ecx", "edx", "esi" );
#endif

#if defined(__WATCOMC__)
      outb(0x3c8, 0);
      for ( i=0; i<256; i++ )
      {
         BYTE C;
         C = CMap[i][RED_F]>>RAMDAC_Shift;
         outb(0x3c9, C );
         C = CMap[i][GREEN_F]>>RAMDAC_Shift;
         outb(0x3c9, C );
         C = CMap[i][BLUE_F]>>RAMDAC_Shift;
         outb(0x3c9, C );
      }
#endif
   }

   return( 0 );
}

/***************************************************************/

EXTERN _G_DRIVER_ __G_VBE_DRIVER_ = 
{
   VBE_Change_Size,
   Get_Event_VBE,
   VBE_Setup,
   VBE_Print_Info,
   VBE_Adapt,
   VBE_Open,
   VBE_Change_CMap
};

/***************************************************************/

#endif   // DOS && USE_VBE

