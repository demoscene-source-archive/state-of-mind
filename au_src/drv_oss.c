/***********************************************
 *                OSS audio                    *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"

#if !defined(NO_SOUND)

#if defined( LNX ) ||  defined( UNIX )

#include "audio.h"

// #define SUNOS_AUDIO           // fast hack
// #define IRIX_AUDIO
/********************************************************************/

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#if defined(SUNOS_AUDIO)

#include <sys/stropts.h>
#include <sys/audioio.h>

#define DSP_NAME  "/dev/audio"

#elif defined(IRIX_AUDIO)

#include "./irix_audio.h"

static ALport Port=0;
static ALconfig Config=0;
static int SQuantum = 0;

#else   // OSS

#include <sys/stat.h>
#include <sys/wait.h>

#ifdef __FreeBSD__
#include <machine/soundcard.h>
#else
//#include <sys/soundcard.h>
#include <linux/soundcard.h>
#endif   // __FreeBSD__
   
#define DEFAULT_FRAGSIZE 17   // 17
#define DEFAULT_NUMFRAGS 4

#define DSP_NAME  "/dev/dsp"

#endif  // !SUNOS_AUDIO && ! IRIX_AUDIO

static int Sound_fd = -1;

/********************************************************************/

static INT Flush( BYTE *Buf, INT Len )
{
#if defined (SUNOS_AUDIO)

   return( (INT)write( Sound_fd, Buf, Len ) );

#elif defined( IRIX_AUDIO)

  if ( Port==0 || Sound_fd==-1 ) return 0;
#if 0
  return( (INT)write( Sound_fd, Buf, Len ) );
#else
  ALwritesamps( Port, Buf, Len/SQuantum );
  return Len;
#endif

#else // OSS

   audio_buf_info Info;

   INT Ok_To_Write;
   fd_set Wrt;
   struct timeval Timeout;

   Timeout.tv_sec = 1;
   Timeout.tv_usec = 0;
   FD_ZERO( &Wrt );
   FD_SET( Sound_fd, &Wrt );
   select( Sound_fd+1, NULL, &Wrt, NULL, &Timeout );
   ioctl( Sound_fd, SNDCTL_DSP_GETOSPACE, &Info );
   Ok_To_Write = Info.fragments*Info.fragsize;
   if ( Len>Ok_To_Write ) Len = Ok_To_Write;
   if ( Len ) Len = (INT)write( Sound_fd, Buf, Len );
   return( Len );

#endif  // OSS
}

static void Flush_Force( BYTE *Buf, INT Len )
{
   INT L;
   while( Len )
   {
#if defined(SUNOS_AUDIO)

      L = (INT)write( Sound_fd, Buf, L );
      Len -= L; Buf += L;

#elif defined(IRIX_AUDIO)

#if 0
      L = (INT)write( Sound_fd, Buf, L );
      Len -= L; Buf += L;
#else
    if ( ALwritesamps( Port, Buf, Len/SQuantum )>=0) { Len -= L; Buf += L; }
#endif

#else // OSS
      audio_buf_info Info;
      INT Ok_To_Write;

      ioctl( Sound_fd, SNDCTL_DSP_GETOSPACE, &Info );
      Ok_To_Write = Info.fragments*Info.fragsize;
      if ( Len>Ok_To_Write ) L = Ok_To_Write;
      else L = Len;
#endif // OSS
   }
}

/********************************************************************/   

static INT Ping( )
{
   _OSS_Drv_.Flags = _AUDIO_FLAGS_;
#if defined(IRIX_AUDIO)
   Config = ALnewconfig();
   if (Config==0) return -1;
   SQuantum = 0;
   return 0;
#else
   return( access( DSP_NAME, W_OK ) );  // 0=Ok. -1=fail
#endif
}

