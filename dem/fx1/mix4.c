/*
 * - Mix4 -
 *
 * Skal 98
 ***************************************/

#include "demo.h"
#define ADV(a,l,T) a = (T*)((BYTE*)a + l)

/********************************************************************/

         // Mixing, new generation

#ifdef UNIX 

EXTERN void Mix_Sat16( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   USHORT *Src2 = (USHORT *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Src1[i] );
         S777 += RGB_16_TO_777(Src2[i]);
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);
   }
}

EXTERN void Mix_Sat16_Clear12( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   USHORT *Src2 = (USHORT *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Src1[i] );
         S777 += RGB_16_TO_777(Src2[i]);
         Src2[i] = Src1[i] = 0x0000;
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);
   }
}

EXTERN void Mix_Sat16_Clear1( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;

   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   USHORT *Src2 = (USHORT *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Src1[i] );
         S777 += RGB_16_TO_777(Src2[i]);
         Src1[i] = 0x0000;
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);
   }
}

EXTERN void Mix_Sat16_Clear2( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   USHORT *Src2 = (USHORT *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Src1[i] );
         S777 += RGB_16_TO_777( Src2[i] );
         Src2[i] = 0x0000;
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);
   }
}

/********************************************************************/

         // Mixing 16bits + 8bits colormapped
         // CMap is 0777

EXTERN void Mix_Sat16_8( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   UINT *CMap = (UINT*)In2->CMap;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Src1[i] );
         S777 += CMap[ Src2[i] ];
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}

EXTERN void Mix_Sat16_8_Clear12( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   UINT *CMap = (UINT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Src1[i] );
         S777 += CMap[ Src2[i] ];
         Src1[i] = Src2[i] = 0x0000;
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}
EXTERN void Mix_Sat16_8_Clear1( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   UINT *CMap = (UINT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Src1[i] );
         S777 += CMap[ Src2[i] ];
         Src1[i] = 0x0000;
         Dst[i] = RGB_777_TO_16( S777 );
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}
EXTERN void Mix_Sat16_8_Clear2( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   UINT *CMap = (UINT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Src1[i] );
         S777 += CMap[ Src2[i] ];
         Src2[i] = 0x0000;
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}

#if 0    // UNUSED NOW

