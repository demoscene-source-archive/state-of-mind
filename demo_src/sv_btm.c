/***********************************************
 *              BTM saver                      *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"

/*******************************************************/
/***************** Save bitmaps... *********************/
/*******************************************************/

EXTERN void Save_BTM( char *Name, BITMAP *Btm )
{
   FILE *Out;
   BITMAP_IO Header;
   INT i, j;

   /* Out = Access_File( Name, WRITE_SWITCH ); */
   Out = fopen( Name, WRITE_SWITCH );
   if ( Out==NULL ) return;

   Mem_Clear( &Header );

   Header.Width  = Btm->Width;
   Header.Height = Btm->Height;
   Header.Nb_Col  = Btm->Nb_Col;

   fwrite( BTM_MAGIC_HEADER, 10, 1, Out );

   fwrite( &Header, sizeof( BITMAP_IO ), 1, Out );
   if ( Btm->Nb_Col )
      Compress_Write( Btm->Pal, Out, 3*Btm->Nb_Col, 8 );

   j = 0; i = Btm->Nb_Col;
   while( i ) { j++; i >>= 1; }
   if ( j<2 ) j = 2;

   Compress_Write( Btm->Bits, Out, Btm->Size, j );

   fclose( Out );
}

/*******************************************************/

