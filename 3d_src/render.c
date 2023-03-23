/***********************************************
 *        renderer setup and entry point       *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

   // What the renderer needs...

EXTERN void *Main_Buffer = NULL;  
EXTERN void *Shade_Buffer = NULL;  
EXTERN void *Convert_Table = NULL;
EXTERN void *Shade_Table = NULL;
EXTERN INT ZBuffer_Size = 0;
EXTERN INT Main_Buffer_Size = 0;
EXTERN SHORT *ZBuffer = NULL;  
EXTERN SHORT *ZBuffer_Front = NULL;  
EXTERN SHORT *ZBuffer_Back = NULL;
EXTERN PIXEL *ZBuffer_Shade = NULL;

/******************************************************************/

EXTERN RENDERER          Renderer = { 0 };     // Current renderer
EXTERN RENDER_PRIMITIVES Primitives = { 0 };
EXTERN CACHE_METHODS     Cache_Methods = { NULL };
EXTERN RENDER_STATE _RCst_;

/******************************************************************/

EXTERN void Select_Primitives( RENDER_PRIMITIVES *Prim )
{
   if ( Prim!=NULL )
      memcpy( &Primitives, Prim, sizeof( RENDER_PRIMITIVES ) );
}
EXTERN void Select_Cache_Methods( CACHE_METHODS *M )
{
   if ( M!=NULL )
      memcpy( &Cache_Methods, M, sizeof( CACHE_METHODS ) );
}

EXTERN void Select_Renderer( RENDERER *What, INT Check_Buf )
{
   if ( What==NULL ) return;  
   memcpy( &Renderer, What, sizeof( RENDERER ) );

   Select_Primitives( What->Draw );
   Select_Cache_Methods( What->Cache_Methods );
   _RCst_.Pix_Width = What->Size_X;
   _RCst_.Pix_Height = What->Size_Y;
   // _RCst_.Pix_BpS = What->Size_X * What->Quantum;
   _RCst_.Pix_BpS = What->BpS;
   _RCst_.Base_Ptr = (PIXEL *)Main_Buffer;
   _RCst_.Clips[0] = (FLT)( 0 );
   _RCst_.Clips[1] = (FLT)( _RCst_.Pix_Width-0 );
   _RCst_.Clips[2] = (FLT)( 0 );
   _RCst_.Clips[3] = (FLT)( _RCst_.Pix_Height-0 );

   _RCst_.Global_Z_Scale = INV_Z_SCALE;   // reset Z-Scale.

      // disabled, for now. User should manually allocate and
      // setup *Main_Buffer,...

//   if ( Render.Setup!=NULL )
//      if ( Renderer.Setup( &Renderer, Check_Buf )==NULL )  // <= this call can fail...
//         Exit_Upon_Error( "Renderer.Setup() failed" );
}

EXTERN RENDERER *Install_Renderer( RENDERER *What, ... )
{
   va_list Arg_List;
   RENDERER_OPTION Option;
   INT Size_X, Size_Y, Use_ZBuf, Check_Buf;
   INT Format;

   Size_X = 320; Size_Y = 200;     // default
   Use_ZBuf = FALSE;
   Check_Buf = FALSE;
   Format = -1;

   va_start( Arg_List, What );
   while( 1 )
   {
      Option = va_arg( Arg_List, RENDERER_OPTION );
      if ( Option == RENDER_END_ARG ) break;
      else switch( Option )
      {
         case RENDER_USE_ZBUFFER: Use_ZBuf = TRUE; break;
         case RENDER_SIZE:
            Size_X = va_arg( Arg_List, INT );
            Size_Y = va_arg( Arg_List, INT );
         break;
         case RENDER_OPEN_MODE:
            Format = va_arg( Arg_List, INT );
         break;
         case RENDER_CHECK_BUFFERS: Check_Buf = TRUE; break;
      }
   }
   va_end( Arg_List );

   What->Size_X = Size_X;
   What->Size_Y = Size_Y;
   What->BpS = Size_X;
   Select_Renderer( What, Check_Buf );

   if ( Use_ZBuf )
   {
      if ( (ZBuffer==NULL) || (Size_X*Size_Y<ZBuffer_Size) )
      {
         M_Free( ZBuffer );
         ZBuffer_Front = ZBuffer_Back = NULL;
         ZBuffer_Shade = NULL;
      }
      if ( ZBuffer==NULL )
      {
         ZBuffer = (SHORT *)New_Fatal_Object( Size_X*Size_Y*( 3+1 ), USHORT );
         ZBuffer_Size = Size_X*Size_Y;
         ZBuffer_Front = ZBuffer + ZBuffer_Size;
         ZBuffer_Back = ZBuffer + 2*ZBuffer_Size;
         ZBuffer_Shade = (PIXEL*)( ZBuffer + 3*ZBuffer_Size );
      }
   }

   return( What );
}

