/***********************************************
 *      Mesh optimization                      *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

// #define BLAH

/******************************************************************/

#if 0

EXTERN void Optimize_Split_Mesh( MESH *M, FLT Thresh1, FLT Thresh2 )
{
   INT i, j, k, n, m, p;
   USHORT *Poly_Index = NULL;
   VECTOR P2, P1;
   FLT Max_L, Max_R, Min_R, Tmp; 
   VECTOR *Old_Vertex;
   F_MAPPING *Old_UV;
   POLY *Old_Polys;

   Thresh2 *= Thresh2;  // we're not using sqrt()

   Poly_Index = New_Fatal_Object( M->Nb_Polys, USHORT );
   n = m = 0;
   for( i=0; i<M->Nb_Polys; ++i )
   {
      Poly_Index[i] = -1;     // a priori: don't split;
      P2[0] = P2[1] = P2[2] = -_HUGE_;    // <=Max
      P1[0] = P1[1] = P1[2] =  _HUGE_;    // <=Min
      for( j=0; j<M->Polys[i].Nb_Pts; ++j )
      {
         k = M->Polys[i].Pt[j];
         if ( M->Vertex[k][0]>P2[0] ) P2[0] = M->Vertex[k][0];
         if ( M->Vertex[k][1]>P2[1] ) P2[1] = M->Vertex[k][1];
         if ( M->Vertex[k][2]>P2[2] ) P2[2] = M->Vertex[k][2];
         if ( M->Vertex[k][0]<P1[0] ) P1[0] = M->Vertex[k][0];
         if ( M->Vertex[k][1]<P1[1] ) P1[1] = M->Vertex[k][1];
         if ( M->Vertex[k][2]<P1[2] ) P1[2] = M->Vertex[k][2];
      }
      Tmp = P2[0] - P1[0];
      P1[0] = (P2[0] + P1[0])/2.0f;  // Center
      P2[0] = Tmp;                  // Size
      Max_L = Tmp;
      Tmp = P2[1] - P1[1];
      P1[1] = (P2[1] + P1[1])/2.0f;  // Center
      P2[1] = Tmp;                  // Size
      if ( Tmp>Max_L ) Max_L = Tmp;
      Tmp = P2[2] - P1[2];
      P1[2] = (P2[2] + P1[2])/2.0f;  // Center
      P2[2] = Tmp;                  // Size
      if ( Tmp>Max_L ) Max_L = Tmp;
      if ( Max_L<Thresh1 ) continue;      // Not enough big
      m++;

      Max_R = -_HUGE_; Min_R = _HUGE_;
      for( j=0; j<M->Polys[i].Nb_Pts; ++j )
      {
         FLT R, Tmp;
         k = M->Polys[i].Pt[j];
         Tmp = M->Vertex[k][0] - P1[0];
         R  = Tmp*Tmp;
         Tmp = M->Vertex[k][1] - P1[1];
         R += Tmp*Tmp;
         Tmp = M->Vertex[k][2] - P1[2];
         R += Tmp*Tmp;
         if ( R>Max_R ) Max_R = R;
         if ( R<Min_R ) Min_R = R;
      }
      if ( Min_R>Max_R*Thresh2 ) continue;    // not stretched enough
      Poly_Index[i] = (USHORT)i;     // mark as to be split
      n++;
   }
#ifdef BLAH
   fprintf( stderr, "[Optimize_Split_Mesh] Tested:%d   To be split: %d    Total:%d\n", m, n, M->Nb_Polys );
   fprintf( stderr, "[Optimize_Split_Mesh] expected: Vtx:%d   Polys:%d\n", M->Nb_Vertex + n*3, M->Nb_Polys + n*3 );
#endif

         // 3 additional vertex per splittable polys
         // 3 additional polys per splittable polys

   Old_Vertex = M->Vertex; M->Vertex = NULL;
   m = M->Nb_Vertex;
   Mesh_New_Vertex( M, m + n*3 );
   memcpy( M->Vertex, Old_Vertex, m*sizeof( VECTOR ) );
   M_Free( Old_Vertex );

   Old_UV = M->UV;
   M->UV = New_Fatal_Object( m+n*3, F_MAPPING );
   memcpy( M->UV, Old_UV, m*sizeof( F_MAPPING ) );
   M_Free( Old_UV );

   Old_Polys = M->Polys; M->Polys = NULL;
   p = M->Nb_Polys;
   Mesh_New_Polys( M, p + n*3 );
   memcpy( M->Polys, Old_Polys, p*sizeof( POLY ) );
   M_Free( Old_Polys );

         // Do the split

   n = p; 
   for( i=0; i<n; ++i )
   {
      INT P0, P1, P2;

      if ( Poly_Index[i] != i ) continue;
      if ( M->Polys[i].Nb_Pts!=3 ) continue;    // Aie!!

      P0 = M->Polys[i].Pt[0];
      P1 = M->Polys[i].Pt[1];
      P2 = M->Polys[i].Pt[2];

         // generate 3 new vertex.

      Add_Vector( M->Vertex[m], M->Vertex[P0], M->Vertex[P1] );      
      M->Vertex[m][0] /= 2.0f; M->Vertex[m][1] /= 2.0f; M->Vertex[m][2] /= 2.0;
      M->UV[m][0] = ( M->UV[P0][0] + M->UV[P1][0] ) / 2.0f;
      M->UV[m][1] = ( M->UV[P0][1] + M->UV[P1][1] ) / 2.0f;

      Add_Vector( M->Vertex[m+1], M->Vertex[P1], M->Vertex[P2] );      
      M->Vertex[m+1][0] /= 2.0f; M->Vertex[m+1][1] /= 2.0f; M->Vertex[m+1][2] /= 2.0;
      M->UV[m+1][0] = ( M->UV[P1][0] + M->UV[P2][0] ) / 2.0f;
      M->UV[m+1][1] = ( M->UV[P1][1] + M->UV[P2][1] ) / 2.0f;

      Add_Vector( M->Vertex[m+2], M->Vertex[P2], M->Vertex[P0] );      
      M->Vertex[m+2][0] /= 2.0f; M->Vertex[m+2][1] /= 2.0f; M->Vertex[m+2][2] /= 2.0;
      M->UV[m+2][0] = ( M->UV[P2][0] + M->UV[P0][0] ) / 2.0f;
      M->UV[m+2][1] = ( M->UV[P2][1] + M->UV[P0][1] ) / 2.0f;

         // generate 4 polys
      memcpy( &M->Polys[p], &M->Polys[i], sizeof( POLY ) );
      memcpy( &M->Polys[p+1], &M->Polys[i], sizeof( POLY ) );
      memcpy( &M->Polys[p+2], &M->Polys[i], sizeof( POLY ) );

      M->Polys[p].Pt[0] = P0;
      M->Polys[p].Pt[1] = m;
      M->Polys[p].Pt[2] = m+2;
      M->Polys[p+1].Pt[0] = m;
      M->Polys[p+1].Pt[1] = P1;
      M->Polys[p+1].Pt[2] = m+1;
      M->Polys[p+2].Pt[0] = m+1;
      M->Polys[p+2].Pt[1] = P2;
      M->Polys[p+2].Pt[2] = m+2;

      M->Polys[i].Pt[0] = m;
      M->Polys[i].Pt[1] = m+1;
      M->Polys[i].Pt[2] = m+2;

      m += 3;
      p += 3;
   }

   M_Free( Poly_Index );
   M->Flags |= OBJ_SPLIT_OK;
}

