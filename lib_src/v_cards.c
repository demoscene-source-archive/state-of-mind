/***********************************************
 *          VBE Graphics Drivers               *
 * Skal 96-98                                  *
 ***********************************************/

#include "main.h"
#include "mem_map.h"
#include "driver.h"

#ifdef USE_VBE

/********************************************************/

   // external calls in drv_bank.asm

extern void Set_Bank_Int10h( MEM_ZONE_VBE *VBE, INT Nb );
extern void Set_Bank_RM_Func( MEM_ZONE_VBE *VBE, INT Nb );
extern void Set_Bank_PM_Func( MEM_ZONE_VBE *VBE, INT Nb );
#ifdef USE_S3
extern void Set_Bank_S3( MEM_ZONE_VBE *VBE, INT Nb );
#endif

/********************************************************/
/************* GENERIC VBE Driver ***********************/
/********************************************************/

static PIXEL *VBE_Get_Scanline_Virtual( MEM_ZONE *M, INT Y )
{
   PIXEL *Ptr;

   if ( Y>=MEM_Height(M) || Y<0 ) return( NULL );

   Ptr = MEM_Base_Ptr(M);
   Ptr += Y*MEM_BpS(M);
   return( Ptr );
}

static INT VBE_Propagate_LFB( MEM_ZONE *M, INT X, INT Y, INT Width, INT Height )
{
   PIXEL *Dst, *Src;
   MEM_ZONE_VBE *VBE;
   VBE = ( MEM_ZONE_VBE *)M;

   if ( Width<0 ) Width = MEM_Width(M);
   if ( Height<0 ) Height = MEM_Height(M);

   if ( Clip_Zones( &VBE->The_Display, &Width, &Height, &X, &Y ) == ZONE_CLIP_OUT )
      return( -1 );

   Dst = VBE->Base_Ptr;
#if defined(__DJGPP__)
   if ( Enabled!=3 ) Dst += __djgpp_conventional_base;
#endif   // __DJGPP__

   Dst += ( Y+MEM_Yo(VBE) )*VBE->The_Display.BpS;
   Dst += ( X+MEM_Xo(VBE) )*VBE->The_Display.Quantum;

   Src = MEM_Base_Ptr(VBE) + Y*MEM_BpS(VBE);
   Src += X*MEM_Quantum(VBE);

#if defined(__DJGPP__)
   if ( Enabled==3 ) DOSemu_MEM_Copy_Rect( M, Dst, Src, Width, Height, VBE->The_Display.BpS );
   else MEM_Copy_Rect( M, Dst, Src, Width, Height, VBE->The_Display.BpS );
#else
   MEM_Copy_Rect( M, Dst, Src, Width, Height, VBE->The_Display.BpS );
#endif

   return( 0 );
}

static INT VBE_Flush_LFB( MEM_ZONE *M )
{
   PIXEL *Dst, *Src;
   MEM_ZONE_VBE *VBE;

   VBE = ( MEM_ZONE_VBE *)M;

   Dst = VBE->Base_Ptr;
#if defined(__DJGPP__)
   if ( Enabled!=3 ) Dst += __djgpp_conventional_base;
#endif   // __DJGPP__

   Dst += ( MEM_Yo(VBE) )*VBE->The_Display.BpS;
   Dst += ( MEM_Xo(VBE) )*VBE->The_Display.Quantum;

   Src = MEM_Base_Ptr(VBE);
#if defined(__DJGPP__)
   if ( Enabled==3 ) DOSemu_MEM_Copy_Rect( M, Dst, Src, MEM_Width(VBE), MEM_Height(VBE), VBE->The_Display.BpS );
   else MEM_Copy_Rect( M, Dst, Src, MEM_Width(VBE), MEM_Height(VBE), VBE->The_Display.BpS );
#else
   MEM_Copy_Rect( M, Dst, Src, MEM_Width(VBE), MEM_Height(VBE), VBE->The_Display.BpS );
#endif

   return( 0 );
}

