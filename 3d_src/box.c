/***********************************************
 *                Boxes                        *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

EXTERN FLT Set_BBox_From_Min_Max( VECTOR Min, VECTOR Max )
{
   FLT Tmp, R=0.0;

   Tmp = Max[1]-Min[1]; 
   if ( Tmp<BOX_EPSILON ) R += BOX_EPSILON;
   else R += Tmp*Tmp;

   Tmp = Max[2]-Min[2]; 
   if ( Tmp<BOX_EPSILON ) R += BOX_EPSILON;
   else R += Tmp*Tmp;
   Tmp = Max[0] - Min[0]; 
   if ( Tmp<BOX_EPSILON ) R += BOX_EPSILON;
   else R += Tmp*Tmp;

   return( (FLT)sqrt(R) );
}

EXTERN void Set_Up_Box( VECTOR *Vertex, INT N, BBOX *B )
{
   INT i;
   FLT Mins[3], Maxs[3];    // THIS IS A HACK FOR SGI / CC7.x!
   Set_Vector( Mins, _HUGE_, _HUGE_, _HUGE_ );
   Set_Vector( Maxs, -_HUGE_, -_HUGE_, -_HUGE_ );

   for( i=N-1; i>=0; --i )
   {
      if ( Vertex[i][0]<Mins[0] ) Mins[0] = Vertex[i][0];
      if ( Vertex[i][1]<Mins[1] ) Mins[1] = Vertex[i][1];
      if ( Vertex[i][2]<Mins[2] ) Mins[2] = Vertex[i][2];

      if ( Vertex[i][0]>Maxs[0] ) Maxs[0] = Vertex[i][0];
      if ( Vertex[i][1]>Maxs[1] ) Maxs[1] = Vertex[i][1];
      if ( Vertex[i][2]>Maxs[2] ) Maxs[2] = Vertex[i][2];
   }

   B->Radius = Set_BBox_From_Min_Max( Mins, Maxs );   
   B->iZ    = 0.0;
   B->Mins[0] = Mins[0]; B->Mins[1] = Mins[1]; B->Mins[2] = Mins[2];
   B->Maxs[0] = Maxs[0]; B->Maxs[1] = Maxs[1]; B->Maxs[2] = Maxs[2];
}

EXTERN INT Insert_In_Upper_Box( BBOX *Up, BBOX *B )
{
   INT Flag;
   Flag = 0;
   if ( Up->Maxs[0]<B->Maxs[0] ) { Flag++; Up->Maxs[0] = B->Maxs[0]; }
   if ( Up->Maxs[1]<B->Maxs[1] ) { Flag++; Up->Maxs[1] = B->Maxs[1]; }
   if ( Up->Maxs[2]<B->Maxs[2] ) { Flag++; Up->Maxs[2] = B->Maxs[2]; }
   if ( Up->Mins[0]>B->Mins[0] ) { Flag++; Up->Mins[0] = B->Mins[0]; }
   if ( Up->Mins[1]>B->Mins[1] ) { Flag++; Up->Mins[1] = B->Mins[1]; }
   if ( Up->Mins[2]>B->Mins[2] ) { Flag++; Up->Mins[2] = B->Mins[2]; }

   if ( Flag )
      Up->Radius = Set_BBox_From_Min_Max( Up->Mins, Up->Maxs );
   return( Flag );
}

/******************************************************************/

#if 0
EXTERN BOX_TEST_RESULT Test_Box_Plane( PLANE Plane, BBOX *Box )
{
   FLT d,d2;
   INT Result, Tmp;  

   d2 = Box->Size[_X]*Plane[_X];
   d  = Box->Center[_X]*Plane[_X];
   d += Box->Center[_Y]*Plane[_Y];
   d += Box->Center[_Z]*Plane[_Z];

   Result = ( (d+d2)>=Plane[_D] );
   Tmp = ( (d-d2)>=Plane[_D] );
   if ( Tmp!=Result ) return( SPLIT );

   d2 = Box->Size[_Y]*Plane[_Y];
   Tmp = ( (d+d2)>=Plane[_D] );
   if ( Tmp!=Result ) return( SPLIT );
   Tmp = ( (d-d2)>=Plane[_D] );
   if ( Tmp!=Result ) return( SPLIT );

   d2 = Box->Size[_Z]*Plane[_Z];
   Tmp = ( (d+d2)>=Plane[_D] );
   if ( Tmp!=Result ) return( SPLIT );
   Tmp = ( (d-d2)>=Plane[_D] );
   if ( Tmp!=Result ) return( SPLIT );

   return( Result );
}
#endif


/******************************************************************/

#if 0

