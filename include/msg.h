/***********************************************
 *        message printing                     *
 * Skal 96                                     *
 ***********************************************/

#ifndef _MSG_H_
#define _MSG_H_

#include <stdarg.h>
#include <stdio.h>

#ifndef SKL_LIGHT_CODE

extern FILE *_Std_Out_;
extern FILE *_Std_Err_;
extern void *P_Error( const char *s );

#else

#define _Std_Out_ stdout
#define _Std_Err_ stderr
#define P_Error(s)   Out_Error( (s) )

#endif   // SKL_LIGHT_CODE

extern void *Out_String( const char *Format, ... );
extern void Exit_Upon_Mem_Error( const char *, INT );
extern void *Out_Message( const char *Format, ... );
extern void *Out_Error( const char *Format, ... );
extern void Exit_Upon_Error( const char *Format, ... );

#endif   // _MSG_H_