EXTERN void Mix_Sat16_8_M( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   UINT *CMap = (UINT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777_M( Src1[i] );
         S777 -= CMap[ Src2[i] ];
         Dst[i] = RGB_777_TO_16_M(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}

#endif

/********************************************************************/

         // Mixing 8bits + 8bits colormapped
         // CMap is 0777

EXTERN void Mix_Sat8_8( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   PIXEL *Src1 = (PIXEL *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   UINT *CMap1 = (UINT*)In1->CMap;
   UINT *CMap2 = (UINT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777  = CMap1[ Src1[i] ];
         S777 += CMap2[ Src2[i] ];
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,PIXEL);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}
EXTERN void Mix_Sat8_8_Clear12( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   PIXEL *Src1 = (PIXEL *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   UINT *CMap1 = (UINT*)In1->CMap;
   UINT *CMap2 = (UINT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777  = CMap1[ Src1[i] ];
         S777 += CMap2[ Src2[i] ];
         Src1[i] = Src2[2] = 0x0000;
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,PIXEL);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}
EXTERN void Mix_Sat8_8_Clear1( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   PIXEL *Src1 = (PIXEL *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   UINT *CMap1 = (UINT*)In1->CMap;
   UINT *CMap2 = (UINT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777  = CMap1[ Src1[i] ];
         S777 += CMap2[ Src2[i] ];
         Src1[i] = 0x0000;
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,PIXEL);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}
EXTERN void Mix_Sat8_8_Clear2( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   PIXEL *Src1 = (PIXEL *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   UINT *CMap1 = (UINT*)In1->CMap;
   UINT *CMap2 = (UINT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777  = CMap1[ Src1[i] ];
         S777 += CMap2[ Src2[i] ];
         Src2[i] = 0x0000;
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,PIXEL);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}

#if 0    // UNUSED NOW

EXTERN void Mix_Sat8_8_Zero( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   PIXEL *Src1 = (PIXEL *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   UINT *CMap1 = In1->CMap;
   UINT *CMap2 = In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         if ( Src1[i] )
         {
            S777  = CMap1[ Src1[i] ];
            S777 += CMap2[ Src2[i] ];
            Dst[i] = RGB_777_TO_16(S777);
         }
         else Dst[i] = 0x0000;
      }
      ADV(Src1,In1->BpS,PIXEL);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);

   }
}

#endif

/********************************************************************/

EXTERN void Mix_16_8_Blend_Mask( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   USHORT *CMap = (USHORT*)In1->CMap;
   USHORT *Src2 = (USHORT *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;

   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         BYTE b = Src1[i]>>8;
         USHORT C = CMap[ Src1[i]&0xFF ];
         UINT S777 = RGB_16_TO_777_Blend( b, C );
         S777 += RGB_16_TO_777_Blend( b^0x0F, Src2[i] );
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);

   }
}
EXTERN void Mix_16_8_Blend_Mask_II( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   PIXEL *Src1 = (PIXEL *)In1->Bits + Out->W;
   UINT *CMap = (UINT*)In1->CMap;
   USHORT *Src2 = (USHORT *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;

   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777 = CMap[ Src1[i] ];
         BYTE b = S777&0xFF; S777 >>= 8;
         S777 += RGB_16_TO_777_Blend( b, Src2[i] );
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,PIXEL);
      ADV(Src2,In2->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);
   }
}

EXTERN void Mix_Sat8_8_Blend( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
   VBUFFER *Bld )
{
   INT j;
   PIXEL *Src1 = (PIXEL *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   PIXEL *Blend = (PIXEL *)Bld->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   USHORT *CMap1 = (USHORT*)In1->CMap;
   USHORT *CMap2 = (USHORT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         BYTE b = Blend[i]; 
         USHORT C = CMap1[ Src1[i] ];
         UINT S777 = RGB_16_TO_777_Blend( b, C );
         b ^= 0x0F; C = CMap2[ Src2[i] ];
         S777 += RGB_16_TO_777_Blend( b, C );
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,PIXEL);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
      ADV(Blend,Bld->BpS,PIXEL);
   }
}

EXTERN void Mix_8_8_Blend_Mask( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   USHORT *CMap1 = (USHORT*)In1->CMap;
   USHORT *CMap2 = (USHORT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         BYTE b = Src1[i]>>8; 
         USHORT C = CMap1[ (Src1[i])&0xFF ];
         UINT S777 = RGB_16_TO_777_Blend( b, C );
         b ^= 0x0F; C = CMap2[ Src2[i] ];
         S777 += RGB_16_TO_777_Blend( b, C );
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}
EXTERN void Mix_8_8_Blend_Mask_II( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   PIXEL *Src1 = (PIXEL *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   UINT *CMap1 = (UINT*)In1->CMap;
   USHORT *CMap2 = (USHORT*)In2->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777 = CMap1[ Src1[i] ];
         BYTE b = S777 & 0xFF;
         USHORT C = CMap2[ Src2[i] ];
         S777 >>= 8;
         S777 += RGB_16_TO_777_Blend( b, C );
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,PIXEL);
      ADV(Src2,In2->BpS,PIXEL);
      ADV(Dst,Out->BpS,USHORT);
   }
}

EXTERN void Mix_16_To_16( VBUFFER *Out, USHORT *TabLo, USHORT *TabHi )
{
   INT j;
   USHORT *Src = (USHORT *)Out->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
         Src[i] = RGB_16_TO_16_Fade( TabLo, TabHi, Src[i] );
      ADV(Src,Out->BpS,USHORT);
   }
}

EXTERN void Mix_16_16_Blend( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
   VBUFFER *Blend, INT Xor, UINT *TabLo, UINT *TabHi )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   USHORT *Src2 = (USHORT *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   PIXEL *Bld = (PIXEL*)Blend->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777; BYTE b = Bld[i];
         S777 = RGB_16_TO_777_Blend_Tab( TabLo, TabHi, b, Src1[i] );
         b ^= Xor;
         S777 += RGB_16_TO_777_Blend_Tab( TabLo, TabHi,  b, Src2[i] );
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);
      ADV(Bld,Blend->BpS,PIXEL);
   }
}

EXTERN void Mix_To_Fade( VBUFFER *Out, VBUFFER *In,
   USHORT *TabLo, USHORT *TabHi )
{
   INT j;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   USHORT *Src = (USHORT *)In->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
         Dst[i] = RGB_16_TO_16_Fade( TabLo, TabHi, Src[i] );
      ADV(Src,In->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);
   }
}

EXTERN void Mix_To_Fade_Feedback( VBUFFER *Out, VBUFFER *In,
   USHORT *TabLo, USHORT *TabHi )
{
   INT j;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   USHORT *Src = (USHORT *)In->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         Dst[i] = Src[i];
         Src[i] = RGB_16_TO_16_Fade( TabLo, TabHi, Src[i] );
      }
      ADV(Src,In->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);
   }
}