static void Do_BNK_SW( MEM_ZONE_VBE *VBE, INT X, INT Y, INT Width, INT Height )
{
   INT Dst_Off, Dst_Limit;
   INT Bank_Nb;
   INT y;
   PIXEL *Dst, *Src;

   Dst = VBE->Base_Ptr;
#if defined(__DJGPP__)
   if ( Enabled!=3 ) Dst += __djgpp_conventional_base;
#endif   // __DJGPP__

   Dst_Off = ( Y+MEM_Yo(VBE) )*VBE->The_Display.BpS;
   Dst_Off += ( X+MEM_Xo(VBE) )*VBE->The_Display.Quantum;

   Src = MEM_Base_Ptr(VBE) + Y*MEM_BpS(VBE);
   Src += X*MEM_Quantum(VBE);

   Dst_Limit = VBE->Bank_Size;
   Bank_Nb = Dst_Off/Dst_Limit;
   Dst_Off -= Bank_Nb*Dst_Limit;
   if ( Bank_Nb != VBE->Cur_Bank ) (*VBE->Set_Bank)( VBE, Bank_Nb );

   if ( VBE->CMapper.Dummy==NULL || VBE->CMapper.Dummy->Col_Convert == NULL )
   {
      INT Pad1, Pad2, y;
      Pad1 = VBE->The_Display.BpS;
      Pad2 = MEM_BpS(VBE);
      Width *= MEM_Quantum(VBE);
      for( y=Height; y; --y )
      {
         if ( Dst_Off+Width>Dst_Limit )
         {
            INT x;
            x = Dst_Limit-Dst_Off;
            memcpy( Dst+Dst_Off, Src, x );
            (*VBE->Set_Bank)(VBE, ++Bank_Nb);
            memcpy( Dst, Src+x, Width-x );
            Dst_Off -= Dst_Limit;
         }
         else memcpy( Dst+Dst_Off, Src, Width );
         Dst_Off += Pad1;
         if ( Dst_Off>Dst_Limit )
         {
            (*VBE->Set_Bank)(VBE, ++Bank_Nb);
            Dst_Off -= Dst_Limit;
         }
         Src += Pad2;
      }
   }
   else
   {
      INT Pad1, Pad2, y;
      Pad1 = VBE->The_Display.BpS;
      Pad2 = MEM_BpS(VBE)/MEM_Quantum(VBE);
      Dst_Limit /= MEM_Quantum(VBE);
      Dst_Off /= MEM_Quantum(VBE);
      for( y=Height; y; --y )
      {
         if ( Dst_Off+Width>Dst_Limit )
         {
            INT x, y;
            x = ( Dst_Limit-Dst_Off );
            (*VBE->CMapper.Dummy->Col_Convert)( &VBE->CMapper, Dst+Dst_Off, Src, x );
            (*VBE->Set_Bank)(VBE, ++Bank_Nb);
            y = Width - x;
            x *= MEM_Quantum(VBE);
            (*VBE->CMapper.Dummy->Col_Convert)( &VBE->CMapper, Dst, Src+x, y );
            Dst_Off -= Dst_Limit;
         }
         else (*VBE->CMapper.Dummy->Col_Convert)( &VBE->CMapper, Dst+Dst_Off, Src, Width );
         Dst_Off += Pad1;
         if ( Dst_Off>Dst_Limit )
         {
            (*VBE->Set_Bank)(VBE, ++Bank_Nb);
            Dst_Off -= Dst_Limit;
         }
         Src += Pad2;
      }
   }
}

static INT VBE_Propagate_BNK_SW( MEM_ZONE *M, INT X, INT Y, INT Width, INT Height )
{
   MEM_ZONE_VBE *VBE;

   VBE = ( MEM_ZONE_VBE *)M;

   if ( Width<0 ) Width = MEM_Width(VBE);
   if ( Height<0 ) Height = MEM_Height(VBE);

   if ( Clip_Zones( &VBE->The_Display, &Width, &Height, &X, &Y ) == ZONE_CLIP_OUT )
      return( -1 );

   Do_BNK_SW( VBE, X, Y, Width, Height );
   return( 0 );
}

static INT VBE_Flush_BNK_SW( MEM_ZONE *M )
{
   Do_BNK_SW( (MEM_ZONE_VBE *)M, 0, 0, MEM_Width(M), MEM_Height(M) );
   return( 0 );
}

/**********************************************************************/
/**********************************************************************/

