/***********************************************
 *          Graphics Drivers                   *
 * Skal 96                                     *
 ***********************************************/

#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "main.h"
#include <stdarg.h>

/********************************************************/

   /* add driver-specific arguments and options here...              */
   /* bits 0-7 contains the number of arguments needed for command.  */
   /* bits >=8 contains a key code different for each command        */
typedef enum {

   DRV_END_ARG    = 0x0100,
   DRV_DETECT     = 0x0200,
   DRV_DETECT_VBE = 0x0300,
   DRV_MODE       = 0x0401,
   DRV_CONVERT    = 0x0500,
   DRV_DISPLAY    = 0x0601,
   DRV_DONT_USE   = 0x0701,
   DRV_NAME       = 0x0801,
   DRV_PRINT_INFO = 0x0900,
   DRV_DIRECT     = 0x0A00,
   DRV_FORCE      = 0x0B01

} VAR_ARG_TYPE;

   // REFRESH Methods for both DOS/VBE and Linux/X11.

typedef enum {

   _VOID_        = 0x00,

      // DOS/VBE specific

   _INT_10h_     = 0x01,     // Basic int(10, ) call
   _RM_BNK_SW_   = 0x02,     // R-Mode call
   _PM_BNK_SW_   = 0x04,     // P-Mode call
   _LFB_         = 0x08,     // LFB
   _VGA_MODE_    = 0x10,

   _SPEC_LFB_    = 0x20,
   _SPEC_BNK_SW_ = 0x40,
   _BNK_SW_      = 0x47,     // all bank switch
   _ALL_LFB_     = 0x28,

      // X11 specific

   _X_WINDOWS_   = 0x0100,
   _SHM_         = 0x0200,

      // DDRAW_SPECIFIC

   _FULL_SCREEN_ = 0x1000,
   _DIRECT_X_    = 0x2000
   
} REFRESH_MTHD;

#define _ALL_DOS_METHODS_  (0x00FF)
#define _ALL_X11_METHODS_  (0x0F00)
#define _ALL_REFRESH_METHODS_  (_ALL_DOS_METHODS_|_ALL_X11_METHODS_)
#define _ALL_DDRAW_METHODS_ (_FULL_SCREEN_|_DIRECT_X_)

typedef struct {
#pragma pack(1)

   UINT Detect, Print, Convert;
   REFRESH_MTHD  Refresh_Method;
   void *Video_Card;
   INT Nb_Modes;
   MEM_IMG *Modes;
   STRING Window_Name;
   STRING Display_Name;
   UINT Direct, Force_Convert;

} DRIVER_ARG;

/********************************************************/

typedef struct DRV_EVENT DRV_EVENT;

   // Events

typedef enum {

   DRV_NULL_EVENT = 0x00,
   DRV_CLICK1     = 0x01,
   DRV_CLICK2     = 0x02,
   DRV_CLICK3     = 0x04,
   DRV_MOVE       = 0x08,
   DRV_RELEASE1   = 0x10,
   DRV_RELEASE2   = 0x20,
   DRV_RELEASE3   = 0x40,
   DRV_KEY_PRESS  = 0x80,
   DRV_CLICK      = (0x01|0x02|0x04),
   DRV_RELEASE    = (0x10|0x20|0x40)

} EVENT_TYPE;

typedef enum {

   DRV_NO_MODIFIER    = 0x00,
   DRV_ALT_MODIFIER   = 0x01, 
   DRV_CTRL_MODIFIER  = 0x02,
   DRV_SHIFT_MODIFIER = 0x04,
   DRV_UP_MODIFIER    = 0x08,
   DRV_DOWN_MODIFIER  = 0x10,
   DRV_LEFT_MODIFIER  = 0x20,
   DRV_RIGHT_MODIFIER = 0x40

} MODIFIER_TYPE;

struct DRV_EVENT {
#pragma pack(1)

   EVENT_TYPE  Event;
   BYTE  Key;
   MODIFIER_TYPE  Modifier;
   INT   x, y;

};

/********************************************************/

typedef struct _G_DRIVER_ _G_DRIVER_;

#define MEM_ZONE_DRIVER_FIELDS   \
   MEM_ZONE_FIELDS               \
   INT          Nb_Modes;        \
   MEM_IMG     *Modes;           \
   INT          Nb_Req_Modes;    \
   MEM_IMG     *Req_Modes;       \
   INT          Cur_Req_Mode;    \
   REFRESH_MTHD Refresh;         \
   DRV_EVENT    Event;           \
   DRV_EVENT    Last_Event;      \
   _G_DRIVER_  *Driver;          \
   INT (*Cleanup_Mode)( MEM_ZONE * );

typedef struct MEM_ZONE_DRIVER MEM_ZONE_DRIVER;

struct MEM_ZONE_DRIVER {
#pragma pack(1)

   MEM_ZONE_DRIVER_FIELDS

};

/********************************************************/

struct _G_DRIVER_ {
#pragma pack(1)

