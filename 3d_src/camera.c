/***********************************************
 *              Camera                         *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

static void Init( OBJECT *Obj )
{
   CAMERA *Cam;

   Cam = (CAMERA *)Obj;

   Cam->Lx = 1.0;
   Cam->Ly = 1.0;
   Cam->Lens = 1.0;  // 18.0;      // (Fov=90deg)
   Cam->D = 5.0;
//   Cam->D2 = Cam->D*Cam->D;
   Cam->Cam_Type = CAM_NOT_SET;
   Set_Vector( Cam->Pos, 0.0, 0.0, 0.0 );
   Set_Vector( Cam->Target, 0.0, 0.0, 1.0 );
   Cam->Bank = 0.0;
   Cam->Vista = NULL;

   Cam->Dummy = NULL;
   Cam->Pos_k = Cam->Target_k = Cam->FOV_k = Cam->Roll_k = NULL;
   Cam->Type = CAMERA_TYPE;

   Cam->Flags = (OBJ_FLAG)( Cam->Flags | OBJ_DONT_RENDER );
}

static void Transform( OBJ_NODE *Node )
{
   CAMERA *Cam;
   Cam = (CAMERA *)Node->Data;
   Camera_Matrix( Cam->Pos, Cam->Target, Cam->Bank, Node->Mo );
}

static INT Finish( OBJ_NODE *Node )
{
   CAMERA *Cam;

   Cam = (CAMERA *)Node->Data;
   if ( Cam==NULL ) return( 0 );
   
   if ( (Node->Flags & OBJ_DO_ANIM) && !(Node->Flags&OBJ_TRACK_OK) )
   {
      Setup_Spline_3ds_Derivates( Cam->Pos_k );
      Setup_Spline_3ds_Derivates( Cam->Target_k );
      Setup_Spline_3ds_Derivates( Cam->FOV_k );
      Setup_Spline_3ds_Derivates( Cam->Roll_k );
      Node->Flags |= OBJ_TRACK_OK;
   }
   return( 0 );
}

static void Destroy( OBJECT *Dum )
{
   CAMERA *Cam;

   Cam = (CAMERA *)Dum;

   Destroy_Spline_3ds( &Cam->Pos_k );
   Destroy_Spline_3ds( &Cam->Target_k );
   Destroy_Spline_3ds( &Cam->FOV_k );
   Destroy_Spline_3ds( &Cam->Roll_k );
#ifndef _SKIP_VISTA_
   if ( Cur_Vista==Cam->Vista ) Cur_Vista = NULL;
   Destroy_Vista( Cam->Vista );
#endif
   Cam->Flags = (OBJ_FLAG)( Cam->Flags & ~OBJ_TRACK_OK );
}

static void Anim( OBJ_NODE *Node, FLT Frame )
{
   CAMERA *Cam;
   FLT Fov;

//   static FLT t = 0.0;
//   Frame = WORLD_SET_FRAME( _RCst_.The_World, t ); t = fmod( t+0.003, 1.0 );
   // Frame = _RCst_.The_World->Time;

   if ( Node->Cur_Time==Frame ) return;

   Cam = (CAMERA *)Node->Data;

   Interpolate_Spline_3ds_Values( Frame, Cam->Pos_k, (FLT *)&Cam->Pos );

   if ( Cam->Dummy!=NULL )       // check dummy's transform??
      Interpolate_Spline_3ds_Values( 
         Frame, ((OBJ_DUMMY*)Cam->Dummy->Data)->Pos_k, (FLT *)&Cam->Target );
   else Interpolate_Spline_3ds_Values( 
      Frame, Cam->Target_k, (FLT *)&Cam->Target );

   if ( Cam->Roll_k != NULL )
      Interpolate_Spline_3ds_Values( Frame, Cam->Roll_k, &Cam->Bank );
   if ( Cam->FOV_k != NULL )
      Interpolate_Spline_3ds_Values( Frame, Cam->FOV_k, &Fov );

   Cam->D = Cam->Lens*tan( (double)Fov/2.0 );   // 18.0: 3ds's magik constant ?? 
   _RCst_._Lx_ = 0.5f * _RCst_.Pix_Width * Cam->D;
//   Cam->D2 = Cam->D*Cam->D;

   // fprintf( stderr, "Tgt:%f %f %f  ", Cam->Target[0], Cam->Target[1], Cam->Target[2] );
   // fprintf( stderr, "Pos:%f %f %f  ", Cam->Pos[0], Cam->Pos[1], Cam->Pos[2] );
   // fprintf( stderr, "Roll: %f, FOV: %f\n", Cam->Bank, Fov );
}

EXTERN NODE_METHODS _CAMERA_ =
{
   sizeof( CAMERA ),
   Init, Finish, Destroy,
   Transform, Anim, 
   NULL,    // Render
   NULL,    // Post
   NULL,    // Copy
   NULL, NULL, NULL, NULL, NULL
};

/******************************************************************/
/******************************************************************/

