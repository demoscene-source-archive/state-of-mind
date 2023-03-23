/*
 * - FX prototypes -
 *
 * Skal 98
 ***************************************/

#ifndef _FX_H_
#define _FX_H_

/********************************************************************/

   // useful macros
#define VB(n)  (Mixer.VBuffers[(n)])
#define DISPOSE(a)     Dispose_VBuffer( &VB((a)) )
#define SELECT_565(a)  Select_VBuffer( &VB((a)), V565, The_W, The_H, 2*The_W, 0x00 )
#define SELECT_565_V(a)  Select_VBuffer( &VB((a)), V565, The_W, The_H, 2*The_W, VBUF_VIRTUAL )
#define SELECT_CMAP(a)  Select_VBuffer( &VB((a)), VCMAP, The_W, The_H, The_W, 0x00 )
#define SELECT_CMAP_V(a)  Select_VBuffer( &VB((a)), VCMAP, The_W, The_H, The_W, VBUF_VIRTUAL )
#define SELECT_777(a)  Select_VBuffer( &VB((a)), V777, The_W, The_H, 4*The_W, 0x00 )

   // some reserved buffers

#define VSCREEN   0
#define VMASK2    4
#define VMASK     5
#define SCRATCH   6
#define JANIM     7
#define VBLUR_I   8
#define VBLUR_II  9
#define VLETTERS_II 10
#define VLETTERS  11
#define MAX_VBUFFER 12

/********************************************************************/

#define MIX_LEVELS   16

      // Lo16_To_777[n]/Hi16_To_777[n] fades to black as
      // n goes from 0 to MIX_LEVELS-1
      // this ones are used for blending 2 16bits sources together
      //
      // Lo16_To_777_White[n]/Hi16_To_White[n] fades to White as
      // n goes from 0 to MIX_LEVELS-1
      //
      // Lo16_To_16[n]/Hi16_To_16[n] and Lo16_To_16_White[n]/Hi16_To_16_White[n] 
      // does the same, but in 16bits.      

extern UINT Lo16_To_777[MIX_LEVELS][256];
extern UINT Hi16_To_777[MIX_LEVELS][256];
extern UINT Lo16_To_777_White[MIX_LEVELS][256];
extern UINT Hi16_To_777_White[MIX_LEVELS][256];
extern UINT Lo16_To_777_M[256];
extern UINT Hi16_To_777_M[256];
extern USHORT RGB71_To_16[3][256];
extern USHORT RGB71_To_16_M[3][256];
extern UINT RGB71_To_16_Remap[3][256];
extern UINT RGB71_To_16_Remap_M[3][256];
extern UINT R_Remap777[256], G_Remap777[256], B_Remap777[256];

extern USHORT Lo16_To_16[MIX_LEVELS][256];
extern USHORT Hi16_To_16[MIX_LEVELS][256];
extern USHORT Lo16_To_16_White[MIX_LEVELS][256];
extern USHORT Hi16_To_16_White[MIX_LEVELS][256];

/********************************************************************/

      // Macros for mixing

#define RGB_16_TO_777(i)  ( Lo16_To_777[0][(i)&0xFF] + Hi16_To_777[0][(i)>>8] )
#define RGB_16_TO_777_M(i)  ( Lo16_To_777_M[(i)&0xFF] + Hi16_To_777_M[(i)>>8] )
#define RGB_16_TO_777_Blend(b,i)  ( Lo16_To_777[(b)][((i)&0xFF)] + Hi16_To_777[(b)][((i)>>8)] )
#define RGB_16_TO_777_Blend_Tab(lo,hi,b,i)  ( (lo)[((b)<<8)|((i)&0xFF)] + (hi)[((b)<<8)|((i)>>8)] )
#define RGB_16_TO_16_Fade(lo,hi,i)  ( (lo)[(i)&0xFF] + (hi)[(i)>>8] )
#define RGB_777_TO_16(a) ( RGB71_To_16[0][(a)&0xFF] + RGB71_To_16[1][((a)>>8)&0xFF] + RGB71_To_16[2][((a)>>16)&0xFF] )
#define RGB_777_TO_16_M(a) ( RGB71_To_16_M[0][(a)&0xFF] + RGB71_To_16_M[1][((a)>>8)&0xFF] + RGB71_To_16_M[2][((a)>>16)&0xFF] )
#define RGB_777_TO_777(i)  ( R_Remap777[(i)>>16] | G_Remap777[((i)>>8)&0xFF] | B_Remap777[(i)&0xFF] )
#define RGB_777_TO_16_Remap(a) ( RGB71_To_16_Remap[0][(a)&0xFF] | RGB71_To_16_Remap[1][((a)>>8)&0xFF] | RGB71_To_16_Remap[2][((a)>>16)&0xFF] )
#define RGB_777_TO_16_Remap_M(a) ( RGB71_To_16_Remap_M[0][(a)&0xFF] | RGB71_To_16_Remap_M[1][((a)>>8)&0xFF] | RGB71_To_16_Remap_M[2][((a)>>16)&0xFF] )

