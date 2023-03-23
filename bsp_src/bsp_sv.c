/***********************************************
 *              BSP                            *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#include "main3d.h"
#include "indy.h"
#include "bsp.h"

/******************************************************************/

EXTERN INT BSP_Save( STRING Name, BSP_TREE *Tree )
{
   FILE *Out;
   INT i;
   INT Nl, Nn, Nf;

   if ( Tree==NULL ) return( 0 );

   Out = Access_File( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( -1 );

   fwrite( BSP_MAGIK, 6, 1, Out );
   Nn = Indy_l( Tree->Nb_Nodes );
   Nl = Indy_l( Tree->Max_Leaves );
   Nf = Indy_l( Tree->Free_Leaves );
   fwrite( &Nn, sizeof(Nn), 1, Out );
   fwrite( &Nl, sizeof(Nl), 1, Out );
   fwrite( &Nf, sizeof(Nf), 1, Out );

   Compress_Write( (BYTE*)Tree->Nodes, Out, Tree->Nb_Nodes*sizeof( BSP_NODE ), 8 );
   Compress_Write( (BYTE*)Tree->Leaves, Out, Tree->Max_Leaves*sizeof( BSP_LEAF ), 8 );

   fclose( Out );
   return( 0 );
}

/******************************************************************/

static void BSP_Print_Leaf( USHORT Leaf, INT Tab )
{
   printf( "%*sLeaf [next:%d][Obj_id:%d][P1/P2:%d/%d]\n",
      Tab, "", BSP_Leaves[Leaf].Next, BSP_Leaves[Leaf].Obj_ID, 
         BSP_Leaves[Leaf].Poly1, BSP_Leaves[Leaf].Poly2 );
   if ( BSP_Leaves[Leaf].Next!=0 ) 
      BSP_Print_Leaf( BSP_Leaves[Leaf].Next, Tab );
}

static void BSP_Print_Node( INT Node, INT Tab )
{
   if ( BSP_Nodes[Node].Flags & BSP_IS_LEAF )
   {
      printf( "%*sLeaf #%d: [fl:0x%.4x][Up:%d][Right:%d]\n",
         Tab, "", Node, BSP_Nodes[Node].Flags, 
         BSP_Nodes[Node].Up, BSP_Nodes[Node].Right );
      printf( "%*s Box:[%.2e,%.2e][%.2e,%.2e][%.2e,%.2e]\n", Tab, "",
         BSP_Nodes[Node].Mins[0], BSP_Nodes[Node].Maxs[0],
         BSP_Nodes[Node].Mins[1], BSP_Nodes[Node].Maxs[1],
         BSP_Nodes[Node].Mins[2], BSP_Nodes[Node].Maxs[2] );
      BSP_Print_Leaf( BSP_Nodes[Node].Right, Tab+2 );
   }
   else
   {
      printf( "%*sNode #%d: [fl:0x%.4x][Up:%d][Right:%d]\n",
         Tab, "", Node, BSP_Nodes[Node].Flags, 
         BSP_Nodes[Node].Up, BSP_Nodes[Node].Right );
      printf( "%*s Box:[%.2e,%.2e][%.2e,%.2e][%.2e,%.2e]\n", Tab, "",
         BSP_Nodes[Node].Mins[0], BSP_Nodes[Node].Maxs[0],
         BSP_Nodes[Node].Mins[1], BSP_Nodes[Node].Maxs[1],
         BSP_Nodes[Node].Mins[2], BSP_Nodes[Node].Maxs[2] );

      if ( BSP_Nodes[Node].Flags & BSP_HAS_LEFT )
      {
         printf( "%*s[->left]\n", Tab,"" );
         BSP_Print_Node( Node+1, Tab+2 );
      }
      else printf( "%*s -no left -\n", Tab, "" );

      if ( BSP_Nodes[Node].Flags & BSP_HAS_RIGHT )
      {
         printf( "%*s[->right]\n", Tab,"" );
         BSP_Print_Node( Node+BSP_Nodes[Node].Right, Tab+2 );
      }
      else printf( "%*s -no right -\n", Tab, "" );
   }
}

EXTERN void BSP_Print_Info( BSP_TREE *Tree )
{
   INT n, l;

   if ( Tree==NULL ) return;

   BSP_Nodes = Tree->Nodes;
   BSP_Leaves = Tree->Leaves;

   BSP_Print_Node( 0, 0 );

   BSP_Nodes = NULL;
   BSP_Leaves = NULL;

   printf( " Nodes ->Nb:%d / max:%d\n", Tree->Nb_Nodes, Tree->Max_Nodes );
   printf( " Leaves ->Max:%d / free:%d\n", Tree->Max_Leaves, Tree->Free_Leaves );
   n = Tree->Free_Leaves;
   if ( n!=-1 )
   {
      while( n!=-1 ) { printf( "[%d]->", n ); n = Tree->Leaves[n].Next; }
      printf( "[]\n" );
   }   
}

/******************************************************************/
