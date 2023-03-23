/*
 * Parts
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

EXTERN BYTE Appear_Map[256+2*256];

   // 65kb tables...
EXTERN UINT Lo16_To_777_Appear[32][256];
EXTERN UINT Hi16_To_777_Appear[32][256];

static BITMAP_16 *Appear, *Appear2;

EXTERN void Pre_Cmp_Appear( )
{
   INT i,j,l;

   Appear = Load_JPEG( "appear2.jpg", &i );
   Appear2 = Load_JPEG( "appear3.jpg", &i );

   for( i=j=0; i<256; i++, j++ ) Appear_Map[j] = 0;
   for( i=0; i<256; i++, j++ ) 
   {
      FLT x;
      x = (1.0f/256.0f)*i;
      x = (FLT)sin( x*M_PI/2.0f );
      x = x*x;
      if ( x<0.0 ) x = Appear_Map[j] = 0;
      else if ( x>=1.0 ) Appear_Map[j] = 31;
      else Appear_Map[j] = (BYTE)floor( 31.0*x );
   }
   for( i=0; i<256; i++, j++ ) Appear_Map[j] = 31;

   for( l=0; l<16; ++l )
   {
      FLT x;
      UINT Rw, Gw, Bw;
      x = 1.0f*(16-l)/16;
      
         // color to fade to...(white)
      Bw = ( 0x7F*l/16 ); Gw = ( 0x7F*l/16 ); Rw = ( 0x7F*l/16 );

      for( i=0; i<256; ++i )
      {
         UINT Ro, G1o, G2o, Bo;
         UINT R, G, G1, G2, B;

               // base colors. (7bits)

         Bo  = 4*(i&0x1F);
         G1o = 2*(i>>5); G2o = (i&0x07)<<4;
         Ro  = 4*(i>>3);

               // Fade to black

         B = (UINT)( Bw * x );
         G = (UINT)( Gw * x );
         R = (UINT)( Rw * x );
         Lo16_To_777_Appear[l+16][i] = B;
         Hi16_To_777_Appear[l+16][i] = (G<<8) | (R<<16);

               // Fade to white

         B  = Bo  + Bw; if (B>0x7F ) B = 0x7F;
         G1 = G1o + Gw; if (G1>0x7F ) G1 = 0x7F;
         G2 = G2o + Gw; if (G2>0x7F ) G2 = 0x7F;
         R  = Ro  + Rw; if (R>0x7F ) R = 0x7F;

               // Fade to white 16bits
         G2 = G2&0x78; G1 = G1&0x07;
         Lo16_To_777_Appear[l][i] = B | (G1<<8);
         Hi16_To_777_Appear[l][i] = (G2<<8) | (R<<16);
      }
   }
}

EXTERN void Init_Appear( INT Param )
{
   switch( Param )
   {
#if 0       // USELESS
      case 0:       
         SELECT_565( VBLUR_II );
         Copy_Buffer_Short( &VB(VBLUR_II), &VB(VSCREEN) );      // copy screen in VB #1
         SELECT_CMAP_V( 1 );
         Check_CMap_VBuffer( &VB(1), 256 );
         VB(1).Bits = Appear->Bits;
      break;
#endif
      default: case 1:  
         SELECT_565( VBLUR_II );
         SELECT_CMAP( 1 );
         Check_CMap_VBuffer( &VB(1), 256 );
      break;
      case 2:
         SELECT_565( VBLUR_I );
         memset( VB(VBLUR_I).Bits, 0, VB(1).Size );    // <= void screen
         SELECT_CMAP( 1 );
         Check_CMap_VBuffer( &VB(1), 256 );
         SELECT_565( VBLUR_II );
      break;
   }
}

/********************************************************************/

static void Remap_Appear( INT VBuffer, INT Size, BYTE *Src )
{
   INT x = Global_x<0.0 ? 0 : Global_x>=1.0 ? 512 : (INT)floor(Global_x*512.0);
   BYTE *Map = Appear_Map + x;
   Remap_Byte( (BYTE*)VB(VBuffer).Bits, Src, Size, Map );
}

static void Remap_Appear_Inv( INT VBuffer, INT Size, BYTE *Src )
{
   INT x = Global_x<0.0 ? 0 : Global_x>=1.0 ? 512 : (INT)floor(Global_x*512.0);
   BYTE *Map = Appear_Map + 512 - x;
   Remap_Byte( (BYTE*)VB(VBuffer).Bits, Src, Size, Map );
}

/********************************************************************/

#if 0

EXTERN void Loop_Appear( )
{
   INT Cut;

   Cut = (INT)( Global_x*256 );
   if ( Cut>254 ) Cut=254;
   else if ( Cut<2 ) Cut = 2;
   Drv_Build_Ramp_16( VB(1).CMap, 0, Cut, 0,0,0, 
      255, 255, 255, 0x777 );
   Drv_Build_Ramp_16( VB(1).CMap, Cut, 256, 255,255,255,
      255, 255, 255, 0x777 );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VBLUR_II), &VB(1) );
}

#endif

EXTERN void Loop_Appear_II( )
{
   Remap_Appear( 1, 64000, (BYTE*)Appear->Bits );
   Loop_14_Anim_Mask_Raw( );
   Mix_16_16_Blend( &VB(VSCREEN), &VB(VBLUR_II), &VB(VSCREEN), &VB(1),
       31, (UINT*)Lo16_To_777_Appear, (UINT*)Hi16_To_777_Appear );
}

EXTERN void Close_Appear_II( )
{
   DISPOSE(1);
}

EXTERN void Loop_Disappear( )
{
   Remap_Appear_Inv( 1, 64000, (BYTE*)Appear2->Bits );
//   Loop_14_Anim_Mask_Raw( );
   Mix_16_16_Blend( &VB(VSCREEN), &VB(VBLUR_II), &VB(VBLUR_I), &VB(1),
       31, (UINT*)Lo16_To_777_Appear, (UINT*)Hi16_To_777_Appear );
}

#if 0
EXTERN void Loop_Disappear_2( )
{
   Remap_Appear_Inv( 1, 64000, (BYTE*)Appear->Bits );
   Mix_16_16_Blend( &VB(VSCREEN), &VB(VBLUR_II), &VB(VBLUR_I), &VB(1),
       31, (UINT*)Lo16_To_777_Appear, (UINT*)Hi16_To_777_Appear );
}
#endif

EXTERN void Close_Disappear( )
{
   DISPOSE(1);
}

EXTERN void Destroy_Appear( )
{
   DISPOSE(1);
   Destroy_16b( Appear );
   Destroy_16b( Appear2 );
}

/********************************************************************/
/********************************************************************/
