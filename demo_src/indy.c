/***********************************************
 *         @%&*(@#$&* Indians                  *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main.h"
#include "indy.h"

#ifdef UNIX

/******************************************************************/

EXTERN USHORT Indy_s( USHORT x )
{
#ifdef DO_INDY_SWAP
   PIXEL Tmp, *Ptr;
   Ptr = (PIXEL *)&x;
   Tmp = Ptr[0]; Ptr[0] = Ptr[1]; Ptr[1] = Tmp;
#endif
   return( x );
}

EXTERN UINT Indy_l( UINT x )
{
#ifdef DO_INDY_SWAP
   USHORT *Ptr, Tmp;
   Ptr = (USHORT *)&x;
   Tmp = Indy_s( Ptr[0] ); Ptr[0] = Indy_s( Ptr[1] ); Ptr[1] = Tmp;
#endif
   return( x );
}

EXTERN FLT Indy_f( FLT x )  
{
#ifdef DO_INDY_SWAP
   UINT Tmp;   
   Tmp = *(UINT *)(&x);
   Tmp = Indy_l( Tmp );
   x = *(FLT *)(&Tmp);
#endif
   return( x );
}

#endif   // UNIX 
