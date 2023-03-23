/***********************************************
 *              Lights                         *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"
#include "color.h"

#ifndef _SKIP_LIGHT_

/******************************************************************/

EXTERN void Vista_Select_Light( OBJ_NODE *Cur_Node, INT Order )
{

}

/******************************************************************/
/******************************************************************/

#ifndef _SKIP_SPOT_

EXTERN void Compute_Spot_Light_Parameters( LIGHT *L )
{
   FLT Norm;

   Assign_Vector( L->Dir, L->Target );
   Sub_Vector_Eq( L->Dir, L->Pos );
   Norm = (FLT)sqrt( Norm_Squared( L->Dir ) );
   L->Cos_Hot = Norm*(FLT)cos( L->Hot_Spot/2.0f );
   if ( L->Fall_Off<=L->Hot_Spot )
   {
      L->Cos_Fall = L->Cos_Hot;
      L->Out_fall = 0.0;
   }
   else
   {
      L->Cos_Fall = Norm*(FLT)cos( L->Fall_Off/2.0f );
      L->Out_fall = 1.0f / ( L->Cos_Hot - L->Cos_Fall );
   }
}

#endif   // _SKIP_SPOT_

EXTERN void Set_Light_Parameter( LIGHT *L,
   FLT Offset, FLT Amplitude, FLT Hot_Spot, FLT Fall_Off )
{
   L->Offset = Offset;
   L->Amplitude = Amplitude;
   L->Fall_Off = Fall_Off;
   L->Hot_Spot = Hot_Spot;
//   if ( L->Light_Type==LIGHT_SPOT ) Compute_Spot_Light_Parameters( L );
}

static void Init( OBJECT *Obj )
{
   LIGHT *Light;

   Light = (LIGHT *)Obj;
   Light->Light_Type = LIGHT_NOT_SET;
   Set_Vector( Light->Dir, 0.0, 0.0, 1.0 );
   Set_Vector( Light->Target, 0.0, 0.0, 1.0 );
   Set_Vector( Light->Pos, 0.0, 0.0, 0.0 );
   Light->Offset = 128.0;
   Light->Amplitude = 128.0;
   Light->Fall_Off = M_PI/2.0;
   Light->Hot_Spot = M_PI/4.0;
   Light->Roll = 0.0;
   Light->Dolly = 1.0;

   Light->Flare = NULL;
   Light->Flare_Scale = 20.0; // Adjust!

   Light->Color[0] = 1.0;
   Light->Color[1] = 1.0;
   Light->Color[2] = 1.0;
   Light->Color[3] = 1.0;
   Light->Last_Viewed = NULL;

   Light->Z_Max = 1.0;   // 42.0;  // aDjust!!
   Light->Z_Min = 0.0;
   Light->Mult = 1.0;

   Light->Where = -1;

   // Light->Shader = &zShadow_Shader;
   Light->Shader = NULL;

   Light->Dummy = NULL;
   Light->Pos_k = Light->Rot_k = Light->Target_k = NULL;
   Light->Roll_k = Light->Color_k = NULL;
   Light->Falloff_k = Light->Hotspot_k = Light->Hide_k = NULL;

   Light->Light_Map = NULL;

   Light->Type = LIGHT_TYPE;
}

static void Transform( OBJ_NODE *Node )
{
   STORE_TRANSFORM_MATRIX( Node );
}

static INT Finish( OBJ_NODE *Node )
{
   LIGHT *Light;

   Light = (LIGHT *)Node->Data;
   if ( Light==NULL ) return( 0 );
   if ( Light->Hot_Spot <0.01f ) Light->Hot_Spot = 0.01f;
#ifndef _SKIP_SPOT_
   Compute_Spot_Light_Parameters( Light );
#endif   // _SKIP_SPOT_

   if ( (Node->Flags&OBJ_DO_ANIM)&& !(Node->Flags&OBJ_TRACK_OK) )
   {
      Setup_Spline_3ds_Derivates( Light->Pos_k );
      Setup_Rot_Spline_3ds_Derivates( Light->Rot_k );
      Setup_Spline_3ds_Derivates( Light->Target_k );
      Setup_Spline_3ds_Derivates( Light->Roll_k );
      Setup_Spline_3ds_Derivates( Light->Color_k );
      Setup_Spline_3ds_Derivates( Light->Falloff_k );
      Setup_Spline_3ds_Derivates( Light->Hotspot_k );
      Node->Flags |= OBJ_TRACK_OK;
   }
   return( 0 );
}

