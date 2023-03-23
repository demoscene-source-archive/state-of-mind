/***********************************************
 *              Sprites                        *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#ifndef _SPRITE_H_
#define _SPRITE_H_

/********************************************************************/

typedef struct 
{
#pragma pack(1)
   INT Uo, Vo;
   INT dU, dV;
   PIXEL *Bits;
   INT BpS;
} SPRITE;

extern INT   Sp_Offsets0[ ];
extern void *Sp_Whats0[ ];
extern UINT *Sp_Dst;
extern INT   Sp_BpS;
extern INT   Sp_dWo, Sp_dHo;
extern void *Sp_CMap;

/********************************************************************/

extern void *Setup_Paste_Sprite( void *Dst, INT BpS, INT Dst_Quantum,
   FLT xo, FLT yo, FLT R, SPRITE *Sprite );

/********************************************************************/

extern void Paste_Sprite_Bitmap_16( USHORT *Dst, INT BpS, FLT xo, FLT yo, FLT R, SPRITE *Sprite );
extern void Paste_Sprite_Asm_16( );   //USHORT *Where, INT BpS, PIXEL **Whats, INT *Offsets, INT dWo, INT dHo );

extern void Paste_Sprite_Bitmap_8( PIXEL *Dst, INT BpS, FLT xo, FLT yo, FLT R, SPRITE *Sprite );
extern void Paste_Sprite_Asm_8( );    //PIXEL *Where, INT BpS, PIXEL **Whats, INT *Offsets, INT dWo, INT dHo );

extern void Paste_Sprite_Bitmap_88( PIXEL *Dst, INT BpS, FLT xo, FLT yo, FLT R, SPRITE *Sprite );
extern void Paste_Sprite_Asm_88( );   //PIXEL *Where, INT BpS, PIXEL **Whats, INT *Offsets, INT dWo, INT dHo );

extern void Paste_Sprite_Bitmap_8_Map16_Or( USHORT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite, USHORT *CMap );
extern void Paste_Sprite_Bitmap_Asm_8_Map16_Or( );

extern void Paste_Sprite_Bitmap_8_Map16( USHORT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite, USHORT *CMap );
extern void Paste_Sprite_Bitmap_Asm_8_Map16( );

extern void Destroy_Sprite( SPRITE *S );
extern SPRITE *Btm_16_To_Sprite( BITMAP_16 *Src );

/********************************************************************/

extern SPRITE *Btm_16_To_Sprite_777( BITMAP_16 *Src );
extern SPRITE *Load_Sprite_777_JPEG( STRING Name );
extern void Destroy_Sprite_777( SPRITE *S );

   // not very useful...
extern SPRITE *Mask_Sprite_777_GIF( STRING Name, SPRITE *Sprite, INT Mix_Levels );
extern SPRITE *Mask_Sprite_777_JPEG( STRING Name, SPRITE *Sprite, INT Mix_Levels );
   
extern void Paste_Sprite_Asm_Sat_32( );
extern void Paste_Sprite_Asm_Mask_32( );
extern void Paste_Sprite_Asm_Raw_Mask_32( );
extern void Paste_Sprite_Asm_32( );

      // old !!!
extern UINT R_Remap777[256], G_Remap777[256], B_Remap777[256];
#define RGB_777_TO_777(i)  ( R_Remap777[(i)>>16] | G_Remap777[((i)>>8)&0xFF] | B_Remap777[(i)&0xFF] )
#define RGB_777_TO_777_BLEND(m,i)  ( R_Remap777[(i)>>16] | G_Remap777[((i)>>8)&0xFF] | B_Remap777[(i)&0xFF] )

/********************************************************************/

#endif   // _SPRITE_H_
