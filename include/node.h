/***********************************************
 *                  node.h                     *
 * Skal 98                                     *
 ***********************************************/

#ifndef _NODE_H_
#define _NODE_H_

/******************************************************************/

struct NODE_METHODS {
#pragma pack(1)

   INT     Data_Size;
   void    (*Init)( OBJECT *Obj );
   INT     (*Finish)( OBJ_NODE * );
   void    (*Destroy)( OBJECT * );
   void    (*Transform)( OBJ_NODE * );
   void    (*Anim)( OBJ_NODE *, FLT );
   void    (*Render)( OBJ_NODE *, MATRIX2 );
   INT     (*Post)( OBJ_NODE * );
   OBJECT *(*Copy)( OBJECT *New, OBJECT *Old );
   INT     (*Intersect)( RAY *Ray, OBJ_NODE *New );
   void    (*Get_Color)( RAY *Ray, OBJ_NODE *Node, FCOLOR Col );
   void    (*Get_Normal)( RAY *Ray, OBJ_NODE *Node, VECTOR Normal );
   void    (*Get_Materials)( RAY *Ray, OBJ_NODE *Node );
   INT     (*Init_Ray_Constants)( OBJ_NODE *Node );

};

      // Shortcuts
#define NODE_ANIM(N,T)  { if ((N)->Methods->Anim!=NULL) (*(N)->Methods->Anim)((N),(T) ); }
#define NODE_TRANSFORM(N)  { if ((N)->Methods->Transform!=NULL) (*(N)->Methods->Transform)((N)); }
#define NODE_COPY(N,O)  ( (*(O)->Methods->Copy)( (N)->Data, (O)->Data ) )
#define NODE_FINISH(N)  ( ((N)->Methods->Finish==NULL) ? 0 : (*(N)->Methods->Finish)((N)) )
#define NODE_RENDER(N,M)   ( (*(N)->Methods->Render)( (N), (M) ) )
#define NODE_INTERSECT(R,N)   { if ( (N)->Methods->Intersect!=NULL ) (*(N)->Methods->Intersect)( (R), (N) ); }
#define RAY_GET_COLOR(R,F)   { if ( (R)->Node!=NULL && (R)->Node->Methods->Get_Color!=NULL ) (*((R)->Node->Methods->Get_Color))( (R), (R)->Node, (F) ); }
#define NODE_GET_COLOR(N,F)   { if ( (N)!=NULL && (N)->Methods->Get_Color!=NULL ) (*((N)->Methods->Get_Color))( NULL, (N), (F) ); }
#define NODE_GET_NORMAL(R,N)   { if ( (R)->Node!=NULL && (R)->Node->Methods->Get_Normal!=NULL ) (*((R)->Node->Methods->Get_Normal))( (R), (R)->Node, (N) ); }
#define NODE_GET_MATERIALS(R,N)   { if ( (N)!=NULL && (N)->Methods->Get_Materials!=NULL ) (*((N)->Methods->Get_Materials))( (R),(N) ); }
#define NODE_INIT_RAY_CONSTANTS(N)   { if ( (N)!=NULL && (N)->Methods->Init_Ray_Constants!=NULL ) (*((N)->Methods->Init_Ray_Constants))( (N) ); }


struct OBJ_NODE {
#pragma pack(1)

   OBJ_TYPE Type;
   OBJ_NODE *Prev, *Next;
   OBJ_NODE *Parent, *Child;
   OBJ_NODE **Top;

   UINT Flags;
   UINT ID;

   NODE_METHODS *Methods;

   TRANSFORM  Transform;
   MATRIX2 Mo, M;          // Mo = Original transform. M=with current Camera
   USHORT Camera_ID;
   BBOX Box;
   UINT Time_Stamp;
   FLT  Cur_Time;
   OBJECT *Data;
   STRING Name;
};

