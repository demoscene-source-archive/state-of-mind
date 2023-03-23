/***********************************************
 *              Mem utilities                  *
 * Skal 96                                     *
 ***********************************************/

#ifndef _MEM_H_
#define _MEM_H_

#include "main.h"
#include <stdlib.h>

/********************************************************/

#if defined(__DJGPP__)

#define BASE_FP(s)    ( (UINT)(s)<<4 )
#define SEG_OFF_FP(s,o)    ( ((UINT)(s)<<4) + (o) )
#define MAKE_FP(s)    ( __djgpp_conventional_base + ( ((UINT)(s))<<4 ) )
#define MK_FP(s,o)    ( __djgpp_conventional_base + ( ((UINT)(s))<<4 ) + (UINT)(o) )

#endif

#if defined(__WATCOMC__)

#define BASE_FP(s)    ( (UINT)(s)<<4 )
#define SEG_OFF_FP(s,o)    ( ((UINT)(s)<<4) + (o) )
#define MAKE_FP(s)    ( ( ((UINT)(s))<<4 ) )

// already done in i86.h...
// #define MK_FP(s,o)    ( ( ((UINT)(s))<<4 ) + (UINT)(o) )


#endif

/********************************************************/

extern void *My_Malloc( int );
extern void *My_Realloc( void *Ptr, int );
extern void  My_Free( void * );
extern void *My_Fatal_Malloc( int );
extern void Print_Mem_State( );

/********************************************************/

#define M_Free(x)   { if ((x)!=NULL) My_Free((x)); (x)=NULL; }
#define New_Object(N,type) ( (type *)My_Malloc( (N)*sizeof(type) ) )
#define New_Fatal_Object(N,type) ( (type *)My_Fatal_Malloc( (N)*sizeof(type) ) )
#define Mem_Clear(P)     memset( (void *)(P), 0, sizeof( *(P) ) )

extern STRING Str_Dup( STRING In );

extern PIXEL *Malloc_64( PIXEL **Real_Bits, PIXEL **Bits, INT Size );

/********************************************************/

#endif   // _MEM_H_

