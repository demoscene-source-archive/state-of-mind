/***********************************************
 *              mesh.c                         *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

EXTERN BYTE Zo_Clipped, Z1_Clipped;
EXTERN BYTE Xo_Clipped, X1_Clipped;
EXTERN BYTE Yo_Clipped, Y1_Clipped;

#define DO_KEY(K)  { if ( (K)>Key ) Key = (K); }
// #define DO_KEY(K)  { Key += (K); }

/******************************************************************/

EXTERN void Transform_Vertices_I( MATRIX2 M )
{
   INT i;
   FLT ZScale, Mesh_Min, Mesh_Max;

   Mesh_Max = -_HUGE_;
   Mesh_Min =  _HUGE_;

      /* TODO: FIx tHaT !! Can't use BZero() here !!!  */
   memset( Vertex_State, 0, Cur_Msh->Nb_Vertex*sizeof(PIXEL) );


      /*
       * 1rst pass. Select vertices of polys that have
       * No.U<0 and all vertices in _RCst_.Out_Clips[]. Update
       * Box.Clips_Z[], P_Vertex[].xp, .yp, .Inv_Z and
       * .Key...
       * Mark vertex state with convention:
       *    0x00: not processed yet                     (=FLAG_VTX_NOT_TESTED)
       *    0x01: rejected                              (=FLAG_VTX_REJECTED)
       *    0x02: Ok. Projected on screen. Not clipped. (=FLAG_VTX_TESTED)
       *    0x03: needs being (Z-)clipped               (== FLAG_VTX_VISIBLE)
       *       if Z-clipped (with _RCst_.Clips[4]), .xp
       *       and .yp are computed.
       *       _RCst_.Clips[4] must be >0.0
       *    Bits 0x04->0x80: Clipping flags           (VTX_CLIP_Yo/Y1 unused)
       */

   Orig_Poly = Cur_Msh->Polys;
   Nb_Poly_Sorted = 0;
   Cur_P_Poly = Poly_To_Sort;
   for( i=0; i<Cur_Msh->Nb_Polys; ++i )
   {
      INT j;
      FLT Key;
      VECTOR N;

            // Backface cull

      Orig_Poly->Clipped = FLAG_POLY_NOT_VISIBLE;
      nA_V( Cur_P_Poly->N, M, Orig_Poly->No );
      if ( Cur_P_Poly->N[2]>B_CULL_EPSILON ) goto End;

            // Project in N, not in P_Vertex[K].N !
            // Otherwise, one may overwrite some previously 
            // shaded value.

      j = Orig_Poly->Pt[0];
      A_V( N, M, (FLT *)( Cur_Msh->Vertex+j ) );
      if ( Dot_Product( N, Cur_P_Poly->N )>0.0 ) goto End;

      Zo_Clipped = Z1_Clipped = Xo_Clipped = X1_Clipped = 
         Yo_Clipped = Y1_Clipped = Orig_Poly->Nb_Pts;

      j = Orig_Poly->Nb_Pts - 1;   /* <= Nb_Vertices_To_Deal_With */
      Key = 0.0;
      for( ;j>=0; --j )
      {
         INT K;

         Cur_Poly_Pt = j;
         Cur_Vtx_Nb = K = Orig_Poly->Pt[j];

         if ( (Vertex_State[K]&FLAG_VTX_VISIBLE)==FLAG_VTX_REJECTED ) goto End;
         else if ( (Vertex_State[K]&FLAG_VTX_VISIBLE)==FLAG_VTX_NOT_TESTED )
         {
            FLT Inv_Z;

                  // Keep projected vertex in .N field

            Cur_P_Vertex = P_Vertex + K;
            if ( j!=0 )
               A_V( P_Vertex[K].N, M, (FLT *)( Cur_Msh->Vertex+K ) );
            else Set_Vector( P_Vertex[K].N, N[0], N[1], N[2] );

            if ( P_Vertex[K].N[2]<_RCst_.Out_Clips[4] ) 
            {
Reject:
               Vertex_State[K] = FLAG_VTX_REJECTED;
               goto End;
            }
            if ( P_Vertex[K].N[2]>_RCst_.Out_Clips[5] ) goto Reject;

            if ( fabs(P_Vertex[K].N[2])<Z_EPSILON )
               P_Vertex[K].N[2] = Z_EPSILON;

            P_Vertex[K].Inv_Z = Inv_Z = 1.0f/P_Vertex[K].N[2];

            Vertex_State[K] = FLAG_VTX_TESTED;

            Inv_Z *= _RCst_._Lx_;

            P_Vertex[K].xp = P_Vertex[K].N[0]*Inv_Z;
            P_Vertex[K].xp += _RCst_._Cx_;
            P_Vertex[K].yp = -P_Vertex[K].N[1]*Inv_Z;
            P_Vertex[K].yp += _RCst_._Cy_;

            if ( P_Vertex[K].N[2]<=_RCst_.Clips[4] )
            {
               Vertex_State[K] |= ( FLAG_VTX_VISIBLE | VTX_CLIP_Zo );
               Zo_Clipped--;
               DO_KEY( _RCst_.Clips[4] );   // Key += _RCst_.Clips[4];  // P_Vertex[K].N[2];
               goto Skip;  // don't set clipping flags now. xp and yp are wrong.
            }
            else if ( P_Vertex[K].N[2]>_RCst_.Clips[6] )
            {
               Vertex_State[K] |= ( FLAG_VTX_VISIBLE | VTX_CLIP_Z1 );
               Z1_Clipped--;
               DO_KEY( _RCst_.Clips[6] );
            }
            else DO_KEY( P_Vertex[K].N[2] );

//            if ( P_Vertex[K].xp<_RCst_.Out__RCst_.Clips[0] || P_Vertex[K].xp>_RCst_.Out_Clips[1] )
//               goto Reject;
            
            if ( P_Vertex[K].xp<_RCst_.Clips[0] )
            {
               Vertex_State[K] |= ( FLAG_VTX_VISIBLE | VTX_CLIP_Xo );
               Xo_Clipped--;
            }
            else if ( P_Vertex[K].xp>=_RCst_.Clips[1] )
            {
               Vertex_State[K] |= ( FLAG_VTX_VISIBLE | VTX_CLIP_X1 );
               X1_Clipped--;
            }

               // for y clipping, don't update flags. it's just for
               // total y-clipping info.

//            if ( P_Vertex[K].yp<_RCst_.Out_Clips[2] || P_Vertex[K].yp>_RCst_.Out_Clips[3] )
//            goto Reject;

            if ( P_Vertex[K].yp<_RCst_.Clips[2] ) 
            {
               Vertex_State[K] |= ( FLAG_VTX_VISIBLE | VTX_CLIP_Yo );
               Yo_Clipped--;
            }
            if ( P_Vertex[K].yp>=_RCst_.Clips[3] )
            {
               Y1_Clipped--;
               Vertex_State[K] |= ( FLAG_VTX_VISIBLE | VTX_CLIP_Y1 );
            }

Skip:

            //////////////// retreive vertex fields /////////////////////
            // should be (((MATERIAL*)Orig_Poly->Ptr)->Shader) here!!! //
            // .N[2] field (=z -> Key) mustn't be touched by shader.   //
            /////////////////////////////////////////////////////////////

            if ( Cur_Shader->Shade_Vertex!=NULL )
               (*Cur_Shader->Shade_Vertex)( M );
         }
         else
         {
            if ( Vertex_State[K]&VTX_CLIP_Zo ) Zo_Clipped--;
            if ( Vertex_State[K]&VTX_CLIP_Z1 ) Z1_Clipped--;
            if ( Vertex_State[K]&VTX_CLIP_Xo ) Xo_Clipped--;
            if ( Vertex_State[K]&VTX_CLIP_X1 ) X1_Clipped--;
            if ( Vertex_State[K]&VTX_CLIP_Yo ) Yo_Clipped--;
            if ( Vertex_State[K]&VTX_CLIP_Y1 ) Y1_Clipped--;
            DO_KEY( P_Vertex[K].N[2] );
         }
         Orig_Poly->Clipped |= Vertex_State[K];
      }

      if ( ( X1_Clipped == 0 ) || ( Xo_Clipped == 0 ) ||
           ( Y1_Clipped == 0 ) || ( Yo_Clipped == 0 ) ||
           ( Zo_Clipped == 0 ) || ( Z1_Clipped == 0 ) )
      {
         Orig_Poly->Clipped = FLAG_POLY_NOT_VISIBLE;
         goto End; // Totally x,y, or z-clipped -> invisible
      }
             // Ok. Put with the rest

      Nb_Poly_Sorted++;
      Cur_P_Poly->Poly_Orig = Orig_Poly;

             // Keep only useful clipping flags

      Orig_Poly->Clipped &= ~FLAG_FOR_CLIPPING_II;
      Orig_Poly->Clipped |= FLAG_POLY_VISIBLE;   // Mark as visible.

      //    Key /= Orig_Poly->Nb_Pts

      if ( Key>Mesh_Max ) Mesh_Max = Key;
      if ( Key<Mesh_Min ) Mesh_Min = Key;
      Cur_P_Poly->Key = Key;

      Cur_P_Poly++;
End:
      Orig_Poly++;
   }
   if ( !Nb_Poly_Sorted ) return;

      // will sort vertex with max precision. Not just
      // global zbuffer scale/min/max...

   ZScale = 65536.0f / ( Mesh_Max-Mesh_Min + .0001f );
   for( i=0; i<Nb_Poly_Sorted; ++i )
   {
      UINT Key = (UINT)floor( ( Mesh_Max - Poly_To_Sort[i].Key )*ZScale );
      Poly_Keys[ i ] = (i<<16) | (Key&0xffff);  // TODO: Truncate??
   }
   Renderer.Sort_Polys( Poly_Keys, Nb_Poly_Sorted );
   if ( Mesh_Max>_RCst_.All_Z_Max ) _RCst_.All_Z_Max = Mesh_Max;
   if ( Mesh_Min<_RCst_.All_Z_Min ) _RCst_.All_Z_Min = Mesh_Min;
}

