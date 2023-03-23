/***********************************************
 *             DDraw Driver                    *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"

#ifdef USE_DDRAW

#include <ddraw.h>

#undef MEM_IMAGE        // <= defined in winnt.h!!!!

#include "mem_map.h"
#include "driver.h"

/********************************************************/

static MEM_ZONE_METHODS DDRAW_Methods;

/********************************************************/

static INT DDRAW_Clean_Up( MEM_ZONE *M )
{
   MEM_ZONE_DDRAW *DDRAW;

   DDRAW = ( MEM_ZONE_DDRAW *)M;

   if ( DDRAW->Win == NULL ) return( -1 );

   if ( M->Dst != NULL )
   {
      ZONE_DESTROY( M->Dst );
      M_Free( M->Dst );
   }

   if ( MEM_Flags(M) & IMG_OWNS_PTR ) M_Free( MEM_Base_Ptr(M) );
   MEM_Flags(M) &= ~IMG_OWNS_PTR;
   Clean_Up_Zone( M );

   return( 0 );
}

static void DDRAW_Destroy( MEM_ZONE *M )
{
   MEM_ZONE_DDRAW *DDRAW;

   DDRAW = ( MEM_ZONE_DDRAW *)M;

   if ( DDRAW->Cleanup_Mode != NULL ) (*DDRAW->Cleanup_Mode)( M );
   DDRAW->Cur_Req_Mode = 0;
   DDRAW->Cleanup_Mode = NULL;

   if ( DDRAW->lpDDSPrimary != NULL ) IDirectDraw_Release( DDRAW->lpDDSPrimary );
   if ( DDRAW->lpDDPalette != NULL ) IDirectDraw_Release( DDRAW->lpDDPalette );
   if ( DDRAW->lpDD != NULL ) IDirectDraw_Release( DDRAW->lpDD );

   if ( DDRAW->Win != NULL )
   {
      CloseWindow( DDRAW->Win );
      ShowCursor( TRUE );
      DDRAW->Win = NULL;
   }             

   M_Free( DDRAW->Modes );
   DDRAW->Nb_Modes = 0;
}

static PIXEL *DDRAW_Get_Scanline( MEM_ZONE *M, INT Y )
{
   PIXEL *Ptr;

   if ( Y>=MEM_Height(M) || Y<0 ) return( NULL );

   Ptr = MEM_Base_Ptr(M);
   Ptr += Y*MEM_BpS(M);
   return( Ptr );
}

static ZONE_CLIP_TYPE DDRAW_Set_Position( MEM_ZONE *M, INT X, INT Y, INT Backup )
{
   MEM_ZONE_DDRAW *DDRAW;

      // Change window's position...
   DDRAW = ( MEM_ZONE_DDRAW *)M;
   
   if ( !DDRAW->Fullscreen )
   {
//      XMoveWindow( DDRAW->display, DDRAW->Win,
//         ( DDRAW->The_Display.Width-MEM_Width(DDRAW) )/2 + X,
//         ( DDRAW->The_Display.Height-MEM_Height(DDRAW) )/2 + Y );
   }
   MEM_Xo(DDRAW) = X;
   MEM_Yo(DDRAW) = Y;
   return( ZONE_NO_CLIP );
}

EXTERN INT DDRAW_Change_CMap( MEM_ZONE_DRIVER *M, INT Nb, COLOR_ENTRY *CMap )
{
   MEM_ZONE_DDRAW *DDRAW;
   INT i;
   PALETTEENTRY Pal[256];

   DDRAW = ( MEM_ZONE_DDRAW *)M;
   if ( DDRAW->lpDDSPrimary==NULL || DDRAW->lpDDPalette==NULL )
      return( 0 );
   for( i=Nb-1; i>=0; --i )
   {
      INT j;
      j = CMap[i][INDEX_F];
      Pal[j].peRed   = CMap[j][RED_F];
      Pal[j].peGreen = CMap[j][GREEN_F];
      Pal[j].peBlue  = CMap[j][BLUE_F];
      Pal[j].peFlags = PC_RESERVED|PC_NOCOLLAPSE;  /* ??? */
   }
   IDirectDrawSurface_SetPalette( DDRAW->lpDDSPrimary, DDRAW->lpDDPalette );
   return( 0 );
}

