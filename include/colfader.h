/*****************************************************
 *          Color fader. Old                         *
 *                                                   *
 *                       Skal96                      *
 *****************************************************/

#ifndef _COLFADER_H_
#define _COLFADER_H_

#include "main.h"

/*****************************************************/

typedef struct {
#pragma pack(1)

   SHORT Timer, Start, Size, Speed; 
   COLOR_ENTRY Pal[256];
   COLOR_ENTRY Dst_Pal[256];
   SHORT Col_Fix8[256*3*2];
   void *Drv;

} COLOR_FADER;

extern COLOR_FADER *Drv_Setup_Fader(
   COLOR_FADER *CF, void *Drv, INT Start, INT Size, INT Speed );
extern SHORT Drv_Prepare_Fade_To_Color( COLOR_FADER *CF, PIXEL R, PIXEL G, PIXEL B );
extern SHORT Drv_Do_Fade_To( COLOR_FADER *CF );

#define DRV_DO_FADE_TO(CF)        if ((CF)->Timer) Drv_Do_Fade_To( (CF) )

/*****************************************************/

#endif   // _COLFADER_H_
