/************************************************
 *       MP3 output signal handling             *
 *                                              *
 * converts unclamped floats input into WAV     *
 * integer format.                              *
 *                                              *
 * Skal 98                                      *
 ************************************************/

#include "main.h"
#include "audio.h"

extern FLT MP3_Out_Scale;

/*******************************************************/

#define SCALE_8BITS  127.0

//#define MAX_16 32767
#define MAX_16 32000
#define SCALE_16BITS ((FLT)MAX_16)

EXTERN void MP3_Basic_Signal_Handler_Stereo_16bits( INT Len, const FLT *Left, const FLT *Right )
{
   INT i;
   USHORT Buffer[2*32*18];
   for( i=0; i<Len; i++ )
      if ( Right!=NULL )
      {
         SHORT Out;
         FLT x;
         x = Left[i]*MP3_Out_Scale;
         if ( x>1.0 ) Out = MAX_16;
         else if ( x<-1.0 ) Out = -MAX_16;
         else Out = (SHORT)( x*SCALE_16BITS );
         Buffer[2*i] = Out;
         x = Right[i]*MP3_Out_Scale;
         if ( x>1.0 ) Out = MAX_16;
         else if ( x<-1.0 ) Out = -MAX_16;
         else Out = (SHORT)( x*SCALE_16BITS );
         Buffer[2*i+1] = Out;
      }
      else for( i=0; i<Len; i++ )
      {
         SHORT Out;
         FLT x;
         x = Left[i]*MP3_Out_Scale;
         if ( x>1.0 ) Out = MAX_16;
         else if ( x<-1.0 ) Out = -MAX_16;
         else Out = (SHORT)( x*SCALE_16BITS );
         Buffer[2*i] = Buffer[2*i+1] = Out;
      }
   Update_Audio( (void*)Buffer, 2*Len*sizeof( USHORT ) );
}

EXTERN void MP3_Basic_Signal_Handler_Mono_16bits( INT Len, const FLT *Left, const FLT *Right )
{
   INT i;
   USHORT Buffer[32*18];
   if ( Right!=NULL ) for( i=0; i<Len; i++ )
   {
      SHORT L, R;
      FLT x;
      x = Left[i]*MP3_Out_Scale;
      if ( x>1.0 ) L = MAX_16;
      else if ( x<-1.0 ) L = -MAX_16;
      else L = (SHORT)( x*SCALE_16BITS );
      x = Right[i]*MP3_Out_Scale;
      if ( x>1.0 ) R = MAX_16;
      else if ( x<-1.0 ) R = -MAX_16;
      else R = (SHORT)( x*SCALE_16BITS );
      Buffer[i] = (L+R)/2;
   }
   else for( i=0; i<Len; i++ )
   {
      SHORT Out;
      FLT x;
      x = Left[i]*MP3_Out_Scale;
      if ( x>1.0 ) Out = MAX_16;
      else if ( x<-1.0 ) Out = -MAX_16;
      else Out = (SHORT)( x*SCALE_16BITS );
      Buffer[i] = Out;
   }
   Update_Audio( (void*)Buffer, Len*sizeof( USHORT ) );
}

EXTERN void MP3_Basic_Signal_Handler_Stereo_8bits( INT Len, const FLT *Left, const FLT *Right )
{
   INT i;
   BYTE Buffer[2*32*18];
   if ( Right!=NULL ) for( i=0; i<Len; i++ )
   {
      BYTE Out;
      FLT x;
      x = Left[i]*MP3_Out_Scale;
      if ( x>1.0 ) x = 1.0;
      else if ( x<-1.0 ) x = -1.0;
      Out = (BYTE)( x*SCALE_8BITS+128 );
      Buffer[2*i] = Out;
      x = Right[i]*MP3_Out_Scale;
      if ( x>1.0 ) x = 1.0;
      else if ( x<-1.0 ) x = -1.0;
      Out = (BYTE)( x*SCALE_8BITS+128.0 );
      Buffer[2*i+1] = Out;
   }
   else for( i=0; i<Len; i++ )
   {
      BYTE Out;
      FLT x;
      x = Left[i]*MP3_Out_Scale;
      if ( x>1.0 ) x = 1.0;
      else if ( x<-1.0 ) x = -1.0;
      Out = (BYTE)( x*SCALE_8BITS+128.0 );
      Buffer[2*i] = Buffer[2*i+1] = Out;
   }
   Update_Audio( (void*)Buffer, 2*Len*sizeof( BYTE ) );
}

EXTERN void MP3_Basic_Signal_Handler_Mono_8bits( INT Len, const FLT *Left, const FLT *Right )
{
   INT i;
   BYTE Buffer[32*18];
   if ( Right!=NULL ) for( i=0; i<Len; i++ )
   {
      USHORT L, R;
      FLT x;
      x = Left[i]*MP3_Out_Scale;
      if ( x>1.0 ) x = 1.0;
      else if ( x<-1.0 ) x = -1.0;
      L = (USHORT)( x*SCALE_8BITS+128.0 );
      x = Right[i]*MP3_Out_Scale;
      if ( x>1.0 ) x = 1.0;
      else if ( x<-1.0 ) x = -1.0;
      R = (USHORT)( x*SCALE_8BITS+128.0 );
      Buffer[i] = (BYTE)( (L+R)/2 );
   }
   else for( i=0; i<Len; i++ )
   {
      BYTE Out;
      FLT x;
      x = Left[i]*MP3_Out_Scale;
      if ( x>1.0 ) x = 1.0;
      else if ( x<-1.0 ) x = -1.0;
      Out = (BYTE)( x*SCALE_8BITS+128.0 );
      Buffer[i] = Out;
   }
   Update_Audio( (void*)Buffer, Len*sizeof( BYTE ) );
}

/*******************************************************/