EXTERN INT Is_Box_Viewed_By_Cam( CAMERA *Cam, BBOX *B )
{
   FLT R2, R2_Inv, OM_dot_U;
   VECTOR T;

   T[0] = B->Center[0]-Cam->Pos[0];
   R2 = T[0]*T[0];
   T[1] = B->Center[1]-Cam->Pos[1];
   R2 += T[1]*T[1];
   T[2] = B->Center[2]-Cam->Pos[2];
   R2 += T[2]*T[2];
   if ( R2<Cam->D2 ) return( FALSE );
   R2_Inv = 1.0/R2;

      /* ... */
   B->Z_1 = sqrt( R2_Inv );
   return( TRUE );
}

#endif   // 0

/******************************************************************/

#if 0

EXTERN INT Is_Sphere_Viewed_By_Cam( CAMERA *Cam, BBOX *Box )
{
   FLT R2, R2_Inv;
   VECTOR T;
   FLT Xp, Yp;

   T[2] = Box->C_Proj[2]-Cam->Pos[2];
   R2 = T[2]*T[2];
   if ( R2<Cam->D2 ) return( FALSE );
   T[0] = Box->C_Proj[0]-Cam->Pos[0];
   R2 += T[0]*T[0];
   T[1] = Box->C_Proj[1]-Cam->Pos[1];
   R2 += T[1]*T[1];

   R2_Inv = 1.0 / sqrt( R2 );

   Box->Z_1 = R2_Inv;  // + 1/Box->Radius;

   Xp = fabs( T[0]*R2_Inv );  // if ( Xp<0.0 ) Xp = -Xp;
   Yp = fabs( T[1]*R2_Inv );  // if ( Yp<0.0 ) Yp = -Yp;

   R2_Inv *= 10.0*Box->Radius/Cam->D;

   if ( Xp>0.5+R2_Inv ) return( FALSE );
   else if ( Yp>_Alpha_ + R2_Inv ) return( FALSE );
   else return( TRUE );
}

#endif   // 0

/******************************************************************/

#if 0

EXTERN INT Is_Sphere_Viewed_By_Light( LIGHT *L, BBOX *Box )
{
   FLT R2, R2_Inv;
   VECTOR T;
   FLT Xp, Yp;

   T[2] = Box->C_Proj[2]-Light->Pos[2];
   R2 = T[2]*T[2];
   if ( R2<Cam->D2 ) return( FALSE );
   T[0] = Box->C_Proj[0]-Light->Pos[0];
   R2 += T[0]*T[0];
   T[1] = Box->C_Proj[1]-Light->Pos[1];
   R2 += T[1]*T[1];

   R2_Inv = 1.0 / sqrt( R2 );

               /* Choose Scale factor !!! */

   Box->Z_1 = R2_Inv;

   Xp = fabs( T[0]*R2_Inv );  // if ( Xp<0.0 ) Xp = -Xp;
   Yp = fabs( T[1]*R2_Inv );  // if ( Yp<0.0 ) Yp = -Yp;

   R2_Inv *= 10.0*Box->Radius/Cam->D;

   if ( Xp>0.5+R2_Inv ) return( FALSE );
   else if ( Yp>_Alpha_+R2_Inv ) return( FALSE );
   else return( TRUE );
}

#endif

/******************************************************************/

EXTERN void BBox_Planes_To_Points( VECTOR Pt[8], BBOX *Box )
{
   Pt[0][0] = Pt[1][0] = Pt[2][0] = Pt[3][0] = Box->Maxs[0];
   Pt[4][0] = Pt[5][0] = Pt[6][0] = Pt[7][0] = Box->Mins[0];
   Pt[0][1] = Pt[1][1] = Pt[4][1] = Pt[5][1] = Box->Maxs[1];
   Pt[2][1] = Pt[3][1] = Pt[6][1] = Pt[7][1] = Box->Mins[1];
   Pt[0][2] = Pt[2][2] = Pt[4][2] = Pt[6][2] = Box->Maxs[2];
   Pt[1][2] = Pt[3][2] = Pt[5][2] = Pt[7][2] = Box->Mins[2];
}

EXTERN void BBox_TPlanes_To_Points( VECTOR Pt[8], BBOX *Box )
{
   Pt[0][0] = Pt[1][0] = Pt[2][0] = Pt[3][0] = Box->TMaxs[0];
   Pt[4][0] = Pt[5][0] = Pt[6][0] = Pt[7][0] = Box->TMins[0];
   Pt[0][1] = Pt[1][1] = Pt[4][1] = Pt[5][1] = Box->TMaxs[1];
   Pt[2][1] = Pt[3][1] = Pt[6][1] = Pt[7][1] = Box->TMins[1];
   Pt[0][2] = Pt[2][2] = Pt[4][2] = Pt[6][2] = Box->TMaxs[2];
   Pt[1][2] = Pt[3][2] = Pt[5][2] = Pt[7][2] = Box->TMins[2];
}

