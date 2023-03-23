/*
 * MP3 streaming basic funcs
 *        Skal98
 *****************************/

#include "main.h"
#include "bit_strm.h"
#include "audio.h"
#include "./mp3.h"

EXTERN FLT MP3_Change_Rate = 0.0;
static BYTE Must_Decode;
static INT Playing = 0;

static MP3_STRM *Lock = NULL;
static INT MP3_Buf_Length = 0;
static INT MP3_Buf_Pos = 0;

/******************************************************************/

EXTERN FLT Init_MP3_Basic( INT Out, INT Speed )
{
   FLT Change_Rate;
   INT Mode, Quantum;

      // MP3_Init_All() must have be called before getting here ..

   Mode = 0; Quantum = 1;
   if ( Out&MP3_OUT_STEREO ) { Mode |= _AUDIO_STEREO_; Quantum*=2; }
   if ( Out&MP3_OUT_16BITS ) { Mode |= _AUDIO_16BITS_; Quantum*=2; }
   if ( Out&MP3_DEBUG ) { Mode |= _AUDIO_DEBUG_; }
   if ( Init_Audio( NULL, Speed, Mode, Out&MP3_ASK_DRIVER )==NULL )
      return( -1.0 );

      // twice minimal ouput byte size in MP3_Buf_Length, counted in SHORT unit
   Init_Audio_Buffer( 2*16384 );   // <= size in bytes...
   MP3_Buf_Length = 16384/Quantum;
   Change_Rate = 1000.0f * MP3_Buf_Length / Speed;

#ifndef SKL_LIGHT_CODE
#if 0 // #ifdef LNX
   Out_Message( " * MPEG3: %dHz [%s]. Buffer:%d bytes. Rate:%.2f\n", Speed, 
      Out&MP3_OUT_STEREO ? "Stereo" : "Mono", 16384, Change_Rate );
#endif
#endif   // SKL_LIGHT_CODE

   return( Change_Rate );
}

/******************************************************************
 *                  MP3 streaming functions                       *
 ******************************************************************/

extern void MP3_Basic_Signal_Handler_Stereo_16bits( INT Len, const FLT *Left, const FLT *Right );
extern void MP3_Basic_Signal_Handler_Mono_16bits( INT Len, const FLT *Left, const FLT *Right );
extern void MP3_Basic_Signal_Handler_Stereo_8bits( INT Len, const FLT *Left, const FLT *Right );
extern void MP3_Basic_Signal_Handler_Mono_8bits( INT Len, const FLT *Left, const FLT *Right );

EXTERN MP3_STRM *MP3_Init( STRING MP3_File, INT MP3_Out, FLT Out_Scale )
{
   MP3_STRM *New;

   if ( MP3_File==NULL ) return( NULL );
   MP3_Init_All( );

   New = MP3_Open_File( MP3_File );
   if ( New==NULL ) return( NULL );

   if ( MP3_Mode==3 ) MP3_Out &= ~MP3_OUT_STEREO;  // =>MP3_OUT_MONO;

   MP3_Change_Rate = Init_MP3_Basic( MP3_Out, New->FS );
   if ( MP3_Change_Rate<0.0 )
   {
      Out_Error( "Sound device initialization failed" );
      return( NULL );
   }

   MP3_Buf_Pos = 0;
   Must_Decode = TRUE;
   Playing = -1;
   
      // default audio streamer..
   if ( Audio_Out_Mode&_AUDIO_16BITS_ )
      if ( Audio_Out_Mode & _AUDIO_STEREO_ )
         MP3_Handle_Signal = MP3_Basic_Signal_Handler_Stereo_16bits;
      else MP3_Handle_Signal = MP3_Basic_Signal_Handler_Mono_16bits;
   else 
      if ( Audio_Out_Mode & _AUDIO_STEREO_ ) MP3_Handle_Signal = MP3_Basic_Signal_Handler_Stereo_8bits;
      else MP3_Handle_Signal = MP3_Basic_Signal_Handler_Mono_8bits;

   MP3_Out_Scale = Out_Scale;

   Lock = New;

   return( New );
}

/******************************************************************/

EXTERN INT MP3_Strm_Decode( MP3_STRM *Strm, INT Nb )
{
   while( Nb-- )
   {
      INT Len;
      if ( Strm!=NULL )
      {
          Len = MP3_Decode_Frame( Strm, 0 );
          if ( Len==-1 ) { 
             Strm=NULL;
      	    Len = 1152;
      	 }
      } 
      if ( Strm==NULL )
      {
         BYTE Buffer[1152];
         memset( Buffer, 0, 1152*sizeof( BYTE ) );
         Update_Audio( Buffer, 1152*sizeof( BYTE ) );
//         return( -1 );
      }
      MP3_Buf_Pos += Len;
      if ( MP3_Buf_Pos>MP3_Buf_Length )   // enough?
      {
         MP3_Buf_Pos -= MP3_Buf_Length;
         Must_Decode = FALSE;                // switch decoding off
         return( 0 );
      }
      if ( Strm==NULL ) return( -1 );
   }
   return( 1 );
}

/******************************************************************/

EXTERN INT MP3_More( MP3_STRM *Strm, long Change, INT Nb_Frames )
{
//   if ( Strm==NULL || Strm!=Lock ) return( -1 );
   Flush_Audio( );

   Change = (INT)( 1.0*Change/MP3_Change_Rate ) & 0x01;
   if ( Playing==-1 ) { Playing = Change; }

   if ( Must_Decode ) { 
      if ( MP3_Strm_Decode( Strm, Nb_Frames ) == -1 ) // decode Nb_Frames ahead...
      {
         Flush_Audio( );
//         Lock = NULL;         // <= warning, lost info...
         return( -1 );  // finished
      }
      return( 0 );
   }   
   if ( Change==Playing ) return( 1 ); // rien a faire qu'a attendre
   Playing = Change;
   Must_Decode = TRUE;  // restart decoding now...
   return( 0 );
}

/******************************************************************/
