/***********************************************
 *              Camera                         *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _CAMERA_H_
#define _CAMERA_H_

/******************************************************************/

   // Cam type

#define CAM_NOT_SET        0
#define CAM_PLANAR         1
#define CAM_SPHERICAL      2

struct CAMERA {
#pragma pack(1)

   OBJECT_FIELDS

   INT       Cam_Type;
   FRUSTUM   Frustum;

   FLT       Lx, Ly, D;  // , D2;
   FLT       Cx, Cy;
   FLT       Apert_X, Apert_Y, Inv_Lx;
   FLT       Lens;
   FLT       Bank;
   FLT       Inv_Z_Min, Inv_Z_Max, dInv_Z;
   VECTOR    Screen_Vo;
   
   VECTOR    Pos, Target;
   OBJ_NODE *Dummy;
   SPLINE_3DS *Pos_k, *FOV_k, *Roll_k, *Target_k;

   VISTA *Vista;

};

/******************************************************************/

extern NODE_METHODS _CAMERA_;

extern void Sort_Objects( OBJ_NODE *Cur );
extern void Select_Camera( CAMERA *Cam );
extern void Render_Camera( OBJ_NODE *Root );
extern void Camera_Matrix( VECTOR Pos, VECTOR Tgt, FLT Omega, MATRIX2 M );
extern void Camera_RYP( FLT Yaw, FLT Pitch, FLT Roll, VECTOR Pos, MATRIX2 M );
extern void Camera_RYP_II( FLT Yaw, FLT Pitch, FLT Roll, VECTOR Pos, MATRIX2 M );

/******************************************************************/

#endif   // _CAMERA_H_
