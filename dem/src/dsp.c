/*
 * - Displacement -
 *
 * Skal 98
 ***************************************/

#include "demo.h"

EXTERN BYTE *Dsp_Radial_1;
EXTERN BYTE *Dsp_Radial_2;
EXTERN BYTE *Dsp_Radial = NULL;

#define OFFX 256
#define OFFY 256
EXTERN INT Dsp_Sat_X[The_W+2*OFFX];
EXTERN INT Dsp_Sat_Y[The_H+2*OFFY];

EXTERN INT Dsp_Parity = 0;

EXTERN BITMAP_16 *Blur012 = NULL;
EXTERN BITMAP_16 *Restb1 = NULL;
EXTERN BITMAP_16 *Restb2 = NULL;

/************************************************************/

EXTERN void Pre_Cmp_Dsp( INT cpu )
{
   INT i, j;
   BYTE *Dst_X, *Dst_Y;
   BYTE *Dst_X2, *Dst_Y2;
   Dsp_Radial_1 = New_Fatal_Object( 2*The_W*The_H, BYTE );
   Dsp_Radial_2 = New_Fatal_Object( 2*The_W*The_H, BYTE );
   Dst_X = Dsp_Radial_1;
   Dst_Y = Dst_X + The_W*The_H;
   Dst_X2 = Dsp_Radial_2;
   Dst_Y2 = Dst_X2 + The_W*The_H;

   for( j=0; j<The_H; ++j )
   {
      FLT y;
      y = 2.0f*j/The_H-1.0f;
      for( i=0; i<The_W; ++i )
      {
         FLT x, r, rr;
         x = 2.0f*i/The_W-1.0f;
         r = x*x*y*y + .01f;
         rr = 0.5f*( 1.0f + (FLT)cos( r*M_PI ) );
         if ( rr>0.0 ) rr = 127.0f*(FLT)pow( rr, .25 );
         else rr = 0.0;
         *Dst_X++ = (BYTE)floor( 127.0f - x*rr );
         *Dst_Y++ = (BYTE)floor( 127.0f - y*rr );

         rr = 0.5f*( 1.0f + (FLT)cos( r*M_PI*2.0 ) );
         if ( r>=1.0f ) r=0.0;
         else r = (FLT)pow( 1.0-r, .25 );
         *Dst_X2++ = (BYTE)floor( 127.0f + y*32.0f*rr - 96.0f*r*x );
         *Dst_Y2++ = (BYTE)floor( 127.0f - x*32.0f*rr - 96.0f*r*y );
      }
   }
   Restb1 = Load_JPEG( "restb1.jpg", &i );
   Restb2 = Load_JPEG( "restb2.jpg", &i );
   Blur012 = Load_JPEG( "blur012.jpg", &i );

   SELECT_565( VBLUR_I );
   SELECT_565( VBLUR_II );
}

/************************************************************/

EXTERN void Init_Blur_II( INT Param )
{
   SELECT_CMAP_V( 1 );
   Check_CMap_VBuffer( &VB(1), 256 );
   VB(1).Bits = Blur012->Bits;
   SELECT_CMAP( 2 );
   Check_CMap_VBuffer( &VB(2), 256 );
   SELECT_CMAP( 3 );
   Extract_DMask_Raw_Col( Cur_Mask, (BYTE*)VB(3).Bits );
}

