/***********************************************
 *        Quick DDraw Driver                   *
 * Skal 98                                     *
 ***********************************************/

#include "gfx_ddrw.h"

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdarg.h>

#include <direct.h>  // for getcwd()...

#define _OUR_CLASS_NAME_   "SkalWZ"

static DDSURFACEDESC ddsd;
static IDirectDraw *lpDD = NULL;
static IDirectDrawSurface *lpDDSPrimary = NULL;
static unsigned RedMask, GreenMask, BlueMask, AlphaMask;
static int Bpp;

extern int DoubleSize;
EXTERN int FullScreen = TRUE;
EXTERN HWND hWndMain;      // may be needed by DirectX objects...
EXTERN HINSTANCE hInst;
EXTERN INT  hWnd_Ok;
static int  Win32_KbHit( void );
static void Win32_Show_hWnd( void );

#define GFX

////////////////////////////////////////////////////////////////////////
#define THE_W	320
#define THE_H	200
static USHORT Buffer[THE_W*THE_H];
////////////////////////////////////////////////////////////////////////

EXTERN void Register_Video_Support( INT Nb, ... )
{
}

EXTERN  INT Driver_Change_CMap( void *M, INT Nb, COLOR_ENTRY *CMap )
{
   return( Nb );
}

EXTERN INT MEM_Width( void *M ) { return( THE_W ); }
EXTERN INT MEM_Height( void *M ) { return( THE_H ); }
EXTERN INT MEM_BpS( void *M ) { return( THE_W*sizeof(USHORT) ); }
EXTERN USHORT *MEM_Base_Ptr( void *M ) { return( Buffer ); }

// This is a terrible mess. Hacked up in 5mins :(

EXTERN void ZONE_FLUSH( void *M )
{
   int i, j, k, BpS;
   USHORT *Src, *Dst;

   if ( lpDD==NULL || lpDDSPrimary==NULL ) return;
   IDirectDrawSurface_Lock( lpDDSPrimary, NULL, &ddsd, 
      DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY, NULL );
   BpS = ddsd.lPitch / sizeof(USHORT);
   Src = (USHORT*)Buffer;
   Dst = (USHORT*)ddsd.lpSurface;
   if ( !FullScreen )
   {
      int max, W, H;
      RECT Rect;
      USHORT *Dst2;
      W = THE_W; H=THE_H;
      if (DoubleSize) { W*=2; H*=2; }

      GetWindowRect( hWndMain, &Rect );
      Rect.left  += GetSystemMetrics(SM_CXFRAME);
      Rect.right -= GetSystemMetrics(SM_CXFRAME);
      Rect.top  += GetSystemMetrics(SM_CYFRAME);
      Rect.top  += GetSystemMetrics(SM_CYCAPTION);
      Rect.bottom -= GetSystemMetrics(SM_CYFRAME);
      if ( Rect.right-Rect.left>W ) 
         Rect.right=Rect.left+W;
      if ( Rect.bottom-Rect.top>H )
        Rect.bottom=Rect.top+H;
      if (Rect.right<=0) goto End;
      if (Rect.bottom<=0) goto End;
      max = GetSystemMetrics(SM_CXSCREEN);
      if (Rect.left>=max) goto End;
      if (Rect.right>=max) Rect.right=max;
      max = GetSystemMetrics(SM_CYSCREEN);
      if (Rect.top>=max) goto End;
      if (Rect.bottom>=max) Rect.bottom=max;

      if ( Rect.left<0 ) {
         if ( DoubleSize ) Src -= Rect.left/2;
         else Src -= Rect.left;
         Rect.left=0;
      }
      if ( Rect.top<0 ) {
         if ( DoubleSize ) Src -= (Rect.top/2) * THE_W;
         else Src -= Rect.top * THE_W;
         Rect.top = 0;
      }
      i = Rect.right-Rect.left;
      j = Rect.bottom-Rect.top;
      Dst += Rect.left;
      Dst += Rect.top*BpS;

      if ( i>W ) i=W;
      if ( j>H ) j=H;
      if (!DoubleSize) goto Draw_Normal;

      i/=2; j/=2;
      Src += i;
      Dst+=2*i;
      Dst2 = Dst + BpS;
      BpS*=2;
      if ( ddsd.ddpfPixelFormat.dwRBitMask == 0xf800 )
      {
        for( ; j>0; --j )
        {    
          for(k=-i; k<0; k++ )
            Dst[2*k] = Dst[2*k+1] = Dst2[2*k] = Dst2[2*k+1] 
              = Src[k];
          Dst += BpS;
          Dst2 += BpS;
          Src += THE_W;
        }
      }
      else {
         for( ; j>0; --j )
         {
            for( k=-i; k<0; ++k ) {
               USHORT Col = ((Src[k]>>1)&0x7fe0) | (Src[k]&0x001f);
               Dst[2*k] = Dst[2*k+1] = Dst2[2*k] = Dst2[2*k+1] = Col;
            }
            Dst += BpS;
            Dst2 += BpS;
            Src += THE_W;
         }
      }
      goto End;
   }
   else { i=THE_W; j=THE_H; }

Draw_Normal:

   if ( ddsd.ddpfPixelFormat.dwRBitMask == 0xf800 )
      for( ; j>0; --j )
      {
         memcpy( Dst, Src, i*sizeof(USHORT) );
         Dst += BpS;
         Src += THE_W;
      }
   else {
      Src += i;
      Dst += i;
      for( ; j>0; --j )
      {
         for( k=-i; k<0; ++k )
            Dst[k] = ((Src[k]>>1)&0x7fe0) | (Src[k]&0x001f);
         Dst += BpS;
         Src += THE_W;
      }
   }
End:
   IDirectDrawSurface_Unlock( lpDDSPrimary, NULL );
}

