/************************************************
 *           MP3 decoder                        *
 * heavily based on 'xamp' from Tomislav Uzelac *
 * Skal 98                                      *
 ************************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "bit_strm.h"
#include "./mp3.h"
#include "mp3_tab.h"

/*******************************************************/

EXTERN INT   MP3_Bit_Pos;
EXTERN UINT  MP3_Buf32;
EXTERN BYTE *MP3_Buf = NULL;

/*******************************************************/

EXTERN void MP3_Init_All( )
{
   MP3_Init_imdct( );
   MP3_Quant_Init( );   // pow() table
#ifndef UNIX
   MP3_Init_FPU( );
#endif
}

/*******************************************************/

static UINT MP3_Read_Header( MP3_HEADER *Header )
{
   UINT S, Ret;

   Ret = MP3_HDR_OK;
   while( 1 )
   {
      S = BSTRM_FETCH_BITS( 12 );
      if ( S==0x0FFE )
      {
         Ret = MP3_HDR_NSTD;
         goto Parse;
      }
      else if ( S==0x0FFF ) goto Parse;
Redo:
        BSTRM_Bit_Pos = (BSTRM_Bit_Pos & ~0x07) + 8;
        BSTRM_Ptr++;
        BSTRM_Buf32 <<= 8; BSTRM_Buf32 |= BSTRM_Ptr[3];
   }

Parse:
   S = BSTRM_GET_BITS( 12 );

      // global variables for all subsequent processing of this chunk

   S = BSTRM_GET_BITS( 20 );
   Header->Emphasis = (BYTE)(S&0x03); S>>=2;
   Header->Original = (BYTE)(S&0x01); S>>=1;
   Header->Copyright = (BYTE)(S&0x01); S>>=1;
   Header->Mode_Extension = (BYTE)(S&0x03); S>>=2;
   MP3_Mode = Header->Mode =  (BYTE)(S&0x03); S>>=2;
   Header->Private_Bit = (BYTE)(S&0x01); S>>=1;
   Header->Padding_Bit = (BYTE)(S&0x01); S>>=1;
   MP3_SFreq = Header->Sampling_Frequency = (BYTE)(S&0x03); S>>=2;
   Header->Bitrate_Index = (BYTE)(S&0x0F); S>>=4;
   Header->Protection_Bit = (BYTE)(S&0x01); S>>=1;
   Header->Layer = (BYTE)(S&0x03); S>>=2;
   MP3_Id = Header->ID = (BYTE)S;

#ifdef _SKIP_MPEG2_
   if ( MP3_Id!=1 ) Exit_Upon_Error( "MPEG2.0 unsupported" );
#endif

   if ( !MP3_Mode ) Header->Mode_Extension = 0;
   MP3_Ext = Header->Mode_Extension;

   MP3_Tab_l = MP3_b8_l[ MP3_Id ][ MP3_SFreq ];
   MP3_Tab_s = MP3_b8_s[ MP3_Id ][ MP3_SFreq ];

//   MP3_Print_Header( &Strm->Header );
   if ( !MP3_Header_Sane( &Strm->Header ) )
   {
//      fprintf( stderr, "Header not sane!!\n" );
      goto Redo;
   }

   if ( Header->Layer==3 ) Ret = MP3_HDR_LAYER1;
   else if ( Header->Bitrate_Index==0 ) Ret = MP3_HDR_F_FRMT;
   else if ( Header->Protection_Bit==0 )  // get_CRC
   {
      USHORT Crc;
      //   BSTRM_Sync( );
      Crc = BSTRM_GET_BITS( 16 );
//      fprintf( stderr, "Crc=%d\n", Crc );
   }
//   MP3_Report_Hdr_Err( Ret );
   return( Ret );
}

/*******************************************************
 * Possible cases:                                     *
 *   Switch | Block_Type | Mix_Flag |=> Block          *
 *     0    |    ???     |    ???   |=> 0 (long)       *
 *     1    |     2      |     0    |=> 1 (short)      *
 *     1    |     2      |     1    |=> 2 (mixed)      *
 *     1    |    0-1     |    ???   |=> 0 (weird?)     *
 *******************************************************/

