/*
 * World #3
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

#define WORLD_NAME "st.3ds"
#define CACHE_NAME "state242.lzw"
#define CACHE_NAME_IV "st.lzw"
#define CACHE_NAME_IVJ "st.jpg"

#define USE_CACHE4
#define MIP_MIN 9

static WORLD *W5;
static CAMERA *Cam5;
static OBJ_NODE *Cam5_Node;

/********************************************************************/
/********************************************************************/

static void Set_UV_Shader_Func_2( OBJ_NODE *Obj )
{
   MESH *Msh;
   Msh = (MESH *)Obj->Data;
   Msh->Shader = &UVc_Shader;
   UVc_Shader.Set_Parameters( (void *)Msh, (MATERIAL *)W5->Materials );
}

/********************************************************************/

EXTERN void Pre_Cmp_World3( )
{
#ifdef USE_CACHE4
   Select_Cache_Methods( &Cache_Methods_III );
#else
   Select_Cache_Methods( &Cache_Methods_II );
#endif

   W5 = Load_World( WORLD_NAME );

#ifdef USE_CACHE4
   Destroy_Objects( (OBJECT *)W5->Materials, _MATERIAL_.Destroy );
   W5->Materials = NULL;
   Traverse_Typed_Nodes( W5->Root, MESH_TYPE, Set_UV_Shader_Func );
   Select_World( W5 );     // compute, optimize, allocate, map polys, etc...
   W5->Cache = Load_Txt_Cache_IV( CACHE_NAME_IV, CACHE_NAME_IVJ, MIP_MIN );
   Shader1 = UVc_Shader;
#else
   W5->Cache = Load_Txt_Cache_II( CACHE_NAME, MIP_MIN );
   Traverse_Typed_Nodes( W5->Root, MESH_TYPE, Set_UV_Shader_Func_2 );
   Select_World( W5 );     // compute, optimize, allocate, map polys, etc...
#endif

   Cam5_Node = Search_Node_From_Type( W5->Root, CAMERA_TYPE );
   Cam5 = (CAMERA *)( Cam5_Node->Data );
   Cam5->Lens = 1.3f;
}

/********************************************************************/
/********************************************************************/

EXTERN void Init_World3( INT Param )
{
   Select_World( W5 );     // compute, optimize, allocate, map polys, etc...
   Install_Renderer( &Renderer_16b, RENDER_SIZE, 
      The_W, The_H, RENDER_END_ARG );

#ifdef USE_CACHE4
   Select_Cache_Methods( &Cache_Methods_III );
#else
   Select_Cache_Methods( &Cache_Methods_II );
#endif

   Select_Primitives( &Primitives_16 );
   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 10.0, 0.0 );
   Setup_Rendering( VB(VSCREEN).Bits, NULL, NULL, NULL, NULL, 
         The_W, The_H, VB(VSCREEN).BpS );
   Setup_Cam_Screen( 0.5*The_W, 0.5*The_H, 0.5*The_W, 0.5*The_H );
   Time_Scale = 1.08f; Time_Off = 0.0;
   Cur_Mask = Head_For;
   Setup_Mask_II( VMASK, Cur_Mask );

   if ( Param==1 )
   {
      SELECT_CMAP( 3 );
      Check_CMap_VBuffer( &VB(3), 256 );
      CMap_To_16bits_With_Fx( (void*)VB(3).CMap, Sky->Pal, Sky->Nb_Col, 0x2565,
         0x000000, NULL, (BYTE)(100), 0 );                     
   }
}

///////////////////////////////////////////////////////////////////////

void Loop_W3_03( ) // World2 + Deform_Blur. Out vbuffer is VB2
{
   WORLD_SET_FRAME( W5, Global_x*Time_Scale + Time_Off );
   NODE_ANIM( Cam5_Node, W5->Time );

   Cam5 = (CAMERA *)Cam5_Node->Data;
   Camera_Matrix( Cam5->Pos, Cam5->Target, Cam5->Bank, Cam5_Node->Mo );
   memcpy( Ptcl_M, Cam5_Node->Mo, sizeof( MATRIX2 ) );
   Set_Bck( (PIXEL*)VB(3).Bits, Sky->Bits );

   Mixer.Mix_8_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(3) );

   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 20.0, 0.0 );
   Render_World( W5, Cam5_Node );

   Scratch_And_Flash( Get_Beat( ) );
   Get_Next_Scratch( 0x0E );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
}

////////////////////////////////////////////////////////////////////////

void Close_World3( )
{
   if ( W5!=NULL ) Destroy_World( W5 );
   W5 = NULL;
   Clear_DMask( );   // Head_for...
}

/********************************************************************/
/********************************************************************/
