/***********************************************
 *                                             *
 *    sw_main.h                                *
 *  #define, type and structs for widget lib   *
 *                                             *
 * Skal widgets v0.0 (c)1997                   *
 * Pascal.Massimino@ens.fr                     *
 ***********************************************/
   
#ifndef _SW_MAIN_H_
#define _SW_MAIN_H_

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "mem_map.h"
#include "driver.h"

/************************************************************/

   /* Drawing types */

#define SW_NOTHING          0
#define SW_PUSHED           1
#define SW_DRAW_BORDER      2
#define SW_HIGHLIGHT        4
#define SW_OUTLINE          8
#define SW_DRAW_ICON        16
#define SW_WAIT_FOR_REFRESH 32
#define SW_AUTO_FILL        64

   /* Printing methods */

#define SW_AS_IT              0
#define SW_CENTER_X           1
#define SW_CENTER_Y           2
#define SW_CENTER             SW_CENTER_X | SW_CENTER_Y
#define SW_RIGHT_FLUSH        4
#define SW_TILED              8
#define SW_WITH_RULER         16

   /* Flags for widgets and others... */

#define SW_NO_FLAG            0
#define SW_IS_ACTIV           1
#define SW_HAS_CHILD          2
#define SW_NO_RECURSE         4
#define SW_CALL_BACK_OK       8
#define SW_TIMER_RUNNING      16
#define SW_NOTIFY_MOVE        32
#define SW_FOCUS_DRAG         64

/************************************************************/

#define SW_DEFAULT_WIDTH    800
#define SW_DEFAULT_HEIGHT   600

#define SW_MAX_ICON         33 
#define SW_ICON_WIDTH       32
#define SW_ICON_HEIGHT      32
#define SW_MAX_POP_UP       5     /* Should be enough */

#define SW_MAX_FONT  3
#define SW_MAX_CHAR  94

#define SW_DEBUG  1

/************************************************************/

typedef enum {   /* types for widgets */

   SW_VOID_TYPE,
   SW_BUTTON_TYPE,
   SW_MESSAGE_TYPE,
   SW_GROUP_TYPE,
   SW_TOP_TYPE,
   SW_GRAPHIC_TYPE,
   SW_SLIDER_TYPE,
   SW_DATA_TYPE,
   SW_MENU_TYPE,
   SW_ICON_TYPE,
   SW_WIDGET_TYPE,
   SW_MAIN_TYPE,
   SW_PIXEL_TYPE,
   SW_PTR_TYPE,
   SW_INT_TYPE,
   SW_U_INT_TYPE,
   SW_DBL_TYPE,
   SW_STRING_TYPE,
   SW_PATH_TYPE,
   SW_TIMER_TYPE,
   SW_SELECTOR_TYPE,
   SW_TOGGLE_TYPE,
   SW_TOP_MENU_TYPE,
   SW_DECLARATION_TYPE,
   SW_CHOICE_TYPE

} SW_TYPE;

/************************************************************/
/*                    Structs                               */
/************************************************************/

typedef struct SW_TOP    SW_TOP;
typedef struct SW_WIDGET SW_WIDGET;
typedef struct SW_TOGGLE SW_TOGGLE;

typedef struct {
#pragma pack(1)

   UINT    Type;
   BITMAP *Icon;
   UINT    Size1, Size2;
   UINT    Shadow, Alpha;

} SW_BORDER;

typedef struct {
#pragma pack(1)

   UINT        Draw_Type;
   PIXEL       Background;
   PIXEL       Outline;
   BITMAP     *Tile;
   SW_BORDER  *Border;

} SW_ASPECT;

/*************************************************************/

typedef struct {
#pragma pack(1)
   UINT    Offset;
   UINT    Height;
   INT     Widths[ SW_MAX_CHAR ];
   PIXEL  *Bits[ SW_MAX_CHAR ];
   BITMAP *Bitmap;
   PIXEL   Col_Convert[ 16 ];    /* 16 colors at max for font */
} SW_FONT;

/************************ Methods ****************************/

typedef UINT (*SW_MOUSE_METHOD)( SW_WIDGET *, INT, INT, DRV_EVENT * );
typedef UINT (*SW_KEY_METHOD)( SW_WIDGET *, SW_TOP * );
typedef void (*SW_ENTER_METHOD)( SW_WIDGET *, INT, INT, DRV_EVENT * );
typedef void (*SW_LEAVE_METHOD)( SW_WIDGET *, INT, INT, DRV_EVENT * );
typedef void (*SW_WIDGET_METHOD)( SW_WIDGET * );
typedef void (*SW_TOP_METHOD)( SW_TOP * );
typedef UINT (*SW_REALIZE_METHOD)( SW_WIDGET * );

