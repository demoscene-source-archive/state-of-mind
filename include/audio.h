/***********************************************
 *              Audio streaming.               *
 * Skal 98                                     *
 ***********************************************/

#ifndef _AUDIO_H_
#define _AUDIO_H_

#define _AUDIO_16BITS_ 0x01
#define _AUDIO_STEREO_ 0x02
#define _AUDIO_DEBUG_  0x04
#define _AUDIO_FLAGS_ (_AUDIO_DEBUG_|_AUDIO_STEREO_|_AUDIO_16BITS_)
#define _AUDIO_ASK_DRIVER_  0x08

/******************************************************************/

typedef struct {
#pragma pack(1)

   STRING Name;
   INT  (*Ping)( );
   INT  (*Init)( INT, INT );
   void (*Reset)( );
   void (*Close)( );
   INT  (*Flush)( BYTE *, INT );
   void (*Flush_Force)( BYTE *, INT );
   USHORT Flags;

} AUDIO_DRIVER;

/******************************************************************/

extern INT ABuf_Len, ABuf_Min_Len;
extern INT ABuf_Head, ABuf_Tail;
extern BYTE *Audio_Buf;
extern USHORT Audio_Out_Mode, Audio_In_Mode;
extern INT Audio_Mix_Freq;

extern AUDIO_DRIVER *Init_Audio( AUDIO_DRIVER *Drv, 
   INT Mix_Freq, INT Mode, INT Ask );
extern void Close_Audio( );
extern INT Flush_Audio( );
extern void Flush_Audio_Force( );
extern void Reset_Audio( );

extern void Update_Audio( void *Buf, INT Len );
// extern void Update_Audio( BYTE *Left, BYTE *Right, INT Len );
extern void Reset_Audio_Buffer( );
extern BYTE *Init_Audio_Buffer( INT Len );

#if defined(__WATCOMC__)
#define inportb(x)  inp(x)
#define inport(x)   inpw(x)
#define outportb(x,y) outp(x,y)
#define outport(x,y)  outw(x,y)
#define disable()      _disable()
#define enable()       _enable()
#endif

#if defined(__DJGPP__)
#define interrupt
#define far
#endif

/******************************************************************/

#ifndef NO_SOUND

#if defined( LNX ) || defined( UNIX )
extern AUDIO_DRIVER _OSS_Drv_;
#define _OSS_DRIVER_    (&_OSS_Drv_)

#endif

#if defined(__DJGPP__)||defined(__WATCOMC__)
extern AUDIO_DRIVER _SB_Drv_;
extern AUDIO_DRIVER _SB16_Drv_;
extern AUDIO_DRIVER _SBPro_Drv_;
extern AUDIO_DRIVER _AWE32_Drv_;
extern AUDIO_DRIVER _WSS_Drv_;


// #define _SB_DRIVER_    (&_SB_Drv_)
#define _SB16_DRIVER_    (&_SB16_Drv_)
#define _SBPRO_DRIVER_    (&_SBPro_Drv_)
//#define _AWE32_DRIVER_    (&_AWE32_Drv_)
#define _WSS_DRIVER_    (&_WSS_Drv_)

#endif

#if defined( WIN32 )
extern AUDIO_DRIVER _DSound_Drv_;
#define _DSOUND_DRIVER_    (&_DSound_Drv_)

#endif // WIN32
#endif   // NO_SOUND

/******************************************************************/

#ifndef _OSS_DRIVER_
#define _OSS_DRIVER_    NULL
#endif
#ifndef _SB_DRIVER_
#define _SB_DRIVER_    NULL
#endif
#ifndef _SB16_DRIVER_
#define _SB16_DRIVER_    NULL
#endif
#ifndef _SBPRO_DRIVER_
#define _SBPRO_DRIVER_    NULL
#endif
#ifndef _AWE32_DRIVER_
#define _AWE32_DRIVER_    NULL
#endif
#ifndef _WSS_DRIVER_
#define _WSS_DRIVER_    NULL
#endif
#ifndef _DSOUND_DRIVER_
#define _DSOUND_DRIVER_    NULL
#endif

/******************************************************************/

#define MAX_AUDIO_DRV   7

extern void Register_Audio_Driver( AUDIO_DRIVER * );
extern void Register_All_Audio_Drivers( );

/******************************************************************/
      // special WAV driver...

// #define WAV_SWAP_BYTE
extern AUDIO_DRIVER _WAV_Drv_;
extern STRING WAV_File_Name;
extern FILE *WAV_Out_File;

/******************************************************************/

#endif   // _AUDIO_H_