/********************************************************/

static INT DDRAW_Do_Flush( MEM_ZONE_DDRAW *DDRAW, INT X, INT Y, INT Width, INT Height )
{
   PIXEL *Dst, *Src;

   IDirectDrawSurface_Lock( DDRAW->lpDDSPrimary, NULL, &DDRAW->dPrim, DDLOCK_WAIT, NULL );
   Dst = (PIXEL*)DDRAW->dPrim.lpSurface;   
   Dst += MEM_Yo(DDRAW)*DDRAW->dPrim.lPitch;
   Dst += MEM_Xo(DDRAW)*DDRAW->The_Display.Quantum;
   Src = MEM_Base_Ptr(DDRAW);
   MEM_Copy_Rect( (MEM_ZONE*)DDRAW, Dst, Src, Width, Height, DDRAW->dPrim.lPitch );
   IDirectDrawSurface_Unlock( DDRAW->lpDDSPrimary, NULL );
   return( 0 );
}

static INT DDRAW_Propagate( MEM_ZONE *M, INT X, INT Y, INT Width, INT Height )
{
   if ( Width<0 ) Width = MEM_Width( M );
   if ( Height<0 ) Height = MEM_Height( M );

   if ( Clip_Zones( MEM_IMG(M), &Width, &Height, &X, &Y ) == ZONE_CLIP_OUT )
      return( -1 );
   return( DDRAW_Do_Flush( (MEM_ZONE_DDRAW *)M, X, Y, Width, Height ) );
}

static INT DDRAW_Flush( MEM_ZONE *M )
{
   return( DDRAW_Do_Flush( (MEM_ZONE_DDRAW *)M, 0, 0, MEM_Width( M ), MEM_Height( M ) ) );
}

/********************************************************/
/********************************************************/

static int Mode_Count;
static MEM_IMG *_Modes_;

static HRESULT CALLBACK ModeCount_Fnc(
   LPDDSURFACEDESC pdds, LPVOID lParam ) 
{
   FORMAT Format;

   if ( Mode_Count==256 ) return( DDENUMRET_OK );   /* !? */
   _Modes_[Mode_Count].Width = pdds->dwWidth;
   _Modes_[Mode_Count].Height = pdds->dwHeight;
   switch( pdds->ddpfPixelFormat.dwRGBBitCount )
   {
      case 15: Format = FMT_555; break;
      case 16: Format = FMT_565; break;
      case 24: Format = FMT_24b; break;
      case 32: Format = FMT_32b; break;
      default: Format = FMT_CMAP; break;
   }
   Format = Compute_Format_Depth( Format );
   _Modes_[Mode_Count].Format = Format;
   _Modes_[Mode_Count].Quantum = Format_Depth( Format );
   _Modes_[Mode_Count].BpS = pdds->lPitch;
   _Modes_[Mode_Count].Size = _Modes_[Mode_Count].Width*_Modes_[Mode_Count].BpS;
   _Modes_[Mode_Count].Pad = (INT)pdds->ddpfPixelFormat.dwRGBBitCount;
   _Modes_[Mode_Count].Base_Ptr = pdds->lpSurface;

   Mode_Count++;
   return( DDENUMRET_OK );
}

EXTERN MEM_IMG *DDRAW_Detect_Available_Modes( MEM_ZONE_DDRAW *M )
{
   int i=0;
   MEM_IMG *Cur;

      // extract Display infos

   _Modes_ = New_Object( 256, MEM_IMG );
   if ( _Modes_ == NULL )
   {
      Exit_Upon_Error( "DDRAW mode specs", 0 );
      return( NULL );
   }
   bzero( _Modes_, 256*sizeof( MEM_IMG ) );
   Mode_Count = 0;
   IDirectDraw_EnumDisplayModes( M->lpDD, 0, NULL, NULL, ModeCount_Fnc );

   M->Nb_Modes = Mode_Count;
   M->Modes = My_Realloc( _Modes_, Mode_Count*sizeof( MEM_IMG ) );
   _Modes_ = NULL;

   return( M->Modes );
}

