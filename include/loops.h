/***********************************************
 *            Loops and shaders                *
 * Skal98                                      *
 ***********************************************/

#ifndef _LOOPS_H_
#define _LOOPS_H_

// #define DO_MAP_STATS

   /***********************************************
    *             8/16bpp loops                   *
    ***********************************************/

   // specific

#include "loops8.h"
#include "loops88.h"
#include "loops16.h"

   // common. In loops1.asm or loops2.c

extern void _Draw_UV_Bump( );
extern void _Draw_UV_16b( );
extern void _Draw_UV_Offset( );
extern void _Draw_zBuf( );
extern void _Draw_Flat_zBuf( );

   // useful??
extern void _Draw_16b_256( );
extern void _Draw_Flat_Shadow( );
extern void _Draw_Flat_zShadow( );
extern void _Draw_Flat_Mix_8( );       // in loops_8.asm
      // in line8.c

extern void Draw_ALine( FLT xo, FLT yo, FLT x1, FLT y1 );
extern void ALine( FLT xo, FLT yo, FLT x1, FLT y1, PIXEL *Dst, INT BpS );


   /***********************************************
    *              Flat shader                    *
    ***********************************************/

extern RENDERING_FUNCTION Shader_Deal_Lighten_Flat( );
extern RENDERING_FUNCTION Shader_Deal_Lighten_Flat2( );
extern RENDERING_FUNCTION Shader_Deal_Flat( );
extern RENDERING_FUNCTION Shader_Deal_Flat3( );

extern void Set_Param_Flat( void *Obj, ... );

extern SHADER_METHODS Flat_Shader, Flat_Lighten_Shader;
extern SHADER_METHODS UVl_Shader;

   /***********************************************
   *           Line & 'design' shader             *
   ***********************************************/

extern void Emit_Outlined_Poly( );
extern void Transform_Outlined_Vertices( MATRIX2 M );
extern void Emit_3D_Line( VECTOR Pi, VECTOR Pf );

extern RENDERING_FUNCTION Shader_Draw_All_Lines( );
extern RENDERING_FUNCTION Shader_Draw_Lines( );
extern RENDERING_FUNCTION Shader_Draw_Visible_Lines( );
extern RENDERING_FUNCTION Shader_Raw_Lines( );
extern SHADER_METHODS Flat_Lighten_Anti_Shader, Flat_Anti_Shader;
extern SHADER_METHODS Raw_Line_Shader, UVc_Line_Shader;
extern SHADER_METHODS Flat_All_Anti_Shader;
extern SHADER_METHODS Only_Line_Shader, Only_All_Line_Shader;
extern SHADER_METHODS UV_Anti_Shader, UV_zBuf_Anti_Shader;

   /***********************************************
   *           Gouraud shader                    *
   ***********************************************/

extern FLT Gouraud_Offset, Gouraud_Amp, Gouraud_Base;
extern VECTOR Total_Dir;
extern VECTOR Total_Pos;

extern void Shader_Shade_Gouraud( MATRIX2 );
extern RENDERING_FUNCTION Shader_Deal_Gouraud( );
extern RENDERING_FUNCTION Shader_Deal_Gouraud2( );

extern SHADER_METHODS Gouraud_Shader, UVg_Shader, UV_Env_Gouraud_Shader;

  /***********************************************
   *                UV shader                    *
   ***********************************************/

extern void Shader_Shade_UV( MATRIX2 );
extern void Shader_Shade_UVl( MATRIX2 );
extern void Shader_Shade_UV_Gouraud( MATRIX2 );
extern RENDERING_FUNCTION Shader_Deal_UV( );;

extern void Set_Param_UV( void *Obj, ... );
extern void Set_Param_UV_Env( void *Obj, ... );

extern SHADER_METHODS UV_Shader;

  /***********************************************
   *       UV sub-pixel/texel shader             *
   ***********************************************/