EXTERN BYTE *ZONE_SCANLINE( void *M, INT j )
{
   return (BYTE*)( Buffer + j*THE_W );
}

EXTERN INT Driver_Get_Event( void *M )
{
   if ( Win32_KbHit( ) ) return( DRV_KEY_PRESS );
   return( DRV_NULL_EVENT );
}

////////////////////////////////////////////////////////////////////////

EXTERN void *Driver_Call( void *Driver, ... )
{
   //   DDSCAPS ddscaps;

   if (lpDD!=NULL) return (void *)lpDD;
   if( DirectDrawCreate( NULL, &lpDD, NULL )!=DD_OK )
      return( NULL );

   if ( !hWnd_Ok ) Win32_Show_hWnd( );

   if ( FullScreen==TRUE )
   {
      if ( IDirectDraw_SetCooperativeLevel( lpDD,
         hWndMain, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT ) != DD_OK )
         FullScreen = FALSE;
      else FullScreen = TRUE;
   }

   if ( FullScreen==FALSE )
   {
      if ( IDirectDraw_SetCooperativeLevel( lpDD, hWndMain, DDSCL_NORMAL) != DD_OK )
         return( NULL );
   }
   else 
   {
      int Ok;
      if ( DoubleSize )
         Ok = IDirectDraw_SetDisplayMode( lpDD, THE_W*2, THE_H*2, 16 );
      else Ok = IDirectDraw_SetDisplayMode( lpDD, THE_W, THE_H, 16 );
      if( Ok != DD_OK )
         return( NULL );
   }

   memset( &ddsd, 0, sizeof (ddsd ) );
   ddsd.dwSize = sizeof( ddsd );
   ddsd.dwFlags = DDSD_CAPS;
   ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
   if ( IDirectDraw_CreateSurface( lpDD, &ddsd, &lpDDSPrimary, NULL ) != DD_OK )
      return( NULL );
   if ( (!FullScreen) && (DoubleSize) ) {
      RECT Rect;
      Rect.left = Rect.top = 0;
      Rect.right = 2*THE_W;
      Rect.bottom = 2*THE_H;
      AdjustWindowRect( &Rect, WS_OVERLAPPEDWINDOW, FALSE );
      SetWindowPos( hWndMain, NULL,
      (GetSystemMetrics(SM_CXSCREEN)-2*THE_W)/2,
      (GetSystemMetrics(SM_CYSCREEN)-2*THE_H)/2,
      Rect.right-Rect.left, Rect.bottom-Rect.top,
      SWP_NOCOPYBITS | SWP_NOZORDER );
   }
   return (void *)lpDD;
}