/********************************************************/

static MEM_ZONE_DRIVER *Open_Mode( MEM_ZONE_DRIVER *Dummy )
{
   MEM_ZONE_DDRAW *DDRAW;
   INT W, H;

   DDRAW = (MEM_ZONE_DDRAW *)Dummy;
   W = MEM_Width( DDRAW );
   H = MEM_Height( DDRAW );

   if ( DDRAW->Win==NULL )  // First call. Set up everything once.
   {
      DDRAW->Win = hWndMain;
      if ( IDirectDraw_SetDisplayMode( DDRAW->lpDD, W, H, DDRAW->Bpp )!=DD_OK )
         goto Failed;

      Mem_Clear( &DDRAW->dPrim );
      DDRAW->dPrim.dwSize = sizeof( DDRAW->dPrim );
      DDRAW->dPrim.dwFlags = DDSD_CAPS;
      DDRAW->dPrim.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
      if ( IDirectDraw_CreateSurface( DDRAW->lpDD, &DDRAW->dPrim, &DDRAW->lpDDSPrimary, NULL ) != DD_OK )
         goto Failed;

               /* Color map */

#if 0
      if ( Format_Depth( MEM_Format( DDRAW ) ) == 1 )
      {
         IDirectDraw_CreatePalette( DDRAW->lpDD, DDPCAPS_8BIT|DDPCAPS_ALLOW256,
            Pal, &DDRAW->lpDDPalette, NULL );
         if ( DDRAW->lpDDPalette==NULL )
         {
CMap_Error:
            SET_DRV_ERR( "CreatePalette() failed" );
            return( NULL );
         }
      }
      else DDRAW->lpDDPalette = NULL;
#endif
   }
   else
   {
      /* XResizeWindow( DDRAW->display, DDRAW->Win, W, H );  */
   }

/*XMoveWindow( DDRAW->display, DDRAW->Win,
   ( M->The_Display.Width-MEM_Width(M) )/2,
      ( M->The_Display.Height-MEM_Height(M) )/2 );
*/

   Dummy->Cleanup_Mode = DDRAW_Clean_Up;

   Format_To_Zone_Type( (MEM_ZONE *)Dummy );

   return( Dummy );

Failed:
   return( NULL );
}

/********************************************************/

static MEM_ZONE_DRIVER *DDRAW_Change_Size( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_DDRAW *DDRAW;

   DDRAW = (MEM_ZONE_DDRAW *)M;
   if ( DDRAW==NULL ) return( NULL );

   return( (MEM_ZONE_DRIVER *)DDRAW );
}

/********************************************************/