/******************************************************************/
/******************************************************************/

// #define PRINT_STATS

static void Render_Mesh( OBJ_NODE *Cur_Node, MATRIX2 M )
{
   INT i;
#ifdef PRINT_STATS
   static ULINT T1, T2, Tm1=0, Tm2=0, Tn=0;
#endif

   Cur_Obj = (OBJECT *)Cur_Node->Data;
   Cur_Msh = (MESH *)Cur_Obj;

      // TODO: some objects are fake in .3ds files (e.g.lights). Fix that!
   if ( Cur_Msh->Polys==NULL ) return;

      // Set texture (and/or MATERIAL ??) for all mesh

   Cur_Shader = Cur_Msh->Shader;

         ////////////  Transform and sort ////////////////

#ifdef PRINT_STATS
   T1 = _Clock( );
#endif

   if ( Cur_Shader->Transform!=NULL )  /* Special transform ?? */
      (*Cur_Shader->Transform)( M );
         /* no special transform method. Process as usual. */
   else  Transform_Vertices_I( M );

#ifdef PRINT_STATS
   T2 = _Clock( );
   T2 = T2-T1; fprintf( stderr, "Transf:%d  ", T2 );
   Tm1 += T2;
#endif

         //////////// and emit polys spans /////////////

#ifdef PRINT_STATS
   T1 = _Clock( );
#endif
   for( i=0; i<Nb_Poly_Sorted; ++i )
   {
      INT j;

      Cur_P_Poly = &Poly_To_Sort[ Poly_Keys[i]>>16 ];
      Orig_Poly = Cur_P_Poly->Poly_Orig;
      Nb_Edges_To_Deal_With = Orig_Poly->Nb_Pts;

      Cache_Methods.Init_From_Poly( );
      Setup_Poly_Vtx( );

      if ( fabs(Area)<=AREA_EPSILON3 ) continue;

// if ( (i==0)&&(Area>0.0) ) fprintf( stderr, "az1=%f az2=%f \n", aZ1, aZ2  );

         // take care of P_Vertexes that will be at stake...
      if ( Orig_Poly->Flags & POLY_MAP_UV )
         for( j=0; j<Orig_Poly->Nb_Pts; ++j )
         {
            Out_Vtx[j]->UV[0] = (1.0f/UV_SCALE)*Orig_Poly->UV[j][0];
            Out_Vtx[j]->UV[1] = (1.0f/UV_SCALE)*Orig_Poly->UV[j][1];
         }

      if ( Cur_Shader->Emit_Poly!=NULL ) (*Cur_Shader->Emit_Poly)( );
      else Renderer.Emit_Poly( );
   }

#ifdef PRINT_STATS
   T2 = _Clock( );
   T2 = T2-T1; fprintf( stderr, "Emit:%d  ", T2 );
   Tm2 += T2;

   fprintf( stderr, "     \r" );
   Tn++;
   if (Tn==100)
   {
      fprintf( stderr, "\n=>_M_:%f %f\n", (FLT)Tm1/(FLT)Tn, (FLT)Tm2/(FLT)Tn );
      Tm1=Tm2=Tn=0;
   }
// Total_Poly_Sorted += Nb_Poly_Sorted;   /* Not useful. Debugging. */

#endif
}

