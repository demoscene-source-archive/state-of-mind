/***********************************************
 *              normal.c                       *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/
/******************************************************************/

#ifdef UNIX

EXTERN void Mesh_Store_Normals2( MESH *M )
{
   INT i;
   POLY *P = M->Polys;
   for( i=M->Nb_Polys;i>0;--i,P++ )
   {
      FLT *V0 = (FLT*)M->Vertex[P->Pt[0]];
      FLT *V1 = (FLT*)M->Vertex[P->Pt[1]];
      FLT *V2 = (FLT*)M->Vertex[P->Pt[2]];
      FLT dx1 = V1[0] - V0[0];
      FLT dx2 = V2[0] - V0[0];
      FLT dy1 = V1[1] - V0[1];
      FLT dy2 = V2[1] - V0[1];
      FLT dz1 = V1[2] - V0[2];
      FLT dz2 = V2[2] - V0[2];
      FLT N;
      P->No[0] = dy1*dz2 - dy2*dz1; 
      P->No[1] = dx2*dz1 - dx1*dz2;
      P->No[2] = dx1*dy2 - dx2*dy1;
      N  = Norm_Squared(P->No);
      if ( N>0.0 )
      {
         N = 1.0/(FLT)sqrt( (double) N );
         P->No[0] *= N;
         P->No[1] *= N;
         P->No[2] *= N;
      }
   }
   M->Flags = (OBJ_FLAG)(M->Flags | OBJ_HAS_NORMALS);

}
#endif /* UNIX */

/******************************************************************/

#ifdef UNIX

EXTERN void Average_Normals( MESH *M )
{
   POLY *P;
   VECTOR *Ns;
   INT i;

   if ( !(M->Flags & OBJ_HAS_NORMALS) ) Mesh_Store_Normals( M );
   Ns = M->Normals;

   for( i=M->Nb_Vertex-1; i>=0; --i )
      Ns[i][0] = Ns[i][1] = Ns[i][2] = 0.0;

   P = M->Polys;
   for( i=M->Nb_Polys; i>0; --i, P++ )
   {     
         /* Only 3 first points are used for the normal... */
      Ns[ P->Pt[0] ][0] += P->No[0];
      Ns[ P->Pt[1] ][0] += P->No[0];
      Ns[ P->Pt[2] ][0] += P->No[0];
      Ns[ P->Pt[0] ][1] += P->No[1];
      Ns[ P->Pt[1] ][1] += P->No[1];
      Ns[ P->Pt[2] ][1] += P->No[1];
      Ns[ P->Pt[0] ][2] += P->No[2];
      Ns[ P->Pt[1] ][2] += P->No[2];
      Ns[ P->Pt[2] ][2] += P->No[2];
   }

   for( i=M->Nb_Vertex-1; i>=0; --i )
   {
      FLT N;

      N  = Ns[i][ 0 ]*Ns[i][ 0 ];
      N += Ns[i][ 1 ]*Ns[i][ 1 ];
      N += Ns[i][ 2 ]*Ns[i][ 2 ];

      if ( N<=0.0 )
      {
         Ns[i][0] = 0.0;
         Ns[i][1] = 0.0;
         Ns[i][2] = 1.0;
      }
      else
      {
         N = 1.0/sqrt( N );
         Ns[i][0] *= N;
         Ns[i][1] *= N;
         Ns[i][2] *= N;
      }
   }
}
#endif /* UNIX */

/******************************************************************/