static INT VBE_Clean_Up( MEM_ZONE *M )
{
   MEM_ZONE_VBE *VBE;

   VBE = ( MEM_ZONE_VBE *)M;

   Free_Dos_Buffer( );
   _Disable( );

   if ( VBE->LFB_To_Be_Mapped != NULL )
      Unmap_Physical_Address( VBE->Base_Ptr );
   VBE->LFB_To_Be_Mapped = NULL;
   VBE->Base_Ptr = NULL;
   M_Free( VBE->VInfo );

   if ( MEM_Flags(M) & IMG_OWNS_PTR ) { M_Free( MEM_Base_Ptr( M ) ); }
   MEM_Flags(M) &= ~IMG_OWNS_PTR;
   Clean_Up_Zone( M );
   return( 0 );
}

static void VBE_Destroy( MEM_ZONE *M )
{
   MEM_ZONE_VBE *VBE;

   VBE = ( MEM_ZONE_VBE *)M;

   if ( VBE->Cleanup_Mode != NULL ) (*VBE->Cleanup_Mode)( M );
   VBE->Cur_Req_Mode = 0;
   VBE->Cleanup_Mode = NULL;

   M_Free( VBE->Modes );
   VBE->Nb_Modes = 0;

   M_Free( VBE->PM_WinFuncPtr );
   VBE->RM_WinFuncPtr = NULL;
}

static ZONE_CLIP_TYPE VBE_Set_Position( MEM_ZONE *M, INT X, INT Y, INT Backup )
{
   ZONE_CLIP_TYPE Ret;
   MEM_ZONE_VBE *VBE;

   VBE = ( MEM_ZONE_VBE *)M;
   Ret = ZONE_NO_CLIP;

   X += ( VBE->The_Display.Width - MEM_Width( M ) )/2;
   Y += ( VBE->The_Display.Height - MEM_Height( M ) )/2;;

   if ( X<0 || X+MEM_Width(M)>VBE->The_Display.Width ) Ret |= ZONE_CLIP_X;
   if ( Y<0 || Y+MEM_Height(M)>=VBE->The_Display.Height ) Ret |= ZONE_CLIP_Y;

   if ( Ret == ZONE_NO_CLIP ) 
   {
      MEM_Xo(M) = X;
      MEM_Yo(M) = Y;
   }   
   return( Ret );
}

/**********************************************************************/
/**********************************************************************/

EXTERN MEM_ZONE_METHODS VBE_Zone_Methods_LFB = 
{
   VBE_Propagate_LFB,
   VBE_Flush_LFB,
   VBE_Destroy,
   VBE_Get_Scanline_Virtual,
   VBE_Set_Position,
};

EXTERN MEM_ZONE_METHODS VBE_Zone_Methods_BNK_SW = 
{
   VBE_Propagate_BNK_SW,
   VBE_Flush_BNK_SW,
   VBE_Destroy,
   VBE_Get_Scanline_Virtual,
   VBE_Set_Position,
};

/**********************************************************************/
/**********************************************************************/

EXTERN USHORT VBE_Set_VESA_Mode( USHORT Mode, MEM_ZONE_VBE *VBE )
{
   USHORT Error;
   __dpmi_regs r;

   Mem_Clear( &r );
   r.x.ax = 0x4f02;
   r.x.bx = (USHORT)Mode;
   __dpmi_int( 0x10, &r );
   Error = r.x.ax;

   if ( Error != 0x004f )
   {
      SET_DRV_ERR2( "Couldn't init VESA mode 0x%.4x.", (STRING)(UINT)Mode );
      return( 0xFFFF );
   }

       // Set RAMDAC width ...

   if ( VBE->The_Display.Quantum==1 && VBE->RAMDAC_Width != 6 )
   {
      Mem_Clear( &r );
      r.x.ax = 0x4f08;
      r.x.bx = (SHORT)VBE->RAMDAC_Width & 0x00FF;
      __dpmi_int( 0x10, &r );
      Error = r.x.ax;

      if ( Error != 0x004f )
      {
         SET_DRV_ERR2( "Couldn't set DAC width to %d.",
            (STRING)(INT)VBE->RAMDAC_Width );
         VBE->RAMDAC_Cur_Width = 6;
      }
      else VBE->RAMDAC_Cur_Width = VBE->RAMDAC_Width; 
   }
   else VBE->RAMDAC_Cur_Width = 6;

   return( Mode );
}

