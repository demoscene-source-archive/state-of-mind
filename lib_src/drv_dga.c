/***********************************************
 *      DGA graphics Drivers                   *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"

#ifdef USE_DGA

#include "mem_map.h"
#include "driver.h"

/********************************************************/

#define DGA_WAIT_OK(D)  { XEvent Event;   \
while( XPending( (D) ) ) XNextEvent( (D), &Event ); }

#define MASKS_DGA KeyReleaseMask | KeyPressMask       \
                 /* | ButtonPressMask*/ | ButtonReleaseMask    \
                    | ButtonMotionMask | PointerMotionMask

static MEM_ZONE_METHODS DGA_Methods;

/********************************************************/

static INT DGA_Cleanup( MEM_ZONE *M )
{
   MEM_Flags(M) &= ~IMG_OWNS_PTR;

   Clean_Up_Zone( M );

      /* Nothing else ?? */

   return( 0 );
}

static void DGA_Destroy( MEM_ZONE *M )
{
   MEM_ZONE_DGA *DGA;

   DGA = ( MEM_ZONE_DGA *)M;

   if ( DGA->Cleanup_Mode != NULL ) (*DGA->Cleanup_Mode)( M );
   DGA->Cur_Req_Mode = 0;
   DGA->Cleanup_Mode = NULL;

   if ( DGA->Win != ( Window )NULL ) 
   {
      XF86DGADirectVideo( DGA->display, DefaultScreen( DGA->display ), 0x0 );
      XUngrabKeyboard( DGA->display, CurrentTime );
      if ( DGA->Cells )
         XFreeColors( DGA->display, DGA->Cmap, DGA->Pixels, 256, 0 );
      DGA->Cells = 0;
      XFreeColormap( DGA->display, DGA->Cmap );      
      M_Free( DGA->Pixels );
      DGA->Win = ( Window )NULL;
   }
   if ( DGA->display != NULL ) XCloseDisplay( DGA->display );
   DGA->display = NULL;

   DGA->Base_Ptr = NULL;

   M_Free( DGA->Modes );
   DGA->Nb_Modes = 0;
}

static PIXEL *DGA_Get_Scanline( MEM_ZONE *M, INT Y )
{
   PIXEL *Ptr;

   if ( Y>=MEM_Height(M) || Y<0 ) return( NULL );

   Ptr = MEM_Base_Ptr(M);
   Ptr += ( Y+MEM_Yo(M) )*MEM_BpS(M);
   Ptr += MEM_Xo(M)*MEM_Quantum(M);

   return( Ptr );
}

static ZONE_CLIP_TYPE DGA_Set_Position( MEM_ZONE *M, INT X, INT Y, INT Backup )
{
   ZONE_CLIP_TYPE Ret;
   MEM_ZONE_DGA *DGA;

   DGA = ( MEM_ZONE_DGA *)M;
   Ret = ZONE_NO_CLIP;

   X += ( DGA->The_Display.Width - MEM_Width( M ) )/2;
   Y += ( DGA->The_Display.Height - MEM_Height( M ) )/2;;

   if ( X<0 || X+MEM_Width(M)>DGA->The_Display.Width ) Ret |= ZONE_CLIP_X;
   if ( Y<0 || Y+MEM_Height(M)>=DGA->The_Display.Height ) Ret |= ZONE_CLIP_Y;

   if ( Ret == ZONE_NO_CLIP ) 
   {
      MEM_Xo(M) = X;
      MEM_Yo(M) = Y;
   }   
   return( Ret );
}

static INT DGA_Change_CMap( MEM_ZONE_DRIVER *M, INT Nb, COLOR_ENTRY *CMap )
{
   MEM_ZONE_DGA *DGA;

   DGA = ( MEM_ZONE_DGA *)M;

   if ( X11_Change_CMap( M, Nb, CMap ) ) return( -1 );
   XF86DGAInstallColormap( DGA->display, DGA->screen, DGA->Cmap );

   return( 0 );
}

/********************************************************/