#endif   // 0

/******************************************************************/

#ifndef _SKIP_OPTIM_MESH_

EXTERN void Optimize_Mesh( MESH *M, FLT Thresh )
{
   USHORT *Vtx_Index = NULL;
   VECTOR *New = NULL;
   INT i, j, n;

   if ( M==NULL || M->Nb_Vertex==0 || M->Nb_Polys == 0 )
      return;

   if ( Thresh==0.0 ) return;

   Vtx_Index = New_Fatal_Object( M->Nb_Vertex, USHORT );

   for( i=0; i<M->Nb_Vertex; ++i )
   {
      Vtx_Index[i] = (USHORT)i;
      for( j=0; j<i; ++j )
      {
         FLT r;
         VECTOR D;
         INT k;
         k = Vtx_Index[j];
         D[0] = M->Vertex[i][0] - M->Vertex[k][0];
         r  = D[0]*D[0];
         D[1] = M->Vertex[i][1] - M->Vertex[k][1];
         r += D[1]*D[1];
         D[2] = M->Vertex[i][2] - M->Vertex[k][2];
         r += D[2]*D[2];
         if ( r<=Thresh )
         {
            Vtx_Index[i] = (USHORT)k;
            M->Vertex[j][0] += D[0]/2.0f;
            M->Vertex[j][1] += D[1]/2.0f;
            M->Vertex[j][2] += D[2]/2.0f;
         }
      }
   }

   for( i=0,n=0; i<M->Nb_Vertex; ++i ) if ( Vtx_Index[i]==i ) n++;

#ifdef BLAH
   fprintf( stderr, "[Optimize_Mesh]  Nb real vertex: %d.  Removed: %d\n", n, M->Nb_Vertex-n );
#endif

   if ( n==M->Nb_Vertex )     // No_Remap
   {
      M_Free( Vtx_Index );
      M->Flags = (OBJ_FLAG)(M->Flags | OBJ_OPTIM_OK);
      return;
   }

         // Remap vertex in polys

   New = New_Fatal_Object( n, VECTOR );

   n = 0;
   for( i=0; i<M->Nb_Vertex; ++i )
   {
      j = Vtx_Index[i];

          // move old vertex in new slot n
      if ( j==i )
      {
         Vtx_Index[i] = (USHORT)n;
         New[n][0] = M->Vertex[i][0];
         New[n][1] = M->Vertex[i][1];
         New[n][2] = M->Vertex[i][2];
         n++;
      }
      else Vtx_Index[i] = Vtx_Index[j];  // get new slot
   }

   M_Free( M->Vertex );
   Mesh_New_Vertex( M, n );
   memcpy( M->Vertex, New, n*sizeof( VECTOR ) );
//   bzero( M->Normals, n*sizeof( VECTOR ) );
   M_Free( New );

         // remap all vertex...
         // we always have Vtx_Index[j]<=Vtx_Index[i]

   for( i=0; i<M->Nb_Polys; ++i )
   {
      for( j=0; j<M->Polys[i].Nb_Pts; ++j )
         M->Polys[i].Pt[j] = Vtx_Index[ M->Polys[i].Pt[j] ];
   }

   M->Flags = (OBJ_FLAG)(M->Flags & ~( OBJ_HAS_NORMALS | OBJ_EDGES_OK ));
   Mesh_Store_Normals( M );
   Average_Normals( M );
   M_Free( M->Edges );
   M->Nb_Edges = 0;

   M_Free( Vtx_Index );
   M->Flags = (OBJ_FLAG)(M->Flags | OBJ_OPTIM_OK);

}

