/***********************************************
 *              MP3 public stuff               *
 * Skal 98                                     *
 ***********************************************/

#ifndef  _USE_PRIVATE_

#ifndef _MP3_H_
#define _MP3_H_

/******************************************************************/

// extern INT MP3_Id, MP3_SFreq, MP3_Mode, MP3_Ext;
extern void (*MP3_Handle_Spectrum)( INT Nb_Chn, FLT Sp[2][32][18] );
extern void (*MP3_Handle_Signal)( INT Len, const FLT *Left, const FLT *Right );

/******************************************************************/

typedef struct {
#pragma pack(1)

   INT Freq, Bits_Per_Sample;
   INT In_Channels, Out_Channels;
   INT Bit_Rate, FS, Mean_Freq_Size;
   INT Cur_Frame;

} MP3_STRM;          // <= must _always_ be used as pointer.
                     // don't allocate it yourself!
                     // use MP3_Open_File() instead.

/******************************************************************/

   // in mp3_dec.c

#define MP3_OUT_8BITS   0
#define MP3_OUT_16BITS  0x01
#define MP3_OUT_MONO    0
#define MP3_OUT_STEREO  0x02
#define MP3_DEBUG       0x04
#define MP3_ASK_DRIVER  0x08

extern void MP3_Init_All( );
extern MP3_STRM *MP3_Open_File( STRING Name );
extern void MP3_Destroy( MP3_STRM *Strm );

extern INT MP3_Decode_Frame( MP3_STRM *Strm, INT Frame );
extern INT MP3_Skip_Frame( MP3_STRM *Strm, INT How_Many );

   // in mpg_strm.c        (stream playing funcs)

extern FLT Init_MP3_Basic( INT Out, INT Speed );
extern MP3_STRM *MP3_Init( STRING MP3_File, INT Channels, FLT Out_Scale );
extern INT MP3_More( MP3_STRM *Strm, long Change, INT Nb_Frames );
extern INT MP3_Strm_Decode( MP3_STRM *Strm, INT Nb );
extern FLT MP3_Change_Rate;

/******************************************************************/

#ifdef MP3_USE_SAMPLES

   // in msamples.c        (old)

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
#define MP3_SLOT0_S   2       // stereo
#define MP3_SLOT1_S   3

extern void Do_Install_Smp( INT Where, INT Off, SHORT *Src, INT Smp_Len );

// extern INT MP3_Read_Header( MP3_STRM *Strm );
// extern INT MP3_Read_Frame_Layer3( MP3_STRM *Strm );

#endif   //  MP3_USE_SAMPLES

/******************************************************************/

#endif   // _MP3_H_

#endif   // _USE_PRIVATE_
