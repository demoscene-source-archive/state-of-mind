/***********************************************
 *          JPEG huffman decoder               *
 * Skal 98                                     *
 ***********************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "./jpeg.h"
#include "./ijpeg.h"
#include "./markers.h"

/*******************************************************/

static const INT lmask[] = {
   0x00000001, 0x00000003, 0x00000007, 0x0000000f,  
   0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,  
   0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,  
   0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,  
   0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,  
   0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,  
   0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,  
   0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
}; 

EXTERN FILE *JPG_F_Out = NULL;
EXTERN INT JPG_Out_Bits = 0x00;
EXTERN INT JPG_Bit_Pos = 7;

/*******************************************************/

#define ffputc(c,f)  { fputc(((c)&0xFF),(f)); if (((c)&0xFF)==JPG_MARKER_MARKER) fputc(0,(f)); }

static void Write_n( INT n, INT b )
{
   INT p;

   if ( n==0 ) return;  // ?!?  Error
   n--;
   b &= lmask[n];
   p = n - JPG_Bit_Pos;
   if ( !p )
   {
      JPG_Out_Bits |= b;
      ffputc( JPG_Out_Bits, JPG_F_Out );
      JPG_Out_Bits = 0x00;
      JPG_Bit_Pos = 7;
      return;
   }
   else if ( p<0 )
   {
      p = -p;
      JPG_Out_Bits |= ( b<<p );
      JPG_Bit_Pos = p-1;
      return;
   }
   JPG_Out_Bits |= ( b>>p );
   ffputc( JPG_Out_Bits, JPG_F_Out );
   while( p>7 )
   {
      p -= 8;
      JPG_Out_Bits = ( b>>p ) & 0xff;
      ffputc( JPG_Out_Bits, JPG_F_Out );
   }
   if ( !p )
   {
      JPG_Out_Bits = 0x00;
      JPG_Bit_Pos = 7;
   }
   else
   {
      JPG_Bit_Pos = 8-p;
      JPG_Out_Bits = (b<<JPG_Bit_Pos) & 0xff;
      JPG_Bit_Pos--;
   }
}

/*******************************************************/
/*******************************************************/

static const INT csize[] = 
{ 
  0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,
  6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
  6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
  7,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  8,8,8,8
};

/*******************************************************/

EXTERN void JPG_Encode_DC( INT Coef, INT *Last_DC, EHUFF *EHuff )
{
   INT s, Diff, cofac;

   Diff = Coef - *Last_DC;
   *Last_DC = Coef;
   cofac = abs( Diff );
   if ( cofac<256 ) s = csize[cofac];
   else { cofac = cofac>>8; s = csize[cofac] + 8; }
   Write_n( EHuff->ehufsi[s], EHuff->ehufco[s] );
   if ( Diff<0 ) Diff--;
   Write_n( s, Diff );
}

EXTERN void JPG_Encode_AC( INT *Matrix, EHUFF *EHuff )
{
   INT i, k, r, s, cofac;

   for( k=0,r=0; ++k<BLOCKSIZE; )
   {
      cofac = abs( Matrix[k] );
      if ( cofac<256 ) s = csize[cofac];
      else { cofac = cofac>>8; s = csize[cofac] + 8; }
      if ( Matrix[k]==0 )
      {
         if ( k==BLOCKSIZE-1 )
         {
            Write_n( EHuff->ehufsi[0], EHuff->ehufco[0] );
            break;
         }
         r++;
      }
      else
      {
         while( r>15 )
         {
            Write_n( EHuff->ehufsi[240], EHuff->ehufco[240] );
            r -= 16;
         }
         i = 16*r + s;
         r = 0;
         Write_n( EHuff->ehufsi[i], EHuff->ehufco[i] );
         if ( Matrix[k]<0 ) Write_n( s, Matrix[k]-1 );
         else Write_n( s, Matrix[k] );
      }
   }
}

/*******************************************************/

