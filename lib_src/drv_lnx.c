/***********************************************
 *      X11 graphics Drivers                   *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"

#ifdef USE_X11

#include "mem_map.h"
#include "driver.h"
//extern "C" {
extern  int XShmQueryExtension( Display *);
extern  int XShmGetEventBase( Display *);
//}

#define ENUM_SET(T,f,w) f = (T)( f | w )
#define ENUM_CLR(T,f,w) f = (T)( f & (~w) )

/********************************************************/

#define X11_WAIT_OK(D)  { XEvent Event; \
   while( XPending( (D) ) ) XNextEvent( (D), &Event ); }

#define XWIN_MASKS   StructureNotifyMask| ExposureMask |    \
                KeyReleaseMask | KeyPressMask  |       \
                ButtonPressMask | ButtonReleaseMask |  \
                ButtonMotionMask | PointerMotionMask

extern MEM_ZONE_METHODS X11_Methods;

/********************************************************/

EXTERN void X11_Destroy_Resources( MEM_ZONE_X11 *X11 )
{
   XUnmapWindow( X11->display, X11->Win );
   if ( X11->Cells )
      XFreeColors( X11->display, X11->Cmap, X11->Pixels, 256, 0 );
   if ( X11->Cmap != None )
      XFreeColormap( X11->display, X11->Cmap );
   X11->Cells = 0;
   M_Free( X11->Pixels );
   XDestroyWindow( X11->display, X11->Win );
   X11->Win = ( Window )NULL;
}

