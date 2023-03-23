/***********************************************
 *              MP3 stuff                      *
 * Lib's internal header.                      *
 * Skal 98                                     *
 ***********************************************/

#ifndef _MP3_H_
#define _MP3_H_

#define MP3_VERSION 003

#define _SKIP_MPEG2_

/******************************************************************/

      // forward dcl of mp3_tab.h

extern SHORT MP3_b8_l[2][3][22];
extern SHORT MP3_b8_s[2][3][13];
extern SHORT *MP3_Tab_l, *MP3_Tab_s;
extern USHORT MP3_Scale_l[2][22];
extern USHORT MP3_Scale_s[2][13][3];
extern INT MP3_imdcts[2];
extern FLT MP3_Out[2][32][18]; // after requantization...

extern INT MP3_Id, MP3_SFreq, MP3_Mode, MP3_Ext;
extern void (*MP3_Handle_Spectrum)( INT Nb_Chn, FLT Sp[2][32][18] );
extern void (*MP3_Handle_Signal)( INT Len, const FLT *Left, const FLT *Right );
extern FLT MP3_Out_Scale;

/******************************************************************/

typedef struct {
#pragma pack(1)

   BYTE ID;
   BYTE Layer;
   BYTE Protection_Bit;
   BYTE Bitrate_Index;
   BYTE Sampling_Frequency;
   BYTE Padding_Bit;
   BYTE Private_Bit;
   BYTE Mode;
   BYTE Mode_Extension;
   BYTE Copyright;
   BYTE Original;
   BYTE Emphasis;

} MP3_HEADER;

typedef struct
{
#pragma pack(1)

   USHORT Main_Data_Begin;
   USHORT scfsi[2][4];
   USHORT Part2_3_Len[2][2];
   USHORT Big_Values[2][2];
   USHORT Global_Gain[2][2];
   USHORT Scale_Factor_Compress[2][2];
   USHORT Window_Switching_Flag[2][2];
   USHORT Block_Type[2][2];
   USHORT Mixed_Block_Flag[2][2];
   USHORT Table_Select[2][2][3];
   USHORT Sub_Block_Gain[2][2][3];
   USHORT Region0_Count[2][2];
   USHORT Region1_Count[2][2];
   BYTE   Preflag[2][2];
   USHORT Scale_Factor_Scale[2][2];
   USHORT Count1_Table_Select[2][2];

   USHORT Block[2][2];     // 0:long, 1:short, 2:mixed      (3:weird?)

}  MP3_INFO;
                                                

#define MP3_BUF_SIZE (2560)      // Theoretically: max=1152*2...

typedef struct {
#pragma pack(1)

      // public. Same as in /include/mp3.h
   INT Freq, Bits_Per_Sample;
   INT In_Channels, Out_Channels;
   INT Bit_Rate, FS, Mean_Freq_Size;
   INT Cur_Frame;

      // private
   INT Header_Size;
   MP3_HEADER Header;
   BIT_STRM  Bit_Strm;
   MP3_INFO Info;
   BYTE MP3_Buffer[MP3_BUF_SIZE];
   BYTE *MP3_Buf;

} MP3_STRM;

/******************************************************************/

#define MP3_HDR_OK     0
#define MP3_HDR_NSTD   1
#define MP3_HDR_LAYER1 2
#define MP3_HDR_F_FRMT 3

/******************************************************************/

   // in mp3_dec.c
#define MP3_OUT_8BITS   0
#define MP3_OUT_16BITS  0x01
#define MP3_OUT_MONO    0
#define MP3_OUT_STEREO  0x02
#define MP3_DEBUG       0x04
#define MP3_ASK_DRIVER  0x08

extern BYTE *MP3_Buf;
extern INT MP3_Bit_Pos;
extern UINT MP3_Buf32;

extern INT MP3_Decode_Frame( MP3_STRM *Strm, INT Frame );
extern INT MP3_Skip_Frame( MP3_STRM *Strm, INT How_Many );

extern void MP3_Init_All( );  // INT Out_Bits, FLT Scale );
extern MP3_STRM *MP3_Open_File( STRING Name );
extern void MP3_Destroy( MP3_STRM *Strm );

   // in huffman.c
extern INT MP3_Huffman_Decode( MP3_INFO *Info, INT gr, INT ch, INT End_Pos );
extern void MP3_Decode_Scale_Factors_MPEG1( MP3_INFO *Info, INT gr, INT ch );
extern void MP3_Decode_Scale_Factors_MPEG2( MP3_INFO *Info, INT ch );

extern INT HUFF_IS_Max[21];
extern INT HUFF_IS[2][578];
extern INT HUFF_Intensity_Scale;
extern INT HUFF_Non_Zero[];

   // in mp3quant.c

extern void MP3_Quant_Init( );
extern void MP3_MPEG1( MP3_INFO *Info, INT Nb_Chn );
extern void MP3_MPEG2( MP3_INFO *Info, INT Nb_Chn );

   // in mp3dct.c

extern void MP3_imdct( INT Block, INT Block_Type, INT n );
extern void MP3_Init_imdct( );
#ifndef UNIX
extern void MP3_Init_FPU( );   // in mp3dct.asm
#endif

   // in mpg_strm.c        (stream playing funcs)

extern FLT Init_MP3_Basic( INT Out, INT Speed );
extern MP3_STRM *MP3_Init( STRING MP3_File, INT Channels, FLT Out_Scale );
extern INT MP3_More( MP3_STRM *Strm, long Change, INT Nb_Frames );
extern INT MP3_Strm_Decode( MP3_STRM *Strm, INT Nb );
extern FLT MP3_Change_Rate;

//////////////////////////////////////////////////////////////////////////

   // in msamples.c

#ifdef MP3_USE_SAMPLES

typedef struct {
#pragma pack(1)

   INT Frame_Offset;
   INT Len;
   INT Final_Len;
   SHORT Nb_Frames;
   SHORT Slot;

} MP3_SAMPLE;

#define MP3_MAX_SAMPLES  16       // max samples
extern SHORT *Sample_Data[ MP3_MAX_SAMPLES ];
extern INT Sample_Len[ MP3_MAX_SAMPLES ];
extern INT Init_MP3_Cache( SHORT *MP3_List, MP3_SAMPLE *Samples, BYTE *The_MP3 );
extern INT Advance_MP3_Cache( INT List_Pos );

   // in mpg_strm.c

#define MP3_SLOT0     0
#define MP3_SLOT1     1
#define MP3_SLOT0_S   2
#define MP3_SLOT1_S   3
extern void Do_Install_Smp( INT Where, INT Off, SHORT *Src, INT Smp_Len );

// extern INT MP3_Read_Header( MP3_STRM *Strm );
// extern INT MP3_Read_Frame_Layer3( MP3_STRM *Strm );

#endif   // MP3_USE_SAMPLES

/******************************************************************/

#include "mp3debug.h"

/******************************************************************/

#endif   // _MP3_H_