EXTERN void Un_Init_Renderer( )
{
   if ( ZBuffer_Size )
   {
      M_Free( ZBuffer );
      M_Free( Shade_Buffer );
      ZBuffer_Size = 0;
   }
   if ( Main_Buffer_Size )
   {
      M_Free( Main_Buffer );
      Main_Buffer_Size = 0;
   }
}

/******************************************************************
 *                                                                *
 *               ENTRY POINT FOR DEFAULT RENDERING                *
 *                                                                *
 ******************************************************************/

#ifdef USE_OLD_SHADOWZ
EXTERN void Setup_Light_I( OBJ_NODE *L )
{
   Cur_Light = (LIGHT *)L->Data;
   NODE_TRANSFORM( L );
//   STORE_TRANSFORM_MATRIX( Cur_Light );

   if ( Cur_Light->Buf != NULL )
      Render_Light( Cur_Light, _RCst_.The_World->Root );
}
#endif

/******************************************************************/

#ifndef _SKIP_LIGHT_

EXTERN void Setup_Light_II( OBJ_NODE *L )
{
   Cur_Light = (LIGHT *)L->Data;
   Check_Node_Transform( L );
   Compose_Light_And_Camera( L, _RCst_.The_Camera );
   Select_Light_Visibility( L );
}

EXTERN void Process_Shadows( OBJ_NODE *L )
{
   Cur_Light = (LIGHT *)L->Data;
   Cur_Shader = Cur_Light->Shader;   // =Light_zBuf_Shader most probably...
   if ( Cur_Shader!=NULL )
      Renderer.Render_Light( );
}

#endif   // _SKIP_LIGHT_

EXTERN void Init_Render( WORLD *W, OBJ_NODE *Camera )
{
      // Enter_Engine

   _RCst_.The_World = W;
   _RCst_.The_Camera = Camera;
   _RCst_.Cur_Camera = (CAMERA *)Camera->Data;
   Cur_Vista = _RCst_.Cur_Camera->Vista;
   //_R_Cst_.Cur_Vista = Cur_Vista;    // TODO: REMOVE/FIX!!

   if ( Cur_Vista!=NULL ) goto Skip;

         // Zscreen = K*Zmin*(1/Z - 1/ZMax)
         //         = (K*Zmin) / Z  + ( -K*Zmin/ZMax )
         //         = Z_Scale * Inv_Z + Z_Off...

   _RCst_.Z_Min = _RCst_.Clips[4];
   _RCst_.Z_Max = _RCst_.Clips[6];  // just in case...
   _RCst_.Z_Scale = _RCst_.Global_Z_Scale * _RCst_.Z_Min;
   _RCst_.Z_Vista_Scale = 65536.0f / ( _RCst_.Z_Max-_RCst_.Z_Min + .0001f );
   _RCst_.Z_Off = -_RCst_.Z_Scale/_RCst_.Z_Max;
   _RCst_.All_Z_Min = _HUGE_;
   _RCst_.All_Z_Max =-_HUGE_;

Skip:
   Total_Poly_Sorted = 0;     // Debugging only
}

/******************************************************************/