EXTERN INT Do_Init_VESA_Mode( MEM_ZONE_VBE *VBE )
{
   VBE_MODE VBE_Buffer;
   VBE_MODE *Info;
   MEM_IMG *Mode;
   REFRESH_MTHD Refresh;

   VBE->LFB_To_Be_Mapped = NULL;
   Mode = MEM_IMG( VBE );

      // Mode number is in .Quantum, Refresh methods in .BpS...

   Info = Get_Mode_Info( VBE->Mode_Number, &VBE_Buffer );
   if ( Info == NULL ) return( -1 );

   Refresh = VBE->Refresh;
   VBE->Base_Ptr = NULL;

   if ( Refresh & _SPEC_LFB_ )
   {
      VBE->Bank_Size = Info->bytePerLine*Info->YRes;
      VBE->LFB_To_Be_Mapped = Info->PhysBasePtr;
      Refresh = _SPEC_LFB_;
      goto Finish;
   }

   if ( Refresh & _LFB_ )
   {
      VBE->Bank_Size = Info->bytePerLine*Info->YRes;  // Do it anyway
      VBE->LFB_To_Be_Mapped = Info->PhysBasePtr;
      Refresh = _LFB_; 
      goto Finish;
   }

   if ( Info->WinAAttributes & 4 )
      VBE->Base_Ptr = ( PIXEL *)BASE_FP( Info->WinASegment );
   else VBE->Base_Ptr = ( PIXEL *)BASE_FP( Info->WinBSegment );
   VBE->VESA_Win_No = ( Info->WinAAttributes & 4 ) ? 0 : 1;
   VBE->Bank_Size = Info->Granularity * 0x400;

   if ( (Refresh & _PM_BNK_SW_) || (Refresh & _SPEC_BNK_SW_) )
      goto Finish;

#ifndef __WATCOMC__

      // Disabled with WATCOM. That suxx. 

   if ( Refresh & (_RM_BNK_SW_|_INT_10h_ ) )
   {
      USHORT *Tmp;

      Mem_Clear( &VBE->SVGA_Regs );
      VBE->SVGA_Regs.x.ax = 0x4F05;
      VBE->SVGA_Regs.x.bx = VBE->VESA_Win_No;  // %bh == 0x00 (set window)
      VBE->SVGA_Regs.x.dx = 0;                 // starting bank
      VBE->RM_WinFuncPtr = Tmp = ( USHORT *)&Info->WinFuncPtr;
      if ( *Tmp != NULL )
      {
         VBE->SVGA_Regs.x.ip = Tmp[0];        // WinFuncPtr offset
         VBE->SVGA_Regs.x.cs = Tmp[1];        // WinFuncPtr segment
         Refresh = _RM_BNK_SW_;
         goto Finish;
      }  // ...SVGA_Regs is ready to be used to set page...
   }
#endif
   Refresh &= _INT_10h_;

Finish:

   VBE->Refresh = Refresh;

      // Finish setup

   VBE->The_Display.Width = Info->XRes;
   VBE->The_Display.Height = Info->YRes;
   VBE->The_Display.Format = IMG_Format( Mode );   // !!! Warning
   VBE->The_Display.Quantum = Format_Depth( IMG_Format( Mode ) );
   VBE->The_Display.BpS = Info->bytePerLine;
   VBE->The_Display.Pad = ( Info->bytePerLine/VBE->The_Display.Quantum - Info->XRes );
   VBE->The_Display.Size = Info->bytePerLine*Info->YRes;

   return( 0 );
}

/***************************************************************/
/***************************************************************/

static MEM_ZONE_DRIVER *VBE_Open_Mode_VGA( MEM_ZONE_DRIVER *Dummy2 )
{
   MEM_ZONE_VBE *VBE;
   PIXEL *Ptr;

   VBE = (MEM_ZONE_VBE *)Dummy2;

   MEM_Quantum(VBE) = 1;
   Ptr = New_Object( MEM_Width(VBE)*MEM_Height(VBE)*MEM_Quantum(VBE), PIXEL );
   if ( Ptr==NULL ) return( NULL );

   VBE_Init_VGA_Mode( VBE->Mode_Number, VBE );

   if ( Mem_Img_Set( MEM_IMG(VBE), 
      IMG_OWNS_PTR, MEM_Width(VBE), MEM_Height(VBE), 
      0, MEM_Quantum(VBE), MEM_Format(VBE), Ptr ) == NULL ) return( NULL );

   *Dummy2->Methods = VBE_Zone_Methods_LFB;
   Dummy2->Cleanup_Mode = VBE_Clean_Up;

   VBE->LFB_To_Be_Mapped = NULL;
   VBE->Set_Bank = NULL;
   return( Dummy2 );
}

