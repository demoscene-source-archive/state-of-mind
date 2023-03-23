/***********************************************
 *             Quaternion stuff                *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#include "main3d.h"

#define _X_ 1
#define _Y_ 2
#define _Z_ 3
#define _W_ 0
#define Q_EPSILON (1.0e-5f)
#define TRACE_EPSILON (0.01f)

/******************************************************************/

static INT _Next[4] = { -1, 2, 3, 1 };
static INT _Diag[4] = { -1, 0, 4, 8 };

EXTERN void Quaternion_To_Matrix( QUATERNION Q, MATRIX M )
{
   FLT S, xs, ys, zs;
   FLT wx, wy, wz;
   FLT xx, yy, zz;
   FLT xy, xz, yz;

   xx = Q[_X_]*Q[_X_]; yy = Q[_Y_]*Q[_Y_]; zz = Q[_Z_]*Q[_Z_];
   S = 2.0f / ( xx + yy + zz + Q[_W_]*Q[_W_] );
   xx *= S; yy *= S; zz *= S;
   xs = Q[_X_]*S; wx = Q[_W_]*xs;  
   ys = Q[_Y_]*S; wy = Q[_W_]*ys; xy = Q[_X_]*ys;
   zs = Q[_Z_]*S; wz = Q[_W_]*zs; xz = Q[_X_]*zs; yz = Q[_Y_]*zs;

   M[9]  = M[0] = 1.0f - (yy+zz);
   M[10] = M[3] = xy + wz;
   M[11] = M[6] = xz - wy;

   M[12] = M[1] = xy - wz;
   M[13] = M[4] = 1.0f - (xx+zz);
   M[14] = M[7] = yz + wx;

   M[15] = M[2] = xz + wy;
   M[16] = M[5] = yz - wx;
   M[17] = M[8] = 1.0f - (xx+yy);
}

EXTERN void Matrix_To_Quaternion( MATRIX M, QUATERNION Q )
{
   FLT Trace;

      /* M *must* be a pure rotation matrix ( 1.0+Tr=4.W^2 )... */

   Trace = 1.0f + M[0]+M[4]+M[8];
   if ( Trace > TRACE_EPSILON )      // Epsilon ?
   {
      FLT S;
      S = (FLT)sqrt( Trace );   // 2.W
      Q[_W_] = 0.5f*S;        // W
      S = 0.5f/S;           // 1/(4W)
      Q[_X_] = ( M[7]-M[5] )*S;
      Q[_Y_] = ( M[2]-M[6] )*S;
      Q[_Z_] = ( M[3]-M[1] )*S;
   }
   else
   {
      INT i,j,k;
      double S;

      i = 1;
      if ( M[4]>M[0] ) i=2;
      if ( M[8]>M[_Diag[i]] ) i=3;
      j = _Next[i]; k = _Next[j];
      S = sqrt( 2.0*(1.0+(double)M[_Diag[i]]) );  // 2.xi
      Q[i] = 0.5f*S;  // xi
      S = 1.0/S;
      switch( i )
      {
         case _X_:
            Q[_Y_] = M[1]*S; Q[_Z_] = M[2]*S;
         break;
         case _Y_:
            Q[_Z_] = M[5]*S; Q[_X_] = M[3]*S;
         break;
         case _Z_:
            Q[_X_] = M[6]*S; Q[_Y_] = M[7]*S;
         break;
      }
      Q[_W_] = 0.0;
   }
   Trace = Q[_W_]*Q[_W_] + Q[_X_]*Q[_X_] + Q[_Y_]*Q[_Y_] + Q[_Z_]*Q[_Z_];
   if ( Trace<0.0001f )
   {
//      fprintf( stderr, "QNorm = 0.0!!!!\n" );
      Q[_W_] = 0.0;
      Q[_X_] = 1.0f;
      Q[_Y_] = 0.0;
      Q[_Z_] = 0.0;
   }
   else
   {
      Trace = (FLT)( 1.0/sqrt( Trace ) );
      Q[_W_] *= Trace;
      Q[_X_] *= Trace;
      Q[_Y_] *= Trace;
      Q[_Z_] *= Trace;
   }
}

