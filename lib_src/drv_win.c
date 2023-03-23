/*
 *
 * This segment simulates main() for Windows, creates a window, etc.
 *
 **********************************************************************/

#include "main.h"

#if defined( WIN32 ) && defined( USE_DDRAW )

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef MEM_IMAGE        // <= defined in winnt.h!!!!
#include "mem_map.h"
#include "driver.h"

EXTERN HWND hWndMain = NULL;    // Forward declaration

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

   while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);      // this might change qhead
      if (qhead != qtail) return( 1 );
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

/////////////////////////////////////////////////////////////////////////

         // initApplication
         // Win $#@#@ initialization stuff...

#if 0
static BOOL initApplication( HANDLE hInstance, int nCmdShow )
{
   hWndMain = CreateWindowEx(
#if 0                         
        WS_EX_APPWINDOW  gives you regular borders?
        WS_EX_TOPMOST    Works as advertised.
#endif
      0, // WS_EX_APPWINDOW,
      _OUR_CLASS_NAME_,
      "Win32",
#if 0
      WS_OVERLAPPED | WS_CAPTION  | WS_THICKFRAME | WS_MAXIMIZEBOX | 
      WS_MINIMIZEBOX | 
//      WS_VISIBLE |    // so we don't have to call ShowWindow
      WS_POPUP |      // non-app window */
      WS_SYSMENU,     // so we get an icon in the tray
      CW_USEDEFAULT, CW_USEDEFAULT,
      GetSystemMetrics(SM_CXSCREEN),
      GetSystemMetrics(SM_CYSCREEN),
#endif
      WS_POPUP,
      0, 0,
      320, 200,
      NULL,
      NULL,
      hInstance,
      NULL
   );
   if( !hWndMain ) return( FALSE );

   SetCursor(NULL);

   ShowWindow( hWndMain, SW_NORMAL );
   UpdateWindow( hWndMain );

   return( TRUE );
}
#endif

//////////////////////// Win Main //////////////////////////////////////

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow )
{
   WNDCLASS   wc;
   HDC hDC;
   char *message = "Press any Key to continue!";
   RECT Rect;

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
      GetSystemMetrics( SM_CXSCREEN ),
      GetSystemMetrics( SM_CYSCREEN ),
      NULL,
      NULL,
      hInstance,
      NULL );

//   if( hWndMain==NULL ) return FALSE;
   if ( hWndMain!=NULL )
   {
      ShowWindow( hWndMain, nCmdShow );
      UpdateWindow( hWndMain );
   }

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

   Launch_Main( lpCmdLine );

   printf( "Exiting winMain()\n" );
   fflush(stdout);

//   DestroyWindow( hWndMain );
   return( 0 );
}

//////////////////////////////////////////////////////////////////

#endif      // WIN32
