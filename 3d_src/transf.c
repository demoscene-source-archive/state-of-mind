/***********************************************
 *              Transformations                *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"
// extern double cos( double ), sin( double );

/******************************************************************/
/******************************************************************/

EXTERN void Transform_To_Matrix( TRANSFORM *T, MATRIX2 M )
{
   double Cx, Sx, Cy, Sy, Cz, Sz, A, B;
   double Scx, Scy, Scz;

   /* Compute Scale+rotation matrix according to type of transform */

   switch( T->Type )
   {
      case TRANSF_RXYZ:

            /* Rz o Ry o Rx */

         Cx = cos( T->Rot.Rot[_X] ); Sx = sin( T->Rot.Rot[_X] );
         Cy = cos( T->Rot.Rot[_Y] ); Sy = sin( T->Rot.Rot[_Y] );
         Cz = cos( T->Rot.Rot[_Z] ); Sz = sin( T->Rot.Rot[_Z] );
         A = Cz*Sy;
         B = Sz*Sy;

         M[0] = M[9]  = Cz*Cy;
         M[1] = M[12] = (A*Sx - Sz*Cx);
         M[2] = M[15] = (A*Cx + Sz*Sx);

         M[3] = M[10] = Sz*Cy;
         M[4] = M[13] = (B*Sx + Cz*Cx);
         M[5] = M[16] = (B*Cx - Cz*Sx);

         M[6] = M[11] = -Sy;
         M[7] = M[14] =  Sx*Cy;
         M[8] = M[17] =  Cx*Cy;

      break;

      case TRANSF_RZYX:
         Cx = cos( T->Rot.Rot[_X] ); Sx = sin( T->Rot.Rot[_X] );
         Cy = cos( T->Rot.Rot[_Y] ); Sy = sin( T->Rot.Rot[_Y] );
         Cz = cos( T->Rot.Rot[_Z] ); Sz = sin( T->Rot.Rot[_Z] );
         A = Cz*Sy;
         B = Sz*Sy;

         M[0] = M[9]  =  Cy*Cz;
         M[1] = M[12] = -Cy*Sz;
         M[2] = M[15] =  Sy;

         M[3] = M[10] = (Sz*Cx + A*Sx);
         M[4] = M[13] = (Cz*Cx - B*Sx);
         M[5] = M[16] = -Cy*Sx;

         M[6] = M[11] = (Sz*Sx - A*Cx);
         M[7] = M[14] = (Cz*Sx + B*Cx);
         M[8] = M[17] = Cy*Cx;

      break;

      case TRANSF_EULER:
      break;

      case TRANSF_QUATERNION:
         Quaternion_To_Matrix( (FLT *)&T->Rot.QRot, M );
      break;
      default: break;   // error
   }

   Scx = T->Scale[_X]; Scy = T->Scale[_Y]; Scz = T->Scale[_Z];
   M[0] *= Scx; M[3] *= Scx; M[6] *= Scx;
   M[1] *= Scy; M[4] *= Scy; M[7] *= Scy;
   M[2] *= Scz; M[5] *= Scz; M[8] *= Scz;

   Scx = 1.0/T->Scale[_X]; Scy = 1.0/T->Scale[_Y]; Scz = 1.0/T->Scale[_Z];
   M[ 9] *= Scx; M[10] *= Scx; M[11] *= Scx;
   M[12] *= Scy; M[13] *= Scy; M[14] *= Scy;
   M[15] *= Scz; M[16] *= Scz; M[17] *= Scz;

   Add_Vector( MATRIX_TRANSL(M), T->Pos, T->Pivot );
   M[21] = -( M[ 9]*M[18] + M[10]*M[19] + M[11]*M[20] );
   M[22] = -( M[12]*M[18] + M[13]*M[19] + M[14]*M[20] );
   M[23] = -( M[15]*M[18] + M[16]*M[19] + M[17]*M[20] );
}

EXTERN void Id_Transform( TRANSFORM *T )
{
   Set_Vector( T->Pos, 0.0, 0.0, 0.0 );
   Set_Vector( T->Scale, 1.0, 1.0, 1.0 );
//   T->Type = TRANSF_RXYZ;
//   T->Rot.QRot[0] = T->Rot.QRot[1] = T->Rot.QRot[2] = 0.0;
//   T->Rot.QRot[3] = 1.0;
   T->Type = TRANSF_QUATERNION;
   T->Rot.QRot[0] = T->Rot.QRot[1] = T->Rot.QRot[2] = 0.0;
   T->Rot.QRot[3] = -1.0;
}

EXTERN TRANSFORM *New_Transform( TRANSFORM *T )
{
   if ( T==NULL ) 
   {
      T = New_Object( 1, TRANSFORM );
      if ( T==NULL ) return( NULL );
      Mem_Clear( T );
   }
   Id_Transform( T );
   return( T );
} 

/******************************************************************/
/*                Matrix/vector utils                             */
/******************************************************************/

