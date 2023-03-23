/***********************************************
 *            RLE-encoded DMASK                *
 * Skal 98                                     *
 ***********************************************/

#ifndef _DMASK_H_
#define _DMASK_H_

/********************************************************************/

#define DMASK_MAGIC "d@MsK"

typedef enum {
   CRLE_EOL   = 0xFF,
   CRLE_SKIP  = 0xFE,
   CRLE_FILL  = 0xFD,
   CRLE_BLOCK = 0xFC,
   CRLE_SOLO  = 0xFB,
   CRLE_LAST  = 0xFA
} DMASK_RLE_TYPE;

typedef struct {
#pragma pack(1)
   INT Width, Height, Size; 
   INT Off_X, Off_Y;
   USHORT *Bits;
   
   INT Nb_Col;
   PIXEL *Pal;
   INT MBits;

      // RLE...
   INT Pak_Size;
   INT *H_Index;
   BYTE *Raw;
   USHORT Alpha;
   USHORT RLE_Type;  // 0=full  1=BLOCK/FILL   2=SKIP/BLOCK/FILL

} DMASK;

extern DMASK *New_DMask( INT W, INT H, INT Nb_Col );
extern DMASK *Load_DMask( STRING Name );
extern USHORT *Split_Mask( BYTE *Src, INT Size, INT MBits );
extern void Destroy_DMask( DMASK *);
extern void *Uncompress_DMask( DMASK *DMask, USHORT *Dst );
extern void *Uncompress_DMask_II( DMASK *DMask, PIXEL *Dst );
extern void DMask_Setup_Pal( UINT *CMap, DMASK *DMask, PIXEL *Pal );

   // in dmaskout.o

extern DMASK *Compress_DMask( DMASK *DMask, PIXEL *In_Buf, 
   INT Alpha, INT Type );
extern DMASK *Save_DMask( DMASK *, STRING Name );

/********************************************************************/

#endif   // _DMASK_H_
