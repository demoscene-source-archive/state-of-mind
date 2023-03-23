/***********************************************
 *             Quaternion stuff                *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _QUAT_H_
#define _QUAT_H_

/******************************************************************/

extern void Quaternion_To_Matrix( QUATERNION, MATRIX );
extern void Matrix_To_Quaternion( MATRIX, QUATERNION );

extern void Mult_Quat_Eq( QUATERNION A, QUATERNION B );
extern void Mult_Quat_Eq_BA( QUATERNION A, QUATERNION B );
extern void Mult_Quat( QUATERNION C, QUATERNION A, QUATERNION B );
extern void Mult_Quat_Conj( QUATERNION C, QUATERNION A, QUATERNION B );
extern FLT Theta_V_To_Quaternion( QUATERNION Out, QUATERNION In );
extern void Quaternion_To_Theta_V( QUATERNION Out, QUATERNION In );
extern void Q_Print_All( QUATERNION  Q );

extern void Quaternion_Linear_Interp( QUATERNION Q, QUATERNION Q1, QUATERNION Q2, FLT t );
// extern void Setup_SQuad( QUATERNION A, QUATERNION Prev, QUATERNION Cur, QUATERNION Next );

extern void Mult_Quat_Eq_Theta_V( QUATERNION A, QUATERNION B );

/******************************************************************/

#endif   // _QUAT_H_