static INT X11_Clean_Up( MEM_ZONE *M )
{
   MEM_ZONE_X11 *X11;

   X11 = ( MEM_ZONE_X11 *)M;

   if ( X11->XImg == NULL ) return( -1 );

   X11_WAIT_OK( X11->display );
   if ( X11->Refresh != _X_WINDOWS_ )
   {
      shmdt( X11->ShmInfo.shmaddr );
      shmctl( X11->ShmInfo.shmid, IPC_RMID, 0 );
      XShmDetach( X11->display, &X11->ShmInfo );
   }
   XDestroyImage( X11->XImg );   // frees Base_Ptr..
   X11->XImg = NULL;

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

static void X11_Destroy( MEM_ZONE *M )
{
   MEM_ZONE_X11 *X11;

   X11 = ( MEM_ZONE_X11 *)M;

   if ( X11->Cleanup_Mode != NULL ) (*X11->Cleanup_Mode)( M );
   X11->Cur_Req_Mode = 0;
   X11->Cleanup_Mode = NULL;

   if ( X11->Win != ( Window )NULL ) X11_Destroy_Resources( X11 );

   if ( X11->display != NULL ) XCloseDisplay( X11->display );
   X11->display = NULL;

   M_Free( X11->Modes );
   X11->Nb_Modes = 0;
}

static PIXEL *X11_Get_Scanline( MEM_ZONE *M, INT Y )
{
   PIXEL *Ptr;

   if ( Y>=MEM_Height(M) || Y<0 ) return( NULL );

   Ptr = MEM_Base_Ptr(M);
   Ptr += Y*MEM_BpS(M);
   return( Ptr );
}

static ZONE_CLIP_TYPE X11_Set_Position( MEM_ZONE *M, INT X, INT Y, INT Backup )
{
   MEM_ZONE_X11 *X11;

   // Change window's position...
   // NOT to be used often... (jams the XEvent buffer)

   X11 = ( MEM_ZONE_X11 *)M;
   XMoveWindow( X11->display, X11->Win,
      ( X11->The_Display.Width-MEM_Width(X11) )/2 + X,
      ( X11->The_Display.Height-MEM_Height(X11) )/2 + Y );
   XFlush( X11->display );
   /* X11_WAIT_OK( X11->display ); */
   XSync( X11->display, False );

   MEM_Xo(X11) = X; MEM_Yo(X11) = Y;
   return( ZONE_NO_CLIP );
}

EXTERN INT X11_Change_CMap( MEM_ZONE_DRIVER *M, INT Nb, COLOR_ENTRY *CMap )
{
   MEM_ZONE_X11 *X11;
   INT i;
   XColor Colors[ 256 ];

   X11 = ( MEM_ZONE_X11 *)M;

//   if ( Format_Depth( MEM_Format(X11) ) != 1 ) return( -1 );

   for( i=Nb-1; i>=0; --i )
   {
      Colors[i].red   = CMap[i][RED_F]<<8;
      Colors[i].green = CMap[i][GREEN_F]<<8;
      Colors[i].blue  = CMap[i][BLUE_F]<<8;
      Colors[i].pixel = X11->Pixels[ CMap[i][INDEX_F] ];
      Colors[i].flags = DoRed | DoGreen | DoBlue;
   }
   XStoreColors( X11->display, X11->Cmap, Colors, Nb );
   XFlush( X11->display );
   // XSync( X11->display, False );
   return( 0 );
}

/********************************************************/

static INT X11_Do_Flush( MEM_ZONE_X11 *X11, INT X, INT Y, INT Width, INT Height )
{
   XEvent Event;
   if ( X11->Refresh == _X_WINDOWS_ ) goto Raw_XImage;

   XShmPutImage( X11->display, X11->Win, X11->gc, X11->XImg,
      X, Y, X, Y, Width, Height, TRUE );

#ifndef SGI
   while ( !XCheckTypedWindowEvent( X11->display, X11->Win,
      X11->Completion_Type, &Event ) );
#endif

/*
   while( XPending(X11->display) )
   {
      XNextEvent( X11->display, &Event );
      if ( Event.type == X11->Completion_Type ) break;
   }
*/
   return(TRUE);

Raw_XImage:

   XPutImage( X11->display, X11->Win, X11->gc, X11->XImg,
      X, Y, X, Y, Width, Height );
/* X11_WAIT_OK( X11->display ); */
   return(TRUE);
}

static INT X11_Propagate( MEM_ZONE *M, INT X, INT Y, INT Width, INT Height )
{
   MEM_ZONE_X11 *X11;

   X11 = ( MEM_ZONE_X11 *)M;

   if ( Width<0 ) Width = MEM_Width( M );
   if ( Height<0 ) Height = MEM_Height( M );

   if ( Clip_Zones( MEM_IMG(M), &Width, &Height, &X, &Y ) == ZONE_CLIP_OUT )
      return( -1 );

   return( X11_Do_Flush( X11, X, Y, Width, Height ) );
}

static INT X11_Flush( MEM_ZONE *M )
{
   return( X11_Do_Flush( (MEM_ZONE_X11 *)M, 0, 0, MEM_Width( M ), MEM_Height( M ) ) );
}

/********************************************************/

#ifndef SKL_NO_CVRT

static INT X11_Propagate_Virtual( MEM_ZONE *M, INT X, INT Y, INT Width, INT Height )
{
   MEM_ZONE_X11 *X11;
   PIXEL *Dst, *Src;

   if ( Width<0 ) Width = MEM_Width( M );
   if ( Height<0 )  Height = MEM_Height( M );

   X11 = ( MEM_ZONE_X11 *)M;
   if ( Clip_Zones( MEM_IMG(X11->Dst), &Width, &Height, &X, &Y ) == ZONE_CLIP_OUT )
      return( -1 );

   Dst = MEM_Base_Ptr(X11->Dst) + Y*MEM_BpS(X11->Dst);
   Dst += X*MEM_Quantum(X11->Dst);

   Src = MEM_Base_Ptr(X11) + Y*MEM_BpS(X11);
   Src += X*MEM_Quantum(X11);

   MEM_Copy_Rect( M, Dst, Src, Width, Height, MEM_BpS(M->Dst) );

   if ( MEM_Flags(X11) & MEM_DONT_PROPAGATE ) return( 0 );
   else return( X11_Do_Flush( X11, X, Y, Width, Height ) );
}

static INT X11_Flush_Virtual( MEM_ZONE *M )
{
   PIXEL *Dst, *Src;

   Dst = MEM_Base_Ptr(M->Dst);
   Src = MEM_Base_Ptr(M);

   MEM_Copy_Rect( M, Dst, Src, MEM_Width(M), MEM_Height(M), MEM_BpS(M->Dst) );

   if ( MEM_Flags(M) & MEM_DONT_PROPAGATE ) return( 0 );
   else return( X11_Do_Flush( (MEM_ZONE_X11 *)M, 0, 0, MEM_Width( M ), MEM_Height( M ) ) );
}

#endif   // SKL_NO_CVRT

/********************************************************/
/********************************************************/

static void Visual_To_Mem_Img( Visual *Vsl, MEM_IMG *Mode, 
   INT depth, Display *Dsp )
{
   FORMAT Format;
   UINT F_Pos;
   UINT R_Pos, G_Pos, B_Pos;
   UINT R_Size, G_Size, B_Size;

   Mode->Flags = IMG_NO_FLAG;

      // Ackward method to get the exact Pixel_Depth of display
   {
      XImage *XImg;
      XImg = XCreateImage(
         Dsp, Vsl, depth, ZPixmap, 0, NULL,
         32, 32, BitmapPad( Dsp ), 0 );
      if ( XImg == NULL ) Mode->Quantum = (depth+7)/8; /* geee!!! XXXX */
      else {
         Mode->Quantum = XImg->bytes_per_line/32;
         XDestroyImage( XImg );
      }
   }
      // Shift & Mask setting

   F_Pos = POS_PACKED;
   Format = (FORMAT)0x0000;
/*   if ( Vsl->class == TrueColor || Vsl->class == DirectColor ) */
   {
      UINT Tmp;

      R_Pos = 0; G_Pos = 0; B_Pos = 0;
      R_Size = 0; G_Size = 0; B_Size = 0;

      if ( (Tmp=Vsl->red_mask) ) for( ; !(Tmp&1); Tmp>>=1 ) R_Pos++;
      for( ;(Tmp&1); Tmp>>=1 ) R_Size++;

      if ( (Tmp=Vsl->green_mask) ) for( ; !(Tmp&1); Tmp>>=1 ) G_Pos++;
      for( ;(Tmp&1); Tmp>>=1 ) G_Size++;

      if ( (Tmp=Vsl->blue_mask) ) for( ; !(Tmp&1); Tmp>>=1 ) B_Pos++;
      for( ;(Tmp&1); Tmp>>=1 ) B_Size++;

         /* Compute gap between color bits, if any. */

      if ( B_Pos>R_Pos )
      {
         UINT Tmp;
         F_Pos |= FMT_REVERSED_BIT;
         Tmp = R_Pos; R_Pos = B_Pos; B_Pos = Tmp;
      }
      G_Pos = G_Pos - B_Size - B_Pos;
      R_Pos = R_Pos - (G_Size+B_Size) - G_Pos;
/*
      Out_Message( "Mask: 0x%.8x 0x%.8x 0x%.8x  =>   0x%.8x",
         Vsl->red_mask, Vsl->green_mask, Vsl->blue_mask, F_Pos );
*/
   }
   /* else if ( Vsl->class == PseudoColor ) */
   /* else { Format = FMT_CMAP; F_Pos = 0; } */
   
   Format = (FORMAT)((R_Size<<8) | (G_Size<<4) | (B_Size));
   Format =(FORMAT)( Format |( ( Mode->Quantum<<12 )&0xF000 ) );
   F_Pos = (R_Pos<<8) | (G_Pos<<4) | (B_Pos);
   Mode->Format = (FORMAT)( Format | (F_Pos<<16) );

      // Store tmp info in some *Mode fields
   Mode->Base_Ptr = (PIXEL *)Vsl;
   Mode->Quantum = depth;
}

EXTERN MEM_IMG *X11_Detect_Available_Modes( MEM_ZONE_X11 *M )
{
   XVisualInfo *Visual_Info = NULL, *Tmp;
   XVisualInfo Template;
   INT i;
   MEM_IMG *Cur;

      // extract Display infos

   Visual_To_Mem_Img( 
      DefaultVisual( M->display, M->screen ),
      &M->The_Display, DefaultDepth( M->display, M->screen ),
      M->display );
   M->The_Display.Width = DisplayWidth( M->display, M->screen );
   M->The_Display.Height = DisplayHeight( M->display, M->screen );
   M->The_Display.Base_Ptr = (PIXEL *)DefaultVisual( M->display, M->screen );
   M->The_Display.Quantum = DefaultDepth( M->display, M->screen );

   Mem_Clear( &Template );
   Template.screen = M->screen;
   Template.visualid = DefaultVisual( M->display, M->screen )->visualid;
   Visual_Info = XGetVisualInfo( M->display,
      /* VisualNoMask */ /* VisualIDMask| */ VisualScreenMask,
      &Template, (int*)&M->Nb_Modes );
   if ( Visual_Info == NULL )
   {
      Out_Message( "Can't get X visual" );
      return( NULL );
   }

   for( i=M->Nb_Modes, Tmp = Visual_Info; i>0; i--, Tmp++ )
   {
         // Get rid of Visual with depth<8
         // and skip bad visuals
      if ( Tmp->depth<8 ) M->Nb_Modes--;
#if defined(__cplusplus) || defined(c_plusplus)
      else if ( Tmp->c_class == StaticColor || Tmp->c_class == StaticGray
         || Tmp->c_class == GrayScale ) M->Nb_Modes--;
#else
      else if ( Tmp->class == StaticColor || Tmp->class == StaticGray
         || Tmp->class == GrayScale ) M->Nb_Modes--;
#endif
   }
      
   if ( M->Nb_Modes==0 )
   {
      Out_Message( "No X visual with depth >=8 !!! Sorry. Need some colors." );
      return( NULL );
   }

   M_Free( M->Modes );
   Cur = M->Modes = New_Object( M->Nb_Modes, MEM_IMG );
   if ( Cur == NULL )
   {
      Out_Error( "no mem for X11 modes specs", 0 );
      return( NULL );
   }

   for( i=0, Tmp = Visual_Info; i<M->Nb_Modes; Tmp++ )
   {
#if 0
      Out_Message( "Visual #%d:", i+1 );
      Out_Message( "  Colormapsize: %d", Tmp->colormap_size );
      Out_Message( "  bits_per_rgb: %d", Tmp->bits_per_rgb );
      Out_Message( "  Class:%s  Depth:%d",
         Tmp->class==TrueColor ? "TrueColor" : 
         Tmp->class==DirectColor ? "DirectColor" :
         Tmp->class==PseudoColor ? "PseudoColor" :
         Tmp->class==StaticColor ? "StaticColor" : 
         Tmp->class==StaticGray ? "StaticGray" : 
         Tmp->class==GrayScale ? "GrayScale" : "???",
         Tmp->depth  );
      Out_Message( "  masks:0x%.4x 0x%.4x 0x%.4x",
         Tmp->red_mask, Tmp->green_mask, Tmp->blue_mask );
#endif
         // Skip bad visuals...
      if ( Tmp->depth<8 ) continue;
#if defined(__cplusplus) || defined(c_plusplus)
      if ( Tmp->c_class == StaticColor || Tmp->c_class == StaticGray
         || Tmp->c_class == GrayScale ) continue;
#else
      else if ( Tmp->class == StaticColor || Tmp->class == StaticGray
         || Tmp->class == GrayScale ) continue;
#endif

      Visual_To_Mem_Img( Tmp->visual, Cur, Tmp->depth, M->display );
      Cur->Width = WidthOfScreen( ScreenOfDisplay( M->display, M->screen ) );
      Cur->Height = HeightOfScreen( ScreenOfDisplay( M->display, M->screen ) );
      Cur++;
      i++;
   }      

   XFree( ( void *)Visual_Info );

   return( M->Modes );
}

/********************************************************/

static REFRESH_MTHD Allocate_XImage( MEM_ZONE_X11 *M )
{
   INT Size;
   INT Width, Pad;
   INT Height, BpS;
   REFRESH_MTHD Method;

   M->XImg = NULL;

      // Pad width to 8

   BpS = MEM_Width( M )*MEM_Quantum( M );
   Pad = ( BpS - (BpS/8)*8 );
   if ( Pad ) { Pad = 8-Pad; BpS += Pad; }
   Height = MEM_Height( M );
   Width = MEM_Width( M );
   Size = BpS*Height;
   Pad /= MEM_Quantum( M );

   Method = M->Refresh;

   if ( !(Method & _SHM_) ) goto Basic_XImage;

   M->ShmInfo.shmid = shmget( IPC_PRIVATE, Size, IPC_CREAT | 0x1c0 );

   if ( M->ShmInfo.shmid < 0 )
   {
      SET_DRV_ERR( "shared memory ID retreival failed" );
      goto Basic_XImage;
   }

   M->ShmInfo.shmaddr = ( char * ) shmat( M->ShmInfo.shmid, NULL, 0 );
   if ( M->ShmInfo.shmaddr == ( void *)(-1) )
   {
      Out_Message( "shared memory segment allocation failed" );
      goto Basic_XImage;
   }

   MEM_Base_Ptr( M ) = ( PIXEL * )( M->ShmInfo.shmaddr );

   M->XImg = XShmCreateImage(
         M->display, M->visual,
         M->depth, ZPixmap,
         (char *)MEM_Base_Ptr( M ),
         &M->ShmInfo,
         Width+Pad, Height );
   if ( M->XImg == NULL ) 
   {
      SET_DRV_ERR( "XShmCreateImage() failed" );
      goto Failed;
   }

   if ( M->XImg->xoffset != 0 )
   {
      SET_DRV_ERR( "Error with XImage xoffset..." );
      goto Failed;
   }

   if ( M->XImg->bytes_per_line != BpS )
   {
      SET_DRV_ERR( "Error with XImg->bytes_per_line" );
      goto Failed;
   }

   M->ShmInfo.readOnly = FALSE;
   if ( !XShmAttach( M->display, &M->ShmInfo ) )
   {
      Out_Message( "Error with XShmAttach..." );
      goto Failed;
   }
#ifndef SGI
   M->Completion_Type = XShmGetEventBase( M->display ) + ShmCompletion;
#else     // ??@!?
  M->Completion_Type = ShmCompletion;
#endif
   M->Refresh = _SHM_;
   goto Finish_Him;

Failed:
   shmdt( M->ShmInfo.shmaddr );
   shmctl( M->ShmInfo.shmid, IPC_RMID, 0 );
   if ( M->XImg != NULL ) XDestroyImage( M->XImg );

Basic_XImage:
   if ( !(Method & _X_WINDOWS_) ) goto Really_Failed;
   M->XImg = XCreateImage(
         M->display, M->visual,
         M->depth, ZPixmap, 0, NULL,
         Width+Pad, Height, 
         BitmapPad( M->display ), 0 );
   if ( M->XImg == NULL ) 
   {
      SET_DRV_ERR( "XCreateImage() failed" );
      goto Really_Failed;
   }
   if ( M->XImg->bytes_per_line != BpS )
   {
      SET_DRV_ERR( "Error with XImg->bytes_per_line" );
      goto Really_Failed;
   }
   M->XImg->data = New_Object( Size, char );
   if ( M->XImg->data == NULL )
   {
      XDestroyImage( M->XImg );
      SET_DRV_ERR( "Malloc failed for XImage" );
      goto Really_Failed;
   }

   MEM_Base_Ptr( M ) = ( PIXEL *)M->XImg->data;
   M->Refresh = _X_WINDOWS_;
   goto Finish_Him;

Really_Failed:
   M->XImg = NULL;
   MEM_Base_Ptr( M ) = NULL;
   M->Refresh = _VOID_;
   return( _VOID_ );

Finish_Him:

   M->Base_Ptr = MEM_Base_Ptr(M);
   memset( M->Base_Ptr, 0, Size );

   Mem_Img_Set( MEM_IMG(M),
      IMG_VIRTUAL, 
      Width, Height, Pad, MEM_Quantum( M ),
      MEM_Format( M ), NULL );
   MEM_Base_Ptr(M) = M->Base_Ptr;

   return( Method );
}

/********************************************************/

EXTERN INT X11_Undefine_Cursor( MEM_ZONE_X11 *X11 )
{
   XColor Dummy;
   Pixmap Mouse_Void_Pixmap;

         // Undefine Cursor

   Dummy.pixel = 0;
   Dummy.red   = Dummy.green = Dummy.blue = 0;
   Dummy.flags = 0;
   Mouse_Void_Pixmap = XCreatePixmap( X11->display, X11->Win, 1, 1, 1 );
   if ( XCreateBitmapFromData( X11->display, 
      Mouse_Void_Pixmap, ( char *)"\1", 1, 1 ) == None )
   {
      SET_DRV_ERR( "X server could not even allocate a 1x1 Pixmap <g> !!" );
      return( -1 );
   }

   X11->Void_Cursor = XCreatePixmapCursor(
      X11->display, 
      Mouse_Void_Pixmap, Mouse_Void_Pixmap,
      &Dummy, &Dummy,
      0, 0 );
   XDefineCursor( X11->display, X11->Win, X11->Void_Cursor );
   XFreePixmap( X11->display, Mouse_Void_Pixmap );
   return( 0 );
}

static MEM_ZONE_DRIVER *Open_Mode( MEM_ZONE_DRIVER *Dummy )
{
   MEM_ZONE_X11 *M;
   XSizeHints Size_Hints;
   XSetWindowAttributes W_Attribs;
   INT W, H;

   M = (MEM_ZONE_X11 *)Dummy;
   W = MEM_Width( M );
   H = MEM_Height( M );

   if ( M->Win==(Window)NULL )
   {
            // First call. Set up everything once

      M->gc = DefaultGC( M->display, M->screen );
      M->Cmap = (Colormap)DefaultColormap( M->display, M->screen );
      M->Cells = 0;
      M->Void_Cursor = (Cursor)NULL;

      W_Attribs.backing_store = Always;

      M->Win = XCreateWindow( 
         M->display, RootWindow( M->display, M->screen ),
         0, 0, W, H, 0,
         CopyFromParent, InputOutput, CopyFromParent,
         CWBackingStore, &W_Attribs );

            // Color map

      if ( Format_Depth( MEM_Format( M ) ) == 1 )
      {
         M->Pixels = New_Object( 256, unsigned long );
         if ( M->Pixels == NULL ) goto CMap_Error;
         M->Cmap = XCreateColormap( 
            M->display, 
            M->Win,
            M->visual,
            AllocNone );
         M->Cells = XAllocColorCells( 
            M->display, M->Cmap, FALSE, 
            NULL, 0,
            M->Pixels, 256 );
         if ( !M->Cells )
         {
CMap_Error:
            SET_DRV_ERR( "XAllocColorCells() failed" );
            M_Free( M->Pixels );
            return( NULL );
         }
      }
      else M->Cmap = None;

      X11_Undefine_Cursor( M );

      XStoreName( M->display, M->Win, M->Window_Name );
      XSelectInput( M->display, M->Win, XWIN_MASKS );
      XMapWindow( M->display, M->Win );
   }
   else XResizeWindow( M->display, M->Win, W, H );

   XMoveWindow( M->display, M->Win,
      ( M->The_Display.Width-MEM_Width(M) )/2,
      ( M->The_Display.Height-MEM_Height(M) )/2 );

      // No resize

   Size_Hints.width  = W;
   Size_Hints.height = H;
   Size_Hints.min_width = Size_Hints.width;
   Size_Hints.max_width = Size_Hints.width;
   Size_Hints.min_height = Size_Hints.height;
   Size_Hints.max_height = Size_Hints.height;
   Size_Hints.flags = PSize | PMinSize | PMaxSize;
   XSetWMNormalHints( M->display, M->Win, &Size_Hints );

   XFlush( M->display );
   XWarpPointer( M->display, 
      RootWindow( M->display, M->screen ), 
      RootWindow( M->display, M->screen ),
      0, 0, M->The_Display.Width, M->The_Display.Height,
      M->The_Display.Width/2, M->The_Display.Height/2  );

   if ( Allocate_XImage( M ) == _VOID_ )
   {
      // TODO:Destroy everything!!
      return( NULL );
   }

   XSync( M->display, False );
   Dummy->Cleanup_Mode = X11_Clean_Up;

   Format_To_Zone_Type( (MEM_ZONE *)Dummy );

   return( Dummy );
}

/********************************************************/

EXTERN Display *X11_Open_Display( STRING Name )
{
   Display *display;

   display = XOpenDisplay( Name );
   if ( display==NULL ) SET_DRV_ERR2( "Can't open DISPLAY '%s'", Name );
   return( display );
}

/********************************************************/

static MEM_ZONE_DRIVER *X11_Change_Size( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_X11 *X11;

   X11 = (MEM_ZONE_X11 *)M;
   if ( X11==NULL ) return( NULL );

   return( (MEM_ZONE_DRIVER *)X11 );
}

/********************************************************/

static EVENT_TYPE Translate_Event_X11( DRV_EVENT *DRV_Event, XEvent *Event )
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
            ENUM_SET( MODIFIER_TYPE, DRV_Event->Modifier, DRV_SHIFT_MODIFIER );
         if ( Touche == XK_Control_L || Touche == XK_Control_R )
            ENUM_SET( MODIFIER_TYPE, DRV_Event->Modifier, DRV_CTRL_MODIFIER );
         if ( Touche == XK_Alt_L || Touche == XK_Alt_R )
            ENUM_SET( MODIFIER_TYPE, DRV_Event->Modifier, DRV_ALT_MODIFIER );

         if ( Touche == XK_Left ) ENUM_SET( MODIFIER_TYPE, DRV_Event->Modifier, DRV_LEFT_MODIFIER);
         if ( Touche == XK_Right ) ENUM_SET( MODIFIER_TYPE, DRV_Event->Modifier, DRV_RIGHT_MODIFIER );
         if ( Touche == XK_Up ) ENUM_SET( MODIFIER_TYPE, DRV_Event->Modifier, DRV_UP_MODIFIER );
         if ( Touche == XK_Down ) ENUM_SET( MODIFIER_TYPE, DRV_Event->Modifier, DRV_DOWN_MODIFIER );
      }
      return( DRV_Event->Event );
   }
   else if ( Type == KeyRelease )
   {
      Mod = Event->xkey.state;
      DRV_Event->Modifier = DRV_NO_MODIFIER;
      DRV_Event->Key = '\0';

      if ( Mod & ShiftMask )   ENUM_SET( MODIFIER_TYPE, DRV_Event->Modifier, DRV_SHIFT_MODIFIER );
      if ( Mod & ControlMask )   ENUM_SET( MODIFIER_TYPE, DRV_Event->Modifier, DRV_CTRL_MODIFIER );
      if ( Mod & Mod1Mask )   ENUM_SET( MODIFIER_TYPE, DRV_Event->Modifier, DRV_ALT_MODIFIER );

      if ( XLookupString( ( XKeyReleasedEvent *)( Event ),
                     &Buffer, 1, &Touche, NULL ) != 1 )
      {
         if ( DRV_Event->Modifier != DRV_NO_MODIFIER )
            DRV_Event->Key = '\0';
         return( DRV_Event->Event );
      }
//      fprintf( stderr, "State: 0x%x Keycode: 0x%x\n", Event->xkey.keycode, Event->xkey.state );
//      fprintf( stderr, "Touche: 0x%x Buffer: 0x%x\n", Touche, Buffer );

      DRV_Event->Key = ( char ) Touche;
      ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_KEY_PRESS );

      return( DRV_Event->Event );
   }
   else if ( Type == MotionNotify )
   {
      XMotionEvent *MEvent;

      MEvent = ( XMotionEvent *)&( Event->xmotion );
      But    = Event->xbutton.state;
      DRV_Event->x = MEvent->x;
      DRV_Event->y = MEvent->y;

      ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_MOVE );

      if ( But & Button1Mask ) ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_CLICK1 );
      else ENUM_CLR( EVENT_TYPE, DRV_Event->Event, DRV_CLICK1 );
      if ( But & Button2Mask ) ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_CLICK2 );
      else ENUM_CLR( EVENT_TYPE, DRV_Event->Event, DRV_CLICK2 );
      if ( But & Button3Mask ) ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_CLICK3 );
      else ENUM_CLR( EVENT_TYPE, DRV_Event->Event, DRV_CLICK3 );
   }
   else if ( ( Type == ButtonRelease ) || ( Type == ButtonPress ) )
   {
      XButtonEvent *BEvent;
      
      BEvent = (XButtonEvent *)&( Event->xbutton );
      DRV_Event->x = BEvent->x;
      DRV_Event->y = BEvent->y;
      
      if ( Type == ButtonRelease )
      {
         But = BEvent->state;

         if ( But & Button1Mask ) ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_RELEASE1 );
         if ( But & Button2Mask ) ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_RELEASE2 );
         if ( But & Button3Mask ) ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_RELEASE3 );
      }
      else if ( Type == ButtonPress )
      {
         But = BEvent->button;

         if ( But & Button1 ) ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_CLICK1 );
         if ( But & Button2 ) ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_CLICK2 );
         if ( But & Button3 ) ENUM_SET( EVENT_TYPE, DRV_Event->Event, DRV_CLICK3 );
      }
      return( DRV_Event->Event );
   }
   return( DRV_Event->Event );
}

