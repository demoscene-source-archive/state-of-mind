/*
 * Blur and scrolls
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

EXTERN BYTE UScroll;

EXTERN void Init_D2( INT Param )
{
   if ( Param==0 )
   {
      SELECT_CMAP( 1 );
      Check_CMap_VBuffer( &VB(1), 256 );

      Init_Caustics( 1 );      
      Time_Scale = 2600.0;
   }
   else if ( Param==1 )
   {
//      Drv_Build_Ramp_16( VB(2).CMap, 0, 8, 0,0,0, 255,240,200, 0x2565 );
   }
   Drv_Build_Ramp_16( VB(1).CMap, 0, 256, 0,0,0, 180, 180, 180, 0x777 );
}

#ifdef UNIX

EXTERN void Do_Blur_Y_Loop( BYTE *Dst, INT Dst_BpS,
   BYTE *Src, INT W, INT H, INT Src_BpS,
   USHORT *Sat1, INT *Int1  )
{
   for( ; H>0; H-- )
   {
      INT i;
      for( i=-W; i<0; ++i )
         Dst[i] = Sat1[ Dst[i]+Int1[Src[i]] ];
      Src += Src_BpS;
      Dst += Dst_BpS;
   }
}

#endif

EXTERN void Do_Blur_Y( VBUFFER *Out )
{
   INT H, W, Off, Off_x;
   FLT x,y;
   INT SCnt;
   BYTE *Dst, *Src;
   USHORT Sat1[350+2*CLAMP_OFF];
   INT Int1[256];

   SCnt = (INT)( Global_x * 256 );
   if ( Global_x<0.3 )
   {
      Off = (INT)( (256.0/.3)*Global_x );
      if ( Off>255 ) Off = 255;
   }
   else Off = 255;   
   Init_Saturation_Table3( Sat1, 0, Off, 1.0, 1, 350 );

   W = Out->W; H = Out->H;
   if ( SCnt&0x08 )
   {
      Src = (BYTE*)Restb1->Bits;
      Off = 0;
      Off_x = (TRandom(1)&0x0F ) - 7;
      if ( !(TRandom(0)&0x1F) ) Off_x += (TRandom(2)&0x1F)-16;
      y = Global_x*255.0f;
   }
   else
   {
      Src = (BYTE*)Restb2->Bits;
      x = (TRandom(0)&0xFF)/128.0f - 1.0f;
      Off = (INT)( x * 24.0f );
      Off_x = ( TRandom(1)&0x07 ) - 3;
      y = 256.0f*(FLT)exp(-x*x*3.5f );
   }
   Init_Saturation_Table4( Int1, 0, (INT)(y), 1.0, 1, 256 );

   Dst = (BYTE *)Out->Bits + Out->W;
   Src += W;
   if ( Off>0 )
   {
      H -= Off;
      Dst += Off*Out->BpS;
   }
   else if ( Off<0 )
   {
      H += Off;
      Src -= Off*The_W;
   }
   if ( Off_x>0 )
   {
      W -= Off_x;
      Dst += Off_x;
   }
   else if ( Off_x<0 )
   {
      W += Off_x;
      Src += Off_x;   // ?!
   }

   Do_Blur_Y_Loop( Dst, Out->BpS, Src, W, H, The_W, Sat1+CLAMP_OFF, Int1 );
}

EXTERN void Loop_D2( )
{
   FLT x;
   INT R, G, B;

   x = Get_Beat( );
   Scratch_And_Flash( x );
   Loop_Caustics_II( );
   Do_Blur_Y( &VB(1) );

   x = (FLT)fmod( 2.0*x, 1.0 ); // 4.0??
   R = (INT)(150.0 + 65.0+sin(x*M_PI) );
   G = (INT)(250.0 - 85.0+sin(x*M_PI) );
   B = (INT)(180.0 - 65.0+sin(x*M_PI) );
   Drv_Build_Ramp_16( (UINT*)VB(3).CMap, 0, 256, 0, 0, 0, R, G, B, 0x777 );

   Mixer.Mix_Sat8_8( &VB(VSCREEN), &VB(1), &VB(3) );
   Scroll_1( );
   UScroll = (BYTE)(25.0f*Global_x*256.0);
}

EXTERN void Loop_D2_Destroy( )
{
   Loop_D2( );
   Destroy_Screen( &VB(VSCREEN), Tick_x );
}

/********************************************************************/

