/***********************************************
 *        Quick DDraw Driver                   *
 * Skal 98                                     *
 ***********************************************/

#ifndef _GFX_DDRAW_H_
#define _GFX_DDRAW_H_

#include "main.h"

#if defined( WIN32 )

extern void Register_Video_Support( INT Nb, ... );
extern void *Driver_Call( void *Driver, ... );
extern void Driver_Close( void * );
extern INT MEM_Height( void *M );
extern INT MEM_Width( void *M );
extern INT MEM_BpS( void *M );
extern USHORT *MEM_Base_Ptr( void *M );
extern void ZONE_FLUSH( void *M );
extern INT Driver_Get_Event( void *M );
extern BYTE *ZONE_SCANLINE( void *M, INT j );
extern int FullScreen;

#define DRV_NULL_EVENT  0
#define DRV_KEY_PRESS   1

typedef void *MEM_ZONE;

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

#define _G_SVGL_DRIVER_ NULL

#endif   // WIN32

#endif   // _GFX_DDRAW_H_
