/***********************************************
 *           vista mesh                        *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

// #define DEBUG_V

#define DO_KEY(K)  { if ( (K)>Key ) Key = (K); }
// #define DO_KEY(K)  { Key += (K); }

/******************************************************************/

EXTERN void Vista_Select_Poly( INT Order )
{
   INT j, Nb_Sorted_Save;
   FLT Key;
   VECTOR N;

   Orig_Poly->Clipped = FLAG_POLY_NOT_VISIBLE;

   nA_V( Cur_P_Poly->N, Cur_Node->M, Orig_Poly->No );
   if ( Cur_P_Poly->N[2]>B_CULL_EPSILON ) return;

   Nb_Sorted_Save = Nb_Sorted;

   j = Orig_Poly->Pt[0];
   A_V( N, Cur_Node->M, (FLT *)( Cur_Msh->Vertex+j ) );
   if ( Dot_Product( N, Cur_P_Poly->N )>0.0 ) return;

   Zo_Clipped = Z1_Clipped = Xo_Clipped = X1_Clipped = 
      Yo_Clipped = Y1_Clipped = Orig_Poly->Nb_Pts;

   j = Orig_Poly->Nb_Pts - 1;   // <= Nb_Vertices_To_Deal_With
   Key = 0.0;
   for( ;j>=0; --j )
   {
      FLT Inv_Z;

      Cur_Poly_Pt = j;
      Cur_Vtx_Nb = Orig_Poly->Pt[j];
      Cur_P_Vertex_Nb = Nb_Sorted_Save + j;
      Cur_P_Vertex = P_Vertex + Cur_P_Vertex_Nb;
      Cur_Vertex_State = Vertex_State + Cur_P_Vertex_Nb;

            // Keep projected vertex in .N field

      if ( j!=0 )
         A_V( Cur_P_Vertex->N, Cur_Node->M, (FLT *)( Cur_Msh->Vertex+Cur_Vtx_Nb ) );
      else Set_Vector( Cur_P_Vertex->N, N[0], N[1], N[2] );

      if ( Cur_P_Vertex->N[2]<_RCst_.Out_Clips[4] ) goto Forget;
      if ( Cur_P_Vertex->N[2]>_RCst_.Out_Clips[5] ) goto Forget;

      if ( fabs(Cur_P_Vertex->N[2])<Z_EPSILON )
         Cur_P_Vertex->N[2] = Z_EPSILON;

      Cur_P_Vertex->Inv_Z = Inv_Z = 1.0f/Cur_P_Vertex->N[2];

      *Cur_Vertex_State = FLAG_VTX_TESTED;

      Inv_Z *= _RCst_._Lx_;

      Cur_P_Vertex->xp = Cur_P_Vertex->N[0]*Inv_Z;
      Cur_P_Vertex->xp += _RCst_._Cx_;
      Cur_P_Vertex->yp = -Cur_P_Vertex->N[1]*Inv_Z;
      Cur_P_Vertex->yp += _RCst_._Cy_;

      if ( Cur_P_Vertex->N[2]<_RCst_.Clips[4] )
      {
         *Cur_Vertex_State |= ( FLAG_VTX_VISIBLE | VTX_CLIP_Zo );
         Zo_Clipped--;
         DO_KEY( _RCst_.Clips[4] );
         goto Skip;  // don't set clipping flags now. xp and yp are wrong.
      }
      else if ( Cur_P_Vertex->N[2]>_RCst_.Clips[6] )
      {
         *Cur_Vertex_State |= ( FLAG_VTX_VISIBLE | VTX_CLIP_Z1 );
         Z1_Clipped--;
         DO_KEY( _RCst_.Clips[6] );
      }
      else DO_KEY( Cur_P_Vertex->N[2] );

      if ( Cur_P_Vertex->xp<_RCst_.Clips[0] )
      {
         *Cur_Vertex_State |= ( FLAG_VTX_VISIBLE | VTX_CLIP_Xo );
         Xo_Clipped--;
      }
      else if ( Cur_P_Vertex->xp>=_RCst_.Clips[1] )
      {
         *Cur_Vertex_State |= ( FLAG_VTX_VISIBLE | VTX_CLIP_X1 );
         X1_Clipped--;
      }

      if ( Cur_P_Vertex->yp<_RCst_.Clips[2] ) 
      {
         *Cur_Vertex_State |= ( FLAG_VTX_VISIBLE | VTX_CLIP_Yo );
         Yo_Clipped--;
      }
      if ( Cur_P_Vertex->yp>=_RCst_.Clips[3] )
      {
         Y1_Clipped--;
         *Cur_Vertex_State |= ( FLAG_VTX_VISIBLE | VTX_CLIP_Y1 );
      }

Skip:

      //////////////// retreive vertex fields /////////////////////
      // should be (((MATERIAL*)Orig_Poly->Ptr)->Shader) here!!! //
      // .N[2] field (=z -> Key) mustn't be touched by shader.   //
      /////////////////////////////////////////////////////////////

      if ( Cur_Shader->Shade_Vertex!=NULL )
         (*Cur_Shader->Shade_Vertex)( Cur_Node->M );

      Orig_Poly->Clipped |= *Cur_Vertex_State;
      Nb_Sorted++;
   }

   if ( ( X1_Clipped == 0 ) || ( Xo_Clipped == 0 ) ||
        ( Y1_Clipped == 0 ) || ( Yo_Clipped == 0 ) ||
        ( Zo_Clipped == 0 ) || ( Z1_Clipped == 0 ) )
   {
      Orig_Poly->Clipped = FLAG_POLY_NOT_VISIBLE;
      goto Forget;  // Totally x,y, or z-clipped -> invisible
   }

         // Ok. Put with the rest
         // Keep only useful clipping flags.

   Orig_Poly->Clipped &= ~FLAG_FOR_CLIPPING_II;
   Orig_Poly->Clipped |= FLAG_POLY_VISIBLE;   // Mark as visible.
   Cur_P_Poly->Poly_Orig = Orig_Poly;

   //    Key /= Orig_Poly->Nb_Pts

   Cur_P_Poly->Key = Key;

      // Key MUST be in range [Z_Min,Z_Max] here...

//   if ( Key<_RCst_.Z_Min ) fprintf( stderr, "Key<Z_Min!!! (%f/%f)\n", Key, _RCst_.Z_Min );
//   if ( Key>_RCst_.Z_Max ) fprintf( stderr, "Key>Z_Max!!! (%f/%f)\n", Key, _RCst_.Z_Max );

   Key = ( _RCst_.Z_Max - Key )*_RCst_.Z_Vista_Scale;
//   if ( Key<0.0 ) fprintf( stderr, "Key<0.0!!! (%f)\n", Key );
//   else if ( Key>65535.0 ) fprintf( stderr, "Key>65535.0!!! (%f)\n", Key );

      // ?!? TODO: FIX!!! shouldn't happen
   if ( Key<0.0 ) Key = 0.0;
   else if ( Key>65535.0 ) Key = 65535.0;

   Poly_Keys[Nb_Poly_Sorted] = (Nb_Poly_Sorted<<16) | (UINT)( Key );

      // Poly's vertex entry in P_Vertex[]
   Sorted[Nb_Poly_Sorted] = Nb_Sorted_Save | (Order<<16); 

   Nb_Poly_Sorted++;

   return;

Forget:
   Nb_Sorted = Nb_Sorted_Save;
   return;
}

/******************************************************************/
/******************************************************************/