#ifdef UNIX
EXTERN INLINE void Raw_A_V( VECTOR Out, MATRIX A, VECTOR In )
{
      /* M = A.V */

   Out[0] = ( A[0]*In[0] + A[1]*In[1] + A[2]*In[2] );
   Out[1] = ( A[3]*In[0] + A[4]*In[1] + A[5]*In[2] );
   Out[2] = ( A[6]*In[0] + A[7]*In[1] + A[8]*In[2] );
}
EXTERN INLINE void Raw_tA_V( VECTOR Out, MATRIX A, VECTOR In )
{
      /* M = A.V */

   Out[0] = ( A[0]*In[0] + A[3]*In[1] + A[6]*In[2] );
   Out[1] = ( A[1]*In[0] + A[4]*In[1] + A[7]*In[2] );
   Out[2] = ( A[2]*In[0] + A[5]*In[1] + A[8]*In[2] );
}
#endif

#ifdef UNIX
EXTERN INLINE void A_V( VECTOR Out, MATRIX2 A, VECTOR In )
{
      /* M = A.V */

   Out[0] = ( A[0]*In[0] + A[1]*In[1] + A[2]*In[2] ) + A[18];
   Out[1] = ( A[3]*In[0] + A[4]*In[1] + A[5]*In[2] ) + A[19];
   Out[2] = ( A[6]*In[0] + A[7]*In[1] + A[8]*In[2] ) + A[20];
}

EXTERN INLINE void A_V_Eq( VECTOR In, MATRIX2 A )
{
   VECTOR Tmp;
   A_V( Tmp, A, In );
   Assign_Vector( In, Tmp );
}
EXTERN INLINE void nA_V( VECTOR Out, MATRIX A, VECTOR In )
{
      /* M = t(A^-1).V */

   Out[0] = ( A[ 9]*In[0] + A[12]*In[1] + A[15]*In[2] );
   Out[1] = ( A[10]*In[0] + A[13]*In[1] + A[16]*In[2] );
   Out[2] = ( A[11]*In[0] + A[14]*In[1] + A[17]*In[2] );
}
EXTERN INLINE void nA_V_Eq( VECTOR In, MATRIX A )
{
   VECTOR Tmp;
   nA_V( Tmp, A, In );
   Assign_Vector( In, Tmp );
}
EXTERN INLINE void A_Inv_V( VECTOR Out, MATRIX A, VECTOR In )
{
      /* M = (A^-1).V */

   Out[0] = ( A[ 9]*In[0] + A[10]*In[1] + A[11]*In[2] ) + A[21];
   Out[1] = ( A[12]*In[0] + A[13]*In[1] + A[14]*In[2] ) + A[22];
   Out[2] = ( A[15]*In[0] + A[16]*In[1] + A[17]*In[2] ) + A[23];
}
EXTERN INLINE void A_Inv_V_Eq( VECTOR In, MATRIX A )
{
   VECTOR Tmp;
   A_Inv_V( Tmp, A, In );
   Assign_Vector( In, Tmp );
}

EXTERN INLINE void nA_Inv_V( VECTOR Out, MATRIX A, VECTOR In )
{
      /* M = tA.V */
   Out[0] = ( A[0]*In[0] + A[3]*In[1] + A[6]*In[2] );
   Out[1] = ( A[1]*In[0] + A[4]*In[1] + A[7]*In[2] );
   Out[2] = ( A[2]*In[0] + A[5]*In[1] + A[8]*In[2] );
}
EXTERN INLINE void nA_Inv_V_Eq( VECTOR In, MATRIX A )
{
   VECTOR Tmp;
   nA_Inv_V( Tmp, A, In );
   Assign_Vector( In, Tmp );
}
#endif

/******************************************************************/
/*                      Matrix utils                              */
/******************************************************************/

EXTERN INLINE void Mult_Matrix_3x3( MATRIX3x3 A, MATRIX3x3 B, MATRIX3x3 C )
{
      /* A = B.C */
   A[0] = B[0]*C[0] + B[1]*C[3] + B[2]*C[6];
   A[1] = B[0]*C[1] + B[1]*C[4] + B[2]*C[7];
   A[2] = B[0]*C[2] + B[1]*C[5] + B[2]*C[8];
   A[3] = B[3]*C[0] + B[4]*C[3] + B[5]*C[6];
   A[4] = B[3]*C[1] + B[4]*C[4] + B[5]*C[7];
   A[5] = B[3]*C[2] + B[4]*C[5] + B[5]*C[8];
   A[6] = B[6]*C[0] + B[7]*C[3] + B[8]*C[6];
   A[7] = B[6]*C[1] + B[7]*C[4] + B[8]*C[7];
   A[8] = B[6]*C[2] + B[7]*C[5] + B[8]*C[8];
}

EXTERN INLINE void Mult_Matrix( MATRIX A, MATRIX B )
{
   MATRIX C;

      /* A.B=>B */

   Mult_Matrix_3x3( C, A, B );
   Mult_Matrix_3x3( MATRIX_INV(C), MATRIX_INV(B), MATRIX_INV(A) );
   memcpy( B, C, sizeof( MATRIX ) );
}

