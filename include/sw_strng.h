/*********************************************
 *                                           *
 *    sw_string.h                            *
 *                                           *
 *                                           *
 * Skal widgets v0.0 (c)1997                 *
 * Pascal.Massimino@ens.fr                   *
 *********************************************/

#ifndef _SW_STRING_H_
#define _SW_STRING_H_

#include "sw_main.h"

/***************************************************************/

#define MAX_STRING_LEN  256
#define SW_HUGE_VALUE   1.0e14

#define New_String(l)   ( (STRING)New_Object( (l)+1, char ) )

extern UINT Str_Len( STRING String );
extern STRING Copy_String( STRING String );
extern STRING Transfert_String( STRING Dest, STRING Src );
extern UINT Fill_String( STRING Dest, UINT Len );

/***************************************************************/

#endif   /* #define _SW_STRING_H_ */