static MEM_ZONE_DRIVER *VBE_Open_Mode_BNK_SW( MEM_ZONE_DRIVER *Dummy2 )
{
   MEM_ZONE_VBE *VBE;
   PIXEL *Ptr;
   INT i;
   VBE = (MEM_ZONE_VBE *)Dummy2;

   Ptr = New_Object( MEM_Width(VBE)*MEM_Height(VBE)*MEM_Quantum(VBE), PIXEL );
   if ( Ptr==NULL ) return( NULL );

   if ( Do_Init_VESA_Mode( VBE ) ) return( NULL );

     // eventually set Bank switch method if needed
   if ( VBE->Refresh & _PM_BNK_SW_ ) VBE->Set_Bank = Set_Bank_PM_Func;
   else if ( VBE->Refresh & _RM_BNK_SW_ ) VBE->Set_Bank = Set_Bank_RM_Func;
   else if ( VBE->Refresh & _INT_10h_ ) VBE->Set_Bank = Set_Bank_Int10h;

   if ( VBE_Set_VESA_Mode( VBE->Mode_Number, VBE )==0xFFFF )
      return( NULL );

      // TODO: Pad to 8, here

   if ( Mem_Img_Set( MEM_IMG(VBE), 
      IMG_OWNS_PTR, MEM_Width(VBE), MEM_Height(VBE), 
      0, MEM_Quantum(VBE), MEM_Format(VBE), Ptr ) == NULL ) return( NULL );

   *Dummy2->Methods = VBE_Zone_Methods_BNK_SW;
   Dummy2->Cleanup_Mode = VBE_Clean_Up;

   VBE->LFB_To_Be_Mapped = NULL;
   (*VBE->Set_Bank)( VBE, 0 );

   return( Dummy2 );
}

static MEM_ZONE_DRIVER *VBE_Open_Mode_LFB( MEM_ZONE_DRIVER *Dummy2 )
{
   MEM_ZONE_VBE *VBE;
   PIXEL *Ptr;
   VBE = (MEM_ZONE_VBE *)Dummy2;

   Ptr = New_Object( MEM_Width(VBE)*MEM_Height(VBE)*MEM_Quantum(VBE), PIXEL );
   if ( Ptr==NULL ) return( NULL );

   if ( Do_Init_VESA_Mode( VBE ) )
   {
      	// for some unknown reason, LFB init failed, although theoretically
      	// available... Return error (and do the Bank Switch if possible... )
Failed:
      M_Free( Ptr );
      return( NULL );
   }

   if ( VBE_Set_VESA_Mode( VBE->Mode_Number | 0x4000, VBE )==0xFFFF )
      goto Failed;
   VBE->Base_Ptr = Map_Physical_Address( VBE->Bank_Size, VBE->LFB_To_Be_Mapped );
   if ( VBE->Base_Ptr == (PIXEL*)0x0001 ) goto Failed;

printf( "To_Be_Mapped:0x%x (%d bytes) =>  Base_Ptr=>0x%x\n", VBE->LFB_To_Be_Mapped, VBE->Bank_Size, VBE->Base_Ptr );

      // TODO: Pad to 8, here

   if ( Mem_Img_Set( MEM_IMG(VBE), 
      IMG_OWNS_PTR, MEM_Width(VBE), MEM_Height(VBE), 
      0, MEM_Quantum(VBE), MEM_Format(VBE), Ptr ) == NULL ) 
      goto Failed;

   *Dummy2->Methods = VBE_Zone_Methods_LFB;
   Dummy2->Cleanup_Mode = VBE_Clean_Up;

   VBE->Set_Bank = NULL;

   return( Dummy2 );
}

/**********************************************************/

