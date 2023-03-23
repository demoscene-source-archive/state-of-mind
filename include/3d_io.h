/***********************************************
 *         3D Objects I/O                      *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _3D_IO_H_
#define _3D_IO_H_

/******************************************************************/
/******************************************************************/

#define SKY_IS_BLUE 0
#define IO_ERROR  (-1)
#define MEM_ERROR (-2)
#define UGLY_BUG  (-3)

#define READ_ONE(X)   if ( F_READ( &(X), F, sizeof( X ) )!=1 ) return(IO_ERROR)
#define READ_M(X)   if ( F_READ( (X), F, sizeof( X ) )!=1 ) return(IO_ERROR)

/******************************************************************/

typedef enum {

   CHK_M3D_VERSION = 0x0002,
   CHK_MESH_VERSION = 0x3D3E,

   CHK_RGB_F     = 0x0010,
   CHK_RGB_B     = 0x0011,
/* CHK_RBGB2     = 0x0012, */

   CHK_ONE_UNIT  = 0x0100,
   
   CHK_MAIN      = 0x4D4D,
   CHK_OBJMESH   = 0x3D3D,
   CHK_MESHVRSN  = 0x3D3E,
   CHK_BKGCOLOR  = 0x1200,
   CHK_AMBCOLOR  = 0x2100,

   CHK_OBJBLOCK  = 0x4000,
   CHK_OBJHIDDEN = 0x4010,

   CHK_OBJNOCAST = 0x4012,
   CHK_OBJ_MATTE = 0x4013,
   CHK_OBJNORCV  = 0x4017,

   CHK_TRIMESH   = 0x4100,
   CHK_VERTLIST  = 0x4110,
   CHK_TVERTLIST = 0x4111,
   CHK_FACELIST  = 0x4120,
   CHK_FACEMAT   = 0x4130,
   CHK_MAPLIST   = 0x4140,
   CHK_SMOOLIST  = 0x4150,
   CHK_TRMATRIX  = 0x4160,
   CHK_LIGHT     = 0x4600,
   CHK_SPOTLIGHT = 0x4610,
   CHK_LGHTZMIN  = 0x4659,
   CHK_LGHTZMAX  = 0x465A,
   CHK_LGHTMULT  = 0x465B,
   CHK_LIGHT_OFF = 0x4620,
   CHK_LGHT_ATTN = 0x4625,

   CHK_WIN_SET   = 0x7001,

   CHK_CAMERA    = 0x4700,

   CHK_MATERIAL  = 0xAFFF,
   CHK_MATNAME   = 0xA000,
   CHK_AMBIENT   = 0xA010,
   CHK_DIFFUSE   = 0xA020,
   CHK_SPECULAR  = 0xA030,
   CHK_TEXTURE   = 0xA200,
   CHK_BUMPMAP   = 0xA230,
   CHK_MAPFILE   = 0xA300,

   CHK_VSCALE    = 0xA354,
   CHK_USCALE    = 0xA356,
   CHK_UOFFSET   = 0xA358,
   CHK_VOFFSET   = 0xA35A,
   CHK_UV_ROT    = 0xA35C,

   CHK_KEYFRAMER    = 0xB000,
   CHK_TRACKAMB     = 0xB001,
   CHK_TRACKMESH    = 0xB002,
   CHK_TRACKCAMERA  = 0xB003,
   CHK_TRACKCAMTGT  = 0xB004,
   CHK_TRACKLIGHT   = 0xB005,
   CHK_TRACKLIGTGT  = 0xB006,
   CHK_TRACKSPOTL   = 0xB007,

   CHK_FRAMES        = 0xB008,
   CHK_KFCURTIME     = 0xB009,
   CHK_KFHDR         = 0xB00A,
   CHK_NODE_HDR      = 0xB010,
   CHK_INSTANCE_NAME = 0xB011,
   CHK_PRESCALE      = 0xB012,
   CHK_PIVOT         = 0xB013,
   CHK_BOUNDBOX      = 0xB014,
   CHK_MORPH_SMOOTH  = 0xB015,
   CHK_TRACKPOS      = 0xB020,
   CHK_TRACKROTATE   = 0xB021,
   CHK_TRACKSCALE    = 0xB022,
   CHK_TRACKFOV      = 0xB023,
   CHK_TRACKROLL     = 0xB024,
   CHK_TRACKCOLOR    = 0xB025,
   CHK_TRACKMORPH    = 0xB026,
   CHK_TRACKHOTSPOT  = 0xB027,
   CHK_TRACKFALLOFF  = 0xB028,
   CHK_TRACKHIDE     = 0xB029,
   CHK_NODE_ID       = 0xB030,

      // proprietary chunks
   CHK_EDGES       = 0xF000,
   CHK_POLY_UV     = 0xF001,
   CHK_POLY_EDGES  = 0xF002,


   CHK_UNKNOWN       = 0xFFFF,
} CHUNK_TYPE;

/******************************************************************/

extern WORLD *Load_World( STRING );

/******************************************************************/

#endif

