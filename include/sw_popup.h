/*********************************************
 *                                           *
 *	   sw_popup.h                             *
 *                                           *
 *                                           *
 * Skal widgets v0.0 (c)1997                 *
 * Pascal.Massimino@ens.fr                   *
 *********************************************/

#ifndef _SW_POPUP_H_
#define _SW_POPUP_H_

#include "sw_main.h"

/***************************************************************/

extern SW_WIDGET *SW_Create_Pop_Up( SW_TOP *Top, INT Length, INT Height,
     INT Pos_x, INT Pos_y, INT Aspect, INT Backup );

/***************************************************************/

extern UINT SW_Open_Dialog_Box( SW_TOP *Top,
   char *Text, char *Yes_Text, char *No_Text, char *Cancel_Text,
   INT Width, INT Height, INT X_Pos, INT Y_Pos, INT Aspect );

extern UINT SW_Pop_Up_Close_Cb( SW_WIDGET *, INT, INT, DRV_EVENT * );
extern void SW_Un_Pop_Up_ize_Widget( SW_WIDGET * );
extern UINT SW_Pop_Up_ize_Widget( SW_WIDGET * );
extern void SW_Clear_All_Pop_Up( SW_WIDGET * );

#define SW_Set_Pop_Up_Quit_Button(x,y)  {  (x)->Data = ( VOID *)(y); SW_Add_Callback( ( SW_WIDGET *)(x), Pop_Up_Close_Cb ); }

#define SW_ANSWER_1    1
#define SW_ANSWER_2    2
#define SW_ANSWER_3    3

#define SW_DIALOG_LINE_SKIP    20
#define SW_DIALOG_BUTTON_LEN   70

/***************************************************************/

#endif	/* _SW_POPUP_H_ */