#if 0
static EVENT_TYPE Translate_Event_DDRAW( DRV_EVENT *DRV_Event, XEvent *Event )
{
   int Type;
   UINT But;
   UINT Mod;
   KeySym  Touche;
   char    Buffer;

   Type = Event->type;

   if ( Type == KeyPress )
   {
      DRV_Event->Modifier = DRV_NO_MODIFIER;
      DRV_Event->Key = '\0';

      if ( XLookupString( ( XKeyReleasedEvent *)( Event ),
                     &Buffer, 1, &Touche, NULL ) != 1 )
      {
         if ( Touche == XK_Shift_L || Touche == XK_Shift_R )
            DRV_Event->Modifier |= DRV_SHIFT_MODIFIER;
         if ( Touche == XK_Control_L || Touche == XK_Control_R )
            DRV_Event->Modifier |= DRV_CTRL_MODIFIER;
         if ( Touche == XK_Alt_L || Touche == XK_Alt_R )
            DRV_Event->Modifier |= DRV_ALT_MODIFIER;

         if ( Touche == XK_Left ) DRV_Event->Modifier |= DRV_LEFT_MODIFIER;
         if ( Touche == XK_Right ) DRV_Event->Modifier |= DRV_RIGHT_MODIFIER;
         if ( Touche == XK_Up ) DRV_Event->Modifier |= DRV_UP_MODIFIER;
         if ( Touche == XK_Down ) DRV_Event->Modifier |= DRV_DOWN_MODIFIER;
      }
      return( DRV_Event->Event );
   }
   else if ( Type == KeyRelease )
   {
      Mod = Event->xkey.state;
      DRV_Event->Modifier = DRV_NO_MODIFIER;
      DRV_Event->Key = '\0';

      if ( Mod & ShiftMask ) DRV_Event->Modifier |= DRV_SHIFT_MODIFIER; 
      if ( Mod & ControlMask ) DRV_Event->Modifier |= DRV_CTRL_MODIFIER;
      if ( Mod & Mod1Mask ) DRV_Event->Modifier |= DRV_ALT_MODIFIER;

      if ( XLookupString( ( XKeyReleasedEvent *)( Event ),
                     &Buffer, 1, &Touche, NULL ) != 1 )
      {
         if ( DRV_Event->Modifier != DRV_NO_MODIFIER )
            DRV_Event->Key = '\0';
         return( DRV_Event->Event );
      }
/*
      fprintf( stderr, "State: 0x%x Keycode: 0x%x\n", 
         Event->xkey.keycode, Event->xkey.state );
      fprintf( stderr, "Touche: 0x%x Buffer: 0x%x\n", Touche, Buffer );
*/
      DRV_Event->Key = ( char ) Touche;
      DRV_Event->Event |= DRV_KEY_PRESS;

      return( DRV_Event->Event );
   }
   else if ( Type == MotionNotify )
   {
      XMotionEvent *MEvent;

      MEvent = ( XMotionEvent *)&( Event->xmotion );
      But    = Event->xbutton.state;
      DRV_Event->x = MEvent->x;
      DRV_Event->y = MEvent->y;

      DRV_Event->Event |= DRV_MOVE;

      if ( But & Button1Mask ) DRV_Event->Event |= DRV_CLICK1;
      else DRV_Event->Event &= ~DRV_CLICK1;
      if ( But & Button2Mask ) DRV_Event->Event |= DRV_CLICK2;
      else DRV_Event->Event &= ~DRV_CLICK2;
      if ( But & Button3Mask ) DRV_Event->Event |= DRV_CLICK3;
      else DRV_Event->Event &= ~DRV_CLICK3;
   }
   else if ( ( Type == ButtonRelease ) || ( Type == ButtonPress ) )
   {
      XButtonEvent *BEvent;
      
      BEvent = ( XButtonEvent *)&( Event->xbutton );
      DRV_Event->x = BEvent->x;
      DRV_Event->y = BEvent->y;
      
      if ( Type == ButtonRelease )
      {
         But = BEvent->state;

         if ( But & Button1Mask ) DRV_Event->Event |= DRV_RELEASE1;
         if ( But & Button2Mask ) DRV_Event->Event |= DRV_RELEASE2;
         if ( But & Button3Mask ) DRV_Event->Event |= DRV_RELEASE3;
      }
      else if ( Type == ButtonPress )
      {
         But = BEvent->button;

         if ( But & Button1 ) DRV_Event->Event |= DRV_CLICK1;
         if ( But & Button2 ) DRV_Event->Event |= DRV_CLICK2;
         if ( But & Button3 ) DRV_Event->Event |= DRV_CLICK3;
      }
      return( DRV_Event->Event );
   }
   return( DRV_Event->Event );
}
#endif

EXTERN EVENT_TYPE DDRAW_Get_Event( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_DDRAW *DDRAW;

   if ( M==NULL || !(M->Type & MEM_IS_DISPLAY) ) return( DRV_NULL_EVENT );
   DDRAW = (MEM_ZONE_DDRAW *)M;

   DDRAW->Event.Event = DRV_NULL_EVENT;

#if 0
   while( XPending( DDRAW->display )>0 )
   {
      XEvent Event;
      XNextEvent( DDRAW->display, &Event );
      if ( Event.xany.window != DDRAW->Win ) continue;
      Translate_Event_DDRAW( &DDRAW->Event, &Event );
   }
#endif
   return( DDRAW->Event.Event );
}