/******************************************************************/
/******************************************************************/

static void Init( OBJECT *Obj )
{
   MESH *New;

   New = (MESH *)Obj;

   New->Nb_Vertex = 0;
   New->Vertex    = NULL;
   New->Normals   = NULL;
   New->UV        = NULL;
   New->Shadow_ID = NULL;
   New->Polys = NULL;
   New->Nb_Polys = 0;

   New->Nb_Edges = 0;
   New->Edges = NULL;

   New->Shader = NULL;

   New->Pos_k = New->Rot_k = New->Scale_k = NULL;
   New->Morph_k = New->Hide_k = NULL;

   New->Morph_Node = NULL;
   New->Vertex_Copy = NULL;
   New->Normals_Copy = NULL;
   New->Morph_Flags = 0x0000;

   New->Cst  = NULL;
   New->Type = MESH_TYPE;
}

static void Destroy( OBJECT *Dum )
{
   MESH *M;

   M = (MESH *)Dum;
   if ( M==NULL ) return;
   M_Free( M->Vertex );
   M_Free( M->UV );
   M_Free( M->Shadow_ID );
   M->Nb_Vertex = 0;

      // we should decrement Mat->Users for each poly, here!
   M_Free( M->Polys );
   M->Nb_Polys = 0;


   Destroy_Spline_3ds( &M->Pos_k );
   Destroy_Spline_3ds( &M->Rot_k );
   Destroy_Spline_3ds( &M->Scale_k );
   Destroy_Spline_3ds( &M->Morph_k );
   Destroy_Spline_3ds( &M->Hide_k );

   M_Free( M->Edges );
   M->Nb_Edges = 0;
   M->Flags = (OBJ_FLAG)( M->Flags &
     (~(OBJ_TRACK_OK|OBJ_HAS_UV|OBJ_HAS_POLY_UV|
       OBJ_EDGES_OK|OBJ_HAS_NORMALS|OBJ_TRACK_OK)) 
   );

   M_Free( M->Vertex_Copy );
   M_Free( M->Normals_Copy );
   M_Free( M->Morph_Node );
   M->Morph_Flags = 0x0000;
}

