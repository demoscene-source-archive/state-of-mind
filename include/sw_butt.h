/*********************************************
 *                                           *
 *	   sw_button.h                            *
 *                                           *
 *                                           *
 * Skal widgets v0.0 (c)1997                 *
 * Pascal.Massimino@ens.fr                   *
 *********************************************/

#ifndef _SW_BUTTON_H_
#define _SW_BUTTON_H_

extern SW_WIDGET *SW_Create_Button( SW_WIDGET *Parent,
   INT Pos_x, INT Pos_y, INT Width, INT Height, STRING Label );

extern void SW_Set_Button_Label( SW_WIDGET *, STRING );
extern void SW_Set_Button_Action( SW_WIDGET *, SW_MOUSE_METHOD Func );

#define SW_Assign_Button_Data(b,d)  ( ( SW_BUTTON *) b )->Data = ( VOID *)(d)

#define SW_Set_Button_Text_Real_Col(x,y)   ( ( SW_BUTTON * ) (x) )->Text_Color = SW_Colors[ (y) ]
#define SW_Set_Button_Text_Col(x,y)   ( ( SW_BUTTON * ) (x) )->Text_Color = SW_Colors[ (y) ]

/***************************************************************/
/***************************************************************/

#endif	/* _SW_BUTTON_H_ */