EXTERN void BBox_Transform_Min_Max( BBOX *Box, MATRIX2 M )
{
   VECTOR Pt[8];
   INT i;

   BBox_Planes_To_Points( Pt, Box );
   for( i=0; i<8; ++i ) A_V_Eq( Pt[i], M );

   Set_Vector( Box->TMins, _HUGE_, _HUGE_, _HUGE_ );
   Set_Vector( Box->TMaxs, -_HUGE_, -_HUGE_, -_HUGE_ );

   for( i=0; i<8; ++i )
   {
      if ( Pt[i][0]<Box->TMins[0] ) Box->TMins[0] = Pt[i][0];
      if ( Pt[i][1]<Box->TMins[1] ) Box->TMins[1] = Pt[i][1];
      if ( Pt[i][2]<Box->TMins[2] ) Box->TMins[2] = Pt[i][2];

      if ( Pt[i][0]>Box->TMaxs[0] ) Box->TMaxs[0] = Pt[i][0];
      if ( Pt[i][1]>Box->TMaxs[1] ) Box->TMaxs[1] = Pt[i][1];
      if ( Pt[i][2]>Box->TMaxs[2] ) Box->TMaxs[2] = Pt[i][2];
   }
   Box->TRadius = Set_BBox_From_Min_Max( Box->TMins, Box->TMaxs );
   Box->TiZ = 0.0;
}

EXTERN FLT Project_Box( BBOX *Box, MATRIX2 M, FLT Clip[6] )
{
   INT i;
   VECTOR Pt[8];
   FLT Min, Max;

      // return values:
      // Key<0.0: we're inside the box.
      // Key==0.0: Box not visible
      // Key>0: Box visible.

#if 0
   FLT Tmp;
   Tmp = Box->Center[0] - M[18];
   if ( fabs(Tmp)>Box->Size[0] ) goto Skip;
   Tmp = Box->Center[1] - M[19];
   if ( fabs(Tmp)>Box->Size[1] ) goto Skip;
   Tmp = Box->Center[2] - M[20];
   if ( fabs(Tmp)<Box->Size[2] )
   {
      Clip[0] = _RCst_.Clips[0]; 
      Clip[1] = _RCst_.Clips[1];
      Clip[2] = _RCst_.Clips[2];
      Clip[3] = _RCst_.Clips[3];
      Clip[4] = _RCst_.Clips[4];
      Clip[5] = _HUGE_;
      // Min = Tmp-Box->Size[1];
      // Max = Tmp-Box->Size[2];
      Tmp = 0.0;  // -BOX_EPSILON;  // Box->Size[2]; // _RCst_.Clips[5];
      return( Tmp );   // inside the box. Negate the return value
   }

Skip:
#endif

   BBox_Planes_To_Points( Pt, Box );

   Clip[0] = _RCst_.Clips[1];
   Clip[1] = _RCst_.Clips[0];
   Clip[2] = _RCst_.Clips[3];
   Clip[3] = _RCst_.Clips[2];
   Clip[4] = _HUGE_;
   Clip[5] = 0.0;

   Min =  _HUGE_;
   Max = -_HUGE_;
   for( i=7; i>=0; --i )
   {
      FLT Inv_Z;
      FLT Xp, Yp;

      A_V_Eq( Pt[i], M );
      Inv_Z = 1.0f/Pt[i][2];
      if ( (Inv_Z<0.0) || (Inv_Z>_RCst_.Clips[5]) ) continue;
      // if ( Inv_Z<Tmp ) Tmp = Inv_Z;    // farthest point
      // if ( Inv_Z>Tmp ) Tmp = Inv_Z;    // nearest point
      if ( Inv_Z<Min ) Min = Inv_Z;    // farthest point
      if ( Inv_Z>Max ) Max = Inv_Z;    // nearest point
      if ( Inv_Z<Clip[4] ) Clip[4] = Inv_Z;
      if ( Inv_Z>Clip[5] ) Clip[5] = Inv_Z;
      Inv_Z *= _RCst_._Lx_;
      Xp = _RCst_._Cx_ + Pt[i][0]*Inv_Z;
      if ( Xp<Clip[0] ) Clip[0] = Xp;
      if ( Xp>Clip[1] ) Clip[1] = Xp;
      Yp = _RCst_._Cy_ - Pt[i][1]*Inv_Z;
      if ( Yp<Clip[2] ) Clip[2] = Yp;
      if ( Yp>Clip[3] ) Clip[3] = Yp;
   }
   if ( Clip[0]<_RCst_.Clips[0] ) Clip[0] = _RCst_.Clips[0];
   if ( Clip[1]>_RCst_.Clips[1] ) Clip[1] = _RCst_.Clips[1];
   if ( Clip[2]<_RCst_.Clips[2] ) Clip[2] = _RCst_.Clips[2];
   if ( Clip[3]>_RCst_.Clips[3] ) Clip[3] = _RCst_.Clips[3];

   if ( Min>=0.0 ) return( Min );
   if ( Max<=0.0 ) return( 0.0 );
   Clip[0] = _RCst_.Clips[0]; Clip[1] = _RCst_.Clips[1];
   Clip[2] = _RCst_.Clips[2]; Clip[3] = _RCst_.Clips[3];
   return( Max );
//   return( Min );
}

/******************************************************************/
