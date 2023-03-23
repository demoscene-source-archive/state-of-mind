/***********************************************
 *      Bump map utilities                     *
 *                                             *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "btm.h"
#include "bump.h"
#include <math.h>

/*****************************************************************/

EXTERN PIXEL *Build_Bump_Light_Tab( PIXEL *Ptr, FLT Gamma )
{
   INT i, j;

   if ( Ptr==NULL )
   {
      Ptr = New_Object( 256*256, PIXEL );
      if ( Ptr == NULL ) return( NULL );
   }

   for( j=0; j<256; ++j )
   {
      FLT y;
      y = (j-128)/128.0f;
      y = y*y;
      for( i=0; i<256; ++i )
      {
         FLT x, z;
         x = (i-128)/128.0f;
         z = 1.0f - (FLT)sqrt( y + x*x );
         if ( z<=0.0 ) z=0.0;
         else 
         {
            if ( Gamma!=1.0 && z!=0.0 ) z = (FLT)pow( (double)z, (double)Gamma );
         }
         Ptr[i+j*256] = (PIXEL)( z*255.0 );
      }
   }
   return( Ptr );
}

/*****************************************************************/

EXTERN void Height_To_Bump( char *Dst, PIXEL *Src, INT W, INT H )
{
   INT i, j;

    // Src[] should be in range [0,64[, so that Dst[] ranges in
    // [-128,127] after deltas...

   for( j=0; j<H-1; ++j )
   {
      for( i=0; i<W-1; ++i )
      {
         Dst[2*i] = (char)Src[i+1] - (char)Src[i];
         Dst[2*i+1] = (char)Src[i+W] - (char)Src[i];
      }
      Dst[2*i] = Dst[2*i-2];
      Dst[2*i+1] = Dst[2*i-1];

      Dst += 2*W; Src += W;
   }
   for( i=0; i<W; ++i ) Dst[i] = Dst[i-W];
}

/*****************************************************************/

#if 1 // #ifndef UNIX

EXTERN void Do_Da_Bump_I( PIXEL *Dst, INT Dst_BpS, PIXEL *Src, PIXEL *Env, 
   INT W, INT H, INT xo, INT yo )
{
   INT i, j, jo;
   xo += 128;
   yo += 128;
   for( j=0; j<H-1; ++j )
   {
      jo = yo-j;
      for( i=0; i<W-1; ++i )
      {
         INT x, y;
         x = Src[i+1]-Src[i];
         x += ( xo-i );
         x = CLAMP_256(x);
         y = Src[i+W]-Src[i];
         y += jo;         
         y = CLAMP_256(y);
         Dst[i] = Env[x+y*256];
      }
      Dst[i] = Dst[i-1];
      Dst += Dst_BpS;
      Src += W;
   }
   for( i=0; i<W; ++i ) Dst[i] = Dst[i-W];
}

EXTERN void Do_Da_Bump_II( PIXEL *Dst, INT Dst_BpS, PIXEL *Src, PIXEL *Env, 
   INT W, INT H, INT xo, INT yo )
{
}

#endif	//	UNIX

/*****************************************************************/

