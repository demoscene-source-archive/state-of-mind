/***********************************************
 *      SVGALIB graphics Drivers               *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"

#ifdef USE_SVGALIB

#include "mem_map.h"
#include "driver.h"

/********************************************************/

static MEM_ZONE_METHODS SVGL_Methods;

/********************************************************/

static INT SVGL_Cleanup( MEM_ZONE *M )
{
   MEM_Flags(M) &= ~IMG_OWNS_PTR;

   Clean_Up_Zone( M );  // Nothing else ??

   return( 0 );
}

static void SVGL_Destroy( MEM_ZONE *M )
{
   MEM_ZONE_SVGL *SVGL;

   SVGL = ( MEM_ZONE_SVGL *)M;

   if ( SVGL->Cleanup_Mode != NULL ) (*SVGL->Cleanup_Mode)( M );
   SVGL->Cur_Req_Mode = 0;
   SVGL->Cleanup_Mode = NULL;

   if ( SVGL->The_SVGA_Context != ( Window )NULL ) 
   {
      gl_freecontext( SVGL->The_SVGA_Context );
      gl_freecontext( SVGL->The_Virtual_Context );
      SVGL->The_Virtual_Context = NULL;
      SVGL->The_SVGA_Context = NULL;
   }

   M_Free( SVGL->Modes );
   SVGL->Nb_Modes = 0;
}

static PIXEL *SVGL_Get_Scanline( MEM_ZONE *M, INT Y )
{
   PIXEL *Ptr;

   if ( Y>=MEM_Height(M) || Y<0 ) return( NULL );

   Ptr = MEM_Base_Ptr(M);
   Ptr += ( Y+MEM_Yo(M) )*MEM_BpS(M);
   Ptr += MEM_Xo(M)*MEM_Quantum(M);

   return( Ptr );
}

static ZONE_CLIP_TYPE SVGL_Set_Position( MEM_ZONE *M, INT X, INT Y, INT Backup )
{
   ZONE_CLIP_TYPE Ret;
   MEM_ZONE_SVGL *SVGL;

   SVGL = ( MEM_ZONE_SVGL *)M;
   Ret = ZONE_NO_CLIP;

   X += ( SVGL->The_Display.Width - MEM_Width( M ) )/2;
   Y += ( SVGL->The_Display.Height - MEM_Height( M ) )/2;;

   if ( X<0 || X+MEM_Width(M)>SVGL->The_Display.Width ) Ret |= ZONE_CLIP_X;
   if ( Y<0 || Y+MEM_Height(M)>=SVGL->The_Display.Height ) Ret |= ZONE_CLIP_Y;

   if ( Ret == ZONE_NO_CLIP ) 
   {
      MEM_Xo(M) = X;
      MEM_Yo(M) = Y;
   }   
   return( Ret );
}

static INT SVGL_Change_CMap( MEM_ZONE_DRIVER *M, INT Nb, COLOR_ENTRY *CMap )
{
   INT i;
   Palette The_Pal;

   for( i=Nb-1; i>=0; --i )
   {
      The_Pal.color[i].red   = CMap[i][RED_F]>>2;
      The_Pal.color[i].green = CMap[i][GREEN_F]>>2;
      The_Pal.color[i].blue  = CMap[i][BLUE_F]>>2;
   }
   gl_setpalette( &The_Pal );
   return( 0 );
}