EXTERN EVENT_TYPE X11_Get_Event( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_X11 *X11;
   XEvent Event;

   if ( M==NULL || !(M->Type & MEM_IS_DISPLAY) ) return( DRV_NULL_EVENT );
   X11 = (MEM_ZONE_X11 *)M;

   X11->Event.Event = DRV_NULL_EVENT;

// while ( XCheckWindowEvent( X11->display, X11->Win, XWIN_MASKS, &Event ) )
//    Translate_Event_X11( &X11->Event, &Event );

   while( XPending( X11->display )>0 )
   {
      XNextEvent( X11->display, &Event );
      if ( Event.xany.window != X11->Win ) continue;
      Translate_Event_X11( &X11->Event, &Event );
   }
   return( X11->Event.Event );
}

/********************************************************/

EXTERN void X11_Print_Zone_Specs( STRING S, MEM_IMG *M )
{
#ifndef SKL_LIGHT_CODE

   UINT Shift[4], Mask[4];
   Visual *V;

   V = (Visual*)IMG_Base_Ptr(M);

   Out_String( "%s: %d x %d x 0x%.4x",
      S, IMG_Width(M), IMG_Height(M), IMG_Format(M) );
   if ( V!=NULL ) Out_String( " (%s)",
      V->class==TrueColor ? "TrueColor" : 
      V->class==DirectColor ? "DirectColor" :
      V->class==PseudoColor ? "PseudoColor" :
      V->class==StaticColor ? "StaticColor" : 
      V->class==StaticGray ? "StaticGray" : 
      V->class==GrayScale ? "GrayScale" : "???" );
   
   if ( IMG_Format(M) == FMT_CMAP )
   {
      Out_Message( " (- Colormap -)" );
      return;
   }
   else Out_Message( " (- RGB -)" );
   Format_Mask_And_Shift( IMG_Format(M), Mask, Shift );
   Out_Message( "    - Shifts:(%d,%d,%d) Masks:(0x%.2x,0x%.2x,0x%.2x)",
      Shift[RED_F], Shift[GREEN_F], Shift[BLUE_F],
      Mask[RED_F], Mask[GREEN_F], Mask[BLUE_F] );

#endif   // SKL_LIGHT_CODE
}

