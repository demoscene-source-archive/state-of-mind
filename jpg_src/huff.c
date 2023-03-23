/***********************************************
 *          JPEG huffman decoder               *
 * Skal 98                                     *
 ***********************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "./jpeg.h"
#include "bit_strm.h"
#include "./markers.h"

/*******************************************************/

static INT SizeTable( const INT bits[36], INT *Huff_Size )
{
   INT i, j, p;
   for( p=0,i=1; i<17; ++i )
   {
      for( j=1; j<=bits[i]; j++ )
         Huff_Size[p++] = i;
   }
   Huff_Size[p] = 0;
   return( p );
}

static void CodeTable( const INT Huff_Size[257], INT Huff_Code[257] )
{
   INT p, Code, Size;

   p = 0; Code = 0;
   Size = Huff_Size[0];
   while( 1 )
   {
      do { Huff_Code[p++] = Code++; } 
      while( (Huff_Size[p]==Size)&&(p<257) );

      if ( Huff_Size[p]==0 ) break;

      do { Code<<=1; Size++; }
      while( Huff_Size[p] != Size );
   }
}

static void DecoderTables( XHUFF *XHuff, DHUFF *DHuff, const INT Huff_Code[257]  )
{
   INT l, p;

   for( DHuff->ml=1,p=0,l=1; l<=16; ++l )
   {
      if ( XHuff->bits[l]==0 )
         DHuff->maxcode[l] = -1;    // -1 = Skip. Wrong jpeg
      else
      {
         DHuff->valptr[l] = p;
         DHuff->mincode[l] = Huff_Code[p];
         p += XHuff->bits[l] - 1;
         DHuff->maxcode[l] = Huff_Code[p];
         DHuff->ml = l;
         p++;
      }
   }
   DHuff->maxcode[DHuff->ml]++;
}
/*******************************************************/

static void OrderCodes( INT Last, XHUFF *XHuff, EHUFF *EHuff, INT Huff_Code[257], INT Huff_Size[257] )
{
   INT Index, p;
   for( p=0; p<Last; ++p )
   {
      Index = XHuff->huffval[p];
      EHuff->ehufco[Index] = Huff_Code[p];
      EHuff->ehufsi[Index] = Huff_Size[p];
   }
}


/*******************************************************/

EXTERN void JPG_ReadHuffman( XHUFF *XHuff, DHUFF *DHuff )
{
   INT Huff_Size[257];
   INT Huff_Code[257];

   INT i, Accum;
   for( Accum=0,i=1; i<=16; ++i )
   {
      XHuff->bits[i] = BSTRM_ONE_BYTE( );
      Accum += XHuff->bits[i];
   }
   for( i=0; i<Accum; ++i )
      XHuff->huffval[i] = BSTRM_ONE_BYTE( );
   SizeTable( XHuff->bits, Huff_Size );
   CodeTable( Huff_Size, Huff_Code );
   DecoderTables( XHuff, DHuff, Huff_Code );
}

/*******************************************************/

EXTERN void JPG_Specified_Huffman( INT *bts, INT *hvls, XHUFF *XHuff, EHUFF *EHuff )
{
   INT i, Accum, Last;
   INT Huff_Size[257];
   INT Huff_Code[257];

   for( Accum=0,i=0; i<16; ++i )
   {
      Accum += bts[i];
      XHuff->bits[i+1] = bts[i];
   }
   for( i=0; i<Accum; ++i ) XHuff->huffval[i] = hvls[i];
   Last = SizeTable( XHuff->bits, Huff_Size );
   CodeTable( Huff_Size, Huff_Code );
   OrderCodes( Last, XHuff, EHuff, Huff_Code, Huff_Size );
}

/*******************************************************/
/*******************************************************/

static INT bit_set_mask[] = { /* This is 2^i at ith position */
0x00000001,0x00000002,0x00000004,0x00000008,
0x00000010,0x00000020,0x00000040,0x00000080,
0x00000100,0x00000200,0x00000400,0x00000800,
0x00001000,0x00002000,0x00004000,0x00008000,
0x00010000,0x00020000,0x00040000,0x00080000,
0x00100000,0x00200000,0x00400000,0x00800000,
0x01000000,0x02000000,0x04000000,0x08000000,
0x10000000,0x20000000,0x40000000,0x80000000};
static int extend_mask[]={       /* Used for sign extensions. */
0xFFFFFFFE,0xFFFFFFFC,0xFFFFFFF8,0xFFFFFFF0,
0xFFFFFFE0,0xFFFFFFC0,0xFFFFFF80,0xFFFFFF00,
0xFFFFFE00,0xFFFFFC00,0xFFFFF800,0xFFFFF000,
0xFFFFE000,0xFFFFC000,0xFFFF8000,0xFFFF0000,
0xFFFE0000,0xFFFC0000,0xFFF80000,0xFFF00000,
//0xFE000000,0xFC000000,0xF8000000,0xF0000000,
//0xE0000000,0xC0000000,0x80000000,0x00000000
};

