/***********************************************
 *              Main 3D header                 *
 *                                             *
 * Skal 97                                     *
 ***********************************************/

#ifndef _MAIN_3D_H_
#define _MAIN_3D_H_

#include "main.h"
#include "btm.h"
#include "truecol.h"
#include "ld_any.h"
#include "drv_io.h"

// #define _SKIP_VISTA_
// #define _SKIP_HIDE_
// #define _SKIP_MORPH_
// #define _SKIP_SPOT_
// #define _SKIP_LIGHT_
// #define _SKIP_COLORS_
// #define _SKIP_MISC_
// #define _SKIP_SHADOWS_

// #define _SKIP_EDGES_
// #define _SKIP_OPTIM_MESH_
// #define _SKIP_MAP_SPHERICAL_

// #define _SKIP_OPTIM_POLY_     // (warning)
// #define _SKIP_OPTIM2_MESH_

/******************************************************************/

typedef struct POLY POLY;
typedef struct VERTICE VERTICE;
typedef struct MSH_EDGE MSH_EDGE;

/******************************************************************/

typedef struct MATERIAL MATERIAL;
typedef struct WORLD WORLD;
typedef struct CAMERA CAMERA;
typedef struct LIGHT LIGHT;
typedef struct OBJECT OBJECT;
typedef struct OBJ_NODE OBJ_NODE;
typedef struct NODE_METHODS NODE_METHODS;
typedef struct OBJ_METHODS OBJ_METHODS;
typedef struct BBOX BBOX;
typedef struct BOUND BOUND;
typedef struct GROUP GROUP;
typedef struct OBJ_DUMMY OBJ_DUMMY;
typedef struct MESH MESH;
typedef struct PARTICLE PARTICLE;
typedef struct RAY RAY;
typedef struct POLY_RAYTRACE_CST POLY_RAYTRACE_CST;
typedef struct SLAB SLAB;
typedef struct SLABS SLABS;
typedef struct RAY_MATERIAL RAY_MATERIAL;

typedef struct TXT_CACHE TXT_CACHE;
typedef struct TEXTURE_MAP TEXTURE_MAP;
typedef struct CACHE_METHODS CACHE_METHODS;

typedef struct BLOB BLOB;
typedef struct MC_BLOB MC_BLOB;

/******************************************************************/

#define MAX_SCANS  480
#define MAX_WIDTH  640
#define MAX_EDGES (MAX_POLY_PTS+2)
#define MAX_ADDITIONAL_VERTICES  10
#define MAX_POLY_PTS 4

/******************************************************************/

#include "obj.h"
#include "render.h"
#include "poly.h"
#include "edges.h"
#include "shader.h"
#include "light.h"
#include "zlight.h"
#include "vshadow.h"

/******************************************************************/
/******************************************************************/

#include "node.h"
#include "group.h"
#include "mat.h"
#include "mesh.h"
#include "parts.h"
#include "vista.h"
#include "frustum.h"
#include "camera.h"
// #include "bsp.h"
#include "world.h"
#include "blob.h"
#include "mcube.h"

#include "render2.h"    // <= main class for global variables

/******************************************************************/

#include "3d_io.h"
#include "sort.h"
#include "mk.h"
#include "convex.h"
#include "raster.h"
#include "clip.h"

#include "tables.h"

/******************************************************************/
/******************************************************************/

#endif // _MAIN_3D_H_

