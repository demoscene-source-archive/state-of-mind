/***********************************************
 *                    ray.h                    *
 * Skal 98                                     *
 ***********************************************/

#ifndef _RAY_H_
#define _RAY_H_

/******************************************************************/

   // used in ray-tracing
struct POLY_RAYTRACE_CST {
#pragma pack(1)

   FLT U2, V2, UV;
   FLT xo, yo;
   VECTOR PoP1, PoP2;
} ;

#define SET_FCOLOR(C,R,G,B,A)   { (C)[0]=(R); (C)[1]=(G); \
   (C)[2]=(B); (C)[3]=(A); }
#define FCOLOR_GRAY(C)  ( ((C)[0]+(C)[1]+(C)[2])/3.0f )
#define Assign_FColor(A,B) { (A)[0]=(B)[0];(A)[1]=(B)[1];(A)[2]=(B)[2];(A)[3]=(B)[3]; }
#define Add_FColor_Eq(A,B) { (A)[0]+=(B)[0]; (A)[1]+=(B)[1];(A)[2]+=(B)[2];(A)[3]+=(B)[3]; }
#define Add_Scaled_FColor_Eq(A,B,C) { (A)[0]+=(C)*(B)[0]; (A)[1]+=(C)*(B)[1];(A)[2]+=(C)*(B)[2];(A)[3]+=(C)*(B)[3]; }
#define Scale_FColor_Eq(A,B) { (A)[0]*=(B);(A)[1]*=(B);(A)[2]*=(B);(A)[3]*=(B); }
#define Scale_FColor(A,B,C) { (A)[0]=(B)[0]*(C);(A)[1]=(B)[1]*(C);(A)[2]=(B)[2]*(C);(A)[3]=(B)[3]*(C); }
#define Clamp_FColor(A,x)  { if( (A)[0]>(x) ) (A)[0]=(x); if( (A)[1]>(x) ) (A)[1]=(x); if( (A)[2]>(x) ) (A)[2]=(x); }
#define Clamp_All_FColor(A)  {            \
   if( (A)[0]>1.0f ) (A)[0]=1.0f; else if ((A)[0]<0.0) (A)[0]=0.0;     \
   if( (A)[1]>1.0f ) (A)[1]=1.0f; else if ((A)[1]<0.0) (A)[1]=0.0;     \
   if( (A)[2]>1.0f ) (A)[2]=1.0f; else if ((A)[2]<0.0) (A)[2]=0.0;     \
}

extern void Set_Ray_Tracing_Constants( OBJ_NODE *Obj );
extern INT Compute_Raytrace_Poly_Constants( OBJ_NODE *Node );

/******************************************************************/

      // info about the raytracer

typedef struct {
#pragma pack(1)

   BYTE Do_Light, Do_Radiosity;
   BYTE Do_UV_Antialias;
   OBJ_NODE *Light_Node;
   FLT Antialias;
   FLT Light_Extend, Light_Jitter, Light_Error_Thresh;
   INT Max_Trace_Level, Cur_Trace_Level;
   FLT Ambient, Cst_Ambient, Diffuse, Radiosity;
   FLT Reflect, Transmit;
   FLT Light_Fall;
   UINT Filter_Flags;
   UINT Backface_Cull;
} RAYTRACER_INFO;

extern RAYTRACER_INFO Ray_Info;
extern RAYTRACER_INFO *Init_Raytracer( );

/******************************************************************/

#define RAY_HAS_PT     0x0001
#define RAY_HAS_NORMAL 0x0002
#define RAY_HAS_UV     0x0004
#define RAY_HAS_COLOR  0x0008

struct RAY {
#pragma pack(1)

   VECTOR Orig;      //  3
   VECTOR Dir;       // +3
   FCOLOR Col;       // +4
   FLT    Z;         // +1
   FLT    Z_Min;     // +1
   FLT    Z_Unit;    // +1

         // Intersection info
   INT    Flags;
   INT    Id;
   VECTOR Pt;
   VECTOR Normal;
   OBJ_NODE *Node;
   FLT    x, y;      // for mapping
};

#define _RAY_HUGE_    (1.0e10f)
#define _RAY_EPSILON_ (1.0e-5f)
#define _DEPTH_EPSILON_ (1.0e-4f)

