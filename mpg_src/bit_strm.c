/***********************************************
 *         Bit-stream IO                       *
 *          Bit reading methods ...            *
 * Skal 98                                     *
 ***********************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "drv_io.h"
#include "bit_strm.h"
#include "./mp3.h"

// #define SAFE_24BITS

/*******************************************************/

EXTERN BIT_STRM *BSTRM_Cur = NULL;
EXTERN BSTRM_MTHDS BSTRM_Mthds = { 0 };   // where we plug current methods
EXTERN INT BSTRM_Pos = 0;
EXTERN INT BSTRM_Bit_Pos = 0;
EXTERN UINT BSTRM_Buf32 = 0;
EXTERN BYTE *BSTRM_Ptr = NULL;

EXTERN UINT BSTRM_Mask[] = {
 0x00000000, 0x00000001, 0x00000003, 0x00000007,
 0x0000000F, 0x0000001F, 0x0000003F, 0x0000007F,
 0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
 0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF,
 0x0000FFFF, 0x0001FFFF, 0x0003FFFF, 0x0007FFFF,
 0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF,
 0x00FFFFFF
#ifdef SAFE_24BITS
           , 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF,
 0x0FFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF,
 0xFFFFFFFF
#endif
};

/*******************************************************/
/*******************************************************/

            // TODO: Little/big endian pb here!?!

EXTERN INT BSTRM_Init_Dflt( BIT_STRM *Bits )
{
   Bits->Raw_Pos = 0;
   Bits->Bit_Pos = 0;
   Bits->Available_Bytes = 0;
   Bits->Raw_Len = 0;

      // 1024 additional padding bytes here, 
      // for safety with BSTRM_Buf32...
      // (in case stream's last frame is weird) 
   Bits->Raw_Ptr = Bits->Raw = 
      New_Fatal_Object( Bits->File_Len+2048, BYTE );
   if ( Bits->Raw==NULL ) return( BSTRM_MEM_OUT );

   if ( F_READ( Bits->Raw, Bits->File, Bits->File_Len )!=1 )
      return( BSTRM_IO_ERR );
   Bits->Available_Bytes = Bits->Raw_Len = Bits->File_Len;
   Bits->Available_Bits = Bits->Available_Bytes * sizeof( BYTE ) * 8;
   F_CLOSE( Bits->File );
   Bits->File = NULL;
   Bits->Buf32 = Bits->Raw[0]<<24;
   if ( Bits->Raw_Len>0 ) Bits->Buf32 |= Bits->Raw[1]<<16;
   if ( Bits->Raw_Len>1 ) Bits->Buf32 |= Bits->Raw[2]<<8;
   if ( Bits->Raw_Len>2 ) Bits->Buf32 |= Bits->Raw[3];
   return( BSTRM_OK );
}

EXTERN INT BSTRM_Select_Dflt( BIT_STRM *Bits )
{
   BSTRM_Cur = Bits;
   BSTRM_Mthds = Bits->Mthds;
   BSTRM_Bit_Pos = Bits->Bit_Pos;                  // at this point, Bit_Pos is in [0..7]
   BSTRM_Ptr = Bits->Raw_Ptr + (BSTRM_Bit_Pos/8);  // anyway, better be cautious :)
   BSTRM_Buf32 = Bits->Buf32;

   return( BSTRM_OK );
}

EXTERN INT BSTRM_Un_Select_Dflt( BIT_STRM *Bits )
{
   Bits->Raw_Ptr = BSTRM_Ptr;
   Bits->Bit_Pos = BSTRM_Bit_Pos&0x07;
   Bits->Buf32   = BSTRM_Buf32;

   return( BSTRM_OK );
}

EXTERN UINT BSTRM_More_Bits_Dflt( INT How_Many )   
{
   UINT Ret;
   INT Bits_Left;

   if ( !How_Many ) return( 0 );
   Bits_Left = BSTRM_Bit_Pos & 0x07;

   Ret = BSTRM_Buf32<<Bits_Left;
#ifdef SAFE_24BITS
   if ( How_Many>24 ) // TODO: really? need more than 24 bits at a time??
   {
      UINT Add;
      Add = BSTRM_Ptr[4];
      Add &= BSTRM_Mask[How_Many-24];
      ((UINT*)&Ret)[3] |= Add << Bits_Left;
   }
#endif

   Ret >>= 32-How_Many;
   Queue_State( "Q:", Ret, How_Many );

   BSTRM_Bit_Pos += How_Many;
   Bits_Left = 8-Bits_Left - How_Many;
   while( Bits_Left<=0 ) { 
      BSTRM_Ptr++; Bits_Left+=8;
      BSTRM_Buf32 <<= 8; BSTRM_Buf32 |= BSTRM_Ptr[3];
   }

//   Ret &= BSTRM_Mask[How_Many];
   return( Ret );
}

EXTERN BYTE BSTRM_One_Bit_Dflt( )
{
   BYTE Ret;
   BYTE Bits_Left;

   Bits_Left = (BSTRM_Bit_Pos & 0x07) ^ 0x07;
   Ret = BSTRM_Ptr[0]>>Bits_Left;
   if ( !Bits_Left ) { 
      BSTRM_Ptr++; 
      BSTRM_Buf32 <<= 8; BSTRM_Buf32 |= BSTRM_Ptr[3];
   }
   BSTRM_Bit_Pos++;
   return( Ret&0x01 );
}

