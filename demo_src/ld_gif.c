/***********************************************
 *              GIF loader                     *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "ld_any.h"
#include "lzw.h"

/*******************************************************/

EXTERN BITMAP *Load_GIF( char *Name )
{
   BITMAP *Bitmap;
   int i, Finished;
   INT Nb_Col;
   INT Height, Width;
   FILE *In;
   PIXEL Buffer[ 16 ];
   PIXEL CMap[ 768 ];  // should be enough...

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

   F_READ( Buffer, In, 13 );
   if ( strncmp( (char *)Buffer, "GIF", 3 ) )
#ifndef SKL_LIGHT_CODE
      Exit_Upon_Error( "'%s' is not in GIF format", Name );
#else
      goto Failed;
#endif
   if ( Buffer[3] != '8' || ( Buffer[4] != '7' && Buffer[4] != '9' )
        || Buffer[5]<'A' || Buffer[5]>'z' )
   {
#ifndef SKL_LIGHT_CODE
      Exit_Upon_Error( "Unsupported GIF version %c%c%c", Buffer[3], Buffer[4], Buffer[5] );
#else
      goto Failed;
#endif
   }
   if ( !(Buffer[10]&0x80) ) 
#ifndef SKL_LIGHT_CODE
      Exit_Upon_Error( "GIF color map error" );
#else
      goto Failed;
#endif

   Nb_Col = 1 << ( (Buffer[10]&0x0F) + 1 );
   F_READ( CMap, In, 3*Nb_Col );   // tmp storage of Cmap

   Finished = FALSE;
   Bitmap = NULL; 
   while( !Finished )
   {
      char C;
      F_READ( &C, In, 1 );
      switch( C )
      {
         case '!':   // Extension block
            F_READ( &C, In, 1 );    // ID
            while( 1 )
            {
               F_READ( &C, In, 1 );
               if ( C<=0 ) break;
               for( i=C; i; --i ) F_READ( &C, In, 1 );
            }
         break;

         case ',':   // Image
            F_READ( Buffer, In, 9 );
            if ( Buffer[8]&0x40 ) 
#ifndef SKL_LIGHT_CODE
               Exit_Upon_Error( "Interlaced GIF not supported" );
#else
      goto Failed;
#endif

            Width  = Buffer[4] | (Buffer[5]<<8 );
            Height = Buffer[6] | (Buffer[7]<<8 );
            if ( Width<=0 || Width>=65536 || Height<=0 || Height>=65536 )
#ifndef SKL_LIGHT_CODE
               Exit_Upon_Error( "GIF invalid size ( %d x %d ) !\n", Width, Height );
#else
               goto Failed;
#endif
            if ( Buffer[8]&0x80 ) 
#ifndef SKL_LIGHT_CODE
               Exit_Upon_Error( "GIF loader: Local colormap not allowed" );
#else
               goto Failed;
#endif
            Bitmap = New_Bitmap( Width, Height, Nb_Col );
            memcpy( Bitmap->Pal, CMap, Nb_Col*3 );
            C = 0;
            F_READ( &C, In, 1 );
            if ( C<2 || C>9 )
#ifndef SKL_LIGHT_CODE
               Exit_Upon_Error( "Invalid GIF pixel size: %d", (INT)C );
#else
               goto Failed;
#endif
            if ( LZW_Decoder( Bitmap->Bits, In, Width*Height, (INT)C ) )
#ifndef SKL_LIGHT_CODE
               Exit_Upon_Error( "Gif decoding failed" );
#else
               goto Failed;
#endif
            Finished = TRUE;
         break;

         case ';': default:
            Finished = TRUE;
         break;
      }
   }
   F_CLOSE( In );
   return( Bitmap ); 

#ifdef SKL_LIGHT_CODE
Failed:
   Exit_Upon_Error( "Load_GIF failed" );
#endif   // SKL_LIGHT_CODE
  return NULL;  // HAPPY!
}


/*******************************************************/

