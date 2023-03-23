/***********************************************
 *        message printing                     *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include <errno.h>

#ifndef SKL_LIGHT_CODE

EXTERN FILE *_Std_Out_ = stdout;
EXTERN FILE *_Std_Err_ = stderr;

#endif

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
#endif

/*******************************************************************/

EXTERN void Exit_Upon_Mem_Error( const char *Str, INT Size )
{
#ifndef WIN32
#ifndef SKL_LIGHT_CODE
   if ( Str == NULL ) fprintf( _Std_Err_, "Malloc Failure !\n" );
   else if ( Size ) fprintf( _Std_Err_, "Can't malloc %d bytes for '%s'.\n",
      Size, Str );
   else fprintf( _Std_Err_, "No more mem for '%s'.\n", Str );
#else
   fprintf( _Std_Err_, "Malloc Failure !\n" );
#endif

#else
   MessageBox( NULL, Str, "Malloc failure", MB_OK|MB_ICONWARNING );
   PostQuitMessage( 0 );
#endif
   exit( 1 );
}

/***********************************************************************/

EXTERN void *Out_Error( const char *Format, ... )
{
   va_list Arg_List;

   if ( Format==NULL ) return( NULL );
#ifndef SKL_LIGHT_CODE
   if ( _Std_Err_ == NULL ) _Std_Err_ = stderr;
#endif

   fprintf( _Std_Err_, "Error:" );
   va_start( Arg_List, Format );
   vfprintf( _Std_Err_, Format, Arg_List );
   va_end( Arg_List );

   fprintf( _Std_Err_, "\n" );
   return( NULL );
}

EXTERN void Exit_Upon_Error( const char *Format, ... )
{
   va_list Arg_List;

   if ( Format==NULL ) exit(1);
#ifndef SKL_LIGHT_CODE
   if ( _Std_Err_ == NULL ) _Std_Err_ = stderr;
#endif
   va_start( Arg_List, Format );

#if !defined( WIN32 )   // || defined( SKL_LIGHT_CODE )

   fprintf( _Std_Err_, "Error:" );
   vfprintf( _Std_Err_, Format, Arg_List );
   va_end( Arg_List );
   fprintf( _Std_Err_, "\n" );

#else       // WIN32

   if ( Format!=NULL )
   {
      char S[2048];
      vsprintf( S, Format, Arg_List );
      MessageBox( NULL, S, "Error", MB_OK|MB_ICONWARNING );
      va_end( Arg_List );
      PostQuitMessage( 0 );
   }
#endif

   exit( 1 );
}

#ifndef SKL_LIGHT_CODE
EXTERN void *P_Error( const char *s )
{
   if ( s==NULL ) return( NULL );
   if ( _Std_Err_ == NULL ) return( NULL );
   fprintf( _Std_Err_, "Error in %s(): %s\n", s, strerror( errno ) );
   fflush( _Std_Err_ );
   return( NULL );
}
#endif // SKL_LIGHT_CODE

/***********************************************************************/

EXTERN void *Out_String( const char *Format, ... )
{
   va_list Arg_List;

   if ( Format==NULL ) return( NULL );
#ifndef SKL_LIGHT_CODE
   if ( _Std_Out_ == NULL ) return( NULL );
#endif

   va_start( Arg_List, Format );

#if !defined( WIN32 )   // || defined( SKL_LIGHT_CODE )

   vfprintf( _Std_Out_, Format, Arg_List );
   fflush( _Std_Out_ );

#else       // WIN32
   {
      char S[2048];
      vsprintf( S, Format, Arg_List );
      MessageBox( NULL, S, "@#%!", MB_OK|MB_ICONWARNING );
   }
#endif

   va_end( Arg_List );
   return( NULL );
}

EXTERN void *Out_Message( const char *Format, ... )
{
   va_list Arg_List;
   if ( Format==NULL ) return( NULL );
#ifndef SKL_LIGHT_CODE
   if ( _Std_Out_ == NULL ) return( NULL );
#endif

   va_start( Arg_List, Format );

#if !defined( WIN32 )   // || defined( SKL_LIGHT_CODE )
   vfprintf( _Std_Out_, Format, Arg_List );
   fprintf( _Std_Out_, "\n" ); 
   fflush( _Std_Out_ );
#else       // WIN32
   {
      char S[2048];
      vsprintf( S, Format, Arg_List );
      MessageBox( NULL, S, "@#%!", MB_OK|MB_ICONWARNING );
   }
#endif

   va_end( Arg_List );
   return( NULL );
}

/***********************************************************************/