EXTERN UINT BSTRM_Fetch_Bits_Dflt( INT How_Many )
{
   UINT Ret;
   INT Bits_Left;

   if ( !How_Many ) return( 0 );
   Bits_Left = BSTRM_Bit_Pos & 0x07;
   Ret = BSTRM_Buf32 << Bits_Left;

#ifdef SAFE_24BITS
   if ( How_Many>24 )
   {
      UINT Add;
      Add = BSTRM_Ptr[4];
      Add &= BSTRM_Mask[How_Many-24];
      ((UINT*)&Ret)[3] |= Add << Bits_Left;
   }
#endif
   Ret >>= 32-How_Many;
   Queue_State( "FQueue", Ret, How_Many );

//   Ret &= BSTRM_Mask[How_Many];
   return( Ret );
}

EXTERN UINT BSTRM_Get_Bytes_Dflt( BYTE *Where, INT How_Many )
{
   INT i;

   if ( !How_Many ) return( 0 );
   if ( (BSTRM_Bit_Pos&0x07)!=0 )
      How_Many++;
   BSTRM_Ptr += How_Many;
   Where += How_Many;
   for( i=-How_Many; i<0; ++i ) Where[i] = BSTRM_Ptr[i];
   BSTRM_Bit_Pos += How_Many*8;
   BSTRM_Buf32  = BSTRM_Ptr[0]<<24;
   BSTRM_Buf32 |= BSTRM_Ptr[1]<<16;
   BSTRM_Buf32 |= BSTRM_Ptr[2]<<8;
   BSTRM_Buf32 |= BSTRM_Ptr[3];
   return( How_Many );
}

EXTERN INT BSTRM_Close_Dflt( BIT_STRM *Bits )
{
   M_Free( Bits->Raw );
   Bits->Raw_Ptr = NULL;
   Bits->Available_Bytes = Bits->Raw_Len = 0;
   Bits->Raw_Pos = 0;
   Bits->Bit_Pos = 0;
   Bits->Buf32   = 0;
   return( BSTRM_OK );
}

EXTERN BYTE BSTRM_Get_One_Byte( )
{
   BYTE Ret;

   Ret = (BYTE)( BSTRM_Buf32>>24 );
   BSTRM_Bit_Pos = (BSTRM_Bit_Pos+8) & ~0x07;
   BSTRM_Ptr++; 
   BSTRM_Buf32 <<= 8; 
   BSTRM_Buf32 |= BSTRM_Ptr[3];
   return( Ret );
}

EXTERN USHORT BSTRM_Get_One_Word( )
{
   USHORT Ret;

   Ret = (USHORT)( BSTRM_Buf32>>16 );
   BSTRM_Bit_Pos = (BSTRM_Bit_Pos+16) & ~0x0F;
   BSTRM_Ptr += 2; 
   BSTRM_Buf32 <<= 16; 
   BSTRM_Buf32 |= (BSTRM_Ptr[2]<<8) | BSTRM_Ptr[3];
   return( Ret );
}

/*******************************************************/

#if 0
EXTERN void BSTRM_Sync( )
{
   if ( (BSTRM_Bit_Pos&0x07) != 0 )
   {
      BSTRM_Bit_Pos = (BSTRM_Bit_Pos | 0x07 ) + 1;
      BSTRM_Ptr++;
      BSTRM_Buf32<<=8;
      BSTRM_Buf32 |= BSTRM_Ptr[3];
   }
}
#endif

/*******************************************************/

EXTERN BSTRM_MTHDS BSTRM_Dflt_Methods = {
   BSTRM_Init_Dflt, BSTRM_Select_Dflt, BSTRM_Un_Select_Dflt,
   BSTRM_More_Bits_Dflt,
   BSTRM_One_Bit_Dflt,
   BSTRM_Get_One_Byte,
   BSTRM_Get_One_Word,
   BSTRM_Fetch_Bits_Dflt,
   BSTRM_Get_Bytes_Dflt,
   NULL, NULL,
   BSTRM_Close_Dflt
};

/*******************************************************/
/*******************************************************/
/*******************************************************/

EXTERN BIT_STRM *BSTRM_Init( BIT_STRM *Bits, STRING Name )
{
   BIT_STRM *New;

   if ( Bits==NULL )
   {
      New = Bits = New_Fatal_Object( 1, BIT_STRM );
      if ( New==NULL ) return( NULL );
   }
   else New = NULL;

   Mem_Clear( Bits );

   Bits->File = Access_File( Name, READ_SWITCH );
   if ( Bits->File==NULL ) goto Failed;
   F_SEEK( Bits->File, 0, SEEK_END );
   Bits->File_Len = F_TELL( Bits->File );
   F_SEEK( Bits->File, 0, SEEK_SET );

      // defaults
   Bits->Mthds = BSTRM_Dflt_Methods;
   return( Bits );

Failed:
   M_Free( New );
   if ( Bits->File != NULL ) F_CLOSE( Bits->File );
   return( NULL );
}

EXTERN void BSTRM_Destroy( BIT_STRM *Strm )
{
   BSTRM_CLOSE( Strm );
   if ( Strm->File != NULL ) F_CLOSE( Strm->File );
   Strm->File = NULL;
   Strm->File_Len = 0;
   if ( BSTRM_Cur==Strm ) BSTRM_Cur = NULL;
}

/*******************************************************/

EXTERN void BSTRM_Select( BIT_STRM *Bits )
{
      // set global variables to avoid passing too much params when decoding

   if ( BSTRM_Cur!=NULL )
      if ( BSTRM_Cur->Mthds.Un_Select!=NULL )
         (*BSTRM_Cur->Mthds.Un_Select)( BSTRM_Cur );

    BSTRM_Cur = Bits;
    if ( Bits==NULL ) return;
    if ( Bits->Mthds.Select!=NULL ) (*Bits->Mthds.Select)( Bits );
}

/*******************************************************/

