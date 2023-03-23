/***********************************************
 *            Obj making                       *
 *                                             *
 * Skal 97                                     *
 ***********************************************/

#ifndef _MK_H_
#define _MK_H_

#if 0

extern OBJECT *Fill_Cyl5_Polys( OBJECT *Obj, INT Nx, INT Ny, 
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius );
extern OBJECT *Make_Cyl5( INT Nx, INT Ny,
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius );

extern OBJECT *Fill_Cyl4_Polys( OBJECT *Obj, INT Nx, INT Ny, 
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius,
   CST_SPLINE *Shape, CST_SPLINE *Angle );
extern OBJECT *Make_Cyl4( INT Nx, INT Ny, 
   SHORT Uo, SHORT U, SHORT Vo, SHORT V,
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius,
   CST_SPLINE *Shape, CST_SPLINE *Angle );

extern OBJECT *Fill_Cyl3_Polys( OBJECT *Obj, INT Nx, INT Ny, 
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius );
extern OBJECT *Make_Cyl3( INT Nx, INT Ny, 
   SHORT Uo, SHORT U, SHORT Vo, SHORT V,
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius );

extern OBJECT *Fill_Cyl2_Polys( OBJECT *Obj, INT Nx, INT Ny, INT Nx2, INT Ny2,
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius );
extern OBJECT *Make_Cyl2( INT Nx, INT Ny, INT Nx2, INT Ny2,
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius );

extern OBJECT *Make_Patch( INT Nx, INT Ny, PATCH P );
extern OBJECT *Make_Patch_uv( INT Nx, INT Ny, PATCH P, 
   SHORT Uo, SHORT U, SHORT Vo, SHORT V );

extern MESH *Make_Plane_XY( INT Nx, INT Ny, FLT Rx, FLT Ry,
   SHORT Uo, SHORT U, SHORT Vo, SHORT V );
extern MESH *Make_Plane_XZ( INT Nx, INT Ny, FLT Rx, FLT Ry,
   SHORT Uo, SHORT U, SHORT Vo, SHORT V );
extern MESH *Make_Plane2( INT Nx, INT Ny, FLT Rx, FLT Ry,
   SHORT Uo, SHORT U, SHORT Vo, SHORT V );

#endif   // 0

extern MESH *Make_Cyl( INT Nx, INT Ny, INT Nx2, INT Ny2, FLT R, FLT H );
extern MESH *Make_Cone( INT Nx, INT Ny, FLT R1, FLT R2, FLT H );
extern MESH *Make_Torus( INT Nx, INT Ny, FLT R1, FLT R2 );
extern MESH *Make_Sphere( INT Nx, INT Ny, FLT Rx, FLT Ry, FLT Rz );
extern MESH *Make_Sphere2( INT Nx, INT Ny, FLT Rx, FLT Ry, FLT Rz, INT, INT );

extern MESH *Make_Plane_XY( INT Nx, INT Ny, FLT Rx, FLT Ry,
   FLT Uo, FLT U, FLT Vo, FLT V );


   // in mk2.c

extern MESH *Make_Sphere2( INT Nx, INT Ny, FLT Rx, FLT Ry, FLT Rz, INT, INT );
extern MESH *Make_Torus2( INT Nx, INT Ny, FLT R1, FLT R2 );

#endif   // _MK_H_
