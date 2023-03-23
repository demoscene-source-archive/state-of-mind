/*
 * Scratch funcs
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

static BITMAP *(BOldz[3]) = { NULL, NULL, NULL };
static BYTE Adv0 = 0x00;
static INT Cycle[16] = 
{
   0,4,7,8,       10,11,12,15,
   14,13,12,10,   7,5,3,2
};
static INT C_Off = 0;

/********************************************************************/

EXTERN void Init_Scratch( )
{
   BOldz[0] = Load_GIF( "old4.gif" );  // <= Size: 320x201
   BOldz[1] = Load_GIF( "old5.gif" );
   if ( !Low_Mem )
      BOldz[2] = Load_GIF( "old6.gif" );
   else BOldz[2] =  BOldz[0];

   Select_VBuffer( &VB(SCRATCH), VCMAP, 
      The_W, The_H, The_W, VBUF_VIRTUAL );
   Check_CMap_VBuffer( &VB(SCRATCH), 16 ); // 16 for safety
   Drv_Build_Ramp_16( VB(SCRATCH).CMap, 0, 8, 0,0,0, 255,240,200, 0x777 );
}

EXTERN void Scratch_Next_Ramp( INT Off )
{
   INT j;
   j = Cycle[ C_Off ];
   Drv_Build_Ramp_16( VB(SCRATCH).CMap, 0, 8, 
      Off,Off,Off, (Off+55)*j/16,(Off+45)*j/16,(Off+40)*j/16, 0x777 );
   C_Off += TRandom(1)&0x01;
   if ( C_Off>=16 ) C_Off = 0;
}

#if 0       // USELESS now

EXTERN void Scratch_Next_Ramp_M( INT Off )
{
   INT j;
   j = Cycle[ C_Off ];
   Drv_Build_Ramp_16( VB(SCRATCH).CMap, 0, 8, 
      (Off+75)*j/16,(Off+45)*j/16,(Off+40)*j/16, Off,Off,Off, 0x777 );
   C_Off += TRandom(0)&0x01;
   if ( C_Off>=16 ) C_Off = 0;
}

#endif

EXTERN void Scratch_And_Flash( FLT x )
{
   C_Off = ((INT)(x*4.0f))& 0x0F;
   if ( x>0.8 ) Scratch_Next_Ramp( (INT)( 240.0*(x-0.8)/0.2) );
   else Scratch_Next_Ramp( 0 );
}

EXTERN void Scratch_And_Flash_II( FLT x )
{
   C_Off = ((INT)(x*4.0f))& 0x0F;
   x *= 2.0;
   if ( x<1.0 && x>0.8 )
      Scratch_Next_Ramp( (INT)( 140.0*(x-0.8)/0.2) );
   else if ( x>1.8 )
      Scratch_Next_Ramp( (INT)( 240.0*(x-1.8)/0.2) );
   else Scratch_Next_Ramp( 0 );
}

EXTERN void Get_Next_Scratch( INT Adv )
{
   static INT Off = 0;
   Adv0 += Adv + ( TRandom(2)&0x03 );
   if ( Adv0>=(0x3<<6) ) Adv0 = 0;
   VB(SCRATCH).Bits = (void*)( BOldz[Adv0>>6]->Bits + Off );
   if ( (TRandom(0)&0x07)==0x00 )
   {
      Off += (TRandom(1)&0x7f) - 64;
      if ( Off>=The_W ) Off -= The_W;
      else if ( Off<0 ) Off += The_W;
   }
   Off += (TRandom(2)&0x07) - 3;
   if ( Off>=320 ) Off = 0;
   else if ( Off<0 ) Off = 319;
}

/********************************************************************/

#if 0

EXTERN void Do_Old_Scratch( VBUFFER *Out )
{
   Scratch_Next_Ramp( 0 );
   Get_Next_Scratch( 0x07 );
   Mixer.Mix_Sat16_8( Out, Out, &VB(SCRATCH) );
}

EXTERN void Do_Old_Scratch_M( VBUFFER *Out )
{
   Scratch_Next_Ramp_M( 0 );
   Get_Next_Scratch( 0x09 );
   Mixer.Mix_Sat16_8_M( Out, Out, &VB(SCRATCH) );
}

#endif

/********************************************************************/
