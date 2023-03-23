/***********************************************
 *              LZW encoder                    *
 * Skal 96                                     *
 ***********************************************/

#include "lzw.h"
#include "drv_io.h"

/*******************************************************/
/*******************************************************/   

static INT Output_Char( INT C, FILE *Out )
{
   INT Size;
   
   Size = 0;
   if ( C != -1 )
      LZW_Byte_Buffer[ LZW_Nb_Avail_Bytes++ ] = ( BYTE )C;
   else if ( !LZW_Nb_Avail_Bytes ) return( Size );
   if ( ( LZW_Nb_Avail_Bytes == 254 ) || ( C==-1 ) )
   {
      if ( Out != NULL )
      {
         fputc( (BYTE)LZW_Nb_Avail_Bytes, Out );
         fwrite( LZW_Byte_Buffer, LZW_Nb_Avail_Bytes, 1, Out );
      }
      Size = LZW_Nb_Avail_Bytes+1;
      LZW_Nb_Avail_Bytes = 0;
      memset( LZW_Byte_Buffer, 0, 256 );
   }
   return( Size );
}

static INT Out_Next_Code( INT Code, FILE *Out )
{
   INT Size;

   Size = 0;
   if ( LZW_Nb_Bits_Left ) Code <<= LZW_Nb_Bits_Left;
   LZW_Cur_Byte |= Code;
   LZW_Nb_Bits_Left += LZW_Curr_Size;   
   if ( LZW_Nb_Bits_Left<8 ) goto End;

   do {
      Size += Output_Char( LZW_Cur_Byte&0xFF, Out );
      LZW_Cur_Byte >>= 8;
      LZW_Nb_Bits_Left -= 8;
   } while ( LZW_Nb_Bits_Left>=8 );
End:
   LZW_Cur_Byte &= (INT)( LZW_Code_Masks[ LZW_Nb_Bits_Left ] );
   return( Size );
}

EXTERN INT LZW_Encoder( BYTE *In_Bits, FILE *Out, INT Total_Size, INT Init_Size )
{
   INT Slot, Top_Slot, Total;
   INT HTab[ LZW_HASH ], Entry, Clear_Code;
   SHORT Code_Tab[ LZW_HASH ];
   BYTE Buffer[ 256 ];

   LZW_Nb_Avail_Bytes = LZW_Nb_Bits_Left = 0;
   LZW_Cur_Byte = 0; LZW_Byte_Buffer = Buffer;

   for( Slot=0; Slot<LZW_HASH; Slot++ ) HTab[Slot] = -1;   // Clear_Hash_Table

   Clear_Code = 1 << Init_Size;
   Slot = Clear_Code + 2;
   LZW_Curr_Size = Init_Size + 1;
   Top_Slot = 1 << LZW_Curr_Size;

   Total = Out_Next_Code( Clear_Code, Out );
   Entry = (INT)( *In_Bits++ ); --Total_Size;
   while( 1 )
   {
      INT i, j, Code, Hash_Code; 

      if ( Total_Size-- <= 0 ) break;
      Code = (INT)( *In_Bits++ );

      Hash_Code = ( Code<<LZW_MAX_BITS ) + Entry;
      i = ( Code<<LZW_HSHIFT ) ^ Entry;

      if ( HTab[i] == Hash_Code )
      {
         Entry = Code_Tab[ i ];
         continue;
      }
      else if ( HTab[i]<0 ) goto No_Match; // Empty slot

      if ( !i ) j = 1;
      else j = LZW_HASH-i;

Probe:
      i -= j;
      if ( i<0 ) i += LZW_HASH;

      if ( HTab[i] == Hash_Code )
      {
         Entry = Code_Tab[ i ];
         continue;
      }
      if ( HTab[i]>0 ) goto Probe;

No_Match:
      Total += Out_Next_Code( Entry, Out );
      Entry = Code;

      if ( Slot<Top_Slot )
      {
         HTab[i] = Hash_Code;
         Code_Tab[i] = (SHORT)Slot++;
         continue;
      }
      if ( LZW_Curr_Size==LZW_MAX_BITS )
      {
         Total += Out_Next_Code( Clear_Code, Out );
         for( i=LZW_HASH-1; i>=0; --i ) HTab[i] = -1;
         Slot = Clear_Code + 2;
         LZW_Curr_Size = Init_Size;
      }
      else Slot++;
      LZW_Curr_Size++;
      Top_Slot = 1 << LZW_Curr_Size;

      // Out_Message( "Curr_Size: %d \tSlot: %d \tTop_Slot: %d   \tTotal_Size: %d",
      //   LZW_Curr_Size, Slot, Top_Slot, Total_Size );
   }
   Total += Out_Next_Code( Entry, Out );
   Total += Out_Next_Code( Clear_Code+1, Out );    // EOF Code
   if ( LZW_Nb_Bits_Left ) Total += Output_Char( LZW_Cur_Byte&0xFF, Out );
   Total += Output_Char( -1, Out );                // Flush left over in buffer

   return( Total );
}

/*******************************************************/

