/***********************************************
 *              BSP                            *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#include "main3d.h"
#include "bsp.h"

EXTERN BSP_NODE *BSP_Nodes = NULL;
EXTERN BSP_LEAF *BSP_Leaves = NULL;
EXTERN BSP_TREE *BSP_Tree = NULL;

/******************************************************************/

static INT BSP_Recurse( USHORT Node, BSP_LEAF **Where )
{
}

EXTERN INT BSP_Classify_Point( USHORT Node, 
   VECTOR P, BSP_LEAF **Where )
{
   BSP_NODE *BNode;
   INT n = 0;
   USHORT Flag, Left, Right;
   FLT Dot;

   BNode = &BSP_Tree->Nodes[Node];
   if ( BNode->Flags&BSP_IS_LEAF )
   {
      *Where = &BSP_Leaves[BNode->Right];
      return( 1 );
   }

   Left = Right = 0;   // empty, a priori

   if ( BNode->Flags & BSP_HAS_RIGHT ) Right = Node + BNode->Right;
   else if ( BNode->Flags & BSP_RIGHT_LEAF ) Right = Node + BNode->Right;

   if ( BNode->Flags & BSP_HAS_LEFT ) Left = Node + 1;
   else if ( BNode->Flags & BSP_LEFT_LEAF ) Left = Node + 1;

   n = 0;
   Dot = Dot_Product( P, BNode->Plane );
   if ( Dot<BNode->Plane[3] )    //  BSP_RIGHT_LEAF
   {
      if ( Right )
      {
         if ( BNode->Flags & BSP_HAS_RIGHT ) n += BSP_Classify_Point( Right, P, Where+n );
         else Where[n++] = &BSP_Leaves[Right]; // store right leaf
      }
      if ( Left ) 
      {
         if ( BNode->Flags & BSP_HAS_LEFT ) n += BSP_Classify_Point( Left, P, Where+n );
         else Where[n++] = &BSP_Leaves[Left];  // store left leaf
      }
   }
   else                             // BSP_LEFT_LEAF
   {
      if ( Left ) 
      {
         if ( BNode->Flags & BSP_HAS_LEFT ) n += BSP_Classify_Point( Left, P, Where+n );
         else Where[n++] = &BSP_Leaves[Left];  // store left leaf
      }
      if ( Right )
      {
         if ( BNode->Flags & BSP_HAS_RIGHT ) n += BSP_Classify_Point( Right, P, Where+n );
         else Where[n++] = &BSP_Leaves[Right]; // store right leaf
      }
   }
   return( n );
}

/******************************************************************/

