/*
 * Win32 interface
 *
 * Skal98 (skal.planet-d.net)
 ***************************************/

//////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
//#include <direct.h>
extern HWND hWndMain;
extern HINSTANCE hInst;
#include "../resource/resource.h"

extern long FAR PASCAL DiagProc(
   HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

// this is a hack
extern int _MP3_Out_;
extern char *MP3_File;
extern int No_Sound;
extern int FullScreen;
extern int DoubleSize;
   // ... same as mp3.h
#define MP3_OUT_8BITS   0
#define MP3_OUT_16BITS  0x01
#define MP3_OUT_MONO    0
#define MP3_OUT_STEREO  0x02
#define MP3_DEBUG       0x04
#define MP3_ASK_DRIVER  0x08

extern void Load_All();

static void Get_Values( HWND hDlg )
{
   No_Sound = IsDlgButtonChecked( hDlg, IDC_NOSOUND );
   FullScreen = !IsDlgButtonChecked( hDlg, IDC_NOFULL );
   DoubleSize = IsDlgButtonChecked( hDlg, IDC_DSIZE );
   if ( IsDlgButtonChecked( hDlg, IDC_8BITS ) )
     _MP3_Out_ &= ~MP3_OUT_16BITS;
   else _MP3_Out_ |= MP3_OUT_16BITS;
   if ( IsDlgButtonChecked( hDlg, IDC_MONO ) )
     _MP3_Out_ &= ~MP3_OUT_STEREO;
   else _MP3_Out_ |= MP3_OUT_STEREO;
}

static void Set_Values( HWND hDlg )
{
   HWND Idc_8bits = GetDlgItem( hDlg, IDC_8BITS );
   HWND Idc_Mono = GetDlgItem( hDlg, IDC_MONO );
   HWND Idc_DSize = GetDlgItem( hDlg, IDC_DSIZE );

   CheckDlgButton( hDlg, IDC_NOFULL, !FullScreen );
   CheckDlgButton( hDlg, IDC_NOSOUND, No_Sound );
   CheckDlgButton( hDlg, IDC_DSIZE, DoubleSize );
   CheckDlgButton( hDlg, IDC_MONO, 
      _MP3_Out_ & MP3_OUT_STEREO ? FALSE : TRUE );
   CheckDlgButton( hDlg, IDC_8BITS, 
      _MP3_Out_ & MP3_OUT_16BITS ? FALSE : TRUE );

   if ( No_Sound )
   {
      EnableWindow( Idc_8bits, FALSE );
      EnableWindow( Idc_Mono, FALSE );
   }
   else
   {
      EnableWindow( Idc_8bits, TRUE );
      EnableWindow( Idc_Mono, TRUE );
   }

   if ( FullScreen )
      EnableWindow( Idc_DSize, FALSE );
   else EnableWindow( Idc_DSize, TRUE );
}

long FAR PASCAL DiagProc( 
   HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch( msg )
   {
      case WM_INITDIALOG:
         Set_Values( hDlg );
      break;   
      case WM_DESTROY: return FALSE; break;
      case WM_COMMAND:
         switch( LOWORD(wParam) )
         {
            case IDOK:
               Get_Values(hDlg);             
               if (FullScreen) DoubleSize = FALSE;
               SetDlgItemText( hDlg, IDC_TEXT, "Loading && massaging..." );
               Load_All();
               EndDialog( hDlg, 1 );
            break;
            case IDC_NOSOUND:
            case IDC_NOFULL:
            case IDC_8BITS:
            case IDC_DSIZE:
            case IDC_MONO:
               Get_Values( hDlg );
               Set_Values( hDlg );
            break;
            case IDCANCEL:
               EndDialog( hDlg, 0 );
            break;
         }
      break;

      default: return FALSE; break;
   }
   return TRUE;
}

int Launch_Dialog() {
   int ret = DialogBoxParam( hInst,
         MAKEINTRESOURCE(IDD_DIALOG),
         hWndMain, (DLGPROC)DiagProc, (LPARAM)NULL );
   if ( ret==1 ) {
      ShowWindow(hWndMain,SW_SHOWNORMAL);
      SetForegroundWindow(hWndMain);
      UpdateWindow(hWndMain);
   }
   return ret;
}


#endif // WIN32
