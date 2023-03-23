/*
 *  Setup.
 *
 * Skal 98 (skal.planet-d.net)
 ***************************************/

#ifndef _DEMO_H_
#define _DEMO_H_

#include "main.h"
#include "demo_drv.h"
#include "btm.h"
#include "truecol.h"
#include "drv_io.h"
#include "ld_any.h"
#include "bump.h"
#include "vbuf.h"
#include "dmask.h"
#include "main3d.h"
// #include "rand.h"
#include "sprite.h"

// #define PREVIEW
#if !defined(WIN32)
#define DUMP_OK
#endif
// #define DEBUG_OK
#define FINAL

#ifdef DEBUG_OK
extern void Debug( );
extern void Setup_Video( );
#endif

/**************************** Global variables ***********************/

#define The_W     320
#define The_H     200
#define The_WxThe_H  (The_W*The_H)

#define BEAT_TIMER_III 1924
extern INT Low_Mem;

/*********************************************************************/

#define PASTE_ALL(X,Y,B) Paste_III( ZONE_SCANLINE( The_Screen_16b, (Y) ) + (X),   \
      MEM_BpS( The_Screen ), (B)->Bits, (B)->Width, (B)->Height, (B)->Width )  \

#define PASTE_ALL_OFF(X,Y,B,O) Paste_III_Offset( ZONE_SCANLINE( The_Screen_16b, (Y) ) + (X),   \
      MEM_BpS( The_Screen ), (B)->Bits, (B)->Width, (B)->Height, (B)->Width, (O) )  \

#define PASTE(X,Y,Xo,Yo,W,H,T) Paste_III( ZONE_SCANLINE( The_Screen, (Y) ) + (X),   \
      MEM_BpS( The_Screen ),                       \
      (T)->Bits + (Xo) + (Yo)*(T)->Width,          \
      (W), (H), (T)->Width )

/***********************************************************************/

#include "fx.h"
#include "mix.h"
#include "bilin.h"
#include "parts.h"      // declarations des parts+effects

/***********************************************************************/
/***********************************************************************/

#endif   // _DEMO_H_