/********************************************************************/

      // Useful tab and mixing setup functions

extern UINT *CMap_To_777( PIXEL *CMap, INT Nb_Col );
extern UINT *Btm_CMap_To_777( BITMAP *Btm );
extern void Init_CMap_Ramp_Pos( BYTE *CMap,
   INT Ro, INT Go, INT Bo, INT Rf, INT Gf, INT Bf, 
   INT Nb, INT Nb2 );
extern void Init_CMap_Ramp_Neg( BYTE *CMap,
   INT Ro, INT Go, INT Bo, INT Rf, INT Gf, INT Bf, 
   INT Nb, INT Nb2 );
extern void CMap_Split( USHORT Maps[3][256], BYTE *CMap, INT Nb );
extern void CMap_Split_Remap( UINT Maps[3][256], 
   BYTE *CMap1, BYTE *CMap2, INT Nb );

/********************************************************************/

      // mixing functions to plug

typedef struct {

   VBUFFER VBuffers[ MAX_VBUFFER ];

   void (*Mix_Sat16)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_Sat16_Clear12)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_Sat16_Clear1)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_Sat16_Clear2)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );


   void (*Mix_Sat16_8)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_Sat16_8_Clear12)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_Sat16_8_Clear1)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_Sat16_8_Clear2)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

   void (*Mix_Sat16_8_M)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

   void (*Mix_Sat8_8)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_Sat8_8_Clear12)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_Sat8_8_Clear1)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_Sat8_8_Clear2)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

   void (*Mix_Sat8_8_Zero)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

   void (*Mix_8_8_Map)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

   void (*Mix_16_16_Mix)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
               UINT *TabLo1, UINT *TabHi1, UINT *TabLo2, UINT *TabHi2 );
   void (*Mix_16_16_Mix_Copy)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
               UINT *TabLo1, UINT *TabHi1, UINT *TabLo2, UINT *TabHi2 );

   void (*Mix_16_16_Blend)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
                VBUFFER *Blend, INT Xor, UINT *TabLo, UINT *TabHi );

   void (*Mix_16_16_Blend_Tab)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
               VBUFFER *Blend, UINT *TabLo, UINT *TabHi );

   void (*Mix_16_8_Blend_Mask)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_16_8_Blend_Mask_II)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

   void (*Mix_Sat8_8_Blend)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2,
               VBUFFER *Bld );

   void (*Mix_8_8_Blend_Mask)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );
   void (*Mix_8_8_Blend_Mask_II)( VBUFFER *Out, VBUFFER *In1, VBUFFER *In2 );

   void (*Mix_16_To_16)( VBUFFER *Out, USHORT *TabLo, USHORT *TabHi );

   void (*Mix_To_Fade)( VBUFFER *Out, VBUFFER *In, USHORT *TabLo, USHORT *TabHi );

   void (*Mix_To_Fade_Feedback)( VBUFFER *Out, VBUFFER *In, USHORT *TabLo, USHORT *TabHi );

   void (*Mix_8_Mask)( VBUFFER *Out, VBUFFER *In );

   void (*Mix_16_Mask)( VBUFFER *Out, VBUFFER *In );

   void (*Mix_Sat16_16_Feedback)( VBUFFER *Out, VBUFFER *In );

   void (*Mix_777_To_16)( VBUFFER *Out, VBUFFER *In );
   void (*Mix_777_To_16_Remap)( VBUFFER *Out, VBUFFER *In );
   void (*Mix_777_To_16_Zero)( VBUFFER *Out, VBUFFER *In );

   void (*Mix_Dsp_Off)( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off );
   void (*Mix_Dsp_Off_Safe_X)( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off, INT *Sat );
   void (*Mix_Dsp_Off_Safe_Y)( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off, INT *Sat );
   void (*Mix_Dsp_Off_Blur_X)( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off, INT *Sat );
   void (*Mix_Dsp_Off_Blur_Y)( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off, INT *Sat );

} MIXER;

/********************************************************************/

      // main call for setup 

extern MIXER Mixer;
extern void Init_Mixing( INT Use_MMX );

/********************************************************************/

   //  stuff in fx2.c

