/***********************************************
 *               WAV driver                    *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "audio.h"
#include "drv_io.h"

/********************************************************************/

static BYTE Wav_Head[ 20 + 2 + 2 ] =
{
   0x52, 0x49, 0x46, 0x46,
   0xfc, 0x59, 0x04, 0x00,
   0x57, 0x41, 0x56, 0x45,
   0x66, 0x6d, 0x74, 0x20,
   0x10, 0x00, 0x00, 0x00,

   0x01, 0x00, 0xFF, 0x00    // 0xFF=Nb_Channels
};

static char Wav_Head_End[ 6 ] = { 16,0,    0x64, 0x61, 0x74, 0x61 };
static INT Total = 0;

EXTERN FILE *WAV_Out_File = NULL;
EXTERN STRING WAV_File_Name = NULL;

/********************************************************************/

static INT Flush( BYTE *Buf, INT Len )
{
   if ( WAV_Out_File==NULL ) return( 0 );
   fwrite( Buf, Len, 1, WAV_Out_File );
   Total += Len;
   return( Len );
}

static void Flush_Force( BYTE *Buf, INT Len )
{
   while( Len>0 ) { INT L = Flush( Buf, Len ); Buf += L; Len-=L; }
}

/********************************************************************/   

static INT Ping( )
{
   _WAV_Drv_.Flags = _AUDIO_FLAGS_;
   return( 0 );
}

static void Close( )
{
   INT i, j;
   char Buf[ 4 ];
   INT Nb_Channels;

   if ( WAV_Out_File==NULL ) return;

   Nb_Channels = Audio_Out_Mode&_AUDIO_STEREO_? 2 : 1;

   rewind( WAV_Out_File );
   Wav_Head[ 22 ] = (BYTE)Nb_Channels;
   fwrite( Wav_Head, 24, 1, WAV_Out_File );

      // Freq 
   j=0; for( i=0; i<32; i+=8 ) Buf[j++] = (Audio_Mix_Freq>>i) & 0xFF;
   fwrite( Buf, 1, 4, WAV_Out_File );

      // Block size
   Audio_Mix_Freq *= 2*Nb_Channels;
   j=0; for( i=0; i<32; i+=8 ) Buf[j++] = (Audio_Mix_Freq>>i) & 0xFF;
   fwrite( Buf, 1, 4, WAV_Out_File );

      // Align
   Audio_Mix_Freq = 2*Nb_Channels;
   j=0; for( i=0; i<16; i+=8 ) Buf[j++] = (Audio_Mix_Freq>>i) & 0xFF;
   fwrite( Buf, 1, 2, WAV_Out_File );

      // bits per sample + end
   Wav_Head_End[0] = Audio_Out_Mode&_AUDIO_16BITS_ ? 16 : 8;
   Wav_Head_End[1] = 0x00;
   fwrite( Wav_Head_End, 6, 1, WAV_Out_File );  

      // Data length
   j=0; for( i=0; i<32; i+=8 ) Buf[j++] = (Total>>i) & 0xFF;
   fwrite( Buf, 1, 4, WAV_Out_File );

   fseek( WAV_Out_File, 0, SEEK_END );

   fclose( WAV_Out_File );
   WAV_Out_File = NULL;
}

static INT Init( INT Mode, INT Mix )
{
   if ( WAV_File_Name==NULL ) return( -1 );
   WAV_Out_File = fopen( WAV_File_Name, WRITE_SWITCH );
   if ( WAV_Out_File==NULL ) return( -1 ); 
   Audio_Out_Mode = Mode & _WAV_Drv_.Flags;
   Audio_Mix_Freq = Mix;
   return( 0 );
}

/********************************************************************/

EXTERN AUDIO_DRIVER _WAV_Drv_ = {

   "WAV driver",
   Ping, Init, NULL, Close, Flush, Flush_Force,
   _AUDIO_FLAGS_
};

/********************************************************************/