/******************************************************************/

EXTERN void Quaternion_Linear_Interp( QUATERNION Q, 
   QUATERNION Q1, QUATERNION Q2, FLT t )
{
   FLT Dot;
   FLT Scale_Q1, Scale_Q2;
   FLT Flip;

      // SLERP. Linearly interpolate between Q1 and Q2

   Dot = Q1[_X_]*Q2[_X_] + Q1[_Y_]*Q2[_Y_] + Q1[_Z_]*Q2[_Z_];
   Dot += Q1[_W_]*Q2[_W_];

   if ( Dot>=0.0 ) Flip = 1.0;
   else { 
//      Dot = -Dot; 
      Flip = -1.0f;
   }

   if ( (1.0f+Dot) > Q_EPSILON )
   {
      if ( (1.0f-Dot)>Q_EPSILON )
      {
         FLT Omega, Sin;
         Omega = (FLT)acos( Dot );
         Sin = 1.0f/sin( Omega );
         Scale_Q1 = (FLT)sin( (1.0f-t)*Omega ) * Sin;
         Scale_Q2 = (FLT)sin( t*Omega ) * Sin;
      }
      else
      {
         Scale_Q1 = 1.0f - t;
         Scale_Q2 = t;
      }
//      Scale_Q2 *= Flip;
      Q[_W_] = Scale_Q1*Q1[_W_] + Scale_Q2*Q2[_W_];
      Q[_X_] = Scale_Q1*Q1[_X_] + Scale_Q2*Q2[_X_];
      Q[_Y_] = Scale_Q1*Q1[_Y_] + Scale_Q2*Q2[_Y_];
      Q[_Z_] = Scale_Q1*Q1[_Z_] + Scale_Q2*Q2[_Z_];
   }
   else
   {
#if 0
      Q[_W_] = Q1[_Z_]; Q[_Z_] = -Q1[_W_];
      Q[_X_] = -Q1[_Y_]; Q[_Y_] = Q1[_X_];
      Scale_Q1 = sin( (1.0-t)*(M_PI/2.0f) );
      Scale_Q2 = sin( t*(M_PI/2.0f) );
      Q[_X_] = Scale_Q1*Q1[_X_] + Scale_Q2*Q[_X_];
      Q[_Y_] = Scale_Q1*Q1[_Y_] + Scale_Q2*Q[_Y_];
      Q[_Z_] = Scale_Q1*Q1[_Z_] + Scale_Q2*Q[_Z_];
#endif
      FLT Omega, Sin;

      Omega = (FLT)acos( Q1[_W_] );
      Sin = (FLT)sin( Omega );
      if ( Sin<Q_EPSILON )
      {
         Q[_W_] = 0.0;
         Q[_X_] = 1.0f;
         Q[_Y_] = 0.0;
         Q[_Z_] = 0.0;
      }
      else
      {
         FLT th;
         th = 1.0f / tan( Omega );
         Q[_W_] =-Sin;
         Q[_X_] = th*Q1[_X_];
         Q[_Y_] = th*Q1[_Y_];
         Q[_Z_] = th*Q1[_Z_];
      }
      Scale_Q1 = cos( t*M_PI ); Scale_Q2 = sin( t*M_PI );
      Q[_X_] = Scale_Q1*Q1[_X_] + Scale_Q2*Q[_X_];
      Q[_Y_] = Scale_Q1*Q1[_Y_] + Scale_Q2*Q[_Y_];
      Q[_Z_] = Scale_Q1*Q1[_Z_] + Scale_Q2*Q[_Z_];
      Q[_W_] = Scale_Q1*Q1[_W_] + Scale_Q2*Q[_W_];
   }
}

/******************************************************************/

