/*
 * Test de NPRR
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

#define USE_CACHE4
#define MIP_MIN 9

#define WORLD_NAME   "nsal.3ds"
#define VISTA_NAME   "nsal.vst"
#define CACHE_IV     "nsal.lzw"
#define CACHE_IVJ    "nsal.jpg"

#define FLARE_FILE   "flare.gif"

EXTERN WORLD *W3 = NULL;
EXTERN CAMERA *Cam3 = NULL;
EXTERN OBJ_NODE *Cam3_Node = NULL;
EXTERN VISTA *Vista3 = NULL;
EXTERN OBJ_NODE *Obj1, *Obj2, *Obj3;
EXTERN OBJ_NODE *Pil1, *Pil2, *Pil3, *Pil4;
EXTERN OBJ_NODE *Fc1, *Fc2, *Fc3;
EXTERN OBJ_NODE *Light2, *Light6;
EXTERN OBJ_NODE *Light1;

EXTERN SHADER_METHODS Shader1;

static USHORT *ZB_Base = NULL;

#define OBJ_OFF(o) (o)->Flags |= OBJ_DONT_RENDER
#define OBJ_ON(o) (o)->Flags &= ~OBJ_DONT_RENDER

/********************************************************************/
/********************************************************************/

#ifdef USE_FLARE

#define FLARE_NB 7
EXTERN FLARE_SPRITE Flare_Sprite[] = {         // flare.gif
  { 192,0,   64,64,   0.9, -0.57 },  // #3
  { 0,0,     64,64,   3.2,  0.00 },  // #0
  { 192,0,   64,64,   1.9,  0.43 },  // #3
  { 0,  64,  64,64,   0.9,  0.76 },  // #4
  { 64, 64,  64,64,   0.6,  0.90 },  // #5
  { 128,0,   64,64,   1.3,  1.75 },  // #2
  { 64,0,    64,64,   2.1,  2.20 }   // #1
};
EXTERN BITMAP *Flare;
EXTERN FLARE Light_Flare;

#endif   //  USE_FLARE

/********************************************************************/
/********************************************************************/

EXTERN void Set_UV_Shader_Func( OBJ_NODE *Obj )
{
   MESH *Msh;
   INT i;
   Msh = (MESH *)Obj->Data;
   Msh->Shader = &Shader1;
   Shader1.Set_Parameters( (void *)Msh, NULL );
   for( i=0; i<Msh->Nb_Polys; ++i ) // this will clear Poly->Ptr field
   {
      Msh->Polys[i].Ptr = NULL;
      Msh->Polys[i].Colors = 0x0000;
   }
}

/********************************************************************/