EXTERN void Select_Camera( CAMERA *Cam )
{
//   Cam->D2 = Cam->D*Cam->D;

#if 0
      /* TODO: Remove !!! */
   _RCst_.Clips[0] = (FLT)( 2 );
   _RCst_.Clips[1] = (FLT)( _RCst_.Pix_Width-2 );
   _RCst_.Clips[2] = (FLT)( 2 );
   _RCst_.Clips[3] = (FLT)( _RCst_.Pix_Height-2 );

   _RCst_._Lx_ = 0.5 * _RCst_.Pix_Width;
   _RCst_._Ly_ = 0.5 * _RCst_.Pix_Height;
   _RCst_._Cx_ = _RCst_._Lx_; 
   _RCst_._Cy_ = _RCst_._Ly_; // Center

   _RCst_._Lx_ *= Cam->D;

   _RCst_.Out_Clips[0] = -4.0*_RCst_._Lx_;
   _RCst_.Out_Clips[1] = 4.0*_RCst_._Lx_;
   _RCst_.Out_Clips[2] = -4.0*_RCst_._Ly_;
   _RCst_.Out_Clips[3] = 4.0*_RCst_._Ly_;
   _RCst_.Out_Clips[4] = -Cam->D * 10.0;	// TODO: debug =>Remove
   _RCst_.Out_Clips[5] = _HUGE_;
//   _RCst_.Clips[4] = Cam->D;
   _RCst_.Clips[5] = 1.0/_RCst_.Clips[4];
   _RCst_.Clips[6] = 200000.0;
   _RCst_.Clips[7] = 1.0/_RCst_.Clips[6];
#endif
}

/******************************************************************/

