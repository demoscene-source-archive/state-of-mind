/***********************************************
 *              Demo driver.                   *
 *           sequences/timing/synchro          *
 * Skal 96                                     *
 ***********************************************/

#ifndef _DEMO_DRV_H_
#define _DEMO_DRV_H_

#if defined ( WIN32 )
#include <time.h>
#endif

#if defined( __WATCOMC__ )
#include <time.h>
#endif

#if defined( LNX ) || defined( UNIX ) || defined(__DJGPP__)
#include <sys/time.h>
#endif

   // in time.c

extern void My_setitimer( ULINT Start );
extern long My_getitimer( );
extern void My_closeitimer( );

typedef struct {
#pragma pack(1)

   long Timing;            // Sequence start
   long Length;            // sequence grouping for Tick_x value
   INT  Param;

   void (*Init)( INT Param );
   void (*Loop)( );
   void (*Close)( void );
   void (*Precompute)( INT );

   INT Pos, Row;           // music position. Useless for MP3
   INT Precompute_Flag;    // useless 

} PHASE;

extern long Timer;
extern INT Global_Timer, Phase;
extern INT No_Sound, Reset_Timer;
extern INT (*Bypass_Loop)( INT Timer );
extern void (*Post_Loop)( INT Timer );

   /* Bar_x is a float going from 0.0 to 1.0 every Bar_Bunch.
      Bar_dx is the increment from previous Bar_x value.
      Bar_x/dx is predicted/corrected every row of music. */
extern INT Bar_Count, Frames_Per_Bunch;
extern float Bar_x, Bar_dx;
extern float Time_Scale;
extern INT Bar_Bunch; 

   /* same, but based on tick-timing */
extern float Tick_x, Tick_dx;
extern float Global_x, Global_dx;

   /* mp_patpos is not a good counter for Linux, since MD_Update()
      is called *at bpm* => mp_patpos ~ jumps from 6 to 6. So,
      perform stat every EVERY_WHAT patpos segment... */
#define EVERY_WHAT   8

extern INT Precompute_Level;

extern void Init_Timing( INT , PHASE * );
extern void Set_Sound_Pos( INT Pos, INT Row );
extern void Check_Sound( );
// extern MODE_SPECS *Select_Video_Mode( INT, INT *, INT, MODE_SPECS * );
extern void Start_Demo( INT );
extern INT Advance_Timer( );
extern void Precompute_All( INT );
extern void Select_Player( char *Song_Name, int Music_Menu, int *Freqs );

#endif  // _DEMO_DRV_H_