#define The_W2 640
#define The_H2 400
extern USHORT Map1_Dsp[The_W2*The_H2];    // tunnel
extern USHORT Map2_Dsp[The_W2*The_H2];    // tunnel II
extern SHORT Dsp[The_W*The_H];            // radial dsp
extern PIXEL *Blend_Ramp;  // -- [3*The_W*The_H];  ramp for blending

extern void Init_Tunnels( INT Use_MMX );

extern BYTE *Build_Plasma_I( int THE_W, int THE_H, FLT Ho );
extern void Do_Plasma_2Pt( void *Dst_p, INT Dst_BpS, 
   void *Src1_p, void *Src2_p, INT Src_BpS,
   INT W, INT H );

/********************************************************************/

   // in fade.c

// extern USHORT Lo_Fade[256], Hi_Fade[256];
// extern USHORT Lo_Fade_White[256], Hi_Fade_White[256];

extern void Loop_Fade( );

// extern INT Cnt_Fade, Cnt_Fade2;
// extern INT Fade[16], Fade_II[16];
// extern void Init_Fade_White( INT Param );
// extern void Loop_Fade_White( );

   // in scratch.c

extern void Init_Scratch( );
extern void Do_Old_Scratch( VBUFFER *Out );
extern void Do_Old_Scratch_M( VBUFFER *Out );
extern void Get_Next_Scratch( INT Adv );
extern void Scratch_Next_Ramp( INT Off );
extern void Scratch_Next_Ramp_M( INT Off );
extern void Scratch_And_Flash( FLT x );
extern void Scratch_And_Flash_II( FLT x );

   // in janim.c

extern LINT Timer0;
extern INT Next_Beat;

extern INT JA_Parity;
extern void Paste_JPEG_Anim( );
extern void Paste_JPEG_Anim_Move( );
extern void Init_JPEG_Anim( );
extern void Setup_Anim( );
extern void Set_Anim_CMap( VBUFFER *VBuf, INT N, 
   INT Ro, INT Go, INT Bo, INT R, INT G, INT B, UINT Format );
extern void Paste_JPEG_Anim_Sprites( );
extern INT Random();
extern INT TRandom( INT Slot );

extern void Pre_Cmp_Destroy_Screen( );
extern void Destroy_Screen( VBUFFER *Out, FLT x );
extern void Destroy_Screen_II( VBUFFER *Out );
extern void Destroy_Screen_III( VBUFFER *Out );
extern void Destroy_Screen_XOr( VBUFFER *Out, FLT x );
extern FLT Get_Beat( );
extern FLT Get_Beat_2( );
extern FLT Get_Beat_3( );
extern FLT Get_Beat_4( );

extern void Init_Anim_And_Mask( INT Format );

   // in mmask.c

// extern void Setup_Mask( INT VMask, DMASK *, UINT Format );
extern void Setup_Mask_II( INT VMask, DMASK * );

extern void Saturate_DMask_Pal( VBUFFER *V, FLT x, DMASK *Mask, UINT Color );
extern void Clear_DMask( );
extern void Clear_DMask_And_Anim( );
extern void Extract_DMask_Raw_Col( DMASK *M, BYTE *Dst );
extern void Extract_DMask_Blend( DMASK *M, BYTE *Dst );

#define Paste_DMask(Out,Mask,In) Mixer.Mix_8_8_Blend_Mask(Out,Mask,In)
#define Paste_DMask_16(Out,Mask,In) Mixer.Mix_16_8_Blend_Mask(Out,Mask,In)
#define Paste_DMask_Move(Out,Mask,In) Mixer.Mix_8_8_Blend_Mask_II(Out,Mask,In )
// extern void Paste_DMask( VBUFFER *Out, VBUFFER *Mask, VBUFFER *In );
// extern void Paste_DMask_16( VBUFFER *Out, VBUFFER *Mask, VBUFFER *In );
// extern void Paste_DMask_Move( VBUFFER *Out, VBUFFER *Mask, VBUFFER *In );
extern void Paste_DMask_Move_II( VBUFFER *Out, VBUFFER *Mask, VBUFFER *In );

   // in spr.c & spr2.asm

extern void Paste_Sprite_Bitmap_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite );
extern void Paste_Sprite_Bitmap_Sat_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite );
extern void Paste_Sprite_Bitmap_Mask_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite );
extern void Paste_Sprite_Bitmap_Raw_Mask_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite );

/********************************************************************/

   // in flare16s.asm

//extern void Paste_Flare_Bitmap_16_Sat( FLT xo, FLT yo, FLT R, 
//   FLARE *Flare, FLARE_SPRITE *Sprite );
//extern void Paste_Flare_Asm_16_Sat( );

/********************************************************************/
/********************************************************************/

#endif   // _FX_H_
