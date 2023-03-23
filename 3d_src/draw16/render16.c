/***********************************************
 *           16bits renderer                   *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "sprite.h"

/******************************************************************/

static void *Setup_16( RENDERER *R, INT Check_Buf )
{
   INT How_Much;

   How_Much = R->Size_X*R->Size_Y*R->Quantum;
   if ( How_Much>Main_Buffer_Size || Shade_Buffer==NULL )
   {
      M_Free( Main_Buffer );
      M_Free( Shade_Buffer );
      Main_Buffer = (void *)New_Fatal_Object( How_Much, BYTE );
      Shade_Buffer = (void *)New_Fatal_Object( How_Much, BYTE );
      Main_Buffer_Size =  How_Much;
   }
   return( Main_Buffer );
}

/******************************************************************/

EXTERN RENDER_PRIMITIVES Primitives_16 = 
{
   NULL, _Draw_Flat_16, _Draw_Flat2_16,
   _Draw_Gouraud_16, _Draw_Gouraud_Ramp_16,
#ifdef USE_OLD_SHADOWZ   
   _Draw_zGouraud_16, 
#else
   NULL,
#endif

   _Draw_UV_16b,
   _Draw_UV_Offset, _Draw_UV_16b,
   _Draw_UV_Bump, NULL,
   _Draw_UV_16b, 

//   _Draw_UVc_Bilin_16,  // test

   _Draw_zBuf, _Draw_Flat_zBuf,
   NULL, NULL,  // (Draw_zBuf_Pos_Shadow, Draw_zBuf_Neg_Shadow,)

   NULL,
   (void(*)()) _Draw_ALine_16,
   NULL,

   (void(*)()) Paste_Flare_Bitmap_16,
   (void(*)()) Paste_Sprite_Bitmap_16,

   NULL,       // <= Plug Render_zBuf_ID here, for instance
   NULL,

   _Draw_UVc_16,
   _Draw_UVc_4_16,  _Draw_UVc_4_16,  _Draw_UVc_8_16,
   _Draw_UVc_16_16, _Draw_UVc_32_16, _Draw_UVc_64_16
   
};

/******************************************************************/

EXTERN RENDERER Renderer_16b = 
{
   sizeof( USHORT ),
   NULL,             // Init_All()
   NULL, // Setup_16,         // Setup()
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
   &Primitives_16,
   &Cache_Methods_II,

   0, 0, 0
};

/******************************************************************/