#endif   // _SKIP_OPTIM_MESH_

/******************************************************************/

static void Mesh_Store_UV_Area( POLY *P, F_MAPPING *UV )
{
   INT P0, P1, P2;
   FLT dU1, dU2, dV1, dV2;

   P0 = P->Pt[0];
   P1 = P->Pt[1];
   P2 = P->Pt[2];

      // *do* compute dUx/dVx with original, non-tiled, values

   dU1 = ( UV[P1][0]-UV[P0][0] );
   dU2 = ( UV[P2][0]-UV[P1][0] );
   dV1 = ( UV[P1][1]-UV[P0][1] );
   dV2 = ( UV[P2][1]-UV[P1][1] );

   P->U1V2_U2V1 = dU1*dV2-dU2*dV1;
}

EXTERN void Mesh_Store_UV_Mapping( MESH *M )
{
   INT i;

   for( i=0; i<M->Nb_Polys; ++i )
   {
//      INT P0, P1, P2;
//      FLT dU1, dU2, dV1, dV2;
      INT j;
      for( j=0; j<M->Polys[i].Nb_Pts; ++j )
      {
         INT k;
         FLT Map;

         k = M->Polys[i].Pt[j];
         Map = M->UV[k][0];
         Map = fmod( Map, 1.0 );
         // while( Map<0.0 ) Map += 1.0;
         // while( Map>1.0 ) Map -= 1.0;
#if 0
         if ( M->Polys[i].Flags & POLY_U_WRAP )
         { 
//            Map = (1.0-UV_EPSILON) - Map;
            M->Polys[i].UV[j][0] = (USHORT)( Map*UV_SCALE );
         }
         else 
#endif
         M->Polys[i].UV[j][0] = (USHORT)( (UINT)( Map*UV_SCALE ) & 0xffff );

         Map = M->UV[k][1];
         Map = fmod( Map, 1.0 );
         // while( Map<0.0 ) Map += 1.0;
         // while( Map>1.0 ) Map -= 1.0;
#if 0
         if ( M->Polys[i].Flags & POLY_V_WRAP )
         {
//            Map = (1.0-UV_EPSILON) - Map;
            M->Polys[i].UV[j][1] = (USHORT)( Map*UV_SCALE );
         }
         else
#endif
         M->Polys[i].UV[j][1] = (USHORT)( (UINT)( Map*UV_SCALE ) & 0xffff );
      }
      Mesh_Store_UV_Area( &M->Polys[i], M->UV );
   }
   M_Free( M->UV );
   M->Flags = (OBJ_FLAG)(M->Flags | OBJ_HAS_POLY_UV);

}