static void Set_Block_Type( MP3_INFO *Info, INT gr, INT n )
{
   if ( Info->Window_Switching_Flag[gr][n] )
   {
      if ( Info->Block_Type[gr][n]==2 )
      {
         if ( Info->Mixed_Block_Flag[gr][n] )
            Info->Block[gr][n] = 3;    // mixed
         else Info->Block[gr][n] = 2;  // short
      }
      else Info->Block[gr][n] = 1;   // weird...
   }
   else Info->Block[gr][n] = 0;   // long;
}

/*******************************************************/

static void MP3_Get_Info( MP3_INFO *Info, INT Nb_Chn )
{
   INT n, gr;

   if ( MP3_Id )
      for( n=0; n<Nb_Chn; ++n )
      {
         Info->scfsi[n][0] = (USHORT)BSTRM_ONE_BIT( );
         Info->scfsi[n][1] = (USHORT)BSTRM_ONE_BIT( );
         Info->scfsi[n][2] = (USHORT)BSTRM_ONE_BIT( );
         Info->scfsi[n][3] = (USHORT)BSTRM_ONE_BIT( );
      }
   for( gr=0; gr<( MP3_Id ? 2 : 1 ); ++gr )
      for( n=0; n<Nb_Chn; ++n )
      {
         Info->Part2_3_Len[gr][n] = (USHORT)BSTRM_GET_BITS( 12 );
         Info->Big_Values[gr][n] = (USHORT)BSTRM_GET_BITS( 9 );
         Info->Global_Gain[gr][n] = (USHORT)BSTRM_GET_BITS( 8 );
         if ( MP3_Id )
            Info->Scale_Factor_Compress[gr][n] = (USHORT)BSTRM_GET_BITS( 4 );
         else Info->Scale_Factor_Compress[gr][n] = (USHORT)BSTRM_GET_BITS( 9 );
         Info->Window_Switching_Flag[gr][n] = (USHORT)BSTRM_ONE_BIT( );

         if ( Info->Window_Switching_Flag[gr][n] )
         {
            Info->Block_Type[gr][n] = (USHORT)BSTRM_GET_BITS( 2 );
            Info->Mixed_Block_Flag[gr][n] = (USHORT)BSTRM_ONE_BIT( );
               // 2 regions
            Info->Table_Select[gr][n][0] = (USHORT)BSTRM_GET_BITS( 5 );
            Info->Table_Select[gr][n][1] = (USHORT)BSTRM_GET_BITS( 5 );
            Info->Table_Select[gr][n][2] = 0;

            Info->Sub_Block_Gain[gr][n][0] = (USHORT)BSTRM_GET_BITS( 3 );
            Info->Sub_Block_Gain[gr][n][1] = (USHORT)BSTRM_GET_BITS( 3 );
            Info->Sub_Block_Gain[gr][n][2] = (USHORT)BSTRM_GET_BITS( 3 );
         }
         else
         {
               // 3 regions
            Info->Table_Select[gr][n][0] = (USHORT)BSTRM_GET_BITS( 5 );
            Info->Table_Select[gr][n][1] = (USHORT)BSTRM_GET_BITS( 5 );
            Info->Table_Select[gr][n][2] = (USHORT)BSTRM_GET_BITS( 5 );

            Info->Region0_Count[gr][n] = (USHORT)BSTRM_GET_BITS( 4 );
            Info->Region1_Count[gr][n] = (USHORT)BSTRM_GET_BITS( 3 );
            Info->Block_Type[gr][n] = 0;
         }
         if ( MP3_Id ) Info->Preflag[gr][n] = BSTRM_ONE_BIT( );
         Info->Scale_Factor_Scale[gr][n] = BSTRM_ONE_BIT( );
         Info->Count1_Table_Select[gr][n] = BSTRM_ONE_BIT( );
         Set_Block_Type( Info, gr, n );
      }
}

/*******************************************************/

      // Standard conventions

static INT MP3_Sampling_Frequency[2][3] = 
{
   { 22050 , 24000 , 16000 }, { 44100 , 48000 , 32000 }
};

static SHORT MP3_Bitrate[2][3][15] = 
{
   {  { 0,32,48,56,64,80,96,112,128,144,160,176,192,224,256 },
      { 0,8,16,24,32,40,48,56,64,80,96,112,128,144,160 },
      { 0,8,16,24,32,40,48,56,64,80,96,112,128,144,160 } },
   {  { 0,32,64,96,128,160,192,224,256,288,320,352,384,416,448 },
      { 0,32,48,56,64,80,96,112,128,160,192,224,256,320,384 },
      { 0,32,40,48,56,64,80,96,112,128,160,192,224,256,320 } }
};