EXTERN void Pre_Cmp_6( )
{
   Shader1 = UVc_Shader;
   Select_Cache_Methods( &Cache_Methods_III );

   W3 = Load_World( WORLD_NAME );
   Destroy_Objects( (OBJECT *)W3->Materials, _MATERIAL_.Destroy );
   W3->Materials = NULL;
   Traverse_Typed_Nodes( W3->Root, MESH_TYPE, Set_UV_Shader_Func );

   Obj1 = Search_Node_From_Name( W3->Root, "mur01.2" );
   Obj2 = Search_Node_From_Name( W3->Root, "Torus2" );
   Obj3 = Search_Node_From_Name( W3->Root, "tun01" );
   Pil1 = Search_Node_From_Name( W3->Root, "p01" );
   Pil2 = Search_Node_From_Name( W3->Root, "p02" );
   Pil3 = Search_Node_From_Name( W3->Root, "p03" );
   Pil4 = Search_Node_From_Name( W3->Root, "p04" );
   Fc1 = Search_Node_From_Name( W3->Root, "f01" );
   Fc2 = Search_Node_From_Name( W3->Root, "f02" );
   Fc3 = Search_Node_From_Name( W3->Root, "f03" );

   Vista3 = Load_Vista( VISTA_NAME, W3 );
    
   Select_World( W3 );     // compute, optimize, allocate, map polys, etc...

#ifdef USE_CACHE3
   W3->Cache = Load_Txt_Cache_III( CACHE_NAME, MIP_MIN );
#else
    W3->Cache = Load_Txt_Cache_IV( CACHE_IV, CACHE_IVJ, MIP_MIN );
#endif

   Cam3_Node = Search_Node_From_Type( W3->Root, CAMERA_TYPE );
   Cam3 = (CAMERA *)( Cam3_Node->Data );
   Cam3->Lens = 1.3f;

#ifdef USE_FLARE
   Flare = Load_GIF( FLARE_FILE );
   Light_Flare.Sprites = Flare_Sprite;
   Light_Flare.Nb_Sprites = FLARE_NB;
   Light_Flare.Bits = Flare->Bits;
   Light_Flare.Width = Flare->Width;
   Light_Flare.BpS = Flare->Width*sizeof(BYTE);
   Light_Flare.Height = Flare->Height;
   Light_Flare.Convert = (PIXEL*)New_Fatal_Object( Flare->Nb_Col, UINT );
   CMap_To_16bits( (void*)Light_Flare.Convert, Flare->Pal, Flare->Nb_Col, 0x777 );
   M_Free( Flare->Pal );
   Flare->Nb_Col = 0;

   Light1 = Search_Node_From_ID( W3->Root, 5 );
   if ( Light1!=NULL )
   {
      ((LIGHT*)Light1->Data)->Flare = &Light_Flare;
      ((LIGHT*)Light1->Data)->Flare_Scale = 0.37*1000;      // adjust!!
   }
   Light2 = Search_Node_From_ID( W3->Root, 2 );
   if ( Light2!=NULL )
   {
      ((LIGHT*)Light2->Data)->Flare = &Light_Flare;
      ((LIGHT*)Light2->Data)->Flare_Scale = 0.45*1000;
   }

#endif   // USE_FLARE
 
}

/********************************************************************/
/********************************************************************/

EXTERN void Init_6( )
{
   Install_Renderer( &Renderer_16b, RENDER_SIZE, The_W, The_H, RENDER_END_ARG );
   Select_Cache_Methods( &Cache_Methods_III );
   Select_Primitives( &Primitives_16 );

      // plug new rendering functions
   Cam3->Vista = Vista3;
   Select_Vista_World( W3, Cam3_Node );      // <= *can* fail 
   Renderer.Render = Render_Vista;
   Renderer.Sort_Objects = Sort_Vista_Polys;
   Shader1 = UVc_Shader;

//   Renderer.Render_Light_Flare = Render_Light_Flare;
   if ( ZB_Base==NULL ) ZB_Base = New_Fatal_Object( The_W*The_H, USHORT );
}

///////////////////////////////////////////////////////////////////////

EXTERN void Loop_6_02( )    // tunnel + mask
{
   Setup_Clipping( 25.0, 300.0, 20.0, The_H-20.0, 0.2f, 0.0 );
   WORLD_SET_FRAME( W3, Global_x*Time_Scale + Time_Off );
   Render_World( W3, Cam3_Node );
   Saturate_DMask_Pal( &VB(VMASK), Get_Beat( ), Cur_Mask, 0xC4C0C2 );
   Mixer.Mix_16_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(VSCREEN) );
}

EXTERN void Loop_6_05( )    // mixed clipped tunnel + Mask
{
   FLT x;

   ZBuffer = (SHORT*)ZB_Base;
   Shader1 = UVc_zBuf_Shader;
   Renderer.Render_Light_Flare = NULL;   

   WORLD_SET_FRAME( W3, Global_x*Time_Scale + Time_Off );
   OBJ_OFF(Obj2);
   OBJ_ON(Fc1); OBJ_ON(Fc2); OBJ_ON(Fc3);
   OBJ_ON(Pil1); OBJ_ON(Pil2);
   OBJ_ON(Pil3); OBJ_ON(Pil4);
   OBJ_ON(Obj1);
   OBJ_ON(Obj3);
   Setup_Clipping( 25.0, 300.0, 20.0, The_H-20.0, 0.2f, 0.0 );
   Render_World( W3, Cam3_Node );

   x = Get_Beat( );
   Saturate_DMask_Pal( &VB(VMASK), x, Cur_Mask, 0xC4C0C2 );
   Mixer.Mix_16_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(VSCREEN) );

   Renderer.Render_Light_Flare = Render_Light_Flare;
   Primitives[RENDER_PASTE_FLARE] = (void(*)())Paste_Flare_Bitmap_16_Sat;

   OBJ_ON(Obj2);
   OBJ_OFF(Fc1); OBJ_OFF(Fc2); OBJ_OFF(Fc3);
   OBJ_OFF(Pil1); OBJ_OFF(Pil2);
   OBJ_OFF(Pil3); OBJ_OFF(Pil4);
   OBJ_OFF(Obj1);
   OBJ_OFF(Obj3);

   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 0.2f, 0.0 );
   Render_World( W3, Cam3_Node );

   OBJ_OFF(Obj2);

   Renderer.Render_Light_Flare = NULL;
   ZBuffer = NULL;
   Shader1 = UVc_Shader;
   Loop_Dsp_III( );
}

