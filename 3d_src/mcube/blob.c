/***********************************************
 *              blob.c                         *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

#define MAX_BLOB_REMESH       10000
#define MAX_BLOB_POLYS        10000

static INT Nb_Good = 0, Nb_Remesh = 0;
static USHORT Good[MAX_BLOB_REMESH];
static USHORT Remesh[MAX_BLOB_REMESH];

   /* Should be transfered into BLOB struct */
static USHORT V_Index[MAX_BLOB_REMESH];
static POLY   Blob_Polys[MAX_BLOB_POLYS];

/******************************************************************/
/******************************************************************/

EXTERN MESH *Blobify( BLOB *Blob, MESH *Mesh, FLT Radius, FLT Strength )
{
   MESH *New;
   INT i, j;

   if ( Blob==NULL ) return( NULL );
   if ( Blob->Nb_Centers==MAX_CENTERS ) return( NULL );

   if ( Mesh==NULL ) return( NULL );

   i = Blob->Nb_Vertex + Mesh->Nb_Vertex;
   j = Blob->Nb_Polys + Mesh->Nb_Polys;
   if ( i>MAX_BLOB_REMESH || j>MAX_BLOB_POLYS ) return( NULL );  // No more space
   Blob->Nb_Vertex = i;
   Blob->Nb_Polys = j;

   i = Blob->Nb_Centers++;
   Blob->Components[i] = Mesh;
   Blob->Radius[i]     = Radius;
   Blob->Strength[i]   = Strength;

   return( Mesh );
}

EXTERN MESH *Finish_Blob( BLOB *Blob )
{
   INT i, j, Vtx, Polys;

   Blob->Orig_R = NULL;
   Blob->Blob = Set_Mesh_Data( Blob->Blob, Blob->Nb_Vertex, Blob->Nb_Polys );
   if ( Blob->Blob == NULL )
      goto Failed;

   Blob->Orig_R = New_Object( Blob->Nb_Vertex, FLT );
   if ( Blob->Orig_R == NULL ) goto Failed;

   Vtx = 0;
   Polys = 0;
   for( i=0; i<Blob->Nb_Centers; ++i )
   {
      VECTOR G;
      FLT Norm;

      Set_Vector( G, 0.0, 0.0, 0.0 );
      for( j=0; j<Blob->Components[i]->Nb_Vertex; ++j )
      {
         G[0] += Blob->Components[i]->Vertex[j][0];
         G[1] += Blob->Components[i]->Vertex[j][1];
         G[2] += Blob->Components[i]->Vertex[j][2];

             /* Just tmp... So Average_Normals() is ok */
         Set_Vector( Blob->Blob->Vertex[Vtx+j],
            Blob->Components[i]->Vertex[j][0],
            Blob->Components[i]->Vertex[j][1],
            Blob->Components[i]->Vertex[j][2] );
      }

      Norm = 1.0 / (FLT)j;
      Blob->G[i][0] = G[0] = G[0]*Norm;
      Blob->G[i][1] = G[1] = G[1]*Norm;
      Blob->G[i][2] = G[2] = G[2]*Norm;

      for( j=0; j<Blob->Components[i]->Nb_Vertex; ++j )
      {
         VECTOR P;
         P[0] = Blob->Components[i]->Vertex[j][0] - G[0];
         P[1] = Blob->Components[i]->Vertex[j][1] - G[1];
         P[2] = Blob->Components[i]->Vertex[j][2] - G[2];
         Norm = (FLT)sqrt( (double)Norm_Squared( P ) );
         Blob->Orig_R[Vtx+j] = Norm;
         Norm = 1.0/Norm;
         Blob->Components[i]->Vertex[j][0] = P[0]*Norm;
         Blob->Components[i]->Vertex[j][1] = P[1]*Norm;
         Blob->Components[i]->Vertex[j][2] = P[2]*Norm;
      }

      for( j=0; j<Blob->Components[i]->Nb_Polys; ++j )
      {
         INT k;
         Blob->Blob->Polys[Polys+j] = Blob->Components[i]->Polys[j];
         for( k=0; k<Blob->Blob->Polys[Polys+j].Nb_Pts; ++k )
            Blob->Blob->Polys[Polys+j].Pt[k] += Vtx;
         Blob_Polys[Polys+j] = Blob->Blob->Polys[Polys+j];
      }
      Polys += j;
      Vtx += Blob->Components[i]->Nb_Vertex;

      Blob->dKi[i] = 2.0*Blob->Strength[i]/( Blob->Radius[i]*Blob->Radius[i] );
   }
   Blob->Blob->Flags |= OBJ_DONT_OPTIMIZ;
   return( Blob->Blob );

Failed:

   if ( Blob->Blob != NULL )
   {
      Destroy_Object( (OBJECT *)Blob->Blob, _MESH_.Destroy );
      M_Free( Blob->Blob );
   }

   M_Free( Blob->Orig_R );
   return( NULL );
}

