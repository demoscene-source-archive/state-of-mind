/*********************************************
 *                                           *
 *	   sw_border.h                            *
 * Bitmap manipulation and drawing routines  *
 *                                           *
 *                                           *
 * Skal widgets v0.0 (c)1997                 *
 * Pascal.Massimino@ens.fr                   *
 *********************************************/

#ifndef _SW_BORDER_H_
#define _SW_BORDER_H_

#include "sw_main.h"

/************************************************************/

typedef enum {

   SW_NO_BORDER,
   SW_BORDER1,
   SW_BORDER2,
   SW_BORDER3,
   SW_BORDER4,
   SW_BORDER5,
   SW_BORDER6

} SW_BORDER_TYPE;

/************************************************************/

extern void SW_Destroy_Border( SW_BORDER *Border );
extern SW_BORDER *SW_New_Border( );

extern SW_BORDER *SW_Set_Widget_Border( SW_WIDGET *W, SW_BORDER_TYPE Type );
extern SW_BORDER *SW_Set_Border( SW_WIDGET *Top, SW_ASPECT *Aspect,
   SW_BORDER_TYPE Type );

extern void SW_Xor_Draw_Box( MEM_ZONE *M, INT x1, INT y1, INT x2, INT y2, PIXEL Mask );
extern void SW_Draw_Box( MEM_ZONE *M, INT x1, INT y1, INT x2, INT y2, PIXEL Col );
extern void SW_Draw_Square( MEM_ZONE *M, INT x, INT y, INT W, INT H, PIXEL Col );

extern void SW_Draw_H_Ruler( MEM_ZONE *M, INT y, INT x1, INT x2,
   PIXEL Col1, PIXEL Col2 );
extern void SW_Draw_V_Ruler( MEM_ZONE *M, INT x, INT y1, INT y2,
   PIXEL Col1, PIXEL Col2 );

extern void SW_Draw_Highlight( MEM_ZONE *M, SW_ASPECT *Aspect );
extern void SW_Draw_Border( MEM_ZONE *M, SW_ASPECT *Aspect );

/************************************************************/

#endif	/* _SW_BORDER_H_ */

