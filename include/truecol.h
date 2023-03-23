/***********************************************
 *           TRUE COLOR bitmaps                *
 * Skal 98                                     *
 ***********************************************/

#ifndef _TRUECOL_H_
#define _TRUECOL_H_

/*******************************************************************/

   // useful macros

#define RGB_TO_332(r,g,b)  ( ((r)&0xE0)|(((g)&0xE0)>>3)|(((b)&0xC0)>>6) )
#define RGB_TO_555(r,g,b)  ( (((r)&0xF8)<<7)|(((g)&0xF8)<<2)|(((b)&0xF8)>>3) )
#define RGB_TO_565(r,g,b)  ( (((r)&0xF8)<<8)|(((g)&0xFC)<<3)|(((b)&0xF8)>>3) )
#define RGB_TO_777(r,g,b)  ( (((r)&0xFE)<<15)|(((g)&0xFE)<<7)|(((b)&0xFE)>>1) )
#define RGB_TO_888(r,g,b)  ( (((r)&0xFF)<<16)|(((g)&0xFF)<<8)|((b)&0xFF) )

/*******************************************************************/

typedef struct 
{
#pragma pack(1)

   INT Width, Height;
   INT Size;
   UINT Format;
   void *Bits;
   INT Nb_Col;
   UINT *Pal;

} BITMAP_16;

extern BITMAP_16 *Load_16b( char *Name );
extern void Destroy_16b( BITMAP_16 *Btm );
extern void Drv_Build_Ramp_16( void *Pal, 
   INT Start, INT n,
	INT r1, INT g1, INT b1, INT r2, INT g2, INT b2,
	INT Format );

   // Type -> 0: fade between Mixing_Col and Col
   //      -> 1: fade between Mixing_Col and T[Col]
   //      -> 2: fade between T[Col] and Col
   //      -> 3: fade between Col and T[Col]
            
extern void CMap_To_16bits( void *CMap, PIXEL *Pal, INT Nb_Col, INT Format );
extern void CMap_To_16bits_With_Fx(
   void *CMap, PIXEL *Pal, INT Nb_Col, INT Format,
   INT Mixing_Col, INT T[3][3], INT Level, INT Type );
extern void CMap_To_16bits_With_Mix( void *CMap,
   PIXEL *Pal, INT Nb_Col, INT Format,   
   INT Mix, INT Ro, INT Go, INT Bo );
extern void Prepare_Remap_RGB565( USHORT *Map1, USHORT *Map2, BYTE Mtx[3][3] );

/*******************************************************/

   // in ld_jpg.c

extern BITMAP_16 *Load_Raw_JPEG( char *Name, INT *Nb_Components, USHORT Format );
#define Load_JPEG(a,b)  Load_Raw_JPEG( (a),(b),0x3888 )
#define Load_JPEG_565(a,b)  Load_Raw_JPEG( (a),(b),0x2565 )

#include "bit_strm.h"

extern BIT_STRM *JPEG_Load_Bitstream( char *Name );
extern BITMAP_16 *JPEG_Decode_Bitstream( BIT_STRM *Bit_Strm, 
   INT *Nb_Components, USHORT Format );

/*******************************************************/

   // in ppm.c

extern BITMAP_16 *Load_PPM_16b( STRING Name, USHORT Format );

/*******************************************************************/

#endif   // _TRUECOL_H_