EXTERN INLINE void Mult_Matrix2_AB( MATRIX2 A, MATRIX2 B )
{
   MATRIX C;
   VECTOR V;
      /* B = A.B */
   Mult_Matrix_3x3( C, A, B );
   Mult_Matrix_3x3( MATRIX_INV(C), MATRIX_INV(B), MATRIX_INV(A) );

   A_V_Eq( MATRIX_TRANSL(B), A );            // Tb = A.Tb + Ta
   A_Inv_V( V, B, MATRIX_INV_TRANSL(A) );    // V = B^-1.( -(A^-1)Ta ) -(B^-1).Tb
   Assign_Vector( MATRIX_INV_TRANSL(B), V ); // => -(AB)^-1.Ta - B^-1.Tb
   memcpy( B, C, sizeof( MATRIX )  );
}

EXTERN INLINE void Mult_Matrix2_BA( MATRIX2 A, MATRIX2 B )
{
   MATRIX C;
   VECTOR V;

      /* A = A.B */
   Mult_Matrix_3x3( C, A, B );
   Mult_Matrix_3x3( MATRIX_INV(C), MATRIX_INV(B), MATRIX_INV(A) );

   A_Inv_V_Eq( MATRIX_INV_TRANSL(A), B );
   A_V( V, A, MATRIX_TRANSL(B) );         // Tb = A.Tb + Ta
   Assign_Vector( MATRIX_TRANSL(A), V );  // => -(AB)^-1.Ta - B^-1.Tb
   memcpy( A, C, sizeof( MATRIX ) );
}

EXTERN INLINE void Mult_Matrix_ABC( MATRIX2 A, MATRIX2 B, MATRIX2 C )
{
      /* C = A.B */
   Mult_Matrix_3x3( C, A, B );
   Mult_Matrix_3x3( MATRIX_INV(C), MATRIX_INV(B), MATRIX_INV(A) );

   A_V( MATRIX_TRANSL(C), A, MATRIX_TRANSL(B) );
   A_Inv_V( MATRIX_INV_TRANSL(C), B, MATRIX_INV_TRANSL(A) );
}

EXTERN INLINE void Id_Matrix( MATRIX M )
{
   M[9]  = M[0] = M[13] = M[4] = M[17] = M[8] = 1.0;
   M[10] = M[1] = M[11] = M[2] = M[12] = M[3] = 0.0;
   M[14] = M[5] = M[15] = M[6] = M[16] = M[7] = 0.0;
}

EXTERN INLINE void Id_Matrix2( MATRIX2 M )
{
   Id_Matrix( M );
   M[18] = M[19] = M[20] = 0.0;
   M[21] = M[22] = M[23] = 0.0;
}

EXTERN void Scale_Matrix( MATRIX2 M, FLT Sx, FLT Sy, FLT Sz )
{
   Id_Matrix2( M );

   if ( Sx==0.0 ) Sx = 1.0f;     // sure-fire hack...
   if ( Sy==0.0 ) Sy = 1.0f;
   if ( Sz==0.0 ) Sz = 1.0f;

   M[0] = Sx; M[4] = Sy; M[8] = Sz;
   M[9] = 1.0f/Sx; M[13] = 1.0f/Sy; M[17] = 1.0f/Sz;
}

/******************************************************************/

EXTERN void Axis_Rotation( VECTOR Axis, FLT Alpha, MATRIX2 M )
{
   FLT l, Cx, Sx;
   VECTOR D;

         // computes the matrix for a rot of Alpha radians
         // around vector Axis.

   l  = Axis[0]*Axis[0];
   l += Axis[1]*Axis[1];
   l += Axis[2]*Axis[2];
   if ( l==0.0 ) { Id_Matrix( M ); return; }
   l = (FLT)( 1.0/sqrt( l ) );
   D[0] = Axis[0]*l; 
   D[1] = Axis[1]*l; 
   D[2] = Axis[2]*l;

   Cx = (FLT)cos( Alpha ); Sx = (FLT)sin( Alpha );

   M[0] = M[9]  = D[0]*D[0]*(1.0f-Cx) + Cx;
   M[1] = M[12] = D[1]*D[0]*(1.0f-Cx) + D[2]*Sx;
   M[2] = M[15] = D[2]*D[0]*(1.0f-Cx) - D[1]*Sx;

   M[3] = M[10] = D[0]*D[1]*(1.0f-Cx) - D[2]*Sx;
   M[4] = M[13] = D[1]*D[1]*(1.0f-Cx) + Cx;
   M[5] = M[16] = D[2]*D[1]*(1.0f-Cx) + D[0]*Sx;

   M[6] = M[11] = D[0]*D[2]*(1.0f-Cx) + D[1]*Sx;
   M[7] = M[14] = D[1]*D[2]*(1.0f-Cx) - D[0]*Sx;
   M[8] = M[17] = D[2]*D[2]*(1.0f-Cx) + Cx;

   M[18] = M[19] = M[20] = 0.0;
   M[21] = M[22] = M[23] = 0.0;
}

/******************************************************************/
