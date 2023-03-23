/********************************************
 *                                          *
 *           sw.h                           *
 * Functions set used by others widgets...  *
 *                                          *
 * SW v0.0 (c)1997                          *
 * Pascal.Massimino@ens.fr		              *
 ********************************************/

#ifndef _SW_H_
#define _SW_H_


   /*********** External entry point ***********/

#define SW_Set_Top_Widget_Tree(x,y) (x)->Widgets = ( SW_WIDGET *)(y)

extern SW_WIDGET *SW_Create_Top_Widget( MEM_ZONE *M );

   /****************** Macros *****************/

#define SW_Set_Background(x,y) { (x)->Background = (y); }

#define SW_Set_Outline_Real_Col(x,y) (x)->Outline = (y)
#define SW_Set_Outline_Col(x,y) (x)->Outline = (y)

#define SW_Set_Descript(x,y) (x)->Descript = (y)
#define SW_No_Descript(x)   (x)->Descript = NULL

#define SW_Set_Flag(w,F)    ( ( SW_WIDGET *)(w) )->Flag |= (F)
#define SW_Clear_Flag(w,F)  ( ( SW_WIDGET *)(w) )->Flag &= ~(F)
#define SW_Is_Flag_Set(w,F) ( ( ( SW_WIDGET *)(w) )->Flag & (F) )
#define SW_Is_Flag_Clear(w,F) ( !( ( ( SW_WIDGET *)(w) )->Flag & (F) ) )

#define SW_Notify_Move(x)          SW_Set_Flag( (x), SW_NOTIFY_MOVE )
#define SW_Notify_Drag_Only(x)     SW_Clear_Flag( (x), SW_NOTIFY_MOVE )

#define SW_Focus_Drag(x)           SW_Set_Flag( (x), SW_FOCUS_DRAG )
#define SW_Dont_Focus_Drag(x)      SW_Clear_Flag( (x), SW_FOCUS_DRAG )

#define SW_Add_Callback(w,c)       { if ( (w) != NULL ) (w)->Methods.Release_Method = (c); }
#define SW_Add_Drag_Callback(s,M)  { if ( (s) != NULL ) (s)->Methods.Drag_Method = (M); SW_Notify_Drag_Only( (s) ); }
#define SW_Add_Move_Callback(s,M)  { if ( (s) != NULL ) (s)->Methods.Drag_Method = (M); SW_Notify_Move( (s) ); }
#define SW_Add_Click_Callback(s,M) { if ( (s) != NULL ) (s)->Methods.Click_Method = (M); }
#define SW_Add_Draw_Callback(s,M)  { if ( (s) != NULL ) (s)->Methods.Draw_Method = (M); }
#define SW_Add_Leave_Callback(s,M) { if ( (s) != NULL ) (s)->Methods.Leave_Method = (M); }
#define SW_Add_Key_Callback(s,M)   { if ( (s) != NULL ) (s)->Methods.Key_For_Widget_Method = (M); }

#define SW_Outline(x)            (x)->Draw_Type |=  SW_OUTLINE
#define SW_No_Outline(x)         (x)->Draw_Type &= ~SW_OUTLINE
#define SW_Highlight_Widget(x)   (x)->Draw_Type |=  SW_HIGHLIGHT
#define SW_No_Highlight(x)       (x)->Draw_Type &= ~SW_HIGHLIGHT
#define SW_Push_Widget(x)        (x)->Draw_Type |=  SW_PUSHED
#define SW_No_Push(x)            (x)->Draw_Type &= ~SW_PUSHED
#define SW_Normal_Aspect(x)      (x)->Draw_Type  =  SW_NOTHING
#define SW_With_Border(x)        (x)->Draw_Type |=  SW_DRAW_BORDER
#define SW_No_Border(x)          (x)->Draw_Type &= ~SW_DRAW_BORDER

#define SW_No_Wait_For_Refresh(x)   (x)->Draw_Type &= ~SW_WAIT_FOR_REFRESH
#define SW_Wait_For_Refresh(x)   (x)->Draw_Type |= SW_WAIT_FOR_REFRESH

#define SW_Auto_Fill_On(x)   (x)->Draw_Type |= SW_AUTO_FILL
#define SW_Auto_Fill_Off(x)   (x)->Draw_Type &= ~SW_AUTO_FILL

#endif	/* _WIDGET_H_ */

