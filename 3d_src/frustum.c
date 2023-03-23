/***********************************************
 *              Frustums                       *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

   // Mo is Node->Mo and D is Cam->D's aperture

EXTERN void Camera_Frustum( FRUSTUM *Frustum, MATRIX2 Mo, 
   FLT Apert_X, FLT Apert_Y )
{
      //    camera's vector: 
      // Mo[6,7,8]    (dir)
      // Mo[0,1,2]    (right)
      // Mo[3,4,5]    (up)

   double a,b;
   VECTOR D;

   a = 1.0 / sqrt( Apert_Y*Apert_Y + Apert_X*Apert_X );

   b = a * Apert_X;
   D[0] = -b*Mo[6];
   D[1] = -b*Mo[7];
   D[2] = -b*Mo[8];
   Frustum->Left[0]  =-b*Mo[0] + D[0];
   Frustum->Right[0] = b*Mo[0] + D[0];
   Frustum->Left[1]  =-b*Mo[1] + D[1];
   Frustum->Right[1] = b*Mo[1] + D[1];
   Frustum->Left[2]  =-b*Mo[2] + D[2];
   Frustum->Right[2] = b*Mo[2] + D[2];
   
   b = a * Apert_Y;
   D[0] = -b*Mo[6];
   D[1] = -b*Mo[7];
   D[2] = -b*Mo[8];
   Frustum->Down[0]  =-b*Mo[3] + D[0];
   Frustum->Up[0]    = b*Mo[3] + D[0];
   Frustum->Down[1]  =-b*Mo[4] + D[1];
   Frustum->Up[1]    = b*Mo[4] + D[1];
   Frustum->Down[2]  =-b*Mo[5] + D[2];
   Frustum->Up[2]    = b*Mo[5] + D[2];

   Frustum->Left[3] = Frustum->Right[3] = 0.0;
   Frustum->Down[3] = Frustum->Up[3] = 0.0;
}

/******************************************************************/

EXTERN void Transform_Frustum( FRUSTUM *Out, FRUSTUM *In, MATRIX2 Mo )
{
   nA_V( Out->Left, Mo, In->Left );
   Out->Left[3] = Dot_Product( Out->Left, (FLT*)&Mo[9] );
   nA_V( Out->Right, Mo, In->Right );
   Out->Right[3] = Dot_Product( Out->Right, (FLT*)&Mo[9] );
   nA_V( Out->Up, Mo, In->Up );
   Out->Up[3] = Dot_Product( Out->Up, (FLT*)&Mo[9] );
   nA_V( Out->Down, Mo, In->Down );
   Out->Down[3] = Dot_Product( Out->Down, (FLT*)&Mo[9] );
   nA_V( Out->Front, Mo, In->Front );
   Out->Front[3] = Dot_Product( Out->Front, (FLT*)&Mo[9] );
   nA_V( Out->Back, Mo, In->Back );
   Out->Back[3] = Dot_Product( Out->Back, (FLT*)&Mo[9] );
}

/******************************************************************/

#define PLANE_EPSILON  0.0001

EXTERN void Camera_Frustum_II( FRUSTUM *Frustum, MATRIX2 Mo, FLT Apert_X, FLT Apert_Y )
{
      //    camera's vector: 
      // Mo[6,7,8]    (dir)
      // Mo[0,1,2]    (right)
      // Mo[3,4,5]    (up)

   double s, c;   
   VECTOR D;

   c = cos( Apert_X ); s = sin( Apert_X );
   D[0] = c; D[1] = 0.0; D[2] = s;
   nA_Inv_V( Frustum->Left, Mo, D );
   Frustum->Left[3] = Dot_Product( Frustum->Left, MATRIX_INV_TRANSL(Mo) ) + PLANE_EPSILON;

   D[0] = -c;
   nA_Inv_V( Frustum->Right, Mo, D );
   Frustum->Right[3] = Dot_Product( Frustum->Right, MATRIX_INV_TRANSL(Mo) ) + PLANE_EPSILON;

   c = cos( Apert_Y ); s = sin( Apert_Y );
   D[0] = 0.0; D[1] = c; D[2] = s;
   nA_Inv_V( Frustum->Down, Mo, D );
   Frustum->Down[3] = Dot_Product( Frustum->Down, MATRIX_INV_TRANSL(Mo) ) + PLANE_EPSILON;
   D[1] = -c;
   nA_Inv_V( Frustum->Up, Mo, D );
   Frustum->Up[3] = Dot_Product( Frustum->Up, MATRIX_INV_TRANSL(Mo) ) + PLANE_EPSILON;

   Set_Vector( D, 0.0, 0.0, 1.0 );
   nA_Inv_V( Frustum->Back, Mo, D );
   Frustum->Back[3] = Dot_Product( Frustum->Back, MATRIX_INV_TRANSL(Mo) ) + PLANE_EPSILON;
   Frustum->Back[3] += _RCst_.Clips[4];

   Set_Vector( D, 0.0, 0.0, -1.0 );
   nA_Inv_V( Frustum->Front, Mo, D );
   Frustum->Front[3] = Dot_Product( Frustum->Front, MATRIX_INV_TRANSL(Mo) ) - PLANE_EPSILON;
   Frustum->Front[3] -= _RCst_.Clips[6];
}