static void X11_Print_Info( MEM_ZONE_DRIVER *M )
{
#ifndef SKL_LIGHT_CODE

   MEM_ZONE_X11 *X11;
   INT i;

   X11 = (MEM_ZONE_X11 *)M;
   if ( X11==NULL ) return;

   Out_Message( "       --- X11 driver ---" );
   X11_Print_Zone_Specs( " * Display", &X11->The_Display );

   if ( !X11->Nb_Modes ) goto Req_Modes;
   Out_Message( " * Available modes:" );
   for( i=0; i<X11->Nb_Modes; ++i )
   {
      char S[32];
      sprintf( S, "Mode %d", i+1 );
      X11_Print_Zone_Specs( S, &X11->Modes[i] );
   }
   if ( X11->Refresh & _SHM_ ) Out_Message( " Using Shared memory" );
   else if ( X11->Refresh & _X_WINDOWS_ ) Out_Message( " Using Raw XImage" );

Req_Modes:
   Drv_Print_Req_Infos( M, X11_Print_Zone_Specs );

#endif   // SKL_LIGHT_CODE
}

/********************************************************/

static MEM_ZONE_DRIVER *X11_Setup( _G_DRIVER_ *Drv, void *Dummy )
{
   MEM_ZONE *M;
   MEM_ZONE_X11 *X11;
   Display *display;
   DRIVER_ARG *Args;

   Args = (DRIVER_ARG *)Dummy;
   if ( Args->Display_Name == NULL ) Args->Display_Name = (STRING)XDisplayName( NULL );
   display = X11_Open_Display( Args->Display_Name );
   if ( display == NULL ) return( NULL );

   M = (MEM_ZONE *)New_MEM_Zone( MEM_ZONE_X11, &X11_Methods );
   if ( M==NULL ) Exit_Upon_Mem_Error( "driver MEM_ZONE", sizeof( MEM_ZONE_X11 ) );

   X11 = (MEM_ZONE_X11 *)M;
   M->Type = (ZONE_TYPE)( M->Type | MEM_IS_DISPLAY );

   X11->display = display;
   X11->screen = DefaultScreen( display );
   X11->Window_Name = Args->Window_Name;

   X11->Refresh = (REFRESH_MTHD)(_ALL_X11_METHODS_ & Args->Refresh_Method);
   if ( !(X11->Refresh & _SHM_) ) goto No_Shm;
   if ( !XShmQueryExtension( display ) )
   {
      // Out_Message( "  - Shared memory extension not available" );
      X11->Refresh = (REFRESH_MTHD)( X11->Refresh & (~_SHM_) );
   }
   else if ( Args->Display_Name[0] != ':' )
   {
      /* Out_Message( " Can't use shared memory when $DISPLAY is redirected..." ); */
      X11->Refresh = (REFRESH_MTHD)( X11->Refresh & (~_SHM_) );
   }

No_Shm:

   if ( X11_Detect_Available_Modes( X11 ) == NULL ) goto Failed;
   return( (MEM_ZONE_DRIVER *)X11 );

Failed:
   if ( M!=NULL ) M_Free( M );
   if ( display != NULL ) XCloseDisplay( display );
   return( NULL );
}