/******************************************************************/
/******************************************************************/

#ifdef SOLVE_HARD

EXTERN void Remesh_Blob( BLOB *Blob, FLT K, FLT Blend )
{
   INT i, j, n, Vtx;
   FLT K2;

   for( i=Blob->Nb_Centers-1; i>=0; --i )
   {
      Transform_To_Matrix( &Blob->Components[i]->Transform, Blob->Components[i]->Mo );
      A_V( Blob->Gi[i], Blob->Components[i]->Mo, Blob->G[i] );
   }

   K2 = (FLT)sqrt( (double)( 1.0-K ) );
   Nb_Good = Nb_Remesh = 0;

   Vtx = 0;
   for( i=Blob->Nb_Centers-1; i>=0; --i )
   {
      for( j=Blob->Components[i]->Nb_Vertex-1; j>=0; --j )
      {
         VECTOR P;
         FLT R;

         nA_V( P, Blob->Components[i]->Mo, Blob->Components[i]->Vertex[j] );

#if 1
               /* Solve #1 */

         R = K2*Blob->Radius[i];
         n = 5;
         while( --n != 0 )
         {
            INT k;
            FLT V, dV;
            V = 0.0; dV = 0.0;

            for( k=0; k<Blob->Nb_Centers; ++k )
            {
               FLT x, Tmp, Dot;

               if ( k==i ) { x = R; Dot = 0.0; }
               else
               {
                  Tmp = Blob->Gi[i][0] - Blob->Gi[k][0];
                  Dot = P[0] * Tmp;
                  Tmp = R*P[0] + Tmp;
                  x  = Tmp*Tmp;

                  Tmp = Blob->Gi[i][1] - Blob->Gi[k][1];
                  Dot += P[1] * Tmp;
                  Tmp = R*P[1] + Tmp;
                  x += Tmp*Tmp;

                  Tmp = Blob->Gi[i][2] - Blob->Gi[k][2];
                  Dot += P[2] * Tmp;
                  Tmp = R*P[2] + Tmp;
                  x += Tmp*Tmp;

                  x = (FLT)sqrt( (double)x );
               }
               x /= Blob->Radius[k];
               if ( x>1.0 ) continue;
               Tmp = 1.0-x;
               V += Tmp*Tmp*Blob->Strength[k];

               x = (R+Dot) * Tmp / x;
               dV += Blob->dKi[k]*x;
            }
            /* fprintf( stderr, "R=%f  V=%f dV=%f K=%f\n", R, V, dV, K ); */
            V -= K;
            if ( V>-K_EPSILON && V<K_EPSILON ) break;
            if ( dV == 0.0 ) break;
            else R += V/dV;
         }
#else

               /* Solve #2 */
         {
            FLT Delta, C;
            VECTOR B;

            C = -K;
            B[0] = B[1] = B[2] = 0.0;

            for( n=Blob->Nb_Centers-1; n>=0; --n )
            {
               FLT N2, Tmp;
               VECTOR B2;

               if ( n==i ) continue;

               Tmp = Blob->Gi[n][0] - Blob->Gi[i][0];
               /* Tmp /= Blob->Radius[n]; */
               B2[0] = Tmp;
               N2    = Tmp*Tmp;

               Tmp = Blob->Gi[n][1] - Blob->Gi[i][1];
               /* Tmp /= Blob->Radius[n]; */
               B2[1] = Tmp;
               N2    += Tmp*Tmp;

               Tmp = Blob->Gi[n][2] - Blob->Gi[i][2];
               /* Tmp /= Blob->Radius[n]; */
               B2[2] = Tmp;
               N2    += Tmp*Tmp;

               if ( N2>1.0 ) continue;
               C += N2;
               B[0] += B2[0];
               B[1] += B2[1];
               B[2] += B2[2];
            }
            B[0] = Dot_Product( B, P );
            Delta = B[0]*B[0] - C;
            if ( Delta<0.0 ) { fprintf( stderr, "!" ); goto Zarb; }
            Delta = (FLT)sqrt( (double)Delta );
            if ( B[0] < 0.0 ) 
               R = C / ( B[0] - Delta );
            else R = B[0] + Delta;            
         }
         R *= Blob->Radius[i];

Zarb:
#endif

         if ( R>K2*Blob->Radius[i] )
         {
            /* R = K2*Blob->Radius[i];*/ /* Needs remesh... */
            Remesh[ Nb_Remesh++ ] = Vtx+j;
         }
         else Good[ Nb_Good++ ] = Vtx+j;

         R = (1.0-Blend)*R + Blend*Blob->Orig_R[Vtx+j];
         Blob->Blob->Vertex[Vtx+j][0] = R*P[0] + Blob->Gi[i][0];
         Blob->Blob->Vertex[Vtx+j][1] = R*P[1] + Blob->Gi[i][1];
         Blob->Blob->Vertex[Vtx+j][2] = R*P[2] + Blob->Gi[i][2];
      }
      Vtx += Blob->Components[i]->Nb_Vertex;
   }

/******************************************************************/
/*                      Best-fit remeshing                        */
/******************************************************************/

   for( i=Blob->Nb_Vertex-1; i>=0; --i ) V_Index[i] = 0xFFFF;

   if ( Nb_Good )
   {
      for( i=Nb_Remesh-1; i>=0; --i )
      {
         USHORT Best;
         INT I;
         FLT D;

         Best = -1;
         D = _HUGE_;

         I = Remesh[i];
         for( j=Nb_Good-1; j>=0; --j )
         {
            FLT Tmp, x;
            INT J;

            J = Good[j];
            
            Tmp = Blob->Blob->Vertex[J][0] - Blob->Blob->Vertex[I][0];
            x = Tmp*Tmp;
            Tmp = Blob->Blob->Vertex[J][1] - Blob->Blob->Vertex[I][1];
            x += Tmp*Tmp;
            Tmp = Blob->Blob->Vertex[J][2] - Blob->Blob->Vertex[I][2];
            x += Tmp*Tmp;
            if ( x<D ) { D = x; Best = J; }
            /* printf( "x:%f D:%f       ", x, D ); */
         }
         V_Index[I] = Best;
      }
   }

/******************************************************************/

   for( i=Blob->Nb_Polys-1; i>=0; --i )
   {
      Blob->Blob->Polys[i].Clipped = 0;
      Blob->Blob->Polys[i].Nb_Pts = Blob_Polys[i].Nb_Pts;
   }

   Blob->Blob->Nb_Polys = Blob->Nb_Polys;

   for( i=Blob->Nb_Polys-1; i>=0; --i )
   {
      INT j;
      for( j=Blob_Polys[i].Nb_Pts-1; j>=0; --j )
      {
         INT K;
         K = Blob_Polys[i].Pt[j];
         if ( V_Index[K] != 0xFFFF )
         {
            K = V_Index[K];
            Blob->Blob->Polys[i].Clipped++;
         }
         Blob->Blob->Polys[i].Pt[j] = K;
      }
   }

#if 0
   for( i=Blob->Nb_Polys-1; i>=0; --i )
   {
      if ( Blob->Blob->Polys[i].Clipped == Blob->Blob->Polys[i].Nb_Pts )
         Blob->Blob->Polys[i].Nb_Pts = 0;
   }   
#endif

/******************************************************************/

   Blob->Blob->Flags &= ~OBJ_HAS_NORMALS;
   Mesh_Store_Normals2( Blob->Blob );
   Average_Normals( Blob->Blob );
}