static void Destroy( OBJECT *Dum )
{
   LIGHT *L;

   L = (LIGHT *)Dum;

   Destroy_Spline_3ds( &L->Pos_k );
   Destroy_Spline_3ds( &L->Rot_k );
   Destroy_Spline_3ds( &L->Target_k );
   Destroy_Spline_3ds( &L->Roll_k );
   Destroy_Spline_3ds( &L->Color_k );
   Destroy_Spline_3ds( &L->Falloff_k );
   Destroy_Spline_3ds( &L->Hotspot_k );
   Destroy_Spline_3ds( &L->Hide_k );
   L->Flags = (OBJ_FLAG)( L->Flags & ~OBJ_TRACK_OK );

      // Don't destroy *Flare, it's user's resource!
   L->Flare = NULL; 
}

static void Anim( OBJ_NODE *Node, FLT Frame )
{
   LIGHT *Light;

   Light = (LIGHT *)Node->Data;

#ifndef _SKIP_HIDE_
   if ( Light->Hide_k!=NULL )
   {
      INT Hide;
      Hide = Interpolate_Spline_3ds_Hidden( Frame + .01f, Light->Hide_k );
      if ( Hide )
         Node->Flags &= ~OBJ_NOT_ACTIVE;
      else
         Node->Flags |= OBJ_NOT_ACTIVE;
   }
#endif   // _SKIP_HIDE_

   if ( Node->Cur_Time==Frame ) return;

   if ( Light->Rot_k!=NULL )
      Interpolate_Rot_Spline_3ds_Values(
         Frame, Light->Rot_k, (FLT *)&Node->Transform.Rot.QRot );
   if ( Light->Pos_k!=NULL )
      Interpolate_Spline_3ds_Values( 
         Frame, Light->Pos_k, (FLT *)&Node->Transform.Pos );
   if ( Light->Color_k!=NULL )
      Interpolate_Spline_3ds_Values( 
            Frame, Light->Color_k, (FLT *)&Light->Color );

#ifndef _SKIP_SPOT_

   if ( Light->Light_Type==LIGHT_SPOT )     // TODO: Finish that
   {
      if ( Light->Dummy!=NULL )     // check dummy's transform??
      {
         SPLINE_3DS *Sp;
         Sp = ((OBJ_DUMMY*)Light->Dummy->Data)->Pos_k;
         if ( Sp!=NULL )
            Interpolate_Spline_3ds_Values( 
               Frame, Sp, (FLT *)&Light->Target );
      }
      else if ( Light->Target_k!=NULL )
         Interpolate_Spline_3ds_Values( 
            Frame, Light->Target_k, (FLT *)&Light->Target );
      if ( Light->Hotspot_k!=NULL )
         Interpolate_Spline_3ds_Values( 
            Frame, Light->Hotspot_k, (FLT *)&Light->Hot_Spot );
      if ( Light->Falloff_k!=NULL )
         Interpolate_Spline_3ds_Values( 
            Frame, Light->Falloff_k, (FLT *)&Light->Fall_Off );
      if ( Light->Roll_k!=NULL )
         Interpolate_Spline_3ds_Values( 
            Frame, Light->Roll_k, (FLT *)&Light->Roll );
//      Compute_Spot_Light_Parameters( Node );
   }
#endif   // _SKIP_SPOT_

}

/******************************************************************/

EXTERN NODE_METHODS _LIGHT_ =
{
   sizeof( LIGHT ),
   Init, Finish, Destroy,
   Transform, Anim, 
   NULL,    // Render
   NULL,    // Post
   NULL,    // Copy
   NULL, NULL, NULL, NULL, NULL  // reserved for ray-tracer
};

#if 0
EXTERN NODE_METHODS _LIGHT_WITH_RENDER_ =
{
   sizeof( LIGHT ),
   Init, Finish, Destroy,
   Transform, Anim, 
   NULL,    // Render
   NULL,    // Post
   NULL,    // Copy
   NULL, NULL, NULL, NULL, NULL  // reserved for ray-tracer
};
#endif

/******************************************************************/

