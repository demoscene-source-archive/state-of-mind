/***********************************************
 *      Spark effect                           *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "btm.h"
#include "rand.h"
#include "spark.h"
#include <math.h>
extern double pow( double, double );
extern double sqrt( double );

/*****************************************************************/

EXTERN PIXEL Spark_Tab[256];

#if 0

EXTERN USHORT *Setup_Spark( USHORT *Map, PIXEL *Bits, INT W, INT H,
  FLT Xo, FLT Yo, INT MapX, INT MapY )
{
   INT i, j;
   FLT Max;

      // MapX and MapY must be in [0,256]

   Max = 0.0;
//   for( i=0; i<W*H; ++i ) if ( (FLT)Bits[i]>Max ) Max = (FLT)Bits[i];
//   if ( Max==0.0 ) return( Map );
//   Max = 1.0*MapY/Max;
   for( j=0; j<H; ++j )
   {
      FLT y;
      y = (FLT)j - Yo;
      for( i=0; i<W; ++i )
      {
         FLT Theta, x, r;
         x = (FLT)i - Xo;
         if ( (x==0.0) && (y==0.0) ) Theta = (FLT)MapX-1;
         else Theta = (FLT)(MapX-1)*( M_PI-atan2( y, x ) )/(2.0*M_PI);
         // Map[i+W*j] = ( ((USHORT)Max*Bits[i+j*W] ) << 8 ) &0xFF00;
         r = sqrt( x*x+y*y )/160.0;
         if ( r!=0.0 ) r = pow ( r, .67 );
         Map[i+W*j] = (USHORT)( 200.0*r )<<8;
         Map[i+W*j] |= (USHORT)Theta & 0xFF;
      }
   }
   return( Map );
}
#endif

EXTERN void Build_Spark_Tab( INT MapX, INT MapY, INT Amp, INT Offset, FLT Gam )
{
   INT i;
   for( i=0; i<256; ++i )
   {
      FLT x;
      x = 1.0f*(i-128)/128.0f;
      x = x*x;
      if ( Gam!=1.0 && x!=0.0 ) x = (FLT)pow( x, Gam );
      Spark_Tab[i] = Offset + (PIXEL)(Amp*x);
   }
}

#if 0

EXTERN void Do_Spark_Blur( FLT Xo, FLT dXo, FLT *Len, FLT *dLen, 
   INT Nb, PIXEL *Dst, INT MapX, INT MapY )
{
   FLT L[50], dL[50];      // <= 100 pts. Enough?
   INT i, j, k;

   for( k=0; k<Nb; ++k ) { L[k] = Len[k]; dL[k] = dLen[k]; }

   for( j=0; j<MapY; ++j )
   {
      FLT A, X1, X2;
      A = 0.0;
      for( k=0; k<Nb; ++k ) 
      {
         L[k] += dL[k];
         if ( L[k]<0.0 ) L[k] = 0.0;
         else A += L[k];
      }
      if ( A<=0.0 ) return;
      A = MapX/A;
      Xo += dXo;
      X1 = Xo*MapX;
      for( k=0; k<Nb; ++k )
      {
         INT Xi, Xf;
         FLT Norm;
         if ( L[k]<=0.0 ) continue;
         Norm = A*L[k];
         Xi = (INT)( X1 );
         X2 = X1 + Norm;
         Xf = (INT)( X2 );
         Norm = 256.0/Norm;
         for( i=0; i<Xf-Xi; ++i ) Dst[(Xi+i)&0xFF] = 
            // (INT)(i*Norm);
            Spark_Tab[ (INT)(i*Norm) ];
         X1 = X2;
      }
      Dst += MapX;
   }
}
#endif      // 0



EXTERN void Do_Spark_Blur_II( FLT Xo, FLT dXo, FLT *Len, FLT *dLen, 
   INT Nb, PIXEL *Dst, INT MapX, INT MapY )
{
   FLT L[50], dL[50];      // <= 100 pts. Enough?
   INT i, j, k;

   for( k=0; k<Nb; ++k ) { L[k] = Len[k]; dL[k] = dLen[k]; }

   for( j=0; j<MapY; ++j )
   {
      FLT A, X1, X2;
      A = 0.0;
      for( k=0; k<Nb; ++k ) 
      {
         Hash_Seed = Hash1[ Hash_Seed ];
         L[k] += dL[k] + (FLT)Rand_Tab[ Hash_Seed ]*0.0011f;
         if ( L[k]<0.0 ) L[k] = 0.0;
         else A += L[k];
      }
      if ( A<=0.0 ) return;
      A = MapX/A;
      Xo += dXo + (FLT)Rand_Tab[ Hash_Seed ]*0.0011f;
      X1 = Xo*MapX;
      for( k=0; k<Nb; ++k )
      {
         INT Xi, Xf;
         FLT Norm;
         if ( L[k]<=0.0 ) continue;
         Norm = A*L[k];
         Xi = (INT)( X1 );
         X2 = X1 + Norm;
         Xf = (INT)( X2 );
         Norm = 256.0f/Norm;
         for( i=0; i<Xf-Xi; ++i ) 
            Dst[(Xi+i)&0xFF] = 
               (Dst[(Xi+i)&0xFF] + Spark_Tab[ (INT)(i*Norm) ] )/2;
         X1 = X2;
      }
      Dst += MapX;
   }
}


#if 0
EXTERN void Paste_Spark( USHORT *Src, PIXEL *Map, PIXEL *Dst, INT BpS,
   INT W, INT H, PIXEL *Conv )
{
   INT i, j;
   for( j=0; j<H; ++j )
   {
      for( i=0; i<W; ++i )
      {
//         Dst[i] = Conv[ Map[ Src[i] ] | (Dst[i]<<8) ];

         Dst[i] = Map[ Src[i] ];
//         Dst[i] = Src[i]&0xFF;
      }
      Dst += BpS;
      Src += BpS;
   }
}
#endif

/*****************************************************************/
