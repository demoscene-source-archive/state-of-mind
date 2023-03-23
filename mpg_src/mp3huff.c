/************************************************
 *           Huffman decoder                    *
 * heavily based on 'xamp' from Tomislav Uzelac *
 * Skal 98                                      *
 ************************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "bit_strm.h"
#include "./mp3.h"
#include "./mp3huff.h"

/*******************************************************/
/*     specific bits reading call for MP3_Buffer       */
/*******************************************************/

            // TODO: Little/big endian pb here!?!

static UINT MP3_One_Bit( )
{
   UINT Ret;
   INT Bits_Left;

   Bits_Left = (MP3_Bit_Pos & 0x07)^0x07;
   Ret = MP3_Buf[0]>>Bits_Left;
   if ( !Bits_Left ) { 
      MP3_Buf++;
      MP3_Buf32 = (MP3_Buf32<<8) | MP3_Buf[3];
   }
   MP3_Bit_Pos++;
   return( Ret&0x01 );
}

static UINT MP3_More_Bits( INT How_Many )   
{
   UINT Ret;
   INT Bits_Left;

   if ( !How_Many ) return( 0 );
   Bits_Left = MP3_Bit_Pos & 0x07;
   Ret = MP3_Buf32<<Bits_Left;
   Ret >>= 32-How_Many;
//   Ret &= BSTRM_Mask[How_Many];

   MP3_Bit_Pos += How_Many;
   Bits_Left = 8-Bits_Left - How_Many;
   while( Bits_Left<=0 ) { 
      MP3_Buf++; Bits_Left+=8;
      MP3_Buf32 = (MP3_Buf32<<8) | MP3_Buf[3];
   }
   return( Ret );
}

static UINT MP3_Fetch_Bits( INT How_Many )
{
   UINT Ret;
   INT Bits_Left;

   if ( !How_Many ) return( 0 );
   Bits_Left = MP3_Bit_Pos & 0x07;
   Ret = MP3_Buf32 << Bits_Left;
   Ret >>= 32-How_Many;
//   Ret &= BSTRM_Mask[How_Many];

   return( Ret ); 
}

/*******************************************************/
/*                  MPEG-1 decoding                    */
/*******************************************************/

static BYTE HUFF_SLen1[16] = { 0,0,0,0, 3,1,1,1, 2,2,2,3, 3,3,4,4 };
static BYTE HUFF_SLen2[16] = { 0,1,2,3, 0,1,2,3, 1,2,3,1, 2,3,2,3 };

EXTERN void MP3_Decode_Scale_Factors_MPEG1( MP3_INFO *Info, INT gr, INT ch )
{
   INT SLen1, SLen2;
   USHORT *Scale_s, *Scale_l;

      // compress factor
   SLen1 = HUFF_SLen1[ Info->Scale_Factor_Compress[gr][ch] ];
   SLen2 = HUFF_SLen2[ Info->Scale_Factor_Compress[gr][ch] ];
   Scale_s = MP3_Scale_s[ch][0];
   Scale_l = MP3_Scale_l[ch];
   if ( Info->Block[gr][ch]>=2 )   // mixed/short blocks
   {
      INT sfb;
      if ( Info->Block[gr][ch]==3 )  // mixed
      {
         Scale_l[0] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[1] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[2] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[3] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[4] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[5] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[6] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[7] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_s += 3*( 3 );  // jump to sfb #3
      }
      else  // short
      {
         for( sfb=0; sfb<3; ++sfb )
         {
            *Scale_s++ = (USHORT)MP3_More_Bits( SLen1 );
            *Scale_s++ = (USHORT)MP3_More_Bits( SLen1 );
            *Scale_s++ = (USHORT)MP3_More_Bits( SLen1 );
         }
      }
      for( sfb=3; sfb<6; ++sfb )
      {
         *Scale_s++ = (USHORT)MP3_More_Bits( SLen1 );
         *Scale_s++ = (USHORT)MP3_More_Bits( SLen1 );
         *Scale_s++ = (USHORT)MP3_More_Bits( SLen1 );
      }
      for( ; sfb<12; ++sfb )
      {
         *Scale_s++ = (USHORT)MP3_More_Bits( SLen2 );
         *Scale_s++ = (USHORT)MP3_More_Bits( SLen2 );
         *Scale_s++ = (USHORT)MP3_More_Bits( SLen2 );
      }
      *Scale_s++ = 0; *Scale_s++ = 0; *Scale_s++ = 0; 
   }
   else       // long blocks
   {
      if ( !Info->scfsi[ch][0] || !gr )
      {
         Scale_l[0] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[1] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[2] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[3] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[4] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[5] = (USHORT)MP3_More_Bits( SLen1 );
      }
      if ( !Info->scfsi[ch][1] || !gr )
      {
         Scale_l[6] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[7] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[8] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[9] = (USHORT)MP3_More_Bits( SLen1 );
         Scale_l[10] = (USHORT)MP3_More_Bits( SLen1 );
      }
      if ( !Info->scfsi[ch][2] || !gr )
      {
         Scale_l[11] = (USHORT)MP3_More_Bits( SLen2 );
         Scale_l[12] = (USHORT)MP3_More_Bits( SLen2 );
         Scale_l[13] = (USHORT)MP3_More_Bits( SLen2 );
         Scale_l[14] = (USHORT)MP3_More_Bits( SLen2 );
         Scale_l[15] = (USHORT)MP3_More_Bits( SLen2 );
      }
      if ( !Info->scfsi[ch][3] || !gr )
      {
         Scale_l[16] = (USHORT)MP3_More_Bits( SLen2 );
         Scale_l[17] = (USHORT)MP3_More_Bits( SLen2 );
         Scale_l[18] = (USHORT)MP3_More_Bits( SLen2 );
         Scale_l[19] = (USHORT)MP3_More_Bits( SLen2 );
         Scale_l[20] = (USHORT)MP3_More_Bits( SLen2 );
      }
      Scale_l[21] = 0;
   }
#if 0
   { 
      static int count =0, i;
      fprintf( stderr, "#%d %d %d %d %d  ", count++, gr, ch, SLen1, SLen2 );
      for( i=0; i<21; ++i ) fprintf( stderr, "%d ", MP3_Scale_l[ch][i] );
      fprintf( stderr, "\n" );
   }
#endif
}