#endif   /* SOLVE_HARD */

/******************************************************************/
/******************************************************************/

#ifdef SOLVE_HARD2

EXTERN void Setup_Blob_Cst( BLOB *Blob )
{
   INT i, j;
   FLT A, C;
   VECTOR B, G;

   for( i=Blob->Nb_Centers-1; i>=0; --i )
   {      /* Dii = New center */
      VECTOR P;
      Transform_To_Matrix( &Blob->Components[i]->Transform, Blob->Components[i]->Mo );
      Add_Vector( P, ((FLT *)(&Blob->Components[i]->Mo) + 18), Blob->G[i] );
      nA_Inv_V( Blob->Gi[i], Blob->Components[i]->Mo, P );
   }

   for( i=Blob->Nb_Centers-1; i>=0; --i )
   {
      FLT Rio, Rio2;

      A = 0.0;
      Set_Vector( B, 0.0, 0.0, 0.0 );
      C = 0.0;

      Rio = Blob->Radius[i];
      Rio2 = Rio*Rio;

      for( j=Blob->Nb_Centers-1; j>=0; --j )
      {
         VECTOR Dij;
         FLT Dij2;
         FLT Rjo, Rjo2, Tmp;

         if ( i==j )
         {
            A += Blob->Strength[i]; /* ...only */
            continue;
         }

         Dij[0] = Blob->Gi[j][0] - Blob->Gi[i][0];
         Dij[1] = Blob->Gi[j][1] - Blob->Gi[i][1];
         Dij[2] = Blob->Gi[j][2] - Blob->Gi[i][2];
         Dij2 = Norm_Squared( Dij );

         Rjo = Blob->Radius[j];
         Tmp = ( Rio + Rjo );
         Tmp = Tmp*Tmp;
         if ( Dij2>Tmp ) continue;

         Rjo2 = Rjo*Rjo;
         C += Blob->Strength[j]*( Dij2/Rjo2 - 1.0 );
         A += Blob->Strength[j]*Rio2/Rjo2;
         B[0] += Blob->Strength[j]*Rio/Rjo2*Dij[0];
         B[1] += Blob->Strength[j]*Rio/Rjo2*Dij[1];
         B[2] += Blob->Strength[j]*Rio/Rjo2*Dij[2];
      }

      Blob->Ai[i] = A;
      Blob->Ci[i] = C;
      Set_Vector( Blob->Bi[i], B[0], B[1], B[2] );
   }
}

