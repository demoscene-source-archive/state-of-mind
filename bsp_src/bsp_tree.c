/***********************************************
 *      BSP tree1. Handling BSP as arrays.     *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/
    
#include "main3d.h"
#include "bsp.h"

/******************************************************************/

static void BSP_Init_Box( BSP_NODE *Cur )
{
   Cur->Mins[0] = Cur->Mins[1] = Cur->Mins[2] =  _HUGE_;
   Cur->Maxs[0] = Cur->Maxs[1] = Cur->Maxs[2] = -_HUGE_;
}

/******************************************************************/

EXTERN BSP_NODE *BSP_New_Node( BSP_TREE *Tree )
{
   BSP_NODE *Cur;
   if ( Tree->Nb_Nodes==Tree->Max_Nodes )
   {
         // double size
      Cur = (BSP_NODE*)My_Realloc( Tree->Nodes, Tree->Max_Nodes*2*sizeof(BSP_NODE) );
      if ( Cur==NULL ) return( NULL );
      Tree->Nodes = Cur;
      Tree->Max_Nodes *= 2;
   }
   Cur = &Tree->Nodes[Tree->Nb_Nodes];
   Cur->Flags = 0x0000;
   Cur->Up = 0;
   Cur->Right = 0;
   Cur->ID = Tree->Nb_Nodes;
   BSP_Init_Box( Cur );
   Tree->Nb_Nodes++;
   return( Cur );
}

extern BSP_NODE *BSP_Dispose_Node( BSP_TREE *Tree )
{
}

/******************************************************************/

EXTERN BSP_LEAF *BSP_New_Leaf( BSP_TREE *Tree )
{
   BSP_LEAF *New;

   if ( Tree->Free_Leaves == Tree->Max_Leaves )
   {
         // double size
      New = (BSP_LEAF*)My_Realloc( Tree->Leaves, Tree->Max_Leaves*2*sizeof(BSP_LEAF) );
      if ( New==NULL ) return( NULL );
      Tree->Leaves = New;
      Tree->Max_Leaves *= 2;
   }
   New = &Tree->Leaves[Tree->Free_Leaves];
   Tree->Free_Leaves = New->Next;
   New->Next = -1;   // unlink
   New->Obj_ID = -1;
   New->Poly1 = New->Poly2 = 0;
   return( New );
}

extern void BSP_Dispose_Leaf( BSP_TREE *Tree, UINT Leaf )
{
   if ( Tree==NULL ) return;
   Tree->Leaves[Leaf].Next = Tree->Free_Leaves;   
   Tree->Free_Leaves = Leaf;
}

/******************************************************************/

extern void BSP_Optimize_Tree( BSP_TREE *Tree )
{
   void *Ptr;

   if ( Tree==NULL ) return;
   if ( Tree->Max_Nodes>Tree->Nb_Nodes )
   {
      Ptr = My_Realloc( Tree->Nodes, Tree->Nb_Nodes*sizeof(BSP_NODE) );
      if ( Ptr!=NULL ) 
      { 
         Tree->Nodes = (BSP_NODE*)Ptr; 
         Tree->Max_Nodes = Tree->Nb_Nodes;
      }
   }
      // don't touch leaves, for now...
      // it's more difficult than nodes, since
      // nodes are always packed, whereas leaves can
      // have holes in the sequence...
}

/******************************************************************/

EXTERN void BSP_Link_Free_Leaves( BSP_TREE *New )
{
   UINT i;

   if ( New==NULL ) return;
   for( i=New->Free_Leaves; i<New->Max_Leaves-1; ++i )
   {
      New->Leaves[i].Next = i+1;
   }
   New->Leaves[i].Next = -1;   // last
}

EXTERN BSP_TREE *BSP_New_Tree( INT Nb_Nodes, INT Nb_Leaves )
{
   BSP_TREE *New;

   New = New_Fatal_Object( 1, BSP_TREE );
   if ( New==NULL ) return( NULL );
   if ( !Nb_Nodes ) Nb_Nodes = 8;   // minimum
   New->Nodes = New_Fatal_Object( Nb_Nodes, BSP_NODE );
   if ( New->Nodes==NULL ) goto Failed;
   New->Max_Nodes = Nb_Nodes;
   New->Nb_Nodes = 0;

   if ( !Nb_Leaves ) Nb_Leaves = 8; // minimum
   New->Leaves = New_Fatal_Object( Nb_Leaves, BSP_LEAF );
   if ( New->Leaves==NULL ) goto Failed;
   New->Max_Leaves = Nb_Leaves;

      // link leaves as free
   New->Free_Leaves = 0;   // first is free.
   BSP_Link_Free_Leaves( New );

   return( New );

Failed:
   M_Free( New->Nodes );
   M_Free( New->Leaves );
   M_Free( New );
   return( NULL );
}

EXTERN void BSP_Destroy_Tree( BSP_TREE *Tree )
{
   if ( Tree==NULL ) return;
   M_Free( Tree->Nodes );
   M_Free( Tree->Leaves );
   M_Free( Tree );
}

/******************************************************************/

