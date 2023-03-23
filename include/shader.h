/***********************************************
 *               shader.h                      *
 * Skal 97                                     *
 ***********************************************/

#ifndef _SHADER_H_
#define _SHADER_H_

/******************************************************************/

typedef struct {
#pragma pack(1)

   FLT xp, yp, Inv_Z;  // 12
   F_MAPPING UV;       // 20
   VECTOR N;           // 32    // Store transformed normal, or else...

} P_VERTICE;

typedef struct {
#pragma pack(1)

   VECTOR N;         // 12
   POLY *Poly_Orig;  // 16
   FLT  Key;         // 20

//   void *Txt;        // 16    // pointer to TEXTURE, or to POLY, or...
//   USHORT C;         // 22
//   USHORT UV[3];     // 28    // should be MAX_POLY_PTS !!

} P_POLY;

typedef struct {
#pragma pack(1)

   void (*Set_Parameters)( void *, ... );
   void (*Transform)( MATRIX2 );

      /* common shaders setup. */

   void (*Split_Edge)( FLT, P_VERTICE *, P_VERTICE *, P_VERTICE * );
   void (*Shade_Vertex)( MATRIX2 );

      /* shaders */

   RENDERING_FUNCTION (*Deal_With_Poly)( );
   RENDERING_FUNCTION (*Deal_With_Poly2)( );
   RENDERING_FUNCTION (*Deal_With_Poly3)( );

   void (*Emit_Poly)( );  // replace Render.Emit_Poly() if not NULL.

} SHADER_METHODS;


#include "loops.h"

extern SHADER_METHODS UV_Shader, UVg_Shader, UVl_Shader, UVl2_Shader;
extern SHADER_METHODS UV_Shader, Env_Shader, UV_Env_Shader;
extern SHADER_METHODS UVc_Shader, UVc2_Shader;

extern SHADER_METHODS UV_Shadow_Lighten_Shader, UV_zShadow_Lighten_Shader;
extern SHADER_METHODS UV_Alpha_Shader;

/******************************************************************/

extern void Emit_Poly( );        // For renderers...
extern void Setup_Poly_Vtx( );

      // Shader loops

extern void Shader_Draw_UV( );
extern void Shader_Draw_UVc( );
extern void Shader_Draw_UV2( );

extern void Shader_Draw_UVc2( );
extern void Shader_Draw_UVc2_An( );
extern void Shader_Draw_UVc2_Ap( );

     // in raster.c

extern void Rasterize_Edges( );
extern void Format_Scanlines( );
extern void Rasterize_Grd( );
extern void Rasterize_iZ( );
extern void Rasterize_UV( );     // with UV[0]/UV[1]
extern void Rasterize_UV2( );    // with N[0]/N[1]
extern void Rasterize_UVc( );

extern void Rasterize_UVc_1( );
extern void Rasterize_UVc( );

/******************************************************************/

extern FLT DxU, DxV, DzU, DxiZ, DxS, DxT;
extern FLT DyU, DyV, DzV, DyiZ, DyS, DyT;
extern FLT U, dU1, dU2;
extern FLT V, dV1, dV2;
extern FLT iZ, diZ1, diZ2;
extern FLT S, dS1, dS2;
extern FLT T, dT1, dT2;

extern FLT dy0, dy1, dx0, dx1;
extern INT dyS, dyT, dyU, dyV, dyiZ;

/******************************************************************/

#endif   // _SHADER_H_

