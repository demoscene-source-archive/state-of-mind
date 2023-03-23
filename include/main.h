/***********************************************
 *              Main header                    *
 * Skal 96                                     *
 ***********************************************/ 

#ifndef _MAIN_H_
#define _MAIN_H_

#define EXTERN

#ifndef __WATCOMC__
#define INLINE    // inline
#else 
#define INLINE
#endif

#ifndef FALSE
#define FALSE     0
#endif
#ifndef TRUE
#define TRUE      1
#endif
#ifndef NULL
#define NULL		0
#endif

typedef void               VOID;
typedef unsigned char      BYTE;
typedef char              *STRING;
typedef unsigned char      PIXEL;
typedef short int          SHORT;
typedef unsigned short int USHORT;
typedef double             DBL;
typedef float              FLT;

typedef unsigned int       UINT;
typedef int                INT;
typedef unsigned long int  ADDR;

   // for the gfx lib...
typedef PIXEL COLOR_ENTRY[4];
typedef FLT FCOLOR[4];  // Red, green, blue, alpha in [0.0,1.0]
#define BLUE_F           0
#define GREEN_F          1
#define RED_F            2
#define ALPHA_F          3
#define INDEX_F          3
#define COL_BITS_FIX     8
#define POS_PACKED       0x00000000


#ifdef __WATCOMC__

typedef long int           LINT;
typedef unsigned long int  ULINT;
#define random() rand()
#define srandom(s) srand((s))

#elif defined( WIN32 )

typedef long int          LINT;
typedef unsigned long int ULINT;
   // RAND_MAX is 0x7fff for Windows!!!
#define random() (rand()^(rand()<<13))
#define srandom(s) srand((s))

#else

typedef long long int          LINT;
typedef unsigned long long int ULINT;

#endif

#include "config.h"

      // I/O standard and custom

//#include <string.h>
#include <time.h>

#include "msg.h"
#include "mem.h"

#include <math.h>

#ifndef M_PI
#define M_PI   (3.1415926535f)
#endif

/***************************************************************/
/***************** Target-specific #defines ... ****************/
/***************************************************************/

extern ULINT _Clock( );  // in clock.c

#ifdef USE_RDTSC

extern INT _RCount_[16];
extern INT _Tics_, _EAX_Save_;
extern void _Print_Tics_( );

#endif   // USE_RDTSC

      /**************** UNIX *******************/

#if defined(UNIX)

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000000
#endif

#define memmove(a,b,c)     bcopy((b),(a),(c))
#define BZero(P,l)   bzero( (P),(l) )
#define MemCopy(D,S,l) memcpy( (D), (S), (l) )
#define F_MemCopy(D,S,l,L) memcpy( (D), (S), (l) )

#ifndef SEEK_SET     // in <stdio.h>...

#define SEEK_SET  0
#define SEEK_CUR  1
#define SEEK_END  2

#endif   // SEEK_*

#if 0

extern char *sys_errlist[];
#define strerror(e)  (sys_errlist[ (e) ])

#ifndef RAND_MAX
#define RAND_MAX (DBL)( ((long long int)1<<32)-1 )
#endif

#endif   // 0


#endif

      /**************** WATCOM *******************/

#if defined(__WATCOMC__)

#define bzero(P,l)   memset( (void*)(P),0,(l) )
#define BZero(P,l)   bzero( (P),(l) )
#define MemCopy(D,S,l) memcpy( (D), (S), (l) )
#define F_MemCopy(D,S,l,L) memcpy( (D), (S), (l) )

#endif

      /****************  WIN32 *******************/

#if defined(WIN32)

#define bzero(P,l)   memset( (void*)(P),0,(l) )
#define BZero(P,l)   bzero( (P),(l) )
#define MemCopy(D,S,l) memcpy( (D), (S), (l) )
#define F_MemCopy(D,S,l,L) memcpy( (D), (S), (l) )

#endif

      /**************** LINUX *******************/

#ifdef LNX

#define memmove(a,b,c)     bcopy((b),(a),(c))

#endif

      /**************** DOS || LINUX *******************/

#if defined( LNX ) || defined(__DJGPP__)

#if 0
#define _Clock(T)                \
      asm( "\t.byte 0x0F\n"      \
           "\t.byte 0x31\n"      \
           "\tmovl %%eax,%0\n"   \
           : "=g" ( (T) )        \
           :                     \
           : "eax", "edx" )
#endif

#define BZero(P,l)          \
asm( "\tmovl %0,%%edi\n"    \
     "\txorl %%eax,%%eax\n" \
     "\tmovl %1,%%ecx\n"    \
     "\tcld\n"              \
     "\trep\n"              \
     "\tstosl\n"            \
     :                      \
     : "g" (P), "g" ((l)/4) \
     : "ecx", "edi", "eax" )

#define MemCopy(D,S,l)               \
asm( "\tmovl %0,%%edi\n"             \
     "\tmovl %1,%%esi\n"             \
     "\tmovl %2,%%ecx\n"             \
     "\tcld\n"                       \
     "\trep\n"                       \
     "\tmovsl\n"                     \
     :                               \
     : "g" (D), "g" (S), "g" ((l)/4) \
     : "ecx", "edi", "esi" )

   // 20% faster using FPU. Size must be 16 multiple. 
   // Dst must not be in the cache. Dst and Src must be 8-aligned
#define F_MemCopy(D,S,l,Lbl)         \
asm( "\tmovl %0,%%edi\n"             \
     "\tmovl %1,%%esi\n"             \
     "\tmovl %2,%%ecx\n"             \
     "\tmovl $16,%%eax\n"            \
     Lbl "_1:\n"                     \
     "\tfildll (%%esi)\n"            \
     "\tfildll 8(%%esi)\n"           \
    "\tfxch\n"                       \
     "\taddl %%eax,%%esi\n"          \
     "\tfistpll (%%edi)\n"           \
     "\tfistpll 8(%%edi)\n"          \
     "\taddl %%eax,%%edi\n"          \
     "\tdecl %%ecx\n"                \
     "\tjg " Lbl "_1\n"             \
     :                               \
     : "g" (D), "g" (S), "g" ((l)/16) \
     : "eax", "ecx", "edi", "esi" )

#endif

/***************************************************************/

#endif // _MAIN_H_