/******************************************************************/

#ifndef _SKIP_MAP_SPHERICAL_

EXTERN void Mesh_Map_Spherical( OBJ_NODE *Node )
{
   INT i;
   MESH *M;

      // (box must be set before...)
   M = (MESH *)Node->Data;
   for( i=0; i<M->Nb_Vertex; ++i )
   {
      VECTOR V;
      FLT T;
      Add_Vector( V, Node->Box.Mins, Node->Box.Maxs );
      Scale_Vector_Eq( V, .5 );  // V = box' center
      Sub_Vector_Eq( V, M->Vertex[i] );
      T = (FLT)sqrt( V[0]*V[0] + V[1]*V[1] );
      if ( T>0.0 )
      {
         M->UV[i][0] = .5f*V[2]/T + .5f;
         M->UV[i][1] = (FLT)atan2( V[1], V[0] )/(2.0f*M_PI) + 0.5f;
      }
      else { M->UV[i][0] = M->UV[i][1] = 0.0; }
   }
   M->Flags = (OBJ_FLAG)( M->Flags | OBJ_HAS_UV );
}

#endif   // _SKIP_MAP_SPHERICAL_

 /******************************************************************/

static INT Finish( OBJ_NODE *Node )
{
   MESH *M;
   INT i;
   MATERIAL *Mat;       // Fix 3DS bug ?!

   M = (MESH *)Node->Data;
   if ( M==NULL ) return( 0 );

      // Mesh optimizations.

//   M->Flags &= ~OBJ_HAS_NORMALS;
//   if ( !(M->Flags & OBJ_HAS_NORMALS) )
   if ( M->Flags & OBJ_DONT_OPTIMIZ )  // do it before ->UV is freed
      Mesh_Store_Normals2( M );
   else Mesh_Store_Normals( M );
   Average_Normals( M );

   Set_Up_Box( M->Vertex, M->Nb_Vertex, &Node->Box ); // compute average size

#ifndef _SKIP_MAP_SPHERICAL_

   if ( !(M->Flags & OBJ_HAS_UV) ) Mesh_Map_Spherical( Node );

#endif // _SKIP_MAP_SPHERICAL_

   if ( !(M->Flags & OBJ_HAS_POLY_UV) )
      Mesh_Store_UV_Mapping( M );    // to be done before the rest...

//   Mat = NULL;
   Mat = (MATERIAL *)_RCst_.The_World->Materials;
   for( i=0; i<M->Nb_Polys; ++i )
   {
      if ( M->Polys[i].Ptr == NULL )
      {
//         fprintf( stderr, "!! " );         // Big bug!!
         M->Polys[i].Ptr = (void *)Mat;
      }
      else Mat = (MATERIAL *)M->Polys[i].Ptr;
   }

#if 0       // Buggy

   if ( !(M->Flags & OBJ_DONT_OPTIMIZ) )  // do it before ->UV is freed
      if ( !(M->Flags & OBJ_SPLIT_OK) )
         Optimize_Split_Mesh( M,
            SPLIT_THRESH1*Node->Box.Radius, SPLIT_THRESH2 );

#endif


#ifndef _SKIP_OPTIM_MESH_

   if ( !(M->Flags & OBJ_DONT_OPTIMIZ) )
      if ( !(M->Flags & OBJ_OPTIM_OK) )
        Optimize_Mesh( M, OPTIM_THRESH*Node->Box.Radius );

#endif   // _SKIP_OPTIM_MESH_


#ifndef _SKIP_EDGES_

   if ( Mesh_Store_Edges( M )==NULL ) return( 0 );

   if ( !(M->Flags & OBJ_DONT_OPTIMIZ) )
      if ( !(M->Flags & OBJ_EDGES_OK) )
         Mesh_Detect_Sharp_Edges( M, EDGE_SHARPNESS );

#endif      // _SKIP_EDGES_

      // Mesh IDs

   for( i=0; i<M->Nb_Polys; ++i )
   {
      VECTOR C;
      INT j;

      j = M->Polys[i].Pt[0];
      C[0] = M->Vertex[j][0];
      C[1] = M->Vertex[j][1];
      C[2] = M->Vertex[j][2];
      M->Polys[i].N_Dot_C = Dot_Product( C, M->Polys[i].No );
      M->Polys[i].ID = i;      
   }

#ifdef USE_OLD_SHADOWZ
   if ( M->Shadow_ID != NULL )
      for( i=0; i<M->Nb_Vertex; ++i )
         M->Shadow_ID[i] = Vertex_ID++;
#endif

   M->ID = Node->ID;

      // Animation setup

   if ( (Node->Flags&OBJ_DO_ANIM) && !(Node->Flags&OBJ_TRACK_OK) )
   {
      Setup_Spline_3ds_Derivates( M->Pos_k );
      Setup_Spline_3ds_Derivates( M->Scale_k );
      Setup_Rot_Spline_3ds_Derivates( M->Rot_k );
      Setup_Spline_3ds_Derivates( M->Morph_k );
//      Setup_Spline_3ds_Derivates( M->Hide_k );   useless
      Node->Flags |= OBJ_TRACK_OK;
   }
   return( 0 );
}

