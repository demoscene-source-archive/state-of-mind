/*
 * World #2
 *
 * Skal 98
 ***************************************/

#include "demo.h"

#define CACHE3
#define MIP_MIN 9

/********************************************************************/

#define WORLD_NAME "villef3.3ds"
#define VISTA_NAME "villef3.vst"
#define CACHE_NAME_II "villef16b.lzw"
#define CACHE_NAME_III "viller2.lzw"
#define CACHE_NAME_IV "v42.lzw"
#define CACHE_NAME_IVJ "v42.jpg"

EXTERN WORLD *W4 = NULL;
EXTERN CAMERA *Cam4 = NULL;
EXTERN OBJ_NODE *Cam4_Node = NULL;
EXTERN VISTA *Vista4 = NULL;

/********************************************************************/
/********************************************************************/

#if 0

typedef struct  
{
   FLT xo, yo, x, y;
   FLT Scale, Rot;
} LINES;

#define NB_LINES 100
static INT Nb_Lines;
static LINES Lines[NB_LINES];


static RENDERING_FUNCTION Shader_Store_Lines( )
{
   INT i;
   if ( Nb_Lines==NB_LINES ) return( RENDER_NONE );
   for( i=0; i<Nb_Out_Edges; ++i )
   {
      FLT xi, yi, xf, yf;
      Lines[Nb_Lines].xo = Edges_x[i] / The_W; 
      Lines[Nb_Lines].yo = Edges_y[i] / The_H;
      Lines[Nb_Lines].x = Edges_x[i+1] / The_W;
      Lines[Nb_Lines].y = Edges_y[i+1] / The_H;
   }
   Nb_Lines++;
   return( RENDER_NONE );
}

static void Set_Lines_Shader_Func( OBJ_NODE *Obj )
{
   MESH *Msh;
   Msh = (MESH *)Obj->Data;
   Msh->Shader = &Shader1;
//   Shader1.Set_Parameters( (void *)Msh, (MATERIAL *)W4->Materials );
}

/********************************************************************/

static void Do_Lines( LINES *Lines, INT Nb, FLT eps, FLT Zoom )
{
   INT i;
   Setup_Clipping( 5.0, The_W-5.0, 5.0, The_H-5.0, 1.0, 3000.0 );
   for( i=0; i<Nb; ++i )
   {
      FLT xi, yi, xf, yf;
      FLT C, S, Cx, Cy;
      FLT Scale, Rot;
      FLT xxi, yyi, xxf, yyf;
      Scale = 1.0 + Lines[i].Scale*eps;
      Rot = Lines[i].Rot*eps;
      C = cos( Rot ); S = sin( Rot );
      xi = ( Lines[i].xo - .5 ) * Zoom + .5;
      yi = ( Lines[i].yo - .5 ) * Zoom + .5;
      xf = ( Lines[i].x - .5 ) * Zoom + .5;
      yf = ( Lines[i].y - .5 ) * Zoom + .5;
      Cx = (xi+xf)/2.0; Cy=(yi+yf)/2.0;
      xi = (xi-Cx)*Scale;
      yi = (yi-Cy)*Scale;
      xf = (xf-Cx)*Scale;
      yf = (yf-Cy)*Scale;
      xxi = xi*C + yi*S + Cx;
      yyi =-xi*S + yi*C + Cy;
      xxf = xf*C + yf*S + Cx;
      yyf =-xf*S + yf*C + Cy;
      ((void (*)(FLT,FLT,FLT,FLT))Primitives[RENDER_LINE])( 
         xxi*The_W, yyi*The_H,
         xxf*The_W, yyf*The_H );
   }
}

static void Randomize_Lines( LINES *Lines, INT Nb )
{
   INT i;
   for( i=0; i<Nb; ++i )
   {
      Lines[i].Scale = 4.0*(Random()&0xFF)/256.0;
      Lines[i].Rot = 0.5*M_PI*(Random()&0xFF)/256.0;
   }
}

/********************************************************************/

EXTERN void Loop_Lines( )
{
   USHORT CMap[256];
   Line_16_CMap = CMap;
   Drv_Build_Ramp_16( (UINT*)CMap, 0, 64, 0,0,0, 130, 190, 230, 0x2565 );
   Do_Lines( Lines, Nb_Lines, 2.0*(1.0-Tick_x), 1.0 + (1.0-Tick_x)*4.0 );
}

EXTERN void Loop_Lines_No_Rot( )
{
   USHORT CMap[256];
   Line_16_CMap = CMap;
   Drv_Build_Ramp_16( (UINT*)CMap, 0, 64, 0,0,0, 130, 190, 230, 0x2565 );
   Do_Lines( Lines, Nb_Lines, 2.0*Tick_x, 4.0 );
}

EXTERN void Loop_Lines_Inv_Rot( )
{
   USHORT CMap[256];
   Line_16_CMap = CMap;
   Drv_Build_Ramp_16( (UINT*)CMap, 0, 64, 0,0,0, 130, 190, 230, 0x2565 );
   Do_Lines( Lines, Nb_Lines, 3.9*Tick_x, 1.0 + (Tick_x)*7.0 );
}

#endif   // 0

/********************************************************************/
/********************************************************************/