EXTERN void Sort_Objects( OBJ_NODE *Cur )
{
   for( ; Cur!=NULL; Cur=Cur->Next )
   {
      FLT Key;

      if ( Cur->Flags & OBJ_DONT_RENDER ) continue;
      if ( Cur->Type==CAMERA_TYPE ) continue;
      if ( Cur->Type==MATERIAL_TYPE ) continue;
      Check_Node_Transform( Cur );        // Check hierarchy transform for Current Time

         // Transform object in camera's space
         // Result for this cam is in Cur->M

      STORE_NODE_MATRIX( Cur, _RCst_.The_Camera ); // <= Cur->M = Cam->Mo.Cur->Mo

            // Box visibility test

      {
         FLT Clip[6];
         Key = Project_Box( &Cur->Box, Cur->M, Clip );

// fprintf( stderr, "Box size: %f %f %f  Key=%f\n", 
//   Cur_Node->Box.Size[0], Cur_Node->Box.Size[1], Cur_Node->Box.Size[2], Key );
// if ( Cur_Node->Flags & OBJ_DRAW_BOX ) Draw_Box( &Cur_Node->Box, M );

 //      if ( Key<=0.0 ) { Key = -Key; goto Inside_Box; } // inside box.
         if ( Key==0.0 ) continue;
      // if ( (Key<_RCst_.Clips[4])||(Key>_RCst_.Clips[5]) ) continue;
         if ( Key<1.0/_RCst_.Out_Clips[4] ) continue;
         if ( Clip[1]<_RCst_.Clips[0] ) continue;
         if ( Clip[0]>_RCst_.Clips[1] ) continue;
         if ( Clip[1]-Clip[0] < 1.0 ) continue;
         if ( Clip[3]<_RCst_.Clips[2] ) continue;
         if ( Clip[2]>_RCst_.Clips[3] ) continue;
         if ( Clip[3]-Clip[2] < 1.0 ) continue;
      }

// Inside_Box:

            // Store parent parent anyway. 
            // Warning: it can be a GROUP or a LIGHT or...!!

      if ( Cur->Flags&OBJ_NOT_ACTIVE ) goto Skip;
      if ( Cur->Methods->Render!=NULL )
      {
         UINT Key2;
         Key *= 65536.0; 
         if ( Key>65535.0 ) Key = 65535.0;
         else if ( Key<0.0 ) Key = 0.0;

            /* Choose Scale factor !!! */
         Key2 = (UINT)Key;
         Key2 |= Nb_Obj_To_Sort<<16;

         Sorted[Nb_Sorted++] = Key2;
         Obj_To_Sort[ Nb_Obj_To_Sort++ ] = (void *)Cur;
      }
Skip:
      if ( Cur->Child!=NULL ) Sort_Objects( Cur->Child );    // recurse
   }
}

/******************************************************************/

   // The_Camera->Mo must be set before getting here...

EXTERN void Render_Camera( OBJ_NODE *Root )  
{
   INT Nb;

         // Classify boxes or whatever..

   Nb_Sorted = 0;
   Nb_Obj_To_Sort = 0;

   Renderer.Sort_Objects( Root );
   if ( !Nb_Sorted ) return;
   Counting_Sort( Sorted, Nb_Sorted );

         // Render every objects

   for( Nb=0; Nb<Nb_Sorted; ++Nb )
   {
      Cur_Node = (OBJ_NODE *)Obj_To_Sort[ Sorted[Nb]>>16 ];
      Cur_Obj = (OBJECT *)Cur_Node->Data;

      NODE_RENDER( Cur_Node, Cur_Node->M );
   }

// fprintf( stderr, "Box size: %f %f %f\n", Cur_Node->Box.Size[0], Cur_Node->Box.Size[1], Cur_Node->Box.Size[2] );
// if ( Cur_Node->Flags & OBJ_DRAW_BOX ) Draw_Box( &Cur_Node->Box, M );
}

/******************************************************************/

static void Camera_Matrix_Set_Position( VECTOR Pos, MATRIX2 M )
{
   MATRIX_INV_TRANSL(M)[0] =  Pos[0];
   MATRIX_TRANSL(M)[0]     = -Pos[0];
   MATRIX_INV_TRANSL(M)[1] =  Pos[1];
   MATRIX_TRANSL(M)[1]     = -Pos[1];
   MATRIX_INV_TRANSL(M)[2] =  Pos[2];
   MATRIX_TRANSL(M)[2]     = -Pos[2];
   nA_V_Eq( MATRIX_TRANSL(M), M );
}

