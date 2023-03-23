/***********************************************
 *       Color converter                       *
 * Skal 96                                     *
 ***********************************************/

#ifndef _CVRT_COL_H_
#define _CVRT_COL_H_

   // Color/format converters cvrt_col.c

#ifdef USE_ASM_CONVERTERS

extern void Skl_CMap8_To_RGB32( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_CMap8_To_RGB24( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_CMap8_To_RGB16( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );

extern void Skl_RGB8_To_RGB8( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB8_To_RGB16( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB8_To_RGB24( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB8_To_RGB32( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB16_To_RGB8( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB16_To_RGB16( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB16_To_RGB24( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB16_To_RGB32( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB24_To_RGB8( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB24_To_RGB16( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB24_To_RGB32( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB32_To_RGB8( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB32_To_RGB16( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void Skl_RGB32_To_RGB24( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );

extern void Skl_RGB8_To_CMap8( SKL_CMAPPER *Mask, PIXEL *Dst, PIXEL *Src, INT Size );
extern void Skl_RGB16_To_CMap8( SKL_CMAPPER *Mask, PIXEL *Dst, PIXEL *Src, INT Size );
extern void Skl_RGB24_To_CMap8( SKL_CMAPPER *Mask, PIXEL *Dst, PIXEL *Src, INT Size );
extern void Skl_RGB32_To_CMap8( SKL_CMAPPER *Mask, PIXEL *Dst, PIXEL *Src, INT Size );

#endif

extern void (*Skl_Convert_Tab[])( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void (*Skl_Indexer_Tab[4])( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
extern void (*Skl_Ditherer_Tab[4])( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );

#endif // _CVRT_COL_H_

