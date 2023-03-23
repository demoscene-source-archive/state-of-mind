/***********************************************
 *              LZW stream decoder             *
 * Skal 97                                     *
 ***********************************************/

#include "lzw.h"
#include "drv_io.h"

/*******************************************************/

EXTERN void Rewind_LZW_Stream( LZW_STREAM *Strm )
{
   if ( Strm==NULL ) return;
   Strm->LZW_Curr_Size = 0;   // means: start new sub-block.
   Strm->Src_Pos = 0;
}

EXTERN LZW_STREAM *Init_LZW_Stream( BYTE *Data, LZW_STREAM *Strm )
{
   if ( Strm==NULL )
   {
      Strm = New_Object( 1, LZW_STREAM );
      if ( Strm==NULL ) return( NULL );
      Mem_Clear( Strm );
   }

   Strm->D_Stack = New_Object( LZW_MAX_CODES*4, BYTE );
   if ( Strm->D_Stack==NULL ) 
      Exit_Upon_Mem_Error( "LZW buffers", LZW_MAX_CODES*4 );
   Strm->Suffix = Strm->D_Stack + LZW_MAX_CODES;
   Strm->Prefix = (USHORT *)( Strm->Suffix + LZW_MAX_CODES );

   if ( Data != NULL )
   {
      INT Size;

      Strm->LZW_Bytes_Ptr = Data+4;
      Size = (INT)Data[0];
      Size += (INT)Data[1]*256;
      Size += (INT)Data[2]*256*256;
      Size += (INT)Data[3]*256*256*256;
      Strm->Total = Size;
      Strm->LZW_Bytes_Ptr = Data+4;
   }
   else
   {
      Strm->LZW_Bytes_Ptr = NULL;
      Strm->Total = 0;
   }
   Rewind_LZW_Stream( Strm );    // means: start new sub-block.

   return( Strm );
}

EXTERN void Close_LZW_Stream( LZW_STREAM *Strm )
{
   if ( Strm==NULL ) return;
   M_Free( Strm->D_Stack );
   Rewind_LZW_Stream( Strm );
   Strm->LZW_Bytes_Ptr = NULL;  // user must free *LZW_Bytes_Ptr himself
   Strm->Total = 0;
}

/*******************************************************/

static BYTE *Fill_Byte_Buffer( LZW_STREAM *Strm )
{
   Strm->LZW_Nb_Avail_Bytes = (INT)Strm->LZW_Bytes_Ptr[ Strm->Src_Pos++ ];
   if ( Strm->LZW_Nb_Avail_Bytes<0 ) return( NULL );
   return( &Strm->LZW_Bytes_Ptr[Strm->Src_Pos] );
}

static SHORT Get_Next_Code( LZW_STREAM *Strm )
{
   USHORT Ret;

   if ( !Strm->LZW_Nb_Bits_Left )
   {
      if ( Strm->LZW_Nb_Avail_Bytes<=0 )
      {
         if ( Fill_Byte_Buffer( Strm )==NULL ) return( -1 );
      }
      Strm->LZW_Cur_Byte = (UINT)( Strm->LZW_Bytes_Ptr[Strm->Src_Pos++] ) & 0xFF;
      Ret = (USHORT)Strm->LZW_Cur_Byte;
      Strm->LZW_Nb_Bits_Left = 8;
      --Strm->LZW_Nb_Avail_Bytes;
   }
   else Ret = (USHORT)( Strm->LZW_Cur_Byte >> ( 8-Strm->LZW_Nb_Bits_Left ) );

   while( Strm->LZW_Curr_Size>Strm->LZW_Nb_Bits_Left )
   {
      if ( Strm->LZW_Nb_Avail_Bytes<=0 )
      {
         if ( Fill_Byte_Buffer( Strm ) == NULL ) return( -1 );
      }
      Strm->LZW_Cur_Byte = (UINT)( Strm->LZW_Bytes_Ptr[Strm->Src_Pos++] )&0xFF;
      Ret |= (USHORT)( Strm->LZW_Cur_Byte << Strm->LZW_Nb_Bits_Left );
      Strm->LZW_Nb_Bits_Left += 8;
      --Strm->LZW_Nb_Avail_Bytes;
   }
   Strm->LZW_Nb_Bits_Left -= Strm->LZW_Curr_Size;
   Ret &= LZW_Code_Masks[ Strm->LZW_Curr_Size ];
   return( (SHORT)Ret );
}

/*******************************************************/

