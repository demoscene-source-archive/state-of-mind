/***********************************************
 *              Lights                         *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _LIGHT_H_
#define _LIGHT_H_

// #define USE_OLD_SHADOWZ
// #define DEBUG_SHADOW

/******************************************************************/

   // In flare.c

#define MAX_FLARE_SPRITES  10

typedef struct 
{
#pragma pack(1)
   INT Uo, Vo;
   INT dU, dV;
   FLT Scale;
   FLT Alpha;
} FLARE_SPRITE;

typedef struct {
#pragma pack(1)
   void *Bits;
   PIXEL *Convert;
   INT Width, Height, BpS;
   INT Nb_Sprites;
   FLARE_SPRITE *Sprites;
} FLARE;

#if 0
   // flr02.gif
#define FLARE1_PHTSHP                  \
{ { 0,0,128,128, 1.0, 0.0 },           \
  { 141,8,75,78, 1.0, 0.5 },           \
  { 0,0,128,128, 0.4, 1.6 },           \
  { 141,8,75,78, 1.0, 1.8 }            \
}
#define FLARE1_PHTSHP_NB_SPRITES  4

   // flare2.gif
#define FLARE2_PHTSHP {                    \
  { 0,0,   182,182, 1.0,  0.0 },           \
  { 240,0, 9,9,   1.5,  0.40 },      /*3*/ \
  { 212,0, 22,22, 2.0,  0.65 },      /*4*/ \
  { 212,0, 22,22, 3.2,  1.20 },      /*5*/ \
  { 240,0, 9,9,   3.0,  1.32 },      /*6*/ \
  { 183,1, 24,24, 2.9,  1.56 },      /*7*/ \
  { 214,30,20,20, 3.5,  1.71 },      /*8*/ \
  { 184,30,27,27, 2.9,  2.19 },     /*11*/ \
  { 184,30,27,27, 2.2,  0.20 },     /*13*/ \
}
#define FLARE2_PHTSHP_NB_SPRITES  9
#endif

extern void Render_Light_Flare( WORLD *W );
// extern PIXEL *Promote_Flare_Btm( BITMAP *Btm, FCOLOR *Big_Tab, INT Total_Col );

   // in flare16.c and flare16s.asm

extern void Paste_Flare_Bitmap_16_Sat( FLT xo, FLT yo, FLT R,
   FLARE *Flare, FLARE_SPRITE *Sprite );
extern void Paste_Flare_Asm_16_Sat( );
   
/******************************************************************/

   // Light type

#define LIGHT_NOT_SET      0
#define LIGHT_DIR          1
#define LIGHT_POINT        2
#define LIGHT_SPOT         3

struct LIGHT {
#pragma pack(1)

   OBJECT_FIELDS
   INT    Light_Type;
   VECTOR Dir, Pos, Target;          /* Original values */
   FLT    Offset, Amplitude;

   FCOLOR Color;
   OBJ_NODE *Last_Viewed;
   void *Vista_Buffer;

      // Spot_Light parameters
   FLT    Hot_Spot, Fall_Off, Roll, Dolly;
   FLT    Cos_Hot, Cos_Fall, Out_fall;


   FLT Z_Min, Z_Max, Mult;
   VECTOR D, P;              /* Transformed values */
   FLT    Xp, Yp, Inv_Z;     /* Projected pos */
   FLT    Dx, Dy, D_Inv_Z;   /* Projected dir */

   FLARE *Flare;

   FLT Flare_Scale;
   INT Where;

   FLT Inv_Z_Min, Inv_Z_Max;
   SHADER_METHODS *Shader;
   OBJ_NODE *Dummy;
   SPLINE_3DS *Pos_k, *Rot_k, *Target_k, *Roll_k;
   SPLINE_3DS *Color_k, *Hotspot_k, *Falloff_k;
   SPLINE_3DS *Hide_k;

   void *Light_Map;
};

extern NODE_METHODS _LIGHT_, _LIGHT_WITH_RENDER_;

/******************************************************************/

extern void Vista_Select_Light( OBJ_NODE *Cur_Node, INT Order );

extern void Default_Light( LIGHT *L );
extern LIGHT *Create_Light( WORLD *W );
extern void Set_Light_Parameters( LIGHT *L,
   FLT Offset, FLT Amplitude, FLT Alpha, FLT Fall_Off );
extern void Compute_Spot_Light_Parameter( LIGHT *L );   
extern void Compose_Light_And_Camera( OBJ_NODE *Node, OBJ_NODE *Cam );
extern void Select_Light_Visibility( OBJ_NODE *Node );
// extern void Set_Light_Pos_And_Dir( LIGHT *Light, VECTOR Pos, VECTOR Look_At );
extern void Light_To_Matrix( LIGHT *, MATRIX2 M );

/******************************************************************/

extern void Sort_Light_Boxes( LIGHT *Light, OBJ_NODE *Cur );

extern void Render_zLight( MATRIX2 M );
extern void Render_zBuf_Light( );
extern void Render_zBuf_Light2( );

extern void *Setup_Paste_Flare( INT Dst_Quantum, FLT xo, FLT yo, FLT R,
   FLARE *Flare, FLARE_SPRITE *Sprite );
   
extern void Paste_Flare_Bitmap_8( FLT X, FLT Y, FLT Ratio, FLARE *, FLARE_SPRITE * );
extern void Paste_Flare_Asm_8( );
extern void Paste_Flare_Bitmap_88( FLT X, FLT Y, FLT Ratio, FLARE *, FLARE_SPRITE * );
extern void Paste_Flare_Asm_88( );
extern void Paste_Flare_Bitmap_16( FLT X, FLT Y, FLT Ratio, FLARE *, FLARE_SPRITE * );
extern void Paste_Flare_Asm_16( );

/******************************************************************/
/******************************************************************/

#ifdef USE_OLD_SHADOWZ

#define LIGHT_BUF_W 256
#define LIGHT_BUF_H 256
#define LIGHT_BUF_SIZE (LIGHT_BUF_W*LIGHT_BUF_H)

   // Max projected vertex

#define MAX_LIGHT_VERTEX   10000
extern USHORT UV_Vertex[ MAX_LIGHT_VERTEX ];
extern FLT Inv_Z_Vertex[ MAX_LIGHT_VERTEX ];    /* Or 16 bits only ?!? */
extern USHORT Vertex_ID;
extern USHORT *Shadow_IDs;
extern USHORT The_ID;

extern USHORT *Light_Buf;
extern OBJECT *Light_Root;

extern USHORT Light_Buffer2[256*256];

extern SHADER_METHODS Shadow_Shader;
extern SHADER_METHODS zShadow_Shader;
extern void Shader_Emit_Shadow_Polys( );

extern void Render_Light( LIGHT *L, OBJ_NODE *Root );

#endif

/******************************************************************/

#endif   // _LIGHT_H_
