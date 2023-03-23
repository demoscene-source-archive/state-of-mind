/***********************************************
 *              Lens flare                     *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#include "main3d.h"
#include "sprite.h"

/******************************************************************/

#ifdef UNIX

EXTERN void Paste_Flare_Asm_8( )
{
   INT dH;
   PIXEL **Whats, *Where;
   INT *Offsets; 

   Sp_dHo--; Sp_dWo--;
   Whats = ((PIXEL**)Sp_Whats0) + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
   Where = ((PIXEL*)Sp_Dst) + Sp_dWo;

   for( dH=-Sp_dHo; dH<=0; ++dH )
   {
      PIXEL *What;
      INT dW;

      What = Whats[dH];
      for( dW=-Sp_dWo; dW<=0; ++dW )
      { 
         PIXEL c;
         c = What[ Offsets[dW] ];
         ((BYTE*)Where)[dW] = ((PIXEL*)Sp_CMap)[ ((BYTE*)Where)[dW] | (c<<8) ];
      }
      Where = (PIXEL*)((BYTE*)Where+Sp_BpS);
   }
}
#endif   // UNIX

EXTERN void Paste_Flare_Bitmap_8( FLT xo, FLT yo, FLT R, 
   FLARE *Flare, FLARE_SPRITE *Sprite )
{
   Sp_CMap = Flare->Convert;
   if ( Setup_Paste_Flare( sizeof( BYTE ), xo, yo, R,
      Flare, Sprite )!=NULL ) Paste_Flare_Asm_8( );
}

/******************************************************************/
