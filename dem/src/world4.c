/*
 * World #4 (+shadows?)
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

#define MIP_MIN   9

#define WORLD_NAME1 "shad5.3ds"
#define CACHE_NAME1 "shad5.lzw"
#define OBJ_NAME1 "Object01"
#define LGHT_11 6
#define LGHT_12 7

#define WORLD_NAME2 "box4.3ds"
#define CACHE_NAME2 "box4.lzw"
#define OBJ_NAME2 "Object03"
#define LGHT_21 4
#define LGHT_22 3

static WORLD *W6, *W7, *Da_World;
static CAMERA *Cam1, *Cam2;
static OBJ_NODE *Cam1_Node, *Cam2_Node, *Da_Cam;
static OBJ_NODE *Obj11_Node, *Obj12_Node;
static OBJ_NODE *Obj21_Node, *Obj22_Node;
static OBJ_NODE *Obj1_Node, *Obj2_Node;
static OBJ_NODE *Light61, *Light71;
static OBJ_NODE *Light62, *Light72;

#define LGHT   Light6

static USHORT *ZB_Base;

/********************************************************************/
/********************************************************************/

EXTERN void Pre_Cmp_World4( )
{
   Select_Cache_Methods( &Cache_Methods_III );

   W6 = Load_World( WORLD_NAME1 );
   Destroy_Objects( (OBJECT *)W6->Materials, _MATERIAL_.Destroy );
   W6->Materials = NULL;
   Shader1 = UVc_Shader;
   Traverse_Typed_Nodes( W6->Root, MESH_TYPE, Set_UV_Shader_Func );
   Obj11_Node = Search_Node_From_Name( W6->Root, OBJ_NAME1 );
   Obj12_Node = Search_Node_From_Name( W6->Root, "Object02" );
   Cam1_Node = Search_Node_From_Name( W6->Root, "Camera01" );
   Cam1 = (CAMERA *)( Cam1_Node->Data );
   Cam1->Lens = 1.0;

   Light61 = Search_Node_From_ID( W6->Root, LGHT_11 );
   if ( Light61!=NULL )
   {
      ((LIGHT*)Light61->Data)->Flare_Scale = 0.37*550;      // adjust!!
      ((LIGHT*)Light61->Data)->Flare = &Light_Flare;
   }
   Light71 = Search_Node_From_ID( W6->Root, LGHT_12 );
   if ( Light71!=NULL )
   {
      ((LIGHT*)Light71->Data)->Flare_Scale = 0.45*550;
      ((LIGHT*)Light71->Data)->Flare = &Light_Flare;
   }

   Select_World( W6 );     // compute, optimize, allocate, map polys, etc...
   W6->Cache = Load_Txt_Cache_III( CACHE_NAME1, MIP_MIN );

   W7 = Load_World( WORLD_NAME2 );
   Destroy_Objects( (OBJECT *)W7->Materials, _MATERIAL_.Destroy );
   W7->Materials = NULL;
   Shader1 = UVc_Shader;
   Traverse_Typed_Nodes( W7->Root, MESH_TYPE, Set_UV_Shader_Func );
   Obj21_Node = Search_Node_From_Name( W7->Root, OBJ_NAME2 );
   Obj22_Node = Search_Node_From_Name( W7->Root, "Object02" );
   Cam2_Node = Search_Node_From_Name( W7->Root, "Camera01" );
   Cam2 = (CAMERA *)( Cam2_Node->Data );
   Cam2->Lens = 2.1f;

   Light62 = Search_Node_From_ID( W7->Root, LGHT_21 );
   if ( Light62!=NULL )
   {
      ((LIGHT*)Light62->Data)->Flare_Scale = 0.37*550;      // adjust!!
      ((LIGHT*)Light62->Data)->Flare = &Light_Flare;
   }
   Light72 = Search_Node_From_ID( W7->Root, LGHT_22 );
   if ( Light72!=NULL )
   {
      ((LIGHT*)Light72->Data)->Flare = &Light_Flare;
	   ((LIGHT*)Light72->Data)->Flare_Scale = 0.45*550;
   }

   Select_World( W7 );     // compute, optimize, allocate, map polys, etc...
   W7->Cache = Load_Txt_Cache_III( CACHE_NAME2, MIP_MIN );
//   fprintf( stderr,"0x%x 0x%x 0x%x 0x%x \n", Obj11_Node, Obj12_Node, Obj21_Node, Obj22_Node );
}

