/***********************************************
 *              render2.h                      *
 * Skal 98                                     *
 ***********************************************/

#ifndef _RENDER2_H_
#define _RENDER2_H_

typedef struct {

   FLT _Lx_, _Ly_, _Cx_, _Cy_;
   FLT Clips[8], Out_Clips[6];

   INT Pix_Width, Pix_Height, Pix_BpS;
   PIXEL *Base_Ptr;

   FLT DxU, DxV, DzU, DxiZ, DxS, DxT;
   FLT DyU, DyV, DzV, DyiZ, DyS, DyT;
   FLT U, dU1, dU2;
   FLT V, dV1, dV2;
   FLT iZ, diZ1, diZ2;
   FLT S, dS1, dS2;
   FLT T, dT1, dT2;

   FLT dy0, dy1, dx0, dx1;
   INT dyS, dyT, dyU, dyV, dyiZ;

   FLT Area, Area2, aZ1, aZ2;

   OBJ_NODE *The_Camera;
   WORLD    *The_World;

   LIGHT          *Cur_Light;
   CAMERA         *Cur_Camera;
   OBJECT         *Cur_Obj;
   OBJ_NODE       *Cur_Node;
   OBJ_NODE      **Cur_Parent;
   MESH           *Cur_Msh;
   SHADER_METHODS *Cur_Shader;
   VISTA          *Cur_Vista;
   FLT             Vista_Time;

   FLT Z_Min, Z_Max, Z_Scale, Z_Off;
   FLT All_Z_Min, All_Z_Max;
   FLT Z_Vista_Scale;
   FLT Global_Z_Scale;
   UINT Frame_Stamp;          // default obj stamp = 0 (<1)

#if 0

      // common data needed by Draw_*() and shaders funcs

   PIXEL *UV_Src, *UV_Src2;
   void *Light_Ramp;
   USHORT Flat_Color, Flat_Color2;
   POLY      *Orig_Poly;
   P_POLY    *Cur_P_Poly;
   P_VERTICE *Cur_P_Vertex;
   PIXEL     *Cur_Vertex_State;
   UINT  Cur_Vtx_Nb;
   UINT  Cur_Poly_Pt;
   UINT  Cur_P_Vertex_Nb;
   TEXTURE_MAP *Cur_Texture;
   PIXEL       *Cur_Texture_Ptr;
   MATERIAL    *Cur_Material;
   INT         Constants_Computed;

   USHORT *Line_16_CMap;

   INT Nb_Edges_To_Deal_With;
   INT Nb_Out_Edges;
   P_VERTICE *Out_Vtx[ MAX_EDGES+1+MAX_ADDITIONAL_VERTICES];
   P_VERTICE *P_Edges[MAX_EDGES+1];
   USHORT Vtx_Flags[ MAX_EDGES+1+MAX_ADDITIONAL_VERTICES];

   FLT Edges_x[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Edges_dx[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
   FLT Edges_y[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Edges_dy[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
   FLT Edges_z[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Edges_dz[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
   FLT Slopes[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Slope_Full[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
   FLT Edges_ey[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES];
   INT Edges_yf[MAX_EDGES+MAX_ADDITIONAL_VERTICES], Edges_yo[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
   INT Edges_Xe[MAX_EDGES+MAX_ADDITIONAL_VERTICES], Edges_dXe[MAX_EDGES+MAX_ADDITIONAL_VERTICES];

   INT Scan_Start, Scan_H;
   INT Nb_Right_Edges;
   USHORT Right_Edges[MAX_EDGES+MAX_ADDITIONAL_VERTICES];

   INT Scan_Pt1[MAX_SCANS];
   INT Scan_Pt2[MAX_SCANS];
   INT Scan_U[MAX_SCANS], Scan_V[MAX_SCANS];
   INT Scan_S[MAX_SCANS], Scan_T[MAX_SCANS];
   FLT Scan_UiZ[MAX_SCANS], Scan_ViZ[MAX_SCANS], Scan_iZ[MAX_SCANS];

//  USHORT XBuffer[ MAX_WIDTH ];

#endif

} RENDER_STATE;

extern RENDER_STATE _RCst_;

/******************************************************************/

#endif   // _RENDER2_H_