EXTERN void Driver_Close( void *M )
{
   if ( lpDD!=NULL )
   {
      if (FullScreen) IDirectDraw_RestoreDisplayMode(lpDD);
      if ( lpDDSPrimary!=NULL ) IDirectDraw_Release(lpDDSPrimary);
      lpDDSPrimary = NULL;
      IDirectDraw_Release(lpDD);
      lpDD = NULL;
   }
}

///////////////////////////////////////////////////////////////////

static int qhead = 0;
static int qtail = 0;
static int queue[256];

//////////////////////////////////////////////////////////////////
// 
// Converts #@%@!@ win's lpCmdLine into argc, argv
//

extern int main(int argc, char **argv);

static void Launch_Main( LPSTR lpCmdLine )
{
   char   *argvbuf[128];
   char    cmdLineBuffer[2048];
   char  *p, *pEnd;
   int    argc;
   char **argv = argvbuf;

   argc = 0;
   argvbuf[argc++] = "(Linux does it better!)";

   if ( lpCmdLine == NULL) goto End;

   strcpy( cmdLineBuffer, lpCmdLine );
   p = cmdLineBuffer;
   pEnd = p + strlen(cmdLineBuffer);
   if ( pEnd >= &cmdLineBuffer[2046] ) pEnd = &cmdLineBuffer[2046];

   while ( 1 )
   {
      while (*p == ' ') p++;   // skip over white space
      if ( p>=pEnd ) break;

      argvbuf[argc++] = p;
      if ( argc>=128 ) break;
   
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

static void Win32_Show_hWnd( )
{
   if ( hWndMain==NULL || hWnd_Ok==TRUE ) return;
   ShowWindow( hWndMain, SW_SHOWNORMAL );
   UpdateWindow( hWndMain );

   SetFocus( hWndMain );
   hWnd_Ok = TRUE;
//  if(FullScreen) while( ShowCursor(NULL)>= 0 );
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
   RECT Rect; 

   if (hPrevInstance!=NULL) return FALSE;
   hInst = hInstance;

   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = MainWndproc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hInstance;
   wc.hIcon = NULL; //LoadIcon( NULL, IDI_APPLICATION);
   wc.hCursor = NULL; //LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = NULL; // GetStockObject( BLACK_BRUSH );
   wc.lpszMenuName = NULL;    // "Sk_Glide";
   wc.lpszClassName = _OUR_CLASS_NAME_;
//  wc.cbSize = sizeof( wc );
   if ( !RegisterClass( &wc ) ) return( 1 );

   Rect.left = Rect.top = 0;
   Rect.right = THE_W;
   Rect.bottom = THE_H;
   AdjustWindowRect( &Rect, WS_OVERLAPPEDWINDOW, FALSE );

   hWndMain = CreateWindowEx(
      WS_EX_APPWINDOW,  // WS_EX_TOPMOST,
      _OUR_CLASS_NAME_,
      "Linux does it better",
      WS_SYSMENU|WS_OVERLAPPED   //WS_VISIBLE|WS_POPUP,
       |WS_THICKFRAME,
      CW_USEDEFAULT, CW_USEDEFAULT,
      Rect.right-Rect.left, Rect.bottom-Rect.top,
      NULL,
      NULL,
      hInstance,
      NULL );

   if( hWndMain==NULL ) return FALSE;

   SetWindowPos( hWndMain, NULL,
      (GetSystemMetrics(SM_CXSCREEN)-THE_W)/2,
      (GetSystemMetrics(SM_CYSCREEN)-THE_H)/2,
      Rect.right-Rect.left, Rect.bottom-Rect.top,
      SWP_NOCOPYBITS | SWP_NOZORDER );
      
   hWnd_Ok = FALSE;

//   ShowWindow( hWndMain, nCmdShow );
//   UpdateWindow( hWndMain );

//   SetFocus( hWndMain );
//   if(FullScreen) while( ShowCursor(NULL)>= 0 );

   Launch_Main( lpCmdLine );

   ShowWindow(hWndMain, SW_HIDE);
   DestroyWindow( hWndMain );
   hWndMain = NULL;
   hWnd_Ok = FALSE;

   return( 0 );
}

//////////////////////////////////////////////////////////////////
