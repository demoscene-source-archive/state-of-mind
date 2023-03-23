/***********************************************
 *               blob.h                        *
 * Skal 97                                     *
 ***********************************************/

#ifndef _BLOB_H_
#define _BLOB_H_

/******************************************************************/

// #define SOLVE_HARD
// #define SOLVE_HARD2
#define K_EPSILON 0.001


#define MAX_CENTERS  8
struct BLOB 
{
#pragma pack(1)

   INT    Nb_Centers;
   MESH  *Components[MAX_CENTERS];  /* Original components */
   MESH  *Blob;                     /* New one */
   INT    Nb_Vertex, Nb_Polys;
   FLT    *Orig_R;

   VECTOR G[MAX_CENTERS];
   VECTOR Gi[MAX_CENTERS];

   FLT    Strength[MAX_CENTERS];
   FLT    Radius[MAX_CENTERS];
   FLT    dKi[MAX_CENTERS];

#ifndef SOLVE_HARD
   FLT    Ai[MAX_CENTERS], Ci[MAX_CENTERS];
   VECTOR Bi[MAX_CENTERS];
#endif

};

/******************************************************************/

extern MESH *Blobify( BLOB *Blob, MESH *Mesh, FLT Radius, FLT Strength );

extern void Setup_Blob_Cst( BLOB *Blob );
extern MESH *Finish_Blob( BLOB *Blob );
extern void Remesh_Blob( BLOB *Blob, FLT K, FLT Blend );

extern void Transform_Vertices_Blob( MATRIX2 M );

/******************************************************************/

#endif   /* _BLOB_H_ */