static INT DGA_Propagate( MEM_ZONE *M, INT X, INT Y, INT Width, INT Height )
{
   MEM_ZONE_DGA *DGA;

      
   DGA = ( MEM_ZONE_DGA *)M;

/*
   {
      XEvent Event;
      if ( !XCheckTypedWindowEvent( DGA->display, DGA->Win,
         DGA->Completion_Type, &Event ) ) return( FALSE );
   }
*/
   return( TRUE );
}

static INT DGA_Flush( MEM_ZONE *M )
{
   MEM_ZONE_DGA *DGA;
   XEvent Event;
      
   DGA = ( MEM_ZONE_DGA *)M;

   return( TRUE );
}

/********************************************************/

static INT DGA_Cleanup_Virtual( MEM_ZONE *M )
{
   MEM_Flags(M) &= ~IMG_OWNS_PTR;
   M_Free( MEM_Base_Ptr(M) );
   Clean_Up_Zone( M );
   *M->Methods = DGA_Methods;
   return( 0 );
}

static PIXEL *DGA_Get_Scanline_Virtual( MEM_ZONE *M, INT Y )
{
   PIXEL *Ptr;

   if ( Y>=MEM_Height(M) || Y<0 ) return( NULL );

   Ptr = MEM_Base_Ptr(M);
   Ptr += Y*MEM_BpS(M);

   return( Ptr );
}

static INT DGA_Propagate_Virtual( MEM_ZONE *M, INT X, INT Y, INT Width, INT Height )
{
   MEM_ZONE_DGA *DGA;
   PIXEL *Dst, *Src;

   if ( Width<0 ) Width = MEM_Width( M );
   if ( Height<0 ) Height = MEM_Height( M );

   DGA = ( MEM_ZONE_DGA *)M;

   if ( Clip_Zones( MEM_IMG(M), &Width, &Height, &X, &Y ) == ZONE_CLIP_OUT )
      return( -1 );  /* Auto clip */
   Dst = DGA->Base_Ptr + ( Y+MEM_Yo(DGA) )*DGA->The_Display.BpS;
   Dst += ( X+MEM_Xo(DGA) )*DGA->The_Display.Quantum;
   Src = MEM_Base_Ptr(M) + Y*MEM_BpS(M);
   Src += X*MEM_Quantum(M);
   MEM_Copy_Rect( M, Dst, Src, Width, Height, DGA->The_Display.BpS );
   return( 0 );
}

static INT DGA_Flush_Virtual( MEM_ZONE *M )
{
   PIXEL *Dst, *Src;
   MEM_ZONE_DGA *DGA;

   DGA = ( MEM_ZONE_DGA *)M;
   Dst = DGA->Base_Ptr + MEM_Yo(M)*DGA->The_Display.BpS;
   Dst += MEM_Xo(M)*DGA->The_Display.Quantum;
   Src = MEM_Base_Ptr(M);
   MEM_Copy_Rect( M, Dst, Src, MEM_Width(M), MEM_Height(M), DGA->The_Display.BpS );
   return( 0 );
}

/********************************************************/
/********************************************************/

static void DGA_Print_Info( MEM_ZONE_DRIVER *M )
{
#ifndef SKL_LIGHT_CODE

   MEM_ZONE_DGA *DGA;
   INT i;

   DGA = (MEM_ZONE_DGA *)M;
   if ( DGA==NULL ) return;

   Out_Message( "       --- DGA v%d.%d driver ---", DGA->Maj, DGA->Min  );
   X11_Print_Zone_Specs( " * Display", &DGA->The_Display );

   if ( !DGA->Nb_Modes ) goto Req_Modes;
   Out_Message( " * Available modes:" );
   for( i=0; i<DGA->Nb_Modes; ++i )
   {
      char S[32];
      sprintf( S, "Mode %d", i+1 );
      X11_Print_Zone_Specs( S, &DGA->Modes[i] );
   }

Req_Modes:
   Drv_Print_Req_Infos( M, X11_Print_Zone_Specs );

#endif   // SKL_LIGHT_CODE
}

/********************************************************/