/*******************************************************/

EXTERN BYTE JPG_One_Bit_Safe( )
{
   BYTE Ret;
   BYTE Bits_Left;

   Bits_Left = (BSTRM_Bit_Pos & 0x07) ^ 0x07;
   Ret = BSTRM_Ptr[0]>>Bits_Left;
   if ( !Bits_Left ) {
      if ( BSTRM_Ptr[0]==JPG_MARKER_MARKER )
      {
         BSTRM_Ptr++;      // BSTRM_Ptr[1] must be 0x00 here
         BSTRM_Bit_Pos += 8;
//         fprintf( stderr, "*%d", BSTRM_Ptr[0] );
      }
      BSTRM_Ptr++;
   }
   BSTRM_Bit_Pos++;
   return( Ret&0x01 );
}

static UINT JPG_Get_Safe_Bits( INT How_Many )
{
   UINT Ret;
   INT Bits_Left, i, j;

   if ( !How_Many ) return( 0 );

      // 24 bits max
   for( i=3, j=0, Ret=0; i>0; i-- )
   {
      Ret = (Ret<<8) | BSTRM_Ptr[j];   
      if ( BSTRM_Ptr[j]==JPG_MARKER_MARKER ) j++;
      j++;
   }

   Bits_Left = BSTRM_Bit_Pos & 0x07;
   Ret <<= Bits_Left+8;
   Ret >>= 32-How_Many;

   BSTRM_Bit_Pos += How_Many;
   Bits_Left = 8-Bits_Left - How_Many;
   while( Bits_Left<=0 ) { 
      if ( BSTRM_Ptr[0]==JPG_MARKER_MARKER ) 
      {
         BSTRM_Ptr++; BSTRM_Bit_Pos += 8;
      }
      BSTRM_Ptr++;
      Bits_Left+=8;
   }

   return( Ret );
}

static INT Decode_Huffman( XHUFF *XHuff, DHUFF *DHuff )
{
   INT Code, l;

   Code = (INT)JPG_One_Bit_Safe( );
   for( l=1; Code>DHuff->maxcode[l]; l++ )
      Code = (Code<<1) | (INT)JPG_One_Bit_Safe( );
//   if ( Code>=DHuff->maxcode[DHuff->ml] ) return( 0 );   // => Read error
   l = DHuff->valptr[l] + Code - DHuff->mincode[l];
   return( XHuff->huffval[l] );
}

EXTERN INT JPG_Decode_DC( INT *Matrix, XHUFF *XHuff, DHUFF *DHuff )
{
   INT s;

   s = Decode_Huffman( XHuff, DHuff );
   if ( s )
   {
      INT Diff;
      Diff = JPG_Get_Safe_Bits( s );
      if ( (Diff&bit_set_mask[s-1])==0x00 )
      {
         Diff |= extend_mask[s-1];
         Diff++;
      }
      Matrix[0] += Diff;      // Matrix[0] = Last_DC
   }
   return( Matrix[0] );
}

EXTERN void JPG_Decode_AC( INT *Matrix, XHUFF *XHuff, DHUFF *DHuff )
{
   INT k;

   memset( &Matrix[1], 0, (BLOCKSIZE-1)*sizeof( INT ) );
   for( k=1; k<BLOCKSIZE; )
   {
      INT r, s, n;
      r = Decode_Huffman( XHuff, DHuff );      
      s = LO15( r );      
      n = HI15( r );
      if ( s )
      {
         k += n;
         if ( k>=BLOCKSIZE ) break;    // JPEG mistake
         
         Matrix[k] = JPG_Get_Safe_Bits( s );
         if ( (Matrix[k]&bit_set_mask[s-1])==0x00 )
         {
            Matrix[k] |= extend_mask[s-1];
            Matrix[k]++;
         }
         k++;
      }
      else if ( n==15 ) k += 16;
      else break;
   }
}

/*******************************************************/
