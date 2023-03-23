/***********************************************
 *            Bit stream IO                    *
 * Skal 98                                     *
 ***********************************************/

#ifndef _BIT_STRM_H_
#define _BIT_STRM_H_

#include "main.h"

#define BSTRM_VERSION 002

/*******************************************************/

typedef struct BIT_STRM BIT_STRM;
typedef struct {
#pragma pack(1)

   INT    (*Init)( BIT_STRM * );
   INT    (*Select)( BIT_STRM * );
   INT    (*Un_Select)( BIT_STRM * );
   UINT   (*Get_More_Bits)( INT How_Many );
   BYTE   (*One_Bit)( );
   BYTE   (*One_Byte)( );
   USHORT (*One_Word)( );
   UINT   (*Fetch_Bits)( INT How_Many );
   UINT   (*Get_Bytes)( BYTE *Where, INT How_Many );
   INT    (*Decode_I)( );
   INT    (*Decode_II)( );
   INT    (*Close)( BIT_STRM * );

} BSTRM_MTHDS;

struct BIT_STRM {
#pragma pack(1)
   INT   Bit_Pos;
   BYTE *Raw_Ptr, *Raw;
   UINT  Buf32;

   INT   Available_Bytes;
   INT   Available_Bits;
   INT   Raw_Pos;
   INT   Raw_Len;

   FILE *File;
   INT   File_Len;
   BSTRM_MTHDS Mthds;

};

/*******************************************************/

#define BSTRM_OK       0
#define BSTRM_EOF      1
#define BSTRM_MEM_OUT  2
#define BSTRM_IO_ERR   3

#define BSTRM_GET_BITS(n)    ( (BSTRM_Mthds.Get_More_Bits)( (n) ) )
#define BSTRM_ONE_BIT()      ( (BSTRM_Mthds.One_Bit)( ) )
#define BSTRM_ONE_BYTE()     ( (BSTRM_Mthds.One_Byte)( ) )
#define BSTRM_ONE_WORD()     ( (BSTRM_Mthds.One_Word)( ) )
#define BSTRM_FETCH_BITS(n)  ( (BSTRM_Mthds.Fetch_Bits)( (n) ) )
#define BSTRM_DO_INIT(B)     ( *(B)->Mthds.Init)( (B) )
#define BSTRM_SELECT(B)      ( *(B)->Mthds.Select)( (B) )
#define BSTRM_UN_SELECT(B)   ( *(B)->Mthds.Un_Select)( (B) )
#define BSTRM_CLOSE(B)       ( *(B)->Mthds.Close)( (B) )
#define BSTRM_GET_BYTES(W,N) ( (BSTRM_Mthds.Get_Bytes)( (W), (N) ) )
#define BSTRM_ONE_BIT()      ( (BSTRM_Mthds.One_Bit)( ) )

/*******************************************************/

      // in bit_strm.c

extern BIT_STRM *BSTRM_Cur;
extern BSTRM_MTHDS BSTRM_Mthds;
extern INT BSTRM_Pos;
extern INT BSTRM_Bit_Pos;
extern BYTE *BSTRM_Ptr;
extern UINT BSTRM_Buf32;
extern UINT BSTRM_Mask[];

extern BSTRM_MTHDS BSTRM_Dflt_Methods;
extern BSTRM_MTHDS BSTRM_File_Methods;

extern void BSTRM_Sync( );

         // high level calls

extern BIT_STRM *BSTRM_Init( BIT_STRM *Bits, STRING Name );
extern void BSTRM_Select( BIT_STRM *Bits );
extern void BSTRM_Destroy( BIT_STRM *Strm );

/*******************************************************/

#endif   // _BIT_STRM_H_
