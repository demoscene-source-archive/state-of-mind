/***********************************************
 *              BTM loader                     *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "indy.h"

/*******************************************************/

EXTERN BITMAP *Load_BTM( char *Name )
{
   BITMAP *Bitmap;
   INT Height, Width;
   INT Nb_Col;
   BITMAP_IO Header;
   FILE *In;
   PIXEL Buffer[10]; 

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

   F_READ( Buffer, In, 10 );
   if ( strncmp( BTM_MAGIC_HEADER, (char *)Buffer, 10 ) )
      Exit_Upon_Error( "'%s' has wrong BTM header.", Name );

   F_READ( &Header, In, sizeof( BITMAP_IO ) );

   Width  = Indy_l( Header.Width );
   Height = Indy_l( Header.Height );
   Nb_Col = Indy_l( Header.Nb_Col );

   Bitmap = New_Bitmap( Width, Height, Nb_Col );
   if ( Nb_Col ) F_COMPRESS_READ( Bitmap->Pal, In, 3*Nb_Col );

   F_COMPRESS_READ( Bitmap->Bits, In, Bitmap->Size );
   F_CLOSE( In );

   return( Bitmap ); 
}

/*******************************************************/

