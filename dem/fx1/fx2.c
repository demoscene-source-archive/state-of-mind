/*
 * - FX II -      ALL USELESS, now
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/
/********************************************************************/

#if 0

EXTERN USHORT Map1_Dsp[The_W2*The_H2];
EXTERN USHORT Map2_Dsp[The_W2*The_H2];
EXTERN SHORT Dsp[The_W*The_H];
EXTERN PIXEL *Blend_Ramp;  // [3*The_W*The_H];   ---ramp for blend

EXTERN void Init_Tunnels( INT cpu )
{
   INT i, j;
   SHORT *Dst;
   BITMAP *Tmp;

   for( j=0; j<The_H2; ++j )
   {
      USHORT Y; FLT y;
      y = 2.0*j/The_H2-1.0;

      for( i=0; i<The_W2; ++i )
      {
         USHORT X; FLT x;
         FLT r, Theta;
         x = 2.0*i/The_W2-1.0;

         Y = (USHORT)( 255.5/sqrt(1.0+y*y+x*x*4.0) );
         if ( x!=0.0 ) Theta = 1.0 - atan2( y, x )/(2.0*M_PI);
         else Theta = 0.0;
         X = (USHORT)(255.5*Theta);
         Map1_Dsp[j*The_W2 + i] = X + (Y<<8);

//         Y = (USHORT)( 255.5/sqrt(1.0+r*r+x*x*4.0) );
//         X = (USHORT)( 255.5*Theta);
         r = sqrt( x*x+y*y );
         r = 127.5*( 1.0 - r*r );
         Theta = sin( x*7.84+y*4.3221 ) + cos( .64+x*y*24.12 );
         X = (USHORT)( 63.5 + .5+r*Theta/2.0 );
         Theta = sin( x*7.14-y*3.3221 ) + sin( 1.2+x*15.7 );
         Y = (USHORT)( 63.5 + .5+r*Theta/2.0 );
         Map2_Dsp[j*The_W2 + i] = X + (Y<<8);
      }
   }
   Dst = Dsp;
   for( j=0; j<The_H; ++j )
   {
      FLT y;
      y = 2.0*j/(The_H-15)-1.0;
      if ( j<8 || j>=The_H-8 ) { memset( Dst, 0, The_W ); Dst+=The_W; }
      else for( i=0; i<The_W; ++i )
      {
         FLT x, t, r;
         if ( i<8||i>=The_W-8 ) *Dst++ = 0;
         else
         {
            x = 2.0*i/(The_W-15)-1.0;
            r = x*x*y*y;
            r = sqrt( 1.0-r*r );
            r *= 7.0;

//            t = cos( M_PI*2.0*x ) + sin( M_PI*8.0*y );
//            t = cos( M_PI*6.0*y+.1352 ) + sin( M_PI*6.0*y-1.23421 );

            *Dst++ = (SHORT)( x*r ) + ((SHORT)(y*r))*The_W;
         }
      }
   }
   Tmp = Load_GIF( "bramp.gif" );
   Blend_Ramp = New_Fatal_Object( The_W*The_H*3, PIXEL );
   memcpy( Blend_Ramp, Tmp->Bits,  The_W*The_H*3 );
   Destroy_Bitmap( Tmp );
}

/********************************************************************/
/********************************************************************/

EXTERN BYTE *Build_Plasma_I( int THE_W, int THE_H, FLT Ho )
{
   int i, j;
   PIXEL *Radial, *Ptr;

   i = THE_W*THE_H*sizeof( PIXEL );
   Radial = ( PIXEL * )My_Malloc( i );
   if ( Radial == NULL ) Exit_Upon_Mem_Error( "Radial", i );
   Ptr = Radial;
   for( j=0; j<THE_H; ++j )
      for( i=0; i<THE_W; ++i )
      {
         FLT Norm;
         Norm = Ho + (Ho/6.0)*( 
            sin(i/12.0) + cos(j/15.0) + cos(i/13.0) + sin(j/11.0)
            + sin((i+j)/17.0 ) 
            + cos( sqrt( (256.0-i)*(256.0-i)+(150.0-j)*(150.0-j) )/13.0 )
         );
         *Ptr++ = ( PIXEL )floor( Norm );
      }

   return( Radial );
} 

#endif   // 0

/********************************************************************/

#if 0       // USELESS now

EXTERN void Do_Plasma_2Pt( void *Dst_p, INT Dst_BpS, 
   void *Src1_p, void *Src2_p, INT Src_BpS,
   INT W, INT H )
{
#if 1
   INT j;
   UINT *Dst, *Src1, *Src2;

   W = -W/4;
   Dst = (UINT*)Dst_p; Dst -= W;
   Src1 = (UINT*)Src1_p; Src1 -= W;
   Src2 = (UINT*)Src2_p; Src2 -= W;
   for( j=H; j>0; --j )
   {
      INT i;
      for( i=W; i<0; ++i )
         Dst[i] = Src1[ i ] + Src2[ i ];
      (BYTE*)Dst += Dst_BpS;
      (BYTE*)Src1 += Src_BpS; (BYTE*)Src2 += Src_BpS;
   }
#else
   INT j;
   char *Dst, *Src1, *Src2;

   W = -W;
   Dst = (char*)Dst_p; Dst -= W;
   Src1 = (char*)Src1_p; Src1 -= W;
   Src2 = (char*)Src2_p; Src2 -= W;
   for( j=H; j>0; --j )
   {
      INT i;
      for( i=W; i<0; ++i )
         Dst[i] = Src1[ i ] + Src2[ i ];
      Dst += Dst_BpS;
      Src1 += Src_BpS; Src2 += Src_BpS;
   }

#endif
}

#endif   // USELESS

/********************************************************************/
/********************************************************************/
