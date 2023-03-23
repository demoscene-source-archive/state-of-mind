/***********************************************
 *          Poly handling                      *
 * Skal 99                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

EXTERN void Project_Poly_2D( POLY_2D *Poly2D, POLY_3D *Poly3D, 
   OBJ_NODE *Cam_Node )
{
   INT i;
   CAMERA *Cam;

   Cam = (CAMERA*)Cam_Node->Data;

         // Warning! this replace Poly3D's vertex (in world space)
         // by the vertex in camera's space...

   for ( i=0; i<Poly3D->Nb_Vtx; i++ )
   {
      FLT  w, x, y;

      A_V_Eq( Poly3D->Vtx[i], Cam_Node->Mo );   // Warning!

      w = Cam->Lx / Poly3D->Vtx[i][2];

      x = Cam->Cx + Poly3D->Vtx[i][0] * w;
      if ( x<_RCst_.Clips[0] ) x = _RCst_.Clips[0];
      else if ( x>=_RCst_.Clips[1] ) x = _RCst_.Clips[1]-1.0;
      Poly2D->Pts[i][0] = x;

      y = Cam->Cy - Poly3D->Vtx[i][1] * w;
      if ( y<_RCst_.Clips[2] ) y = _RCst_.Clips[2];
      else if ( y>=_RCst_.Clips[3] ) y = _RCst_.Clips[3]-1.0;
      Poly2D->Pts[i][1] = y;
      Poly2D->Index[i] = &Poly2D->Pts[i];
   }

   Poly2D->Index[i] = Poly2D->Index[0];  // close points cycle
   Poly2D->Index[i+1] = NULL;             // mark end
}

EXTERN void Project_Poly_2D_Direct( POLY_2D *Poly2D, POLY_3D *Poly3D, 
   OBJ_NODE *Cam_Node )
{
   INT i;
   CAMERA *Cam;

   Cam = (CAMERA*)Cam_Node->Data;

   for ( i=0; i<Poly3D->Nb_Vtx; i++ )
   {
      FLT  w, x, y;

      w = Cam->Lx / Poly3D->Vtx[i][2];

      x = Cam->Cx + Poly3D->Vtx[i][0] * w;
      if ( x<_RCst_.Clips[0] ) x = _RCst_.Clips[0];
      else if ( x>=_RCst_.Clips[1] ) x = _RCst_.Clips[1]-1.0;
      Poly2D->Pts[i][0] = x;

      y = Cam->Cy - Poly3D->Vtx[i][1] * w;
      if ( y<_RCst_.Clips[2] ) y = _RCst_.Clips[2];
      else if ( y>=_RCst_.Clips[3] ) y = _RCst_.Clips[3]-1.0;
      Poly2D->Pts[i][1] = y;
      Poly2D->Index[i] = &Poly2D->Pts[i];
   }

   Poly2D->Index[i] = Poly2D->Index[0];  // close points cycle
   Poly2D->Index[i+1] = NULL;             // mark end
}

EXTERN INT Poly_Backface( VECTOR Vertex, PLANE Plane, VECTOR Where )
{
   VECTOR Tmp;
   Sub_Vector( Tmp, Vertex, Where );
   return( Dot_Product( Plane, Tmp ) < B_CULL_LIMIT );
}

/******************************************************************/

EXTERN void Compute_Gradients( SURFACE *Surf, POLY_3D *Poly )
{
   VECTOR Q0, Q1, Q2;
   VECTOR Df;
   float N2;

        // Store Qi = Pj^Pk

   Cross_Product( Q0, Poly->Vtx[1], Poly->Vtx[2] );
   Cross_Product( Q1, Poly->Vtx[2], Poly->Vtx[0] );
   Cross_Product( Q2, Poly->Vtx[0], Poly->Vtx[1] );

        // the 'normal'

   Surf->Tdw[0]  = Q0[0] + Q1[0] + Q2[0];
   Surf->Tdw[1]  = Q0[1] + Q1[1] + Q2[1];
   Surf->Tdw[2]  = Q0[2] + Q1[2] + Q2[2];

        // Mixed product

   Surf->Area = Dot_Product( Poly->Vtx[0], Q0 );    

        // gradients: N^Y
        // with Y = (f1-f2).P0 + (f2-f0).P1 + (f0-f1).P2

        // we could throw away the original .U and .V field and replace them
        // by the gradients, but better not obfuscate the code too much...

   Scale_Vector( Df, Poly->Vtx[0], Poly->Vtx[1][3] - Poly->Vtx[2][3] );
   Add_Scaled_Vector_Eq( Df, Poly->Vtx[1], Poly->Vtx[2][3] - Poly->Vtx[0][3] );
   Add_Scaled_Vector_Eq( Df, Poly->Vtx[2], Poly->Vtx[0][3] - Poly->Vtx[1][3] );
   Cross_Product( Surf->TdUw, Surf->Tdw, Df );

   Scale_Vector( Df, Poly->Vtx[0], Poly->Vtx[1][4] - Poly->Vtx[2][4] );
   Add_Scaled_Vector_Eq( Df, Poly->Vtx[1], Poly->Vtx[2][4] - Poly->Vtx[0][4] );
   Add_Scaled_Vector_Eq( Df, Poly->Vtx[2], Poly->Vtx[0][4] - Poly->Vtx[1][4] );
   Cross_Product( Surf->TdVw, Surf->Tdw, Df );

   N2 = 1.0f/Norm_Squared( Surf->Tdw );
   Scale_Vector_Eq( Surf->TdUw, N2 );
   Scale_Vector_Eq( Surf->TdVw, N2 );

      // If you want to have a real-normalized normal
      // stored in dw, just uncomment these two lines

   Scale_Vector_Eq( Surf->Tdw, sqrt(N2) );
   Surf->Area *= sqrt(N2);

}

