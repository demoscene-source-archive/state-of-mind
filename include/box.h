/***********************************************
 *              Boxes                          *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _BOXES_H_
#define _BOXES_H_

/******************************************************************/

#define _HUGE_    1.0e10f
#define BOX_EPSILON  0.01f

struct BBOX {
#pragma pack(1)

   VECTOR Mins,  Maxs;   // original
   VECTOR TMins, TMaxs;  // transformed
   FLT    Radius, TRadius;
   FLT    iZ, TiZ;

};

/******************************************************************/

typedef enum {
#pragma pack(1)

 SIDE_1, SIDE_2, SPLIT

} BOX_TEST_RESULT;

extern BOX_TEST_RESULT Test_Box_Plane( PLANE, BBOX * );
extern void Set_Up_Box( VECTOR *Vertex, INT N, BBOX *B );
// extern void Set_BBox_From_Min_Max( BBOX *B, VECTOR Min, VECTOR Max );
extern INT Insert_In_Upper_Box( BBOX *Up, BBOX *B );

extern INT Is_Box_Viewed_By_Cam( CAMERA *Cam, BBOX *B );
extern INT Is_Sphere_Viewed_By_Cam( CAMERA *Cam, BBOX *Box );
extern INT Is_Sphere_Viewed_By_Light( LIGHT *L, BBOX *Box );

extern void BBox_Planes_To_Points( VECTOR Pt[8], BBOX *Box );
extern void BBox_TPlanes_To_Points( VECTOR Pt[8], BBOX *Box );
extern FLT Project_Box( BBOX *Box, MATRIX2 M, FLT Clip[6] );
extern void BBox_Transform_Min_Max( BBOX *Box, MATRIX2 M );

extern void Draw_Box( BBOX *Box, MATRIX2 M );

/******************************************************************/

#endif   /* _BOXES_H_ */
