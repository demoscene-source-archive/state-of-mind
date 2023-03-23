/***********************************************
 *             Any format saver                *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "sv_any.h"

/*******************************************************/

EXTERN void Save_Any( STRING Name, BITMAP *Btm )
{
   INT L;
   STRING Ptr;

   L = strlen( Name );
   if ( L<3 ) goto Do_Gif;
   Ptr = Name + L-3;
   if ( !strcmp( Ptr, "gif" ) || !strcmp( Ptr, "GIF" ) )
      goto Do_Gif;
   else if ( !strcmp( Ptr, "ppm" ) || !strcmp( Ptr, "PPM" ) )
      goto Do_PPM;
   else if ( !strcmp( Ptr, "btm" ) || !strcmp( Ptr, "BTM" ) )
      goto Do_BTM;
   else if ( !strcmp( Ptr, "raw" ) || !strcmp( Ptr, "RAW" ) )
      goto Do_RAW;
   else Out_Error( " unrecognized out format for '%s'", Name );
   return;

Do_BTM:
   Save_BTM( Name, Btm );
   Out_Message( " Saved '%s' BTM file", Name );
   return;
Do_PPM:
   Save_PPM( Name, Btm, Btm->Pal );
   Out_Message( " Saved '%s' PPM file", Name );
   return;
Do_Gif:
   Save_GIF( Name, Btm );
   Out_Message( " Saved '%s' GIF file", Name );
   return;
Do_RAW:
   Save_RAW( Name, Btm, Btm->Pal );
   Out_Message( " Saved '%s' RAW file", Name );
   return;
}

/*******************************************************/

