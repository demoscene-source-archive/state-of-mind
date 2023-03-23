/*
 *  old junk
 *
 * Skal 96
 ***************************************/

#include "demo.h"

/**************************************************************************/
/**************************************************************************/

      // Old junk

/**************************************************************************/
/**************************************************************************/

#if 0

UINT Main_Table[ 4*512 ] = { 0 };      /* 8K should be enough... */
EXTERN void Init_Plasma_1( )
{
   int i;
   PIXEL *Ptr;
   SHORT *Ptr2;

   Ptr = ( PIXEL *)Main_Table;
   Ptr2 = ( SHORT *)&( Ptr[ 512 ] );
   for( i=0; i<256; ++i )
   {
      double x, Tmp, Phi;
      x = (double)i/256.0;
      Phi = x*2.0*M_PI;

      Tmp = ( sin( Phi * 1.0 ) + 1.0 )*255.9/2.0;
      Ptr[i] = (PIXEL)(Tmp*2);
      Tmp = ( sin( Phi * 1.0 ) + 1.0 )*255.9/2.0;
      Ptr2[i] = (SHORT)(127-Tmp/2);
      Tmp = ( cos( Phi * 0.0 ) + 1.0 )*255.9/2.0;
      Ptr[i+256] = (PIXEL)(Tmp*2);
      Tmp = ( cos( Phi * 1.0 + 1.2212 ) + 1.0 )*255.9/2.0;
      Ptr2[i+256] = (SHORT)Tmp/2;
   }
}

EXTERN void Init_Plasma_2( )
{
   int i;
   PIXEL *Ptr;
   SHORT *Ptr2;

   Ptr = ( PIXEL *)Main_Table;
   Ptr2 = ( SHORT *)&( Ptr[ 512 ] );
   for( i=0; i<256; ++i )
   {
      double x, Phi, Tmp;
      x = (double)i/256.0;
      Phi = x*2.0*M_PI;

      Tmp = ( sin( Phi * 1.0 ) + 1.0 )/2.0;
      Tmp = Tmp*255.5;
      Ptr[i] = (PIXEL)Tmp;

      Tmp = ( sin( Phi * 1.0 ) + 1.0 )*(255.5)/2.0;
      Ptr2[i] = (SHORT)(Tmp);

      Tmp = ( sin( Phi * 1.0 ) + 1.0 )*(255.5)/2.0;
      Ptr[i+256] = (PIXEL)Tmp;

      Tmp = ( cos( Phi * 1.0 ) + 1.0 )*(255.5)/2.0;
      Ptr2[i+256] = (SHORT)Tmp/2;

   }
}

EXTERN void Init_Plasma_3( )
{
   int i;
   PIXEL *Ptr;
   SHORT *Ptr2;

   Ptr = ( PIXEL *)Main_Table;
   Ptr2 = ( SHORT *)&( Ptr[ 512 ] );
   for( i=0; i<256; ++i )
   {
      double x, Tmp, Phi;
      x = (double)i/256.0;
      Phi = x*2.0*M_PI;

      Tmp = ( sin( Phi*6.0 ) + 2.0 )*255.9/2.0;
      Ptr2[i] = (SHORT)(Tmp);

      Tmp = ( cos( Phi*7.0 - .706 ) + 1.0 )*255.9/2.0;
      Ptr2[i+256] = (SHORT)(Tmp/2);
   }
}

#endif      // 0

/**************************************************************************/

#if 0

EXTERN UINT *Build_Norm_Tab( int THE_W, int THE_H, double Zo )
{
   int i, j;
   UINT *Norm_Tab;
   char *Ptr1;

   i = THE_W*2*THE_H*2*sizeof( UINT );
   Norm_Tab = ( UINT * )My_Malloc( i );
   if ( Norm_Tab == NULL ) Exit_Upon_Mem_Error( "Norm_Tab", i );

   Ptr1 = (char *)Norm_Tab;
   for( j=0; j<2*THE_H; ++j )
   {
      for( i=0; i<2*THE_W; ++i )
      {
         double Norm, x, y, z;

         x = (double)(i-THE_W);   
         y = (double)(j-THE_H);
         z = Zo;
         Norm = 127.9/sqrt( x*x + y*y + z*z );
         Ptr1[0] = (char)( x*Norm );
         Ptr1[1] = (char)( y*Norm );
         Ptr1[2] = (char)( z*Norm );
         Ptr1 += 4;
      }
   }

   return( Norm_Tab );
}

#endif   // 0

/**************************************************************************/
