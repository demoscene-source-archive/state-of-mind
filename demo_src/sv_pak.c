/***********************************************
 *              PAK saver                      *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "mem_map.h"
#include "pak.h"
#include "indy.h"

/*******************************************************/
/*******************************************************/

EXTERN void Save_PAK( char *Name, MEM_PAK *Pak )
{
   FILE *Out;
   BITMAP_IO Header;
   INT Size;

   Out = Access_File( Name, WRITE_SWITCH );
   if ( Out==NULL ) return;

   Mem_Clear( &Header );

   Header.Width  = Indy_l( MEM_Width(Pak) );
   Header.Height = Indy_l( MEM_Height(Pak) );
   Header.Nb_Col  = Indy_l( Pak->Nb_Col );
   Size = Indy_l( Pak->Pak_Size );

   fwrite( PAK_MAGIC_HEADER, 10, 1, Out );

   fwrite( &Header, sizeof( BITMAP_IO ), 1, Out );
   fwrite( &Size, sizeof( INT ), 1, Out );

   if ( Pak->Nb_Col )
      Compress_Write( (BYTE*)Pak->CMap, Out, Pak->Nb_Col*sizeof( COLOR_ENTRY ),  8 );

   /* Compress_Write( MEM_Base_Ptr(Pak), Out, Pak->Pak_Size*sizeof( PIXEL ), 8 ); */
   fwrite( (void*)MEM_Base_Ptr(Pak), sizeof( PIXEL ), Pak->Pak_Size, Out );
   fwrite( (void*)Pak->H_Index, sizeof(INT), MEM_Height(Pak), Out );

   fclose( Out );
}

/*******************************************************/

