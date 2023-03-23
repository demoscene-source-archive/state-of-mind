/***********************************************
 *      Z-Blur utilities                       *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "zblur.h"
#include "mix_tab.h"

extern double exp( double );
extern double fabs( double );
extern double pow( double, double );
extern double floor( double );

/*****************************************************************/

EXTERN UINT *Init_ZBlur_Index( INT Z_Max, FLT Focal, FLT Gamma, UINT **Z_Index, INT Levels )
{
   INT n;

   if ( (Levels>256) || (Levels<=0) ) return( NULL );

   if ( *Z_Index==NULL )
      *Z_Index = New_Fatal_Object( Z_Max, UINT );

   for( n=0; n<Z_Max; ++n )
   {
      FLT z;
      UINT Err;
      UINT A;

      z = 1.0f*n/Z_Max;
      if ( z<Focal ) z = ( Focal-z ) / Focal;
      else z = ( z-Focal ) / ( 1.0f-Focal );
      if ( z!=0.0 ) z = (FLT)pow( z, Gamma );  // z_blur value. in [0,1];
      z *= Levels;
      A = (UINT)floor(z);
      z = ( z - 1.0f*A ) * 256.0f / 4.0f;
      Err = (UINT)z;    // <= Magik
//      fprintf( stderr, "z=%f  A=0x%x Err=0x%x\n", z, A, Err );
      (*Z_Index)[n] = (A<<16) | Err;      
   }
   return( *Z_Index );
}

#if 0

EXTERN PIXEL *Init_ZBlur_Tables( 
   INT Z_Max, INT Levels,
   PIXEL **W_Tab, COLOR_ENTRY *Col_Tab, INT Nb )
{
   INT i, j, k, n;
   FLT F_Wghts[256];
   INT Wghts[256];
   INT Matrix[256][4];
   FLT F_Matrix[256][4];
   INT Max, Last, Cur;

   if ( (Levels>256) || (Levels<=0) ) return( NULL );

   for( n=0; n<Levels; ++n )
   {
      FLT All, Off;
      FLT z, Fall_Off;

      z = 1.0*n/Levels;

      Fall_Off = 1.9*4.0*z + 1.0;
      Fall_Off = -1.0/Fall_Off;
      All = 0.0;

      for( j=0; j<2; ++j )
      {
         FLT y;
         y = 1.0*j;
         for( i=0; i<2; ++i )
         {
            FLT x, r2;
            x = 1.0*i;
            r2 = x*x + y*y;
            r2 = r2*Fall_Off;
            r2 = exp( r2 );
            F_Matrix[n][i+j*2] = r2;
            if ( r2>All ) All=r2;
            // All += r2;
         }
      }
      All = 1.0 / All; Off = 0.0;
      // Off = z*.5; All *= ( 1.0-z );
      fprintf( stderr, "Matrix #%d. z=%f Fall_Off=%f Off=%f All=%f\n", 
         n, z, Fall_Off, Off, All );
      for( j=0; j<2; ++j )
      {
         for( i=0; i<2; ++i )
         {
            Matrix[n][i+j*2] = (PIXEL)( 
               ( Off + All*F_Matrix[n][i+j*2] )*255.0 );
            fprintf( stderr, "[%d]", Matrix[n][i+j*2] );
         }
         fprintf( stderr, "\n" );
      }
      fprintf( stderr, "--------------------\n" );
   }  

   for( i=0; i<256; ++i ) Wghts[i] = 0;
   Max = 0;
   for( n=0; n<Levels; ++n )
      for( k=0; k<4; ++k )
      {
         Wghts[ Matrix[n][k] ] = 1;
         if ( Matrix[n][k]>Max ) 
            Max = Matrix[n][k];
      }

   Last = 0;
   for( n=0; n<256; ++n )
   {
      if ( Wghts[ n ] )
      {
         F_Wghts[ Last ] = 1.0*n/Max;
         for( i=0 ; i<Levels; ++i )
            for( j=0; j<4; ++j )
               if ( Matrix[i][j]==n )
                  Matrix[i][j] = Last;
         fprintf( stderr, "Weights[%d]=%f <=> %d\n",
            Last, F_Wghts[Last], n );
         Last++;
      }
   }
   Last--;     // last level isn't used
   F_Wghts[Last] = 0.0;
   fprintf( stderr, "Max=%d  Last=%d\n", Max, Last );

   if ( (W_Tab!=NULL) && (*W_Tab==NULL) )
      *W_Tab = Build_Weight_Tab( Col_Tab, F_Wghts, Nb, Last );

   for( k=0; k<Levels; ++k )
   {
      fprintf( stderr, "Level #%d:  [ ", k );
      for( j=0; j<4; ++j )
      {
         fprintf( stderr, "%d ", Matrix[k][j] );
      }
      fprintf( stderr, "]\n" );
   }

   return( *W_Tab );
} 
#endif

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

