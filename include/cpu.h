/***********************************************
 *          basic CPU detecting                *
 * Skal 96                                     *
 ***********************************************/

#ifndef _CPU_H_
#define _CPU_H_

#include "main.h"

#define _CPU_UNKNOWN_   0x0
#define _CPU_x86_       0x1
#define _CPU_OLD486_    0x2
#define _CPU_486_       0x3
#define _CPU_586_       0x4

extern UINT _CPU_, _CPU_Speed_;

extern UINT ASM_Detect_CPU( );   // in cpu.asm
extern UINT Detect_CPU( );
extern UINT Print_CPU_Specs( );

#endif   // _CPU_H_

