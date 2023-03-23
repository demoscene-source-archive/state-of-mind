/***********************************************
 *      misc tables                            *
 *                                             *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "btm.h"
#include "bump.h"
#include <math.h>

EXTERN PIXEL I_Table[256+2*CLAMP_OFF];
EXTERN USHORT I_Table2[256+2*CLAMP_OFF];

extern double pow( double, double );

/*****************************************************************/

extern double pow( double, double );

EXTERN void Init_Saturation_Table( INT C0, INT C1, FLT Gamma )
{
   INT i;
   for( i=0; i<=CLAMP_OFF; ++i )
   {
      I_Table[i] = (BYTE)C0;
      I_Table[255+CLAMP_OFF+i] = (BYTE)C1;
   }
   if ( Gamma!=1.0 ) for( i=1; i<255; ++i )
   {
      FLT x = 1.0f*i/256.0f;
      x = (FLT)pow( (double)x, (double)Gamma );
      I_Table[CLAMP_OFF+i] = (BYTE)floor( C0 + ( x*(C1-C0) ) +.5 );
   }
   else for( i=1; i<255; ++i )
   {
      FLT x = 1.0f*i/256.0f;
      I_Table[CLAMP_OFF+i] = (BYTE)floor( C0 + ( x*(C1-C0) ) +.5 );
   }
}

EXTERN void Init_Saturation_Table2( INT C0, INT C1, FLT Gamma, INT W )
{
   INT i;
   for( i=0; i<=CLAMP_OFF; ++i )
   {
      I_Table2[i] = (USHORT)( W*C0 );
      I_Table2[255+CLAMP_OFF+i] = (USHORT)(W*C1);
   }
   if ( Gamma!=1.0 ) for( i=1; i<255; ++i )
   {
      FLT x = 1.0f*i/256.0f;
      x = (FLT)pow( (double)x, (double)Gamma );
      I_Table2[CLAMP_OFF+i] = W*(USHORT)floor( C0 + ( x*(C1-C0) + .5 ) );
   }
   else for( i=1; i<255; ++i )
   {
      FLT x = 1.0f*i/256.0f;
      I_Table2[CLAMP_OFF+i] = W*(USHORT)floor( C0 + ( x*(C1-C0) + .5 ) );
   }
}

EXTERN void Init_Saturation_Table3( 
   USHORT *Tab, INT C0, INT C1, FLT Gamma, INT W, INT Amp )
{
   INT i, j;

   for( i=0,j=0; i<=CLAMP_OFF; ++i, j++ )
      Tab[j] = (USHORT)(W*C0);
   if ( Gamma!=1.0 ) for( i=1; i<Amp; ++i, j++ )
   {
      FLT x = 1.0f*(FLT)i/(FLT)Amp;
      x = (FLT)pow( (double)x, (double)Gamma );
      Tab[j] = W*(USHORT)floor( ( C0 + ( x*(C1-C0) + .5 ) ) );
   }
   else for( i=1; i<Amp; ++i, j++ )
   {
      FLT x = 1.0f*(FLT)i/(FLT)Amp;
      Tab[j] = W*(USHORT)floor( ( C0 + ( x*(C1-C0) + .5 ) ) );
   }
   for( i=0; i<CLAMP_OFF; ++i, j++ ) Tab[j] = (USHORT)(W*C1);
}

EXTERN void Init_Saturation_Table4( 
   INT *Tab, INT C0, INT C1, FLT Gamma, INT W, INT Amp )
{
   INT i;

   Tab[0] = W*C0;
   if (Gamma!=1.0) for( i=1; i<Amp; ++i )
   {
      FLT x = 1.0f*(FLT)i/(FLT)Amp;
      x = (FLT)pow( (double)x, (double)Gamma );
      Tab[i] = W*(INT)floor( ( C0 + ( x*(C1-C0) + .5 ) ) );
   }
   else for( i=1; i<Amp; ++i )
   {
      FLT x = 1.0f*(FLT)i/(FLT)Amp;
      Tab[i] = W*(INT)floor( ( C0 + ( x*(C1-C0) + .5 ) ) );
   }
}

EXTERN void Init_Saturation_Table5( 
   INT *Tab, INT C0, INT C1, FLT Gamma, INT W, INT Off1, INT Amp, INT Off2 )
{
   INT i, j;
   FLT dx = 1.0f/(FLT)(Amp);

   for( i=j=0; i<=Off1; ++i, ++j ) Tab[j] = W * C0;
   if (Gamma!=1.0) for( i=1; i<Amp; ++i, ++j )
   {
      FLT x = dx*(FLT)i;
      x = (FLT)pow( (double)x, (double)Gamma );
      Tab[j] = W*(INT)floor( ( C0 + ( x*(C1-C0) + .5 ) ) );
   }
   else for( i=1; i<Amp; ++i, ++j )
   {
      FLT x = dx*(FLT)i;
      Tab[j] = W*(INT)floor( ( C0 + ( x*(C1-C0) + .5 ) ) );
   }
   for( i=0; i<Off2; ++i, ++j ) Tab[j] = W * C1;
}

/*****************************************************************/