void VBE_Print_Info( MEM_ZONE_DRIVER *M )
{
#ifndef SKL_LIGHT_CODE

   MEM_ZONE_VBE *VBE;
   PIXEL *VInfo;
   char S[ 256 ];

   VBE = (MEM_ZONE_VBE *)M;
   VInfo = NULL;
   if ( Get_VBE_Info( &VInfo, TRUE ) == NULL ) return;

   Out_Message( "  # Version: %c%c%c%c %d.%d\n  # Type: %s",
      VInfo[0], VInfo[1], VInfo[2], VInfo[3], VInfo[5], VInfo[4],
      _Retreive_String( VInfo+6, 256, S ) );
   Out_Message( "  # %s (Currently: With=%d)",
      VInfo[10] & 0x01 ? "8bits switchable DAC" : "Fixed Width DAC" ,
      VBE->RAMDAC_Cur_Width );
   Out_Message( "  # %d megs of adressable mem.", *( USHORT *)( VInfo+18 )>>4 );

   if ( VInfo[5] >= 2 )    // VBE2 only
   {
      Out_String( "  - Product name: %s", _Retreive_String( VInfo+26, 256, S ) );
      Out_Message( " ( Rev.: %s )", _Retreive_String( VInfo+30, 256, S ) );
      Out_Message( "  - Vendor name: %s", _Retreive_String( VInfo + 22, 256, S ) );
      Out_Message( "  - Software rev. %d.%d", VInfo[21], VInfo[20] );
   }
   Out_Message( "\n" );
   Free_Dos_Buffer( );
   M_Free( VInfo );   
   _Disable( );

#endif   // SKL_LIGHT_CODE
}

/**********************************************************/

CARD_METHODS _VBE_Methods_ = { 
   "Generic VBE2.0 support v0.1",
   _INT_10h_ | _RM_BNK_SW_ | _PM_BNK_SW_ | _LFB_ | _VGA_MODE_,
   NULL,    // no need for special detect
   VBE_Print_Info,
   VBE_Open_Mode_VGA, VBE_Open_Mode_LFB,
   VBE_Open_Mode_BNK_SW
};
#define _VBE_GENERIC_   (&_VBE_Methods_)


/************************************************************
 ********************** S3 Driver ***************************
 **                                                        **
 ** You must #define USE_S3 in the makefile, or elsewhere, **
 ** and uncomment the Set_Bank_S3 func in drv_bank.asm     **
 **                                                        **
 ** disabled, for now. Needs some fixing up...             **
 **                                                        **
 ************************************************************
 ************************************************************/

#ifdef USE_S3

// #define S3_EXT_ON    \
//   asm( " movw $0x03d4,%%dx\n" " movw $0x4838,%%ax\n" " outw %%ax,%%dx\n" \
//  : : : "eax", "edx" );       \
//  asm( " movw $0x03d4,%%dx\n" " movw $0xa539,%%ax\n" " outw %%ax,%%dx\n" \
//  : : : "eax", "edx" )
//#define S3_EXT_OFF   \
//asm( " movw $0x03d4,%%dx\n" " movw $0x0039,%%ax\n" " outw %%ax,%%dx\n" \
//  : : : "eax", "edx" );       \
//  asm( " movw $0x03d4,%%dx\n" " movw $0x0038,%%ax\n" " outw %%ax,%%dx\n" \
//  : : : "eax", "edx" )

#define S3_EXT_ON   { outw( 0x03d4, 0x4838 ); outw( 0x03d4, 0xa539 ); }
#define S3_EXT_OFF  { outw( 0x03d4, 0x0039 ); outw( 0x03d4, 0x0038 ); }

static void S3_Map_Physical( PIXEL *Ptr, INT m )
{
   S3_EXT_ON;
   // outCR( 0x31, 0x08 );
   outCR( 0x58, ( inCR( 0x58 ) & 0xEC ) );
   outCR( 0x59, ( ((UINT)Ptr)>>24 )&0xFF );
   outCR( 0x5A, ( ((UINT)Ptr)>>16 )&0xFF );
   outCR( 0x58, ( inCR( 0x58 ) & 0xEC ) | m | 0x10 );
   outCR( 0x40, inCR( 0x40 ) | 0x08 );    // Fast flush on
   S3_EXT_OFF;
}

static INT S3_Detect( VBE_INFO *Info )
{
   USHORT Error;
   PIXEL al;
   __dpmi_regs r;

   Mem_Clear( &r );
   r.x.ax = 0x1DAA;
   r.x.bx = 0xFDEC;
   __dpmi_int( 0x10, &r );
   Error = r.x.bx;
   al = r.h.al;

   if ( Error == 0xCEDF ) return( TRUE );

      // Stealth

   if ( al != 0x01 || al != 0x02 )
      return ( FALSE );

   return( TRUE );
}

