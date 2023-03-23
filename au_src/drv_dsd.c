/***********************************************
 *             DSound driver                   *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "audio.h"

#if defined( WIN32 )

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <dsound.h>

extern HWND hWndMain;   // <- initialized somewhere, for sure.. .:)

static LPDIRECTSOUND        lpDS = NULL;
static LPDIRECTSOUNDBUFFER  lpDSBStreamBuffer = NULL;
static INT Playing = FALSE;
static INT SBuf_Pos, SBuf_Len;

// #define LOG


#ifdef LOG
#include <stdio.h>
static FILE *Out = NULL;
#endif

/********************************************************************/

static void Close( )
{
#ifdef LOG
   if ( Out!=NULL ) fclose( Out );
#endif
   if ( lpDSBStreamBuffer!=NULL ) 
   {
      if ( Playing ) IDirectSoundBuffer_Stop( lpDSBStreamBuffer );
      Playing = FALSE;
      IDirectSoundBuffer_Release( lpDSBStreamBuffer );
   }
   lpDSBStreamBuffer = NULL;
   if ( lpDS!=NULL ) IDirectSound_Release( lpDS );
   lpDS = NULL;
   SBuf_Pos = SBuf_Len = 0;
}

static INT Ping( )
{
#ifdef LOG
   if ( Out==NULL ) Out = fopen( "log.txt", "w" );
#endif

   _DSound_Drv_.Flags = _AUDIO_FLAGS_;
   if ( lpDS!=NULL ) return( 0 );  // already there

   lpDSBStreamBuffer = NULL;   // useless...
   Playing = FALSE;

   if ( DirectSoundCreate( NULL, &lpDS, NULL ) != DS_OK ) return( -1 );
   if ( IDirectSound_SetCooperativeLevel( lpDS, hWndMain,
      // DSSCL_NORMAL
      // DSSCL_PRIORITY
      DSSCL_EXCLUSIVE 
      // DSSCL_WRITEPRIMARY
      )!= DS_OK )
   {
      Close( );
      return( -1 );
   }
   else return( 0 );
}

static INT Init( INT Mode, INT Mix )
{
   DSBUFFERDESC      dsbd;
   PCMWAVEFORMAT     wfmt;

   if ( Ping( )==-1 ) return( -1 );

   Audio_Out_Mode = Mode & _DSound_Drv_.Flags;
   Audio_Mix_Freq = Mix;
   
   bzero( &dsbd, sizeof( dsbd ));
   dsbd.dwSize = sizeof( dsbd );
   dsbd.dwFlags = DSBCAPS_CTRLDEFAULT;
      //| DSBCAPS_STICKYFOCUS;
      //| DSBCAPS_GLOBALFOCUS;
       //| DSBCAPS_CTRLPOSITIONNOTIFY
       //| DSBCAPS_GETCURRENTPOSITION2;
   
   bzero( &wfmt, sizeof( wfmt ) );
   wfmt.wf.wFormatTag = WAVE_FORMAT_PCM;
   wfmt.wf.nChannels = (Mode&_AUDIO_STEREO_) ? 2 : 1;
   wfmt.wf.nSamplesPerSec = Mix;
   wfmt.wf.nBlockAlign = wfmt.wf.nChannels*((Mode&_AUDIO_16BITS_)?2:1);
   wfmt.wf.nAvgBytesPerSec = Mix*wfmt.wf.nBlockAlign;  
   wfmt.wBitsPerSample = (Mode&_AUDIO_16BITS_) ? 16 : 8;

   SBuf_Pos = 0; SBuf_Len = 16384;

   dsbd.dwBufferBytes = SBuf_Len;
   dsbd.lpwfxFormat = (LPWAVEFORMATEX)&wfmt;

   if ( IDirectSound_CreateSoundBuffer( lpDS, &dsbd, &lpDSBStreamBuffer, NULL ) != DS_OK ) goto Failed;
   if ( IDirectSoundBuffer_SetVolume( lpDSBStreamBuffer, 0 ) != DS_OK) goto Failed;
   if ( IDirectSoundBuffer_SetPan( lpDSBStreamBuffer, 0 ) !=DS_OK ) goto Failed;
   if ( IDirectSoundBuffer_SetFrequency( lpDSBStreamBuffer, Mix ) != DS_OK ) goto Failed;

#ifdef LOG
   fprintf( Out, "Mix=%d    ", Mix );
   IDirectSoundBuffer_GetFrequency( lpDSBStreamBuffer, &Mix );
   fprintf( Out, "Freq:%d\n", Mix );
#endif
   return( 0 );
Failed:
   Close( );
   return( -1 );
}

