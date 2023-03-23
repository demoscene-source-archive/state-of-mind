/***********************************************
 *      _Clock() Pentium time stamp            *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"

/*****************************************************************/

#ifdef USE_RDTSC

EXTERN INT _RCount_[16] = {0};
EXTERN INT _Tics_, _EAX_Sv_, _EBX_Sv_, _ECX_Sv_, _EDX_Sv_;
EXTERN INT _EDI_Sv_, _ESI_Sv_, _EBP_Sv_, _ESP_Sv_;
EXTERN INT _Extra1_, _Extra2_, _Extra3_, _Extra4_;
EXTERN double _f1_,_f2_,_f3_,_f4_,_f5_,_f6_,_f7_,_f8_; 
#include <stdio.h>

void _Print_Tics_( )
{
   INT i;
   fprintf( stderr, "Counters:\n" );
   for( i=0; i<16; ++i ) fprintf( stderr, "[%d]", _RCount_[i] );
   fprintf( stderr, "\n" );
   fprintf( stderr, "EAX:0x%.8x   EBX:0x%.8x   ECX:0x%.8x   EDX:0x%.8x\n",
      _EAX_Sv_, _EBX_Sv_, _ECX_Sv_, _EDX_Sv_ );
   fprintf( stderr, "ESI:0x%.8x   EDI:0x%.8x   EBP:0x%.8x   ESP:0x%.8x\n",
      _ESI_Sv_, _EDI_Sv_, _EBP_Sv_, _ESP_Sv_ );
   fprintf( stderr, "st0:%lf  st1:%.lf  st2:%.lf  st3:%.lf\n", _f1_, _f2_, _f3_, _f4_ );
   fprintf( stderr, "st4:%lf  st5:%.lf  st6:%.lf  st7:%.lf\n", _f5_, _f6_, _f7_, _f8_ );
   fprintf( stderr, "W1:0x%.8x    W2:0x%.8x    W3:0x%.8x    W4:0x%.8x\n",
      _Extra1_, _Extra2_, _Extra3_, _Extra4_ );
   exit(1);
}

#endif

/*****************************************************************/

#if defined( LNX ) || defined(__DJGPP__)

EXTERN ULINT _Clock( )
{
   ULINT T;
   asm( "\t.byte 0x0F\n"
      "\t.byte 0x31\n"  
      "\tmovl %%eax,%0\n"
      : "=g" ( (T) )     
      :                 
      : "eax", "edx" );
   return( T );
}

#endif

/*****************************************************************/

#if defined( UNIX ) || defined( __WATCOMC__ )
EXTERN ULINT _Clock( )
{
   return( 0 );
}
#endif
 
/*****************************************************************/