/*******************************************************/
/*                 MPEG-2 decoding                     */
/*******************************************************/

#ifndef _SKIP_MPEG2_ 

EXTERN void MP3_Decode_Scale_Factors_MPEG2( MP3_INFO *Info, INT ch )
{
   INT Index, Spooky_Index;
   INT Int_Scalefac_Compress;
   INT SLen[5], Nb_sfb[5]; // actually, there's four of each, not five, 
                           // labelled 1 through 4, but 
                           // what's a word of storage compared to
                           // one's sanity. so [0] is irrelevant.

 // Spooky_Index: indicates whether we use the normal set of slen eqs 
 //   and nr_of_sfb tables // or the one for the right channel of intensity
 //   stereo coded frame
 // Index :       corresponds to the value of scalefac_compress, 
 //   as listed in the standard 
                   
   Index = 0;

   Int_Scalefac_Compress = Info->Scale_Factor_Compress[0][ch];

   if ( ( MP3_Ext==1 || MP3_Ext==3 ) && (ch==1) )
   {
               // right channel

      Int_Scalefac_Compress >>= 1;
      Spooky_Index = 1;
      Info->Preflag[0][1] = 0;
      SLen[4] = 0;
      if ( Int_Scalefac_Compress<180 )
      {
         SLen[1] = Int_Scalefac_Compress/36;
         SLen[2] = (Int_Scalefac_Compress%36)/6;
         SLen[3] = (Int_Scalefac_Compress%36)%6;
         Index = 0;
      }
      else if ( Int_Scalefac_Compress<244 )
      {
         INT Tmp;
         Tmp = Int_Scalefac_Compress - 180;
         SLen[1] = ( Tmp%64 )>>4;
         SLen[2] = ( Tmp%16 )>>2;
         SLen[3] = Tmp%4;
         Index = 1;
      }
      else //  if ( Int_Scalefac_Compress<255 )
      {
         INT Tmp;
         Tmp = Int_Scalefac_Compress - 244;
         SLen[1] = Tmp/3;
         SLen[2] = Tmp%3;
         SLen[3] = 0;
         Index = 2;
      }
   }
   else     // mono
   {
      Spooky_Index = 0;
      if ( Int_Scalefac_Compress<400 )
      {
         SLen[1] = (Int_Scalefac_Compress>>4)/5;
         SLen[2] = (Int_Scalefac_Compress>>4)%5;
         SLen[3] = (Int_Scalefac_Compress%16)>>2;
         SLen[4] = Int_Scalefac_Compress%4;
         Info->Preflag[0][ch] = 0;
         Index = 0;
      }
      else if ( Int_Scalefac_Compress<500 )
      {
         INT Tmp;
         Tmp = Int_Scalefac_Compress - 400;
         SLen[1] = (Tmp>>2)/5;
         SLen[2] = (Tmp>>2)%5;
         SLen[3] = Tmp%4;
         SLen[4] = 0;
         Info->Preflag[0][ch] = 0;
         Index = 1;
      }
      else //  if ( Int_Scalefac_Compress<512 )
      {
         INT Tmp;
         Tmp = Int_Scalefac_Compress - 500;
         SLen[1] = Tmp/3;
         SLen[2] = Tmp%3;
         SLen[3] = 0;
         SLen[4] = 0;
         Info->Preflag[0][ch] = 1;
         Index = 2;
      }
   }

   {
      INT j, S;
      S = Info->Block[0][ch];
      if ( S>=1 ) S--;
      for( j=0; j<4; ++j )
         Nb_sfb[j+1] = HUFF_Spooky_Tab[ Spooky_Index ][ Index ][ S ][j];
   }

   if ( Info->Block[0][ch]<=1 )  // long or weird
   {
      INT sfb, j, k;
      sfb = 0;
      for( j=1; j<=4; ++j )
      {
         for( k=0; k<Nb_sfb[j]; ++k )
         {
            MP3_Scale_l[ch][sfb] = (USHORT)MP3_More_Bits( SLen[j] );
            if ( ch ) HUFF_IS_Max[sfb] = ( 1<<SLen[j] )-1;
            sfb++;
         }
      }
   }
   else if ( Info->Block[0][ch]==2 )   // short
   {
      INT sfb, j, k;
      sfb = 0;
      for( j=1; j<=4; ++j )
         for( k=0; k<Nb_sfb[j]; k+=3 )
         {
            MP3_Scale_s[ch][sfb][0] = (USHORT)MP3_More_Bits( SLen[j] );
            MP3_Scale_s[ch][sfb][1] = (USHORT)MP3_More_Bits( SLen[j] );
            MP3_Scale_s[ch][sfb][2] = (USHORT)MP3_More_Bits( SLen[j] );
            if ( ch ) HUFF_IS_Max[sfb+6] = ( 1<<SLen[j] )-1;
            sfb++;
         }
   }
   else     // mixed
   {
         // what we do here is:
         // 1. assume that for every fs, the two lowest subbands are equal to the
         //    six lowest scalefactor bands for long blocks/MPEG2. they are.
         // 2. assume that for every fs, the two lowest subbands are equal to the
         //    three lowest scalefactor bands for short blocks. they are.
         //
      INT sfb, j, k;
      sfb = 0;
      for( k=0; k<6; ++k )
      {
         MP3_Scale_l[ch][sfb] = (USHORT)MP3_More_Bits( SLen[1] );
         if ( ch ) HUFF_IS_Max[sfb] = ( 1<<SLen[1] )-1;
         sfb++;
      }
      Nb_sfb[1] -= 6;
      sfb = 3;
      for( j=1; j<=4; ++j )
         for( k=0; k<Nb_sfb[j]; k+=3 )
         {
            MP3_Scale_s[ch][sfb][0] = (USHORT)MP3_More_Bits( SLen[j] );
            MP3_Scale_s[ch][sfb][1] = (USHORT)MP3_More_Bits( SLen[j] );
            MP3_Scale_s[ch][sfb][2] = (USHORT)MP3_More_Bits( SLen[j] );
            if ( ch ) HUFF_IS_Max[sfb+6] = ( 1<<SLen[j] )-1;
            sfb++;
         }
   }
   
#if 0
   {
      INT k;
      fprintf( stderr, " o Scale_l[0/%d]:", ch );
      for( k=0; k<=21; ++k ) fprintf( stderr, "%d ", MP3_Scale_l[ch][k] );
      fprintf( stderr, "\n" );
   }
#endif

}
#endif   //   !_SKIP_MPEG2_ 