static void Transform( OBJ_NODE *Node )
{
   STORE_TRANSFORM_MATRIX( Node );
}

static void Anim( OBJ_NODE *Node, FLT Frame )
{
   MESH *Mesh;

   Mesh = (MESH *)Node->Data;

#ifndef  _SKIP_HIDE_

   if ( Mesh->Hide_k!=NULL )
   {
      INT Hide;
      if ( Frame<Mesh->Hide_k->Keys[0].Frame )
         Hide = (INT)( Mesh->Hide_k->Keys[0].Values[0])^0x01;
      else Hide = Interpolate_Spline_3ds_Hidden( Frame + .01f, Mesh->Hide_k );
      if ( Hide ) Node->Flags &= ~OBJ_NOT_ACTIVE;
      else Node->Flags |= OBJ_NOT_ACTIVE;
   }

#endif   //  _SKIP_HIDE_

   if ( Node->Cur_Time==Frame ) return;

   if ( Mesh->Rot_k != NULL )
      Interpolate_Rot_Spline_3ds_Values( Frame, 
         Mesh->Rot_k, (FLT *)&Node->Transform.Rot.QRot );
   if ( Mesh->Pos_k != NULL )
      Interpolate_Spline_3ds_Values( Frame,
         Mesh->Pos_k, (FLT *)&Node->Transform.Pos );

#if 0
   fprintf( stderr, "Frame:%.2f  %.2f %.2f %.2f %.2f\n", Frame, 
         Node->Transform.Rot.QRot[0],
         Node->Transform.Rot.QRot[1],
         Node->Transform.Rot.QRot[2],
         Node->Transform.Rot.QRot[3] );
#endif

   if ( Mesh->Scale_k != NULL )
      Interpolate_Spline_3ds_Values( Frame, 
         Mesh->Scale_k, (FLT *)&Node->Transform.Scale );


#ifndef _SKIP_MORPH_

   if ( Mesh->Morph_Flags & MESH_DO_MORPH )
   {
      INT i, ID;
      FLT *Va, *Vb, *Na, *Nb, *V_Dst, *N_Dst;
      OBJ_NODE *Tmp;
      FLT t, u, v;

      Interpolate_Spline_3ds_Values( Frame, Mesh->Morph_k, &t );

      ID = (INT)floor( t );
      v = t-1.0f*ID; u = 1.0f-v;

      Tmp = Mesh->Morph_Node[ID];
      if ( Tmp != Node )
      {
         Va = (FLT*)((MESH*)Tmp->Data)->Vertex;
         Na = (FLT*)((MESH*)Tmp->Data)->Normals;
      }
      else
      {
         Va = (FLT*)Mesh->Vertex_Copy;
         Na = (FLT*)Mesh->Normals_Copy;
      };
      ID++;
      if ( ID==Mesh->Morph_k->Nb_Keys ) ID=0;
      Tmp = Mesh->Morph_Node[ID];
      if ( Tmp != Node )
      {
         Vb = (FLT*)((MESH*)Tmp->Data)->Vertex;
         Nb = (FLT*)((MESH*)Tmp->Data)->Normals;
      }
      else
      {
         Vb = (FLT*)Mesh->Vertex_Copy;
         Nb = (FLT*)Mesh->Normals_Copy;
      };
      V_Dst = (FLT*)Mesh->Vertex;
      N_Dst = (FLT*)Mesh->Normals;
      if ( Mesh->Morph_Flags & MESH_MORPH_NORMAL )
      {
         for( i=0; i<Mesh->Nb_Vertex*3; ++i )
         {
            V_Dst[i] = Va[i]*u + Vb[i]*v;
            N_Dst[i] = Na[i]*u + Nb[i]*v;
         }
      }
      else
      {
         for( i=0; i<Mesh->Nb_Vertex*3; ++i )
         {
            V_Dst[i] = Va[i]*u + Vb[i]*v;
         }
         Mesh_Store_Normals2( Mesh );
      }
      if ( Mesh->Morph_Flags & MESH_MORPH_AVERAGE_NORMAL )
         Average_Normals( Mesh );
      if ( Mesh->Morph_Flags & MESH_MORPH_SHARP_EDGES )
         Mesh_Detect_Sharp_Edges( Mesh, EDGE_SHARPNESS );      
      if ( Mesh->Flags & OBJ_USE_RAYTRACE )
         NODE_INIT_RAY_CONSTANTS( Node );
   }

#endif   // _SKIP_MORPH_

}