EXTERN void Camera_Frustum_II_Direct( FRUSTUM *Frustum, FLT Apert_X, FLT Apert_Y )
{
   double s, c;   

   c = cos( Apert_X ); s = sin( Apert_X );
   Set_Vector( Frustum->Left, c, 0.0, s );
   Frustum->Left[3] = PLANE_EPSILON;

   Set_Vector( Frustum->Right, -c, 0.0, s );
   Frustum->Right[3] = PLANE_EPSILON;

   c = cos( Apert_Y ); s = sin( Apert_Y );
   Set_Vector( Frustum->Down, 0.0, c, s );
   Frustum->Down[3] = PLANE_EPSILON;

   Set_Vector( Frustum->Up, 0.0, -c, s );
   Frustum->Up[3] = PLANE_EPSILON;

   Set_Vector( Frustum->Back, 0.0, 0.0, 1.0 );
   Frustum->Back[3] = _RCst_.Clips[4];

   Set_Vector( Frustum->Front, 0.0, 0.0, -1.0 );
   Frustum->Front[3] = -_RCst_.Clips[6];
}

/******************************************************************/

EXTERN INT Poly_Clip_Plane( POLY_3D *Out, PLANE Plane, POLY_3D *In )
{
   INT  i, k, Next_Vtx;
   INT Flag, Next_Flag;
   FLT  Dot, Next_Dot;

   Dot = Dot_Product( In->Vtx[0], Plane );
   Flag = ( Dot>=Plane[3] );

   k = 0;     // out vertex...
   Next_Vtx = 0;
   for( i=0; i<In->Nb_Vtx; i++ )
   {
      Next_Vtx++;
      if ( Next_Vtx==In->Nb_Vtx ) Next_Vtx = 0;

      if ( Flag ) { 
         memcpy( Out->Vtx[k], In->Vtx[i], sizeof( VTX_3D ) );
         k++;
      }

      Next_Dot = Dot_Product( In->Vtx[Next_Vtx], Plane );
      Next_Flag = ( Next_Dot>=Plane[3] );

      if ( Flag!=Next_Flag )
      {
         FLT x, y;
         INT j;
         x = ( Plane[3]-Dot ) / ( Next_Dot-Dot );
         y = 1.0-x;
         for( j=0; j< sizeof(VTX_3D)/sizeof(FLT); ++j )
            Out->Vtx[k][j] = In->Vtx[i][j]*y + In->Vtx[Next_Vtx][j]*x;
         k++;
      }
      Dot = Next_Dot;
      Flag = Next_Flag;
   }
   Out->Nb_Vtx = k;
   Out->Nb_Fields = In->Nb_Fields;
   if ( k<3 ) return( 0 );
   return( 1 );
}

EXTERN INT Frustum_Clip( POLY_3D *Out, POLY_3D *In, FRUSTUM *F )
{
   POLY_3D P1, P2;

   if ( !Poly_Clip_Plane( &P1, F->Left, In ) ) return( 0 );
   if ( !Poly_Clip_Plane( &P2, F->Right, &P1 ) ) return( 0 );
   if ( !Poly_Clip_Plane( &P1, F->Up, &P2 ) ) return( 0 );

#if 1          // additional back/front clipping
   if ( !Poly_Clip_Plane( &P2, F->Front, &P1 ) ) return( 0 );
   if ( !Poly_Clip_Plane( &P1, F->Back, &P2 ) ) return( 0 );
#endif

   if ( !Poly_Clip_Plane( Out, F->Down, &P1 ) ) return( 0 );
   return( 1 );
}

EXTERN INT Frustum_Clip_Direct( POLY_3D *Out, POLY_3D *In, OBJ_NODE *Node )
{
   CAMERA *Cam = (CAMERA*)Node->Data;
   FRUSTUM Frustum;

   Camera_Frustum_II_Direct( &Frustum, Cam->Apert_X, Cam->Apert_Y );
   return( Frustum_Clip( Out, In, &Frustum ) );
}

/******************************************************************/