/*******************************************************/
/*                  HUFFMAN decoding                   */
/*******************************************************/

static BYTE HUFF_Atomic_Decode( UINT *H_Tab, BYTE *H_Cue )
{
   BYTE Len;
   UINT Chunk;

   Chunk = MP3_Fetch_Bits( 19 );
   H_Cue += Chunk>>HUFF_NC;
   H_Tab += H_Cue[ 0 ];
   Len = ( H_Tab[0]>>8 ) & 0xff;    // <= &0x1f, originally

         // heap me hard! ..or is it just Heap Hop ? :)

   if ( ( H_Tab[0]>>(32-Len) ) != (Chunk>>(19-Len)) )
   {
      UINT Lag, Half_Lag;

      Lag = H_Cue[1]-H_Cue[0];

      Chunk <<= 32-19;
      Chunk |= 0x01ff;

      Half_Lag = Lag>>1;
      H_Tab += Half_Lag;
      Lag -= Half_Lag;

      while( Lag>1 )
      {
         Half_Lag = Lag>>1;
         Lag -= Half_Lag;
         if ( H_Tab[0]<Chunk ) H_Tab += Half_Lag;
         else H_Tab -= Half_Lag;
      }

      Len = (H_Tab[0]>>8) & 0xff;      // <= &0x1f, originally
      if ( ( H_Tab[0]>>(32-Len) ) != (Chunk>>(32-Len)) )
      {
         if ( H_Tab[0]>Chunk ) H_Tab--;
         else H_Tab++;
         Len = (H_Tab[0]>>8) & 0xff;   // <= &0x1f, originally
      }
   }

   MP3_More_Bits( Len );
   return( (BYTE)H_Tab[0] );
}