EXTERN void Camera_Matrix( VECTOR Pos, VECTOR Tgt, FLT Omega, MATRIX2 M )
{
   double Theta, Phi;
   double Cx, Sx, Cz, Sz, Cy, Sy;
   double A, B;
   VECTOR D;

   D[0] = Tgt[0] - Pos[0]; A  = D[0]*D[0];
   D[1] = Tgt[1] - Pos[1]; A += D[1]*D[1];
   D[2] = Tgt[2] - Pos[2];

   if ( A<0.00001 )
   {
      Id_Matrix( M );
      M[18] = -Pos[0]; M[21] = Pos[0];
      M[19] = -Pos[1]; M[22] = Pos[1];
      M[20] = -Pos[2]; M[23] = Pos[2];
      return;
   }

   Theta = (FLT)atan2( sqrt( A ), D[2] );
   Phi = (FLT)atan2( D[1], D[0] );    // -D[1], -D[0] ??

   Cx = cos( Phi );   Sx = sin( Phi );
   Cy = cos( Theta ); Sy = sin( Theta );
   Cz = cos( Omega ); Sz = sin( Omega );

   A = Cx*Cy; B = Sx*Cy;
   M[ 9] = M[0] = -A*Sz - Sx*Cz;
   M[12] = M[1] = -B*Sz + Cx*Cz;
   M[15] = M[2] =  Sy*Sz;

   M[10] = M[3] = -A*Cz + Sx*Sz;
   M[13] = M[4] = -B*Cz - Cx*Sz;
   M[16] = M[5] =  Sy*Cz;

   M[11] = M[6] =  Cx*Sy;
   M[14] = M[7] =  Sx*Sy;
   M[17] = M[8] =  Cy;

   Camera_Matrix_Set_Position( Pos, M );
}

/******************************************************************/

//          | 1  0  0  |         | cr sr 0|         | cy 0 -sy |
//  Pitch = | 0  cp sp |  Roll = |-sr cr 0|   Yaw = | 0  1  0  |
//          | 0 -sp cp |         | 0  0  1|         | sy 0  cy |
//
//   Computes R.Y.P
//
// Result can be thought as giving the 3 local vectors:
//           [ Right   ]
//  R.Y.P =  [ Up      ]
//           [ Forward ]

EXTERN void Camera_RYP( FLT Yaw, FLT Pitch, FLT Roll, VECTOR Pos, MATRIX2 M )
{
   double cy, sy, cr, sr, cp, sp, A, B;

   cy = cos( Yaw );   sy = sin( Yaw );
   cp = cos( Pitch ); sp = sin( Pitch );
   cr = cos( Roll );  sr = sin( Roll );

   A = sy*sr;
   B = cy*sr;

   M[9]  = M[0] = (FLT)( cy*cr );
   M[12] = M[1] = (FLT)( -sr );
   M[15] = M[2] = (FLT)( -sy*cr );

   M[10] = M[3] = (FLT)( B*cp + sy*sp );
   M[13] = M[4] = (FLT)( cr*cp );
   M[16] = M[5] = (FLT)(-A*cp + cy*sp );

   M[11] = M[6] = (FLT)(-B*sp + sy*cp );
   M[14] = M[7] = (FLT)(-cr*sp );
   M[17] = M[8] = (FLT)( A*sp + cy*cp );

   Camera_Matrix_Set_Position( Pos, M );
}

      // same as above, but with: Y = -Z and Z = Y

EXTERN void Camera_RYP_II( FLT Yaw, FLT Pitch, FLT Roll, VECTOR Pos, MATRIX2 M )
{
   double cy, sy, cr, sr, cp, sp, A, B;

   cy = cos( Yaw );   sy = sin( Yaw );
   cp = cos( Pitch ); sp = sin( Pitch );
   cr = cos( Roll );  sr = sin( Roll );

   A = sy*sr;
   B = cy*sr;

   M[9]  = M[0] = (FLT)(  cy*cr );
   M[12] = M[1] = (FLT)( sy*cr );
   M[15] = M[2] = (FLT)( -sr );

   M[10] = M[3] = (FLT)( B*cp + sy*sp );
   M[13] = M[4] = (FLT)( A*cp - cy*sp );
   M[16] = M[5] = (FLT)(  cr*cp );

   M[11] = M[6] = (FLT)(-B*sp + sy*cp );
   M[14] = M[7] = (FLT)(-A*sp - cy*cp );
   M[17] = M[8] = (FLT)( -cr*sp );

   Camera_Matrix_Set_Position( Pos, M );
}

/******************************************************************/

