/***********************************************
 *              Particles                      *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

static void Init( OBJECT *Obj )
{
   PARTICLE *New;

   New = (PARTICLE *)Obj;
   New->Nb_Parts = 0;
   New->P = New->V = NULL;
   New->Type = PARTICLE_TYPE;
}

static void Destroy( OBJECT *Dum )
{
   PARTICLE *P;

   P = (PARTICLE *)Dum;
   if ( P==NULL ) return;
   M_Free( P->P );
   P->V = NULL;
   P->Nb_Parts = 0;

   Destroy_Spline_3ds( &P->Pos_k );
   Destroy_Spline_3ds( &P->Rot_k );
   Destroy_Spline_3ds( &P->Scale_k );
   Destroy_Spline_3ds( &P->Hide_k );

   P->Flags = (OBJ_FLAG)(P->Flags & ~OBJ_TRACK_OK);

}

static INT Finish( OBJ_NODE *Node )
{
   PARTICLE *P;

   P = (PARTICLE *)Node->Data;
   if ( P==NULL ) return( 0 );

      // Animation setup

   if ( (Node->Flags&OBJ_DO_ANIM) && !(Node->Flags&OBJ_TRACK_OK) )
   {
      Setup_Spline_3ds_Derivates( P->Pos_k );
      Setup_Spline_3ds_Derivates( P->Scale_k );
      Setup_Rot_Spline_3ds_Derivates( P->Rot_k );
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
   PARTICLE *P;

   P = (PARTICLE *)Node->Data;

#ifndef _SKIP_HIDE_

   if ( P->Hide_k!=NULL )
   {
      INT Hide;
      Hide = Interpolate_Spline_3ds_Hidden( Frame + .01f, P->Hide_k );
      if ( Hide ) Node->Flags &= ~OBJ_NOT_ACTIVE;
      else Node->Flags |= OBJ_NOT_ACTIVE;
   }

#endif   // _SKIP_HIDE_

   if ( Node->Cur_Time==Frame ) return;

   if ( P->Rot_k != NULL )
      Interpolate_Rot_Spline_3ds_Values( Frame, 
         P->Rot_k, (FLT *)&Node->Transform.Rot.QRot );
   if ( P->Pos_k != NULL )
      Interpolate_Spline_3ds_Values( Frame,
         P->Pos_k, (FLT *)&Node->Transform.Pos );
   if ( P->Scale_k != NULL )
      Interpolate_Spline_3ds_Values( Frame, 
         P->Scale_k, (FLT *)&Node->Transform.Scale );
}

/******************************************************************/

EXTERN INT New_Particles( OBJ_NODE *Node, INT Nb )
{
   PARTICLE *P;

   if ( Node==NULL || Node->Type != PARTICLE_TYPE ) return( 0 );
   P = (PARTICLE *)Node->Data;
   M_Free( P->P );
   P->V = NULL;
   P->Nb_Parts = 0;
   if ( Nb<=0 ) return( 0 );
   P->P = New_Fatal_Object( 2*Nb, VECTOR );
   if ( P->P==NULL) return( 0 );
   P->V = P->P + Nb;
   P->Nb_Parts = Nb;
   return( Nb );   
}

static void Render_Particles( OBJ_NODE *Cur_Node, MATRIX2 M )
{
   PARTICLE *P;

   if ( Cur_Node==NULL || Cur_Node->Type != PARTICLE_TYPE ) return;
   P = (PARTICLE *)Cur_Node->Data;   
}

/******************************************************************/

EXTERN NODE_METHODS _PARTICLE_ =
{
   sizeof( PARTICLE ),
   Init, Finish, Destroy,
   Transform, Anim, 
   Render_Particles,
   NULL,    // Post
   NULL,    // Copy
   NULL, NULL, NULL, NULL, NULL     // ray-tracing
};

/******************************************************************/
