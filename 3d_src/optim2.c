/***********************************************
 *      Mesh optimization                      *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "ray.h"

#ifndef _SKIP_OPTIM2_MESH_

/******************************************************************/

static OBJ_NODE *Opt_Root;
static MESH *Opt_Msh;
static OBJ_NODE *Opt_Node;
static INT Nb_Opt_Vtx;
static VECTOR *V_Stack;

#define O_EPS     0.00001

/******************************************************************/

static INT Test_Point_Inside_Poly( POLY_RAYTRACE_CST *Cst, VECTOR I )
{
   FLT x, y, UI, VI;

   UI = Dot_Product( Cst->PoP1, I );
   VI = Dot_Product( Cst->PoP2, I );

   x = Cst->V2 * UI - Cst->UV * VI + Cst->xo;
   if ( x<0.0 ) return( -1 );
   y =-Cst->UV * UI + Cst->U2 * VI + Cst->yo;
   if ( y<0.0 ) return( -1 );
   if ( x+y>1.0 ) return( -1 );
   return( 0 );
}

/******************************************************************/

#if 0

static void Optimize_AB( OBJ_NODE *Node )
{
   MESH *Msh;
   INT i, p;
   VECTOR Vertex[32768];
   FLT Dot_V[32768];

   if ( Node==Opt_Node ) return;  // don't test against oneself

//   if ( Node->ID>Opt_Node->ID ) return;  // already done before...
                                       // since tree is traversed
                                       // along *decreasing* ID...


   Msh = (MESH*)Node->Data;
   if ( Msh->Nb_Vertex==0 || Msh->Nb_Polys==0 ) return;

   Check_Node_Transform( Node );

         // Check boxes
#if 0
   BBox_Transform_Min_Max( &Node->Box, Node->Mo );   
   if ( Node->Box.TMins[0]> Opt_Node->Box.TMaxs[0] ) return;
   if ( Node->Box.TMaxs[0]< Opt_Node->Box.TMins[0] ) return;
   if ( Node->Box.TMins[1]> Opt_Node->Box.TMaxs[1] ) return;
   if ( Node->Box.TMaxs[1]< Opt_Node->Box.TMins[1] ) return;
   if ( Node->Box.TMins[2]> Opt_Node->Box.TMaxs[2] ) return;
   if ( Node->Box.TMaxs[2]< Opt_Node->Box.TMins[2] ) return;
#endif
   fprintf( stderr, "Checking Opt_Node #%d against Node #%d\n",
      Opt_Node->ID, Node->ID );

   for( p=0; p<Msh->Nb_Polys; ++p )
   {
      POLY *P;
      VECTOR N, Po;
      POLY_RAYTRACE_CST *Cst;
      INT Pos, Neg, i, i1, j;
      FLT xo;

      P = &Msh->Polys[p];
      Cst = &Msh->Cst[p];

      nA_V( N, Node->Mo, P->No );
      Pos = Neg = 0;
      A_V( Po, Node->Mo, Msh->Vertex[ P->Pt[0] ] );
      xo = Dot_Product( Po, N );

      for( i=0; i<Opt_Msh->Nb_Vertex; ++i )
      {
         Dot_V[i] = Dot_Product( V_Stack[i], N ) - xo;
         if ( Dot_V[i]>O_EPS ) Pos++;
         else if ( Dot_V[i]<O_EPS ) Neg++;
      }

      if ( (Pos==0) || (Neg==0) ) continue; // all vertex on same side

      if ( Neg || Pos ) fprintf( stderr, "Poly #%d: Pos=%d Neg=%d\n", p, Pos, Neg );

      for( j=0; j<Opt_Msh->Nb_Polys; ++j )
      {
         POLY *P2 = &Opt_Msh->Polys[j];

         for( i=i1=0; i<P2->Nb_Pts; ++i )
         {
            INT p1, p2, K;
            FLT x, y;
            RAY Ray;

            i1++;
            if ( i1==P2->Nb_Pts ) i1=0;
            p1 = P2->Pt[i];
            p2 = P2->Pt[i1];
            if ( Dot_V[p1]==0.0 && Dot_V[p2]==0.0 ) continue; // already done
            if ( (Dot_V[p1]<=O_EPS)&&(Dot_V[p2]<=O_EPS) ) continue;
            if ( (Dot_V[p1]>=O_EPS)&&(Dot_V[p2]>=O_EPS) ) continue;

            INIT_RAY( &Ray );
            A_Inv_V( Ray.Orig, Node->Mo, V_Stack[p1] );
            A_Inv_V( Ray.Dir, Node->Mo, V_Stack[p2] );
            Sub_Vector_Eq( Ray.Dir, Ray.Orig );
            Normalize( Ray.Dir );
            Ray.Z_Unit = 1.0;
            if ( Intersect_Mesh( &Ray, Node )!=-1 )
               fprintf( stderr, "z=%f\n", Ray.Z );
            else continue;
 
              // Test edge
              // first, check is edge actually cross the inner
              // surface of poly, and not just its plane

            x = Dot_V[p2] - Dot_V[p1];
            if ( fabs(x)<O_EPS ) continue;      // really too close
            x = -Dot_V[p1] / x;                 // displacement
            y = 1.0-x;

                  // jump to poly's plane

            Po[0] = V_Stack[p1][0]*y + V_Stack[p2][0]*x;
            Po[1] = V_Stack[p1][1]*y + V_Stack[p2][1]*x;
            Po[2] = V_Stack[p1][2]*y + V_Stack[p2][2]*x;

                  // Inv transform in poly's original base

            A_Inv_V_Eq( Po, Node->Mo );
            if ( Test_Point_Inside_Poly( Cst, Po ) )
               continue;

            fprintf( stderr, "." );
            if ( x<0.5 ) { x *= 1.0-O_EPS; y = 1.0-x; K = p1; Dot_V[K] = -O_EPS; }
            else { y *= 1.0-O_EPS; x = 1.0-y; K = p2; Dot_V[K] = +O_EPS;  }
            V_Stack[K][0] = V_Stack[p1][0]*y + V_Stack[p2][0]*x;
            V_Stack[K][1] = V_Stack[p1][1]*y + V_Stack[p2][1]*x;
            V_Stack[K][2] = V_Stack[p1][2]*y + V_Stack[p2][2]*x;
            A_Inv_V( Opt_Msh->Vertex[K], Opt_Node->Mo, V_Stack[K] );
            Dot_V[K] = 0.0; // Dot_Product( V_Stack[K], N ) - xo;
         }
      }
   }
}
#endif // 0