EXTERN void Loop_6_06( )    // tunnel + bump. Out vbuffer is VSCREEN
{
   ZBuffer = (SHORT*)ZB_Base;
   Renderer.Render_Light_Flare = Render_Light_Flare;
   Primitives[RENDER_PASTE_FLARE] = (void(*)())Paste_Flare_Bitmap_16_Sat;
   Shader1 = UVc_zBuf_Shader;
   Setup_Clipping( 0.0, The_W, 0.0, The_H, 0.2f, 0.0 );

   WORLD_SET_FRAME( W3, Global_x*Time_Scale + Time_Off );
   Render_World( W3, Cam3_Node );

   Renderer.Render_Light_Flare = NULL;
   Shader1 = UVc_Shader;
   ZBuffer = NULL;

   Scratch_And_Flash( Get_Beat( ) );
   Get_Next_Scratch( 0x0E );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
   Loop_Dsp_I( );
}

EXTERN void Loop_6_08( )    // tunnel + Flashs II. Out vbuffer is VSCREEN
{
   ZBuffer = (SHORT*)ZB_Base;
   Renderer.Render_Light_Flare = Render_Light_Flare;
   Primitives[RENDER_PASTE_FLARE] = (void(*)())Paste_Flare_Bitmap_16_Sat;
   Shader1 = UVc_zBuf_Shader;
   Setup_Clipping( 0.0, The_W, 0.0, The_H, 0.2f, 0.0 );

   WORLD_SET_FRAME( W3, Global_x*Time_Scale + Time_Off );
   Render_World( W3, Cam3_Node );

   Saturate_DMask_Pal( &VB(VMASK), Get_Beat( ), Cur_Mask, 0xC4C0C2 );
   Mixer.Mix_16_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(VSCREEN) );

   Renderer.Render_Light_Flare = NULL;
   Shader1 = UVc_Shader;
   ZBuffer = NULL;

   Scratch_And_Flash( Get_Beat( ) );
   Get_Next_Scratch( 0x0E );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );
}

///////////////////////////////////////////////////////////////////////

#if 0
EXTERN void Init_6_zBuf( )
{
   Install_Renderer( &Renderer_16b, RENDER_SIZE, The_W, The_H, RENDER_END_ARG );
   Select_Cache_Methods( &Cache_Methods_III );
   Select_Primitives( &Primitives_16 );

      // plug new rendering functions
   Select_Vista_World( W3, Cam3_Node );      // <= *can* fail 
   Renderer.Render = Render_Vista;
   Renderer.Sort_Objects = Sort_Vista_Polys;
   Renderer.Render_Light_Flare = NULL;
}

EXTERN void Loop_6_zBuf( )
{
   ZBuffer = ZB_Base;
   Renderer.Render_Light_Flare = Render_Light_Flare;
   Primitives[RENDER_PASTE_FLARE] = (void(*))Paste_Flare_Bitmap_16_Sat;
   Shader1 = UVc_zBuf_Shader;
   WORLD_SET_FRAME( W3, Global_x*Time_Scale + Time_Off );
   Render_World( W3, Cam3_Node );
   Renderer.Render_Light_Flare = NULL;
   Shader1 = UVc_Shader;
   ZBuffer = NULL;
}
#endif

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

EXTERN void Close_6( )
{
   if ( W3!=NULL ) Destroy_World( W3 );
   W3 = NULL;
   M_Free( ZB_Base );
   Clear_DMask( );
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
