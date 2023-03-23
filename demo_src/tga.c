/***********************************************
 *              TGA loader                     *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "ld_any.h"
#include "lzw.h"
#include "tga.h"

/*******************************************************/

static INT Read_TGA_Color( PIXEL *Dst, INT Size, FILE *In )
{
   PIXEL R, G, B, F, C1, C2;
   switch( Size )
   {
      case 8:
         F_READ( &R, In, sizeof( R ) );
         Dst[0] = Dst[1] = Dst[2] = R;
      break;

      case 15: case 16:
         F_READ( &C1, In, sizeof( C1 ) );
         F_READ( &C2, In, sizeof( C2 ) );
         R = ( C2&0x7C )>>2;
         G = ( ( C2&0x03 )<<3 ) | ( (C1&0xE0)>>5 );
         B = C1 & 0x1F;
         Dst[0] = R;
         Dst[1] = G;
         Dst[2] = B;
      break;

      case 24: case 32:
         F_READ( &R, In, sizeof( R ) );
         F_READ( &G, In, sizeof( G ) );
         F_READ( &B, In, sizeof( B ) );
         if ( Size==32 ) F_READ( &F, In, sizeof( F ) );  // alpha. Discard.
         Dst[0] = R;
         Dst[1] = G;
         Dst[2] = B;
      break;
   }
   return( 0 );
}

/*******************************************************/

EXTERN BITMAP *Load_TGA( char *Name )
{
   BITMAP *Btm;
   int i, jo, j_base, j_real;
   INT Nb_Col, Col_Min;
   INT Height, Width;
   FILE *In;
   TGA_HEAD Head;
   PIXEL Buffer[256];

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

      // Indy pb here!!
   F_READ( &Head, In, sizeof( TGA_HEAD ) );        // *Do* use F_READ
   if ( Head.IDLength!=0 )
      F_READ( Buffer, In, Head.IDLength );

   Width = Head.Width_lo + 256*Head.Width_hi;
   Height = Head.Height_lo + 256*Head.Height_hi;
   if ( Head.ImgType!=TGA_Map )
   {
Failed:
#ifdef SKL_LIGHT_CODE
      Out_Message( "Unknown TGA format" );
#endif // SKL_LIGHT_CODE
      F_CLOSE( In );
      return( NULL );
   }

//   if ( Head.CoSize!=24 ) goto Failed;
   Col_Min = Head.Index_lo + 256*Head.Index_hi;
   Nb_Col = Col_Min + Head.Length_lo + 256*Head.Length_hi;

   if ( Nb_Col>256 ) goto Failed;

   Btm = New_Bitmap( Width, Height, Nb_Col );
   for( i=Col_Min; i<Nb_Col; ++i )
   {
      Read_TGA_Color( &Btm->Pal[3*i], Head.CoSize, In );
   }
   Btm->Nb_Col = Nb_Col;

   j_base = 0;
   j_real = 0;
   for( jo=0; jo<Height; ++jo )
   {
      INT Row;
      Row = j_real;
      if ( Head.OrgBit==0 ) Row = (Height-1) - Row;
      F_READ( &Btm->Bits[Row*Btm->Width], In, Width );
      if ( Head.IntrLve==TGA_IL_Four )
         j_real += 4;
      else if ( Head.IntrLve==TGA_IL_Two )
         j_real += 2;
      else j_real++;
      if ( j_real>Height ) { j_real = ++j_base; }
   }
   F_CLOSE( In );
   return( Btm );
}

/*******************************************************/
