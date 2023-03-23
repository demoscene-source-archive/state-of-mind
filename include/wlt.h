/***********************************************
 *              Wavelet stuff                  *
 * Skal 98                                     *
 ***********************************************/

#ifndef _WLT_H_
#define _WLT_H_

#include "main.h"

/*******************************************************/

#define WLT_2D
// #define WLT_LOG         // LOG (ie:slow) compression ?

// #define WLT_8BITS
// #define WLT_OLD_PAK
#define MAGIC_WLT "Wlt"
#define WLT_VERSION  0.1
#define WLT_OFFSET   1.0

/*******************************************************/

typedef struct {
#pragma pack(1)
   INT BufX, BufY, SizeX, SizeY;
   INT Nb_Frames;
   USHORT **RLE;
   FLT *Main_Buffer, *Buf2, *Buf3;
   USHORT *Frame;
   INT Nb_Col;
   PIXEL CMap[768]; 
} WLT_STREAM;

   // in wlt_strm.c

extern WLT_STREAM *Init_Wavelet_Stream( WLT_STREAM *Strm,
   INT SizeX, INT SizeY, INT Nb_Frames );
extern void Clear_Wavelet_Stream( WLT_STREAM *Strm );
extern void Destroy_Wavelet_Stream( WLT_STREAM *Strm );

extern FLT *Wavelet_Decode_Frame( WLT_STREAM *Strm, INT N );
extern WLT_STREAM *Load_Wavelet_Stream( STRING Name );

   // in wlt_enc.c

extern INT Save_Wavelet_Stream( WLT_STREAM *Strm, STRING Name );

/*******************************************************/

   // in wlt_enc.c

extern FLT *Wavelet_Encode_Stream( WLT_STREAM *Strm );
extern FLT *Wavelet_Encoder( FLT *Where, FLT *Src, INT Size );
extern FLT *Wavelet_Encoder_2D( FLT *Buf1, FLT *Buf2, FLT *Src, INT N, INT M );
extern void Do_Daub4( FLT *Dst, FLT *Src, INT Len );

extern FLT Decimate_Wavelet( FLT *C, INT Size, FLT Thresh );
extern USHORT Wavelet_Flt_To_16bits( USHORT *Dst, FLT *Src, 
   FLT *Max, FLT *Min, INT Size );
extern INT Wavelet_To_RLE( USHORT *Dst, USHORT *Src, 
   USHORT Key, FLT Max, FLT Min, INT Size );

#ifdef WLT_OLD_PAK
extern INT Pack_Wavelet( UINT *Dst, FLT *C, INT Size, FLT Thresh );
extern INT Pack_Wavelet_II( UINT *Dst, FLT *C, INT Size, FLT Thresh );
#endif

#ifdef WLT_8BITS
extern void Wavelet_Flt_To_8bits( PIXEL *Dst, FLT *Src, 
   FLT *Max, FLT *Min, INT Size );
#endif

   // in wlt_dec.c

extern double WLT_Daub4[4];
extern FLT *Wavelet_Decode_Stream( WLT_STREAM *Strm );
extern FLT *Wavelet_Decoder( FLT *Where, FLT *Src, INT Size );
extern FLT *Wavelet_Decoder_2D( FLT *Buf1, FLT *Buf2, FLT *Src, INT N, INT M );
extern void Do_Inv_Daub4( FLT *Dst, FLT *Src, INT Len );
extern void Wavelet_16bits_To_Flt( FLT *Dst, USHORT *Src, 
   FLT Max, FLT Min, INT Size );
extern INT Wavelet_Decode_RLE( USHORT *Dst, USHORT *Src, 
   FLT *Max, FLT *Min );

#ifdef WLT_8BITS
extern void Wavelet_8bits_To_Flt( FLT *Dst, PIXEL *Src, 
   FLT Max, FLT Min, INT Size );
#endif 

/*******************************************************/

#endif   // _WLT_H_
