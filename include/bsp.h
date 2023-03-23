/***********************************************
 *              BSP                            *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#ifndef _BSP_H_
#define _BSP_H_

// #define DEBUG_BSP

// #include "transf.h"     // for PLANE

/******************************************************************/

typedef struct BSP_NODE BSP_NODE;
typedef struct BSP_LEAF BSP_LEAF;

typedef struct BSP_TREE BSP_TREE;

/******************************************************************/

#define BSP_NO_FLAG  0x0000
#define BSP_HAS_RIGHT  0x0001
#define BSP_HAS_LEFT   0x0002
#define BSP_RIGHT_LEAF 0x0004
#define BSP_LEFT_LEAF  0x0008
#define BSP_IS_LEAF    0x0010

/******************************************************************/

   // struct for tree once linear
   // maximum node+leaf: 65536

struct BSP_NODE
{
#pragma pack(1)

   USHORT Flags;
   USHORT Up;       // 1
   USHORT Right;          // Left is always = 1
   USHORT ID;       // 2
   VECTOR Mins,  Maxs;
   PLANE Plane;     // 12

};

struct BSP_LEAF
{
#pragma pack(1)

   UINT Next;
   UINT Obj_ID;
   UINT Poly1, Poly2; 

};

/******************************************************************/

struct BSP_TREE
{
#pragma pack(1)

   BSP_NODE *Nodes;
   INT Nb_Nodes, Max_Nodes;
   BSP_LEAF *Leaves;
   UINT Free_Leaves, Max_Leaves;

};

/******************************************************************/

      // current values

extern BSP_NODE *BSP_Nodes;
extern BSP_LEAF *BSP_Leaves;
extern BSP_TREE *BSP_Tree;

/******************************************************************/

extern BSP_NODE *BSP_New_Node( BSP_TREE * );
extern BSP_LEAF *BSP_New_Leaf( BSP_TREE * );

extern BSP_NODE *BSP_Dispose_Node( BSP_TREE * );
extern void BSP_Dispose_Leaf( BSP_TREE *Tree, UINT Leaf );

extern void BSP_Optimize_Tree( BSP_TREE * );

extern void BSP_Link_Free_Leaves( BSP_TREE *New );
extern BSP_TREE *BSP_New_Tree( INT Nb_Nodes, INT Nb_Leaves );
extern void BSP_Destroy_Tree( BSP_TREE *Tree );


extern BSP_TREE *BSP_Setup_Tree( WORLD *W );

// extern void BSP_Recurse_Tree( BSP_NODE *Node, void (*Func)( BSP_NODE *) );
// extern void BSP_Sort_Objects( CAMERA *Cam );

#define BSP_MAGIK "&!bsp1"

extern BSP_TREE *BSP_Load( STRING Name );
extern INT BSP_Save( STRING Name, BSP_TREE *New );
extern void BSP_Print_Info( BSP_TREE *Tree );

extern VECTOR BSP_Planes[3];
extern void BSP_Split_Node( BSP_TREE *Tree, INT Node );
extern OBJ_NODE **BSP_Obj_To_Split, **BSP_New_Objs;
extern INT BSP_Node_Start, BSP_Node_End, BSP_Max_Nodes;
extern USHORT BSP_Box_Points_Flags[8];

extern INT BSP_Classify_Point( USHORT Node, VECTOR P, BSP_LEAF **Where );
   
/******************************************************************/
/******************************************************************/

   // parameters for building...

typedef struct {

   INT Max_Try;
   INT Split_Stop_Nb;
   FLT Test_Amp_Base, Test_Amp;

} BSP_CST;

extern BSP_CST Bsp_Params;
extern void BSP_Init_Params( );

/******************************************************************/


#endif   // _BSP_H_