EXTERN void Mesh_Reset_Poly_Material( OBJ_NODE *Node )
{
   INT i;
   MESH *M;

   if ( Node->Type!=MESH_TYPE ) return;
   M = (MESH *)Node->Data;
   if ( M==NULL ) return;
   for( i=0; i<M->Nb_Polys; ++i ) M->Polys[i].Ptr = NULL;
}

/******************************************************************/

EXTERN NODE_METHODS _MESH_ =
{
   sizeof( MESH ),
   Init, Finish, Destroy,
   Transform, Anim, 
   Render_Mesh,
   NULL,    // Post
   NULL,    // Copy
   NULL, NULL, NULL, NULL, NULL     // reserved for ray-tracer
};

/******************************************************************/
/******************************************************************/

EXTERN INT Find_Max_Vertex( OBJECT *Cur )
{
   INT Max;
   
   Max = 0;
   for( ; Cur!=NULL; Cur=Cur->Next ) 
   {
      if ( !(Cur->Flags&OBJ_DONT_RENDER) )
         if ( Cur->Type == MESH_TYPE )
         {
            INT n;
            n = ((MESH*)Cur)->Nb_Vertex;
            if ( n>Max ) Max = n;
         }
   }
   return( Max );
}

/******************************************************************/

EXTERN OBJECT *Copy_Mesh( OBJECT *M )
{
   MESH *New, *Msh;

   if ( M==NULL || M->Type != MESH_TYPE ) return( NULL );
   Msh = (MESH *)M;

   New = Set_Mesh_Data( NULL, Msh->Nb_Vertex, Msh->Nb_Polys );
   if ( New==NULL ) return( NULL );

   if ( Msh->Vertex!=NULL )
      memcpy( New->Vertex, Msh->Vertex, Msh->Nb_Vertex * sizeof( VECTOR ) );
   if ( Msh->UV != NULL )
      memcpy( New->UV, Msh->UV, Msh->Nb_Vertex * sizeof( F_MAPPING ) );
   if ( Msh->Polys != NULL )
      memcpy( New->Polys, Msh->Polys, Msh->Nb_Polys * sizeof( POLY ) );

   Average_Normals( New );

#ifndef _SKIP_EDGES_

   if ( !(M->Flags & OBJ_DONT_OPTIMIZ) )
   {
      Mesh_Store_Edges( New );
      if ( !(Msh->Flags & OBJ_EDGES_OK) )
         Mesh_Detect_Sharp_Edges( New, EDGE_SHARPNESS );
   }

#endif   // _SKIP_EDGES_

      // TODO: Copy animation Splines!! */
   // New->Transform = Msh->Transform;
            /* ... */

      /* This obj needs being FINISH()'ed, too... */

   return( (OBJECT*)New );
}

