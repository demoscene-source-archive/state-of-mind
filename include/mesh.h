/***********************************************
 *               mesh.h                        *
 * Skal 97                                     *
 ***********************************************/

#ifndef _MESH_H_
#define _MESH_H_

/******************************************************************/

#define POLY_NO_FLAG    0x00
#define POLY_MAP_UV     0x01
#define POLY_U_WRAP     0x02
#define POLY_V_WRAP     0x04
#define POLY_CA_OK      0x08
#define POLY_BC_OK      0x10
#define POLY_AB_OK      0x20
#define POLY_RAYTRACE   0x40
#define POLY_SMOOTH     0x80

#define EDGE_TYPE_NORMAL 0x00
#define EDGE_TYPE_FLAT   0x01
#define EDGE_TYPE_SIL    0x02
#define EDGE_TYPE_SHARP  0x04
#define EDGE_BORDER_EDGE 0x08

#define EDGE_STATE_VOID  0x00       // not really useful
#define EDGE_STATE_DRAWN 0x01
#define EDGE_STATE_SEEN  0x02

#define EDGE_P1_BACKCULL 0x04
#define EDGE_P2_BACKCULL 0x08
#define EDGE_P1_SHADOWED 0x10
#define EDGE_P2_SHADOWED 0x20
#define EDGE_POS_SHADOW  0x40
// #define EDGE_OF_P1       0x80
// #define EDGE_OF_P2       0x00

#define EDGE_REGION_N1   0x10    // alternate flags, in vshadow5.c
#define EDGE_REGION_N2   0x20

#define EDGE_FLATNESS   0.95f
#define EDGE_SHARPNESS  0.03f
#define OPTIM_THRESH    0.05f
#define SPLIT_THRESH1   0.2f         // poly size thresh before split
#define SPLIT_THRESH2   0.7f         // aspect ratio requested before split


#define UV_EPSILON      0.001f

// #define MAX_POLY_PTS 4

struct POLY {
#pragma pack(1)

   USHORT ID;        // 0
   USHORT Flags;     // 2
   USHORT Colors;    // 4        // Texture number for poly, or flat Color over 16bits
   USHORT Pt[MAX_POLY_PTS]; // 6
   PIXEL  Nb_Pts;    // 14
   PIXEL  Clipped;   // 15
   VECTOR No;        // 16
   void  *Ptr;       // 28       // Material name/ptr
   USHORT Edges[MAX_POLY_PTS]; // 32
   FLT    U1V2_U2V1; // 40
   FLT    N_Dot_C;   // 44
   I_MAPPING UV[4];  // 48
                     // 64
};

struct MSH_EDGE {
#pragma pack(1)

   USHORT Start, End;      // 4 bytes
   USHORT Pt_P1, Pt_P2;    // 8
   USHORT Poly1, Poly2;    // 12
   SHORT Sharpness;        // 14
   PIXEL State, Type;      // 16
};

struct MESH {
#pragma pack(1)

   OBJECT_FIELDS

   VECTOR     *Vertex;
   VECTOR     *Normals;
   F_MAPPING  *UV;
   POLY       *Polys;
   USHORT     *Shadow_ID;
   INT         Nb_Vertex;
   INT         Nb_Polys;
   MSH_EDGE   *Edges;
   INT Nb_Edges;

   SHADER_METHODS *Shader;
   SPLINE_3DS *Pos_k, *Rot_k, *Scale_k, *Morph_k, *Hide_k;

   OBJ_NODE **Morph_Node;
   VECTOR *Vertex_Copy;
   VECTOR *Normals_Copy;
   USHORT Morph_Flags;

   POLY_RAYTRACE_CST *Cst;     // constants for ray-tracing the mesh's polys

   VECTOR *dUw;
   VECTOR *dVw;
   VECTOR *dw;
   FLT    *Area;
};

#define MESH_DO_MORPH             0x0001
#define MESH_MORPH_NORMAL         0x0002
#define MESH_MORPH_AVERAGE_NORMAL 0x0004
#define MESH_MORPH_SHARP_EDGES    0x0008
#define MESH_MORPH_DEFAULT_FLAGS  (MESH_DO_MORPH|MESH_MORPH_NORMAL|MESH_MORPH_SHARP_EDGES)

/******************************************************************/

extern BYTE Zo_Clipped, Z1_Clipped;
extern BYTE Xo_Clipped, X1_Clipped;
extern BYTE Yo_Clipped, Y1_Clipped;

extern NODE_METHODS _MESH_;

extern POLY *Mesh_New_Polys( MESH *Obj, INT Nb_Polys );
extern VECTOR *Mesh_New_Vertex( MESH *Obj, INT Nb_Vtx );
extern MESH *Set_Mesh_Data( MESH *, INT Nb_Vtx, INT Nb_Polys );

extern void Mesh_Store_Normals( MESH * );
extern void Mesh_Store_Normals2( MESH *M );
extern void Average_Normals( MESH *M );

extern void Mesh_Map_Spherical( OBJ_NODE *Node );
extern INT Find_Max_Vertex( OBJECT *Cur );
extern void Mesh_Reset_Poly_Material( OBJ_NODE *Node );  // to traverse nodes with...

extern OBJECT *Copy_Mesh( OBJECT *M );

extern void Sub_Vertex_Eq( MESH *M, VECTOR T );
extern void Add_Vertex_Eq( MESH *M, VECTOR T );
extern void Matrix_Vertex_Eq( MESH *M, MATRIX T );

// extern INT Do_Project_Vertex( P_VERTICE *P );
extern void Transform_Vertices_I( MATRIX2 M );
extern void Transform_Vertices_No_Cull( MATRIX2 M );

   // in vmesh.c
extern void Vista_Select_Poly( INT Order );

   // in optimesh.c

extern MSH_EDGE *Mesh_Store_Edges( MESH *M );
extern void Optimize_Split_Mesh( MESH *M, FLT Thresh1, FLT Thresh2 );
extern void Mesh_Detect_Sharp_Edges( MESH *M, FLT Thresh );
extern void Mesh_Store_UV_Mapping( MESH *M );
extern void Optimize_Mesh( MESH *M, FLT Thresh );
extern void Do_Mesh_Store_Edges( MESH *M );

   // in optim2.c

extern void Setup_World_Light( WORLD *W );
extern void Optimize_Mesh_Vertex( OBJ_NODE *Cur );
extern void Optimize_All_Meshes( WORLD *W );

/******************************************************************/

#endif   // _MESH_H_
