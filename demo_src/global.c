/***********************************************
 *              Demo driver.                   *
 *      frequently used global variables       *
 * Skal97                                      *
 ***********************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "config.h"
#include "main.h"

/********************************************************************/

EXTERN long Timer = -1;       // current sequence timer
EXTERN INT Global_Timer = 0;  // whole demo counter
EXTERN INT Phase = -1;
EXTERN INT No_Sound = FALSE;

EXTERN FLT Tick_x = 0.0, Tick_dx = 0.0;
EXTERN FLT Global_x = 0.0, Global_dx = 0.0;

EXTERN INT Precompute_Level = 2;
EXTERN INT (*Bypass_Loop)( INT Timer ) = NULL;
EXTERN void (*Post_Loop)( INT Timer ) = NULL;

/********************************************************************/

#endif   // _GLOBAL_H_