/*******************************************************/

static void MP3_Parse_Info( MP3_STRM *Strm )
{
   INT Bits_Ok, Bits_Reject;

//   BSTRM_Sync( );
   if ( MP3_Mode == 3 )       // mono
   {
      Strm->In_Channels = 1;
      if ( MP3_Id ) {
         Bits_Ok = 9; Bits_Reject = 5;
         Strm->Header_Size = 21;
      }
      else { 
         Bits_Ok = 8; Bits_Reject = 1;
         Strm->Header_Size = 13;
      }
   }
   else  // stereo
   {
      Strm->In_Channels =  2;
      if ( MP3_Id ) {
         Bits_Ok = 9; Bits_Reject = 3; 
         Strm->Header_Size = 36;
      }
      else {
         Bits_Ok = 8; Bits_Reject = 2;
         Strm->Header_Size = 21;
      }
   }
   if ( Strm->Header.Protection_Bit==0 ) 
      Strm->Header_Size += 2;  // crc 

   Strm->Info.Main_Data_Begin = (USHORT)BSTRM_GET_BITS( Bits_Ok );
   BSTRM_GET_BITS( Bits_Reject );

   MP3_Get_Info( &Strm->Info, Strm->In_Channels );

   Strm->Bit_Rate = MP3_Bitrate[ MP3_Id ][ 3-Strm->Header.Layer ][ Strm->Header.Bitrate_Index ];
   Strm->FS = MP3_Sampling_Frequency[ MP3_Id ][ MP3_SFreq ];
   Strm->Mean_Freq_Size = 144000 * Strm->Bit_Rate / Strm->FS;
   if ( !MP3_Id ) Strm->Mean_Freq_Size /= 2;
}

/*******************************************************/

static INT MP3_Read_Frame_Layer3( MP3_STRM *Strm )
{
   INT MP3_Data_Len;
   INT Len;

         // Load every data in MP3_Buffer before decoding...
         // MP3_Buf points to old data, here

   MP3_Data_Len = Strm->Mean_Freq_Size+Strm->Header.Padding_Bit;
   MP3_Data_Len -= Strm->Header_Size;
   BSTRM_GET_BYTES( MP3_Buf, MP3_Data_Len );

         // update MP3_Buf with old bits

   MP3_Buf -= Strm->Info.Main_Data_Begin;
   MP3_Data_Len += Strm->Info.Main_Data_Begin;
   MP3_Bit_Pos = 0;
   MP3_Buf32 = (MP3_Buf[0]<<24)|(MP3_Buf[1]<<16)|(MP3_Buf[2]<<8)|MP3_Buf[3];

   if ( MP3_Id==1 )
   {
      MP3_MPEG1( &Strm->Info, Strm->In_Channels );
      Len = 2*18*32;
   }
#ifndef _SKIP_MPEG2_
   else           // MPEG2.0
   {
      MP3_MPEG2( &Strm->Info, Strm->In_Channels );
      Len = 18*32;
   }
#endif

      // recopy unused data at beginning of MP3_Buffer for next frame

   MP3_Data_Len -= MP3_Bit_Pos/8;
   if ( MP3_Data_Len )
      memcpy( Strm->MP3_Buffer, MP3_Buf, MP3_Data_Len );
   MP3_Buf = Strm->MP3_Buffer + MP3_Data_Len;   // <= set position just after old data...

   return( Len ); // number of SHORT decoded per channel
}

#if 0
static INT MP3_Read_Frame_Layer2( MP3_STRM *Strm )
{
   return( -1 );
}
#endif

/*******************************************************/