static MEM_ZONE_DRIVER *DGA_Setup( _G_DRIVER_ *Drv, void *Dummy )
{
   MEM_ZONE *M;
   MEM_ZONE_DGA *DGA;
   Display *display;
   int Flags, i;
   DRIVER_ARG *Args;

   Args = (DRIVER_ARG *)Dummy;
   if ( geteuid() )
   {
      SET_DRV_ERR( "Not suid root" );
      return( NULL );
   }

   if ( Args->Display_Name == NULL )
      Args->Display_Name = (STRING)XDisplayName( NULL );
   display = X11_Open_Display( Args->Display_Name );
   if ( display == NULL ) return( NULL );

   M = (MEM_ZONE *)New_MEM_Zone( MEM_ZONE_DGA, &DGA_Methods );
   if ( M==NULL ) Exit_Upon_Mem_Error( "driver MEM_ZONE", sizeof( MEM_ZONE_DGA ) );

   DGA = (MEM_ZONE_DGA *)M;
   M->Type |= MEM_IS_DISPLAY;

   DGA->Base_Ptr = NULL;
   DGA->display = display;
   DGA->screen = DefaultScreen( display );

   i = XF86DGAQueryDirectVideo( display, DefaultScreen( display ), &Flags );
   if ( !i || !( Flags&XF86DGADirectPresent ) ) goto Failed;

   if (    !XF86DGAQueryVersion( DGA->display, &DGA->Maj, &DGA->Min )
        || !XF86DGAQueryExtension( DGA->display, 
               &DGA->Completion_Type, &DGA->Error_Base ) 
      )
      goto Failed;

/*
   if ( XF86DGAForkApp( DGA->screen ) )
   {
      P_Error( "XF86DGAForkApp" );
      return( 0 );
   }
*/

   if ( X11_Detect_Available_Modes( (MEM_ZONE_X11*)DGA ) == NULL ) goto Failed;

   XF86DGAGetViewPortSize( DGA->display, DGA->screen,
      (int *)&DGA->The_Display.Width, (int *)&DGA->The_Display.Height );

   for( i=0; i<DGA->Nb_Modes; ++i )  /* Force mode size to that of ViewPort */
   {
      DGA->Modes[i].Width = DGA->The_Display.Width;
      DGA->Modes[i].Height = DGA->The_Display.Height;
   }

   DGA->Window_Name = Args->Window_Name;
   return( (MEM_ZONE_DRIVER *)DGA );

Failed:
   if ( M!=NULL ) M_Free( M );
   if ( display != NULL ) XCloseDisplay( display );
   return( NULL );
}

/********************************************************/