EXTERN void Mix_16_16_Mix( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
   UINT *TabLo1, UINT *TabHi1, UINT *TabLo2, UINT *TabHi2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   USHORT *Src2 = (USHORT *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;

   for( j=Out->H; j>0; --j )
   {
      INT i;

      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_16_Fade( TabLo1, TabHi1, Src1[i] );
         S777 += RGB_16_TO_16_Fade( TabLo2, TabHi2, Src2[i] );
         Dst[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);
   }
}
EXTERN void Mix_16_16_Mix_Copy( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
   UINT *TabLo1, UINT *TabHi1, UINT *TabLo2, UINT *TabHi2 )
{
   INT j;
   USHORT *Src1 = (USHORT *)In1->Bits + Out->W;
   USHORT *Src2 = (USHORT *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;

   for( j=Out->H; j>0; --j )
   {
      INT i;

      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_16_Fade( TabLo1, TabHi1, Src1[i] );
         S777 += RGB_16_TO_16_Fade( TabLo2, TabHi2, Src2[i] );
         Dst[i] = Src1[i] = RGB_777_TO_16(S777);
      }
      ADV(Src1,In1->BpS,USHORT);
      ADV(Src2,In2->BpS,USHORT);
      ADV(Dst,Out->BpS,USHORT);
   }
}

EXTERN void Mix_777_To_16( VBUFFER *Out, VBUFFER *In1 )
{
   INT j;
   UINT *Src = (UINT *)In1->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
         Dst[i] = RGB_777_TO_16(Src[i]);
      ADV(Src,In1->BpS,UINT);
      ADV(Dst,Out->BpS,USHORT);
   }
}

#endif   // UNIX

/********************************************************************
 *
 *                TO FINISH!!
 *
 ********************************************************************/

#if 1 // UNIX

#if 0    // UNUSED NOW

EXTERN void Mix_8_Mask( VBUFFER *Out, VBUFFER *In )
{
   INT j;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   USHORT *Src = (USHORT *)In->Bits + Out->W;
   USHORT *CMap = (USHORT*)In->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
         if ( Src[i] ) Dst[i] = CMap[ Src[i] ];
      (PIXEL*)Src += In->BpS;
      (PIXEL*)Dst += Out->BpS;
   }
}

EXTERN void Mix_16_Mask( VBUFFER *Out, VBUFFER *In )
{
   INT j;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   USHORT *Src = (USHORT *)In->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
         if ( Src[i] ) Dst[i] = Src[i];
      (PIXEL*)Src += In->BpS;
      (PIXEL*)Dst += Out->BpS;
   }
}

EXTERN void Mix_Sat16_16_Feedback( VBUFFER *Out, VBUFFER *In )
{
   INT j;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   USHORT *Src = (USHORT *)In->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Src[i] );
         S777 += RGB_16_TO_777(Dst[i]);
         S777 = RGB_777_TO_16_Remap(S777);
         Dst[i] = (USHORT)(S777 & 0xFFFF);
         Src[i] = (USHORT)(S777>>16);
      }
      (PIXEL*)Src += In->BpS;
      (PIXEL*)Dst += Out->BpS;
   }
}

EXTERN void Mix_8_8_Map( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 )
{
   INT j;
   PIXEL *Src1 = (PIXEL *)In1->Bits + Out->W;
   PIXEL *Src2 = (PIXEL *)In2->Bits + Out->W;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   USHORT *CMap = (USHORT*)In1->CMap;
   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         USHORT S;
         S = Src1[i] | (Src2[i]<<8);
         Dst[i] = CMap[S];
      }
      (PIXEL*)Src1 += In1->BpS;
      (PIXEL*)Src2 += In2->BpS;
      (PIXEL*)Dst += Out->BpS;
   }
}

#endif   // 0 <-> UNUSED


#endif   // UNIX

/********************************************************************/

#ifdef UNIX

EXTERN void Paste_256( VBUFFER *Out, PIXEL *Src, INT U1, UINT *CMap )
{
   INT j;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;

   for( j=Out->H; j>0; --j )
   {
      INT i;
      BYTE U = (BYTE)(U1)&0xff;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Dst[i] );
         S777 += CMap[ Src[U++] ];
         Dst[i] = RGB_777_TO_16(S777);
      }
      Src += 256;
      ADV(Dst,Out->BpS,USHORT);
   }
}

EXTERN void Paste_256_Y( VBUFFER *Out, PIXEL *Src, 
   INT V1, UINT *CMap, INT Src_W )
{
   INT j;
   USHORT *Dst = (USHORT *)Out->Bits + Out->W;
   PIXEL *Src1 = Src + V1*Src_W + Out->W;

   V1&=0xff;

   for( j=Out->H; j>0; --j )
   {
      INT i;
      for( i=-Out->W; i<0; ++i )
      {
         UINT S777;
         S777 = RGB_16_TO_777( Dst[i] );
         S777 += CMap[ Src1[i] ];
         Dst[i] = RGB_777_TO_16(S777);
      }
      ++V1; if ( V1==256 ) { Src1 = Src + Out->W; V1 = 0; }
      else Src1 += Src_W;
      ADV(Dst,Out->BpS,USHORT);
   }
}

#endif   // UNIX

/********************************************************************/

