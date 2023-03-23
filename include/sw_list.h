/*********************************************
 *                                           *
 *	   sw_list.h                              *
 *                                           *
 * Skal widgets v0.0 (c)1997                 *
 * Pascal.Massimino@ens.fr                   *
 *********************************************/

#ifndef _SW_LIST_H_
#define _SW_LIST_H_

#include "sw_main.h"

/***************************************************************/

extern void SW_Link_With_Parent( SW_WIDGET *Parent, SW_WIDGET *W );
extern void SW_Link_With_Widget( SW_WIDGET *, SW_WIDGET * );
extern VOID *SW_Add_To_List( SW_WIDGET *, SW_WIDGET * );
extern void SW_Remove_From_List( SW_WIDGET *, SW_WIDGET * );
extern void SW_Remove_Child_From_Group( SW_WIDGET *, SW_GROUP * );
extern void SW_Add_Child_To_Group( SW_WIDGET *, SW_GROUP * );
extern UINT SW_Realize_List( SW_WIDGET * );
extern void SW_UnMap_List( SW_WIDGET * );
extern void SW_Draw_List( SW_WIDGET * );
extern void SW_Copy_List( SW_WIDGET * );
extern void SW_Destroy_List( SW_WIDGET * );
extern void SW_Destroy_List_Element( SW_WIDGET * );
extern void SW_Set_Flag_List( SW_WIDGET *, UINT );
extern void SW_Clear_Flag_List( SW_WIDGET *, UINT );
extern void SW_Disable_List( SW_WIDGET * );
extern void SW_Enable_List( SW_WIDGET * );

#define SW_FORTH      0
#define SW_BACK       1
#define SW_RECURSE_UP 2

extern SW_WIDGET *SW_Search_Relative_Widget( SW_WIDGET *, UINT, UINT );

/***************************************************************/

#endif	/* _SW_LIST_H_ */

