/***********************************************
 *        Fake *itimer() funcs :(((            *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "demo_drv.h"

/********************************************************************/

#if defined( LNX ) || defined(__DJGPP__) || defined( UNIX )

static FLT Timer_f = 0.0;
static FLT Timer_o = 0.0;
#define SEC_MAX      1000
#define TIMING_MAX   ( SEC_MAX*1000000 )

#define TMODE  ITIMER_REAL
// #define TMODE  ITIMER_VIRTUAL
// #define TMODE  ITIMER_PROF

EXTERN void My_setitimer( ULINT Start )
{
   struct itimerval Val;
   Val.it_value.tv_sec = SEC_MAX;
   Val.it_value.tv_usec = 0;
   Val.it_interval.tv_sec = SEC_MAX;
   Val.it_interval.tv_usec = 0;
   setitimer( TMODE, &Val, 0 );
   Timer_o = TIMING_MAX;
   Timer_f = 1.0*Start;
}

EXTERN long My_getitimer( )
{
   FLT Timer;
   struct itimerval Val;

   getitimer( TMODE, &Val );

#if defined( __DJGPP__ )

   Timer = (FLT)Val.it_value.tv_usec*1000000.0/290.466;  // ?!?

#else

   Timer = (FLT)Val.it_value.tv_sec*1000000 + (FLT)Val.it_value.tv_usec;

#endif
   Timer = (Timer_o-Timer)/1000.0 + Timer_f;
//   fprintf( stderr, "%.3f %ld\n", Timer, (long)Timer );
   return( (long)Timer );
}

EXTERN void My_closeitimer( )
{
}

#endif   //    LNX

/********************************************************************/

#if 0	// #ifdef __DJGPP__

#include <bios.h>
static ULINT Timer_o = 0;

EXTERN void My_setitimer( ULINT Start )
{
   Timer_o = ( ULINT )biostime( _TIME_GETCLOCK, 0 );
   Timer_f = 1.0*Start;
}

EXTERN long My_getitimer( )
{
   ULINT Timer;
   FLT Timer2;

   Timer = (ULINT)biostime( _TIME_GETCLOCK, 0 );
   Timer2 = (FLT)(Timer-Timer_o)*(5000.0/91) + Timer_f;
   return( (long)Timer2 );
}

EXTERN void My_closeitimer( )
{
}

#endif   //    __DJGPP__

/********************************************************************/
/********************************************************************/

#if defined( __WATCOMC__ ) || defined( WIN32 )

#include <sys/timeb.h>
static long Timer_o = 0;
static long Timer_f = 0;

   // Warning: Timer.time can be very big. Year 2030 bug?? :)
EXTERN void My_setitimer( ULINT Start )
{
   struct timeb Time;

   ftime( &Time );
   Timer_o = 1000*(long)Time.time + (long)Time.millitm;
   Timer_f = (long)Start;
}

EXTERN long My_getitimer( )
{
   long Timer;
   struct timeb Time;

   ftime( &Time );
   Timer = 1000*(long)Time.time + (long)Time.millitm;
   Timer = Timer-Timer_o + Timer_f;
   return( (long)Timer );
}

EXTERN void My_closeitimer( )
{

}

#endif   //    __WATCOMC__

/********************************************************************/
/********************************************************************/