/********************************************************************/
/********************************************************************/

EXTERN void Init_World4( INT Param )
{
   if ( Param==0 )
   {
      Install_Renderer( &Renderer_16b, RENDER_SIZE, 
         The_W, The_H, RENDER_END_ARG );
      Select_Cache_Methods( &Cache_Methods_III );
      Select_Primitives( &Primitives_16 );
      Setup_Rendering( VB(VSCREEN).Bits, NULL, NULL, NULL, NULL, 
            The_W, The_H, VB(VSCREEN).BpS );
      Setup_Cam_Screen( 0.5*The_W, 0.5*The_H, 0.5*The_W, 0.5*The_H );
      Cur_Vista = NULL;
      ZB_Base = New_Fatal_Object( The_W*The_H, USHORT );
   }

   if ( Param==1 )
   {
      Da_World = W7;
      Da_Cam = Cam2_Node;
      Obj1_Node = Obj21_Node;
      Obj2_Node = Obj22_Node;
   }
   else
   {
      Da_World = W6;
      Da_Cam = Cam1_Node;
      Obj1_Node = Obj11_Node;
      Obj2_Node = Obj12_Node;
   }
   Select_World( Da_World );     // compute, optimize, allocate, map polys, etc...
}

///////////////////////////////////////////////////////////////////////

#ifdef DEBUG_OK

static void Render_zBuf_Light_4( )
{
   INT i, j;
   USHORT *Dst;
   SHORT *Src2, *Src3, *Src4;
   BYTE *Src;

      // Clear counters
   memset( ZBuffer_Front, 0, The_W*The_H*sizeof(USHORT) );
   memset( ZBuffer_Shade, 0, The_W*The_H*sizeof(BYTE) );
 
   // Debug
   memset( ZBuffer_Back, 0, The_W*The_H*sizeof(USHORT) );

   Render_zBuf_Shade( );            // Render every objects

#if 1
   Dst = VB(VSCREEN).Bits;
   Dst += The_W;
   Src2 = ZBuffer_Back;
   Src2 += The_W;
   Src3 = ZBuffer_Front;
   Src3 += The_W;
   Src4 = ZBuffer;
   Src4 += The_W;
   Src = ZBuffer_Shade;
   Src += The_W;
   for( j=VB(VSCREEN).H; j>0; j-- )
   {
      for( i=-The_W; i<0; ++i )
      {
         // if (Src3[i]) if (Src2[i]==0) Dst[i] = 0x001f;  //&= 0x48c3;
         if (Src3[i]-Src2[i]>0 ) Dst[i] |= 0x18c3; //(Src3[i]-Src2[i]);
         // if ( Src2[i] ) Dst[i] = Src2[i];
         // if (Src2[i]>0) Dst[i] |= 0x3800;  //&= 0x48c3;
      }
      (BYTE*)Dst += VB(VSCREEN).BpS;
      (BYTE*)Src += The_W;  
      (SHORT*)Src2 += The_W;
      (SHORT*)Src3 += The_W;
      (SHORT*)Src4 += The_W;
   }
#endif
}