static void Optimize_AB( OBJ_NODE *Node )
{
   MESH *Msh;
   INT i, i1, j;
//   INT p;
//   VECTOR Vertex[32768];
//   FLT Dot_V[32768];

   if ( Node==Opt_Node ) return;  // don't test against oneself

//   if ( Node->ID>Opt_Node->ID ) return;  // already done before...
                                       // since tree is traversed
                                       // along *decreasing* ID...


   Msh = (MESH*)Node->Data;
   if ( Msh->Nb_Vertex==0 || Msh->Nb_Polys==0 ) return;

   Check_Node_Transform( Node );

         // Check boxes
#if 0
   BBox_Transform_Min_Max( &Node->Box, Node->Mo );   
   if ( Node->Box.TMins[0]> Opt_Node->Box.TMaxs[0] ) return;
   if ( Node->Box.TMaxs[0]< Opt_Node->Box.TMins[0] ) return;
   if ( Node->Box.TMins[1]> Opt_Node->Box.TMaxs[1] ) return;
   if ( Node->Box.TMaxs[1]< Opt_Node->Box.TMins[1] ) return;
   if ( Node->Box.TMins[2]> Opt_Node->Box.TMaxs[2] ) return;
   if ( Node->Box.TMaxs[2]< Opt_Node->Box.TMins[2] ) return;
#endif
   fprintf( stderr, "Checking Opt_Node #%d against Node #%d\n",
      Opt_Node->ID, Node->ID );

   for( j=0; j<Opt_Msh->Nb_Polys; ++j )
   {
      POLY *P2 = &Opt_Msh->Polys[j];

      for( i=i1=0; i<P2->Nb_Pts; ++i )
      {
         INT p1, p2, K;
         FLT x, y;
         RAY Ray;

         i1++;
         if ( i1==P2->Nb_Pts ) i1=0;
         p1 = P2->Pt[i];
         p2 = P2->Pt[i1];

         INIT_RAY( &Ray );
         A_Inv_V( Ray.Orig, Node->Mo, V_Stack[p1] );
         A_Inv_V( Ray.Dir, Node->Mo, V_Stack[p2] );
         Sub_Vector_Eq( Ray.Dir, Ray.Orig );
         x = sqrt( Norm_Squared( Ray.Dir ) );
         if ( x==0.0 ) continue;
         Ray.Z_Unit = 1.0;
         if ( Intersect_Mesh( &Ray, Node )==-1 ) continue;
         x = Ray.Z ; // / x;
         if ( x<=0.0 || x>=1.0 ) continue;
         fprintf( stderr, "x=%f\n", x );
         fprintf( stderr, "." );
         if ( x<0.5 ) { x *= 1.0-O_EPS; y = 1.0-x; K = p1; }
         else { y *= 1.0-O_EPS; x = 1.0-y; K = p2; }
         Add_Scaled_Vector( V_Stack[K], Ray.Orig, Ray.Dir, Ray.Z );
         A_V_Eq( V_Stack[K], Node->Mo );
         A_Inv_V( Opt_Msh->Vertex[K], Opt_Node->Mo, V_Stack[K] );
      }
   }
}