EXTERN EVENT_TYPE SVGL_Get_Event( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_SVGL *SVGL;
   int K, x, y, Buttons;

   SVGL = ( MEM_ZONE_SVGL *)M;
   if ( M==NULL || !(M->Type & MEM_IS_DISPLAY) ) return( DRV_NULL_EVENT );
   if ( SVGL->The_SVGA_Context == NULL ) return( DRV_NULL_EVENT );

   SVGL->Event.Event = DRV_NULL_EVENT;

      // Keys

   SVGL->Event.Modifier = DRV_NO_MODIFIER;
   K = vga_getkey( );
   if ( K != '\0' ) {
      SVGL->Event.Key = K;
      SVGL->Event.Event |= DRV_KEY_PRESS;
   }

      // Mouse...

   mouse_update();
   x = mouse_getx(); x -= MEM_Xo( M );
   if ( x<0 ) x = 0; else if ( x>=MEM_Width(M) ) x=MEM_Width(M)-1;
   y = mouse_gety(); y -= MEM_Yo( M );
   if ( y<0 ) y = 0; else if ( y>=MEM_Height(M) ) y=MEM_Height(M)-1;

   if ( x != SVGL->Last_Event.x || y != SVGL->Last_Event.y )
      SVGL->Event.Event |= DRV_MOVE;
   SVGL->Event.x = x;
   SVGL->Event.y = y;

   Buttons = mouse_getbutton();
   if ( Buttons & MOUSE_LEFTBUTTON )
      SVGL->Event.Event |= DRV_CLICK1;
   else if ( SVGL->Last_Event.Event & DRV_CLICK1 )
      SVGL->Event.Event |= DRV_RELEASE1;

   if ( Buttons & MOUSE_MIDDLEBUTTON )
      SVGL->Event.Event |= DRV_CLICK2;
   else if ( SVGL->Last_Event.Event & DRV_CLICK2 )
      SVGL->Event.Event |= DRV_RELEASE2;

   if ( Buttons & MOUSE_RIGHTBUTTON )
      SVGL->Event.Event |= DRV_CLICK3;
   else if ( SVGL->Last_Event.Event & DRV_CLICK3 )
      SVGL->Event.Event |= DRV_RELEASE3;

   return( SVGL->Event.Event );
}

/********************************************************/

static INT BS_Propagate( MEM_ZONE *M, INT X, INT Y, INT Width, INT Height )
{
   MEM_ZONE_SVGL *SVGL;
      
   SVGL = ( MEM_ZONE_SVGL *)M;

   if ( SVGL->The_SVGA_Context!=NULL )
      gl_copyscreen( SVGL->The_SVGA_Context );

   return( TRUE );
}

static INT BS_Flush( MEM_ZONE *M )
{
   MEM_ZONE_SVGL *SVGL;
      
   SVGL = ( MEM_ZONE_SVGL *)M;

   if ( SVGL->The_SVGA_Context!=NULL )
      gl_copyscreen( SVGL->The_SVGA_Context );

   return( TRUE );
}

static INT LFB_Propagate( MEM_ZONE *M, INT X, INT Y, INT Width, INT Height )
{
   MEM_ZONE_SVGL *SVGL;
   PIXEL *Dst, *Src;
   INT Pad_Src, Pad_Dst;
   
   if ( Width<0 ) Width = MEM_Width( M );
   if ( Height<0 ) Height = MEM_Height( M );

   SVGL = ( MEM_ZONE_SVGL *)M;

   if ( Clip_Zones( MEM_IMG(M), &Width, &Height, &X, &Y ) == ZONE_CLIP_OUT )
      return( -1 );  /* Auto clip */
   Dst = SVGL->The_Display.Base_Ptr + ( Y+MEM_Yo(SVGL) )*SVGL->The_Display.BpS;
   Dst += ( X+MEM_Xo(SVGL) )*SVGL->The_Display.Quantum;
   Src = MEM_Base_Ptr(M) + Y*MEM_BpS(M);
   Src += X*MEM_Quantum(M);
   Width *= MEM_Quantum( M );
   Pad_Src = MEM_BpS( M );
   Pad_Dst = SVGL->The_Display.BpS;
   while( Height-->0 )
   {
      memcpy( Dst, Src, Width );    // !!! Beware of alignment
      Dst += Pad_Dst;
      Src += Pad_Src;
   }
   return( 0 );
}

static INT LFB_Flush( MEM_ZONE *M )
{
   PIXEL *Dst, *Src;
   MEM_ZONE_SVGL *SVGL;
   INT Pad_Src, Pad_Dst, Width, Height;

   SVGL = ( MEM_ZONE_SVGL *)M;
   Dst = SVGL->The_Display.Base_Ptr + MEM_Yo(M)*SVGL->The_Display.BpS;
   Dst += MEM_Xo(M)*SVGL->The_Display.Quantum;
   Src = MEM_Base_Ptr(M);
   Pad_Src = MEM_BpS( M );
   Pad_Dst = SVGL->The_Display.BpS;
   Width = MEM_Width( M ) * MEM_Quantum( M );      
   Height = MEM_Height( M );
   
   while( Height-->0 )
   {
      memcpy( Dst, Src, Width );    // !!! Beware of alignment
      Dst += Pad_Dst;
      Src += Pad_Src;
   }
   return( 0 );
}

