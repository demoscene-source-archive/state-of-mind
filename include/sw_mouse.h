/***********************************************
 *         Interface functions                 *
 * Skal 97                                     *
 ***********************************************/

#ifndef _SW_MOUSE_H_
#define _SW_MOUSE_H_

#include "sw_main.h"

typedef struct {
#pragma pack(1)

   MEM_ZONE *Zone;   
   INT X, Y, Last_X, Last_Y;

} SW_INTERFACE;

/********************************************************************/

extern void SW_Destroy_Interface( SW_TOP *Top );
extern SW_INTERFACE *SW_Init_Interface( SW_TOP *Top );
extern void SW_Set_Mouse_Position( SW_TOP *Top, INT x, INT y );
extern INT SW_Set_Mouse_Pixmap( SW_TOP *Top, BITMAP *Btm );

/********************************************************************/

#endif   /* _SW_MOUSE_H_ */

