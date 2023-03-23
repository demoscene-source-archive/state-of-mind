/***********************************************
 *       Rand utilities                        *
 * Skal 96                                     *
 ***********************************************/ 

#ifndef _RAND_H_
#define _RAND_H_

#include "main.h"

extern unsigned long int Seed;
extern PIXEL Hash1[ ], Rand_Tab[ ], Hash_Seed;
extern void Init_Hash1( PIXEL Amp, PIXEL Seed );
extern PIXEL My_T_Rand( );
extern void My_SRand( unsigned long int i );
extern INT My_I_Rand( );
extern double My_Rand( );
#define Low_Rand    ( Rand_Tab[ Hash_Seed = Hash1[ Hash_Seed ] ] )

#endif /* _RAND_H_ */