EXTERN void Setup_Rendering( void *Main_Buf, void *ZBuf, 
   void *ZBuf_F, void *ZBuf_B, void *Shade_Buf,
   INT W, INT H, INT BpS )
{
   Main_Buffer = (SHORT*)Main_Buf;
   _RCst_.Base_Ptr = (PIXEL*)Main_Buffer;
   ZBuffer = (SHORT*)ZBuf;
   ZBuffer_Front = (SHORT*)ZBuf_F;
   ZBuffer_Back = (SHORT*)ZBuf_B;
   Shade_Buffer = Shade_Buf;
   _RCst_.Pix_Width = W;
   _RCst_.Pix_Height = H;
   _RCst_.Pix_BpS = BpS;
}

EXTERN void Setup_Clipping( FLT xmin, FLT xmax, FLT ymin, FLT ymax, 
   FLT zmin, FLT zmax )
{
   _RCst_.Clips[0] = xmin;
   _RCst_.Clips[1] = xmax;
   _RCst_.Clips[2] = ymin;
   _RCst_.Clips[3] = ymax;
   _RCst_.Clips[4] = zmin;
   _RCst_.Clips[5] = 1.0f/zmin;
   if ( zmax<zmin ) zmax = _HUGE_;
   _RCst_.Clips[6] = zmax;
   _RCst_.Clips[7] = 1.0f/zmax;
}

EXTERN void Setup_Cam_Screen( FLT Lx, FLT Ly, FLT Cx, FLT Cy )
{
   _RCst_._Lx_ = Lx; 
   _RCst_._Ly_ = Ly;
   _RCst_._Cx_ = Cx; 
   _RCst_._Cy_ = Cy;
   _RCst_.Out_Clips[0] = -4.0f*Lx;
   _RCst_.Out_Clips[1] = 4.0f*Lx;
   _RCst_.Out_Clips[2] = -4.0f*Ly;
   _RCst_.Out_Clips[3] = 4.0f*Ly;
   _RCst_.Out_Clips[4] = -10.0f*Lx;
   _RCst_.Out_Clips[5] = _HUGE_;
}

/******************************************************************/

EXTERN void Render_World( WORLD *W, OBJ_NODE *Camera )
{
      // Enter_Engine

   if ( Renderer.Render_Init!=NULL )
      Renderer.Render_Init( W, Camera );

	// THIS IS A HACK! SOME DDRAW DRIVER DON'T RESTORE
	// FPU CONTROL WORD. WE NEED TO RE-INIT. :(
#if defined(WIN32)	
	Enter_Engine_Asm();   
#endif

   _RCst_.Frame_Stamp++;

#ifdef USE_OLD_SHADOWZ
         // Process all lights. Compute shadow map if needed
   Cur_Light = NULL;
   if ( W->Lights!=NULL )
      Traverse_Typed_Nodes( W->Root, LIGHT_TYPE, Setup_Light_I );
#endif

         // Setup camera matrix

   if ( Camera->Flags & OBJ_DO_ANIM )
   {
      if ( Camera->Cur_Time!=W->Time )
         NODE_ANIM( Camera, W->Time );
   }
   NODE_TRANSFORM( Camera );
   Camera->Time_Stamp = _RCst_.Frame_Stamp;

         // Process all lights. Transform in camera's space
         // *Cur_Light will be the last in data array
         // (no multi-light for gouraud...)

#ifndef _SKIP_LIGHT_
   Cur_Light = NULL;
   if ( W->Lights!=NULL )
      Traverse_Typed_Nodes( W->Root, LIGHT_TYPE, Setup_Light_II );
#endif   // _SKIP_LIGHT_

               //////// Go! ////////

   Renderer.Render( W->Root );

      // Process all lights. Spread volumic shadows

#ifndef _SKIP_LIGHT_
   if ( (Renderer.Render_Light!=NULL) && (W->Lights!=NULL) )
      Traverse_Typed_Nodes( W->Root, LIGHT_TYPE, Process_Shadows );
#endif   // _SKIP_LIGHT_

      // Post-process

   if ( Renderer.Post_Process!=NULL )
      Renderer.Post_Process( );

#ifndef _SKIP_LIGHT_      

      // Add light/lens flares

   if ( (Renderer.Render_Light_Flare!=NULL) && (W->Lights!=NULL) )
      Renderer.Render_Light_Flare( W );

#endif   // _SKIP_LIGHT_
}

/******************************************************************/

