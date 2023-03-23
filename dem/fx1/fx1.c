/*
 * - FX I -
 *
 * Skal 98
 ***************************************/

#include "demo.h"

// #define HARDWAY

EXTERN MIXER Mixer; // ={0}; <= important to be zeroed...

      // Lo16_To_777[n]/Hi16_To_777[n] fades to black as
      // n goes from 0 to MIX_LEVELS-1
      // this ones are used for blending 2 16bits sources together
      //
      // Lo16_To_777_White[n]/Hi16_To_White[n] fades to White as
      // n goes from 0 to MIX_LEVELS-1
      //
      // Lo16_To_16[n]/Hi16_To_16[n] and Lo16_To_16_White[n]/Hi16_To_16_White[n] 
      // does the same, but in 16bits.      

EXTERN UINT Lo16_To_777[MIX_LEVELS][256];
EXTERN UINT Hi16_To_777[MIX_LEVELS][256];
EXTERN USHORT RGB71_To_16[3][256];
EXTERN UINT R_Remap777[256], G_Remap777[256], B_Remap777[256];

#if 0    // UNUSED NOW

EXTERN USHORT Lo16_To_16[MIX_LEVELS][256];
EXTERN USHORT Hi16_To_16[MIX_LEVELS][256];

EXTERN UINT Lo16_To_777_M[256];
EXTERN UINT Hi16_To_777_M[256];
EXTERN USHORT RGB71_To_16_M[3][256];
EXTERN UINT RGB71_To_16_Remap_M[3][256];

EXTERN UINT RGB71_To_16_Remap[3][256];

#endif

#ifdef HARDWAY

EXTERN UINT Big_16_To_777[256*256];
EXTERN USHORT Big_777_To_16_GB[256*256];

#endif

/********************************************************************/
/********************************************************************/

EXTERN void Init_CMap_Ramp_Pos( BYTE *CMap,
   INT Ro, INT Go, INT Bo, INT Rf, INT Gf, INT Bf, 
   INT Nb, INT Nb2 )
{
   INT i, j;
   if ( Nb<2 ) return;
   for( i=j=0; i<Nb; ++i,j+=3 )
   {
      FLT x = (1.0f*i)/(Nb-1);
      FLT y = 1.0f-x;
      CMap[j+0] = (BYTE)( y*Ro + x*Rf );
      CMap[j+1] = (BYTE)( y*Go + x*Gf );
      CMap[j+2] = (BYTE)( y*Bo + x*Bf );
   }
   for ( i=0; i<Nb2; ++i, j+=3 )
   {
      CMap[j+0] = (BYTE)Rf;
      CMap[j+1] = (BYTE)Gf;
      CMap[j+2] = (BYTE)Bf;
   }
}

#if 0       // USELESS NOW

EXTERN void Init_CMap_Ramp_Neg( BYTE *CMap,
   INT Ro, INT Go, INT Bo, INT Rf, INT Gf, INT Bf, 
   INT Nb, INT Nb2 )
{
   INT i, j;

   for ( i=j=0; i<Nb; ++i, j+=3 )
   {
      CMap[j+0] = (BYTE)Ro;
      CMap[j+1] = (BYTE)Go;
      CMap[j+2] = (BYTE)Bo;
   }
   if ( Nb2<2 ) return;
   for( i=0; i<Nb2; ++i,j+=3 )
   {
      FLT x = (1.0f*i)/(Nb2-1);
      FLT y = 1.0f-x;
      CMap[j+0] = (BYTE)( y*Ro + x*Rf );
      CMap[j+1] = (BYTE)( y*Go + x*Gf );
      CMap[j+2] = (BYTE)( y*Bo + x*Bf );
   }
}

#endif      // 0


EXTERN void CMap_Split( USHORT Maps[3][256], BYTE *CMap, INT Nb )
{
   INT i,j;

   if ( Nb>255 ) Nb=255;
   for( i=j=0; i<Nb; ++i, j+=3 )
   {
      USHORT Col;
      Col = (USHORT)CMap[j];
      Maps[0][i] = ( Col>>3 ) &0x001F;
      Col = (USHORT)CMap[j+1];
      Maps[1][i] = ( Col<<3 ) &0x07E0;
      Col = (USHORT)CMap[j+2];
      Maps[2][i] = ( Col<<8 ) &0xF800;
   }
}

#if 0          // USELESS NOW

EXTERN void CMap_Split_Remap( UINT Maps[3][256], 
   BYTE *CMap1, BYTE *CMap2, INT Nb )
{
   INT i,j;

   if ( Nb>255 ) Nb=255;
   for( i=j=0; i<Nb; ++i, j+=3 )
   {
      UINT Col;

      Col = (UINT)CMap1[j];
      Maps[0][i] = ( Col>>3 ) &0x001F;
      Col = (UINT)CMap2[j];
      Maps[0][i] |= ( ( Col>>3 ) &0x001F ) << 16;

      Col = (UINT)CMap1[j+1];
      Maps[1][i] = ( Col<<3 ) &0x07E0;
      Col = (UINT)CMap2[j+1];
      Maps[1][i] |= ( ( Col<<3 ) &0x07E0 ) << 16;

      Col = (UINT)CMap1[j+2];
      Maps[2][i] = ( Col<<8 ) &0xF800;
      Col = (UINT)CMap2[j+2];
      Maps[2][i] |= ( ( Col<<8 ) &0xF800 ) << 16;
   }
}