EXTERN void Quaternion_To_Theta_V( QUATERNION Out, QUATERNION In )
{
   FLT N;

   N = (FLT)sqrt( In[_X_]*In[_X_] + In[_Y_]*In[_Y_] + In[_Z_]*In[_Z_] );

   if ( N==0.0 )  // => no axis...really?
   {
      Out[_W_] = 0.0;
      Out[_X_] = 1.0f;
      Out[_Y_] = 0.0;
      Out[_Z_] = 0.0; 
      return;
   }

   if ( N>1.1 ) {
//      fprintf( stderr, "\n Unormalized!!! N=%f\n", N );
      N = 1.0f;
      Out[_W_] = M_PI;
   }
   else
   {
//      Out[_W_] = 2.0*asin( N );        // asin() is in ]-Pi/2; Pi/2 ]
         // Out[_W_] = 2.0*acos( In[_W_] );     // acos() is in ]0,Pi]

      Out[_W_] = 2.0*atan2( N, In[_W_] );     // atan2() is in ]-Pi,Pi]
                                              // Out[_W_] is in ]0,Pi] since N>0
   }
   N = 1.0f/N;
   Out[_X_] = In[_X_]*N;
   Out[_Y_] = In[_Y_]*N;
   Out[_Z_] = In[_Z_]*N;
}

EXTERN FLT Theta_V_To_Quaternion( QUATERNION Out, QUATERNION In )
{
   double Tmp, N;

   Tmp = (double)In[_W_];
   Out[_W_] = cos( Tmp/2.0 );
   Tmp = sin( Tmp/2.0 );
   N = In[_X_]*In[_X_] + In[_Y_]*In[_Y_] + In[_Z_]*In[_Z_];
   if ( N==0.0 )  // => no axis...really?
   {
//      fprintf( stderr, "No axis (%s)!!\n", Cur_Node->Name );
         // some rotation keys are zeroed in 3ds!! => Matrix = Id
      Out[_W_] = 1.0f;
      Out[_X_] = 0.0;
      Out[_Y_] = 0.0;
      Out[_Z_] = 0.0; 
   }
   else
   {
      Tmp /= sqrt( N );
      Out[_X_] = Tmp*In[_X_];
      Out[_Y_] = Tmp*In[_Y_];
      Out[_Z_] = Tmp*In[_Z_];
   }
#if 0
   if ( In[_W_]>M_PI )
   {
      Out[_W_] = -Out[_W_];
      Out[_X_] = -Out[_X_];
      Out[_Y_] = -Out[_Y_];
      Out[_Z_] = -Out[_Z_];
   }
#endif
   return( (FLT)N );
}

/******************************************************************/

EXTERN void Mult_Quat_Eq( QUATERNION A, QUATERNION B )
{
   /* A = A.B */
   QUATERNION Tmp;
   Tmp[0] = A[0]*B[0] - A[1]*B[1] - A[2]*B[2] - A[3]*B[3];
   Tmp[1] = A[0]*B[1] + A[1]*B[0] + A[2]*B[3] - A[3]*B[2];
   Tmp[2] = A[0]*B[2] + A[2]*B[0] + A[3]*B[1] - A[1]*B[3];
   Tmp[3] = A[0]*B[3] + A[3]*B[0] + A[1]*B[2] - A[2]*B[1];
   A[0] = Tmp[0];
   A[1] = Tmp[1];
   A[2] = Tmp[2];
   A[3] = Tmp[3];
}

#if 0       // useless, actually

EXTERN void Mult_Quat_Conj( QUATERNION C, QUATERNION A, QUATERNION B )
{
   /* C = tA.B */
   C[0] = A[0]*B[0] + A[1]*B[1] + A[2]*B[2] + A[3]*B[3];
   C[1] = A[0]*B[1] - A[1]*B[0] - A[2]*B[3] + A[3]*B[2];
   C[2] = A[0]*B[2] - A[2]*B[0] - A[3]*B[1] + A[1]*B[3];
   C[3] = A[0]*B[3] - A[3]*B[0] - A[1]*B[2] + A[2]*B[1];
}
EXTERN void Mult_Quat( QUATERNION C, QUATERNION A, QUATERNION B )
{
   /* C = A.B */
   C[0] = A[0]*B[0] - A[1]*B[1] - A[2]*B[2] - A[3]*B[3];
   C[1] = A[0]*B[1] + A[1]*B[0] + A[2]*B[3] - A[3]*B[2];
   C[2] = A[0]*B[2] + A[2]*B[0] + A[3]*B[1] - A[1]*B[3];
   C[3] = A[0]*B[3] + A[3]*B[0] + A[1]*B[2] - A[2]*B[1];
}

