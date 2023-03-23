/*
 * - ASM mixings and stuff -
 *
 * Skal 98 (skal.planet-d.net)
 ***************************************/

#ifndef _MIX_H_
#define _MIX_H_

/********************************************************************/

      // in mix.asm

extern void Mix_Sat16( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   // USHORT *Dst, INT Dst_BpS,
   // USHORT *Src1, USHORT *Src2, INT W, INT H, INT Src_BpS );
extern void Mix_Sat16_Clear12( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_Sat16_Clear1( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_Sat16_Clear2( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

extern void Mix_Sat16_8( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_Sat16_8_Clear12( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_Sat16_8_Clear1( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_Sat16_8_Clear2( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_Sat16_8_M( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

extern void Mix_Sat8_8( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_Sat8_8_Clear12( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_Sat8_8_Clear1( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_Sat8_8_Clear2( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

extern void Mix_Sat8_8_Zero( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

extern void Mix_8_8_Map( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

extern void Mix_16_16_Mix( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
   UINT *TabLo1, UINT *TabHi1, UINT *TabLo2, UINT *TabHi2 );
extern void Mix_16_16_Mix_Copy( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
   UINT *TabLo1, UINT *TabHi1, UINT *TabLo2, UINT *TabHi2 );

extern void Mix_16_16_Blend( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
   VBUFFER *Blend, INT Xor, UINT *TabLo, UINT *TabHi );

extern void Mix_16_8_Blend_Mask( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_16_8_Blend_Mask_II( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

extern void Mix_Sat8_8_Blend( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
   VBUFFER *Bld );

extern void Mix_8_8_Blend_Mask( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
extern void Mix_8_8_Blend_Mask_II( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

extern void Mix_16_To_16( VBUFFER *Out, USHORT *TabLo, USHORT *TabHi );

extern void Mix_To_Fade( VBUFFER *Out, VBUFFER *In, USHORT *TabLo, USHORT *TabHi );

extern void Mix_To_Fade_Feedback( VBUFFER *Out, VBUFFER *In, USHORT *TabLo, USHORT *TabHi );

extern void Mix_8_Mask( VBUFFER *Out, VBUFFER *In );

extern void Mix_16_Mask( VBUFFER *Out, VBUFFER *In );

extern void Mix_Sat16_16_Feedback( VBUFFER *Out, VBUFFER *In );
extern void Mix_777_To_16( VBUFFER *Out, VBUFFER *In );
extern void Mix_777_To_16_Remap( VBUFFER *Out, VBUFFER *In );
extern void Mix_777_To_16_Zero( VBUFFER *Out, VBUFFER *In );

extern void Mix_Dsp_Off( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off );
extern void Mix_Dsp_Off_Safe_X( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off, INT *Sat );
extern void Mix_Dsp_Off_Safe_Y( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off, INT *Sat );
extern void Mix_Dsp_Off_Blur_X( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off, INT *Sat );
extern void Mix_Dsp_Off_Blur_Y( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off, INT *Sat );

extern void Do_Plasma_256( PIXEL *Dst, PIXEL *In,
   PIXEL U1, PIXEL V1, PIXEL U2, PIXEL V2 );
extern void Paste_256( VBUFFER *Out, PIXEL *Src, INT U1, UINT *CMap );
extern void Paste_256_Y( VBUFFER *Out, PIXEL *Src, 
   INT V1, UINT *CMap, INT Src_W );

/********************************************************************/

      // in paste.asm

extern void Paste_Sprite_Raw_8x8( PIXEL *Dst, PIXEL *Src );
extern void Paste_Sprite_Raw_4x4( PIXEL *Dst, PIXEL *Src );
extern void Paste_Sprite_Sat_8x8( PIXEL *Dst, PIXEL *Src );
extern void Paste_Sprite_Sat_4x4( PIXEL *Dst, PIXEL *Src );

extern void Do_Blur_Y_Loop( BYTE *Dst, INT Dst_BpS,
   BYTE *Src, INT W, INT H, INT Src_BpS,
   USHORT *Sat1, INT *Int1  );

/********************************************************************/

      // in mix2.asm

extern void Map_Tunnel( PIXEL *Dst, PIXEL *Map, USHORT *Src, USHORT UoVo );
extern void Map_Tunnel_Add( PIXEL *Dst, PIXEL *Map, USHORT *Src, USHORT UoVo,
   USHORT *Src2, USHORT UoVo2 );

extern void Map_Tunnel_II( PIXEL *Dst, PIXEL *Dst2,
   PIXEL *Map, PIXEL *Map2, USHORT *Src, USHORT UoVo );

extern void Do_Da_Bump( PIXEL *Dst, INT Dst_BpS, PIXEL *Src,
   PIXEL *Env_Map, INT xo, INT yo );
extern void Do_Da_Bump_2( PIXEL *Src, INT Dst_BpS, 
   PIXEL *Env_Map, INT xo, INT yo );

extern void Paste_III( PIXEL *Dst, 
   INT Dst_BpS, PIXEL *Src, INT W, INT H, INT Src_BpS );
extern void Paste_III_Map( PIXEL *Dst,
   INT Dst_BpS, PIXEL *Src, INT W, INT H, INT Src_BpS, USHORT *CMap );

/********************************************************************/

   // should be in asm...

extern void Remap_Byte( BYTE *Dst, BYTE *Src, INT Size, BYTE *Map );
extern void Remap_RGB16( USHORT *Dst, USHORT *Src, INT Size, BYTE *Map1, BYTE *Map2 );

/********************************************************************/

#endif   // _MIX_H_
