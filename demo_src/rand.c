/***********************************************
 *       Rand utilities                        *
 * Skal 96                                     *
 ***********************************************/ 

#include "main.h"

EXTERN unsigned long int Seed = 969413051; 
EXTERN PIXEL Hash_Seed = 125;
EXTERN PIXEL Hash1[ 256 ], Rand_Tab[ 256 ];

/*******************************************************/

#if 0

EXTERN INLINE void My_SRand( unsigned long int i )
{
   Seed = ( i == 0 ? 873 : i ); 
}

EXTERN INLINE INT My_I_Rand( )
{
   Seed = 65539*Seed + 0xFE239A27; 
   return( Seed );
}

EXTERN INLINE double My_Rand( )
{
   Seed *= 65539; 
   return( (2.0*2.328306437081e-10)*Seed - 1.0 );
}

EXTERN INLINE PIXEL My_T_Rand( )
{
   Hash_Seed = ( Hash_Seed * 17 ) & 0xFF;
   return( Hash_Seed );
}

#endif

EXTERN void Init_Hash1( PIXEL Amp, PIXEL S )
{
   int i;

   if ( S == 0 ) S = 81;
   Hash_Seed = S;
   srandom( (unsigned int)S );
   for( i=0; i<256; ++i ) { Hash1[ i ] = i; Rand_Tab[i] = i%Amp; }

   for( i=0; i<500; ++i )
   {
      PIXEL j,k;
      j = ( random( ) >> 9 ) & 0xFF;
      k = ( random( ) >> 5 ) & 0xFF;
      if ( k==j ) continue;
      {
         PIXEL Swap = Hash1[ k ];
         Hash1[ k ] = Hash1[ j ];
         Hash1[ j ] = Swap;
      }
   }
}

/*******************************************************/
