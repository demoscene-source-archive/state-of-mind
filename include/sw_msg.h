/***********************************************
 *                                             *
 *    sw_msg.h                                 *
 *                                             *
 * Skal widgets v0.0 (c)1997                   *
 * Pascal.Massimino@ens.fr                     *
 ***********************************************/

#ifndef _SW_MSG_H_
#define _SW_MSG_H_

extern SW_WIDGET *SW_Create_Message( SW_WIDGET *Head,
   UINT Pos_x, UINT Pos_y, UINT Width, UINT Height, UINT Line_Height );

extern SW_WIDGET *SW_Create_Data( SW_WIDGET *Parent,
   UINT Pos_x, UINT Pos_y, UINT Width, UINT Height, SW_TYPE Data_Type );

extern void SW_End_Edition( SW_WIDGET * );
extern void SW_Attach_Data( SW_WIDGET *, void *, STRING, SW_TYPE );

extern void SW_Data_W_Set_Value( SW_DATA *, void * );
extern void SW_Data_W_Init_Value( SW_DATA *, void * );

#define SW_Set_Message_Real_Col_Background(x,y) { (x)->Aspect.Background = (y); SW_Fill_Widget( (x) ); }
#define SW_Set_Message_Background(x,y) { (x)->Aspect.Background = (y); SW_Fill_Widget( (x) ); }
#define SW_Set_Message_Real_Col(x,y)   ( (SW_MESSAGE *) (x) )->Text_Color = (y)
#define SW_Set_Message_Col(x,y)   ( (SW_MESSAGE *) (x) )->Text_Color = (y)
#define SW_Set_Data_Real_Text_Col(x,y)   ( (SW_DATA *) (x) )->Text_Color = (y)
#define SW_Set_Data_Text_Col(x,y)   ( (SW_DATA *) (x) )->Text_Color = (y)
#define SW_Set_Changed_Data_Cb(x,y) ( (SW_DATA *) (x) )->Data_Changed = (y)
#define SW_Set_Min_Max_Data_W(w,m,M) { (w)->Min = (m); (w)->Max = (M); }
#define SW_Data_W_New_Value(w) ( ( *((w)->Get_Value) )( (SW_WIDGET *)(w) ) )

#endif	/* _SW_MSG_H_ */

