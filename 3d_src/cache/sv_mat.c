/***********************************************
 *             cache saving                    *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

EXTERN INT Save_Txt_Cache_I( STRING Name, void *C )
{
   FILE *Out;
   TXT_CACHE *Cache;

   Cache = (TXT_CACHE*)C;

   Out = Access_File( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( TRUE );

//   Cache->Nb_Texture_Maps = ( (Cache->Nb_Texture_Maps-1)/3 + 1 ) * 3;

   fwrite( TXT_MAGIK, 5, 1, Out );
   fprintf( Out, "%c", Cache->Nb_Texture_Maps&0xFF );
   fprintf( Out, "%c", ( Cache->Nb_Texture_Maps>>8 ) & 0xFF );

   if ( Cache->Nb_Texture_Maps )       // Saves all
   {
         // TODO: Indian problems here!!!
      fwrite( Cache->Maps, sizeof( TEXTURE_MAP ),
         Cache->Nb_Texture_Maps, Out );
      Compress_Write( Cache->Texture_Maps_Ptr, Out,
         TEXTURE_MAP_SIZE*(Cache->Nb_Texture_Maps/3), 8 );
   }
   fprintf( Out, "%c", Cache->Nb_Col&0xFF );
   fprintf( Out, "%c", (Cache->Nb_Col>>8)&0xFF );
   if ( Cache->Nb_Col )
      Compress_Write( Cache->CMap, Out, 3*Cache->Nb_Col,  8 );

   fprintf( Out, "%c", Cache->Nb_Offset&0xFF );
   fprintf( Out, "%c", (Cache->Nb_Offset>>8)&0xFF );

   fclose( Out );
   return( FALSE );
}

/******************************************************************/