/******************************************************************/

#ifndef _SKIP_EDGES_

EXTERN void Do_Mesh_Store_Edges( MESH *M )
{
   INT i, Stored;
   MSH_EDGE *New;

   Stored = 0;
   for( i=0; i<M->Nb_Polys; ++i )
   {
      INT S, j;

      S = M->Polys[i].Pt[0];
      j = 1;
      do {
         INT F, k, Cur, Pt;

         F = M->Polys[i].Pt[j];
         for( k=0; k<Stored; ++k ) // Find common edge already stored
         {
            if ( (M->Edges[k].Start==S) && (M->Edges[k].End==F) ) goto Found;
            if ( (M->Edges[k].Start==F) && (M->Edges[k].End==S) ) goto Found;
         }

              // edge not found. Create.
              // [S-F] segment always belong to Poly #1
              // [F-S] is for Poly #2
         M->Edges[Stored].Start = (USHORT)S;
         M->Edges[Stored].End = (USHORT)F;
         M->Edges[Stored].Poly1 = (USHORT)i;
         M->Edges[Stored].Poly2 = 0xFFFF;    // <= means: border
         M->Edges[Stored].Pt_P2 = 0xffff;    // edge, a priori...
         M->Edges[Stored].State = EDGE_STATE_VOID;
         M->Edges[Stored].Type = EDGE_TYPE_SHARP|EDGE_BORDER_EDGE;
         Cur = Stored;
         Stored++;
         goto Continue;

Found:
            // already stored...

         if ( M->Edges[k].Poly2==0xFFFF )    // empty Poly2 in edge.
         {
            M->Edges[k].Poly2 = (USHORT)i;
            M->Edges[k].Type = EDGE_TYPE_NORMAL;
            Cur = k;
         }
         else  // multifolded border => Create new.
         {
            M->Edges[Stored].Start = M->Edges[k].Start;  // keep orientation 
            M->Edges[Stored].End   = M->Edges[k].End;    // for poly #1
            M->Edges[Stored].Pt_P1 = M->Edges[k].Pt_P1;
            M->Edges[Stored].Poly1 = M->Edges[k].Poly1;
            M->Edges[Stored].Poly2 = (USHORT)i;
            M->Edges[Stored].State = EDGE_STATE_VOID;
            M->Edges[Stored].Type = EDGE_TYPE_NORMAL;
            Cur = Stored;
            Stored++;
         }
Continue:
            // find third point for poly

         for( Pt=0; Pt<M->Polys[i].Nb_Pts; ++Pt )
            if ( (M->Polys[i].Pt[Pt]!=S) && (M->Polys[i].Pt[Pt]!=F) )
               break;
         if ( i==M->Edges[Cur].Poly1 )
            M->Edges[Cur].Pt_P1 = M->Polys[i].Pt[Pt];
         else M->Edges[Cur].Pt_P2 = M->Polys[i].Pt[Pt];

            // Update poly. Edges[k] contains ID of edge
            // starting from this vertex...

         k = j-1;
         if ( k<0 ) k = M->Polys[i].Nb_Pts-1;
         M->Polys[i].Edges[k] = (USHORT)Cur;

         j++;
         if ( j==M->Polys[i].Nb_Pts ) j=0;
         S = F;
      } while( j!=1 );
   }
#ifdef BLAH
   fprintf( stderr, "[Do_Mesh_Store_Edges] Stored:%d Msh->Nb_Edges:%d\n", Stored, M->Nb_Edges );
#endif
   New = New_Fatal_Object( Stored, MSH_EDGE );
   memcpy( New, M->Edges, Stored*sizeof( MSH_EDGE ) );
   M_Free( M->Edges );
   M->Edges = New;
   M->Nb_Edges = Stored;   // Store the actual number of edges.
}