static INT VBE_Clean_Up_S3( MEM_ZONE *M )
{
   MEM_ZONE_VBE *VBE;

   VBE = ( MEM_ZONE_VBE *)M;

   Free_Dos_Buffer( );
   _Disable( );

   if ( VBE->LFB_To_Be_Mapped != NULL )
      Unmap_Physical_Address( VBE->Base_Ptr );
   VBE->LFB_To_Be_Mapped = NULL;

   VBE->Base_Ptr = NULL;
   M_Free( VBE->VInfo );

   if ( MEM_Flags(M) & IMG_OWNS_PTR ) { M_Free( MEM_Base_Ptr( M ) ); }
   MEM_Flags(M) &= ~IMG_OWNS_PTR;
   Clean_Up_Zone( M );
   return( 0 );
}

static MEM_ZONE_DRIVER *S3_Open_Mode_VGA( MEM_ZONE_DRIVER *Dummy2 )
{
   MEM_ZONE_DRIVER *VBE;

   VBE = VBE_Open_Mode_VGA( Dummy2 );
   if ( VBE == NULL ) return( NULL );
   S3_Map_Physical( ((MEM_ZONE_VBE *)VBE )->Base_Ptr, 0 );
   return( VBE );
}

static MEM_ZONE_DRIVER *S3_Open_Mode_BNK_SW( MEM_ZONE_DRIVER *Dummy2 )
{
   MEM_ZONE_VBE *VBE;
   PIXEL *Ptr;

   VBE = (MEM_ZONE_VBE *)Dummy2;

   Ptr = New_Object( MEM_Width(VBE)*MEM_Height(VBE)*MEM_Quantum(VBE), PIXEL );
   if ( Ptr==NULL ) return( NULL );

   VBE->Set_Bank = Set_Bank_S3;

   if ( Do_Init_VESA_Mode( VBE ) ) return( NULL );
   if ( VBE_Set_VESA_Mode( VBE->Mode_Number, VBE )==0xFFFF )
      return( NULL );

      // TODO: Pad to 8, here

   if ( Mem_Img_Set( MEM_IMG(VBE), 
      IMG_OWNS_PTR, MEM_Width(VBE), MEM_Height(VBE), 
      0, 1, MEM_Format(VBE), Ptr ) == NULL ) return( NULL );

   *Dummy2->Methods = VBE_Zone_Methods_BNK_SW;
   Dummy2->Cleanup_Mode = VBE_Clean_Up_S3;

   VBE->LFB_To_Be_Mapped = NULL;
   (*VBE->Set_Bank)( VBE, 0 );

   return( Dummy2 );
}

static MEM_ZONE_DRIVER *S3_Open_Mode_LFB( MEM_ZONE_DRIVER *Dummy2 )
{
   MEM_ZONE_VBE *VBE;
   PIXEL *Ptr, *LFB;
   INT m;

   VBE = (MEM_ZONE_VBE *)Dummy2;

   Ptr = New_Object( MEM_Width(VBE)*MEM_Height(VBE)*MEM_Quantum(VBE), PIXEL );
   if ( Ptr==NULL ) return( NULL );

   if ( Do_Init_VESA_Mode( VBE ) ) return( NULL );
   if ( VBE_Set_VESA_Mode( VBE->Mode_Number, VBE )==0xFFFF )
      return( NULL );

   S3_EXT_ON;
   LFB = VBE->LFB_To_Be_Mapped =
      (PIXEL *)( (inCR( 0x59 )<<24) | (inCR( 0x5A )<<16) );
   S3_EXT_OFF;

   VBE->Base_Ptr = Map_Physical_Address( VBE->Bank_Size, VBE->LFB_To_Be_Mapped );
   if ( VBE->Base_Ptr==0x0001 ) return( NULL );

   S3_Map_Physical( LFB, 3 );
// fprintf( stderr, "S3_LFB=0x%x", LFB );

      // TODO: Pad to 8, here
   if ( Mem_Img_Set( MEM_IMG(VBE), 
      IMG_OWNS_PTR, MEM_Width(VBE), MEM_Height(VBE), 
      0, 1, MEM_Format(VBE), Ptr ) == NULL ) return( NULL );

   *Dummy2->Methods = VBE_Zone_Methods_LFB;
   Dummy2->Cleanup_Mode = VBE_Clean_Up_S3;

   return( Dummy2 );
}

