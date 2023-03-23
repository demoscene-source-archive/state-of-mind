/***********************************************
 *              Move                           *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _MOVE_H_
#define _MOVE_H_

/******************************************************************/

typedef struct {
#pragma pack(1)

   QUATERNION Q;   
   VECTOR P;
   FLT t;

} MOVE_KEY;

typedef struct {
#pragma pack(1)

   DATA_ARRAY Keys;
   INT Cache;
   INT Last_Pt;

} MOVE_SPLINE;

extern void Add_Point( VECTOR P, QUATERNION Q, FLT t, MOVE_SPLINE *Spl );
extern void Insert_Point( VECTOR P, QUATERNION Q, FLT t, MOVE_SPLINE *Spl );
extern MOVE_SPLINE *New_Move_Spline( INT Nb );
extern void Set_Up_Move( MOVE_SPLINE *Spl );
extern void Move_Matrix( FLT t, MOVE_SPLINE *Spl, MATRIX2 M );
extern void Move_Matrix2( FLT t, MOVE_SPLINE *Spl, MATRIX2 M );

/******************************************************************/

#endif /* _MOVE_H_ */