extern void Shader_Shade_UV_16b( MATRIX2 );
extern RENDERING_FUNCTION Shader_Deal_UV( );
extern SHADER_METHODS UV_Shader, UV_16b_Shader;

  /***********************************************
   *       UV corrected sub-pixel/texel shader   *
   ***********************************************/

extern void Shader_Shade_UVc_Lighten( MATRIX2 );
extern void Shader_Shade_UVc_Gouraud( MATRIX2 );
extern void Shader_Shade_UVc_Gouraud2( MATRIX2 );

extern RENDERING_FUNCTION Shader_Deal_UVc( );

extern SHADER_METHODS UVc_Shader, UVc_Lighten_Shader;
extern SHADER_METHODS UVc_Gouraud, UVc_Gouraud2;

#ifdef DO_MAP_STATS

extern INT St_Lin, St_64p, St_32p, St_16p, St_8p, St_4p, St_2p;
#define ADD_STAT(x)  (x)
extern void P_St_Stats( );

#else

#define ADD_STAT(x)
#define P_St_Stats()

#endif   // DO_MAP_STATS

  /***********************************************
   *       Bump mapping shader                   *
   ***********************************************/

extern SHADER_METHODS Bump_Shader;
extern SHADER_METHODS Bump_Shader2;
extern void Shader_Shade_Bump( MATRIX2 );

extern RENDERING_FUNCTION Shader_Deal_Bump5( );
extern RENDERING_FUNCTION Shader_Deal_Bump4( );

  /***********************************************
   *               Env. shader                   *
   ***********************************************/

extern void Shader_Env_Mapper( MATRIX2 );
extern void Shader_Env_Mapper2( MATRIX2 );
extern RENDERING_FUNCTION Shader_Deal_UV_Env( );
extern RENDERING_FUNCTION Shader_Deal_UV_Env2( );
extern SHADER_METHODS Env_Shader, UV_Env_Shader;

  /***********************************************
   *                 Z. shader                   *
   ***********************************************/

extern void Shader_Shade_UV_Gouraud_zBuf( MATRIX2 );
extern void Shader_Shade_UV_Gouraud2_zBuf( MATRIX2 );

extern RENDERING_FUNCTION Shader_Deal_zBuf( );

extern SHADER_METHODS UVc_zBuf_Shader;
extern SHADER_METHODS UV_Gouraud_zBuf_Shader;
extern SHADER_METHODS UVc_Gouraud_zBuf_Shader;
extern SHADER_METHODS UVc_Gouraud2_zBuf_Shader;
extern SHADER_METHODS UVc_Flat3_zBuf_Shader;

extern SHADER_METHODS UVc_Lighten_zBuf_Shader;
extern SHADER_METHODS UV_Env_Gouraud_zBuf;
extern SHADER_METHODS zBuf_Shader;

  /***********************************************
   *             Shadow. stuff                   *
   * (in sh_shdw.c)                              *
   ***********************************************/

#ifdef USE_OLD_SHADOWZ

extern void (*Shader_Deal_UV_Shadow( ))( );
extern void (*Shader_Deal_UV_zShadow( ))( );
extern void (*Shader_Deal_zShadow( ))( );
extern void (*Shader_Deal_UV_Shadow2( ))( );

extern void Shader_Deal_Lighten_Shadow( MATRIX2 );
extern void Shader_Deal_Lighten_zShadow( MATRIX2 );

extern SHADER_METHODS Flat_Shadow_Lighten_Shader;
extern SHADER_METHODS Flat_zShadow_Lighten_Shader;
extern SHADER_METHODS Flat_zShadow_Lighten_Shader;
extern SHADER_METHODS UV_Shadow_Lighten_Shader;
extern SHADER_METHODS UV_zShadow_Lighten_Shader;

#endif   // USE_OLD_SHADOWZ

/******************************************************************/

#endif   // _LOOPS_H_
