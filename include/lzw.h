/***********************************************
 *              Compress data                  *
 * Skal 96                                     *
 ***********************************************/

#ifndef _LZW_H_
#define _LZW_H_

#include "main.h"

/*******************************************************/

#define LZW_MAX_CODES 4096
#define LZW_MAX_BITS 12
#define LZW_HSHIFT 4
#define LZW_HASH ( 5003 )

   // in lzw.c

extern INT LZW_Curr_Size, LZW_Nb_Avail_Bytes, LZW_Nb_Bits_Left;
extern BYTE *LZW_Byte_Buffer;
extern USHORT LZW_Code_Masks[ ];
extern UINT LZW_Cur_Byte;
extern BYTE *LZW_Bytes_Ptr;

   // in lzw_strm.c

typedef struct LZW_STREAM LZW_STREAM;
struct LZW_STREAM
{
#pragma pack(1)

   INT     LZW_Curr_Size;
   INT     LZW_Nb_Avail_Bytes;
   INT     LZW_Nb_Bits_Left;
   UINT    LZW_Cur_Byte;
   BYTE   *LZW_Bytes_Ptr;
   INT     Top_Slot, Slot, Clear_Code, Stack_Count;
   SHORT   OCode, FCode, Size;
   INT     Src_Pos, Total;
   BYTE   *D_Stack;
   BYTE   *Suffix;
   USHORT *Prefix;

};


extern LZW_STREAM *Init_LZW_Stream( BYTE *Data, LZW_STREAM *Strm );
extern void Rewind_LZW_Stream( LZW_STREAM *Strm );
extern void Close_LZW_Stream( LZW_STREAM *Strm );
extern INT Decode_LZW_Stream( LZW_STREAM *Strm, BYTE *Where, INT How_Much );

/*******************************************************/

extern INT LZW_Encoder( BYTE *, FILE *, INT, INT );
extern INT LZW_Decoder( BYTE *, FILE *, INT, INT );

/*******************************************************/

#endif   // _LZW_H_