#define T1  0.45f
#define T2  0.60f
EXTERN void Loop_Blur_II( FLT x )
{
   INT A;
   if ( x<T1 )
   {
      x /= T1;
      A = (INT)( x*255.0 );
      Drv_Build_Ramp_16( VB(1).CMap, 0, A, 0,0,0, 255,255,250, 0x777 );
      Drv_Build_Ramp_16( VB(1).CMap, A, 256, 255,255,250, 255,255,250, 0x777 );
      CMap_To_16bits_With_Fx( (void*)VB(VMASK).CMap, Cur_Mask->Pal, Cur_Mask->Nb_Col, 
         0x777, 0x000000, NULL, A^0xFF, 0 );
      Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 230,220,180, 0x777 );
      Mixer.Mix_Sat8_8( &VB(VSCREEN), &VB(1), &VB(JANIM) );
   }
   else if ( x<T2 )
   {
      INT i, B;
      BYTE *Src, *Src2, *Dst;
      BYTE Off_X[256], Off_Y[256];
      x = ( x-T1 ) / ( T2-T1 );
      A = (INT)( x*255.0 );
      Drv_Build_Ramp_16( VB(2).CMap, 0, A, 0,0,0, 0,0,0, 0x777 );
      Drv_Build_Ramp_16( VB(2).CMap, A, 240, 0,0,0,155,155,150, 0x777 );
      Drv_Build_Ramp_16( VB(2).CMap, 240, 256, 155,155,150, 255,255,250, 0x777 );
      B = A^0xFF;
      for( i=0; i<256; ++i )
      {
         Off_X[i] = (BYTE)( (A*i)>>8 );
         Off_Y[i] = (BYTE)( (B*i)>>8 );
      }
      Src  = (BYTE*)VB(3).Bits + The_W*The_H;
      Src2 = (BYTE*)VB(1).Bits + The_W*The_H;
      Dst  = (BYTE*)VB(2).Bits + The_W*The_H;
      for( i=-The_W*The_H; i<0; ++i )
         Dst[i] = (BYTE)( Off_Y[ Src2[i] ] + Off_X[ Src[i] ] );
      Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 230,220,180, 0x777 );
      Mixer.Mix_Sat8_8( &VB(VSCREEN), &VB(2), &VB(JANIM) );
   }
   else
   {
      BYTE Pal[256*4];
      x = (x-T2) / ( 1.0f-T2 );
      x = 1.0f - x;
      x = 1.0f - x*x;
      A = (INT)( x*255.0f );
      Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 230,220,180, 0x2565 );
      CMap_To_16bits_With_Fx( (void*)Pal, Cur_Mask->Pal, Cur_Mask->Nb_Col, 0x777,
         0xdFdFdF, NULL, A, 0 );
      DMask_Setup_Pal( (UINT*)VB(VMASK).CMap, Cur_Mask, (PIXEL*)Pal );      
      Mixer.Mix_8_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(JANIM) );
   }
}

EXTERN void Close_Blur_II( )
{ 
   Destroy_16b( Blur012 );
   Clear_DMask( );      // NOT the anim
}

/************************************************************/

#define Init_Dsp_XY( Ax, Ay )                                       \
{  Init_Saturation_Table4( Dsp_Off_X, -Ax, Ax, 1.0, 1, 256 );       \
   Init_Saturation_Table4( Dsp_Off_Y, -Ay, Ay, 1.0, The_W, 256 ); } \

#define Init_Dsp_XY_2( Ax )                                   \
   Init_Saturation_Table4( Dsp_Off_X, -Ax, Ax, 1.0, 1, 256 )

#define Init_Dsp_X_3( Ax )                                    \
   Init_Saturation_Table4( Dsp_Off_X, -2*Ax,0, 1.0, 1, 256 )

#define Init_Dsp_Y_3( Ax )                                    \
   Init_Saturation_Table4( Dsp_Off_X, -2*Ax,0, 1.0, 1, 256 )

/************************************************************/

EXTERN void Init_Dsp( INT Param )
{
   Init_Saturation_Table5( 
      Dsp_Sat_X, 0, The_W-1, 1.0, 1, OFFX, The_W-1, OFFX+1 );
   Init_Saturation_Table5(    // The_W should be Out->BpS, here...
      Dsp_Sat_Y, 0, The_H-1, 1.0, The_W, OFFY, The_H-1, OFFY+1 );
}

/************************************************************/

EXTERN void Loop_Dsp_I( )     // Dsp_Radial_X/Y = #1
{
   INT Dsp_Off_X[256], Dsp_Off_Y[256];

   FLT x = Get_Beat_2( );
   if ( x==0.0 ) return;

   Init_Dsp_XY( (INT)( 16.0*x ), (INT)( 16.0*x ) );
   Mixer.Mix_Dsp_Off_Safe_X( &VB(VBLUR_I), &VB(VSCREEN), 
      Dsp_Radial, Dsp_Off_X, Dsp_Sat_X+OFFX );
   Mixer.Mix_Dsp_Off_Safe_Y( &VB(VSCREEN), &VB(VBLUR_I), 
      Dsp_Radial+The_W*The_H, Dsp_Off_X, Dsp_Sat_Y+OFFY );
#if 0
   Mixer.Mix_Dsp_Off( &VB(VBLUR_I), &VB(VSCREEN), Dsp_Radial, Dsp_Off_X );
   Mixer.Mix_Dsp_Off( &VB(VSCREEN), &VB(VBLUR_I), Dsp_Radial+The_W*The_H, Dsp_Off_Y );
#endif
}