/******************************************************************/

EXTERN void Sub_Vertex_Eq( MESH *M, VECTOR T )
{
   INT i;
   i = M->Nb_Vertex;
   while( --i>=0 ) Sub_Vector_Eq( M->Vertex[i], T );
}

EXTERN void Add_Vertex_Eq( MESH *M, VECTOR T )
{
   INT i;
   i = M->Nb_Vertex;
   while( --i>=0 ) Add_Vector_Eq( M->Vertex[i], T );
}

EXTERN void Matrix_Vertex_Eq( MESH *M, MATRIX T )
{
   INT i;
   for( i=0; i<M->Nb_Vertex; ++i )
   {
      nA_V_Eq( M->Vertex[i], T );
   }
}

/******************************************************************/

EXTERN POLY *Mesh_New_Polys( MESH *Obj, INT Nb_Polys )
{
   Obj->Polys = New_Fatal_Object( Nb_Polys, POLY );
   if ( Obj->Polys==NULL ) return( NULL );
   memset( Obj->Polys, 0, Nb_Polys*sizeof( POLY ) );
   Obj->Nb_Polys = Nb_Polys;
   return( Obj->Polys );
}

EXTERN VECTOR *Mesh_New_Vertex( MESH *Obj, INT Nb_Vtx )
{
   Obj->Vertex = New_Fatal_Object( 2*Nb_Vtx, VECTOR );
   if ( Obj->Vertex==NULL ) return( NULL );
   memset( Obj->Vertex, 0, 2*Nb_Vtx*sizeof( VECTOR ) );
   Obj->Normals = Obj->Vertex + Nb_Vtx;

   Obj->Nb_Vertex = Nb_Vtx;
   return( Obj->Vertex );
}

