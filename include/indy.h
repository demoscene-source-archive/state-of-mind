/***********************************************
 *         @%&*(@#$&* Indians                  *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _INDY_H_
#define _INDY_H_

/******************************************************************/

#ifndef UNIX

#define Indy_s(x)    (x)
#define Indy_l(x)    (x)
#define Indy_f(x)    (x)

#else

extern USHORT Indy_s( USHORT x );
extern UINT Indy_l( UINT x );
extern FLT Indy_f( FLT x );

#endif

#endif   // _INDY_H_