/******************************************************************/

static FLT Mip_Scl[9] =
{ 1/256.0, 1/128.0, 1/64.0, 1/32.0, 1/16.0, 1/8.0, 1/4.0, 1/2.0, 1.0 };
static INT Mip_Off[3][8] = { 
  { 1, 2, 4, 8, 16, 32, 64, 128 },
  { 1*256, 2*256, 4*256, 8*256, 16*256, 32*256, 32*256, 128*256 },
  { 1*256+1, 2*256+2, 4*256+4, 8*256+8, 16*256+16, 32*256+32, 64*256+64, 128*256+128 }
};

EXTERN void Mesh_To_Edges( OBJ_NODE *Mesh, OBJ_NODE *Cam_Node )
{
   INT i, j, Mip;
   MESH *Cur_Msh;
   CAMERA *Cam;

   Cur_Msh = (MESH*)Mesh->Data;
   Cam = (CAMERA*)Cam_Node->Data;

   memset( Vertex_State, 0, Cur_Msh->Nb_Vertex*sizeof(BYTE) );

   Orig_Poly = Cur_Msh->Polys;
   Cur_P_Poly = Poly_To_Sort;

   for( i=0; i<Cur_Msh->Nb_Polys; i++, Orig_Poly++, Cur_P_Poly++ )
   {
      POLY_3D   Poly0, Poly1;
      POLY_2D   Poly2D;
      SURFACE *Surf;
      FLT wo;

         // check visibility from view point

      wo = Cur_Msh->Area[i] - Dot_Product( &Mesh->M[21], Cur_Msh->dw[i] );
      if ( wo>=0.0 ) continue;
      wo = 1.0 / wo;


         // transform poly in real space

      Poly0.Nb_Vtx = Orig_Poly->Nb_Pts;
      for( j=0; j<Poly0.Nb_Vtx; j++ )
      {
         INT K;
         K = Orig_Poly->Pt[j];
         if ( !Vertex_State[K] )
         {
            A_V( P_Vertex[K].N, Mesh->Mo, (FLT *)&Cur_Msh->Vertex[K] );
            Vertex_State[K] = 0x01;    // Pos
         }
         Assign_Vector( Poly0.Vtx[j], P_Vertex[ K ].N );
         Poly0.Vtx[j][3] = 1.0f*Orig_Poly->UV[j][0];
         Poly0.Vtx[j][4] = 1.0f*Orig_Poly->UV[j][1];
         Poly0.Nb_Fields = 1;
      }
      nA_V( Cur_P_Poly->N, Mesh->Mo, Orig_Poly->No );

          // Compute gradients with original poly

//      if ( !Poly_Backface( Poly0.Vtx[0], Cur_P_Poly->N, MATRIX_INV_TRANSL(Cam_Node->Mo) ) )
//         continue;

         // clip against frustum

      if ( !Frustum_Clip( &Poly1, &Poly0, &Cam->Frustum ) ) continue;


         // transform in view space and project on screen
         // WARNING!! Poly1's vertex will be in-place transformed
         // from object's space to camera's space..

      Project_Poly_2D( &Poly2D, &Poly1, Cam_Node );

         // rasterize and store edges. Allocate surface.

      Surf = Poly2D_To_Edges( Poly2D.Index );

//      Mip = (INT)( sqrt( wo )*7.0 );
//      if ( Mip>=8 ) Mip = 7;
//      else if ( Mip<3 ) Mip = 3;

      Mip = 7;
      Surf->Scale = Mip_Scl[ Mip ];
      Cur_Material = (MATERIAL*)Orig_Poly->Ptr;
      if ( Cur_Material != NULL && Cur_Material->Txt1!=NULL )
      {
         Surf->Texture = (Cur_Material->Txt1)->Ptr;
         Surf->Texture = (void*)((ADDR)Surf->Texture 
            + Mip_Off[ Cur_Material->Txt1->Slot ][ Mip ] );
      }
      else Surf->Texture = NULL;
      
      Scale_Vector( Surf->Tdw, Cur_Msh->dw[i], wo );
      nA_V_Eq( Surf->Tdw, Mesh->M );

      Surf->w0   = -Cam->Cx * Surf->Tdw[0];
      Surf->w0  +=  Cam->Cy * Surf->Tdw[1];
      Surf->w0  +=  Cam->Lx * Surf->Tdw[2];

      wo = Dot_Product( &Mesh->M[21], Cur_Msh->dUw[i] );
      nA_V( Surf->TdUw, Mesh->M, Cur_Msh->dUw[i] );
      Add_Scaled_Vector_Eq( Surf->TdUw, Surf->Tdw, wo );

      wo = Dot_Product( &Mesh->M[21], Cur_Msh->dVw[i] );
      nA_V( Surf->TdVw, Mesh->M, Cur_Msh->dVw[i] );
      Add_Scaled_Vector_Eq( Surf->TdVw, Surf->Tdw, wo );

      Surf->uw0  = -Cam->Cx * Surf->TdUw[0];
      Surf->uw0 +=  Cam->Cy * Surf->TdUw[1];
      Surf->uw0 +=  Cam->Lx * Surf->TdUw[2];
      Surf->vw0  = -Cam->Cx * Surf->TdVw[0];
      Surf->vw0 +=  Cam->Cy * Surf->TdVw[1];
      Surf->vw0 +=  Cam->Lx * Surf->TdVw[2];

         // Time to initialize associated surface

      Surf->Color = Orig_Poly->Colors;
      Surf->Draw_Span = ( void (*)() )Cur_Msh->Shader;   // Draw_Span;
   }
}

