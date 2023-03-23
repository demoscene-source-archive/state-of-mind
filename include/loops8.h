/***********************************************
 *          8bpp loops                         *
 * Skal98                                      *
 ***********************************************/

#ifndef _LOOPS8_H_
#define _LOOPS8_H_

extern void _Draw_Flat_8( );
extern void _Draw_Flat2_8( );
extern void _Draw_Gouraud_8( );
extern void _Draw_Gouraud_Ramp_8( );
extern void _Draw_Gouraud2_8( );
extern void _Draw_UV_8( );
extern void _Draw_UV2_8( );
extern void _Draw_UVc_1_8( );
extern void _Draw_UVc_2_8( );
extern void _Draw_UVc_4_8( );
extern void _Draw_UVc_8_8( );
extern void _Draw_UVc_16_8( );
extern void _Draw_UVc_32_8( );
extern void _Draw_UVc_64_8( );
extern void _Draw_UVc_8( );
extern void _Draw_zBuf_Gouraud_8( );
extern void _Draw_zGouraud_8( );
extern void _Draw_16b_zShadow_256_8( );

extern void _Draw_ALine_8( FLT xo, FLT yo, FLT x1, FLT y1 );

#endif   // _LOOPS8_H_