#define STORE_TRANSFORM_MATRIX(Node)      \
   if ( (Node)->Time_Stamp<_RCst_.Frame_Stamp )  \
   { Transform_To_Matrix( &(Node)->Transform, (Node)->Mo ); \
     Node->Time_Stamp = _RCst_.Frame_Stamp; }

#define STORE_NODE_MATRIX(Node,Cam)      \
   { Mult_Matrix_ABC( (Cam)->Mo, (Node)->Mo, (Node)->M );   \
     (Node)->Camera_ID = (USHORT)(Cam)->ID; }

#define CHECK_NODE_MATRIX(Node,Cam) \
   { if ( (Node)->Camera_ID != (Cam)->ID )  STORE_NODE_MATRIX((Node),(Cam) ); }

/******************************************************************/
/******************************************************************/

      // in node.c

extern INT Obj_Node_ID;

extern OBJ_NODE *New_Node( );
extern OBJ_NODE *Nodify( OBJECT *Data, NODE_METHODS *Methods );

extern OBJ_NODE *Remove_Node( OBJ_NODE * );
extern void Destroy_Node( OBJ_NODE * );
extern void Destroy_Nodes( OBJ_NODE *Cur );

extern void Destroy_Objects( OBJECT *Cur, void (*Destroy)(OBJECT *) );
extern void Destroy_Object( OBJECT *Data, void (*Destroy)(OBJECT *) );
extern OBJECT *Insert_Object( OBJECT *Obj, OBJECT **Root );
extern OBJ_NODE *Insert_Node( OBJ_NODE *Node, OBJ_NODE **Head, OBJECT **Root );
extern OBJ_NODE *Insert_Child_Node( OBJ_NODE *Node, OBJ_NODE *Parent, OBJECT **Root );

extern void Default_Obj_Fields( OBJECT *Obj );
extern OBJECT *New_Node_Data( NODE_METHODS *Methods );
extern OBJ_NODE *Attach_Node_Data( OBJ_NODE *Node, OBJECT *Data, NODE_METHODS *Methods );
extern OBJ_NODE *Instantiate( NODE_METHODS * );
extern OBJ_NODE *Copy_Node( OBJ_NODE *Node );

/******************************************************************/
/******************************************************************/

      // in tree.c

extern INT Count_Objects( OBJECT *Cur );
extern INT Count_Nodes( OBJ_NODE *Cur, INT *Nb, INT *P, INT *E );
extern INT Finish_Objects( OBJ_NODE *Cur );
extern INT Finish_Object_Data( OBJECT *Data, INT (*Finish)(OBJECT *) );
extern INT Finish_All_Data( OBJECT *Data, INT (*Finish)(OBJECT *) );

extern void Traverse_Nodes( OBJ_NODE *Cur, void (*Func)( OBJ_NODE *) );
extern void Traverse_All_Nodes( OBJ_NODE *Cur, void (*Func)( OBJ_NODE *) );
extern void Traverse_Typed_Nodes( OBJ_NODE *Cur, OBJ_TYPE Type,
   void (*Func)( OBJ_NODE *) );
extern void Traverse_Objects( OBJECT *Cur, void (*Func)( OBJECT *) );
extern void Traverse_Nodes_With_Transform( OBJ_NODE *Cur, 
   MATRIX2 Base_Matrix, void (*Func)( OBJ_NODE *, MATRIX2 ) );
extern void Check_Node_Transform( OBJ_NODE *Cur );

extern OBJECT *Search_Object_From_Name( OBJECT *Cur, STRING Name );
extern OBJECT *Search_Object_From_ID( OBJECT *Cur, UINT ID );

extern OBJ_NODE *Search_Node_From_Name( OBJ_NODE *Cur, STRING Name );
extern OBJ_NODE *Search_Node_From_ID( OBJ_NODE *Cur, UINT ID );

extern OBJ_NODE *Search_Node_From_Type( OBJ_NODE *Cur, OBJ_TYPE Type );

/******************************************************************/

#endif   // _NODE_H_
