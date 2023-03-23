/***********************************************
 *           8bits renderer                   *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "sprite.h"

/******************************************************************/

static void *Setup_8( RENDERER *R, INT Check_Buf )
{
   if ( R->Size_X*R->Size_Y*R->Quantum>Main_Buffer_Size )
   {
      M_Free( Main_Buffer );
      Main_Buffer = (void *)New_Fatal_Object( R->Size_X*R->Size_Y*R->Quantum, BYTE );
      Main_Buffer_Size = R->Size_X*R->Size_Y*sizeof( BYTE );
   }
   return( Main_Buffer );
}

/******************************************************************/

EXTERN RENDER_PRIMITIVES Primitives_8 = 
{
   NULL, 
   _Draw_Flat_8, 
   _Draw_Flat_Mix_8,     // <= same a RENDER_FLAT_I ?
   _Draw_Gouraud_8, _Draw_Gouraud_Ramp_8,
#ifdef USE_OLD_SHADOWZ   
   _Draw_zGouraud_8, 
#else
   NULL,
#endif

   _Draw_UV_8,
   _Draw_UV_Offset, _Draw_UV_16b,
   _Draw_UV_Bump, NULL,
   _Draw_UV2_8, 

   _Draw_zBuf, _Draw_Flat_zBuf,
   NULL, NULL,    // Draw_zBuf_Pos_Shadow, Draw_zBuf_Neg_Shadow,

   NULL,
   (void(*)()) _Draw_ALine_8,
   NULL,

   (void(*)()) Paste_Flare_Bitmap_8,
   (void(*)()) Paste_Sprite_Bitmap_8,

   NULL,       // <= Plug Render_zBuf_ID here, for instance
   NULL,

   _Draw_UVc_8,
   _Draw_UVc_2_8,  _Draw_UVc_4_8,  _Draw_UVc_8_8,
   _Draw_UVc_16_8, _Draw_UVc_32_8, _Draw_UVc_64_8

};

/******************************************************************/

EXTERN RENDERER Renderer_8b = 
{
   sizeof( PIXEL ),
   NULL,             // Init_All()
   NULL, // Setup_8,             // Setup()
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
   NULL, // Render_zBuf_Light,   // NULL?
#ifndef _SKIP_LIGHT_
   NULL, // Render_Light_Flare() <-> Render_Light_Flare_3dfx()
#else
   NULL,
#endif
   NULL,    // Post-process

   Counting_Sort,
   Emit_Poly,
   Rasterize_Edges,
   &Primitives_8,
   &Cache_Methods_I,

   0,0,0
};

/******************************************************************/
