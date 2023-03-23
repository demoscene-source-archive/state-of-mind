/***********************************************
 *        Quick DDraw Driver                   *
 * Skal 98                                     *
 ***********************************************/

#include "gfx_ddraw.h"

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdarg.h>

#define _OUR_CLASS_NAME_   "SkalWZ"

static DDSURFACEDESC dPrim;

static IDirectDraw *lpDD = NULL;
static IDirectDrawSurface *lpDDSPrimary = NULL;
static IDirectDrawPalette *lpDDPalette = NULL;
static unsigned RedMask, GreenMask, BlueMask, AlphaMask;
static INT Bpp;
static FullScreen = FALSE;
static INT Width, Height, BpS;
static void *Base_Ptr = NULL;
static INT Locked = FALSE;

static HWND hWndMain;
static int  Win32_KbHit( void );

EXTERN void Register_Video_Support( INT Nb, ... )
{
}

EXTERN  INT Driver_Change_CMap( void *M, INT Nb, COLOR_ENTRY *CMap )
{
   PALETTEENTRY Pal[256];
   INT i;

   if ( lpDD==NULL ) return( 0 );
   for( i=0; i<Nb; ++i )
   {
      Pal[i].peFlag = PC_NOCOLLAPSE;
      Pal[i].peRed = CMap[3*i];
      Pal[i].peGreen = CMap[3*i+1];
      Pal[i].peBlue = CMap[3*i+2];
   }
   if(lpDD!=NULL)
      lpDD->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256,Pal,&lpDDPalette,NULL);
   if(lpDDSPrimary!=NULL)
      lpDDSPrimary->SetPalette(lpDDPalette);
   return( Nb );
}

EXTERN INT MEM_Width( void *M ) { return( Width ); }
EXTERN INT MEM_Height( void *M ) { return( Height ); }
EXTERN INT MEM_BpS( void *M ) { return( BpS ); }

EXTERN void *MEM_Base_Ptr( MEM_IMAGE M ) 
{
   DDSURFACEDESC ddsd;
   if ( lpDD==NULL ) return( NULL );
   if ( !Locked ) IDirectDrawSurface_Unlock(lpDDSPrimary, NULL);
   Locked = TRUE;
   Base_Ptr = (void*)ddsd.lpSurface;
   Width = ddsd.dwWidth;
   Height = ddsd.dwHeight;
   BpS = ddsd.lPitch;
   return( Base_Ptr );
}

EXTERN void ZONE_FLUSH( void *M )
{
   if ( lpDD==NULL ) return;
   if ( Locked ) IDirectDrawSurface_Unlock( lpDDSPrimary, NULL );
   Locked = FALSE;
}

EXTERN INT Driver_Get_Event( void *M )
{
   if ( Win32_KbHit( ) ) return( DRV_KEY_PRESS );
   return( DRV_NULL_EVENT );
}

////////////////////////////////////////////////////////////////////////

EXTERN void *Driver_Call( _G_DRIVER_ *Driver, ... )
{
   DDSURFACEDESC ddsd;
   DDSCAPS ddscaps;

   if (lpDD!=NULL) return DDE_ALREADYINITIALIZED;
   if( DirectDrawCreate( NULL, &lpDD, NULL )!=DD_OK )
      return( NULL );
   if ( IDirectDraw_SetCooperativeLevel( lpDD,
      hWndMain, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT ) != DD_OK )
      FullScreen = FALSE;
   else FullScreen = TRUE;
   
   if ( FullScreen==FALSE )
      IDirectDraw_SetCooperativeLevel( lpDD, hWndMain, DDSCL_NORMAL);

   if ( FullScreen )
       if( lpDD->SetDisplayMode( 640, 480, 16 ) != DD_OK )
         return( NULL );

   bzero( &ddsd, sizeof (ddsd ) );
   ddsd.dwSize = sizeof( ddsd );
   ddsd.dwFlags = DDSD_CAPS;
   ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
   if ( IDirectDraw_CreateSurface( lpDD, &ddsd, &lpDDSPrimary, NULL ) != DD_OK )
      return( NULL );
   IDirectDrawSurface_Lock( lpDDSPrimary, NULL, &ddsd, DDLOCK_WAIT, NULL );
   Base_Ptr = (void*)ddsd.lpSurface;
   Width = ddsd.dwWidth;
   Height = ddsd.dwHeight;
   BpS = ddsd.lPitch;
   IDirectDrawSurface_Unlock( lpDDSPrimary, NULL );
   Locked = FALSE;
}

EXTERN void Driver_Close( void *M )
{
   if ( lpDD!=NULL )
   {
      if ( lpDDPalette!=NULL ) IDirectDraw_Release( lpDDPalette );
      lpDDPalette = NULL;
      if ( lpDDSPrimary!=NULL ) IDirectDraw_Release(lpDDSPrimary);
      lpDDSPrimary = NULL;
      IDirectDraw_Release(lpDD);
      lpDD = NULL;
   }
}

///////////////////////////////////////////////////////////////////

static int qhead = 0;
static int qtail = 0;
static int queue[256] = {0};

//////////////////////////////////////////////////////////////////
// 
// Converts #@%@!@ win's lpCmdLine into argc, argv
//

static char    *argvbuf[64];
static char    cmdLineBuffer[2048];
extern int main(int argc, char **argv);

