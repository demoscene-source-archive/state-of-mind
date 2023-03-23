/***********************************************
 *          Poly handling                      *
 * Skal 99                                     *
 ***********************************************/

#ifndef _POLY_H_
#define _POLY_H_

/******************************************************************/

#define MAX_VERT_2D  8
#define MAX_VERT_3D  8

typedef struct 
{
#pragma pack(1)

    PT_2D  *Index[MAX_VERT_2D+1];
    PT_2D   Pts[MAX_VERT_2D];
} POLY_2D;

typedef FLT VTX_3D[8];        // x,y,z  + u,v + u2,v2 + gouraud

typedef struct 
{
#pragma pack(1)

    USHORT    Nb_Vtx;
    USHORT    Nb_Fields;
    VTX_3D    Vtx[MAX_VERT_3D];
} POLY_3D;

typedef struct SURFACE SURFACE;
struct SURFACE {
#pragma pack(1)

   SURFACE  *Next, *Prev;
   USHORT    State;
   SHORT     xo;
   FLT       w;
   void      (*Draw_Span)( );
   FLT       Area;
   VECTOR    Tdw;
   VECTOR    TdUw;
   VECTOR    TdVw;
   FLT       w0, uw0, vw0;
   UINT      Color;
   void     *Texture;
   FLT       Scale;
   INT       Mip;
};

extern void Project_Poly_2D( POLY_2D *Poly2D, POLY_3D *Poly3D, 
   OBJ_NODE *Cam_Node );
EXTERN void Project_Poly_2D_Direct( POLY_2D *Poly2D, POLY_3D *Poly3D, 
   OBJ_NODE *Cam_Node );
extern INT Poly_Backface( VECTOR Vertex, PLANE plane, VECTOR Where );
extern void Mesh_To_Edges( OBJ_NODE *Mesh, OBJ_NODE *Cam_Node );
extern void Compute_Gradients( SURFACE *Surf, POLY_3D *Poly );
extern void Store_Poly_Gradients( MESH *Msh );

/******************************************************************/

#endif   // _POLY_H_