/********************************************************/
/********************************************************/

static void SVGL_Print_Info( MEM_ZONE_DRIVER *M )
{
#ifndef SKL_LIGHT_CODE

   MEM_ZONE_SVGL *SVGL;
   INT i;

   SVGL = (MEM_ZONE_SVGL *)M;
   if ( SVGL==NULL ) return;

   Out_Message( "       --- SVGALIB driver ---" );

   if ( !SVGL->Nb_Modes ) goto Req_Modes;
   Out_Message( " * Available modes:" );
   for( i=0; i<SVGL->Nb_Modes; ++i )
   {
      char S[32];
      sprintf( S, "Mode %d", i+1 );
      Print_Zone_Specs( S, &SVGL->Modes[i] );
   }

Req_Modes:
   Drv_Print_Req_Infos( M, Print_Zone_Specs );

#endif   // SKL_LIGHT_CODE
}

/********************************************************/

static MEM_IMG *SVGL_Detect_Available_Modes( MEM_ZONE_SVGL *SVGL )
{
   MEM_IMG *Cur;
   USHORT i, Nb_Modes;
   vga_modeinfo *Mode_Info;

   SVGL->Nb_Modes = 0;
   Nb_Modes = GLASTMODE + 1;
   Cur = SVGL->Modes = New_Object( Nb_Modes, MEM_IMG );
   if ( Cur==NULL ) return( NULL ) ;   // <geee!!!>
   for( i=1; i<Nb_Modes; ++i )   // Don't account for TEXT mode #0
   {
      FORMAT Format;

      // if ( !vga_hasmode( i ) ) continue;
      Mode_Info = vga_getmodeinfo( i );
      if ( Mode_Info==NULL ) continue;
      if ( Mode_Info->bytesperpixel == 0 ) continue;      
      if ( Mode_Info->colors < 256 ) continue;

      Format = Mode_Info->bytesperpixel<<12;

      // Convert to MODE_SPECS

      switch( Mode_Info->colors )
      {
         case ( 1<<15 ): Format |= 0x555; break;            
         case ( 1<<16 ): Format |= 0x565; break;
         case ( 1<<24 ): Format |= 0x888; break;
         default: case 256: Format |= 0x000; break; // XXX 0x332 ??
      }

      if ( Mode_Info->flags & RGB_MISORDERED )
         Format |= FMT_REVERSED_BIT;

         // Finish template

      Cur->Width = Mode_Info->width;
      Cur->Height = Mode_Info->height;
      Cur->Pad = _INT_10h_;   // Always ok ??
      Cur->BpS = Mode_Info->maxlogicalwidth;
      if ( i<G640x480x256 ) Cur->Pad |= _VGA_MODE_;
      if ( Mode_Info->flags & CAPABLE_LINEAR )   // Linear capable
          Cur->Pad |= _SPEC_LFB_;
      Cur->Quantum = i;    // Store SVGALIB mode number into Quantum
      Cur->Format = Format;
      Cur->Size = Mode_Info->maxpixels * Mode_Info->bytesperpixel;
      Cur++;
      SVGL->Nb_Modes++;
   }
   SVGL->Modes = My_Realloc( SVGL->Modes, SVGL->Nb_Modes*sizeof( MEM_IMG ) );
   return( SVGL->Modes );
}

static MEM_ZONE_DRIVER *SVGL_Setup( _G_DRIVER_ *Drv, void *Dummy )
{
   MEM_ZONE *M;
   MEM_ZONE_SVGL *SVGL;
   DRIVER_ARG *Args;

   Args = (DRIVER_ARG *)Dummy;
   if ( geteuid() )
   {
      SET_DRV_ERR( "Not suid root" );
      return( NULL );
   }
   if ( vga_init( ) ) return( NULL );
   setuid( getuid( ) ); // temporarly give up our privileges

   M = (MEM_ZONE *)New_MEM_Zone( MEM_ZONE_SVGL, &SVGL_Methods );
   if ( M==NULL ) Exit_Upon_Mem_Error( "driver MEM_ZONE", sizeof( MEM_ZONE_SVGL ) );

   SVGL = (MEM_ZONE_SVGL *)M;
   M->Type |= MEM_IS_DISPLAY;

   SVGL->The_SVGA_Context = gl_allocatecontext( );
   if ( SVGL->The_SVGA_Context == NULL ) goto Failed;

   SVGL->The_Virtual_Context = gl_allocatecontext( );
   if ( SVGL->The_Virtual_Context == NULL ) goto Failed;

   if ( SVGL_Detect_Available_Modes( SVGL ) == NULL )
      goto Failed;

   vga_disabledriverreport( );
   vga_setmousesupport( 1 );  // Automatic setup during vga_setmode()

   return( (MEM_ZONE_DRIVER *)SVGL );

Failed:
   if ( SVGL->The_SVGA_Context!= NULL ) 
      gl_freecontext( SVGL->The_SVGA_Context );
   if ( SVGL->The_Virtual_Context != NULL )
      gl_freecontext( SVGL->The_Virtual_Context );
   if ( M!=NULL ) M_Free( M );
   return( NULL );
}