/*******************************************************/

static INT Q_Mask[4] = { 0x10, 0x20, 0x40, 0x80 };

#define MIN(a,b)     ( (a)<(b)?(a):(b) )

EXTERN INT MP3_Huffman_Decode( MP3_INFO *Info, INT gr, INT ch, INT End_Pos )
{
   INT i;
   SHORT r[3], l;
   INT *Ptr;
   UINT *H_Tab;
   BYTE *H_Cue;

   l = 0; Ptr = HUFF_IS[ch];
   r[2] = Info->Big_Values[gr][ch] << 1;  // Big_Value;
   if ( r[2]==0 ) goto Skip;
   if ( Info->Block_Type[gr][ch]==0 )   
   {
      INT k = Info->Region0_Count[gr][ch];
      r[0] = MP3_Tab_l[ k ] + 1;
      r[1] = MP3_Tab_l[ k+1 + Info->Region1_Count[gr][ch] ] + 1;
      r[0] = MIN( r[0],r[2] ); 
      r[1] = MIN( r[1],r[2] );
   }
   else  // short/mixed
   {
      r[0] = MIN( 36, r[2] );     // always 36?
      r[1] = r[2];
   }

   for( i=0; i<3; ++i )
   {
      INT j;

      j = Info->Table_Select[gr][ch][i];  // Table selector
      H_Tab = HUFF_Tables[ j ];
      H_Cue = HUFF_Cue[ j ];
      j = HUFF_L_Inbits[ j ];

      if ( j>0 )
      {
         for( ; l<r[i]; l+=2 )
         {
            INT x;
            BYTE Tmp;

            Tmp = HUFF_Atomic_Decode( H_Tab, H_Cue );

            x = (Tmp>>4)&0x0F;         
            if ( x==0x0F ) x += MP3_More_Bits( j );
            if ( x ) if ( MP3_One_Bit() ) x = -x;
            Ptr[l] = x; 

            x = Tmp&0x0F;
            if ( x==0x0F ) x += MP3_More_Bits( j );
            if ( x ) if ( MP3_One_Bit() ) x = -x;
            Ptr[l+1] = x;
         }
      }
      else
      {
         for( ; l<r[i]; l+=2 )
         {
            INT x;
            BYTE Tmp;

            Tmp = HUFF_Atomic_Decode( H_Tab, H_Cue );

            x = (Tmp>>4)&0x0F;         
            if ( x ) if ( MP3_One_Bit() ) x = -x;
            Ptr[l] = x; 

            x = Tmp&0x0F;
            if ( x ) if ( MP3_One_Bit() ) x = -x;
            Ptr[l+1] = x;
         }
      }
   }
   Ptr += l;

Skip:
   i = 32 + Info->Count1_Table_Select[gr][ch];
   H_Tab = HUFF_Tables[ i ];
   H_Cue = HUFF_Cue[ i ];
   
   while ( ( MP3_Bit_Pos<End_Pos ) && ( l<576 ) )
   {
      INT i;
      BYTE x;
      
      x = HUFF_Atomic_Decode( H_Tab, H_Cue );
      l += 4; Ptr += 4;
      for( i=-4; i<0; ++i )
      {
         if ( x&Q_Mask[4+i] )
         {
            if ( MP3_One_Bit() ) Ptr[i] = -1;
            else Ptr[i] = 1;
         }
         else Ptr[i] = 0;
      }
   }

   End_Pos -= MP3_Bit_Pos;
   if ( End_Pos>0 ) // we're too short... jump to end
   {
      INT Tmp;
      Tmp = MP3_Bit_Pos & 0x07;
      MP3_Bit_Pos += End_Pos;
      Tmp = (End_Pos + Tmp)/8;
      if ( Tmp ) 
      {
         MP3_Buf += Tmp;
         MP3_Buf32 = (MP3_Buf[0]<<24)|(MP3_Buf[1]<<16)|(MP3_Buf[2]<<8)|MP3_Buf[3];
      }
   }

   i = MIN( 576,l );                // Total of valid data. can be equal to 578
   for( ; l<576; ++l ) *Ptr++ = 0;  // clean up the rest

   return( i );       // ?!? TODO: Fix that!
}

/*******************************************************/
/*******************************************************/
