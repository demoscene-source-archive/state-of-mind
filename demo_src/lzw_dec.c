/***********************************************
 *              LZW decoder                    *
 * Skal 96                                     *
 ***********************************************/

#include "lzw.h"
#include "drv_io.h"

/*******************************************************/

static BYTE *Fill_Byte_Buffer( FILE *In )
{
   BYTE C;

   if ( F_READ( &C, In, 1 ) != 1 ) return( NULL );
   LZW_Nb_Avail_Bytes = (INT)C;
   if ( LZW_Nb_Avail_Bytes<0 ) return( NULL );
   else if ( LZW_Nb_Avail_Bytes>0 )
      if ( F_READ( LZW_Byte_Buffer, In, LZW_Nb_Avail_Bytes ) != 1 )
         return( NULL );

   return( LZW_Byte_Buffer );
}

static SHORT Get_Next_Code( FILE *In )
{
   USHORT Ret;

   if ( !LZW_Nb_Bits_Left )
   {
      if ( LZW_Nb_Avail_Bytes<=0 )
      {
         LZW_Bytes_Ptr = Fill_Byte_Buffer( In );
         if ( LZW_Bytes_Ptr == NULL ) return( -1 );
      }
      LZW_Cur_Byte = (UINT)( *LZW_Bytes_Ptr++ )&0xFF;
      Ret = (USHORT)LZW_Cur_Byte;
      LZW_Nb_Bits_Left = 8;
      --LZW_Nb_Avail_Bytes;
   }
   else Ret = (USHORT)( LZW_Cur_Byte >> ( 8-LZW_Nb_Bits_Left ) );

   while( LZW_Curr_Size>LZW_Nb_Bits_Left )
   {
      if ( LZW_Nb_Avail_Bytes<=0 )
      {
         LZW_Bytes_Ptr = Fill_Byte_Buffer( In );
         if ( LZW_Bytes_Ptr == NULL ) return( -1 );
      }
      LZW_Cur_Byte = (UINT)( *LZW_Bytes_Ptr++ )&0xFF;
      Ret |= (USHORT)( LZW_Cur_Byte << LZW_Nb_Bits_Left );
      LZW_Nb_Bits_Left += 8;
      --LZW_Nb_Avail_Bytes;
   }
   LZW_Nb_Bits_Left -= LZW_Curr_Size;
   Ret &= LZW_Code_Masks[ LZW_Curr_Size ];
   return( (SHORT)Ret );
}

/*******************************************************/

EXTERN INT LZW_Decoder( BYTE *Out_Bits, FILE *In, INT Total_Size, INT Size )
{
   INT Code, Top_Slot, Slot, Clear_Code, Stack_Count;
   BYTE Buffer[ 256 ];
   BYTE D_Stack[ LZW_MAX_CODES ];
   BYTE Suffix[ LZW_MAX_CODES ];
   USHORT Prefix[ LZW_MAX_CODES ];

   Clear_Code = 1 << Size;
   LZW_Curr_Size = Size + 1;

   LZW_Byte_Buffer = Buffer;
   LZW_Nb_Avail_Bytes = LZW_Nb_Bits_Left = 0;
   Stack_Count = LZW_MAX_CODES;

   Top_Slot = 1 << LZW_Curr_Size;
   Slot = Clear_Code + 2;

   while( ( Code=Get_Next_Code( In ) ) != Clear_Code+1 )
   {
      SHORT OCode, FCode;
      if ( Code<0 ) return( TRUE );
      if ( Code == Clear_Code )
      {
         LZW_Curr_Size = Size + 1;
         Slot = Clear_Code + 2;
         Top_Slot = 1<<LZW_Curr_Size;         
         while( Code == Clear_Code ) Code = Get_Next_Code( In );
         if ( Code == Clear_Code+1 ) break;
         // if ( Code>=Slot ) Code = 0;  <= Shouldn't happen ;)
         OCode = FCode = (SHORT)Code;
         if ( Total_Size-- ) *Out_Bits++ = (BYTE)Code;
         else goto Premature_EOF;
      }
      else 
      {
         SHORT Code_Save;

         Code_Save = (SHORT)Code;
         if ( Code >= Slot )
         {
            Code = OCode;
            D_Stack[ --Stack_Count ] = (BYTE)FCode;
         }
         while( Code >= Clear_Code+2 )
         {
            D_Stack[ --Stack_Count ] = Suffix[ Code ];
            Code = Prefix[ Code ];
         }
         D_Stack[ --Stack_Count ] = (BYTE)Code;

         Suffix[ Slot ] = (BYTE)( FCode = (SHORT)Code );
         Prefix[ Slot++ ] = OCode; OCode = Code_Save;

         if ( ( Slot==Top_Slot ) && ( LZW_Curr_Size<LZW_MAX_BITS ) )
         {
            Top_Slot <<= 1;
            ++LZW_Curr_Size;
         }

         Total_Size -= LZW_MAX_CODES-Stack_Count;
//         if ( Total_Size<0 ) goto Premature_EOF;
         while( Stack_Count<LZW_MAX_CODES )
            *Out_Bits++ = D_Stack[ Stack_Count++ ];         
      }
   }
   return( FALSE );  // ok...

Premature_EOF:
   // Out_Message( "Premature EOF reading LZW data" );
   return( FALSE );
}

/*******************************************************/

