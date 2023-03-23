/***********************************************
 *         Poly rasterizer                     *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _RASTER_H_
#define _RASTER_H_

/******************************************************************/

      // used in clip.c

extern INT Nb_Edges_To_Deal_With;
extern INT Nb_Out_Edges;
extern P_VERTICE *Out_Vtx[ MAX_EDGES+1+MAX_ADDITIONAL_VERTICES];
extern P_VERTICE *P_Edges[MAX_EDGES+1];
extern USHORT Vtx_Flags[ MAX_EDGES+1+MAX_ADDITIONAL_VERTICES];

extern FLT Edges_x[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Edges_dx[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
extern FLT Edges_y[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Edges_dy[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
extern FLT Edges_z[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Edges_dz[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
extern FLT Slopes[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Slope_Full[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
extern FLT Edges_ey[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES];
extern INT Edges_yf[MAX_EDGES+MAX_ADDITIONAL_VERTICES], Edges_yo[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
extern INT Edges_Xe[MAX_EDGES+MAX_ADDITIONAL_VERTICES], Edges_dXe[MAX_EDGES+MAX_ADDITIONAL_VERTICES];

extern INT Scan_Start, Scan_H;
extern INT Span_Len, Span_Y;
extern INT Nb_Right_Edges;
extern USHORT Right_Edges[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
extern INT Nb_Left_Edges;
extern USHORT Left_Edges[MAX_EDGES+MAX_ADDITIONAL_VERTICES];

/******************************************************************/

   // TODO: The following should be condensed in lesser arrays...

extern INT Scan_Pt1[MAX_SCANS];
extern INT Scan_Pt2[MAX_SCANS];
extern INT Scan_U[MAX_SCANS], Scan_V[MAX_SCANS];
extern INT Scan_S[MAX_SCANS];
// extern INT Scan_T[MAX_SCANS];
extern FLT Scan_UiZ[MAX_SCANS], Scan_ViZ[MAX_SCANS], Scan_iZ[MAX_SCANS];
extern FLT Scan_UiZ_2[MAX_SCANS], Scan_ViZ_2[MAX_SCANS], Scan_iZ_2[MAX_SCANS];

// extern USHORT XBuffer[ MAX_WIDTH ];

extern FLT Area, Area2, aZ1, aZ2;

         /* Fine-tuned constants */

#define B_CULL_EPSILON  0.75f
#define B_CULL_LIMIT   -0.005f

#define SLOPE_EPSILON      0.001f
#define GOURAUD_EPSILON    0.06f    // color underflow cutoff
#define GOURAUD_EPSILON2   0.005f   // gradient cut off
#define GOURAUD_SCALE      (65536.0*255.9f)
#define iZ_SCALE           1.0f      // 256.0
#define UV_SCALE           65535.0f     // float->int convertion for UV
#define SHADOW_SCALE       (65536.0f)
#define INV_Z_SCALE        (65536.0f*32768.0f*64.0f)
#define INV_Z_EPSILON      -256.0f
#define Z_EPSILON          0.00001f
#define AREA_EPSILON       (1.0/256.0f)    // limit ~= 16x16 pixels
#define AREA_EPSILON2      (1.0/4.0f)      // limit ~= 2x2 pixels
#define AREA_EPSILON3      (.2f)           // limit ~= .7x.7 pixels
#define NZc_EPSILON        .1f

#define UVc_EPSILON_LIN   (0.07f)      // Full persp. correction
#define UVc_EPSILON64     (0.14f)      // 64pixels persp. correction
#define UVc_EPSILON32     (0.20f)      // 32pixels persp. correction
#define UVc_EPSILON16     (0.30f)      // 16pixels persp. correction
#define UVc_EPSILON8      (0.44f)      // 8pixels persp. correction
#define UVc_EPSILON4      (0.55f)      // 4pixels persp. correction
#define UVc_EPSILON2      (0.80f)      // 2pixels persp. correction


#define EPS_MIP_BLF  (1.0/256.0f)
#define EPS_MIP_0    (4.0/256.0f)
#define EPS_MIP_1    (8.0/256.0f)
#define EPS_MIP_2    (16.0/256.0f)
#define EPS_MIP_3    (32.0/256.0f)
#define EPS_MIP_4    (64.0/256.0f)
#define EPS_MIP_5    (128.0/256.0f)


   /* common data needed by Draw_*() and shaders funcs */

extern PIXEL *UV_Src, *UV_Src2;
extern void *Light_Ramp;
extern USHORT Flat_Color, Flat_Color2;
extern POLY *Orig_Poly;
extern P_POLY *Cur_P_Poly;
extern P_VERTICE *Cur_P_Vertex;
extern PIXEL *Cur_Vertex_State;
extern UINT Cur_Vtx_Nb;
extern UINT Cur_Poly_Pt;
extern UINT Cur_P_Vertex_Nb;
extern TEXTURE_MAP *Cur_Texture;
extern PIXEL *Cur_Texture_Ptr;
extern MATERIAL *Cur_Material;
extern INT Constants_Computed;

/******************************************************************/

// extern INT Scratch_Int;

#if 1

#define FLT2INT(I,Fl)   (*(I)) = ceil( *(Fl) )
#define FLT2INT(I,Fl)   (*(I)) = ceil( *(Fl) )
/* #define FLT2INT(I,Fl)   (*(I)) = floor( *(Fl) ) */
/* #define FLT2INT(I,Fl)   (*(I)) = (INT)( *(Fl) ) */
#define FRAC_FLT(Res,Fl)  { FLT2INT( &Scratch_Int, &(Fl) ); (Res) = (FLT)(Scratch_Int)-(Fl); }

#endif

/******************************************************************/

#endif   // _RASTER_H_