EXTERN void Compose_Light_And_Camera( OBJ_NODE *Node, OBJ_NODE *Cam )
{
   LIGHT *Light;

   Light = (LIGHT *)Node->Data;

      // Select light matrix for this Cam->Mo matrix
      // *Cam must be set before...

   STORE_NODE_MATRIX( Node, Cam );

      // compute position and direction in camera's space. Project.

   Assign_Vector( Light->Pos, MATRIX_TRANSL( Node->Mo ) );
   A_V( Light->P, Cam->Mo, Light->Pos );

#ifndef _SKIP_SPOT_
   if ( Light->Light_Type == LIGHT_SPOT )
   {
      Compute_Spot_Light_Parameters( Light );
      nA_V( Light->D, Cam->Mo, Light->Dir );
   }
#endif
}

EXTERN void Select_Light_Visibility( OBJ_NODE *Node )
{
   FLT Inv_Z;
   LIGHT *Light;

   Light = (LIGHT *)Node->Data;

         // not for raytrace?

   if ( Light->P[2]<=_RCst_.Clips[4] )
   {
Not_Seen:
      Light->Where = -1;
      return;
   }
   
   Light->Inv_Z = Inv_Z = 1.0f/Light->P[2];
   Inv_Z *= _RCst_._Lx_;
   Light->Xp = (INT)( _RCst_._Cx_ - Light->P[0]*Inv_Z );
   if ( Light->Xp>=_RCst_.Clips[1] || Light->Xp<=_RCst_.Clips[0] ) goto Not_Seen;
   Light->Yp = (INT)( _RCst_._Cy_ - Light->P[1]*Inv_Z );
   if ( Light->Yp>=_RCst_.Clips[3] || Light->Xp<=_RCst_.Clips[2] ) goto Not_Seen;
   Light->Where = (INT)( Light->Yp )*_RCst_.Pix_Width + (INT)( Light->Xp );

#ifndef _SKIP_SPOT_
   if ( Light->Light_Type == LIGHT_SPOT )
   {
      Inv_Z = Light->P[2] + Light->D[2];
      if ( Inv_Z<_RCst_.Out_Clips[4] ) Inv_Z = _RCst_.Out_Clips[4];
      Light->D_Inv_Z = Inv_Z = 1.0f / Inv_Z;
      Inv_Z *= _RCst_._Lx_;
      Light->Dx = Light->Xp - (INT)( ( Light->P[0]+Light->D[0] )*Inv_Z );
      Light->Dy = Light->Yp - (INT)( ( Light->P[1]+Light->D[1] )*Inv_Z );
   }
#endif   // _SKIP_SPOT_
}

#if 0    // OLD

EXTERN void Set_Light_Pos_And_Dir( LIGHT *Light, VECTOR Pos, VECTOR Look_At )
{
   Set_Vector( Light->Pos, Pos[0], Pos[1], Pos[2] );
   Sub_Vector_Eq( Look_At, Pos );
   Normalize_Safe( Look_At );
   Set_Vector( Light->Dir, Look_At[0], Look_At[1], Look_At[2] );
}
#endif

/******************************************************************/
/*                           SHADOWS                              */
/******************************************************************/

EXTERN void Light_To_Matrix( LIGHT *Light, MATRIX2 M )
{
   FLT Tmp, Cx, Sx, Cz, Sz;
   VECTOR V;

   // Light->P and Light->D must be set before getting here...

   Cz = Light->D[1]; Tmp  = Cz*Cz;
   Sz = Light->D[0]; Tmp += Sz*Sz;

   if ( Tmp<0.0001f )
   {
      Id_Matrix( M );
      M[18] = -Light->P[0];
      M[19] = -Light->P[1];
      M[20] = -Light->P[2];
      Assign_Vector( MATRIX_INV_TRANSL(M), Light->P );
      return;
   }
   Tmp = 1.0f / (FLT)sqrt( (double)Tmp );
   Cz *= Tmp; Sz *= Tmp;

   Cx = Light->D[2]*Tmp;
   Tmp = (FLT)acos( Cx );    /* Theta */
   Sx = (FLT)sin( Tmp );
   
   M[9]  = M[0] =  Cz;
   M[12] = M[1] = -Sz;
   M[15] = M[2] =  0;

   M[10] = M[3] =  Cx*Sz;
   M[13] = M[4] =  Cx*Cz;
   M[16] = M[5] = -Sx;

   M[11] = M[6] =  Sx*Sz;
   M[14] = M[7] =  Sx*Cz;
   M[17] = M[8] =  Cx;

   V[0] = -Light->P[0];
   V[1] = -Light->P[1];
   V[2] = -Light->P[2];
   Raw_A_V( MATRIX_TRANSL(M), M, V );
   Raw_A_V( MATRIX_INV_TRANSL(M), MATRIX_INV(M), Light->P );
}