#if 0
EXTERN void Loop_Dsp_II( )    // Dsp_Radial_X/Y = #2
{
   FLT x;
   INT Dsp_Off_X[256];
   x = Get_Beat_2( );
   if ( x==0.0 ) return;
   Init_Dsp_XY_2( (INT)( 32.0*x ) );
   Mixer.Mix_Dsp_Off_Safe_X( &VB(VBLUR_I), &VB(VSCREEN), 
      Dsp_Radial, Dsp_Off_X, Dsp_Sat_X+OFFX );
   Mixer.Mix_Dsp_Off_Safe_Y( &VB(VSCREEN), &VB(VBLUR_I), 
      Dsp_Radial+The_W*The_H, Dsp_Off_X, Dsp_Sat_Y+OFFY );
}
#endif

EXTERN void Loop_Dsp_III( )   // beats...
{
   FLT x;
   INT Dsp_Off_X[256];
   x = Get_Beat_3( );
   if ( x==0.0 ) { Dsp_Radial=NULL; return; }
   if ( Dsp_Radial==NULL )
   {
      if ( TRandom(2)&0x08 ) Dsp_Radial = Dsp_Radial_1;
      else Dsp_Radial = Dsp_Radial_2;
   }
   Init_Dsp_XY_2( (INT)( 15.0*x ) );
   Mixer.Mix_Dsp_Off_Safe_X( &VB(VBLUR_I), &VB(VSCREEN), Dsp_Radial, Dsp_Off_X, Dsp_Sat_X+OFFX );
   Mixer.Mix_Dsp_Off_Safe_Y( &VB(VSCREEN), &VB(VBLUR_I), Dsp_Radial+The_W*The_H, Dsp_Off_X, Dsp_Sat_Y+OFFY );
}

/************************************************************/

EXTERN void Init_Deform_Blur( )
{
    Init_Saturation_Table5( 
       Dsp_Sat_X, 0, The_W-1, 1.0, 1, OFFX, The_W-1, OFFX+1 );
    Init_Saturation_Table5(    // The_W should be Out->BpS, here...
       Dsp_Sat_Y, 0, The_H-1, 1.0, The_W, OFFY, The_H-1, OFFY+1 );
}

EXTERN void Deform_Blur( FLT Amp, INT i, INT j )
{
   INT Dsp_Off_X[256];

   Init_Dsp_XY_2( (INT)(Amp) );
   Mixer.Mix_Dsp_Off_Safe_X( &VB(VBLUR_I), &VB(VBLUR_II), 
      Dsp_Radial, Dsp_Off_X, Dsp_Sat_X+OFFX );
   Mixer.Mix_Dsp_Off_Safe_Y( &VB(VBLUR_II), &VB(VBLUR_I), 
      Dsp_Radial+The_W*The_H, Dsp_Off_X, Dsp_Sat_Y+OFFY );
   Mixer.Mix_16_16_Mix( &VB(VSCREEN), &VB(VBLUR_II), &VB(VSCREEN),
      Lo16_To_777[i], Hi16_To_777[i],
      Lo16_To_777[j], Hi16_To_777[j] );
}

EXTERN void Deform_Blur_II( FLT Amp, INT i, INT j )
{
   INT Dsp_Off_X[256];

   Init_Dsp_XY_2( (INT)(Amp) );
   Mixer.Mix_Dsp_Off_Safe_X( &VB(VBLUR_I), &VB(VSCREEN), 
      Dsp_Radial, Dsp_Off_X, Dsp_Sat_X+OFFX );
   Mixer.Mix_Dsp_Off_Safe_Y( &VB(VSCREEN), &VB(VBLUR_I), 
      Dsp_Radial+The_W*The_H, Dsp_Off_X, Dsp_Sat_Y+OFFY );
   Mixer.Mix_16_16_Mix_Copy( &VB(VSCREEN), &VB(VBLUR_II), &VB(VSCREEN),
      Lo16_To_777[i], Hi16_To_777[i],
      Lo16_To_777[j], Hi16_To_777[j] );
}

EXTERN void Deform_Blur_III( FLT Amp, INT i, INT j ) // Radial=#2
{
   INT Dsp_Off_X[256];
   Init_Dsp_XY_2( (INT)(Amp) );
   Mixer.Mix_Dsp_Off_Safe_X( &VB(VBLUR_I), &VB(VBLUR_II), 
      Dsp_Radial, Dsp_Off_X, Dsp_Sat_X+OFFX );
   Mixer.Mix_Dsp_Off_Safe_Y( &VB(VBLUR_II), &VB(VBLUR_I), 
      Dsp_Radial+The_W*The_H, Dsp_Off_X, Dsp_Sat_Y+OFFY );
   Mixer.Mix_16_16_Mix( &VB(VSCREEN), &VB(VBLUR_II), &VB(VSCREEN),
      Lo16_To_777[i], Hi16_To_777[i],
      Lo16_To_777[j], Hi16_To_777[j] );
}

