/***********************************************
 *               clip.h                        *
 *                                             *
 * Sutherland-Hodgman clipper algorithm        *
 * Skal 97                                     *
 ***********************************************/

#ifndef _CLIP_H_
#define _CLIP_H_

/******************************************************************/

#define VTX_CLIP_Xo   0x04
#define VTX_CLIP_X1   0x08
#define VTX_CLIP_Yo   0x10    // used for 3dfx
#define VTX_CLIP_Y1   0x20    // used for 3dfx
#define VTX_CLIP_Zo   0x40
#define VTX_CLIP_Z1   0x80

#define FLAG_POLY_NOT_VISIBLE 0x00     // warning. Don't change.
#define FLAG_POLY_VISIBLE     0x02     // warning. Don't change.

#define FLAG_VTX_NOT_TESTED 0x00
#define FLAG_VTX_REJECTED   0x01 // warning. Don't change.
#define FLAG_VTX_TESTED     0x02 // warning. Don't change.
#define FLAG_VTX_VISIBLE    0x03 // =(FLAG_VTX_TESTED|FLAG_VTX_REJECTED). Don't change.

#define FLAG_FOR_CLIPPING   (FLAG_VTX_VISIBLE|VTX_CLIP_Yo|VTX_CLIP_Y1)
#define FLAG_CLIP_USEFUL    (VTX_CLIP_Xo|VTX_CLIP_X1|VTX_CLIP_Zo|VTX_CLIP_Z1)
#define FLAG_FOR_CLIPPING_II   (FLAG_VTX_VISIBLE)
#define FLAG_CLIP_USEFUL_II    (VTX_CLIP_Xo|VTX_CLIP_X1|VTX_CLIP_Yo|VTX_CLIP_Y1|VTX_CLIP_Zo|VTX_CLIP_Z1)

/******************************************************************/

extern void SH_Clip( USHORT All_Flags );
extern void SH_Clip_Full( USHORT All_Flags );

/******************************************************************/

extern void Split_UVc( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );
extern void Split_UVc_Gouraud( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );

extern void Split_UV( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );
extern void Split_Raw( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );
extern void Split_UV_Gouraud( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );
extern void Split_UV_Gouraud2( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );
extern void Split_Gouraud( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );
extern void Split_Shadow( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );
extern void Split_Shadow_UV( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );
extern void Split_UV_Z( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );
extern void Split_Raw_Z( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );

/******************************************************************/

#endif   // _CLIP_H_