/********************************************************/

static INT SVGL_Adapt( MEM_ZONE_DRIVER *M, void *Dummy )
{
   MEM_ZONE_SVGL *SVGL;
   INT i;
   DRIVER_ARG *Args;

   Args = (DRIVER_ARG *)Dummy;
   SVGL = (MEM_ZONE_SVGL *)M;
   if ( SVGL==NULL ) return( 0 );

   for( i=0; i<Args->Nb_Modes; ++i )
   {
      MEM_IMG *Req;
      INT Width, Height;
      FORMAT Format;
      INT j, Best, The_Mode;

      Req = &Args->Modes[i];
      Width = IMG_Width( Req ); Height = IMG_Height( Req );
      if ( Width == -1 || Height == -1 ) { Width = 640; Height = 480; }
      Format = IMG_Format( Req );
      if ( Format == 0xFFFFFFFF ) Format = FMT_CMAP;

      Best = -1;
      for( j=SVGL->Nb_Modes-1; j>=0; --j )
      {
         if ( SVGL->Modes[j].Format != Format ) continue;
         if ( SVGL->Modes[j].Width < Width ) continue;
         if ( SVGL->Modes[j].Height < Height ) continue;
         if ( Best!=-1 ) 
         {
            if ( SVGL->Modes[Best].Width<SVGL->Modes[j].Width ) continue;
            if ( SVGL->Modes[Best].Height<SVGL->Modes[j].Height ) continue;
         }
         Best = j;
      }
      if ( Best!=-1 )
      {
            // retreive temporaly stored info
         The_Mode = SVGL->Modes[Best].Quantum;
         goto Mode_Ok;
      }

#ifndef FINAL
      if ( !Args->Convert ) 
      {
         SET_DRV_ERR2( "Convertion needed for requested mode #%d", (STRING)(INT)i+1 );
      }
      else if ( Args->Direct )
      {
         SET_DRV_ERR2( "Can use direct access for requested mode #%d", (STRING)(INT)i+1 );
      }

      SET_DRV_ERR( "SVGALIB, Help !" );
#endif
      return( 0 );

Mode_Ok:
         // Store additional info in *Req
      Req->Quantum = (The_Mode<<16) | Best;
      Req->Format = Format;
      if ( Args->Direct ) Req->Flags |= IMG_DIRECT;
   }

   return( i );
}

/************************************************************************************/
/************************************************************************************/

static MEM_ZONE_DRIVER *Open_Mode( MEM_ZONE_DRIVER *Dummy2 )
{
   MEM_ZONE_SVGL *SVGL;

   SVGL = (MEM_ZONE_SVGL *)Dummy2;

   setuid( geteuid() );

   gl_setcontextvgavirtual( SVGL->The_SVGA_Mode );
   gl_getcontext( SVGL->The_Virtual_Context );

   vga_setmode( SVGL->The_SVGA_Mode );
   gl_setcontextvga( SVGL->The_SVGA_Mode );
   gl_getcontext( SVGL->The_SVGA_Context );

   gl_disableclipping( );
   graph_mem = vga_getgraphmem( );   
   vga_setpage( 0 );
   // gl_clearscreen( 0 );

   if ( SVGL->The_Display.Pad & _SPEC_LFB_ )
   {
      if ( vga_setlinearaddressing( ) == -1 ) goto Bnk_SW;
      SVGL->Methods->Flush = LFB_Flush;
      SVGL->Methods->Propagate = LFB_Propagate;
      gl_setcontext( SVGL->The_SVGA_Context );
      // fprintf( stderr, "Using LFB\n" );
   }
   else
   {
Bnk_SW:
      SVGL->Methods->Flush = BS_Flush;
      SVGL->Methods->Propagate = BS_Propagate;
      gl_setcontext( SVGL->The_Virtual_Context );
   }

   SVGL->Dst = NULL;

   SVGL->The_Display.Base_Ptr = graph_mem;
   SVGL->The_Display.Quantum = Format_Depth( SVGL->The_Display.Format );
   SVGL->The_Display.Pad = SVGL->The_Display.BpS-SVGL->The_Display.Width;
   SVGL->The_Display.Pad /= SVGL->The_Display.Quantum;
   SVGL->The_Display.Size = SVGL->The_Display.BpS*SVGL->The_Display.Height;

   setuid( getuid() );

   return( Dummy2 );
}