typedef struct {
#pragma pack(1)
   SW_MOUSE_METHOD     Click_Method;
   SW_MOUSE_METHOD     Release_Method;
   SW_MOUSE_METHOD     Drag_Method;
   SW_ENTER_METHOD     Enter_Method;
   SW_LEAVE_METHOD     Leave_Method;
   SW_KEY_METHOD       Key_For_Widget_Method;
   SW_WIDGET_METHOD    Draw_Method;
   SW_REALIZE_METHOD   Realize_Method;
   SW_WIDGET_METHOD    Destroy_Method;
   SW_WIDGET_METHOD    Copy_Method;
   SW_MOUSE_METHOD     Action;
} SW_METHODS;

#if 0
#define SW_CLICK(a,b) ( (*((a)->Methods.Click_Method))( (SW_WIDGET *)(a), (b)->Event.x-(a)->Zone->Xo, (b)->Event.y-(a)->Zone->Yo, &(b)->Event ) )
#define SW_RELEASE(a,b) ( (*((a)->Methods.Release_Method))( (SW_WIDGET *)(a), (b)->Event.x-(a)->Zone->Xo, (b)->Event.y-(a)->Zone->Yo, &(b)->Event ) )
#define SW_DRAG(a,b) ( (*((a)->Methods.Drag_Method))( (SW_WIDGET *)(a), (b)->Event.x-(a)->Zone->Xo, (b)->Event.y-(a)->Zone->Yo, &(b)->Event ) )
#define SW_ENTER(a,b) ( (*((a)->Methods.Enter_Method))( (SW_WIDGET *)(a), (b)->Event.x-(a)->Zone->Xo, (b)->Event.y-(a)->Zone->Yo, &(b)->Event ) )
#define SW_LEAVE(a,b) ( (*((a)->Methods.Leave_Method))( (SW_WIDGET *)(a), (b)->Event.x-(a)->Zone->Xo, (b)->Event.y-(a)->Zone->Yo, &(b)->Event ) )
#define SW_KEY(a,b) ( (*((a)->Methods.Key_For_Widget_Method))( (SW_WIDGET *)(a), (b) ) )
#else
#define SW_CLICK(a,b) ( (*((a)->Methods.Click_Method))( (SW_WIDGET *)(a), (b)->Event.x-(a)->Abs_x, (b)->Event.y-(a)->Abs_y, &(b)->Event ) )
#define SW_RELEASE(a,b) ( (*((a)->Methods.Release_Method))( (SW_WIDGET *)(a), (b)->Event.x-(a)->Abs_x, (b)->Event.y-(a)->Abs_y, &(b)->Event ) )
#define SW_DRAG(a,b) ( (*((a)->Methods.Drag_Method))( (SW_WIDGET *)(a), (b)->Event.x-(a)->Abs_x, (b)->Event.y-(a)->Abs_y, &(b)->Event ) )
#define SW_ENTER(a,b) ( (*((a)->Methods.Enter_Method))( (SW_WIDGET *)(a), (b)->Event.x-(a)->Abs_x, (b)->Event.y-(a)->Abs_y, &(b)->Event ) )
#define SW_LEAVE(a,b) ( (*((a)->Methods.Leave_Method))( (SW_WIDGET *)(a), (b)->Event.x-(a)->Abs_x, (b)->Event.y-(a)->Abs_y, &(b)->Event ) )
#define SW_KEY(a,b) ( (*((a)->Methods.Key_For_Widget_Method))( (SW_WIDGET *)(a), (b) ) )
#endif

#define SW_DESTROY(a) ( (*((a)->Methods.Destroy_Method))( (SW_WIDGET *)(a) ) )

#define SW_REALIZE(x) ( (*((x)->Methods.Realize_Method))( (x) ) )
#define SW_DRAW(x) if ( ((x)!=NULL) && (SW_Is_Flag_Set((x),SW_IS_ACTIV)) ) \
   (*((x)->Methods.Draw_Method))( (x) )
#define SW_COPY(x) ( (*((a)->Methods.Copy_Method))( (x) ) )

#include "sw_mouse.h"

/******************* Widgets ******************************/

