/***********************************************
 *        Bitmaps as MEM_ZONE                  *
 * Skal 97                                     *
 ***********************************************/

#ifndef _MEM_BTM_H_
#define _MEM_BTM_H_

/************************** #defines *******************************/

#define BTM_MAGIC_HEADER   "&skal_btm#"

#define MBTM_WITHOUT_CMAP      0x01
#define MBTM_WITHOUT_BLUR_PAL  0x02
#define MBTM_WITHOUT_SAT_PAL   0x04
#define MBTM_WITHOUT_BITS      0x08
#define MBTM_SPLIT_CHANNELS    0x10   /* else: BTM_PACKED_PIXEL */
#define MBTM_IS_MIP_MAP        0x20
#define MBTM_HAS_TRANSPARENT   0x40
#define MBTM_HAS_ALPHA         0x80
#define MBTM_PACKED            0x100

#define Mem_Btm_Has_Transparent(B)   ( (B)->Type & BTM_HAS_TRANSPARENT )
#define Mem_Btm_Has_Alpha(B)         ( (B)->Type & BTM_HAS_ALPHA )

/********************* main struct *********************************/

typedef struct MEM_BITMAP MEM_BITMAP;
struct MEM_BITMAP
{
#pragma pack(1)

   MEM_ZONE_FIELDS

   PIXEL *Bits;

   COLOR_ENTRY *Pal;
   int Nb_Col;

   PIXEL *Blur_Pal;
   int Nb_Blur_Col;
   
   PIXEL *Sat_Pal;
   int Nb_Sat_Col;

   INT Transparent;
   float Alpha; 

};

   /* Abstract type for MEM_BITMAP I/O... */

typedef struct {
#pragma pack(1)

   INT Type;
   INT Width, Height;
   INT Pixel_Depth, CMap_Depth;
   int Nb_Col, Nb_Blur_Col, Nb_Sat_Col;

} MEM_BITMAP_IO;

/*******************************************************************/

   /* functions in ld_btm.c and sv_btm.c */

extern MEM_BITMAP *Load_MEM_BITMAP( char * );
extern void Save_MEM_BITMAP( char *, MEM_BITMAP * );

   /* functions in mem_btm.c */
extern MEM_BITMAP *Full_MEM_BITMAP( 
   FORMAT Format, 
   int Width, int Height,  PIXEL *Bits,
   int Nb_Col, int Nb_Blur_Pal_Col, int Nb_Sat_Pal_Col );
extern MEM_BITMAP *New_MEM_BITMAP( FORMAT Format, int Width, int Height, int Nb_Col );
extern MEM_BITMAP *Align_MEM_BTM_64( MEM_BITMAP * );

extern MEM_BITMAP *Duplicate_MEM_BITMAP( MEM_BITMAP *Src, INT Width, INT Height );
extern void Resize_MEM_BITMAP( MEM_BITMAP *, int, int );
extern MEM_BITMAP *Destroy_Mem_Bitmap( MEM_BITMAP *Bitmap );

/*******************************************************************/

#endif   // _MEM_BTM_H_