#endif   // 0

/********************************************************************/
/********************************************************************/

EXTERN void Init_Mixing( INT cpu ) 
{
   INT i, l;
   BYTE CMap[256*3];
   BYTE CMap2[256*3];

         // Mixing, new generation

   Init_CMap_Ramp_Pos( CMap, 0,0,0, 255,255,255, 127, 129 );
   Init_CMap_Ramp_Pos( CMap2, 0,0,0, 200,200,200, 256, 0 );
   CMap_Split( RGB71_To_16, CMap, 256 );

#if 0 // UNUSED NOW
   CMap_Split_Remap( RGB71_To_16_Remap, CMap, CMap2, 256 );

   Init_CMap_Ramp_Neg( CMap, 0,0,0, 255,255,255, 128, 128 );
   Init_CMap_Ramp_Neg( CMap2, 0,0,0, 255,255,255, 128, 128 );
   CMap_Split( RGB71_To_16_M, CMap, 256 );
   CMap_Split_Remap( RGB71_To_16_Remap_M, CMap, CMap2, 256 );
#endif

   for( l=0; l<MIX_LEVELS; ++l )
   {
      FLT x;
      UINT Rw, G1w, G2w, Bw;
      x = 1.0f*(MIX_LEVELS-1-l)/(MIX_LEVELS-1);
      Rw = ( 0xFF*l/(MIX_LEVELS-1) ) >> 3;   // -> 0x1F
      G1w = ( l*0xFF/(MIX_LEVELS-1) ) &0xE0;
      G2w = ( l*0xFF/(MIX_LEVELS-1) ) >> 5;  // ->0x07
      Bw = ( l*0xFF/(MIX_LEVELS-1) ) &0xF8;
      for( i=0; i<256; ++i )
      {
         UINT Ro, G1o, G2o, Bo;
         UINT R, G1, G2, B;

               // base colors. resp 5,3+3,5 bits

         Ro  = (UINT)(i&0x1F);
         G1o = (UINT)(i&0xE0);
         G2o = (UINT)(i&0x07);
         Bo  = (UINT)(i&0xF8);

               // Fade to black

         R  = (UINT)floor(  Ro      * 4.00*x );
         G1 = (UINT)floor( (G1o>>5) * 2.00*x );
         G2 = (UINT)floor( (G2o<<3) * 2.00*x );
         B  = (UINT)floor( (Bo >>3) * 4.00*x );
         Lo16_To_777[l][i] = R | (G1<<8);
         Hi16_To_777[l][i] = (G2<<8) | (B<<16);

#if 0    // UNUSED NOW
         if ( l==0 )
         {
            Lo16_To_777_M[i] = Lo16_To_777[0][i] | 0x000080;
            Hi16_To_777_M[i] = Hi16_To_777[0][i] | 0x808000;
         }

               // Fade to black 16bits


         R  = ((UINT)floor(  Ro      * x )   ) & 0x1F;
         G1 = ((UINT)floor( (G1o>>5) * x )<<5) & 0xE0;
         G2 = ((UINT)floor( (G2o<<3) * x )>>3) & 0x07;
         B  = ((UINT)floor( (Bo >>3) * x )<<3) & 0xF8;
         Lo16_To_16[l][i] = (USHORT)( R | G1 );
         Hi16_To_16[l][i] = (USHORT)( ( G2 | B )<<8 );



               // Fade to white

         R  = Ro  + Rw; if (R>0x1F ) R = 0x1F;
         G1 = G1o + G1w; if (G1>0xE0 ) G1 = 0xE0; else G1 &= 0xE0;
         G2 = G2o + G2w; if (G2>0x07 ) G2 = 0x07;
         B  = Bo  + Bw; if (B>0xF8 ) B = 0xF8;  else B &= 0xF8;
         Lo16_To_16_White[l][i] = (USHORT)(R | G1);
         Hi16_To_16_White[l][i] = (USHORT)(( G2 | B )<<8);


               // Fade to white 16bits

         R = R<<2; G1 = G1>>4; G2 = G2<<4; B = B>>1;
         Lo16_To_777_White[l][i] = R | (G1<<8);
         Hi16_To_777_White[l][i] = ( G2<<8) | (B<<16);
#endif
      }
   }

#ifdef HARDWAY

   for( i=0; i<256*256; ++i )
   {
      UINT R, G, B;
      B = (UINT)(i&0x001F) << 2;
      G = (UINT)(i&0x07E0) >> 4;
      R = (UINT)(i&0xF800) >> 9;
      Big_16_To_777[i] = (R<<16) | (G<<8) | B;
   }
   memset( Big_777_To_16_GB, 0, 256*256*sizeof( Big_777_To_16_GB[0] ) );
   for( i=0; i<256; ++i )
   {
      INT j, G, R, k;
      if (i>127) j = 255; else j = 2*i;
      G = (j<<3)&0x07e0;
      R = (j<<8)&0xf800;
      for( k=0; k<256; ++k )
      {
         Big_777_To_16_GB[ i + k*256] |= G;
         Big_777_To_16_GB[ k + i*256] |= R;
      }
   }

#endif

         // 777 color remapping. Used in sprite4.asm

   for( i=0; i<256; ++i )
   {
      INT j;
//      if (i>=512/3) j = 255; else j = 3*i/2;
      if (i>=128) j = 255; else j = 2*i;
      R_Remap777[i] = (j<<15)&0x007F0000;
      G_Remap777[i] = (j<< 7)&0x00007F00;
      B_Remap777[i] = (j>> 1)&0x0000007F;
   }

   Mixer.Mix_Sat16 = Mix_Sat16;
   Mixer.Mix_Sat16_Clear12 = Mix_Sat16_Clear12;
   Mixer.Mix_Sat16_Clear1  = Mix_Sat16_Clear1;
   Mixer.Mix_Sat16_Clear2  = Mix_Sat16_Clear2;

   Mixer.Mix_Sat16_8 = Mix_Sat16_8;
   Mixer.Mix_Sat16_8_Clear12 = Mix_Sat16_8_Clear12;
   Mixer.Mix_Sat16_8_Clear1  = Mix_Sat16_8_Clear1;
   Mixer.Mix_Sat16_8_Clear2  = Mix_Sat16_8_Clear2;

   Mixer.Mix_Sat8_8 = Mix_Sat8_8;
   Mixer.Mix_Sat8_8_Clear12 = Mix_Sat8_8_Clear12;
   Mixer.Mix_Sat8_8_Clear1  = Mix_Sat8_8_Clear1;
   Mixer.Mix_Sat8_8_Clear2  = Mix_Sat8_8_Clear2;

   Mixer.Mix_16_16_Mix = Mix_16_16_Mix;
   Mixer.Mix_16_16_Mix_Copy = Mix_16_16_Mix_Copy;
   Mixer.Mix_16_16_Blend = Mix_16_16_Blend;
   Mixer.Mix_Sat8_8_Blend = Mix_Sat8_8_Blend;

   Mixer.Mix_16_8_Blend_Mask = Mix_16_8_Blend_Mask;
   Mixer.Mix_16_8_Blend_Mask_II = Mix_16_8_Blend_Mask_II;
   Mixer.Mix_8_8_Blend_Mask = Mix_8_8_Blend_Mask;
   Mixer.Mix_8_8_Blend_Mask_II = Mix_8_8_Blend_Mask_II;

   Mixer.Mix_16_To_16 = Mix_16_To_16;
   Mixer.Mix_To_Fade = Mix_To_Fade;
   Mixer.Mix_To_Fade_Feedback = Mix_To_Fade_Feedback;


   Mixer.Mix_777_To_16 = Mix_777_To_16;


   Mixer.Mix_Dsp_Off = Mix_Dsp_Off;
   Mixer.Mix_Dsp_Off_Safe_X = Mix_Dsp_Off_Safe_X;
   Mixer.Mix_Dsp_Off_Safe_Y = Mix_Dsp_Off_Safe_Y;

      // disabled since not used in the demo...

   Mixer.Mix_777_To_16_Remap = NULL;   // Mix_777_To_16_Remap;
   Mixer.Mix_Sat16_8_M = NULL;   // Mix_Sat16_8_M;
   Mixer.Mix_16_Mask = NULL;  // Mix_16_Mask;
   Mixer.Mix_8_Mask = NULL;   // Mix_8_Mask;
   Mixer.Mix_Sat8_8_Zero = NULL; // Mix_Sat8_8_Zero;
   Mixer.Mix_8_8_Map = NULL;  // Mix_8_8_Map;
   Mixer.Mix_Sat16_16_Feedback = NULL; // Mix_Sat16_16_Feedback;
   Mixer.Mix_Dsp_Off_Blur_X = NULL; // Mix_Dsp_Off_Blur_X;
   Mixer.Mix_Dsp_Off_Blur_Y = NULL; // Mix_Dsp_Off_Blur_Y;
}

EXTERN void Remap_Byte( BYTE *Dst, BYTE *Src, INT Size, BYTE *Map )
{
   INT i;
   Src += Size; Dst += Size;
   for( i=-Size; i<0; ++i ) Dst[i] = Map[Src[i]];
}

#if 0       // USELESS now

EXTERN void Remap_RGB565( USHORT *Dst, USHORT *Src, INT Size, USHORT *Map1, USHORT *Map2 )
{
   INT i;
   Src += Size; Dst += Size;
   for( i=-Size; i<0; ++i )
      Dst[i] = ( Map2[Src[i]>>8] ) + ( Map1[Src[i]&0xff] );
}
#endif

/********************************************************************/