static void Launch_Main( LPSTR lpCmdLine )
{
   char *p, *pEnd;
   int  argc;
   char    **argv;

   argc = 0;
   argvbuf[argc++] = "(Linux does it better!)";

   if ( lpCmdLine == NULL) goto End;

   strcpy( cmdLineBuffer, lpCmdLine );
   p = cmdLineBuffer;
   pEnd = p + strlen(cmdLineBuffer);
   if ( pEnd >= &cmdLineBuffer[2046] ) pEnd = &cmdLineBuffer[2046];

   fflush(stdout);

   while ( 1 )
   {
      fflush(stdout);

      while (*p == ' ') p++;   // skip over white space
      if ( p>=pEnd ) break;

      argvbuf[argc++] = p;
      if ( argc>=64 ) break;
   
      while (*p && (*p != ' ')) p++; // skip till there's 0 or space
      if (*p == ' ') *p++ = 0;
   }

End:
   main( argc, argv );  // go for it, and remember to behave :)
}

//////////////////////////////////////////////////////////////////

static int  Win32_KbHit( void )
{
   MSG msg;

   if ( qhead != qtail ) return( 1 );

   while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);      // this might change qhead
      if ( qhead!=qtail ) return( 1 );
   }
   return 0;
}

char Win32_GetCH( void )
{
   MSG  msg;
   char rv;

   if ( qtail!=qhead )
   {
      rv = queue[qtail++];
      qtail &= 255;
      return rv;
   }

   while( GetMessage( &msg, NULL, 0, 0 ) )
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if ( qtail != qhead )
      {
         rv = queue[qtail++];
         qtail &= 255;
         return rv;
      }
   }

    // Should never get here!!
}

void Win32_Exit( )
{
   PostMessage( hWndMain, WM_CLOSE, 0, 0 );
}


static void Win32_getWindowSize( float *width, float *height )
{
#if 0
   RECT rect;

   if ( fullScreen )
   {
      GetWindowRect(hWndMain, &rect);
      *width = (float) (rect.right-rect.left);
      *height = (float) (rect.bottom-rect.top);
   }
   else {
      GetClientRect(hWndMain, &rect);
      *width = (float) (rect.right-rect.left);
      *height = (float) (rect.bottom-rect.top);
   }
#endif
}


//////////////////////////////////////////////////////////////////
///////////// MainWndproc. Callback for Win messages /////////////
//////////////////////////////////////////////////////////////////

long FAR PASCAL MainWndproc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
   PAINTSTRUCT ps;
   HDC         hdc;

   switch( message )
   {
      case WM_SETCURSOR:
         SetCursor(NULL);
         return TRUE;
      break;

      case WM_CREATE:
      break;

      case WM_PAINT:
         hdc = BeginPaint( hWnd, &ps );
         EndPaint( hWnd, &ps );
      return 1;

      case WM_CLOSE:
         queue[qhead++] = 'q'; qhead &= 255;
      break;

      case WM_DESTROY:
         Driver_Close( NULL );
         PostQuitMessage( 0 );
      break;

      case WM_MOVE:
      break;

      case WM_DISPLAYCHANGE:
      case WM_SIZE:
      {
         float  width, height;
         Win32_getWindowSize( &width, &height );
      }
      break;

      case WM_KEYDOWN:
         if ( wParam==VK_ESCAPE ) PostMessage(hWnd, WM_CLOSE, 0, 0);
      break;

      case WM_CHAR:
         if (!isascii(wParam)) break;
#if 0
        printf("Posting keystroke %.02x\n", wParam);
        fflush(stdout);
#endif
        queue[qhead++] = tolower(wParam); 
        qhead &= 255;
      break;

      default:
      break;
   }
   return( DefWindowProc(hWnd, message, wParam, lParam) );
}

////////////////////////////////////////////////////////////////////////
//////////////////////// Win Main //////////////////////////////////////

         // Win $#@#@ initialization stuff...


int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow )
{
   WNDCLASS   wc;
   HDC hDC;
   char *message = "Press any Key to continue!";
   RECT Rect;
   MSG Msg;

   wc.style = CS_HREDRAW | CS_VREDRAW;;
   wc.lpfnWndProc = MainWndproc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hInstance;
   wc.hIcon = LoadIcon( NULL, IDI_APPLICATION);
   wc.hCursor = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = NULL; // GetStockObject( BLACK_BRUSH );
   wc.lpszMenuName = NULL;    // "Sk_Glide";
   wc.lpszClassName = _OUR_CLASS_NAME_;
//  wc.cbSize = sizeof( wc );
   if ( !RegisterClass( &wc ) ) return( 1 );

   hWndMain = CreateWindowEx(
      WS_EX_TOPMOST,
      _OUR_CLASS_NAME_,
      "W95 suxx",
      WS_POPUP,   
      0, 0,
      320, 200,
      NULL,
      NULL,
      hInstance,
      NULL );

   if( hWndMain==NULL ) return FALSE;

   ShowWindow( hWndMain, nCmdShow );
   UpdateWindow( hWndMain );
   SetFocus( hWndMain );

//   if(FullScreen) while( ShowCursor(NULL)>= 0 );
      /* 
       * Since printfs go into the bit bucket on Win32,
       * put up a message in the window.
       */

#if 0
   hDC = GetDC(hWndMain);
   GetClientRect( hWndMain, &Rect );
   SetTextColor( hDC, RGB( 255,0,0 ) );
   SetBkColor( hDC, RGB(0,0,0) );
   SetTextAlign( hDC, TA_CENTER );
   ExtTextOut( hDC, Rect.right/2, Rect.bottom/2, ETO_OPAQUE, NULL, 
       message, strlen(message), NULL );
   ReleaseDC(hWndMain, hDC);
#endif

#if 0
   while (GetMessage(&Msg, NULL, 0, 0))
   {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg); 
   }
#endif

   Launch_Main( lpCmdLine );

   printf( "Exiting winMain()\n" );
   fflush(stdout);

//   DestroyWindow( hWndMain );
   return( 0 );
}

//////////////////////////////////////////////////////////////////
