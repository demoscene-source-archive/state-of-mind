/***********************************************
 *               vshadow.h                     *
 * Skal 97                                     *
 ***********************************************/

#ifndef _VSHADOW_H_
#define _VSHADOW_H_

/******************************************************************/

#define MAX_SIL_VTX  1024        // !!!

extern P_VERTICE *Cap_Vtx;
extern BYTE *Cap_State;

#define Dz_EPSILON      (-.01f)
#define _TMAX_         (40.0f)

   // in sh_vshdw.c

extern void Render_zBuf_Shade( );

   // in vshadow.c

extern INT Shader_Emit_VShadow_Poly( );
extern void Build_Shadow_Volume( MATRIX2 M );
extern SHADER_METHODS Light_zBuf_Shader;

   // in vshadow2.c
extern SHADER_METHODS Light_zBuf_Shader2;

   // in vshadow3.c
extern SHADER_METHODS Light_zBuf_Shader3;

   // in vshadow4.c
extern void Select_Sil_Edges3( MATRIX2 M );
extern void Build_Shadow_Volume2( MATRIX2 M );
extern SHADER_METHODS Light_zBuf_Shader4;

   // in vshadow5.c
extern SHADER_METHODS Light_zBuf_Shader5;

/******************************************************************/

#endif   // _VSHADOW_H_