extern INT Intersect_Node( RAY *Ray, OBJ_NODE *Node );
extern void Get_Mesh_Color( RAY *Ray, OBJ_NODE *Node, FCOLOR Col );
extern void Get_Mesh_Normal( RAY *Ray, OBJ_NODE *Node, VECTOR Normal );


   // Ray->Col[] needs to be set to atmospheric one
#define INIT_RAY(R) { (R)->Z=_RAY_HUGE_; (R)->Z_Min=0.0; (R)->Id=-1; (R)->Node=NULL; (R)->Flags=0x0000;  }
#define UPDATE_RAY(R,N) { (R)->Z=(N)->Z; (R)->Id=(N)->Id; \
   (R)->Node=(N)->Node; (R)->Normal[0]=(N)->Normal[0];   \
   (R)->Normal[1]=(N)->Normal[1]; (R)->Normal[2]=(N)->Normal[2];  \
   (R)->Pt[0]=(N)->Pt[0]; (R)->Pt[1]=(N)->Pt[1]; (R)->Pt[2]=(N)->Pt[2];  \
   (R)->x=(N)->x; (R)->y=(N)->y; (R)->Flags=(N)->Flags; }

extern void Init_Camera_Ray( RAY *Ray, FLT u, FLT v );
extern void Ray_Check_Intersection( RAY *Ray );
extern void Compute_Reflected_Ray( VECTOR Result, RAY *Ray );
   
extern void Local_Ray( RAY *Out, RAY *In, MATRIX2 M );
extern void Choose_Ortho_Basis( VECTOR N, VECTOR n2, VECTOR n3 );
   
         // Main recursive entries

extern INT Intersect_All_Nodes( RAY *Ray, OBJ_NODE *Node );
extern INT Intersect_All_Nodes_Except_Cached( RAY *Ray,
   OBJ_NODE *Node, OBJ_NODE *First );
extern INT Intersect_All_Nodes_Cached( RAY *Ray,
   OBJ_NODE *Node, OBJ_NODE *First );
extern INT Trace_Ray( RAY *Ray, OBJ_NODE *Node );
extern void Compute_Intersection_Color( RAY *Ray );

extern INT Compute_Reflected_Color( RAY *Ray, FCOLOR Result );

/******************************************************************/
/******************************************************************/

struct SLAB {
#pragma pack(1)

   VECTOR N;
   FLT d;
};

struct SLABS {
#pragma pack(1)

   INT Nb;
   SLAB *Slabs;
};

#define _SLAB_EPSILON_ (1.0e-6f)
#define _SLAB_HUGE_    (1.0e10f)

extern INT Intersect_Slabs( RAY *Ray, SLABS *Slabs, FLT *Min, FLT *Max );
extern INT Intersect_Box( RAY *Ray, BBOX *BBox, FLT *Min, FLT *Max );

      // Intersection methods for node

extern INT Intersect_Group_Union( RAY *Ray, OBJ_NODE *Node );
extern INT Intersect_Mesh( RAY *Ray, OBJ_NODE *Node );


/******************************************************************/

      // in rlight.c

#define _LIGHT_EPSILON_ (1.0e-6f)
extern void Get_Light_Ray_Color( RAY *Ray, OBJ_NODE *Node, FCOLOR Col );
extern INT Ray_Trace_Lights( RAY *Ray, FCOLOR Result );
extern INT Ray_Trace_One_Light( RAY *Ray, FCOLOR Result, OBJ_NODE *L );
extern INT Compute_Light_Diffuse( RAY *Ray, FCOLOR Light_Col );
         
/******************************************************************/

   // in rmat.c

struct RAY_MATERIAL {
#pragma pack(1)

   MATERIAL *Source;
   FLT Uo_Mip, Vo_Mip;
   FLT Mip_Scale;
   TEXTURE_MAP *Cur_Texture;
   PIXEL *Cur_Texture_Ptr;

   PIXEL *CMap;
   INT Nb_Col;

   FLT R, T, IOR;
   FCOLOR Ambient, Diffuse, Specular;
};


#define MAX_RAY_MATERIALS 5
extern INT Nb_Texture_Layers;
extern RAY_MATERIAL Ray_Materials[], *Cur_Ray_Mat;
extern POLY *Cur_Ray_Poly;

extern void Get_Mesh_Ray_Materials( RAY *Ray, OBJ_NODE *Node );

/******************************************************************/
/******************************************************************/

      // octree.c