EXTERN INT MP3_Skip_Frame( MP3_STRM *Strm, INT How_Many )
{
   INT i;

   if ( Strm==NULL || How_Many<=0 ) return( 0 );
   for( i=0; i<How_Many; ++i )
   {
      INT MP3_Data_Len;

      if ( (ADDR)Strm->Bit_Strm.Raw_Ptr >=
            (ADDR)Strm->Bit_Strm.Raw + Strm->Bit_Strm.Available_Bytes 
      )
         break;   // EOF

      MP3_Buf = Strm->MP3_Buf;   // select input buffer state
      BSTRM_Select( &Strm->Bit_Strm );
      MP3_Read_Header( &Strm->Header );
      MP3_Parse_Info( Strm );
      MP3_Data_Len = Strm->Mean_Freq_Size+Strm->Header.Padding_Bit;
      MP3_Data_Len -= Strm->Header_Size;
      BSTRM_GET_BYTES( MP3_Buf, MP3_Data_Len );
      MP3_Buf -= Strm->Info.Main_Data_Begin;
      MP3_Data_Len += Strm->Info.Main_Data_Begin;
      if ( MP3_Data_Len )
         memcpy( Strm->MP3_Buffer, MP3_Buf, MP3_Data_Len );
      MP3_Buf = Strm->MP3_Buffer + MP3_Data_Len;
      Strm->MP3_Buf = MP3_Buf;
      BSTRM_UN_SELECT( &Strm->Bit_Strm );
   }
   Strm->Cur_Frame += i;
   return( i );
}

EXTERN INT MP3_Decode_Frame( MP3_STRM *Strm, INT Frame )
{
   INT Len;

   if ( (ADDR)Strm->Bit_Strm.Raw_Ptr>=(ADDR)Strm->Bit_Strm.Raw + Strm->Bit_Strm.Available_Bytes )
      return( -1 );   // EOF

   MP3_Buf = Strm->MP3_Buf;   // select input buffer state

   BSTRM_Select( &Strm->Bit_Strm ); // ...and stream
   MP3_Read_Header( &Strm->Header );
   MP3_Parse_Info( Strm );
   Strm->Cur_Frame++;

   if ( Strm->Header.Layer==1 ) Len = MP3_Read_Frame_Layer3( Strm );
   else Exit_Upon_Error( "layer2 unsupported" );

#if 0       // don't decode non-MPEG1.0layer3, for now...
   Len = 0;
   if ( Strm->Header.Layer==2 ) Len = MP3_Read_Frame_Layer2( Strm );
   if ( Len==-1 ) Exit_Upon_Error( "Can't decode frame" );
#endif

   Strm->MP3_Buf = MP3_Buf;   // update bit stream position
   BSTRM_UN_SELECT( &Strm->Bit_Strm );
   return( Len );
}

static void MP3_Decode_First_Frame( MP3_STRM *Strm )
{
   BSTRM_Select( &Strm->Bit_Strm );
   MP3_Read_Header( &Strm->Header );
   MP3_Parse_Info( Strm );

         // rewind to start

   Strm->Cur_Frame = 0;
   BSTRM_Ptr = Strm->Bit_Strm.Raw;
   BSTRM_Bit_Pos = 0;
   BSTRM_Buf32 = BSTRM_Ptr[0]<<24;
   if ( BSTRM_Cur->Raw_Len>0 ) BSTRM_Buf32 |= BSTRM_Ptr[1]<<16;
   if ( BSTRM_Cur->Raw_Len>1 ) BSTRM_Buf32 |= BSTRM_Ptr[2]<<8;
   if ( BSTRM_Cur->Raw_Len>2 ) BSTRM_Buf32 |= BSTRM_Ptr[3];
   BSTRM_UN_SELECT( &Strm->Bit_Strm );
}

/*******************************************************/
/*******************************************************/

EXTERN MP3_STRM *MP3_Open_File( STRING Name )
{
   MP3_STRM *New;

   New = New_Fatal_Object( 1, MP3_STRM );
   if ( New==NULL ) return( NULL );

   if ( BSTRM_Init( &New->Bit_Strm, Name )==NULL )
      goto Failed;

   New->Cur_Frame = 0;
   New->In_Channels = 0;
   New->Out_Channels = 1;
   New->MP3_Buf = New->MP3_Buffer;

   BSTRM_DO_INIT(&New->Bit_Strm);

   BSTRM_Select( &New->Bit_Strm );
   MP3_Decode_First_Frame( New );   // first frame. Will set parameters up

   return( New );

Failed:
   
   M_Free( New );
//   Exit_Upon_Error( "Can't open mp3 file '%s'", Name );
   return( NULL );
}

EXTERN void MP3_Destroy( MP3_STRM *Strm )
{
   if ( Strm==NULL ) return;
   BSTRM_Destroy( &Strm->Bit_Strm );
   M_Free( Strm );
}

/*******************************************************/
/*******************************************************/

