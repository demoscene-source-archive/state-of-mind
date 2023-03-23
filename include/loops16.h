/***********************************************
 *         16bpp loops                         *
 * Skal98                                      *
 ***********************************************/

#ifndef _LOOPS16_H_
#define _LOOPS16_H_

extern void _Draw_Flat_16( );
extern void _Draw_Flat2_16( );
extern void _Draw_Gouraud_16( );
extern void _Draw_Gouraud_Ramp_16( );
extern void _Draw_Gouraud2_16( );
extern void _Draw_UVc_4_16( );
extern void _Draw_UVc_8_16( );
extern void _Draw_UVc_16_16( );
extern void _Draw_UVc_32_16( );
extern void _Draw_UVc_64_16( );
extern void _Draw_UVc_16( );
extern void _Draw_zBuf_Gouraud_16( );
extern void _Draw_zGouraud_16( );
extern void _Draw_16b_zShadow_256_16( );

extern void _Draw_ALine_16( FLT xo, FLT yo, FLT x1, FLT y1 );

   // in draw16/bilin16.c

extern void _Draw_UVc_Bilin_16( );
extern void Init_UV_Mult_Tab_16b( );

extern float UV_Mult_Tab_I[64*256][2];
extern float UV_Mult_Tab_II[64*256][2];
extern float Cf_32_RB[65536];
extern float Cf_32_G[65536];

#endif   // _LOOPS16_H_