static void Close( )
{
#if defined(IRIX_AUDIO)

   if (Port!=0) ALcloseport(Port);
   Port=0;
   if (Config!=0) ALfreeconfig(Config);
   Config=0;
   SQuantum = 0;
#else
   if ( Sound_fd!=-1 )
   {
#ifndef SUNOS_AUDIO
      ioctl( Sound_fd, SNDCTL_DSP_RESET, 0 );
#endif
      close(Sound_fd);
   }
#endif
   Sound_fd = -1;
}

#if defined(SUNOS_AUDIO)

static INT Init( INT Mode, INT Mix )
{
   int Stereo;
   audio_info_t Audio_Info;

   Sound_fd = open( DSP_NAME, O_WRONLY, 0 );
   if ( Sound_fd==-1 ) return( -1 );

   if ( ioctl( Sound_fd, AUDIO_GETINFO, &Audio_Info )<0 )
      goto Failed;

   Audio_Out_Mode = Mode & _OSS_Drv_.Flags;
   Audio_Mix_Freq = Mix;

   Audio_Info.play.precision = (Mode&_AUDIO_16BITS_) ? 16 : 8;
   Audio_Info.play.encoding = AUDIO_ENCODING_LINEAR;

   Stereo =  (Mode&_AUDIO_STEREO_) ? 1 : 0;
   Audio_Info.play.channels = (Mode&_AUDIO_STEREO_) ? 2 : 1;
   Audio_Info.play.sample_rate = Mix;
   Audio_Info.play.gain = 255;

   if ( ioctl( Sound_fd, AUDIO_SETINFO, &Audio_Info )<0 )
      goto Failed;

   return( 0 );

Failed:
   Close( );
   return( -1 );
}

#elif defined( IRIX_AUDIO )

static INT Init( INT Mode, INT Mix )
{
   int Rate;
   long pvbuffer[2];
   if (Config==0) return -1;

   switch(Mix) {
     case 44100: Rate = AL_RATE_44100; break;
     case 32000: Rate = AL_RATE_32000; break;
     case 22050: Rate = AL_RATE_22050; break;
     default: return -1; break;
   }
   Audio_Out_Mode = Mode & _OSS_Drv_.Flags;
   Audio_Mix_Freq = Mix;

   if ( ALsetchannels( Config, (Mode&_AUDIO_STEREO_) ? AL_STEREO : AL_MONO )<0 )
     goto Failed;
   if ( ALsetwidth( Config, (Mode&_AUDIO_16BITS_) ? AL_SAMPLE_16 : AL_SAMPLE_8 )<0 )
     goto Failed;
   if ( ALsetsampfmt( Config, AL_SAMPFMT_TWOSCOMP )<0 )
     goto Failed;
   if ( ALsetqueuesize( Config, 131069)<0 )
     goto Failed;

   Port = ALopenport( "mind", "w", Config );
   if ( Port==0 ) goto Failed;

   pvbuffer[0] = AL_OUTPUT_RATE;
   pvbuffer[1] = Rate;
   if ( ALsetparams( AL_DEFAULT_DEVICE, pvbuffer, 2 )<0 )
     goto Failed;
#if 0
   pvbuffer[0] = AL_LEFT_SPEAKER_GAIN;
   pvbuffer[1] = 100;
   if ( ALsetparams( AL_DEFAULT_DEVICE, pvbuffer, 2 )<0 )
     goto Failed;
   pvbuffer[0] = AL_RIGHT_SPEAKER_GAIN;
   pvbuffer[1] = 100;
   if ( ALsetparams( AL_DEFAULT_DEVICE, pvbuffer, 2 )<0 )
     goto Failed;
#endif

   Sound_fd = ALgetfd(Port);
   if ( Sound_fd<0 ) goto Failed;
   SQuantum = 1;
//   if ( Audio_Out_Mode&_AUDIO_STEREO_) SQuantum*=2;
   if ( Audio_Out_Mode&_AUDIO_16BITS_) SQuantum*=2;
   return( 0 );

Failed:
   Close( );
   return( -1 );
}

#else // OSS

