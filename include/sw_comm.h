/*********************************************
 *                                           *
 *	   sw_common.h                            *
 *                                           *
 *                                           *
 * Skal widgets v0.0 (c)1997                 *
 * Pascal.Massimino@ens.fr                   *
 *********************************************/

#ifndef _SW_COMMON_H_
#define _SW_COMMON_H_

#include "sw_main.h"

/***************************************************************/

extern void SW_Destroy_Widget( SW_WIDGET *Widget );
extern void SW_UnMap( SW_WIDGET *Widget );
extern UINT SW_Realize( SW_WIDGET *Widget );
extern void SW_Draw( SW_WIDGET *Widget );

extern void SW_Common_Frees( SW_WIDGET *Widget );
extern SW_WIDGET *SW_Common_Defaults( SW_WIDGET *Widget, SW_WIDGET *Parent,
   INT Width, INT Height, INT Pos_x, INT Pos_y, INT Backup );

extern void SW_Disable( SW_WIDGET *W );
extern void SW_Enable( SW_WIDGET *W );

extern void SW_Error( STRING Msg, SW_TOP *Top );
extern void SW_Exit( SW_TOP *Top, INT Out );

/***************************************************************/

#endif   /* _SW_COMMON_H_ */