static void Set_UV_Shader_Func_1( OBJ_NODE *Obj )
{
   MESH *Msh;
   INT i;
   Msh = (MESH *)Obj->Data;
   Msh->Shader = &UVc_Shader;
   UVc_Shader.Set_Parameters( (void *)Msh, (MATERIAL *)W4->Materials );
#ifdef CACHE3
   for( i=0; i<Msh->Nb_Polys; ++i ) // this will clear Poly->Ptr field
   {
      Msh->Polys[i].Ptr = NULL;
      Msh->Polys[i].Colors = 0x0000;
   }
#endif
}

/********************************************************************/

EXTERN void Pre_Cmp_World2( )
{
#ifdef CACHE3
   Select_Cache_Methods( &Cache_Methods_III );
#else
   Select_Cache_Methods( &Cache_Methods_II );
#endif

   W4 = Load_World( WORLD_NAME );

   Vista4 = Load_Vista( VISTA_NAME, W4 );
#ifdef CACHE3
   Destroy_Objects( (OBJECT *)W4->Materials, _MATERIAL_.Destroy );
   W4->Materials = NULL;
#else
   W4->Cache = Load_Txt_Cache_II( CACHE_NAME_II );
#endif

//   Shader1 = Raw_Line_Shader;
//   Shader1.Deal_With_Poly = Shader_Store_Lines;
//   Shader1.Deal_With_Poly2 = NULL;
//   Traverse_Typed_Nodes( W4->Root, MESH_TYPE, Set_Lines_Shader_Func );

   Traverse_Typed_Nodes( W4->Root, MESH_TYPE, Set_UV_Shader_Func_1 );
   Select_World( W4 );     // compute, optimize, allocate, map polys, etc...
#ifdef CACHE3
   // W4->Cache = Load_Txt_Cache_III( CACHE_NAME_III, MIP_MIN );
   W4->Cache = Load_Txt_Cache_IV( CACHE_NAME_IV, CACHE_NAME_IVJ, MIP_MIN );
#endif
   Cam4_Node = Search_Node_From_Type( W4->Root, CAMERA_TYPE );
   Cam4 = (CAMERA *)( Cam4_Node->Data );
   Cam4->Lens = 1.9f;

#if 0
   Select_Vista_World( W4, Cam4_Node );      // <= *can* fail 
   Renderer.Render = Render_Vista;
   Renderer.Sort_Objects = Sort_Vista_Polys;

   Install_Renderer( &Renderer_16b, RENDER_SIZE, The_W, The_H, RENDER_END_ARG );
   Select_Primitives( &Primitives_16 );
   WORLD_SET_FRAME( W4, 0.0 );


   Nb_Lines = 0;
   Setup_Clipping( 1.0, The_W-1.0, 1.0, The_H-1.0, 1.0, 3000.0 );
   Setup_Rendering( NULL, NULL, NULL, NULL, NULL, The_W, The_H, The_W*2 );
   Setup_Cam_Screen( 0.5*The_W, 0.5*The_H, 0.5*The_W, 0.5*The_H );
   Render_World( W4, Cam4_Node );
//   fprintf( stderr, "Nb_Lines=%d\n", Nb_Lines );

   Randomize_Lines( Lines, Nb_Lines );
   Traverse_Typed_Nodes( W4->Root, MESH_TYPE, Set_UV_Shader_Func_1 );
#endif
}

/********************************************************************/
/********************************************************************/

EXTERN void Init_World2( )
{
   Install_Renderer( &Renderer_16b, RENDER_SIZE, The_W, The_H, RENDER_END_ARG );
#ifdef CACHE3
   Select_Cache_Methods( &Cache_Methods_III );
#else
   Select_Cache_Methods( &Cache_Methods_II );
#endif

   Select_Primitives( &Primitives_16 );

   Select_Vista_World( W4, Cam4_Node );      // <= *can* fail 

      // plug new rendering functions

   Renderer.Render = Render_Vista;
   Renderer.Sort_Objects = Sort_Vista_Polys;
}

///////////////////////////////////////////////////////////////////////

EXTERN void Loop_W2_01( ) // World2 + Deform_Blur. Out vbuffer is VB2
{
   FLT x, Amp;
   INT i,j;

   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 1.0, 3000.0 );
   WORLD_SET_FRAME( W4, Global_x*Time_Scale + Time_Off );
   Render_World( W4, Cam4_Node );

   x = 1.0f - Tick_x;
   Amp = 128.0f*x*x;

   x = Tick_x; x = x*x;
   i = (INT)(2+6*x); j = (INT)(2+4*x);
   Deform_Blur( Amp, i, j );
}

EXTERN void Loop_W2_03( )
{
   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 1.0, 3000.0 );
   WORLD_SET_FRAME( W4, Global_x*Time_Scale + Time_Off );
   Render_World( W4, Cam4_Node );
   Scratch_And_Flash( Get_Beat( ) );
   Get_Next_Scratch( 0x0E );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
}

////////////////////////////////////////////////////////////////////////

EXTERN void Close_World2( )
{
   if ( W4!=NULL ) Destroy_World( W4 );
   W4 = NULL;
}

/********************************************************************/
/********************************************************************/