/**********************************************************/

void S3_Print_Info( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_VBE *VBE;
   char S[256];
   USHORT Error;
   PIXEL ah, al;
   __dpmi_regs r;
   USHORT si, di;

   VBE = (MEM_ZONE_VBE *)M;

   Mem_Clear( &r );
   r.x.ax = 0x1DAA;
   r.x.bx = 0xFDEC;
   __dpmi_int( 0x10, &r );
   Error = r.x.bx;
   ah = r.h.ah;
   al = r.h.al;
   si = r.x.si;
   di = r.x.di;

   if ( Error != 0xCEDF ) goto Stealth;

   Out_String( "   # DAC type: " );
   switch( ah & 0x0F )
   {
      case 0x00: Out_Message( " Normal." ); break;
      case 0x01: Out_Message( " Sierra." ); break;
      case 0x02: Out_Message( " Diamond SS24." ); break;
      case 0x03: Out_Message( " Bt485 (135MHz). " ); break;
      case 0x04: Out_Message( " STG1700." ); break;
      case 0x05: Out_Message( " STG1702." ); break;
      case 0x06: Out_Message( " AT&T 20c49x. " ); break;
      case 0x07: Out_Message( " STG1703." ); break;
      case 0x08: Out_Message( " S3 SDAC." ); break;
      case 0x09: Out_Message( " Bt485 (175Mhz). " ); break;
   }
   Out_Message( "   # %dMb of mem.", al );

   Out_Message( "   # Bus type: %s.",
     ( ah & 0x10 ) == 0x10 ? "PCI" : "VESA VLB" );

   goto Print_Copyright_S3;

Stealth:

   if ( al != 0x01 )
      Out_String( "   # Stealth VRAM: " );
   else Out_String( "   # Stealth 24: " );

   switch( ah )
   {
      case 0x00: Out_Message( "Standard VGA DAC." ); break;
      case 0x11: Out_Message( "Highcolor DAC. Com. Reg. non writable." ); break;
      case 0x23: Out_Message( "SS2410 DAC." ); break;
      case 0x33: Out_Message( "Highcolor DAC without RS2." ); break;
      case 0x43: Out_Message( "Highcolor DAC with RS2." ); break;
   }

Print_Copyright_S3:
   Out_Message( "   # %s", _Retreive_String2( si, di, 256, S ) );
}

/**********************************************************/

CARD_METHODS _S3_Methods_ = {
   "S3 support v0.1", 
   _SPEC_BNK_SW_ | _SPEC_LFB_ | _VGA_MODE_,
   NULL, /*S3_Detect, */
   S3_Print_Info,
   S3_Open_Mode_VGA, S3_Open_Mode_LFB,
   S3_Open_Mode_BNK_SW
};

#define _S3_DRIVER_   (&_S3_Methods_)

/**********************************************************/

#else

#define _S3_DRIVER_ NULL

// CARD_METHODS _S3_Methods_ = { "S3 no-support", NULL };

#endif   // USE_S3

/**********************************************************/
/*           Other potential drivers... (later)           */
/**********************************************************/


// etc...

// CARD_METHODS _ET4000_Methods_ = { "ET4000 support v0.0", NULL };
// CARD_METHODS _ET6000_Methods_ = { "ET6000 support v0.0", NULL };
// CARD_METHODS _MATROX_Methods_ = { "MATROX support v0.0", NULL };
// #define _ET4000_DRIVER_ (&_ET4000_Methods_)
// #define _ET6000_DRIVER_ (&_ET6000_Methods_)
// #define _MATROX_DRIVER_ (&_MATROX_Methods_)


   // For now, just disable

#define _ET4000_DRIVER_ NULL
#define _ET6000_DRIVER_ NULL
#define _MATROX_DRIVER_ NULL

/**********************************************************/
/**********************************************************/
/**********************************************************/

EXTERN CARD_METHODS *_Registered_Card_Drivers_[] =
{
   _S3_DRIVER_, _ET4000_DRIVER_,
   _ET6000_DRIVER_, _MATROX_DRIVER_,

   _VBE_GENERIC_        // <= always last
};

/**********************************************************/
/**********************************************************/

#endif   // USE_VBE
