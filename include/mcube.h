/***********************************************
 *    Marching cubes (for blobs)               *
 * Skal 97                                     *
 ***********************************************/

#ifndef _MCUBE_H_
#define _MCUBE_H_

// #define USE_MCUBE2
#define USE_MCUBE3
// #define USE_MCUBE4

/******************************************************************/

#ifndef UNIX

#define GROW_NEIGHBOURS_ASM
#define EMIT_MC_POLYS2_ASM
#define EMIT_MC_POLYS3_ASM
#define COMPUTE_MC_NORMAL_ASM
#define DO_V_ASM
#define DO_V2_ASM
// #define COMPUTE_MC_NORMAL3_ASM

   // old
#define EMIT_MC_POLYS_ASM
#define FILL_SLICE3_ASM
#define EMIT_ALL_MC_ASM

#endif      // UNIX

/******************************************************************/

/* #define MC_SAFE_BOUNDS */

#define MCX 32
#define MCY 32
#define MCZ 32

extern FLT MC_SCALE_X, MC_SCALE_Y, MC_SCALE_Z;
extern FLT I_MC_SCALE_X, I_MC_SCALE_Y, I_MC_SCALE_Z;
extern FLT MC_OFF_X, MC_OFF_Y, MC_OFF_Z;
extern FLT MC_OFF_END_X, MC_OFF_END_Y, MC_OFF_END_Z;

#define MAX_ISO_MESH 4
struct MC_BLOB 
{
#pragma pack(1)

   INT    Nb_Centers;
   MESH  *Blob;               // final mesh

   FLT K;

   VECTOR G[MAX_CENTERS];

   FLT    Strength[MAX_CENTERS];
   FLT    Radius[MAX_CENTERS];
   FLT    Inv_Radius2[MAX_CENTERS];
   FLT    Radius2[MAX_CENTERS];

   INT     Nb_Iso_Mesh;
   FLT    *Iso_Mesh[MAX_ISO_MESH];
   FLT    *Ho[MAX_ISO_MESH];
   VECTOR *No[MAX_ISO_MESH];
   USHORT *Prox[MAX_ISO_MESH];
   FLT     Blend0, Blend1;
   FLT     Tension;

};

/******************************************************************/

extern USHORT X_Edge[MCX*MCY];
extern USHORT Y_Edge[MCX*MCY];
extern USHORT Z_Edge[MCY];
extern USHORT Top_yEdge[MCY];
extern USHORT Vtx_6, Vtx_10;

extern FLT *Slice_A, *Slice_B;
extern PIXEL *Flags_A, *Flags_B;
extern FLT Raw_Slice[2*MCX*MCY];
extern PIXEL Raw_Flags[2*MCX*MCY];
extern UINT MC_Last_Flags[ 2*MCX*MCY ];  // 1 bit per z-slice
extern UINT *Flags1, *Flags2;
extern UINT MC_Mask;
extern UINT Ngh_Tab[256];   

extern FLT *Iso_Z;

extern VECTOR Slice;
extern MESH *MC_Blob;
extern MC_BLOB *O_Blob;

/******************************************************************/

      // in mcube.c / mcube_a.c / mcube_a.asm
      // old.

extern void Emit_All_MC( MC_BLOB *Blob );
extern void Emit_MC_Polys( INT Off, INT Index, INT io, INT jo );
extern void Fill_Slice3( FLT *Ptr, PIXEL *Flags, MC_BLOB *Blob );
extern void Clear_Slice_A( FLT K );

extern void Do_MCube( MC_BLOB *Blob );
extern void Do_MCube2( MC_BLOB *Blob, FLT *Iso_M, FLT Blend );

extern void MC_Blobify( MC_BLOB *Blob, FLT Radius, FLT Strength );
extern MESH *MC_Finish_Blob( MC_BLOB *Blob, INT Max_Vtx, INT Max_Polys );
extern FLT *Mesh_To_Iso( MC_BLOB *Blob, OBJ_NODE *Node );
extern FLT *Install_Iso_Mesh( MC_BLOB *Blob, OBJ_NODE *Node, INT Slot );

/******************************************************************/

      // in mcube3.c or mcube_a3.asm

extern INT (*_Do_V_)( INT );
extern void (*_Compute_MC_Normal_)( INT );

extern void MC_Grow_Neighbours( );
extern void Emit_MC_Polys3( INT Off, INT Index, INT jo );
extern INT Do_V( INT );
extern void Compute_MC_Normal( INT );
extern INT Do_V2( INT );
extern void Compute_MC_Normal2( INT );
extern void Do_MCube3( MC_BLOB *Blob );
extern void Do_MCube4( MC_BLOB *Blob );
extern void MC_Blobify2( MC_BLOB *Blob, FLT Radius, FLT Strength );
extern MESH *MC_Finish_Blob2( MC_BLOB *Blob, INT Max_Vtx, INT Max_Polys );
extern void MC_Set_Constants2( MC_BLOB *Blob );

extern void Build_Ngh_Table( );
extern void Build_Default_Blob_Table( );
extern void Build_MCube_Tab( );

extern void Set_MC_Dimensions( FLT Xo, FLT Yo, FLT Zo, FLT Dx, FLT Dy, FLT Dz );

/******************************************************************/

#define MC_PTS 32
extern PIXEL Poly_Order[256][MC_PTS];
extern PIXEL Edge_Order[256][16];

#define BLOB_TABLE_SIZE 256
extern FLT Blob_Table[ BLOB_TABLE_SIZE ];
extern FLT Blob_Table2[ BLOB_TABLE_SIZE ];

/******************************************************************/
/******************************************************************/

extern void Do_MCube_With_Edges( MC_BLOB *Blob );

/******************************************************************/
/******************************************************************/

#endif   // _MCUBE_H_
