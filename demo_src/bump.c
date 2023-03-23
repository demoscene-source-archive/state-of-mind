/***********************************************
 *      Bump map utilities                     *
 *                                             *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "btm.h"
#include <math.h>

/*****************************************************************/

EXTERN SHORT *Build_Mult_Tab( )
{
   INT i, j;
   SHORT *Mult_Tab;
   SHORT *Ptr1;

   Mult_Tab = New_Object( 256*256, SHORT );
   if ( Mult_Tab == NULL ) return( NULL );

   Ptr1 = Mult_Tab;
   for( j=0; j<256; ++j )
   {
      for( i=0; i<256; ++i )
         *Ptr1++ = (char)(i)*(char)(j);
   }
   return( Mult_Tab );
}

/*****************************************************************/

EXTERN BITMAP *Do_The_Emboss( BITMAP *BTM, PIXEL Mask )
{
   INT Width, Height, W;
   int x, y;
   BITMAP *ZMP;
   PIXEL *Src;
   char *Dst;

      /* Makes ZMP out of 16bits bitmap (Height:8 + Color: 8) */

   W = BTM->Width;
   Width = W/2;
   Height = BTM->Height;
   ZMP = New_Bitmap( 2*W, Height, BTM->Nb_Col ); /* XXX */
   memcpy( BTM->Pal, ZMP->Pal, BTM->Nb_Col*3 );
   Src = BTM->Bits;
   Dst = (char*)( ZMP->Bits );

   for( y=Height-1; y>0; --y )
   {
      for( x=Width-1; x>0; --x )
      {
         int h1, h2;
         double Nx, Ny, Norm;

         h1 = Src[ W+2 ] - Src[ 0 ];
         h2 = Src[ W ] - Src[ 2 ];

         Nx = ( h1 + h2 ) / 32.0;
         Ny = ( h2 - h1 ) / 32.0;

         Norm = 127.5/sqrt( Nx*Nx + Ny*Ny + 1.0 );

         Dst[0] = (char)( Nx*Norm );       /* normal */
         Dst[1] = (char)( Ny*Norm );    
         Dst[2] = (char)( Norm );
         Dst[3] = (char)( Src[1] & Mask ); /* color */
         Dst += 4;
         Src += 2;
      }
      /* Pad last row */
      Dst[0] = Dst[-4];
      Dst[1] = Dst[-3];
      Dst[2] = Dst[-2];
      Dst[3] = ( char )( Src[1] & Mask );
      Dst += 4;
      Src += 2;
   }
   W *= 2;
   for( x=Width; x>0; --x )
   { 
      Dst[0] = Dst[-W+0];
      Dst[1] = Dst[-W+1];
      Dst[2] = Dst[-W+2];
      Dst[3] = ( char )( Src[1] & Mask );
      Dst += 4;
      Src += 2;
   }
   /* BTM should be useless now. Everything's in ZMP */
   return( ZMP );
}

/*****************************************************************/



