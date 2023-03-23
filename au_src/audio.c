/***********************************************
 *             audio streaming                 *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "audio.h"
#include "music.h"

static AUDIO_DRIVER *Audio_Drv = NULL;
static AUDIO_DRIVER *Registered_Drv[MAX_AUDIO_DRV] = { NULL };
static INT Last_Driver = 0;

EXTERN USHORT Audio_Out_Mode = 0, Audio_In_Mode = 0;
EXTERN INT Audio_Mix_Freq = 0; 
EXTERN INT ABuf_Len = 0;
static INT ABuf_Head = 0, ABuf_Tail = 0;
EXTERN BYTE *Audio_Buf = NULL;

/******************************************************************/

EXTERN AUDIO_DRIVER *Init_Audio( AUDIO_DRIVER *Drv, 
   INT Mix, INT Mode, INT Ask )
{
   if ( Drv!=NULL )
   {
      if ( (*Drv->Ping)( ) ) return( NULL );
   }
   else 
   {
      INT i, j, Answer;
      for( i=0; i<MAX_AUDIO_DRV; ++i )
      {
         if ( Registered_Drv[i]==NULL ) continue;
         if ( (*Registered_Drv[i]->Ping)( ) ) Registered_Drv[i] = NULL;
         else Drv = Registered_Drv[i]; 
      }
      if ( Drv==NULL ) return( NULL );
#ifndef SKL_LIGHT_CODE
      if ( Ask )
      {
         i=0; j=1; 
         Out_Message( "Use which driver?" );
         for( i=0; i<MAX_AUDIO_DRV; ++i )
         {
            if ( Registered_Drv[i]==NULL ) continue;
            Out_Message( "%d. %s.", j++, Registered_Drv[i]->Name );
         }
Try:
         Answer = Get_Me_A_Number( stdin );
         if ( ( Answer<=0 ) || ( Answer>=j ) ) goto Try;
         j = 1;
         for( i=0; i<MAX_AUDIO_DRV; ++i )
         {
            if ( Registered_Drv[i]==NULL ) continue;
            if ( j==Answer ) { Drv = Registered_Drv[i]; break; }
            j++;
         }
      }
#endif
   }
   Audio_In_Mode = (USHORT)Mode;
   Audio_Mix_Freq = Mix;

   if ( (*Drv->Init)( Mode, Mix ) ) return( NULL );

#ifndef SKL_LIGHT_CODE
#if 0    // #ifdef LNX
   Out_Message( " * '%s' initialized:  [%d], Out=%d bits, [%s] %s",
      Drv->Name,
      Audio_Mix_Freq,
      Audio_Out_Mode & _AUDIO_16BITS_ ? 16 : 8,
      Audio_Out_Mode & _AUDIO_STEREO_ ? "Stereo" : "Mono",
      Audio_Out_Mode & _AUDIO_DEBUG_ ? "[debug mode]" : "" );
#endif
#endif   // SKL_LIGHT_CODE

   Audio_Drv = Drv;

   return( Drv );
}

/******************************************************************/

EXTERN void Flush_Audio_Force( ) 
{
   if ( Audio_Drv==NULL ) return;
   if ( ABuf_Tail==ABuf_Head ) return;
   if ( ABuf_Head<ABuf_Tail )
   {
      INT Len;
      Len = ABuf_Len - ABuf_Tail;
      (*Audio_Drv->Flush_Force)( Audio_Buf + ABuf_Tail, Len );
      ABuf_Tail += Len;
      if ( ABuf_Tail>=ABuf_Len ) ABuf_Tail -= ABuf_Len;
   }
   (*Audio_Drv->Flush_Force)( Audio_Buf + ABuf_Tail, ABuf_Head-ABuf_Tail );
   ABuf_Head = ABuf_Tail;
}

EXTERN INT Flush_Audio( ) // try to flush as many buffer bytes as possible
{
   INT Len;
   if ( Audio_Drv==NULL ) return( 0 );
   if ( ABuf_Tail==ABuf_Head ) return( 0 );
   if ( ABuf_Head<ABuf_Tail )
   {
      Len = ABuf_Len - ABuf_Tail;
#if 0
      if ( Len<4096 )   // avoid too short a transfert...
      {
         INT i;
         for( i=ABuf_Head-1; i>=0; --i ) Audio_Buf[Len+i] = Audio_Buf[i];
         ABuf_Head += Len;
         if ( Len ) memcpy( Audio_Buf, Audio_Buf+ABuf_Tail, Len );
         ABuf_Tail = 0;
      }
      else
#endif
      {
         Len = (*Audio_Drv->Flush)( Audio_Buf + ABuf_Tail, Len );
         ABuf_Tail += Len;
         Len = ABuf_Len - ABuf_Tail;
         if ( Len ) return( Len );
         ABuf_Tail = 0;
      }
   }
   Len = ABuf_Head-ABuf_Tail;
//   if ( Len<4096 ) return( Len ); 
   Len = (*Audio_Drv->Flush)( Audio_Buf + ABuf_Tail, Len );
   ABuf_Tail += Len;
   return( ABuf_Head-ABuf_Tail ); // ==0 if ok.
}

EXTERN void Update_Audio( void *Buf, INT Len )
{
   INT How;
   if ( Audio_Drv==NULL ) return;
   if ( ABuf_Head>=ABuf_Tail ) How = ABuf_Len-ABuf_Head;
   else How = ABuf_Tail-ABuf_Head;
   if ( Len<How ) How = Len; 
   memcpy( Audio_Buf+ABuf_Head, Buf, How );
   Len -= How;
   if ( Len>0 )  // still to transfert
   {
      Buf = (void*)( (BYTE*)Buf  + How );
      memcpy( Audio_Buf, Buf, Len );
      ABuf_Head = Len;
   }
   else ABuf_Head += How;
}

/******************************************************************/

EXTERN void Close_Audio( )
{
   if ( Audio_Drv!=NULL ) (*Audio_Drv->Close)( );
   Reset_Audio_Buffer( );
   Audio_Drv = NULL;
}

EXTERN void Reset_Audio_Buffer( )
{
   M_Free( Audio_Buf );
   ABuf_Head = ABuf_Tail = ABuf_Len = 0;
}
 
EXTERN BYTE *Init_Audio_Buffer( INT Len )
{
   if ( Len<16384 ) Len = 16384;
   Reset_Audio_Buffer( );
   Audio_Buf = (BYTE*)My_Malloc( (Len)*sizeof( BYTE ) );
   if ( Audio_Buf == NULL ) return( NULL );
   ABuf_Len = Len;
   ABuf_Head = ABuf_Tail = 0;
   return( Audio_Buf );
}

EXTERN void Reset_Audio( )
{
   if ( Audio_Drv!=NULL ) (*Audio_Drv->Reset)( );
}

/******************************************************************/

EXTERN void Register_Audio_Driver( AUDIO_DRIVER *Drv )
{
   if ( Last_Driver<MAX_AUDIO_DRV ) Registered_Drv[Last_Driver++] = Drv;
}

/******************************************************************/