EXTERN MESH *Set_Mesh_Data( MESH *Obj, INT Nb_Vtx, INT Nb_Polys )
{
   INT i;

   if ( Obj==NULL ) 
   {
      Obj = (MESH *)New_Node_Data( &_MESH_ );
      if ( Obj==NULL ) return( NULL );
   }

   if ( (Nb_Vtx>0) && (Nb_Vtx!=Obj->Nb_Vertex) )
   {
      M_Free( Obj->Vertex );
      M_Free( Obj->UV );
      if ( Mesh_New_Vertex( Obj, Nb_Vtx ) ==NULL )
         return( NULL );
      Obj->UV = New_Fatal_Object( Nb_Vtx, F_MAPPING );
      if ( Obj->UV==NULL ) { M_Free( Obj->Vertex); return( NULL ); }
      memset( Obj->UV, 0, Nb_Vtx*sizeof( F_MAPPING ) );
   }

   if ( (Nb_Polys>0) && (Nb_Polys!=Obj->Nb_Polys) )
   {
      M_Free( Obj->Polys );
      if ( Mesh_New_Polys( Obj, Nb_Polys ) == NULL )
         return( NULL );
   }

         // Finish set up...

   for( i=0; i<Obj->Nb_Polys; ++i )
   {
      Obj->Polys[i].Ptr = NULL;
      Obj->Polys[i].ID = 0xFFFF;
   }
   if ( Obj->Edges != NULL )
   {
      M_Free( Obj->Edges );
      Obj->Nb_Edges = 0;
//      Mesh_Store_Edges( Obj );
   }
   return( Obj );
}

/******************************************************************/

EXTERN void Mesh_Store_Normals( MESH *M )
{
   POLY *P;
   INT i, I;

   P = M->Polys;
   for( i=0; i<M->Nb_Polys; )
   {
      FLT *V0, *V1, *V2;
      FLT dx1, dx2, dy1, dy2, dz1, dz2;
      FLT N;

      if (P->Pt[0]==P->Pt[1]) goto Trash;
      else V0 = (FLT*)M->Vertex[P->Pt[0]];
      if (P->Pt[1]==P->Pt[2]) goto Trash;
      else V1 = (FLT*)M->Vertex[P->Pt[1]];
      if (P->Pt[2]==P->Pt[0]) goto Trash;
      else V2 = (FLT*)M->Vertex[P->Pt[2]]; 
      dx1 = V1[0]-V0[0]; dx2 = V2[0]-V0[0];
      dy1 = V1[1]-V0[1]; dy2 = V2[1]-V0[1];
      dz1 = V1[2]-V0[2]; dz2 = V2[2]-V0[2];

      P->No[0] = dy1*dz2 - dy2*dz1; N  = P->No[0]*P->No[0];
      P->No[1] = dx2*dz1 - dx1*dz2; N += P->No[1]*P->No[1];
      P->No[2] = dx1*dy2 - dx2*dy1; N += P->No[2]*P->No[2];
      if ( N<=0.0 )
      {
Trash:
         M->Nb_Polys--;
         for( I=i; I<M->Nb_Polys; ++I )
            M->Polys[I] = M->Polys[I+1];
         continue;
      }
      N = 1.0/(FLT)sqrt( (double) N );
      P->No[0] *= N;
      P->No[1] *= N;
      P->No[2] *= N;
      P++;
      i++;
   }
   M->Flags = (OBJ_FLAG)( M->Flags | OBJ_HAS_NORMALS );
}

/******************************************************************/
/******************************************************************/