static INT X11_Adapt( MEM_ZONE_DRIVER *M, void *Dummy )
{
   MEM_ZONE_X11 *X11;
   Visual *visual;
   int depth;
   INT i;
   DRIVER_ARG *Args;

   Args = (DRIVER_ARG *)Dummy;
   X11 = (MEM_ZONE_X11 *)M;
   if ( X11==NULL ) return( 0 );

   for( i=0; i<Args->Nb_Modes; ++i )
   {
      MEM_IMG *Req;
      INT Width, Height;
      FORMAT Format, Format2;
      INT j;

      Req = &Args->Modes[i];
      Width = IMG_Width( Req );
      if ( Width == -1 ) Width = Req->Width = X11->The_Display.Width;
      Height = IMG_Height( Req );
      if ( Height == -1 ) Height = Req->Height = X11->The_Display.Height;
      Format = IMG_Format( Req );
      if ( Format == 0xFFFFFFFF ) Format = Req->Format = X11->The_Display.Format;

      Format2 = Format; // A priori..

      for( j=X11->Nb_Modes-1; j>=0; --j )
      {
         if ( X11->Modes[j].Format != Format ) continue;
           // no size limit for windows
         /* if ( X11->Modes[j].Width < Width ) continue;
            if ( X11->Modes[j].Height < Height ) continue; */
         visual = (Visual *)X11->Modes[j].Base_Ptr;
         depth = X11->Modes[j].Quantum;
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

          // Search for possible convertion

      if ( (Format&0xFFFF) == FMT_CMAP )
      {
         Format2 = IMG_Format( &X11->The_Display );
         visual = (Visual *)X11->The_Display.Base_Ptr;
         depth = X11->The_Display.Quantum;          
      }                         
      else if ( Format_Depth( IMG_Format( &X11->The_Display ) )==1 )
      {
         Format2 = FMT_332;
         visual = (Visual *)X11->The_Display.Base_Ptr;
         depth = X11->The_Display.Quantum;
      }
      else 
      {
         INT Best;
         Best = -1;
         Format2 = (FORMAT)0x0000;
         for( j=X11->Nb_Modes-1; j>=0; --j )
         {
            if ( ( X11->Modes[j].Format&0xFFFF ) == FMT_CMAP ) continue;
            if ( X11->Modes[j].Width < Width ) continue;
            if ( X11->Modes[j].Height < Height ) continue;
            if ( Format2 <= X11->Modes[j].Format )
            { Format2 = X11->Modes[j].Format; Best = j; }

         }
         if ( Best<0 )
         {
            SET_DRV_ERR2( "Can't convert requested mode #%d", (STRING)(INT)i+1 );
            return( 0 );
         }
         visual = (Visual *)X11->Modes[Best].Base_Ptr;
         depth = X11->Modes[Best].Quantum;                           
      }

#endif   // SKL_NO_CVRT

Mode_Ok:

         // Store additional info in *Req

      Req->Pad = Format2;
      Req->Quantum = depth;
      Req->Base_Ptr = (PIXEL *)visual;
      if ( Args->Direct ) Req->Flags |= IMG_DIRECT;
   }

   return( i );
}

static MEM_ZONE *X11_Open( MEM_ZONE_DRIVER *M, INT Nb )
{
   MEM_IMG *Req;
   FORMAT Format, Format2;
   MEM_ZONE_X11 *X11;

   if ( M==NULL || !(M->Type&MEM_IS_DISPLAY) ) return( NULL );
   X11 = (MEM_ZONE_X11 *)M;

   if ( Nb<1 ) // Clean all
   {
      ZONE_DESTROY( M );
      return( (MEM_ZONE *)M );
   }

   if ( Nb>X11->Nb_Req_Modes )
   {
      SET_DRV_ERR2( "Invalid mode #%d request", (STRING)(INT)Nb );
      return( NULL );
   }

   Req = &X11->Req_Modes[Nb-1];
   Format = IMG_Format( Req );
   Format2 = (FORMAT)Req->Pad;

   MEM_Width(M) = IMG_Width( Req );
   MEM_Height(M) = IMG_Height( Req );
   X11->depth = IMG_Quantum( Req );      // depth
   X11->visual = (Visual *)IMG_Base_Ptr( Req );    // visual
   MEM_Pad(M) = 0;
   MEM_BpS( M ) = 0;
   MEM_Base_Ptr( M ) = NULL;
   MEM_Quantum(M) = Format_Depth( Format2 );
   MEM_Format(M) = Format2;
   if ( Open_Mode( M ) == NULL ) goto Failed;

   M->Type = (ZONE_TYPE)( M->Type | MEM_IS_DISPLAY);

#ifndef SKL_NO_CVRT

   if ( Format2!=Format )
   {
         // Transfert real final zone in M->Dst. Allocate a new
         // M->Zone for convertion buffer

      M->Dst = New_MEM_Zone( MEM_ZONE, NULL );
      if ( M->Dst==NULL ) goto Failed;
      *MEM_IMG(M->Dst) = *MEM_IMG(M);
      M->Dst->Type = M->Type;

         // Now, fill M->Zone with real desired format

      if ( Mem_Img_Set( MEM_IMG(M), IMG_OWNS_PTR,
         MEM_Width(M), MEM_Height(M), 0, 0, Format, NULL )==NULL )
      {
         SET_DRV_ERR( "convertion setup failed" );
         return( NULL );
      }
      Format_To_Zone_Type( (MEM_ZONE *)M );
      M->Methods->Propagate = X11_Propagate_Virtual;
      M->Methods->Flush = X11_Flush_Virtual;

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
         Install_Index_To_Any( (MEM_ZONE *)M, (FORMAT)0xFFFFFFFF, NULL );
      }
      if ( !(Req->Flags & IMG_DIRECT) ) 
         MEM_Flags( M ) |= IMG_DIRECT;
   }

   if ( X11->Cmap != None )
   {
      XSetWindowColormap( X11->display, X11->Win, X11->Cmap );
      XInstallColormap( X11->display, X11->Cmap );
   }

   return( (MEM_ZONE *)M );

Failed:
   SET_DRV_ERR( "Open_Mode() failed" );
   return( NULL );
}

/********************************************************/

static MEM_ZONE_METHODS X11_Methods = 
{
   X11_Propagate,
   X11_Flush,
   X11_Destroy,
   X11_Get_Scanline,
   X11_Set_Position,
};

EXTERN _G_DRIVER_ __G_X11_DRIVER_ = 
{
   X11_Change_Size,
   X11_Get_Event,
   X11_Setup,
   X11_Print_Info,
   X11_Adapt,
   X11_Open,
   X11_Change_CMap
};

/********************************************************/

#endif   // USE_X11