/********************************************************************/

static INT Flush( BYTE *Buf, INT Len )
{
   LPBYTE  lpWrite1=NULL, lpWrite2=NULL;
   DWORD   dwLen1=0, dwLen2=0;
   DWORD Play_Pos, Write_Pos;

   if ( SBuf_Pos+Len>SBuf_Len ) Len = SBuf_Len-SBuf_Pos;
   if ( Playing )
   {
      IDirectSoundBuffer_GetCurrentPosition( lpDSBStreamBuffer, &Play_Pos, &Write_Pos );
      if ( SBuf_Pos<(INT)Play_Pos )
         if ( SBuf_Pos+Len>(INT)Play_Pos )
            Len = Play_Pos-SBuf_Pos;
   }
   if ( Len<=0 ) return( 0 );

   if ( IDirectSoundBuffer_Lock( lpDSBStreamBuffer, 
      SBuf_Pos, Len,
      &lpWrite1, &dwLen1, // NULL, NULL,
      &lpWrite2, &dwLen2,
      0 //SBLOCK_FROMWRITECURSOR  // 0
   ) != DS_OK ) return( 0 );

   if ( dwLen1 ) memcpy( lpWrite1, Buf, dwLen1 );
   if ( dwLen2 ) memcpy( lpWrite2, Buf+dwLen1, dwLen2 );

   if ( IDirectSoundBuffer_Unlock( lpDSBStreamBuffer, 
      lpWrite1, dwLen1, // NULL, NULL
      lpWrite2, dwLen2
   )!=DS_OK ) return( 0 );

#ifdef LOG
   fprintf( Out, "L1=0x%x Ptr1=0x%x L2=0x%x Ptr2=0x%x   ", dwLen1, lpWrite1, dwLen2, lpWrite2 );
#endif
   if ( !Playing )
   {
      if ( IDirectSoundBuffer_Play( lpDSBStreamBuffer, 0, 0,
         DSBPLAY_LOOPING )!=DS_OK ) return( 0 );
      Playing = TRUE;
   }
   Len = dwLen1 + dwLen2;

#ifdef LOG
   {  DWORD p,w;
      IDirectSoundBuffer_GetCurrentPosition( lpDSBStreamBuffer, &p, &w );
      fprintf( Out, " Play=0x%x  Write=0x%x SBuf_Pos=0x%x   ", p, w, SBuf_Pos ); 
   }
#endif

   SBuf_Pos += Len;
   if ( SBuf_Pos>=SBuf_Len ) SBuf_Pos -= SBuf_Len;

#ifdef LOG
   fprintf( Out, "SBuf_Pos=0x%x\n", SBuf_Pos ); 
#endif
   return( Len );
}

static void Flush_Force( BYTE *Buf, INT Len )
{
   do { Len -= Flush( Buf, Len ); } while( Len>0 );
}

/********************************************************************/

static void Reset( )
{
   Reset_Audio_Buffer( );
}

/********************************************************************/

EXTERN AUDIO_DRIVER _DSound_Drv_ = {
   "DirectSound driver",
   Ping, Init, Reset, Close, Flush, Flush_Force,
   _AUDIO_FLAGS_
};

/********************************************************************/

#endif      // WIN32
