/***********************************************
 *              GIF saver                      *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "sv_any.h"
#include "lzw.h"

/*******************************************************/

EXTERN void Save_GIF( char *Name, BITMAP *Btm )
{
   FILE *Out;
   INT i, Width, Height;
   INT Nb_Col, Pixel_Depth;
   PIXEL Buffer[13];

//   Out = Access_File( Name, WRITE_SWITCH );
   Out = fopen( Name, WRITE_SWITCH );
   if ( Out==NULL ) return;

   Width  = Btm->Width;
   Height = Btm->Height;
   Nb_Col = Btm->Nb_Col;

   Pixel_Depth = 0; i = 2*Nb_Col-1;
   while( i ) { Pixel_Depth++; i >>= 1; }
   Pixel_Depth--;
   if ( Pixel_Depth<2 ) Pixel_Depth = 2;

   fwrite( "GIF87a", 1, 6, Out );
   Buffer[6] = (BYTE)(Width&0xFF);
   Buffer[7] = (BYTE)(Width>>8);
   Buffer[8] = (BYTE)(Height&0xFF);
   Buffer[9] = (BYTE)(Height>>8);
   Buffer[10] = Nb_Col ? 0x80 : 0x00;
   Buffer[10] |= (Pixel_Depth-0)<<5;
   Buffer[10] |= (Pixel_Depth-1);
   Buffer[11] = 0;            // Background
   Buffer[12] = 0;            // Future expansion
   fwrite( Buffer+6, 7, 1, Out );
   if ( Nb_Col ) 
   {
      fwrite( Btm->Pal, 3, Nb_Col, Out );
      for( i=Nb_Col; i<1<<(Pixel_Depth); ++i )
      {
         fputc( 0, Out ); fputc( 0, Out ); fputc( 0, Out );
      }
// Out_Message( "Saved %d col out of %d", Nb_Col, 1<<(Pixel_Depth) );
   }   

      // Image header

   Buffer[0] = ',';
   Buffer[1] = Buffer[2] = 0;
   Buffer[3] = Buffer[4] = 0;
   Buffer[5] = (BYTE)(Width&0xFF);
   Buffer[6] = (BYTE)(Width>>8);
   Buffer[7] = (BYTE)(Height&0xFF);
   Buffer[8] = (BYTE)(Height>>8);
   Buffer[9] = 0x00;             // No interlace

   Buffer[10] = (BYTE)Pixel_Depth;     //  initial code siz
   fwrite( Buffer, 11, 1, Out );

   LZW_Encoder( Btm->Bits, Out, Width*Height, Pixel_Depth );

   fputc( 0, Out );
   fputc( ';', Out );            // GIF terminator

   fclose( Out );
}


/*******************************************************/