/********************************************************/

static void DDRAW_Print_Info( MEM_ZONE_DRIVER *M )
{
#ifndef SKL_LIGHT_CODE

   MEM_ZONE_DDRAW *DDRAW;
   INT i;

   DDRAW = (MEM_ZONE_DDRAW *)M;
   if ( DDRAW==NULL ) return;

   Out_Message( "       --- DDRAW driver ---" );

   if ( !DDRAW->Nb_Modes ) goto Req_Modes;
   Out_Message( " * Available modes:" );
   for( i=0; i<DDRAW->Nb_Modes; ++i )
   {
      char S[32];
      sprintf( S, "Mode %d", i+1 );
      Print_Zone_Specs( S, &DDRAW->Modes[i] );
   }
   if ( DDRAW->Refresh & _FULL_SCREEN_ )
      Out_Message( " Using full screen" );

Req_Modes:
   Drv_Print_Req_Infos( M, Print_Zone_Specs );

#endif   // SKL_LIGHT_CODE
}

/********************************************************/

static MEM_ZONE_DRIVER *DDRAW_Setup( _G_DRIVER_ *Drv, void *Dummy )
{
   MEM_ZONE *M;
   MEM_ZONE_DDRAW *DDRAW;
   DRIVER_ARG *Args;

   Args = (DRIVER_ARG *)Dummy;

   M = (MEM_ZONE *)New_MEM_Zone( MEM_ZONE_DDRAW, &DDRAW_Methods );
   if ( M==NULL ) Exit_Upon_Mem_Error( "driver MEM_ZONE", sizeof( MEM_ZONE_DDRAW ) );

   DDRAW = (MEM_ZONE_DDRAW *)M;
   M->Type |= MEM_IS_DISPLAY;

   DDRAW->Window_Name = Args->Window_Name;
   DDRAW->Refresh = _ALL_DDRAW_METHODS_ & Args->Refresh_Method;

   DDRAW->Refresh =  _DIRECT_X_; // hack!!

   if ( !(DDRAW->Refresh & _DIRECT_X_) ) goto Test_GDI;

            // DDRAW detection.

   if ( DirectDrawCreate( NULL, &DDRAW->lpDD, NULL ) != DD_OK )
      goto Failed;

   if ( DDRAW->Refresh & _FULL_SCREEN_ )
   {
      if ( IDirectDraw_SetCooperativeLevel( DDRAW->lpDD,
         hWndMain, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT ) != DD_OK )
            DDRAW->Refresh &= ~_FULL_SCREEN_;
      else DDRAW->Fullscreen = TRUE;
   }

   if ( !(DDRAW->Refresh & _FULL_SCREEN_) )
   {
      if ( IDirectDraw_SetCooperativeLevel( 
         DDRAW->lpDD, hWndMain, DDSCL_NORMAL ) != DD_OK )
         goto Test_GDI;
      DDRAW->Fullscreen = FALSE;
   }
   if ( DDRAW_Detect_Available_Modes( DDRAW ) == NULL ) goto Failed;
   goto Ok;

Test_GDI:
   DDRAW->Refresh &= ~_DIRECT_X_;      // remove DIRECT_X

   if ( DDRAW_Detect_Available_Modes( DDRAW ) == NULL ) goto Failed;
Ok:

   return( (MEM_ZONE_DRIVER *)DDRAW );

Failed:
   if ( DDRAW->lpDD != NULL ) IDirectDraw_Release( DDRAW->lpDD );

   M_Free( M );
   return( NULL );
}

