/***********************************************
 *              RAW format                     *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "ld_any.h"
#include "truecol.h"

static PIXEL Head[8] = { 
   0x6d, 0x68, 0x77, 0x61, 0x6e, 0x68, 0x00, 0x04
};

/*******************************************************/
/*******************************************************/

EXTERN void Save_RAW( char *Name, BITMAP *Bitmap, PIXEL *Pal )
{
   PIXEL Buf[ 768 ];
   FILE *Out;

   Out = fopen( Name, WRITE_SWITCH );
   if ( Out==NULL ) return;

   memset( Buf, 0, 768 );
   memcpy( Buf, Head, 8 );
   Buf[8] = (Bitmap->Width>>8)&0xFF;
   Buf[9] = Bitmap->Width&0xFF;
   Buf[10] = (Bitmap->Height>>8)&0xFF;
   Buf[11] = Bitmap->Height&0xFF;
   fwrite( Buf, 32, 1, Out );

   if ( Pal==NULL ) Pal = Bitmap->Pal;
   memset( Buf, 0, 768 );
   if ( Bitmap->Nb_Col ) memcpy( Buf, Pal, Bitmap->Nb_Col*3 );
   fwrite( Buf, 256, 3, Out );

   fwrite( Bitmap->Bits, Bitmap->Width, Bitmap->Height, Out );

   fclose( Out );
}

EXTERN BITMAP *Load_RAW( char *Name )
{
   FILE *In;
   INT i, Nb_Col, Width, Height;   
   PIXEL Pal[ 768 ], New_Pal[ 768 ], *Bits;
   PIXEL Map[ 256 ];
   BITMAP *Btm;

   Btm = NULL;

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );
   F_READ( Pal, In, 32 );
   for( i=0;i<8;++i ) if ( Pal[i] != Head[i] )
   {
//       Out_Message( "'%s' has wrong RAW header...", Name );
       goto Failed;
   }

   Width  = Pal[8]*256 + Pal[9];
   Height = Pal[10]*256 + Pal[11];

   F_READ( Pal, In, 768 );
   Nb_Col = 0;
   for( i=0; i<256; ++i )
   {
      INT j, r, g, b;

      r = Pal[3*i];
      g = Pal[3*i+1];
      b = Pal[3*i+2];
      for( j=0; j<Nb_Col; ++j )
      {
          if ( r==New_Pal[3*j] && g==New_Pal[3*j+1] && b==New_Pal[3*j+2] )
          {
             Map[ i ] = (PIXEL)j;
             break;
          }
      }
      if ( j == Nb_Col )
      {
         New_Pal[ 3*j   ] = (PIXEL)r;
         New_Pal[ 3*j+1 ] = (PIXEL)g;
         New_Pal[ 3*j+2 ] = (PIXEL)b;
         Map[ i ] = (PIXEL)j;
         Nb_Col++;
      }
   }
   Btm = New_Bitmap( Width, Height, Nb_Col );
   if ( Btm==NULL ) goto Failed;
   memcpy( Btm->Pal, New_Pal, Nb_Col*3 );

   Bits = Btm->Bits;
   for( ; Height>0; --Height )
   {
      F_READ( Bits, In, Width );
      for( i=0; i<Width; ++i ) Bits[i] = Map[ Bits[i] ];
      Bits += Width;
   }

Failed:
   if ( In!=NULL ) F_CLOSE( In );
   return( Btm );

}
