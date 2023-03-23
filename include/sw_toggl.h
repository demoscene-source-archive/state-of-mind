/*********************************************
 *                                           *
 *	   sw_button.h                            *
 * Buttons and menu buttons                  *
 *                                           *
 * Widget lib v0.1 (c)1996                   *
 * Pascal.Massimino@ens.fr                   *
 *********************************************/

#ifndef _SW_TOGGLE_H_
#define _SW_TOGGLE_H_

SW_WIDGET *Create_Toggle_W( SW_TOP *Top, SW_WIDGET *Parent,
   INT Pos_x, INT Pos_y,
   char *Label1, char *Label2, INT Icon1, INT Icon2 );

SW_WIDGET *Create_Radio_B_W( SW_TOP *Top, SW_WIDGET *Parent,
                           INT Pos_x, INT Pos_y, 
                           char *Label1, char *Label2,
                           INT Icon1, INT Icon2, SW_TOGGLE *First );

#define SW_Assign_Toggle_Data(b,d)  ( ( SW_TOGGLE *) b )->Data = ( VOID *)(d)
#define SW_Set_Toggle_State(b,s)  ( ( SW_TOGGLE *) b )->State = ( UINT )(s)

extern void SW_Set_Toggle_Warn_Callback( SW_TOGGLE *, VOID *, VOID (*)( SW_TOGGLE *, VOID *) );

/***************************************************************/

#endif	/* _SW_TOGGLE_H_ */

