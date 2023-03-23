/***********************************************
 *              Frustums                       *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#ifndef _FRUSTUM_
#define _FRUSTUM_

typedef struct { 
   PLANE Back, Front;
   PLANE Left, Right;
   PLANE Up, Down;
} FRUSTUM;

extern void Camera_Frustum( FRUSTUM *Frustum, MATRIX2 Mo, FLT Apert_X, FLT Apert_Y );
extern void Transform_Frustum( FRUSTUM *Out, FRUSTUM *In, MATRIX2 Mo );
extern void Camera_Frustum_II( FRUSTUM *Frustum, MATRIX2 Mo, FLT Apert_X, FLT Apert_Y );
extern void Camera_Frustum_II_Direct( FRUSTUM *Frustum, FLT Apert_X, FLT Apert_Y );

extern INT Poly_Clip_Plane( POLY_3D *Out, PLANE plane, POLY_3D *In );
extern INT Frustum_Clip( POLY_3D *Out, POLY_3D *In, FRUSTUM *F );
extern INT Frustum_Clip_Direct( POLY_3D *Out, POLY_3D *In, OBJ_NODE *Node );

/******************************************************************/

#endif   // _FRUSTUM_
