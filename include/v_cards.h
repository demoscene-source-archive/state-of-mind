/***********************************************
 *          Graphics Drivers                   *
 * Skal 96                                     *
 ***********************************************/

#ifndef _V_CARDS_H_
#define _V_CARDS_H_

/********************************************************/
/********************************************************/

typedef struct CARD_METHODS CARD_METHODS;
struct CARD_METHODS {
#pragma pack(1)

   STRING Name;
   REFRESH_MTHD	Refresh;
   INT (*Detect)( VBE_INFO * );
   void (*Print_Info)( MEM_ZONE_DRIVER * );
   MEM_ZONE_DRIVER *(*Init_VGA)( MEM_ZONE_DRIVER * );
   MEM_ZONE_DRIVER *(*Init_LFB)( MEM_ZONE_DRIVER * );
   MEM_ZONE_DRIVER *(*Init_BNK_SW)( MEM_ZONE_DRIVER * );
};

/********************************************************/

#define _VBE_GENERIC_  (&_VBE_Methods_)
extern CARD_METHODS _VBE_Methods_;

extern CARD_METHODS *_Registered_Card_Drivers_[];

extern _G_DRIVER_ __G_VBE_DRIVER_;
#define _G_VBE_DRIVER_ (&__G_VBE_DRIVER_)

/********************************************************/
/********************************************************/

#endif   // _V_CARDS_H_