void Loop_W4_Shad( )
{
#if 0
   USHORT ZB[The_W*The_H];
   USHORT ZBFront[The_W*The_H];
   USHORT ZBBack[The_W*The_H];
   PIXEL  ZBShade[The_W*The_H];

   ZBuffer = ZB;
   ZBuffer_Front = ZBFront;
   ZBuffer_Back = ZBBack;
   ZBuffer_Shade = ZBShade;
   Shader1 = UVc_zBuf_Shader;
   Renderer.Render_Light_Flare = NULL;
   Renderer.Render_Light = NULL;
   ((LIGHT*)LGHT->Data)->Shader = NULL;

   WORLD_SET_FRAME( W6, Global_x*Time_Scale + Time_Off );

   VBuffer_Clear( &VB(VSCREEN) );

   Setup_Clipping( 10.0, The_W-10.0, 10.0, The_H-10.0, 1.0, 2000.0 );

   _RCst_.Global_Z_Scale = 65536.0f*32767.0f*16.0;
   Obj2_Node->Flags &= ~OBJ_DONT_RENDER;
   Obj1_Node->Flags |= OBJ_DONT_RENDER;
   Render_World( W6, Da_Cam );

//   Renderer.Render_Light_Flare = Render_Light_Flare;
//   Primitives[RENDER_PASTE_FLARE] = (void(*))Paste_Flare_Bitmap_16_Sat;

   ((LIGHT*)LGHT->Data)->Shader = &Light_zBuf_Shader5;
   ((LIGHT*)LGHT->Data)->Z_Max = 800.0;
   Renderer.Render_Light = Render_zBuf_Light_4;

   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 1.0, 2000.0 );
   Obj2_Node->Flags |= OBJ_DONT_RENDER;
   Obj1_Node->Flags &= ~(OBJ_DONT_RENDER|OBJ_NO_SHADOW);
   ((MESH*)Obj1_Node->Data)->Flags &= ~(OBJ_DONT_RENDER|OBJ_NO_SHADOW);
   Render_World( W6, Da_Cam );
   Obj2_Node->Flags &= ~OBJ_DONT_RENDER;

      // reset state.
   _RCst_.Global_Z_Scale = INV_Z_SCALE;
   Renderer.Render_Light_Flare = NULL;
   ZBuffer = NULL;
   ZBuffer_Front = NULL;
   ZBuffer_Back = NULL;
   ZBuffer_Shade = NULL;
   Renderer.Render_Light = NULL;
   Shader1 = UVc_Shader;
   ((LIGHT*)LGHT->Data)->Shader = NULL;
#endif
}

#endif   // DEBUG_OK

///////////////////////////////////////////////////////////////////////

void Loop_W4_02( )
{
   ZBuffer = (SHORT*)ZB_Base;
   Shader1 = UVc_zBuf_Shader;
   Renderer.Render_Light_Flare = NULL;   

   WORLD_SET_FRAME( Da_World, Global_x*Time_Scale + Time_Off );

   Setup_Clipping( 6.0, The_W-6.0, 10.0, The_H-10.0, 1.0, 2000.0 );
   Obj2_Node->Flags &= ~OBJ_DONT_RENDER;
   Obj1_Node->Flags |= OBJ_DONT_RENDER;
   Render_World( Da_World, Da_Cam );

   Saturate_DMask_Pal( &VB(VMASK), Get_Beat(), Cur_Mask, 0xd8d8e0 );
   Mixer.Mix_16_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(VSCREEN) );

   Renderer.Render_Light_Flare = Render_Light_Flare;
   Primitives[RENDER_PASTE_FLARE] = (void(*)())Paste_Flare_Bitmap_16_Sat;

   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 1.0, 2000.0 );
   Obj2_Node->Flags |= OBJ_DONT_RENDER;
   Obj1_Node->Flags &= ~OBJ_DONT_RENDER;
   Render_World( Da_World, Da_Cam );
   Obj2_Node->Flags &= ~OBJ_DONT_RENDER;

   Renderer.Render_Light_Flare = NULL;
   ZBuffer = NULL;
   Shader1 = UVc_Shader;
}


////////////////////////////////////////////////////////////////////////

void Close_World4( )
{
   if ( W6!=NULL ) Destroy_World( W6 );
   if ( W7!=NULL ) Destroy_World( W7 );
   W6 = NULL; W7 = NULL;
   Clear_DMask( );
   M_Free( ZB_Base );
}

/********************************************************************/
/********************************************************************/