/******************************************************************/

EXTERN void Remesh_Blob( BLOB *Blob, FLT K, FLT Blend )
{
   INT i, j, n, Vtx;
   FLT K2;

   Setup_Blob_Cst( Blob );
   Vtx = 0;
   for( i=0; i<Blob->Nb_Centers; ++i )
   {

      FLT Do, A, C;
      C = Blob->Ci[i] + K;
      A = Blob->Ai[i];
      Do = A*C;

      for( j=0; j<Blob->Components[i]->Nb_Vertex; ++j )
      {
         VECTOR P;
         FLT R;

               /* New position */
         nA_V( P, Blob->Components[i]->Mo, Blob->Components[i]->Vertex[j] );

               /* Solve */
         {
            FLT Delta, B;

            B = Dot_Product( Blob->Bi[i], P );            
            Delta = B*B - Do;
            if ( Delta<0.0 ) R = 1.0;
            else
            {
               FLT Q;
               Delta = (FLT)sqrt( (double)Delta );
               if ( B>0.0 ) Q = B - Delta;
               else Q = B + Delta;
               if ( Q >= 0.0 ) R = Q / A;
               else R = C / Q;
               if ( R>1.0 ) R = 1.0;
            }
         }
         R *= Blob->Radius[i];
         /* R = (1.0-Blend)*R + Blend*Blob->Orig_R[Vtx+j]; */
         Blob->Blob->Vertex[Vtx+j][0] = R*P[0] + Blob->Gij[i][i][0];
         Blob->Blob->Vertex[Vtx+j][1] = R*P[1] + Blob->Gij[i][i][1];
         Blob->Blob->Vertex[Vtx+j][2] = R*P[2] + Blob->Gij[i][i][2];
      }
      Vtx += Blob->Components[i]->Nb_Vertex;
   }

   Blob->Blob->Flags &= ~OBJ_HAS_NORMALS;
   Average_Normals( Blob->Blob );
}

#endif   /* SOLVE_HARD2 */

/******************************************************************/
