/***********************************************
 *              render.h                       *
 * Skal 98                                     *
 ***********************************************/

#ifndef _RENDER_H_
#define _RENDER_H_

/******************************************************************/

   // Needs being defined by user

extern void *Main_Buffer;
extern void *Shade_Buffer;
extern void *Convert_Table;
extern void *Shade_Table;
extern INT ZBuffer_Size;
extern INT Main_Buffer_Size;
extern SHORT *ZBuffer;
extern SHORT *ZBuffer_Front;
extern SHORT *ZBuffer_Back;
extern PIXEL *ZBuffer_Shade;
// extern COLOR_ENTRY CMap[256];
extern USHORT *Line_16_CMap;

/******************************************************************/

typedef enum 
{
   RENDER_NONE,

   RENDER_FLAT_I,
   RENDER_FLAT_II,
   RENDER_GOURAUD,
   RENDER_GOURAUD_RAMP,
   RENDER_Z_GOURAUD,
   RENDER_UV,
   RENDER_UV_OFFSET,
   RENDER_UV_16b,
   RENDER_UV_BUMP,
   RENDER_UV_BUMP_II,
   RENDER_UV2,

   RENDER_ZBUF,
   RENDER_FLAT_ZBUF,
   RENDER_ZBUF_POS_SHADOW,
   RENDER_ZBUF_NEG_SHADOW,

   RENDER_POINT,
   RENDER_LINE,
   RENDER_CIRCLE,

   RENDER_PASTE_FLARE,
   RENDER_PASTE_SPRITE,

   RENDER_USER1, RENDER_USER2,

   RENDER_UVC,
   RENDER_UVC_2,
   RENDER_UVC_4,
   RENDER_UVC_8,
   RENDER_UVC_16,
   RENDER_UVC_32,
   RENDER_UVC_64,

   LAST_RENDERING_FUNCTION
} RENDERING_FUNCTION;

typedef enum {

   RENDER_OPEN_MODE,
   RENDER_SIZE,
   RENDER_USE_ZBUFFER,
   RENDER_CHECK_BUFFERS,
   RENDER_END_ARG

} RENDERER_OPTION;

typedef void (*RENDER_PRIMITIVES[LAST_RENDERING_FUNCTION] )( );
typedef struct RENDERER RENDERER;

struct RENDERER
{
#pragma pack(1)

   INT Quantum;
   void  (*Init_All)( RENDERER *R, INT Check_Buf );
   void *(*Setup)( RENDERER *R, INT Check_Buf );
   void  (*Close)( RENDERER *R );
   void  (*Install_CMap)( COLOR_ENTRY *, INT, PIXEL * );
   void  (*Flush_Raw)( PIXEL *, PIXEL *, INT , INT );

   void (*Render_Init)( WORLD *W, OBJ_NODE *Camera ); // Init_Render()
   void (*Sort_Objects)( OBJ_NODE *Cur );             // Select_Boxes()
   void (*Sort_Lights)( LIGHT *Light, OBJ_NODE *Cur );// Select_Light_Boxes()
   void (*Render)( OBJ_NODE *Cur );                   // Render_Camera()
   void (*Render_Light)( );                           // Render_zBuf_Light()
                                                      // Render_zBuf_Light2()
                                                      // or Render_zBuf_Light3()
   void (*Render_Light_Flare)( WORLD *W );            // =Render_Light_Flare()
   void (*Post_Process)( );                           // Post-process

   void (*Sort_Polys)( UINT *A, INT Nb );             // Counting_Sort()   
   void (*Emit_Poly)( );                              // Emit_Poly()

   void (*Rasterize)( );                              // Rasterize_Edges()

   RENDER_PRIMITIVES *Draw;
   CACHE_METHODS     *Cache_Methods;

   INT Size_X, Size_Y, BpS;
};


extern RENDERER Renderer;
extern RENDER_PRIMITIVES Primitives;
extern CACHE_METHODS Cache_Methods;

         // HACK!!! Fix that!!

extern void Setup_Rendering( void *Main_Buf, void *ZBuf, 
   void *ZBuf_F, void *ZBuf_B, void *Shade_Buf,
   INT W, INT H, INT BpS );
extern void Setup_Clipping( FLT xmin, FLT xmax, FLT ymin, FLT ymax, FLT zmin, FLT zmax );
extern void Setup_Cam_Screen( FLT Lx, FLT Ly, FLT Cx, FLT Cy );


extern void Select_Renderer( RENDERER *What, INT Check_Buf );
extern void Select_Primitives( RENDER_PRIMITIVES *Prim );
extern void Select_Cache_Methods( CACHE_METHODS *M );
extern RENDERER *Install_Renderer( RENDERER *, ... );
extern void Un_Init_Renderer( );

/******************************************************************/

extern RENDERER Renderer_16b;                // 16bit renderer
extern RENDER_PRIMITIVES Primitives_16;

extern RENDERER Renderer_88b;                // 8+8bits renderer

extern RENDERER Renderer_8b;                 // 8bit renderer
extern RENDER_PRIMITIVES Primitives_8;

/******************************************************************/

   // TODO: this has nothing to do here!!!
extern void RotoZoom_3D( FLT Rot, FLT Scale, FLT Cx, FLT Cy, void *Ptr );
extern void Spherical_3D( F_MAPPING Uo, F_MAPPING U1, F_MAPPING U2, void *Ptr );

/******************************************************************/
/******************************************************************/

#endif   // _RENDER_H_
