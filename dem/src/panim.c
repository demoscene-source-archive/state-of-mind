/*
 * Anim setup
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

EXTERN FLT Time_Scale = 1.0, Time_Off = 0.0;

/********************************************************************/

EXTERN BITMAP_16 *Anim12; // march right
EXTERN BITMAP_16 *Anim32; // march left
EXTERN BITMAP_16 *Anim93; // avion
EXTERN BITMAP_16 *Anim72; // tubes
EXTERN BITMAP_16 *Anim82; // famille

EXTERN BITMAP_16 *Cur_Anim = NULL;
EXTERN BYTE *Cur_Anim_Bits = NULL;
EXTERN INT Anim_Cnt = 0, Anim_Max_Cnt = 0;
EXTERN INT Cur_Anim_Size = 0, Cur_Anim_Max_Size = 0;
EXTERN INT Cur_Anim_Width = 0, Cur_Anim_Height = 0;

EXTERN DMASK *Mask012=NULL; // powered by
EXTERN DMASK *Mask022=NULL; // advisory
EXTERN DMASK *MaskDie=NULL;
EXTERN DMASK *MaskWork=NULL;
EXTERN DMASK *MaskConsume=NULL;
EXTERN DMASK *MaskBuy=NULL;
EXTERN DMASK *Mask052=NULL; // Welcome to
EXTERN DMASK *Mask072=NULL; // a World of technology
EXTERN DMASK *Mask082=NULL; // join us
EXTERN DMASK *Mask112=NULL; // be different.Improve
EXTERN DMASK *Mask122=NULL; // [logo]
EXTERN DMASK *Mask132=NULL; // kiss the future
EXTERN DMASK *Mask162=NULL; // No way

EXTERN DMASK *MaskGreets1=NULL; // Greet #1
EXTERN DMASK *MaskGreets2=NULL; // Greet #2
EXTERN DMASK *MaskGreets3=NULL; // Greet #3

EXTERN DMASK *MRest=NULL;
EXTERN DMASK *Mask212=NULL; // ready to take off?
EXTERN DMASK *Mask222=NULL; // bomb

EXTERN DMASK *Head_For=NULL, *Modern=NULL;
EXTERN DMASK *Upgrade=NULL, *Resist=NULL;

EXTERN BITMAP_16 *End_Pic;

EXTERN BITMAP_16 *Cnt4, *Cnt5;
EXTERN BITMAP_16 *Count1, *Count2, *Count3;

EXTERN DMASK *Cur_Mask = NULL;

/********************************************************************/
/********************************************************************/

 EXTERN void Pre_Cmp_Anim( )
{
   INT i;

   Init_JPEG_Anim( );      // setup VB(JANIM) and Anim_Ramp[]

   Anim12 = Load_JPEG( "anim12.jpg", &i );
   if ( !Low_Mem )
   {
      Anim32 = Load_JPEG( "anim32.jpg", &i );
      Anim93 = Load_JPEG( "anim93.jpg", &i );
      Anim72 = Load_JPEG( "anim72.jpg", &i );
      Anim82 = Load_JPEG( "anim82.jpg", &i );
   }
   else Anim32 = Anim93 = Anim72 = Anim82 = Anim12;
}

EXTERN void Pre_Cmp_Mask( )
{
   INT i;

   SELECT_565_V( VMASK );   // sets up VB(VMASK)...

   Mask012 = Load_DMask( "layer012m.msk" );
   Mask022 = Load_DMask( "layer022m.msk" );
   MaskWork = Load_DMask( "work.msk" );
   MaskBuy = Load_DMask( "buy.msk" );
   MaskConsume = Load_DMask( "cons.msk" );
   MaskDie = Load_DMask( "die.msk" );
   Mask052 = Load_DMask( "welcome.msk" );
   Mask072 = Load_DMask( "techno.msk" );
   Mask082 = Load_DMask( "join.msk" );
   Mask112 = Load_DMask( "layer112m.msk" );
   Mask122 = Load_DMask( "layer122m.msk" );
   Mask132 = Load_DMask( "layer132m.msk" );
   Mask162 = Load_DMask( "layer162m.msk" );

   MaskGreets1 = Load_DMask( "gr1.msk" );
   if ( !Low_Mem )
   {
      MaskGreets2 = Load_DMask( "gr2.msk" );
      MaskGreets3 = Load_DMask( "gr3.msk" );
   }
   else MaskGreets2 = MaskGreets3 = MaskGreets1;

   Head_For = Load_DMask( "tomorrow.msk" );
   Resist   = Load_DMask( "resist.msk" );
   Modern   = Load_DMask( "modern.msk" );
   Upgrade  = Load_DMask( "upgrade.msk" );

   Mask212 = Load_DMask( "layer212m.msk" );  // ready?
   MRest = Load_DMask( "rest.msk" );  // ready?
   Mask222 = Load_DMask( "layer222m.msk" );  // Bomb.

   End_Pic = Load_JPEG_565( "endb01.jpg", &i );

   Cnt4 = Load_JPEG( "cnt4.jpg", NULL );
   Count1 = Load_JPEG( "count1.jpg", NULL );
   if ( !Low_Mem )
   {
      Cnt5 = Load_JPEG( "cnt5.jpg", NULL );
      Count2 = Load_JPEG( "count2.jpg", NULL );
      Count3 = Load_JPEG( "count3.jpg", NULL );
   }
   else { Cnt5 = Cnt4; Count2 = Count3 = Count1; }
}

/********************************************************************/
