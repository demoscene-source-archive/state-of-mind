/***********************************************
 *              BITMAP utilities               *
 * Skal 96                                     *
 ***********************************************/

#ifndef _BTM_H_
#define _BTM_H_

/************************** #defines *******************************/

#define BTM_MAGIC_HEADER   "&skal_btm#"

typedef struct BITMAP BITMAP;
struct BITMAP
{
#pragma pack(1)

   INT Width, Height, Size;
   PIXEL *Bits;

   PIXEL *Pal;
   int Nb_Col;

};

   // Abstract type for BITMAP I/O...

typedef struct {
#pragma pack(1)

   INT Width, Height, Nb_Col;

} BITMAP_IO;

/*******************************************************************/

#include "color.h"

   // in btm_aa.c

extern void Resize_Bitmap_Anti_Alias( BITMAP *, int, int );

   // in btm.c

extern BITMAP *New_Bitmap( int Width, int Height, int Pal_Size );
extern void Destroy_Bitmap( BITMAP * );

   // in btm_util.c

extern BITMAP *Resize_Bitmap( BITMAP *, int, int );

#ifndef SKL_LIGHT_CODE_

extern BITMAP *Duplicate_Bitmap( BITMAP *Src, INT Width, INT Height );
extern void Print_BTM_Infos( BITMAP *, FILE *);

extern PIXEL Search_Max_Color( BITMAP *);
extern void Filter_Bitmap( BITMAP *, PIXEL And, PIXEL Or );
extern void Scroll_Bitmap_Up( PIXEL *Src, USHORT H, USHORT W, USHORT  );
extern void Scroll_Bitmap_Down( PIXEL *Src, USHORT H, USHORT W, USHORT );

#endif   // SKL_LIGHT_CODE_

   // in remapbtm.c

#ifndef SKL_LIGHT_CODE_

extern void Remap_BTM( BITMAP *Btm, PIXEL *Pal, INT Nb );
extern void Remap_BTM_II( BITMAP *Btm, PIXEL *Pal, INT Nb, INT Transp );
extern void Offset_BTM( BITMAP *Btm, PIXEL Offset );
extern void Offset_BTM_2( BITMAP *Btm, PIXEL Offset, PIXEL Zero );
extern void Rescale_BTM( BITMAP *Btm, PIXEL Min, PIXEL Max );
extern void Rescale_BTM_2( BITMAP *Btm, PIXEL Min, PIXEL Max, PIXEL Zero );

extern void Remap_8bits( PIXEL *Ptr, PIXEL *Map, INT Size );
extern INT Reduce_CMap( PIXEL *Order, PIXEL *CMap, INT Nb_Col, INT Pad_To );

#endif   // SKL_LIGHT_CODE_

/*******************************************************************/

#endif   // _BTM_H_