typedef struct OCTREE_DATA OCTREE_DATA;
typedef struct OCTREE_NODE OCTREE_NODE;
typedef struct OCTREE_ID OCTREE_ID;
typedef struct OCTREE OCTREE;

/******************************************************************/

      // every type of data one want to store in octree.
      // must start with *Next field so cast are ok
      // while dealing generally with them

typedef struct OCTREE_DUMMY_DATA OCTREE_DUMMY_DATA;

struct OCTREE_DUMMY_DATA {
#pragma pack(1)

   OCTREE_DUMMY_DATA *Next;
};

/******************************************************************/

struct OCTREE_ID {
#pragma pack(1)

   INT x,y,z;
   INT Size;
};

struct OCTREE_NODE {
#pragma pack(1)

   OCTREE_ID Id;
   OCTREE_DUMMY_DATA *Data;
   OCTREE_NODE *Children[8];
};

/******************************************************************/

      // struct to gather info about 
struct  OCTREE {
#pragma pack(1)

   OCTREE_NODE *Root;
   VECTOR Center, Scale;
   OCTREE_NODE *Last_Hit;
   INT Nb_Nodes, Nb_Data;

         // specific methods for creation/handling of data
   INT Data_Size;          // sizeof( OCTREE_DATA )
   void *(*Init_Data_Block)( );
   void (*Destroy_Data_Block)(void *);
};

/******************************************************************/

extern OCTREE *Cur_Octree;

extern void Print_Octree_Node( OCTREE_NODE *Node, INT Level );

extern OCTREE *Create_Base_Octree( INT Data_Size );
extern void Destroy_Octree_Node( OCTREE *Octree, OCTREE_NODE **Node );
extern void Destroy_Octree( OCTREE *Octree );
extern OCTREE_NODE *Octree_New_Node( OCTREE *Octree );
extern void Octree_Check_Parent( OCTREE_ID *Father, OCTREE_ID *Child );
extern void Octree_Insert( OCTREE_NODE **Root, OCTREE_DUMMY_DATA *Block,
   OCTREE_ID *New_Id );
extern void Octree_New_Root( OCTREE_NODE **Root );
extern void Octree_Find_Box( VECTOR Min, VECTOR Max, OCTREE_ID *Id );
extern void Octree_Find_Sphere( VECTOR C, FLT R, OCTREE_ID *Id );

extern INT Octree_Point_In_Node( VECTOR Pt, OCTREE_ID *Id ); 
extern INT Octree_Traverse( OCTREE_NODE *Node,
   INT (*Func)( OCTREE_DUMMY_DATA *, void *Handle1 ),
      void *Handle );


/******************************************************************
 *                   Specially for Radiosity                      *
 ******************************************************************/

struct OCTREE_DATA {
#pragma pack(1)

   OCTREE_DATA *Next;
   INT Bounce_Level;
   VECTOR Pt, N;
   FCOLOR Illuminance;
   FLT drdx, dgdx, dbdx,  drdy, dgdy, dbdy,  drdz, dgdz, dbdz;
   FLT  Harmonic_Mean_Distance, Nearest_Distance;
   VECTOR To_Nearest_Surface;
};

typedef struct {
#pragma pack(1)

   OCTREE *Octree;
   WORLD *World;

   FLT Error_Bound, Cur_Error_Bound;
   FLT Min_ReUse, Max_Dist;
   INT Max_Count, Nearest_Count;
   FLT Gray_Level, Brightness;
   INT Max_Radiosity_Level, Cur_Radiosity_Level;
   BYTE Show_Rad_Smp;
} RAD_INFO;

      // specific calls for radiosity...

extern INT Octree_Distance_Traverse( OCTREE_NODE *Node,
   VECTOR Point, INT Bounce,
      INT (*Func)(  OCTREE_DATA *, void *Handle1 ),
         void *Handle );
extern OCTREE *Create_Radiosity_Octree( );

      // in rad.c

extern INT Compute_Ambient_Radiosity( VECTOR Orig, VECTOR Normal, FCOLOR
   Result );

extern RAD_INFO *Init_Radiosity( );
extern INT Close_Radiosity( );

      // in sh_ray

typedef struct {
#pragma pack(1)

   USHORT U, V;
   USHORT Z;
   SHORT Nx, Ny, Nz;
   USHORT Node_Id, P_Id;
} RAY_CELL;

extern SHADER_METHODS Ray_Shader;

/******************************************************************/
/******************************************************************/

#endif   // _RAY_H_
