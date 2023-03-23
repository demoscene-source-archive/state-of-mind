/***********************************************
 *             cache saving                    *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

  // from ijpeg.h
extern BYTE *JPEG_Out( STRING Name, BYTE *Bits, 
   INT Width, INT Height, INT Type );
   
/******************************************************************/

EXTERN INT Save_Txt_Cache_II( STRING Name, void *C )
{
   FILE *Out;
   TXT_CACHE *Cache;

   Cache = (TXT_CACHE*)C;

   Out = Access_File( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( TRUE );

//   Cache->Nb_Texture_Maps = ( (Cache->Nb_Texture_Maps-1)/3 + 1 ) * 3;

   fwrite( TXT_MAGIK2, 6, 1, Out );
   fprintf( Out, "%c", Cache->Nb_Texture_Maps&0xFF );
   fprintf( Out, "%c", ( Cache->Nb_Texture_Maps>>8 ) & 0xFF );

   if ( Cache->Nb_Texture_Maps )       // Saves all
   {
         // TODO: Indian problems here!!!
      fwrite( Cache->Maps, sizeof( TEXTURE_MAP ),
         Cache->Nb_Texture_Maps, Out );
      Compress_Write( Cache->Texture_Maps_Ptr, Out,
         TEXTURE_MAP_SIZE_16b*(Cache->Nb_Texture_Maps/3), 8 );
   }

   fclose( Out );
   return( FALSE );
}

/******************************************************************/

EXTERN INT Save_Txt_Cache_IIbis( STRING Name, void *C )
{
   FILE *Out;
   TXT_CACHE *Cache;
   char Tmp_Name[256];
   INT i;

   Cache = (TXT_CACHE*)C;

   Out = Access_File( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( TRUE );

//   Cache->Nb_Texture_Maps = ( (Cache->Nb_Texture_Maps-1)/3 + 1 ) * 3;

   fwrite( TXT_MAGIK2, 6, 1, Out );
   fprintf( Out, "%c", Cache->Nb_Texture_Maps&0xFF );
   fprintf( Out, "%c", ( Cache->Nb_Texture_Maps>>8 ) & 0xFF );

   if ( Cache->Nb_Texture_Maps )       // Saves all
   {
         // TODO: Indian problems here!!!
      fwrite( Cache->Maps, sizeof( TEXTURE_MAP ),
         Cache->Nb_Texture_Maps, Out );

      sprintf( Tmp_Name, "%s", Name );
      for( i=0; i<(INT)strlen( Name ); ++i )
         if ( Name[i] == '.' ) break;
      sprintf( Tmp_Name, "%s", Name );
      sprintf( Tmp_Name+i+1, "jpg" );
      Out_Message( "Saving additional jpeg %s for cache '%s'.",
         Tmp_Name, Name );
      JPEG_Out( Tmp_Name, Cache->Texture_Maps_Ptr, 
         256, (Cache->Nb_Texture_Maps/3)*256, 0x03 | 0x08 );
      Out_Message( "\n - Done." );
   }

   fclose( Out );
   return( FALSE );
}

/******************************************************************/
