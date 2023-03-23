/***********************************************
 *         misc compile flags                  *
 *                                             *
 * all these flags can be overwritten throught *
 * gcc's options                               *
 * Skal 97                                     *
 ***********************************************/

#ifndef _CONFIG_H_
#define _CONFIG_H_

////////////////////// General //////////////////////////////////////////////

#ifndef DO_INDY_SWAP
#define DO_INDY_SWAP      // for SGI & SUN
#endif

#ifndef NO_SOUND
//#define NO_SOUND
//#define SUNOS_AUDIO           // fast hack
//#define IRIX_AUDIO           // fast hack
#endif

#ifndef USE_RDTSC
// #define USE_RDTSC
#endif

   // Skip some non-vital functions (for final compil)

#ifndef SKL_LIGHT_CODE
#define SKL_LIGHT_CODE
#endif


// #define DEBUG(x)   x
#define DEBUG(x)      

#define FUNC(x)      (x)
// #define FUNC(x)

// #define FINAL


/////////////////////// misc defines and config flags ///////////////////////

#if defined( LNX ) || defined( UNIX )

#ifndef USE_X11         // shall we use X11 ?
// #define USE_X11
#endif

#ifndef  USE_SVGALIB    // shall we use SVGALIB ?
// #define USE_SVGALIB
#endif

#ifndef USE_DGA         // shall we use DGA?
// #define USE_DGA
#endif

#endif   // LNX or UNIX


#if defined(__DJGPP__)

#ifndef USE_VBE
#define USE_VBE
#endif

#ifndef USE_DDRAW
// #define USE_DDRAW
#endif

#endif   // DJGPP

#if defined(__WATCOMC__)

#ifndef USE_VBE
#define USE_VBE
#endif

#ifndef USE_DDRAW
// #define USE_DDRAW
#endif

#endif   //WATCOM

#if defined( WIN32 )

#ifndef USE_DDRAW
// #define USE_DDRAW
#endif

#endif   //WIN32

/*****************************************************************/
/* old.                                                          */
/*****************************************************************/

   // shall we use mp3 decompression for modules's ? (msamples.c)

#ifndef MP3_USE_SAMPLES
// #define MP3_USE_SAMPLES
#endif

/*****************************************************************/
/*****************************************************************/

#endif   // _CONFIG_H_