#define SW_WIDGET_FIELDS                          \
                                                  \
   SW_TYPE      Type;                             \
   SW_WIDGET   *Next, *Prev;                      \
   SW_WIDGET   *Parent;                           \
   SW_TOP      *Top;                              \
   UINT         Flag;                             \
   MEM_ZONE    *Zone;                             \
   SW_ASPECT   *Aspect;                           \
   STRING       Descript;                         \
   INT          Abs_x, Abs_y;                     \
                                                  \
         /* Methods */                            \
                                                  \
   SW_METHODS  Methods;                           \
   BYTE        Accelerator;


/* Struct for casts */

struct SW_WIDGET {
#pragma pack(1)
   SW_WIDGET_FIELDS
};

/******************** Inheritances *************************/

struct SW_TOP {
#pragma pack(1)
   SW_WIDGET_FIELDS

   SW_WIDGET *Widgets;    /* Always first. Same as in SW_GROUP */

         /* Fonts & Icons */

   INT     Nb_Fonts;
   SW_FONT Fonts[ SW_MAX_FONT ];

   UINT    Nb_Icons;
   BITMAP *Icons[ SW_MAX_ICON ];

   SW_WIDGET    *Accelerators[26];

   SW_INTERFACE Interface;
   DRV_EVENT Event;
   DRV_EVENT Forced_Event;      

   SW_WIDGET *Prev_Widget;
   SW_WIDGET *Widget_Of_Interest; 

   INT        Nb_Pop_Up;
   SW_WIDGET *Pop_Up_Widgets[ SW_MAX_POP_UP ];

   UINT       Exit_Loop_Upon_Request;
};


typedef struct {
#pragma pack(1)
   SW_WIDGET_FIELDS

   PIXEL   Text_Color;   /* Always first... */
   UINT    State;
   VOID   *Data;
   STRING  Label;
   BITMAP *Icon;
   SW_MOUSE_METHOD Action;
} SW_BUTTON;

typedef struct {
#pragma pack(1)
   SW_WIDGET_FIELDS

   UINT   Backup;
   VOID  *Data;
} SW_GRAPHIC;


typedef struct {
#pragma pack(1)
   SW_WIDGET_FIELDS

   SW_WIDGET *Child;    /* Always first */
   SW_WIDGET *Cache;   /* For events */
   SW_WIDGET *Object_Of_Concern;
   UINT       Backup;
} SW_GROUP;


typedef struct {
#pragma pack(1)
   SW_WIDGET_FIELDS

   PIXEL Text_Color;
   UINT Row;
   UINT Line_Height;
} SW_MESSAGE;

struct SW_TOGGLE {
#pragma pack(1)
   SW_WIDGET_FIELDS

   PIXEL      Text_Color;
   UINT       State;
   SW_TOGGLE *Next_Toggle_W;
   STRING     Label1, Label2;
   BITMAP    *Icon1, *Icon2;
   VOID      *Data;
   VOID     (*Warn)( SW_TOGGLE *, VOID * );
};

typedef struct {
#pragma pack(1)
   SW_WIDGET_FIELDS

   PIXEL   Text_Color, Old_Background;
   UINT    Data_Type;
   STRING  Field, Edit_Field;
   SHORT   Max_Len, Cursor_Pos, String_Len;
   SHORT   Displayable_Len, First_Displayed;
   UINT    Old_Border_Type;
   void   *Additional_Data;
   double  Min, Max;

   void  *(*Get_Value)( SW_WIDGET *);
   void   (*Data_Changed)( SW_WIDGET *, void *, STRING );
} SW_DATA;

#if 0

typedef struct {
#pragma pack(1)
   SW_WIDGET_FIELDS

   PIXEL      Text_Color;
   UINT       Aspect;
   BITMAP    *Icon;
/*   SW_REGION  Cursor_Reg; */
   SW_BORDER  Cursor_Border;
   STRING     Label;
   UINT       Val_Pos;
   double     Min, Pos, Range;
   VOID      *Additional_Data;
   VOID     (*Data_Changed)( SW_WIDGET *, VOID * );
   STRING    *Strings;
} SW_SLIDER;

#endif

/************************************************************/

#include "sw.h"   /* Macros & global functions */
#include "sw_color.h"   /* For colors... */
#include "sw_reg.h"  /* often used...*/
#include "sw_font.h"
#include "sw_bord.h"
#include "sw_strng.h"

#include "sw_group.h"
#include "sw_butt.h"
#include "sw_gph.h"
#include "sw_toggl.h"
#include "sw_msg.h"
#include "sw_comm.h"
#include "sw_list.h"

/************************************************************/
/************************************************************/

#endif /* _SW_MAIN_H_ */
      
