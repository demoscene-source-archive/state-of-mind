/*********************************************
 *                                           *
 *	   sw_btm.h                               *
 * Bitmap manipulation and drawing routines  *
 *                                           *
 *                                           *
 * Skal widgets v0.0 (c)1997                 *
 * Pascal.Massimino@ens.fr                   *
 *********************************************/

#ifndef _SW_BTM_H_
#define _SW_BTM_H_

#include "sw_main.h"

/********************************************************/

extern void SW_Scroll_Up( SW_WIDGET *, UINT, UINT );

extern void UnMap_Widget( SW_WIDGET *Widget );
extern void SW_Fill_Region( SW_WIDGET *W );
extern void SW_Auto_Fill( SW_WIDGET *W );

extern void SW_Refresh( SW_WIDGET *Widget );

extern void SW_Draw_Line( MEM_ZONE *, INT, INT, INT, INT, PIXEL );
extern void SW_Xor_Draw_Line( MEM_ZONE *, INT, INT, INT, INT, PIXEL );

/********************************************************/

#endif	/* _SW_BTM_H_ */