EXTERN void Optimize_Mesh_Vertex( OBJ_NODE *Cur )
{
   VECTOR Stack[32768];
   INT i;
   if ( Cur==NULL ) return;

   Opt_Node = Cur;
   Opt_Msh = (MESH*)Cur->Data;
   if ( Opt_Msh->Type != MESH_TYPE ) return;
   if ( Opt_Msh->Nb_Vertex==0 || Opt_Msh->Nb_Polys==0 )
      return;

   Check_Node_Transform( Cur );    // Cur->Mo is ok.
   BBox_Transform_Min_Max( &Opt_Node->Box, Opt_Node->Mo );

   Nb_Opt_Vtx = Opt_Msh->Nb_Vertex;
   for( i=0; i<Nb_Opt_Vtx; ++i )
   {
      A_V( Stack[i], Opt_Node->Mo, Opt_Msh->Vertex[i] );
   }
   V_Stack = Stack;
   Traverse_Typed_Nodes( Opt_Root, MESH_TYPE, Optimize_AB );
}

EXTERN void Optimize_All_Meshes( WORLD *W )
{
   Opt_Root = W->Root;
   if ( Opt_Root==NULL ) return;
   Traverse_Typed_Nodes( Opt_Root, MESH_TYPE, Optimize_Mesh_Vertex );
}

/******************************************************************/

EXTERN INT Finish_Objects_Light( OBJ_NODE *Cur )
{
   while( Cur!=NULL )
   {
      Set_Ray_Tracing_Constants( Cur );
      if ( Cur->Data!=NULL )
      {
         Cur->Data->ID = Cur->ID;
         if ( NODE_FINISH( Cur ) )
            return( -1 );
        // else => Unkown object. Should set Err to -1 ??
      }

      if ( Cur->Parent != NULL ) 
         Insert_In_Upper_Box( &Cur->Parent->Box, &Cur->Box );

      if ( Cur->Child!=NULL ) Finish_Objects_Light( Cur->Child );
      Cur = Cur->Next;

   }
   return( 0 );
}

EXTERN void Setup_World_Light( WORLD *W )   
{
   W->State = 0x00;

   Finish_Objects_Light( W->Root );

   W->Max_Obj = 0; W->Max_Polys = 0; W->Max_Edges = 0;

   Count_Nodes( W->Root, &W->Max_Obj, &W->Max_Polys, &W->Max_Edges );

   W->Max_Vertex = Find_Max_Vertex( W->Objects );

   if ( W->Max_Edges<W->Max_Polys )    // some edges are missing...
      W->Max_Edges = W->Max_Polys + W->Max_Vertex + 10; // !! Genus?

   W->State = 0x01;
}

/******************************************************************/

#endif   // _SKIP_OPTIM_MESH_