static INT Init( INT Mode, INT Mix )
{
//   char *Err = NULL;
   int Nb_Frag, Frag_Len, Frag_Size;
   int Smp_Size, Stereo;

   Sound_fd = open( DSP_NAME, O_WRONLY, 0 );
   if ( Sound_fd==-1 ) return( -1 );

   Smp_Size = AFMT_S16_LE;
   if ( ioctl(Sound_fd, SNDCTL_DSP_SETFMT, &Smp_Size ) == -1 )
      goto Failed;

//   ioctl( Sound_fd, SNDCTL_DSP_RESET, 0 );

#ifndef __FreeBSD__
   if ( Mode & _AUDIO_DEBUG_ )
   {
      Nb_Frag = DEFAULT_NUMFRAGS;
      Frag_Len = DEFAULT_FRAGSIZE;
      Frag_Size = ( Nb_Frag<<16 ) | Frag_Len;

      if( ioctl( Sound_fd, SNDCTL_DSP_SETFRAGMENT, &Frag_Size )<0 )
      {
//         Err = "SNDCTL_DSP_SETFRAGMENT";
         goto Failed;
      }
   }
#endif   // __FreeBSD__

   Audio_Out_Mode = Mode & _OSS_Drv_.Flags;
   Audio_Mix_Freq = Mix;

   Smp_Size = (Mode&_AUDIO_16BITS_) ? 16 : 8;
   if ( ioctl( Sound_fd, SNDCTL_DSP_SAMPLESIZE, &Smp_Size ) == -1 )
   {
      if ( Smp_Size==8 ) goto Failed;

      Smp_Size = 8;  // try 8 bits output...
      if ( ioctl( Sound_fd, SNDCTL_DSP_SAMPLESIZE, &Smp_Size ) == -1 )
      {
//         Err = "SNDCTL_DSP_SAMPLESIZE";
         goto Failed;
      }
      Audio_Out_Mode &= ~_AUDIO_16BITS_;
   }

   Stereo =  (Mode&_AUDIO_STEREO_) ? 1 : 0;
   if ( ioctl( Sound_fd, SNDCTL_DSP_STEREO, &Stereo ) == -1 )
   {
      if ( Stereo ) goto Failed;
      Stereo = 0; // try mono output...
      if ( ioctl( Sound_fd, SNDCTL_DSP_STEREO, &Stereo ) == -1 )
      {
//         Err = "SNDCTL_DSP_STEREO";
         goto Failed;
      }
      Audio_Out_Mode &= ~_AUDIO_STEREO_;
   }

   if ( ioctl( Sound_fd, SNDCTL_DSP_SPEED, &Audio_Mix_Freq ) == -1 )
   {
//      Err = "SNDCTL_DSP_SPEED";
      goto Failed;
   }

   Smp_Size = 0;
   if ( ioctl( Sound_fd, SNDCTL_DSP_GETBLKSIZE, &Smp_Size ) == -1 )
   {
//      Err = "SNDCTL_DSP_GETBLKSIZE";
      goto Failed;
   }

   return( 0 );

Failed:
   Close( );
   return( -1 );
}

#endif   // OSS

static void Reset( )
{
#if defined( SUNOS_AUDIO )
#elif defined( IRIX_AUDIO )
#else
   if ( Sound_fd!=-1 ) ioctl( Sound_fd, SNDCTL_DSP_RESET, 0 );
#endif
   Reset_Audio_Buffer( );
}

/********************************************************************/

EXTERN AUDIO_DRIVER _OSS_Drv_ = {

#if defined( SUNOS_AUDIO )
   "SunOS driver",
#elif defined( IRIX_AUDIO )
   "IRIX driver",
#else
   "OSS driver",
#endif
   Ping, Init, Reset, Close, Flush, Flush_Force,
   _AUDIO_FLAGS_
};

/********************************************************************/

#endif   // LNX - UNIX

#endif  // NO_SOUND
