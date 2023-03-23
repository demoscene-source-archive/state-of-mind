/*
 * Part14
 *
 * Skal98 (skal.planet-d.net)
 ***************************************/

#include "demo.h"

/********************************************************************/

void Loop_14_Anim( )
{
   Scratch_And_Flash_II( Get_Beat( ) );
   Paste_JPEG_Anim( );
   Get_Next_Scratch( 0x08 );
   Mixer.Mix_Sat8_8( &VB(VSCREEN), &VB(JANIM), &VB(SCRATCH) );
}

#if 0       // USELESS now

void Loop_14_Anim_Destroy( )
{
   Loop_14_Anim( );
   Destroy_Screen( &VB(VSCREEN), Get_Beat( ) );
}

void Loop_14_Anim_Flash( )
{
   Anim_Cnt = (INT)( Global_x*2.0*Anim_Max_Cnt ) % Anim_Max_Cnt;
   Scratch_Next_Ramp_M( 0 );
   Paste_JPEG_Anim( );
   Get_Next_Scratch( 0x07 );
   Mixer.Mix_Sat8_8( &VB(VSCREEN), &VB(JANIM), &VB(SCRATCH) );
}

#endif

void Loop_14_Anim_Mask_Raw( )
{
   Scratch_And_Flash( Get_Beat( ) );
   Paste_JPEG_Anim( );
   Mixer.Mix_8_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(JANIM) );
}

void Loop_14_Anim_Mask( )
{
   Paste_JPEG_Anim( );
   Scratch_And_Flash( Get_Beat( ) );
   Mixer.Mix_8_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(JANIM) );
   Get_Next_Scratch( 0x08 );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
}

void Loop_14_Anim_Mask_Sat( )
{
   FLT x;

   Scratch_And_Flash( Get_Beat( ) );

   x = Global_x;
   if ( x<0.1f ) x = x*0.4f;
   else if ( x>0.9f ) x = (1.0f-x)*0.4f;
   else x = 1.0f;
   Saturate_DMask_Pal( &VB(VMASK), x*2.0f, Cur_Mask, 0xE0C0B2 );

   Paste_JPEG_Anim( );
   Mixer.Mix_8_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(JANIM) );
   Get_Next_Scratch( 0x08 );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
}

EXTERN void Loop_14_Anim_Mask_Move( )
{
   FLT x;

   x = Get_Beat( );
   Scratch_And_Flash( x );

   if ( (random()&0x1F)==0x05 ) JA_Parity = 5;
   Paste_JPEG_Anim_Move( );

   Saturate_DMask_Pal( &VB(VMASK), x, Cur_Mask, 0xC8C8C8 );
   Mixer.Mix_8_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(JANIM) );
   Get_Next_Scratch( 0x08 );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
}

EXTERN void Loop_14_Anim_Mask_Move_Sprites( )
{
   FLT x;

   x = Get_Beat( );
   Scratch_And_Flash( x );

   Paste_JPEG_Anim_Sprites(  );   // JA_Parity => selects source for sprites

   Saturate_DMask_Pal( &VB(VMASK), x, Cur_Mask, 0xC8C8C8 );
   Mixer.Mix_8_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(JANIM) );

   Get_Next_Scratch( 0x08 );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
   Loop_Dsp_I( );   
}

EXTERN void Loop_14_Anim_Mask_Move_II( )
{
   FLT x;

   x = Get_Beat( );
   Scratch_And_Flash( x );

   if ( (random()&0x1f)==0x0A ) JA_Parity = 16;
   Paste_JPEG_Anim_Move( );

   if ( JA_Parity>5 )
   {
      UINT Col;
      x = ( JA_Parity-6 ) / ( 15.0f-6.0f );
      Col  = (UINT)( 0x80*(1.0-x)+0xE0*x )<<16;
      Col |= (UINT)( 0xA0*(1.0-x)+0xE0*x )<<8;
      Col |= (UINT)( 0xC2*(1.0-x)+0xF0*x );
      Saturate_DMask_Pal( &VB(VMASK), 0.3f, Cur_Mask, Col );
      Paste_DMask_Move_II( &VB(VSCREEN), &VB(VMASK), &VB(JANIM) );
   }
   else 
   {
      PIXEL Pal[256*4];
      CMap_To_16bits( (void*)Pal, Cur_Mask->Pal, Cur_Mask->Nb_Col, 0x777 );
      DMask_Setup_Pal( (UINT*)VB(VMASK).CMap, Cur_Mask, (PIXEL*)Pal );
      Mixer.Mix_8_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(JANIM) );
   }
   Get_Next_Scratch( 0x08 );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
}

EXTERN void Loop_14_Blur_I( )
{
   Get_Beat( );
   if ( (random()&0x0f)==0x05 ) JA_Parity = 5;
   Paste_JPEG_Anim_Move( );

   if ( Tick_x>0.3f ) Loop_Blur_II( (1.0f-Tick_x)/0.7f );
   else Loop_Blur_II( Tick_x/0.3f );
}

EXTERN void Loop_14_Powered( )
{
   Loop_14_Blur_I( );
   Scroll_1( );
   UScroll = (BYTE)(Global_x*7.0*256.0);
}

EXTERN void Loop_14_No_Way( )
{
   Loop_14_Anim_Mask_Move_II( );
   Scroll_3( 4 );
   UScroll = (BYTE)(Global_x*2.0*256.0);
}

EXTERN void Loop_14_No_Way_II( )
{
   Loop_14_Anim( );
   Scroll_3( (INT)((1.0-Tick_x)*4) );
   UScroll = (BYTE)(Global_x*2.0*256.0);
}

EXTERN void Loop_14_Ready( )
{
   static INT Cnt = 0;
   Loop_14_Anim_Mask_Move_II( );
   if ( !Cnt )
   {
      if ( (random()&0x3F)==0x01 ) Cnt = random()&0x1f;
   }
   if ( Cnt )
   {
      Destroy_Screen( &VB(VSCREEN), Get_Beat( ) );
      Cnt--;
   }
//   if ( Tick_x>0.6 ) Loop_10_Sprites( (Tick_x-0.6f) / (1.0f-0.6f) );      
}

EXTERN void Loop_14_End( )
{
   Loop_14_Anim_Mask( );
}

/********************************************************************/
/********************************************************************/

