/***********************************************
 *              PAK loader                     *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "mem_map.h"
#include "pak.h"
#include "indy.h"

/*******************************************************/

EXTERN MEM_PAK *Load_PAK( char *Name )
{
   MEM_PAK *Pak;
   int i;
   INT Height, Width;
   INT Nb_Col, Pak_Size;
   BITMAP_IO Header;
   FILE *In;
   PIXEL Buffer[10]; 

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

   F_READ( Buffer, In, 10 );        /* *Do* use F_READ */

   if ( strncmp( PAK_MAGIC_HEADER, (char *)Buffer, 10 ) )
      Exit_Upon_Error( "'%s' has wrong PAK header.", Name );

   F_READ( &Header, In, sizeof( BITMAP_IO ) );

   Width  = Indy_l( Header.Width );
   Height = Indy_l( Header.Height );
   Nb_Col = Indy_l( Header.Nb_Col );
   F_READ( &Pak_Size, In, sizeof( INT ) );
   Pak_Size = Indy_l( Pak_Size );

   Pak = Create_Mem_Pak( Height, Nb_Col );

   if ( Nb_Col )
      F_COMPRESS_READ( (BYTE *)Pak->CMap, In, Nb_Col*sizeof( COLOR_ENTRY ) );
   for( i=0; i<Nb_Col; ++i ) *(UINT*)(Pak->CMap+i) = Indy_l( *(UINT*)(Pak->CMap+i) );

   MEM_Width( Pak ) = MEM_BpS( Pak ) = Width;
   MEM_Base_Ptr(Pak) = New_Fatal_Object( Pak_Size, PIXEL );
   Pak->Pak_Size = Pak_Size;

   /* F_COMPRESS_READ( MEM_Base_Ptr(Pak), In, Pak_Size*sizeof( PIXEL ) ); */
   F_READ( MEM_Base_Ptr(Pak), In, Pak_Size*sizeof( PIXEL ) );
   F_READ( Pak->H_Index, In, Height*sizeof( INT ) );
   for( i=0; i<Height; ++i ) Pak->H_Index[i] = Indy_l( Pak->H_Index[i] );

   F_CLOSE( In );

   return( Pak ); 
}

/*******************************************************/

