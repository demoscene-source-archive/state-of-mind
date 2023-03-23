/*
 * Blobz & Objz
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

static WORLD *W_Bl = NULL;
static CAMERA *Cam_Bl = NULL;
static MC_BLOB Blob = { 0 };
static OBJ_NODE *Cam_Node;
EXTERN OBJ_NODE *Blob_Node;

EXTERN MATERIAL *Material01 = NULL;
#define All_R     1.6
#define B_MAX_VTX 4000
#define B_MAX_POLYS 5000
// #define B_MAX_EDGES 1000

#define CACHE_NAME "maps1.lzw"
#define CACHE_NAME2 "maps1.jpg"

EXTERN void Pre_Cmp_Blb( )
{
   INT i;

   W_Bl = Create_World( );
   W_Bl->Cache = (void*)Load_Txt_Cache_IIbis( CACHE_NAME, CACHE_NAME2 );  
   Material01 = New_Material( );

   Cam_Node = New_Object_By_Type( NULL, &_CAMERA_, W_Bl );
   Cam_Bl = (CAMERA *)( Cam_Node->Data );
   Cam_Bl->Lens = 1.6f;

   Mem_Clear( &Blob );

   for( i=0; i<5; ++i )
      MC_Blobify2( &Blob, .8f, 1.0f );

   Blob.K = .35f;
   Set_MC_Dimensions( 0.0, 0.0, 0.0, 3.0, 3.0, 3.0 );

      // !!! Adjust!
   if ( MC_Finish_Blob2( &Blob, B_MAX_VTX, B_MAX_POLYS ) == NULL )
      Exit_Upon_Error( "blb failure" );

   M_Free( Blob.Blob->Edges );
   Blob.Blob->Nb_Edges = 0;

   Blob_Node = Nodify( (OBJECT*)Blob.Blob, &_MESH_ );
   Insert_Node( Blob_Node, &W_Bl->Root, &W_Bl->Objects );

   Material01->Txt1 = &((TXT_CACHE*)W_Bl->Cache)->Maps[0];
   Material01->Txt2 = Material01->Txt3 = NULL;

   Select_World( W_Bl );

   ((MESH *)Blob_Node->Data)->Shader = &Env_Shader;
   Env_Shader.Set_Parameters( (void *)Blob_Node->Data, Material01 );
   Blob_Node->Flags |= OBJ_DONT_RENDER;
}

EXTERN void Init_Blb( INT Param )
{
   Install_Renderer( &Renderer_16b, RENDER_SIZE, The_W, The_H, RENDER_END_ARG );
   Select_Cache_Methods( &Cache_Methods_II ); 
   Select_Primitives( &Primitives_16 );
   Setup_Rendering( VB(VSCREEN).Bits, NULL, NULL, NULL, NULL, 
      VB(VSCREEN).W, VB(VSCREEN).H, VB(VSCREEN).BpS );
   Setup_Cam_Screen( 0.5*The_W, 0.5*The_H, 0.5*The_W, 0.5*The_H );
   Select_World( W_Bl );

   Set_Vector( Cam_Bl->Pos, 0.0, 0.0, -2.0 );
   Set_Vector( Cam_Bl->Target, 0.0, 0.0, 0.0 );
}

static void Set_Node_Rot_II( OBJ_NODE *Node, FLT Rot )
{
   Node->Transform.Rot.Rot[0] = 40.0f*M_PI_180;
   Node->Transform.Rot.Rot[1] = Rot*M_PI_180;
   Node->Transform.Rot.Rot[2] = 30.0*M_PI_180;
}

static void Set_Blob_Position( FLT B_Rot, FLT R )
{
   Blob.G[0][0] = R*.66f*(FLT)sin( .243f*B_Rot*M_PI_180 );
   Blob.G[0][1] = R*0.6f*(FLT)cos( 1.9012f*B_Rot*M_PI_180 );
   Blob.G[0][2] = R*0.7f*(FLT)sin( .543f*B_Rot*M_PI_180+.2142f );

   Blob.G[1][0] = R*.9212f*(FLT)sin( 0.7613f*B_Rot*M_PI_180 );
   Blob.G[2][2] = R*0.883f*(FLT)sin( 1.443f*B_Rot*M_PI_180 );
   Blob.G[3][2] = R*0.9524f*(FLT)sin( 1.64f*B_Rot*M_PI_180 );
   Blob.G[3][1] = R*0.92151f*(FLT)cos( 1.64f*B_Rot*M_PI_180 );
   Blob.G[4][0] = R*2*0.4f*(FLT)sin( 1.312f*B_Rot*M_PI_180 );
   Blob.G[4][2] = R*2*0.3151f*(FLT)cos( .735f*B_Rot*M_PI_180+.124f );
}

static void Setup_Blb( INT Flag )
{
   FLT x, y;

   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 0.2f, 0.0 );
   if ( Flag )
   {
      FLT Cy;
      Cy = 0.5f + .08f*(TRandom(1)&0x07)/( 16.0f );
      Setup_Cam_Screen( 0.3*The_W, 0.5*The_H, 0.3*The_W, Cy*The_H );
   }
   else Setup_Cam_Screen( 0.3*The_W, 0.5*The_H, 0.3*The_W, 0.5*The_H );

   x = Get_Beat( );
   x = (FLT)cos( x*2.0*M_PI );
   x = x*x; x = x*x;
   y = Global_x*1500.0f;
   Set_Blob_Position( y, 0.5f + 0.69f*x );   // 0.6?
   Set_Node_Rot_II( Blob_Node, 134.0f );
   Do_MCube3( &Blob );
}

static void Render_Obj( OBJ_NODE *Node )
{
   Node->Flags &= ~(OBJ_DONT_RENDER|OBJ_DO_ANIM);
   W_Bl->Time += 1.0f;  // so everything is updated
   Render_World( W_Bl, Cam_Node );
   Node->Flags |= (OBJ_DONT_RENDER|OBJ_DO_ANIM);
} 

EXTERN void Close_Blb( )
{
   Destroy_World( W_Bl );
}

/********************************************************************/

EXTERN void Loop_Anim_Blb_I( )
{
   static PIXEL Count = 0;

   Loop_14_Anim_Mask_Raw( );

   if ( Count )
   {
      Setup_Blb( TRUE );
      Count--;
   }
   else
   {
      Setup_Blb( FALSE );
      if ( Tick_x<0.90 ) 
         if ( (TRandom(1)&0x3f)==0x00 )
            Count = 10 + (TRandom(0)&0x0f);
   }
   Render_Obj( Blob_Node );

   if ( Count ) Destroy_Screen_III( &VB(VSCREEN) );

   Scroll_2( );
   UScroll = (BYTE)(25.0f*Global_x*256.0);
}

/********************************************************************/