EXTERN void Mult_Quat_Eq_BA( QUATERNION A, QUATERNION B )
{
   /* A = B.A */
   QUATERNION Tmp;
   Tmp[0] = B[0]*A[0] - B[1]*A[1] - B[2]*A[2] - B[3]*A[3];
   Tmp[1] = B[0]*A[1] + B[1]*A[0] + B[2]*A[3] - B[3]*A[2];
   Tmp[2] = B[0]*A[2] + B[2]*A[0] + B[3]*A[1] - B[1]*A[3];
   Tmp[3] = B[0]*A[3] + B[3]*A[0] + B[1]*A[2] - B[2]*A[1];
   A[0] = Tmp[0];
   A[1] = Tmp[1];
   A[2] = Tmp[2];
   A[3] = Tmp[3];
}
#endif

/******************************************************************/

EXTERN void Mult_Quat_Eq_Theta_V( QUATERNION A, QUATERNION B )
{
      // A = (s1,V1), B=(s2,V2)  
      // =>  B = (s1,V1).(s2,V2) = ( s1.s2-V1.V2, s1.V2 + s2.V1 + V1 ^ V2 )

   QUATERNION Tmp;

   Tmp[_W_] = A[_W_]*B[_W_] - Dot_Product( &A[_X_], &B[_X_] );
   Cross_Product( &Tmp[_X_], &A[_X_], &B[_X_] );
   Tmp[_X_] += A[_W_]*B[_X_] + B[_W_]*A[_X_];
   Tmp[_Y_] += A[_W_]*B[_Y_] + B[_W_]*A[_Y_];
   Tmp[_Z_] += A[_W_]*B[_Z_] + B[_W_]*A[_Z_];
   B[0] = Tmp[0];
   B[1] = Tmp[1];
   B[2] = Tmp[2];
   B[3] = Tmp[3];
}

/******************************************************************/

#if 0
EXTERN void Q_Print_All( QUATERNION  Q )
{
   QUATERNION  Q2;
   Out_Message( "Q: (%lf %lf %lf %lf) => Norm:%lf", 
      Q[0], Q[1], Q[2], Q[3],
      sqrt( Q[0]*Q[0] + Q[1]*Q[1] + Q[2]*Q[2] + Q[3]*Q[3] ) );

   Quaternion_To_Theta_V( Q2, Q );
   Out_Message( " =>: (%lf %lf %lf %lf) |V|=%lf", 
      Q2[0], Q2[1], Q2[2], Q2[3],
      sqrt( Q2[1]*Q2[1] + Q2[2]*Q2[2] + Q2[3]*Q2[3] ) );
}
#endif

/******************************************************************/
/******************************************************************/

#if 0    // needed in move.c

EXTERN void Setup_SQuad( QUATERNION A, QUATERNION Prev, QUATERNION Cur, QUATERNION Next )
{
   QUATERNION X1, X2, Tmp;
   FLT Theta;

/*
   Q_Print_All( Prev );
   Q_Print_All( Cur );
   Q_Print_All( Next );
*/
   Mult_Quat_Conj( X1, Cur, Next );
   Quaternion_To_Theta_V( X1, X1 );
   Mult_Quat_Conj( X2, Cur, Prev );
   Quaternion_To_Theta_V( X2, X2 );
   Tmp[1] = -.25*( X1[0]*X1[1] + X2[0]*X2[1] );
   Tmp[2] = -.25*( X1[0]*X1[2] + X2[0]*X2[2] );
   Tmp[3] = -.25*( X1[0]*X1[3] + X2[0]*X2[3] );
   Theta = sqrt( Tmp[1]*Tmp[1] + Tmp[2]*Tmp[2] + Tmp[3]*Tmp[3] );
   Tmp[0] = cos( Theta );
   Theta = sin(Theta)/Theta;
   Tmp[1] *= Theta;
   Tmp[2] *= Theta;
   Tmp[3] *= Theta;
   Mult_Quat( A, Cur, Tmp );   
/*   Q_Print_All( A ); */

}

#endif   // 0

/******************************************************************/


