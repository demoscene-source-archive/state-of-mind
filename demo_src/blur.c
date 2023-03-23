/***********************************************
 *      Blur utilities                         *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"

/*****************************************************************/

extern double exp( double );

// #define PRINT_TAB

EXTERN void Init_Blur_Map_I( PIXEL *Tab, INT Size, INT Levels, 
   INT Amp, INT Offset )
{
   INT i, l;
   FLT Ao, So;

   if ( (Levels<=0) || (Amp<=0) || (Size<=0) )
   {
      memset( Tab, 0, Levels*Size );
      return;
   }
   Tab[ 0*Size + Size/2 ] = (PIXEL)(Offset+Amp);
   for( l=1; l<Levels-1; ++l )
   {
      FLT x, A;
      
      x = (FLT)l/(Levels-1);            // in [0,1]
      Ao = (FLT)Amp*x;
      So = 15.0f*x*x; // /(FLT)Size;
      So = So*So;
      A = (FLT)exp( -So );
#ifdef PRINT_TAB
      printf( "--%d--\n", l );
#endif
      for( i=0; i<Size; ++i )
      {
         FLT y, Gauss;
         y = (FLT)(i-Size/2) / ( Size/2 );      // in [-1,1]         
         Gauss = Ao*( (FLT)exp( -y*y*So ) - A ) / (1.0f-A);
         Tab[i + l*Size] = Offset + (PIXEL)Gauss;
#ifdef PRINT_TAB
         printf( "[%d->%d]   ", i, Tab[i+l*Size] );
#endif
      }
      Tab[ l*Size + Size/2 ] = (PIXEL)(Offset+Amp);
#ifdef PRINT_TAB
      printf( "\n" );
#endif
   }
}

/*****************************************************************/

#if 0
EXTERN void Init_Blur_Map_II( PIXEL *Tab, INT Size, INT Levels, 
   INT Amp, INT Offset )
{
   INT i, l;
   FLT Ao, So, x, Co;

   if ( (Levels<=0) || (Amp<=0) || (Size<=0) )
   {
      bzero( Tab, Levels*Size );
      return;
   }

   for( l=1; l<Levels; ++l )
   {
      FLT x, A;
      
      x = (FLT)l/(Levels-1);            // in [0,1]
      Ao = (FLT)Amp*x / 2.0;
      So = 15.0*x*x; // /(FLT)Size;
      So = So*So;
      Co = (1.0-x);
      A = exp( -(1.0+Co)*(1.0+Co)*So );

#ifdef PRINT_TAB
      printf( "--%d--[Co=%f] [Ao=%f] [So=%f]\n", l, Co, Ao, So );
#endif

      for( i=0; i<Size; ++i )
      {
         FLT y, yo, Gauss;

         yo = (FLT)(i-Size/2) / ( Size/2 );      // in [-1,1]
         y = yo+Co; Gauss = ( exp( -y*y*So ) - A ) / (1.0-A);
         y = yo-Co; Gauss += ( exp( -y*y*So ) - A ) / (1.0-A);

         Gauss = Ao*Gauss;
         Tab[i + l*Size] = Offset + (PIXEL)Gauss;
#ifdef PRINT_TAB
         printf( "[%d]", (PIXEL)Gauss );
#endif
      }
#ifdef PRINT_TAB
      printf( "\n " );
#endif
   }
}
#endif

/*****************************************************************/