static INT DGA_Adapt( MEM_ZONE_DRIVER *M, void *Dummy )
{
   MEM_ZONE_DGA *DGA;
   Visual *visual;
   int depth;
   INT i;
   DRIVER_ARG *Args;

   Args = (DRIVER_ARG *)Dummy;
   DGA = (MEM_ZONE_DGA *)M;
   if ( DGA==NULL ) return( 0 );

   for( i=0; i<Args->Nb_Modes; ++i )
   {
      MEM_IMG *Req;
      INT Width, Height;
      FORMAT Format, Format2;
      INT j;

      Req = &Args->Modes[i];
      Width = IMG_Width( Req );
      if ( Width == -1 ) Width = Req->Width = DGA->The_Display.Width;
      Height = IMG_Height( Req );
      if ( Height == -1 ) Height = Req->Height = DGA->The_Display.Height;
      if ( Width > DGA->The_Display.Width ||
         Height > DGA->The_Display.Height )
      {
         SET_DRV_ERR( "oversized mode for available display" );
         return( 0 );
      }
      Format = IMG_Format( Req );
      if ( Format == 0xFFFFFFFF ) Format = Req->Format = DGA->The_Display.Format;

      Format2 = Format; /* A priori... */

      for( j=DGA->Nb_Modes-1; j>=0; --j )
      {
         if ( DGA->Modes[j].Format != Format ) continue;
         if ( DGA->Modes[j].Width < Width ) continue;
         if ( DGA->Modes[j].Height < Height ) continue;
         visual = (Visual *)DGA->Modes[j].Base_Ptr;
         depth = DGA->Modes[j].Quantum;
         goto Mode_Ok;
      }

      if ( !Args->Convert ) 
      {
         SET_DRV_ERR2( "Convertion needed for requested mode #%d", (STRING)(INT)i+1 );
         return( 0 );
      }

      if ( Args->Direct )
      {
         SET_DRV_ERR2( "Can use direct access for requested mode #%d", (STRING)(INT)i+1 );
         return( 0 );
      }

#ifndef SKL_NO_CVRT

          /* Search for possible convertion */

      if ( (Format&0xFFFF) == FMT_CMAP )
      {
         Format2 = IMG_Format( &DGA->The_Display );
         visual = (Visual *)DGA->The_Display.Base_Ptr;
         depth = DGA->The_Display.Quantum;          
      }                         
      else if ( Format_Depth( IMG_Format( &DGA->The_Display ) )==1 )
      {
         Format2 = FMT_332;
         visual = DefaultVisual( DGA->display, DGA->screen );
         depth = DGA->The_Display.Quantum;
      }
      else 
      {
         INT Best;
         Best = -1;
         Format2 = 0x0000;
         for( j=DGA->Nb_Modes-1; j>=0; --j )
         {
            if ( ( DGA->Modes[j].Format&0xFFFF ) == FMT_CMAP ) continue;
            if ( DGA->Modes[j].Width < Width ) continue;
            if ( DGA->Modes[j].Height < Height ) continue;
            if ( Format2 <= DGA->Modes[j].Format )
            {   Format2 = DGA->Modes[j].Format; Best = j; }

         }
         if ( Best<0 )
         {
            SET_DRV_ERR2( "Can't convert requested mode #%d", (STRING)(INT)i+1 );
            return( 0 );
         }
         visual = (Visual *)DGA->Modes[Best].Base_Ptr;
         depth = DGA->Modes[Best].Quantum;                           
      }

#endif   // SKL_NO_CVRT

Mode_Ok:
         /* Store additional info in *Req */
      Req->Pad = Format2;
      Req->Quantum = depth;
      Req->Base_Ptr = (PIXEL *)visual;
      if ( Args->Direct ) Req->Flags |= IMG_DIRECT;
   }

   return( i );
}

/************************************************************************************/
/************************************************************************************/

static MEM_ZONE_DRIVER *Open_Mode( MEM_ZONE_DRIVER *Dummy2 )
{
   MEM_ZONE_DGA *M;

   M = (MEM_ZONE_DGA *)Dummy2;

      /* Finish setup */

   if ( M->Base_Ptr == NULL )
   {
      if ( !XF86DGAGetVideo( 
         M->display, M->screen,
         (char **)&(M->Base_Ptr), &M->Line_Width,
         &M->Bank_Size, &M->Mem_Size ) )
      {
         P_Error( "XF86DGAGetVideo" ); /* Fatal, here. */
         return( NULL );
      }

      if ( M->Bank_Size >= M->Mem_Size )
      {
         M->Bank_Nb = 0; /* Linear buffer */
         M->Left_Over = 0;
      }
      else
      {     /* Banked video */
         M->Bank_Nb = M->Mem_Size/M->Bank_Size;
         M->Left_Over = M->Mem_Size - M->Bank_Size*M->Bank_Nb;
      }
      M->The_Display.Quantum = Format_Depth( M->The_Display.Format );
      M->The_Display.Pad = M->Line_Width-M->The_Display.Width;
      M->The_Display.BpS = M->Line_Width*M->The_Display.Quantum;
      M->The_Display.Size = M->The_Display.BpS*M->The_Display.Height;

      M->The_Display.Flags |= IMG_VIRTUAL;
   }

   if ( M->Win == (Window)NULL )
   {
         /* First call. Set up everything once. */

      M->Win = DefaultRootWindow( M->display );
      XSelectInput( M->display, M->Win, MASKS_DGA );
/*
      XGrabPointer( M->display, M->Win, 
         True, PointerMotionMask,
         GrabModeAsync, GrabModeAsync, 
         None, None, CurrentTime );
*/

      M->Cmap = XCreateColormap( 
            M->display, 
            M->Win,
            M->visual,
            AllocNone );
      M->Cells = 0;

            /* Color map */

      if ( Format_Depth( M->The_Display.Format ) == 1 )
      {
         M->Pixels = New_Object( 256, unsigned long );
         if ( M->Pixels == NULL ) goto CMap_Error;
         M->Cells = XAllocColorCells( 
            M->display, M->Cmap, FALSE, 
            NULL, 0,
            M->Pixels, 256 );
         if ( !M->Cells )
         {
CMap_Error:
            Out_Message( "XAllocColorCells() failed" );
            M_Free( M->Pixels );
            return( NULL );
         }
      }

      XGrabKeyboard( M->display, M->Win,
         True, GrabModeAsync, GrabModeAsync, CurrentTime );

      if ( !XF86DGADirectVideo( M->display, M->screen,
         XF86DGADirectGraphics | XF86DGADirectMouse | XF86DGADirectKeyb ) )
      {
         P_Error( "XF86DGADirectVideo" );
         return( NULL );
      }

/* not needed...
      X11_Undefine_Cursor( (MEM_ZONE_X11 *)M );
      XStoreName( M->display, M->Win, M->Window_Name );
      XMapWindow( M->display, M->Win );
      XRaiseWindow( M->display, M->Win );
*/

         /* Give up root privileges */
      setuid( getuid() );      

      BZero( M->Base_Ptr, M->Bank_Size );
   }

   XF86DGASetViewPort( M->display, M->screen, 0, 0 );

   XFlush( M->display );
   XSync( M->display, False );
   DGA_WAIT_OK( M->display );

   M->Dst = NULL;

   return( Dummy2 );
}