/******************************************************************/

EXTERN MSH_EDGE *Mesh_Store_Edges( MESH *M )
{
//   if ( M->Edges != NULL ) return( M->Edges );  // already done...

   M_Free( M->Edges );

      // We don't know the object's genus... well, can't be
      // greater than Nb_Polys, isn't it? :)

   M->Nb_Edges = 3*( M->Nb_Polys + M->Nb_Vertex );  // <=hope its enough!!! XXXXX
   M->Nb_Edges += 1000;

   M->Edges = New_Object( M->Nb_Edges, MSH_EDGE );
   if ( M->Edges == NULL ) return( NULL );
   memset( M->Edges, 0, M->Nb_Edges*sizeof( MSH_EDGE ) );
   Do_Mesh_Store_Edges( M );
   return( M->Edges );
}

/******************************************************************/

EXTERN void Mesh_Detect_Sharp_Edges( MESH *M, FLT Thresh )
{
   INT i;

   if ( M->Edges==NULL ) return;
//      if ( Mesh_Store_Edges( M ) == NULL )
//         return;

   for( i=0; i<M->Nb_Edges; ++i )
   {
      INT P1, P2;
      FLT Angle, N;
      INT j;
      VECTOR C;

      P1 = M->Edges[i].Poly1;
      P2 = M->Edges[i].Poly2;
      if ( P2==0xFFFF )
      {
         M->Edges[i].Type |= EDGE_TYPE_SHARP;
         M->Edges[i].Type &= ~EDGE_TYPE_FLAT;
         M->Edges[i].Type |= EDGE_BORDER_EDGE;
         M->Edges[i].Sharpness = 0x7FFF;
         continue;
      }

      Angle = Dot_Product( M->Polys[P1].No, M->Polys[P2].No );
//      N = sqrt( Norm_Squared( M->Polys[P1].No ) );
//      N *= sqrt( Norm_Squared( M->Polys[P2].No ) );
//      if ( N>0.0 ) Angle /= N;
#ifdef BLAH
      else { fprintf( stderr, "N=%f!!\n", N ); N = 1.0; }
#endif
      if ( Angle>EDGE_FLATNESS )
         M->Edges[i].Type |= EDGE_TYPE_FLAT;
      else M->Edges[i].Type &= ~EDGE_TYPE_FLAT;

      M->Edges[i].Type &= ~EDGE_TYPE_SHARP;
      j = M->Polys[P2].Pt[0];
      Set_Vector( C, M->Vertex[j][0], M->Vertex[j][1], M->Vertex[j][2] );
      j = M->Polys[P2].Pt[1];
      Add_Vector_Eq( C, M->Vertex[j] );
      j = M->Polys[P2].Pt[2];
      Add_Vector_Eq( C, M->Vertex[j] );
      j = M->Polys[P1].Pt[0];
      Sub_Vector_Eq( C, M->Vertex[j] );
      j = M->Polys[P1].Pt[1];
      Sub_Vector_Eq( C, M->Vertex[j] );
      j = M->Polys[P1].Pt[2];
      Sub_Vector_Eq( C, M->Vertex[j] );

      Angle = Dot_Product( C, M->Polys[P1].No );
      if ( Angle<Thresh )
         M->Edges[i].Type |= EDGE_TYPE_SHARP;
      N = Norm_Squared( C );
      if ( N>0.0 ) Angle /= (FLT)sqrt( N );
      else Angle = -1.0;
      M->Edges[i].Sharpness = (SHORT)(Angle*32767.0f);
   }
   M->Flags = (OBJ_FLAG)(M->Flags | OBJ_EDGES_OK);

}

/******************************************************************/

#endif   // _SKIP_EDGES_
