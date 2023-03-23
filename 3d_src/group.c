/***********************************************
 *              group.c                        *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

static void Init( OBJECT *Obj )
{
   GROUP *New;
   New = (GROUP *)Obj;
   New->Pos_k = New->Rot_k = New->Scale_k = New->Hide_k = NULL;
   New->Type = GROUP_TYPE;
}

static void Destroy( OBJECT *Dum )
{
   GROUP *M;

   M = (GROUP *)Dum;
   if ( M==NULL ) return;

   Destroy_Spline_3ds( &M->Pos_k );
   Destroy_Spline_3ds( &M->Rot_k );
   Destroy_Spline_3ds( &M->Scale_k );
   Destroy_Spline_3ds( &M->Hide_k );
   M->Flags = (OBJ_FLAG)( M->Flags & ~OBJ_TRACK_OK );
}

static INT Finish( OBJ_NODE *Node )
{
   GROUP *M;

   M = (GROUP *)Node->Data;
   if ( M==NULL ) return( 0 );
   if ( (M->Flags&OBJ_DO_ANIM) && !(M->Flags&OBJ_TRACK_OK) )
   {
      Setup_Spline_3ds_Derivates( M->Pos_k );
      Setup_Spline_3ds_Derivates( M->Scale_k );
      Setup_Rot_Spline_3ds_Derivates( M->Rot_k );
//      Setup_Spline_3ds_Derivates( M->Hide_k );
      M->Flags = (OBJ_FLAG)( M->Flags | OBJ_TRACK_OK );
   }
   return( 0 );
}

static void Transform( OBJ_NODE *Node )
{
   STORE_TRANSFORM_MATRIX( Node );
}

static void Anim( OBJ_NODE *Node, FLT Frame )
{
   GROUP *Group;

   Group = (GROUP *)Node->Data;

#ifndef _SKIP_HIDE_

   if ( Group->Hide_k!=NULL )
   {
      INT Hide;
      Hide = Interpolate_Spline_3ds_Hidden( Frame + .01f, Group->Hide_k );
      if ( Hide )
         Node->Flags &= ~OBJ_NOT_ACTIVE;
      else
         Node->Flags |= OBJ_NOT_ACTIVE;
   }

#endif   // _SKIP_HIDE_

   if ( Node->Cur_Time==Frame ) return;

   if ( Group->Rot_k != NULL )
      Interpolate_Rot_Spline_3ds_Values(
         Frame, Group->Rot_k, (FLT *)&Node->Transform.Rot.QRot );
   if ( Group->Pos_k != NULL )
      Interpolate_Spline_3ds_Values( 
         Frame, Group->Pos_k, (FLT *)&Node->Transform.Pos );
   if ( Group->Scale_k != NULL )
      Interpolate_Spline_3ds_Values(
         Frame, Group->Scale_k, (FLT *)&Node->Transform.Scale );
}

/******************************************************************/

EXTERN NODE_METHODS _GROUP_ =
{
   sizeof( GROUP ),
   Init, Finish, Destroy,
   Transform, Anim, 
   NULL,
   NULL,    // Post
   NULL,    // Copy
   NULL, NULL, NULL, NULL, NULL     // reserved for ray-tracer
};

/******************************************************************/
/******************************************************************/

static void Init_Dummy( OBJECT *Obj )
{
   OBJ_DUMMY *New;

   New = (OBJ_DUMMY *)Obj;
   New->Pos_k = New->Rot_k = New->Scale_k = New->Hide_k = NULL;
   New->FOV_k = New->Roll_k = New->Morph_k = NULL;
   New->Type = OBJ_DUMMY_TYPE;
}

static INT Finish_Dummy( OBJ_NODE *Node )
{
   OBJ_DUMMY *Dummy;

   Dummy = (OBJ_DUMMY *)Node->Data;
   if ( Dummy==NULL ) return( 0 );
   
   if ( (Node->Flags & OBJ_DO_ANIM) && !(Node->Flags&OBJ_TRACK_OK) )
   {
      Setup_Spline_3ds_Derivates( Dummy->Pos_k );
      Setup_Rot_Spline_3ds_Derivates( Dummy->Rot_k );
      Setup_Spline_3ds_Derivates( Dummy->Scale_k );
      Setup_Spline_3ds_Derivates( Dummy->FOV_k );
      Setup_Spline_3ds_Derivates( Dummy->Roll_k );
//      Setup_Spline_3ds_Derivates( Dummy->Hide_k );
      Setup_Spline_3ds_Derivates( Dummy->Morph_k );
      Node->Flags |= OBJ_TRACK_OK;
   }
   return( 0 );
}

static void Destroy_Dummy( OBJECT *Dum )
{
   OBJ_DUMMY *Dummy;

   Dummy = (OBJ_DUMMY *)Dum;

   Destroy_Spline_3ds( &Dummy->Pos_k );
   Destroy_Spline_3ds( &Dummy->Rot_k );
   Destroy_Spline_3ds( &Dummy->Scale_k );
   Destroy_Spline_3ds( &Dummy->FOV_k );
   Destroy_Spline_3ds( &Dummy->Roll_k );
   Destroy_Spline_3ds( &Dummy->Hide_k );
   Destroy_Spline_3ds( &Dummy->Morph_k );
   Dummy->Flags = (OBJ_FLAG)( Dummy->Flags & ~OBJ_TRACK_OK );
}

static void Anim_Dummy( OBJ_NODE *Node, FLT Frame )
{
   OBJ_DUMMY *Dummy;

   Dummy = (OBJ_DUMMY *)Node->Data;

#ifndef _SKIP_HIDE_

   if ( Dummy->Hide_k!=NULL )
   {
      INT Hide;
      Hide = Interpolate_Spline_3ds_Hidden( Frame + .01f, Dummy->Hide_k );
      if ( Hide )
         Node->Flags &= ~OBJ_NOT_ACTIVE;
      else
         Node->Flags |= OBJ_NOT_ACTIVE;
   }

#endif   // _SKIP_HIDE_

   if ( Node->Cur_Time==Frame ) return;

   if ( Dummy->Rot_k != NULL )
      Interpolate_Rot_Spline_3ds_Values(
         Frame, Dummy->Rot_k, (FLT *)&Node->Transform.Rot.QRot );
   if ( Dummy->Pos_k != NULL )
      Interpolate_Spline_3ds_Values( 
         Frame, Dummy->Pos_k, (FLT *)&Node->Transform.Pos );
   if ( Dummy->Scale_k != NULL )
      Interpolate_Spline_3ds_Values(
         Frame, Dummy->Scale_k, (FLT *)&Node->Transform.Scale );

      // What about the rest of animation keys!??
}

EXTERN NODE_METHODS _OBJ_DUMMY_ =
{
   sizeof( OBJ_DUMMY ),
   Init_Dummy, Finish_Dummy, Destroy_Dummy,
   Transform, Anim_Dummy, 
   NULL,
   NULL,    // Post
   NULL,    // Copy
   NULL, NULL, NULL, NULL, NULL
};

/******************************************************************/
/******************************************************************/
