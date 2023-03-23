/***********************************************
 *                  obj.h                      *
 * Skal 97                                     *
 ***********************************************/

#ifndef _OBJ_H_
#define _OBJ_H_

/******************************************************************/

typedef enum {

   WORLD_TYPE,
   CAMERA_TYPE,
   LIGHT_TYPE,
   OBJECT_TYPE,
   BBOX_TYPE,
   BOUND_TYPE,
   GROUP_TYPE,
   OBJ_DUMMY_TYPE,
   MESH_TYPE,
   PARTICLE_TYPE,
   MATERIAL_TYPE,

   TRANSF_RXYZ,   // transform type
   TRANSF_RZYX,
   TRANSF_EULER,
   TRANSF_QUATERNION,

   VOID_TYPE

} OBJ_TYPE;

typedef enum {

   OBJ_NO_FLAG      = 0x0000,
   OBJ_OWNS_METHODS = 0x0001,
   OBJ_OWNS_NAME    = 0x0002,
   OBJ_DO_ANIM      = 0x0004,
   OBJ_TRACK_OK     = 0x0008,   
   OBJ_NOT_ACTIVE   = 0x0010,
   OBJ_DONT_RENDER  = 0x0020,
   OBJ_NO_SHADOW    = 0x0040,
   OBJ_FINISHED     = 0x0080,

      // more specific flags

   OBJ_SPLIT_OK     = 0x0100,
   OBJ_HAS_UV       = 0x0200,
   OBJ_HAS_NORMALS  = 0x0400,
   OBJ_NOT_LIT      = 0x0800,
   OBJ_EDGES_OK     = 0x1000,
   OBJ_HAS_POLY_UV  = 0x2000,
   OBJ_OPTIM_OK     = 0x4000,
   OBJ_DONT_OPTIMIZ = 0x8000,

   OBJ_USE_RAYTRACE = 0x00010000,
   OBJ_DONT_RECEIVE_SHDW = 0x00020000

} OBJ_FLAG;

#include "array.h"
#include "transf.h"
#include "quat.h"
#include "move.h"
#include "anim.h"

#include "box.h"

/******************************************************************/

#define OBJECT_FIELDS   \
   OBJ_TYPE Type;       \
   OBJECT *Prev, *Next; \
   OBJECT **Top;        \
   INT Users;           \
                        \
   OBJ_FLAG Flags;      \
   UINT ID;             \
                        \
   STRING Name;         \

struct OBJECT {     // Abstract struct
#pragma pack(1)

   OBJECT_FIELDS

};

/******************************************************************/
/******************************************************************/


#endif   // _OBJ_H_
