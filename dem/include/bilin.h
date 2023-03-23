/*
 * - Bilinear -   (/ray-tracing)
 *
 * Skal 98
 ***************************************/

#ifndef _BILIN_H_
#define _BILIN_H_

   // in bilin2.c
extern void Do_Bilinear_8( PIXEL *Out, PIXEL *In,
   INT W, INT H, INT Dst_BpS );
extern void Do_Bilinear_4( PIXEL *Out, PIXEL *In,
   INT W, INT H, INT Dst_BpS );
extern void Do_Bilinear_UV_8( USHORT *Out, INT *UV,
   INT W, INT H, INT Dst_BpS );
extern void Do_Bilinear_UV_Map_8( PIXEL *Out, INT *UV,
   INT W, INT H, INT Dst_BpS, PIXEL *Map );
extern void Do_Bilinear_UV_Map_32b_8( UINT *Out, INT *In,
   INT W, INT H, INT Dst_BpS, UINT *Map );

   // in bilin.asm
extern void Do_Bilin_Line_8( PIXEL *Out, SHORT *C, SHORT *dC, INT W );
extern void Do_Bilin_Line_4( PIXEL *Out, SHORT *C, SHORT *dC, INT W );
extern void Do_Bilin_Line_UV_8( USHORT *Out, UINT *UV, UINT *dUV, INT W );
extern void Do_Bilin_Line_UV_Map_8( PIXEL *Out, USHORT *UV, USHORT *dUV, INT W, PIXEL *Map );
extern void Do_Bilin_Line_UV_Map_32b_8( UINT *Out, SHORT *UV, SHORT *dUV, INT W, UINT *Map );

#endif   // _BILIN_H_
