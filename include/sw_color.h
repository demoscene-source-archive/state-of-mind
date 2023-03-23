/***********************************************
 *                                             *
 *    sw_color.h                               *
 *                                             *
 * Skal widgets v0.0 (c)1997                   *
 * Pascal.Massimino@ens.fr                     *
 ***********************************************/
   
#ifndef _SW_COLOR_H_
#define _SW_COLOR_H_

#include "sw_main.h"
   
typedef PIXEL SW_COLOR[ 3 ];

enum {
   SW_BLACK  = 0,
   SW_GRAY_1 = 1,
   SW_GRAY_2 = 2,
   SW_GRAY_3 = 3,
   SW_GRAY_4 = 4,
   SW_GRAY_5 = 5,
   SW_GRAY_6 = 6,
   SW_GRAY_7 = 7,
   SW_GRAY_8 = 8,
   SW_GRAY_9 = 9,
   SW_GRAY_10 = 10,
   SW_WHITE  = 11, 
   SW_RED    = 12, 
   SW_BLUE   = 13,
   SW_GREEN  = 14,
   SW_ORANGE = 15, 
   SW_PINK   = 16, 
   SW_SKY_BLUE = 17,
   SW_YELLOW   = 18, 
   SW_TRANSPARENT = 19,
   SW_LAST_COLOR = 20
};

extern PIXEL SW_Colors[ SW_LAST_COLOR ];
extern void SW_Store_Default_CMap( SW_WIDGET *Top );
extern void SW_Store_Global_CMap( SW_WIDGET *Top, INT Nb, COLOR_ENTRY *New );

#endif /* _SW_COLOR_H_ */
