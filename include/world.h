/***********************************************
 *              world.h                        *
 * Skal 96                                     *
 ***********************************************/

#ifndef _WORLD_H_
#define _WORLD_H_

/******************************************************************/

typedef struct {
#pragma pack(1)

   INT Nb_Obj, Nb_Nodes;
   INT Nb_Polys, Nb_Vertex, Nb_Edges;
// INT Nb_Mat, Nb_Txt;

} WORLD_RSC;

typedef struct {         // keeps count of allocated resources
#pragma pack(1)

   INT Sorted_Sz, Poly_Keys_Sz; 
   INT Obj_To_Sort_Sz;
   INT Vertex_Sz, Poly_To_Sort_Sz;
   INT P_Vertex_Sz, Vertex_State_Sz;

} WORLD_REAL_RSC;

   // in wrld_rsc.c

extern void World_Clean_Resources( );
extern void *World_Check_Resources( WORLD_REAL_RSC *New );

         // various array of resources...

extern P_POLY    *Poly_To_Sort;
extern UINT      *Poly_Keys;
extern P_VERTICE *P_Vertex;
extern PIXEL     *Vertex_State;
extern void     **Obj_To_Sort;
extern UINT      *Sorted;

/******************************************************************/

struct WORLD {
#pragma pack(1)

   OBJ_NODE *Root;
   OBJ_NODE **Node_Ptr;

   OBJECT   *Objects;
   OBJECT   *Cameras;
   OBJECT   *Lights;
   OBJECT   *Planes;
   OBJECT   *Materials;

   FLT   Time;
   FLT   Frames_Start, Frames_End;
   void *Cache;

   UINT State;
   INT Max_Obj, Max_Nodes;
   INT Max_Polys, Max_Vertex, Max_Edges;

   WORLD_RSC   Rsc;

   void *Octree;
   void *Bsp;

};

/******************************************************************/
/******************************************************************/

extern WORLD *Create_World( );
extern void   Destroy_World( WORLD *W );
extern void   Clear_World( WORLD *W );
extern void   Finish_World_Init( WORLD *W );
extern void   Set_Up_World( WORLD *W );
extern void   Select_World( WORLD *W );

extern OBJ_NODE **Init_Node_Ptr( WORLD *W );

extern OBJ_NODE *New_Object_By_Type( 
   STRING Name, NODE_METHODS *Methods, WORLD *W );

   // in wld_info.c
extern void Print_World_Info( WORLD *W );

/******************************************************************/

extern void Enter_Engine_Asm( );
extern void Exit_Engine_Asm( );

      // in render.c
extern void Render_World( WORLD *W, OBJ_NODE *Camera );
extern void Init_Render(  WORLD *W, OBJ_NODE *Camera );
extern void Un_Init_Renderer( );

/******************************************************************/
/******************************************************************/

#define WORLD_SET_FRAME(W,x)  (W)->Time = (W)->Frames_Start + (x)*( (W)->Frames_End - (W)->Frames_Start )

/******************************************************************/

extern INT      Nb_Poly_Sorted;
extern INT      Total_Poly_Sorted;
extern UINT     Nb_Obj_To_Sort;
extern INT      Nb_Sorted;


   /* this should be put elsewhere ( in raster.h/.c ) */

extern LIGHT          *Cur_Light;
extern CAMERA         *Cur_Camera;
extern OBJECT         *Cur_Obj;
extern OBJ_NODE       *Cur_Node;
// extern OBJ_NODE      **Cur_Parent;
extern MESH           *Cur_Msh;
extern SHADER_METHODS *Cur_Shader;
extern VISTA          *Cur_Vista;
extern FLT             Vista_Time;

/******************************************************************/
/******************************************************************/

#endif   // _WORLD_H_
