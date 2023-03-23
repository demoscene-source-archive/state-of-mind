/***********************************************
 *            picture savers                   *
 * Skal 98                                     *
 ***********************************************/

#ifndef _SV_ANY_H_
#define _SV_ANY_H_

/*******************************************************************/

   // in sv_any.c
extern void Save_Any( char *Name, BITMAP *Btm );

   // in sv_gif.c
extern void Save_GIF( char *, BITMAP * );

   // in ppm.c
extern void Save_PPM( char *Name, BITMAP *Bitmap, PIXEL *Pal );

   // in sv_btm.c
extern void Save_BTM( char *, BITMAP * );

   // in raw.c
extern void Save_RAW( char *Name, BITMAP *Bitmap, PIXEL *Pal );

/*******************************************************************/

#endif   // _SV_ANY_H_
