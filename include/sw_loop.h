/*********************************************
 *                                           *
 *	   sw_loop.h                              *
 *                                           *
 *                                           *
 * Skal widgets v0.0 (c)1997                 *
 * Pascal.Massimino@ens.fr                   *
 *********************************************/

#ifndef _SW_LOOP_H_
#define _SW_LOOP_H_

#include "sw_main.h"

   /* External feedback supplied by user's program */
   /* Can be NULL-defined...                       */

extern void ( *SW_Print_Description )( SW_TOP *, SW_WIDGET * );
extern void ( *SW_Print_To_Console )( SW_TOP *, STRING );
extern void ( *SW_Print_Allocs )( UINT );

/***************************************************************/

extern INT SW_Decode_Accelerator( BYTE );

extern EVENT_TYPE SW_Main_Loop( SW_WIDGET * );
extern void SW_Force_Event( SW_TOP *,
   UINT Event, char Key, UINT Modifier, INT x, INT y );
extern UINT SW_Filter_Event( SW_TOP *, UINT Event_Filter );

extern void SW_Set_Accelerator( SW_WIDGET *Widget, char Accel );
extern void SW_Remove_Accelerator( SW_WIDGET *Widget );

/***************************************************************/

#endif   /* _SW_LOOP_H_ */