/********************************************************/
/********************************************************/

static MEM_ZONE *DGA_Open( MEM_ZONE_DRIVER *M, INT Nb )
{
   MEM_IMG *Req;
   FORMAT Format, Format2;
   MEM_ZONE_DGA *DGA;

   if ( M==NULL || !(M->Type&MEM_IS_DISPLAY) ) return( NULL );
   DGA = (MEM_ZONE_DGA *)M;

   if ( Nb<1 )      /* Clean all */
   {
      ZONE_DESTROY( M );
      return( (MEM_ZONE *)M );
   }

   if ( Nb>DGA->Nb_Req_Modes )
   {
      SET_DRV_ERR2( "Invalid mode #%d request", (STRING)(INT)Nb );
      return( NULL );
   }

   Req = &DGA->Req_Modes[Nb-1];
   Format = IMG_Format( Req );
   Format2 = (FORMAT)Req->Pad;

   MEM_Width(M) = IMG_Width( Req );
   MEM_Height(M) = IMG_Height( Req );
   DGA->depth = IMG_Quantum( Req );      /* depth */
   DGA->visual = (Visual *)IMG_Base_Ptr( Req );    /* visual */
   MEM_Quantum(M) = Format_Depth( Format2 );
   MEM_Format(M) = Format2;

   if ( Open_Mode( M ) == NULL ) goto Failed;
   Format_To_Zone_Type( (MEM_ZONE *)M );

   DGA->Cleanup_Mode = DGA_Cleanup;

   MEM_Pad(M) = DGA->The_Display.Width + DGA->The_Display.Pad - MEM_Width(M);
   MEM_BpS(M) = DGA->Line_Width*MEM_Quantum(M);
   MEM_Size(M) = MEM_Height(M)*MEM_BpS( M );
   MEM_Base_Ptr( M ) = DGA->Base_Ptr;

#ifndef SKL_NO_CVRT

   if ( Format2!=Format )
   {
         /* Now, fill M->Zone with real desired format */
      if ( Mem_Img_Set( MEM_IMG(M), IMG_OWNS_PTR,
         MEM_Width(M), MEM_Height(M), 0, 0, Format, NULL )==NULL )
      {
         SET_DRV_ERR( "convertion setup failed" );
         return( NULL );
      }
      Format_To_Zone_Type( (MEM_ZONE *)M );
      M->Methods->Propagate = DGA_Propagate_Virtual;
      M->Methods->Flush = DGA_Flush_Virtual;
      M->Methods->Get_Scanline = DGA_Get_Scanline_Virtual;
      /* M->Methods->Set_Position = DGA_Set_Position; */
      M->Cleanup_Mode = DGA_Cleanup_Virtual;

      if ( Format_Depth( Format ) == 1 )
      {
         Install_Index_To_Any( (MEM_ZONE *)M, Format2, NULL );
      }
      else Install_RGB_To_Any( (MEM_ZONE *)M, Format2, NULL );
   }
   else
#endif   // SKL_NO_CVRT
   if ( !(Req->Flags & IMG_DIRECT) )
   {
      /* !!!! TEST */
      if ( Mem_Img_Set( MEM_IMG(M), IMG_OWNS_PTR,
         MEM_Width(M), MEM_Height(M), 0, 0, Format, NULL )==NULL )
         goto Failed;
      Format_To_Zone_Type( (MEM_ZONE *)M );
      M->Methods->Propagate = DGA_Propagate_Virtual;
      M->Methods->Flush = DGA_Flush_Virtual;
      M->Methods->Get_Scanline = DGA_Get_Scanline_Virtual;
      M->Cleanup_Mode = DGA_Cleanup_Virtual;
/* End of TEST */

      if ( Format_Depth( Format ) == 1 )
         Install_Index_To_Any( (MEM_ZONE *)M, 0xFFFFFFFF, NULL );
   }
   else
   {
      MEM_Flags( M ) = IMG_VIRTUAL | IMG_DIRECT;
      // MEM_Base_Ptr( M ) = DGA->Base_Ptr;  /* LFB */
      Format_To_Zone_Type( (MEM_ZONE *)M );
      M->Methods->Propagate = DGA_Propagate;
      M->Methods->Flush = DGA_Flush;
      M->Methods->Get_Scanline = DGA_Get_Scanline;
      M->Cleanup_Mode = NULL;
      if ( Format_Depth( Format ) == 1 )
         Install_Index_To_Any( (MEM_ZONE *)M, 0xFFFFFFFF, NULL );
   }
   M->Type |= MEM_IS_DISPLAY;

      /* Center the window */

   MEM_Xo(M) = ( DGA->The_Display.Width - MEM_Width( M ) )/2;
   MEM_Yo(M) = ( DGA->The_Display.Height - MEM_Height( M ) )/2;

   XF86DGAInstallColormap( DGA->display, DGA->screen, DGA->Cmap );

   return( (MEM_ZONE *)M );

Failed:
   SET_DRV_ERR( "Open_Mode() failed" );
   return( NULL );
}

