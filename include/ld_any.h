/***********************************************
 *           picture loaders                   *
 * Skal 98                                     *
 ***********************************************/

#ifndef _LD_ANY_H_
#define _LD_ANY_H_

/*******************************************************************/

   // in ld_any.c
extern BITMAP *Load_Any( char *Name );

   // in ld_gif.c
extern BITMAP *Load_GIF( char * );

   // in ppm.c
extern BYTE *Load_Raw_PPM( STRING Name, USHORT Format,
   INT *W, INT *H ); // <= used only for cache III ?
extern BITMAP *Load_PPM( char *Name );

   // in ld_btm.c
extern BITMAP *Load_BTM( char * );
extern BITMAP *Load_BTM_Old( char * );

   // in tga.c
extern BITMAP *Load_TGA( char *Name );

   // in bmp.c
extern BITMAP *Load_BMP( char *Name );

   // in raw.c
extern BITMAP *Load_RAW( char *Name );

/*******************************************************************/

#endif   // _LD_ANY_H_