/********************************************************/
/********************************************************/

static MEM_ZONE *SVGL_Open( MEM_ZONE_DRIVER *M, INT Nb )
{
   MEM_IMG *Req;
   FORMAT Format;
   MEM_ZONE_SVGL *SVGL;

   if ( M==NULL || !(M->Type&MEM_IS_DISPLAY) ) return( NULL );
   SVGL = (MEM_ZONE_SVGL *)M;

   if ( Nb<1 )      // Clean all
   {
      if ( SVGL->Cur_Req_Mode ) vga_setmode( TEXT );
      ZONE_DESTROY( M );
      return( (MEM_ZONE *)M );
   }

   if ( Nb>SVGL->Nb_Req_Modes )
   {
      SET_DRV_ERR2( "Invalid mode #%d request", (STRING)(INT)Nb );
      return( NULL );
   }

   Req = &SVGL->Req_Modes[Nb-1];
   Format = IMG_Format( Req );

   MEM_Width(M) = IMG_Width( Req );
   MEM_Height(M) = IMG_Height( Req );
   SVGL->The_SVGA_Mode = IMG_Quantum( Req ) >> 16;      // (Mode)
   SVGL->The_Display = SVGL->Modes[ IMG_Quantum( Req ) & 0xFFFF ];
   MEM_Quantum(M) = Format_Depth( Format );
   
   if ( Open_Mode( M ) == NULL ) goto Failed;
   Format_To_Zone_Type( (MEM_ZONE *)M );

   SVGL->Cleanup_Mode = SVGL_Cleanup;

   MEM_BpS(M) = SVGL->The_Display.Width*SVGL->The_Display.Quantum;
   MEM_Pad(M) = SVGL->The_Display.Width - MEM_Width(M);
   MEM_Size(M) = MEM_BpS(M)*MEM_Height(M);

   MEM_Base_Ptr(M) = (PIXEL *)SVGL->The_Virtual_Context->vbuf;
   bzero( MEM_Base_Ptr(M), 
      SVGL->The_Virtual_Context->height*SVGL->The_Virtual_Context->bytewidth ); 

   if ( Format_Depth( Format ) == 1 )
   {
      Install_Index_To_Any( (MEM_ZONE *)M, 0xFFFFFFFF, NULL );
   }

   M->Type |= MEM_IS_DISPLAY;

      /* Center the window */

   MEM_Xo(M) = ( SVGL->The_Display.Width - MEM_Width( M ) )/2;
   MEM_Yo(M) = ( SVGL->The_Display.Height - MEM_Height( M ) )/2;

   return( (MEM_ZONE *)M );

Failed:
   SET_DRV_ERR( "Open_Mode() failed" );
   return( NULL );
}

/********************************************************/

static MEM_ZONE_DRIVER *SVGL_Change_Size( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_SVGL *SVGL;

   SVGL = (MEM_ZONE_SVGL *)M;

   return( M );
}

/********************************************************/

static MEM_ZONE_METHODS SVGL_Methods = 
{
   BS_Propagate,
   BS_Flush,
   SVGL_Destroy,
   SVGL_Get_Scanline,
   SVGL_Set_Position,
};

EXTERN _G_DRIVER_ __G_SVGL_DRIVER_ = 
{
   SVGL_Change_Size,
   SVGL_Get_Event,
   SVGL_Setup,
   SVGL_Print_Info,
   SVGL_Adapt,
   SVGL_Open,
   SVGL_Change_CMap
};

/********************************************************/

#endif   // USE_SVGALIB