/************************************************************/

EXTERN void Init_Dissolve( INT Param )
{
   Dsp_Radial = Dsp_Radial_1;
   Init_Saturation_Table5( Dsp_Sat_X, 0, The_W-1, 1.0, 1, OFFX, The_W-1, OFFX+1 );

   if ( Param==1 ) Dsp_Parity = 0x00;  // (for Dissolve_X2  #1)
   else if ( Param==2 ) Dsp_Parity = 0x01;  // (for Dissolve_Y #1)
}

EXTERN void Do_Dissolve( ) // Radial=#2
{
   FLT Amp;
   INT Dsp_Off_X[256];
   INT i,j;

   i = (INT)( 9.0f+Tick_x*3.0 );
   j = (INT)( 7.0f+Tick_x*3.0 );
   Amp = Tick_x * 50.0f;
   Init_Dsp_XY_2( (INT)(Amp) );
   Mixer.Mix_Dsp_Off_Safe_X( &VB(VBLUR_I), &VB(VSCREEN), 
      Dsp_Radial, Dsp_Off_X, Dsp_Sat_X+OFFX );
   Mixer.Mix_16_16_Mix( &VB(VSCREEN), &VB(VBLUR_I), &VB(VSCREEN),
      Lo16_To_777[i], Hi16_To_777[i],    // 6,9?
      Lo16_To_777[j], Hi16_To_777[j] );
}

EXTERN void Do_Dissolve_X( ) // Radial=#2
{
   FLT Amp;
   INT i,j;
   INT Dsp_Off_X[256];

   i = (INT)( 6.0f+Tick_x*4.0 );
   j = (INT)( 7.0f+Tick_x*4.0 );
   Amp = Tick_x*(1.0f-Tick_x)*4.0f*8.0f;
   Amp += (TRandom(2)&0xff)*(5.0f/256.0f) - 2.5f;

   Init_Dsp_X_3( (INT)(Amp) );
   Mixer.Mix_Dsp_Off_Safe_X( &VB(VBLUR_I), &VB(VSCREEN), 
      Dsp_Radial, Dsp_Off_X, Dsp_Sat_X+OFFX );
   Mixer.Mix_16_16_Mix( &VB(VSCREEN), &VB(VBLUR_I), &VB(VSCREEN),
      Lo16_To_777[i], Hi16_To_777[i],    // 6,8?
      Lo16_To_777[j], Hi16_To_777[j] );
}

EXTERN void Do_Dissolve_X2( ) // Radial=#2
{
   FLT Amp;
   INT Dsp_Off_X[256];

   Amp = Tick_x*(1.0f-Tick_x)*4.0f*10.0f;
   Amp += (TRandom(2)&0xff)*(4.0f/256.0f) - 2.0f;
   if ( Dsp_Parity ) Amp = -Amp;
   // if ( (TRandom(2)&0x03)==0x00 ) 
   Dsp_Parity ^= 0x01;
   Init_Dsp_XY_2( (INT)(Amp) );
   Mixer.Mix_Dsp_Off_Safe_X( &VB(VBLUR_I), &VB(VSCREEN), 
      Dsp_Radial, Dsp_Off_X, Dsp_Sat_X+OFFX );
   Mixer.Mix_16_16_Mix( &VB(VSCREEN), &VB(VBLUR_I), &VB(VSCREEN),
      Lo16_To_777[7], Hi16_To_777[7],    // 7,8?
      Lo16_To_777[9], Hi16_To_777[9] );
}

EXTERN void Do_Dissolve_Y( ) // Radial=#2
{
   FLT Amp;
   INT Dsp_Off_X[256];

   Amp = Tick_x*(1.0f-Tick_x)*4.0f*15.0f;
   Amp += (TRandom(1)&0xff)*(4.0f/256.0f) - 2.0f;
   if ( Dsp_Parity ) Amp = -Amp;
//   if ( (TRandom(0)&0x08)==0x00 ) 
   Dsp_Parity ^= 0x01;
   Init_Dsp_Y_3( (INT)(Amp) );
   Mixer.Mix_Dsp_Off_Safe_Y( &VB(VBLUR_I), &VB(VSCREEN), 
      Dsp_Radial+The_W*The_H, Dsp_Off_X, Dsp_Sat_Y+OFFY );
   Mixer.Mix_16_16_Mix( &VB(VSCREEN), &VB(VBLUR_I), &VB(VSCREEN),
      Lo16_To_777[7],  Hi16_To_777[7],    // 6,8?
      Lo16_To_777[8], Hi16_To_777[8] );
}