#endif   // _SKIP_LIGHT_

/******************************************************************/
/******************************************************************/
/*                        OLD SHADOWS                             */
/******************************************************************/
/******************************************************************/

#ifndef  _SKIP_LIGHT_

#ifdef USE_OLD_SHADOWZ

EXTERN USHORT UV_Vertex[ MAX_LIGHT_VERTEX ];
EXTERN FLT Inv_Z_Vertex[ MAX_LIGHT_VERTEX ];

EXTERN USHORT Vertex_ID;
// EXTERN USHORT *Shadow_IDs;


EXTERN OBJECT *Light_Root;
EXTERN USHORT *Light_Buf;     // <= remove!

   // *_RCst_.The_Camera must be set before...
EXTERN void Sort_Light_Boxes( LIGHT *Light, OBJ_NODE *Cur )
{
   for( ; Cur!=NULL; Cur = Cur->Next )
   {
      if ( Cur->Flags & ( OBJ_NO_SHADOW|OBJ_DONT_RENDER ) ) continue;

      Cur->Flags |= OBJ_NOT_LIT; /* A priori. */

#if 0
      {
         VECTOR C;
         Add_Vector( C, Cur->Box.Mins, Cur->Box.Maxs );
         Scale_Vector_Eq( C, .5 );  // V = box' center
         Add_Vector_Eq( C, Cur->Transform.Pos );
         A_V( Cur->Box.C_Proj, _RCst_.The_Camera->Mo, C );

         // Needless. All boxes are seen.
         // if ( !Is_Sphere_Viewed_By_Light( Light, &Cur->Box ) ) continue;
      }
#endif

      Cur->Flags &= ~OBJ_NOT_LIT;   /* Ok. This one is seen. */
      {
         UINT Key;

            /* Choose Scale factor !!! */
         Key = (UINT)(Cur->Box.Z_1*65535.9) | (Nb_Obj_To_Sort<<16);
         Sorted[Nb_Sorted++] = Key;
         Obj_To_Sort[ Nb_Obj_To_Sort++ ] = (void *)Cur;
      }

      if ( Cur->Child!=NULL )      // recurse
         Sort_Light_Boxes( Light, Cur->Child );
   }   
}

/******************************************************************/

   // *_RCst_.The_Camera must be set before getting here...
   
EXTERN void Render_Light( LIGHT *Light, OBJ_NODE *Root )
{
   INT Nb;

   Light_Root = Root;
   Cur_Light = Light;

    /* TODO: should be elsewhere */

   Light_Buf = (USHORT *)MEM_Base_Ptr( Light->Buf );
   BZero( Light_Buf, 256*256*sizeof( USHORT ) );

#if 0
   Light->Inv_Z_Min = _HUGE_;
   Light->Inv_Z_Max = 1.0;       /* Default FOV !?? */
#endif

      /* Setup _RCst_.The_Camera->Mo once for all  */
      /* It will be used only while                */
      /* traversing the tree                       */

   Light_To_Matrix( Light, _RCst_.The_Camera->Mo );

   Nb_Sorted = 0;
   Nb_Obj_To_Sort = 0;
   Renderer.Sort_Lights( Light, Root );
   if ( !Nb_Sorted ) return;
   Counting_Sort( Sorted, Nb_Sorted );

         /* Render shadow map */

   Cur_Shader = Light->Shader;
   for( Nb=0; Nb<Nb_Sorted; ++Nb )
   {
      MATRIX2 M;

      Cur_Msh = (MESH *)Obj_To_Sort[ Sorted[Nb]>>16 ];

      if ( Cur_Msh->Type!=MESH_TYPE ) continue;

      // Shadow_IDs = Cur_Msh->Shadow_ID;

         /* TODO: M have already been computed ?!? */
      Transform_To_Matrix( &Cur_Msh->Transform, M );
      Mult_Matrix2( Cur_Msh->Parent->Local_Matrix, M );

      (*Cur_Shader->Transform)( M );
      (*Cur_Shader->Emit_Poly)( );     // BUG!! OLD!!! FIX!!!
   }
}

#endif      // OLD_SHADOWZ

#endif   // _SKIP_LIGHT_

/******************************************************************/
/******************************************************************/