EXTERN void Init_Scroll( INT Param )
{
   JA_Parity = 0;
   SELECT_565_V(4);
   if ( Param==0 )
   {
      Extract_Virtual_VBuffer( &VB(4), &VB(VSCREEN), 0, 145, 320, 49 );
      Check_CMap_VBuffer( &VB(4), 256 );
      Drv_Build_Ramp_16( VB(4).CMap, 0, 256, 0,0,0, 200,220,255, 0x777 );
   }
   else if ( Param==1 )
   {
      Extract_Virtual_VBuffer( &VB(4), &VB(VSCREEN), 0, 0, 68, 200 );
      Check_CMap_VBuffer( &VB(4), 256 );
      Drv_Build_Ramp_16( VB(4).CMap, 0, 256, 0,0,0, 200,220,255, 0x777 );
   }
   else if ( Param==2 ) // blobz
   {
   }
   else if ( Param==3 )    // (powered+imagine)
   {
      Extract_Virtual_VBuffer( &VB(4), &VB(VSCREEN), 0, 5, 320, 49 );
      Check_CMap_VBuffer( &VB(4), 256 );
      Drv_Build_Ramp_16( VB(4).CMap, 0, 256, 0,0,0, 200,220,255, 0x777 );
   }
}

EXTERN void Scroll_1( )
{
   if ( (TRandom(3)&0x07)==0x03 ) JA_Parity = 3+(TRandom(1)&0x03);
   if ( JA_Parity )
   {
      Paste_256( &VB(4), (PIXEL*)Cnt5->Bits+JA_Parity*Cnt5->Width, UScroll, (UINT*)VB(4).CMap );
      Scratch_And_Flash_II( Get_Beat( ) );
      Get_Next_Scratch( 0x08 );
      Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
      JA_Parity--;
   }
   else Paste_256( &VB(4), (PIXEL*)Cnt4->Bits+7*Cnt4->Width, UScroll, (UINT*)VB(4).CMap );
}

EXTERN void Scroll_2( )
{
   static BITMAP_16 *Src = NULL;

   if ( (!JA_Parity) && (TRandom(2)&0x07)==0x03 )
   {
      JA_Parity = 3+(TRandom(0)&0x07);
      if ( TRandom(1)&0x08 ) Src = Count2;
      else Src = Count3;
   }
   if ( JA_Parity )
   {
      Paste_256_Y( &VB(4), (BYTE*)Src->Bits, UScroll, (UINT*)VB(4).CMap, Src->Width );
      JA_Parity--;
   }
   else Paste_256_Y( &VB(4), (BYTE*)Count1->Bits, UScroll, (UINT*)VB(4).CMap, Count1->Width );
}

#define BASE   195
EXTERN void Scroll_3( INT n )
{
   static BITMAP_16 *Src = NULL;
   UINT CMap[256];

   if ( (!JA_Parity) && (TRandom(2)&0x07)==0x03 )
      JA_Parity = 3+(TRandom(1)&0x07);
   if ( JA_Parity ) { Src = Cnt5; JA_Parity--; }
   else Src = Cnt4;

   Drv_Build_Ramp_16( CMap, 0, 256, 0,0,0, 200,220,255, 0x777 );

   if ( n>0 )
   {
      Extract_Virtual_VBuffer( &VB(4), &VB(VSCREEN), 0, BASE-20, 320, 20 );
      Paste_256( &VB(4), (BYTE*)Src->Bits+212*Src->Width, UScroll, CMap );
   }
   if ( n>1 )
   {
      Extract_Virtual_VBuffer( &VB(4), &VB(VSCREEN), 0, BASE-30-5, 320, 30 );
      Paste_256( &VB(4), (BYTE*)Src->Bits+182*Src->Width, (UScroll*2)&0xFF, CMap );
   }
   if ( n>2 )
   {
      Extract_Virtual_VBuffer( &VB(4), &VB(VSCREEN), 0, BASE-40-10, 320, 40 );
      Paste_256( &VB(4), (BYTE*)Src->Bits+138*Src->Width, (UScroll*3)&0xFF, CMap );
   }
   if ( n>3 )
   {
      Extract_Virtual_VBuffer( &VB(4), &VB(VSCREEN), 0, BASE-70-15, 320, 68 );
      Paste_256( &VB(4), (BYTE*)Src->Bits+68*Src->Width, (UScroll*4)&0xFF, CMap );
   }
}

EXTERN void Close_Scroll( )
{
   DISPOSE( 4 );
}

/********************************************************************/

EXTERN void Close_D2( )
{ 
   DISPOSE( 1 );
   DISPOSE( 3 );
   if ( Cur_Anim!=NULL ) Destroy_16b( Cur_Anim );
   Cur_Anim = NULL;
   Destroy_16b( Restb1 );
   Destroy_16b( Restb2 );
}

/********************************************************************/

