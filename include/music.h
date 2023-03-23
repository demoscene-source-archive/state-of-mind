/*
 * Music headers
 *
 ******************/

#ifndef _MUSIC_H_
#define _MUSIC_H_

#define DISABLE_MIKMOD

#define ASK_MIX_FREQ    1
#define ASK_DRIVER      2

#define DEFAULT_FREQ 44100
// #define DEFAULT_FREQ 32000

/********************************************************************/

#ifndef DISABLE_MIKMOD

#ifdef LNX
#define USE_MIKMOD_3
#endif

#ifdef DOS

#define USE_MIKMOD_3
// #define USE_MIKMOD_2

#endif

#ifdef __WATCOMC__

#define USE_MIKMOD_3
// #define USE_MIKMOD_2

#endif

/********************************************************************/

// #define USE_LOAD_IT
#define USE_LOAD_S3M
// #define USE_LOAD_UNI


typedef struct {

   void *The_Song;

   void  (*Init_Player)( char *Name, int Menu, int *Freqs );
   void  (*Close_Player)( void *Song );
   void  (*Start_Song)( void *Song );
   void  (*Set_Song_Pos)( INT Pos, INT Row );
   void  (*Update_Sound)( );

   void  (*Set_Sample_Length)( INT Smp, INT Length );
   void *(*Get_Sample_Address)( INT Smp );
   void  (*Print_Infos)( void *Song );

} SOUND_DRIVER;


extern void *The_Song;

extern void *Init_Player( char *Name, int Menu, int *Freqs );
extern void Close_Player( void *mf );
extern void Start_Song( void *Song );
extern void Set_Sound_Pos( INT Pos, INT Row );
extern void Update_Sound( );

/********************************************************************/
/*                Mikmod v3.x interface                             */
/********************************************************************/

#ifdef USE_MIKMOD_3

// #ifdef __WATCOMC__
      // used in mikmod's  tdefs.h  (line 66-67)
#define __int64      long int
// #endif

#if defined(__DJGPP__)||defined(__WATCOMC__)
#include "mikmodd.h"
#endif

#ifdef LNX
#include "mikmodl.h"
#endif

#endif   // USE_MIKMOD_3

/********************************************************************/
/*                Mikmod v2.x interface                             */
/********************************************************************/

#ifdef USE_MIKMOD_2

#if defined(__DJGPP__)||defined(__WATCOMC__)
#include "mikmod2.h"
#endif

#endif      // USE_MIKMOD_2

/********************************************************************/

#endif   // DISABLE_MIKMOD

#endif   // _MUSIC_H_

