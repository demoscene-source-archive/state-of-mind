/***********************************************
 *      X11 graphics Drivers                   *
 * Skal 96                                     *
 ***********************************************/

#ifndef _DRV_LNX_H_
#define _DRV_LNX_H_

#include <stdarg.h>

/********************************************************/
/********************************************************/

#ifdef USE_X11

#include <signal.h> 
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

/*
extern char *shmat( int, char *, int );
extern int   shmdt( char * );
*/

#define X11_FIELDS               \
   Display        *display;      \
   int             screen;       \
   int             depth;        \
   Visual         *visual;       \
   MEM_IMG        The_Display;   \
   PIXEL          *Base_Ptr;     \
   unsigned long  *Pixels;       \
   Colormap        Cmap;         \
   UINT            Cells;        \
   Window Win;                   \
   Cursor Void_Cursor;           \
   STRING         Window_Name

typedef struct {     /* Let's inherit...*/

   MEM_ZONE_DRIVER_FIELDS

   X11_FIELDS;

      /* X11 specific */

   GC             gc;
   XImage        *XImg;

      /* Shared memory */

   USHORT dummy;    // for alignment
   XShmSegmentInfo ShmInfo;
   int Completion_Type;

} MEM_ZONE_X11;


extern _G_DRIVER_ __G_X11_DRIVER_;
#define _G_X11_DRIVER_ (&__G_X11_DRIVER_)


   // needed by drv_dga.c.  => external

extern Display *X11_Open_Display( STRING Name );
extern void X11_Destroy_Resources( MEM_ZONE_X11* );
extern MEM_IMG *X11_Detect_Available_Modes( MEM_ZONE_X11 *);
extern INT X11_Undefine_Cursor( MEM_ZONE_X11 *X11 );
extern EVENT_TYPE X11_Get_Event( MEM_ZONE_DRIVER *M );
extern void X11_Print_Zone_Specs( STRING S, MEM_IMG *M );

extern INT X11_Change_CMap( MEM_ZONE_DRIVER *, INT Nb, COLOR_ENTRY *CMap );

#endif      // USE_X11

/******************** DGA display device ***********************/

#if defined( USE_DGA ) && !defined(UNIX)

#include <X11/extensions/xf86dga.h>

typedef struct {

   MEM_ZONE_DRIVER_FIELDS

      /* Same as X11 */

   X11_FIELDS;

      /* DGA specific */

   int    Maj, Min;
   int    First_Bank, Bank_Nb, Cur_Bank, Line_Width;
   int    Left_Over;
   int    Bank_Size, Mem_Size;
   int    Width, Height;
   int    Completion_Type, Error_Base;

} MEM_ZONE_DGA;

extern _G_DRIVER_ __G_DGA_DRIVER_;
#define _G_DGA_DRIVER_ (&__G_DGA_DRIVER_)

#endif      // USE_DGA

/********************************************************/

#if defined( USE_SVGALIB ) && !defined(UNIX)

#include "vga.h"
#include "vgagl.h"
#include "vgamouse.h"

typedef struct {

   MEM_ZONE_DRIVER_FIELDS

      /* SVGALIB specific */

   MEM_IMG          The_Display;
   GraphicsContext *The_SVGA_Context;
   GraphicsContext *The_Virtual_Context;
   int              The_SVGA_Mode;

} MEM_ZONE_SVGL;


extern _G_DRIVER_ __G_SVGL_DRIVER_;
#define _G_SVGL_DRIVER_ (&__G_SVGL_DRIVER_)

#endif      // USE_SVGALIB

/********************************************************/
/********************************************************/

#endif   // _DRV_LNX_H_