   MEM_ZONE_DRIVER *(*Change_Mode)( MEM_ZONE_DRIVER * );
   EVENT_TYPE (*Get_Event)( MEM_ZONE_DRIVER * );
   MEM_ZONE_DRIVER *(*Setup)( _G_DRIVER_ *, void *);
   void (*Print_Info)( MEM_ZONE_DRIVER *);
   INT (*Adapt)( MEM_ZONE_DRIVER *, void *);
   MEM_ZONE *(*Open)( MEM_ZONE_DRIVER *, INT );
   INT (*Change_Colors)( MEM_ZONE_DRIVER *, INT, COLOR_ENTRY *CMap );

};

/********************************************************/ 

#if defined( USE_DDRAW )
#include "drv_ddrw.h"
#undef MEM_IMAGE     // <= defined in winnt.h!!!!
#endif

#if defined(__DJGPP__) || defined(__WATCOMC__)
#include "drv_vbe.h"
#endif

#if defined( LNX ) || defined( UNIX )
#include "drv_lnx.h"
#endif

extern void Register_Video_Support( INT Nb, ... );

#define MAX_VIDEO_SUPPORT   3

/********************************************************/

      // complete setup for all drivers
#ifndef _G_X11_DRIVER_
#define _G_X11_DRIVER_ NULL
#endif
#ifndef _G_DGA_DRIVER_
#define _G_DGA_DRIVER_ NULL
#endif
#ifndef _G_SVGL_DRIVER_
#define _G_SVGL_DRIVER_ NULL
#endif

#ifndef _G_VBE_DRIVER_
#define _G_VBE_DRIVER_  NULL
#endif
#ifndef _G_DDRAW_DRIVER_
#define _G_DDRAW_DRIVER_   NULL
#endif

/********************************************************/

#ifdef SKL_LIGHT_CODE

#define SET_DRV_ERR(S)  { }
#define SET_DRV_ERR2(S,S2)  { }
#define Driver_Print_Error( ) { }
#define _Drv_Err_ NULL
#define _Drv_Err_2_ NULL

#else

#define SET_DRV_ERR(S)  { _Drv_Err_ = (S); _Drv_Err_2_ = NULL; }
#define SET_DRV_ERR2(S,S2)  { _Drv_Err_ = (S); _Drv_Err_2_ = (S2); }
extern STRING _Drv_Err_, _Drv_Err_2_;
extern void Driver_Print_Error( );

#endif   // SKL_LIGHT_CODE

extern MEM_IMAGE Driver_Call( _G_DRIVER_ *Driver, ... );

extern MEM_IMAGE Driver_Open_Mode( MEM_IMAGE M, INT Nb );
extern void Driver_Close( MEM_IMAGE  );
extern EVENT_TYPE Driver_Get_Event( MEM_IMAGE M );
extern INT Driver_Change_CMap( MEM_IMAGE M, INT Nb, COLOR_ENTRY *CMap );
extern INT Driver_Nb_Req_Modes( MEM_IMAGE M );
extern INT Driver_Cur_Req_Mode( MEM_IMAGE M );
extern void Drv_Print_Req_Infos( MEM_ZONE_DRIVER *M, void (*Print)( STRING, MEM_IMG * ) );
extern INT Zone_Width( MEM_IMAGE M );
extern INT Zone_Height( MEM_IMAGE M );
extern INT Zone_BpS( MEM_IMAGE M );
extern INT Zone_Pad( MEM_IMAGE M );
extern PIXEL *Zone_Base_Ptr( MEM_IMAGE M );
extern INT Zone_Quantum( MEM_IMAGE M );
extern FORMAT Zone_Format( MEM_IMAGE M );
extern INT Zone_Size( MEM_IMAGE M );
extern MEM_IMAGE Zone_Dst( MEM_IMAGE M );

extern PIXEL *Zone_Scanline( MEM_IMAGE M, INT Y );
extern INT Zone_Propagate( MEM_IMAGE M, INT X, INT Y, INT W, INT H );
extern INT Zone_Flush( MEM_IMAGE M );
extern INT Zone_Flush_Safe( MEM_IMAGE M );
extern void Zone_Set_Position( MEM_IMAGE M, INT X, INT Y );
extern void Zone_Destroy( MEM_IMAGE M );

extern INT Zone_Flags( MEM_IMAGE M );
extern INT Zone_Clear_Flags( MEM_IMAGE M, INT Flag );
extern INT Zone_Set_Flags( MEM_IMAGE M, INT Flag );
extern INT Zone_Xo( MEM_IMAGE M );
extern INT Zone_Yo( MEM_IMAGE M );

extern DRV_EVENT *Zone_Event( MEM_IMAGE M );

extern MEM_IMG *Parse_Mode_String( STRING S, INT *Nb_Modes, MEM_IMG **M );
extern void Print_Zone_Specs( STRING What, MEM_IMG *Zone );

/********************************************************/
/********************************************************/
/********************************************************/

#endif   // _DRIVER_H_