#if 0

EXTERN void Init_Dissolve_II( INT Param )
{
   Dsp_Radial = Dsp_Radial_1;
   Init_Saturation_Table5( 
      Dsp_Sat_X, 0, The_W-1, 1.0, 1, OFFX, The_W-1, OFFX+1 );
   Init_Saturation_Table5( 
      Dsp_Sat_Y, 0, The_H-1, 1.0, The_W, OFFY, The_H-1, OFFY+1 );
   SELECT_565(1);
   Copy_Buffer_Short( &VB(1),&VB(VSCREEN) );
}

EXTERN void Do_Dissolve_II( ) // Radial=#2
{
   FLT Amp;
   INT Dsp_Off_X[256];

   Amp = Tick_x*20.0f;
   Init_Dsp_XY_2( (INT)(Amp) );
   Mixer.Mix_Dsp_Off_Safe_Y( &VB(VBLUR_I), &VB(VSCREEN), 
      Dsp_Radial+The_W*The_H, Dsp_Off_X, Dsp_Sat_Y+OFFY );
   Mixer.Mix_Dsp_Off_Safe_X( &VB(VBLUR_II), &VB(VBLUR_I), 
      Dsp_Radial, Dsp_Off_X, Dsp_Sat_X+OFFX );
   Mixer.Mix_16_16_Mix_Copy( &VB(VSCREEN), &VB(1), &VB(VBLUR_II),
      Lo16_To_777[9], Hi16_To_777[9],     // 8,7?
      Lo16_To_777[8], Hi16_To_777[8] );
}
#endif

/************************************************************/

EXTERN void Init_Blur_I( INT Param )
{
   Init_Deform_Blur( );
   Copy_Buffer_Short( &VB(VBLUR_II), &VB(VSCREEN) );

   if ( Param==0 ) Dsp_Radial = Dsp_Radial_1; // default
   else if ( Param==1 ) Dsp_Radial = Dsp_Radial_2;
}

EXTERN void Loop_Deform_Blur_I( )      // disappear
{
   FLT x, Amp;

   x = (FLT)cos( Tick_x*M_PI); x = 1.0f - x*x;
   Amp = x*28.0f;  // 80.0?
   x = 1.0f-Tick_x; x = 1.0f-x*x;
   Deform_Blur_II( Amp, (INT)(6+9*x), (INT)(5+6*x) );
}

EXTERN void Loop_Deform_Blur_IV( )      // Crunch disappear
{
   FLT x, Amp;

   Dsp_Radial = Dsp_Radial_2;
   x = (FLT)cos( Tick_x*M_PI); x = 1.0f - x*x;
   Amp = -x*7.0f;  // 80.0?
   x = 1.0f-Tick_x; x = 1.0f-x*x;
   Deform_Blur_II( Amp, (INT)(6+4*x), (INT)(6+5*x) );
}

EXTERN void Loop_Deform_Blur_II( )  // Parental advisory
{
   FLT x, Amp;
   x = 1.0f - Tick_x;
   Amp = x*90.0f;
   Copy_Buffer_Short( &VB(VBLUR_II), &VB(1) );
   Deform_Blur( Amp, (INT)(4+8*x), (INT)(4+9*x) );
   Scratch_And_Flash( Get_Beat( ) );
   Get_Next_Scratch( 0x08 );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
}

EXTERN void Loop_Deform_Blur_III( )
{
   FLT x, Amp;

   x = 1.0f - Tick_x;
   Amp = 7.0f + x*25.0f + 50.0f*x*(FLT)sin( x*M_PI*6.0 );
   Mixer.Mix_16_16_Mix( &VB(VBLUR_II), &VB(VSCREEN), &VB(1),
      Lo16_To_777[9], Hi16_To_777[9],
      Lo16_To_777[8], Hi16_To_777[8] );
   Deform_Blur_III( Amp, (INT)(5.5+5.5*x), (INT)(5.5+5.5*x) );
   Scratch_And_Flash( Get_Beat( ) );
   Get_Next_Scratch( 0x08 );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
}

/************************************************************/

// EXTERN void Close_Dsp( ) { DISPOSE(1); }

/************************************************************/