/******************************************************************/

EXTERN void Store_Poly_Gradients( MESH *Msh )
{
   INT i;

   M_Free( Msh->dUw );
   M_Free( Msh->dVw );
   M_Free( Msh->dw );
   M_Free( Msh->Area );
   Msh->dUw  = New_Fatal_Object( Msh->Nb_Polys, VECTOR );
   Msh->dVw  = New_Fatal_Object( Msh->Nb_Polys, VECTOR );
   Msh->dw   = New_Fatal_Object( Msh->Nb_Polys, VECTOR );
   Msh->Area = New_Fatal_Object( Msh->Nb_Polys, FLT );

   for( i=0; i<Msh->Nb_Polys; ++i )
   {
      POLY *Poly;
      INT p0, p1, p2;      
      VECTOR Q0, Q1, Q2;
      VECTOR Df;
      float N2;

      Poly = &Msh->Polys[i];
      p0 = Poly->Pt[0];
      p1 = Poly->Pt[1];
      p2 = Poly->Pt[2];

           // Store Qi = Pj^Pk

      Cross_Product( Q0, Msh->Vertex[p1], Msh->Vertex[p2] );
      Cross_Product( Q1, Msh->Vertex[p2], Msh->Vertex[p0] );
      Cross_Product( Q2, Msh->Vertex[p0], Msh->Vertex[p1] );

           // the 'normal'

      Msh->dw[i][0]  = Q0[0] + Q1[0] + Q2[0];
      Msh->dw[i][1]  = Q0[1] + Q1[1] + Q2[1];
      Msh->dw[i][2]  = Q0[2] + Q1[2] + Q2[2];

           // Mixed product

      Msh->Area[i] = Dot_Product( Msh->Vertex[p0], Q0 );    

           // gradients: N^Df
           // with Df = (f1-f2).P0 + (f2-f0).P1 + (f0-f1).P2

           // we could throw away the original .U and .V field and replace them
           // by the gradients, but better not obfuscate the code too much...

      Scale_Vector( Df, Msh->Vertex[p0], Poly->UV[1][0] - Poly->UV[2][0] );
      Add_Scaled_Vector_Eq( Df, Msh->Vertex[p1], Poly->UV[2][0] - Poly->UV[0][0] );
      Add_Scaled_Vector_Eq( Df, Msh->Vertex[p2], Poly->UV[0][0] - Poly->UV[1][0] );
      Cross_Product( Msh->dUw[i], Msh->dw[i], Df );

      Scale_Vector( Df, Msh->Vertex[p0], Poly->UV[1][1] - Poly->UV[2][1] );
      Add_Scaled_Vector_Eq( Df, Msh->Vertex[p1], Poly->UV[2][1] - Poly->UV[0][1] );
      Add_Scaled_Vector_Eq( Df, Msh->Vertex[p2], Poly->UV[0][1] - Poly->UV[1][1] );
      Cross_Product( Msh->dVw[i], Msh->dw[i], Df );

      N2 = 1.0f/Norm_Squared( Msh->dw[i] );
      Scale_Vector_Eq( Msh->dUw[i], N2 );
      Scale_Vector_Eq( Msh->dVw[i], N2 );

         // If you want to have a real-normalized normal
         // stored in dw, just uncomment these two lines

      Scale_Vector_Eq( Msh->dw[i], sqrt(N2) );
      Msh->Area[i] *= sqrt(N2);
   }
}

/******************************************************************/