EXTERN INT Decode_LZW_Stream( LZW_STREAM *Strm, BYTE *Where, INT How_Much )
{
   INT Nb_Read;

   if ( Strm==NULL || Strm->LZW_Bytes_Ptr==NULL ) return( 0 );

   if ( Strm->LZW_Curr_Size==0 )    // Init sub-block.
   {
         // New sub-block

      INT Size;

      Size = Strm->LZW_Bytes_Ptr[ Strm->Src_Pos++ ];
      if ( Size==0x02 )
      {
            // LZW block

         Size = Strm->LZW_Bytes_Ptr[ Strm->Src_Pos++ ];
         Strm->Size = (SHORT)Size;
         Strm->LZW_Nb_Avail_Bytes = Strm->LZW_Nb_Bits_Left = 0;
         Strm->Clear_Code = 1 << Size;
         Strm->LZW_Curr_Size = Size + 1;
         Strm->Stack_Count = LZW_MAX_CODES;

// useless, since stream SHOULD start with a Clear_Code
//         Strm->Top_Slot = 1 << Strm->LZW_Curr_Size;
//         Strm->Slot = Strm->Clear_Code + 2;
         
      }
      else
      {
         INT Size;
            // Raw block
         Size = (INT)Strm->LZW_Bytes_Ptr[Strm->Src_Pos++];
         Size += (INT)Strm->LZW_Bytes_Ptr[Strm->Src_Pos++] * 256;
         Size += (INT)Strm->LZW_Bytes_Ptr[Strm->Src_Pos++] * 256*256;
         Size += (INT)Strm->LZW_Bytes_Ptr[Strm->Src_Pos++] * 256*256*256;
         Strm->LZW_Nb_Avail_Bytes = Size;
         Strm->LZW_Curr_Size = 1;
      }
   }

         // Raw uncompressed block

   if ( Strm->LZW_Curr_Size==1 )
   {
      if ( How_Much<0 )
         How_Much = Strm->LZW_Nb_Avail_Bytes - Strm->Src_Pos;
      else if ( Strm->Src_Pos + How_Much > Strm->LZW_Nb_Avail_Bytes )
         Nb_Read = Strm->LZW_Nb_Avail_Bytes - Strm->Src_Pos;
      memcpy( Where, Strm->LZW_Bytes_Ptr + Strm->Src_Pos, Nb_Read );
      Strm->Src_Pos += Nb_Read;
      if ( Strm->Src_Pos==Strm->LZW_Nb_Avail_Bytes )
         goto Finished;
      else goto End;
   }

         // LZW-compressed block

   Nb_Read = 0;
   if ( How_Much<0 ) How_Much = 0x7FFFFFFF;  // =>read maximum available...
   while( Nb_Read<How_Much )
   {
      SHORT Code;

      while( Strm->Stack_Count<LZW_MAX_CODES )
      {
            // Still got some stuff to transfert from stack...
         Where[ Nb_Read++ ] = Strm->D_Stack[ Strm->Stack_Count++ ];
         if ( Nb_Read==How_Much )
            goto End;
      }

      Code = Get_Next_Code( Strm );
      if ( Code == Strm->Clear_Code+1 )   // => Finished.
         goto Finished;
      if ( Code<0 ) return( -1 );   // Error...
      if ( Code == Strm->Clear_Code )
      {
         Strm->LZW_Curr_Size = Strm->Size + 1;
         Strm->Slot = Strm->Clear_Code + 2;
         Strm->Top_Slot = 1<<Strm->LZW_Curr_Size;         
         while( Code == Strm->Clear_Code )
            Code = Get_Next_Code( Strm );
         if ( Code == Strm->Clear_Code+1 )
            goto Finished;   // => finished.

         // if ( Code>=Slot ) Code = 0;  <= Shouldn't happen ;)

         Strm->OCode = Strm->FCode = Code;
         Where[Nb_Read++] = (BYTE)Code;
      }
      else 
      {
         SHORT Code_Save;

         Code_Save = Code;
         if ( Code >= Strm->Slot )
         {
            Code = Strm->OCode;
            Strm->D_Stack[ --Strm->Stack_Count ] = (BYTE)Strm->FCode;
         }
         while( Code >= Strm->Clear_Code+2 )
         {
            Strm->D_Stack[ --Strm->Stack_Count ] = Strm->Suffix[ Code ];
            Code = Strm->Prefix[ Code ];
         }
         Strm->D_Stack[ --Strm->Stack_Count ] = (BYTE)Code;

         Strm->Suffix[ Strm->Slot ] = (BYTE)( Strm->FCode = Code );
         Strm->Prefix[ Strm->Slot++ ] = Strm->OCode;
         Strm->OCode = Code_Save;

         if ( (Strm->Slot==Strm->Top_Slot) && 
              (Strm->LZW_Curr_Size<LZW_MAX_BITS) )
         {
            Strm->Top_Slot <<= 1;
            ++Strm->LZW_Curr_Size;
         }

         while( Strm->Stack_Count<LZW_MAX_CODES )
         {
            Where[Nb_Read++] = Strm->D_Stack[ Strm->Stack_Count++ ];
            if ( Nb_Read==How_Much ) goto End;
         }
      }      
   }
End:
         // it's now user's responsability to add Nb_Read
         // to *Where before next call...

   return( Nb_Read );

Finished:
   if ( Strm->Src_Pos==Strm->Total )
      Rewind_LZW_Stream( Strm );
   else Strm->LZW_Curr_Size = 0;
   return( Nb_Read );
}

/*******************************************************/

