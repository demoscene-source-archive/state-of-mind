/***********************************************
 *                DDRAW Driver                 *
 * Skal 97                                     *
 ***********************************************/

#ifndef _DRV_DDRW_H_
#define _DRV_DDRW_H_

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdarg.h>

typedef struct MEM_ZONE_DDRAW MEM_ZONE_DDRAW;

struct MEM_ZONE_DDRAW {   

   MEM_ZONE_DRIVER_FIELDS

      /* DDRAW specific */

   MEM_IMG      The_Display;
   STRING       Window_Name;

   IDirectDraw *lpDD;
   DDSURFACEDESC dPrim;
   HWND         Win;  // Window to which is attached the DDraw
   int          Fullscreen;

   IDirectDrawSurface *lpDDSPrimary;
   IDirectDrawPalette *lpDDPalette;
   unsigned RedMask, GreenMask, BlueMask, AlphaMask;
   unsigned Pitch;
   INT Bpp;

//   IDirectDrawClipper *lpDDClipper;
//   IDirectDrawSurface *lpDDSBack;
};

extern HWND hWndMain;

extern _G_DRIVER_ __G_DDRAW_DRIVER_;
#define _G_DDRAW_DRIVER_ (&__G_DDRAW_DRIVER_)

#define _OUR_CLASS_NAME_   "SkalWZ"

/********************************************************/
/********************************************************/

#endif   // _DRW_DDRW_H_