/********************************************************/

static MEM_ZONE_DRIVER *DGA_Change_Size( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_DGA *DGA;

   DGA = (MEM_ZONE_DGA *)M;

   return( M );
}
EXTERN EVENT_TYPE DGA_Get_Event( MEM_ZONE_DRIVER *DGA )
{
   INT x, y;
   EVENT_TYPE Ev;

   x = DGA->Event.x; DGA->Event.x = 0;
   y = DGA->Event.y; DGA->Event.y = 0;
   Ev = X11_Get_Event( DGA );
   DGA->Event.x += x;
   if ( DGA->Event.x<0 ) DGA->Event.x = 0;
   else if ( DGA->Event.x>=MEM_Width( DGA ) )
      DGA->Event.x = MEM_Width( DGA )-1;
   DGA->Event.y += y;
   if ( DGA->Event.y<0 ) DGA->Event.y = 0;
   else if ( DGA->Event.y>=MEM_Height( DGA ) )
      DGA->Event.y = MEM_Height( DGA )-1;
   return( Ev );
}

/********************************************************/

static MEM_ZONE_METHODS DGA_Methods = 
{
   DGA_Propagate,
   DGA_Flush,
   DGA_Destroy,
   DGA_Get_Scanline,
   DGA_Set_Position,
};

EXTERN _G_DRIVER_ __G_DGA_DRIVER_ = 
{
   DGA_Change_Size,
   DGA_Get_Event,
   DGA_Setup,
   DGA_Print_Info,
   DGA_Adapt,
   DGA_Open,
   DGA_Change_CMap
};

/********************************************************/

#endif   // USE_DGA

