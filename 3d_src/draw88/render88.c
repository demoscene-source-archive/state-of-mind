/***********************************************
 *           8+8bits renderer                  *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "sprite.h"

/******************************************************************/

static void *Setup_88( RENDERER *R, INT Check_Buf )
{
   if ( R->Size_X*R->Size_Y*R->Quantum>Main_Buffer_Size )
   {
      M_Free( Main_Buffer );
      Main_Buffer = (void *)New_Fatal_Object( R->Size_X*R->Size_Y*R->Quantum, USHORT );
      Main_Buffer_Size = R->Size_X*R->Size_Y*sizeof( USHORT );
   }
   return( Main_Buffer );
}

/******************************************************************/

EXTERN RENDER_PRIMITIVES Primitives_88 = 
{
   NULL, _Draw_Flat_88, _Draw_Flat2_88,
   _Draw_Gouraud_88, _Draw_Gouraud_Ramp_88,
#ifdef USE_OLD_SHADOWZ   
   _Draw_zGouraud_88, 
#else
   NULL,
#endif

   _Draw_UV_88,
   _Draw_UV_Offset, _Draw_UV_16b,
   _Draw_UV_Bump, NULL,
   _Draw_UV2_88, 

   _Draw_zBuf, _Draw_Flat_zBuf,
   NULL, NULL,    // Draw_zBuf_Pos_Shadow, Draw_zBuf_Neg_Shadow,

   NULL,
   (void(*)()) _Draw_ALine_88,
   NULL,

   (void(*)()) Paste_Flare_Bitmap_88,
   (void(*)()) Paste_Sprite_Bitmap_88,

   NULL,       // <= Plug Render_zBuf_ID here, for instance
   NULL,

   NULL, // <= _Draw_UVc_88
   _Draw_UVc_2_88,  _Draw_UVc_4_88,  _Draw_UVc_8_88,
   _Draw_UVc_16_88, _Draw_UVc_32_88, _Draw_UVc_64_88

};

/******************************************************************/

EXTERN RENDERER Renderer_88b = 
{
   sizeof( USHORT ),
   NULL,             // Init_All()
   NULL,    //   Setup_88,         // Setup()
   NULL,             // Close()
   NULL,             // Install_CMap + Convert_Table
   NULL,             // Flush_Raw

   Init_Render,
   Sort_Objects,
#ifdef USE_OLD_SHADOWZ
   Sort_Light_Boxes,
#else
   NULL,
#endif
   Render_Camera,
   NULL,             // Render_zBuf_Light
#ifndef _SKIP_LIGHT_
   NULL, // Render_Light_Flare() <-> Render_Light_Flare_3dfx()
#else
   NULL,
#endif
   NULL,    // Post-process

   Counting_Sort,
   Emit_Poly,
   Rasterize_Edges,
   &Primitives_88,
   &Cache_Methods_I,

   0, 0, 0
};

/******************************************************************/