static INT DDRAW_Adapt( MEM_ZONE_DRIVER *M, void *Dummy )
{
   MEM_ZONE_DDRAW *DDRAW;
   INT i;
   DRIVER_ARG *Args;

   Args = (DRIVER_ARG *)Dummy;
   DDRAW = (MEM_ZONE_DDRAW *)M;
   if ( DDRAW==NULL ) return( 0 );

   for( i=0; i<Args->Nb_Modes; ++i )
   {
      MEM_IMG *Req;
      INT Width, Height, The_DDraw_Mode;
      FORMAT Format;
      INT j;

      Req = &Args->Modes[i];
      Width = IMG_Width( Req );
      if ( Width == -1 ) Width = Req->Width = 320;
      Height = IMG_Height( Req );
      if ( Height == -1 ) Height = Req->Height = 200;
      Format = IMG_Format( Req );
      if ( Format == 0xFFFFFFFF ) Format = Req->Format = FMT_CMAP;

      for( j=DDRAW->Nb_Modes-1; j>=0; --j )
      {
         if ( DDRAW->Modes[j].Format != Format ) continue;
         The_DDraw_Mode = j;
         goto Mode_Ok;
      }

      if ( !Args->Convert ) 
      {
         SET_DRV_ERR2( "Convertion needed for requested mode #%d", (STRING)(INT)i+1 );
         return( 0 );
      }

      SET_DRV_ERR( "DDRAW, Help !" );
      return( 0 );

Mode_Ok:
      // Store corresponding DDraw mode in .Quantum field
      Req->Quantum = The_DDraw_Mode;
   }

   return( i );
}

static MEM_ZONE *DDRAW_Open( MEM_ZONE_DRIVER *M, INT Nb )
{
   MEM_IMG *Req;
   FORMAT Format;
   MEM_ZONE_DDRAW *DDRAW;

   if ( M==NULL || !(M->Type&MEM_IS_DISPLAY) ) return( NULL );
   DDRAW = (MEM_ZONE_DDRAW *)M;

   if ( Nb<1 )      /* Clean all */
   {
      ZONE_DESTROY( M );
      return( (MEM_ZONE *)M );
   }

   if ( Nb>DDRAW->Nb_Req_Modes )
   {
      SET_DRV_ERR2( "Invalid mode #%d request", (STRING)(INT)Nb );
      return( NULL );
   }

   Req = &DDRAW->Req_Modes[Nb-1];
   Format = IMG_Format( Req );

      /* Copy DDRAW mode into .Display */
   DDRAW->The_Display = M->Modes[ Req->Quantum ];/* Contains the DDRAW_Mode number */
   MEM_Width(M) = IMG_Width( Req );
   MEM_Height(M) = IMG_Height( Req );
   DDRAW->Bpp = Req->Pad;
   MEM_Pad(M) = 0;
   MEM_BpS( M ) = 0;
   MEM_Base_Ptr( M ) = NULL;
   MEM_Quantum(M) = Format_Depth( Format );
   MEM_Format(M) = Format;
   if ( Open_Mode( M ) == NULL ) goto Failed;

   M->Type |= MEM_IS_DISPLAY;

   if ( Format_Depth( Format ) == 1 )
   {
      Install_Index_To_Any( (MEM_ZONE *)M, 0xFFFFFFFF, NULL );
   }

   if ( DDRAW->lpDDPalette != NULL )
   {
/*
      XSetWindowColormap( DDRAW->display, DDRAW->Win, DDRAW->Cmap );
      XInstallColormap( DDRAW->display, DDRAW->Cmap );
*/
   }

   return( (MEM_ZONE *)M );

Failed:
   SET_DRV_ERR( "Open_Mode() failed" );
   return( NULL );
}

/********************************************************/

static MEM_ZONE_METHODS DDRAW_Methods = 
{
   DDRAW_Propagate,
   DDRAW_Flush,
   DDRAW_Destroy,
   DDRAW_Get_Scanline,
   DDRAW_Set_Position
};

EXTERN _G_DRIVER_ __G_DDRAW_DRIVER_ = 
{
   DDRAW_Change_Size,
   DDRAW_Get_Event,
   DDRAW_Setup,
   DDRAW_Print_Info,
   DDRAW_Adapt,
   DDRAW_Open,
   DDRAW_Change_CMap
};

/********************************************************/

#endif   // USE_DDRAW
