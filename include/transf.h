/***********************************************
 *              Transformations                *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _TRANSF_H_
#define _TRANSF_H_

/******************************************************************/

#define _X  0
#define _Y  1
#define _Z  2
#define _W  3  /* for Quaternions */

#define _D  3  /* for planes */
#define _U  0
#define _V  1

#define M_PI_180   ( M_PI/180.0f )

typedef FLT    PT_2D[2];
typedef FLT    VECTOR[3];
typedef FLT    QUATERNION[4];
typedef FLT    F_MAPPING[2];
typedef USHORT I_MAPPING[2];

typedef FLT MATRIX[9+9];     // 9 for direct matrix, 9 for inv matrix...
typedef FLT MATRIX2[9+9+6];  // 9 for direct, 9 for inv, 3+3 for translation
typedef FLT MATRIX3x3[9];
#define MATRIX_INV(M)         ((FLT*)&(M)[9])
#define MATRIX_TRANSL(M)      ((FLT*)&(M)[18])
#define MATRIX_INV_TRANSL(M)  ((FLT*)&(M)[21])

typedef FLT PLANE[4];
typedef VECTOR PATCH[16];


#define Copy_Type(D,S)   memcpy( (D), (S), sizeof( *(S) ) )
#define Copy_Type2(D,S)   memcpy( (D), (S), sizeof( (S) ) )

#define Assign_Vector(A,B) { (A)[0]=(B)[0];(A)[1]=(B)[1];(A)[2]=(B)[2]; }
#define Set_Vector(V,x,y,z) { (V)[0] = (x); (V)[1] = (y); (V)[2] = (z); }
#define Add_Vector(A,B,C) { (A)[0] = (B)[0]+(C)[0]; (A)[1] = (B)[1]+(C)[1]; (A)[2] = (B)[2]+(C)[2]; }
#define Sub_Vector(A,B,C) { (A)[0] = (B)[0]-(C)[0]; (A)[1] = (B)[1]-(C)[1]; (A)[2] = (B)[2]-(C)[2]; }
#define Add_Vector_Eq(A,B) { (A)[0] += (B)[0]; (A)[1] += (B)[1]; (A)[2] += (B)[2]; }
#define Sub_Vector_Eq(A,B) { (A)[0] -= (B)[0]; (A)[1] -= (B)[1]; (A)[2] -= (B)[2]; }
#define Scale_Vector_Eq(A,B) { (A)[0]*=(B);(A)[1]*=(B);(A)[2]*=(B); }
#define Scale_Vector(A,B,C) { (A)[0]=(C)*(B)[0]; (A)[1]=(C)*(B)[1];(A)[2]=(C)*(B)[2]; }
#define Add_Scaled_Vector_Eq(A,B,C) { (A)[0]+=(C)*(B)[0]; (A)[1]+=(C)*(B)[1];(A)[2]+=(C)*(B)[2]; }
#define Sub_Scaled_Vector_Eq(A,B,C) { (A)[0]-=(C)*(B)[0]; (A)[1]-=(C)*(B)[1];(A)[2]-=(C)*(B)[2]; }
#define Add_Scaled_Vector(A,B,C,D) { (A)[0]=(B)[0]+(D)*(C)[0];(A)[1]=(B)[1]+(D)*(C)[1];(A)[2]=(B)[2]+(D)*(C)[2];}

#define Cross_Product(A,B,C) {        \
   (A)[0] = (B)[1]*(C)[2] - (B)[2]*(C)[1];    \
   (A)[1] = (B)[2]*(C)[0] - (B)[0]*(C)[2];    \
   (A)[2] = (B)[0]*(C)[1] - (B)[1]*(C)[0];    \
}

#define Dot_Product(B,C) ( (B)[0]*(C)[0] + (B)[1]*(C)[1] +  (B)[2]*(C)[2] )
#define Norm_Squared(A) ( (A)[0]*(A)[0] + (A)[1]*(A)[1] + (A)[2]*(A)[2] )
#define Normalize(P) { FLT _Tmp = 1.0f/(FLT)sqrt( (double)Norm_Squared(P) ); \
   P[0] *= _Tmp; P[1] *= _Tmp; P[2] *= _Tmp; }
#define Normalize_Safe(P) { FLT _Tmp = (FLT)sqrt( (double)Norm_Squared(P) ); \
   if ( _Tmp>1.0e-6 ) { _Tmp = 1.0f/_Tmp; P[0] *= _Tmp; P[1] *= _Tmp; P[2] *= _Tmp; } }

/******************************************************************/
/******************************************************************/

typedef struct {
#pragma pack(1)

   OBJ_TYPE Type;
   union {
      VECTOR Rot; /* (Rx, Ry, Rz)   or   (Theta,Phi,Omega) */
      QUATERNION QRot;
   } Rot;
   VECTOR Scale;
   VECTOR Pos;
   VECTOR Pivot;

} TRANSFORM;

/******************************************************************/

extern void Transform_To_Matrix( TRANSFORM *T, MATRIX2 M );
//extern void Transform_Inv_To_Matrix( TRANSFORM *T, MATRIX2 M );
extern TRANSFORM *New_Transform( TRANSFORM *T );
extern void Id_Transform( TRANSFORM *T );

extern void Raw_A_V( VECTOR M, MATRIX2 A, VECTOR V );
extern void Raw_tA_V( VECTOR M, MATRIX2 A, VECTOR V );

extern void A_V( VECTOR M, MATRIX2 A, VECTOR V );
extern void A_Inv_V( VECTOR M, MATRIX2 A, VECTOR V );
extern void nA_V( VECTOR M, MATRIX A, VECTOR V );
extern void nA_Inv_V( VECTOR M, MATRIX A, VECTOR V );

#ifdef UNIX
extern void A_V_Eq( VECTOR M, MATRIX2 A );
extern void A_Inv_V( VECTOR M, MATRIX2 A, VECTOR V );
extern void nA_V_Eq( VECTOR M, MATRIX2 A );
extern void nA_Inv_V_Eq( VECTOR M, MATRIX2 A );
#else
#define A_V_Eq(V,A)  A_V( (V),(A),(V) )
#define A_Inv_V_Eq(V,A)  A_Inv_V( (V),(A),(V) )
#define nA_V_Eq(V,A)  nA_V( (V),(A),(V) )
#define nA_Inv_V_Eq(V,A)  nA_Inv_V( (V),(A),(V) )
#endif

extern void Mult_Matrix_3x3( MATRIX3x3 C, MATRIX3x3 A, MATRIX3x3 B );
extern void Mult_Matrix( MATRIX A, MATRIX B );
extern void Mult_Matrix2_AB( MATRIX2 A, MATRIX2 B );
extern void Mult_Matrix2_BA( MATRIX2 A, MATRIX2 B );
extern void Mult_Matrix_ABC( MATRIX2 A, MATRIX2 B, MATRIX2 C );

extern void Id_Matrix( MATRIX M );
extern void Id_Matrix2( MATRIX2 M );

extern void Scale_Matrix( MATRIX2 M, FLT Sx, FLT Sy, FLT Sz );
extern void Axis_Rotation( VECTOR Axis, FLT Alpha, MATRIX2 M );

/******************************************************************/

#endif   // _TRANSF_H_
