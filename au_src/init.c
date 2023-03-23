/***********************************************
 *      registers all available drivers        *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "audio.h"

/********************************************************************/

EXTERN void Register_All_Audio_Drivers( )
{

#if defined( LNX ) || defined( UNIX )
   Register_Audio_Driver( _OSS_DRIVER_ );
#elif defined( WIN32 )
   Register_Audio_Driver( _DSOUND_DRIVER_ );
#endif

}

/********************************************************************/
