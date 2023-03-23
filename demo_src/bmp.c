/***********************************************
 *         BMP (BM6 only) loader               *
 *                                             *
 * not full-featured...                        *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "ld_any.h"

/*******************************************************/

EXTERN BITMAP *Load_BMP( char *Name )
{
   FILE *In;
   INT i, Nb_Col, Width, Height;
   PIXEL Pal[ 256*4 ], New_Pal[ 768 ], *Bits;
   PIXEL Map[ 256 ];
   BITMAP *Btm;
   
   Btm = NULL;

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );
   F_READ( Pal, In, 54 );
   if ( Pal[0]!=0x42 || Pal[1]!=0x4D )
   {
//       Out_Message( "'%s' has wrong BM6 header...", Name );
       goto Failed;
   }

   Width  = Pal[19]*256 + Pal[18];
   Height = Pal[23]*256 + Pal[22];

   F_READ( Pal, In, 256*sizeof(INT) );
   Nb_Col = 0;
   for( i=0; i<256; ++i )
   {
      INT j, r, g, b;

      b = Pal[4*i];
      g = Pal[4*i+1];
      r = Pal[4*i+2];
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
         New_Pal[ 3*Nb_Col   ] = (PIXEL)r;
         New_Pal[ 3*Nb_Col+1 ] = (PIXEL)g;
         New_Pal[ 3*Nb_Col+2 ] = (PIXEL)b;
         Map[ i ] = (PIXEL)Nb_Col;
         Nb_Col++;
      }
   }
   Btm = New_Bitmap( Width, Height, Nb_Col );
   if ( Btm==NULL ) goto Failed;
   memcpy( Btm->Pal, New_Pal, Nb_Col*3 );

   Bits = Btm->Bits + (Height-1)*Width;
   for( ; Height>0; --Height )
   {
      F_READ( Bits, In, Width );
      for( i=0; i<Width; ++i ) Bits[i] = Map[ Bits[i] ];
      Bits -= Width;
   }

Failed:
   if ( In!=NULL ) F_CLOSE( In );
   return( Btm );
}

/*******************************************************/

