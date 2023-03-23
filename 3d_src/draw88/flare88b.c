/***********************************************
 *              Lens flare                     *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#include "main3d.h"
#include "sprite.h"

/******************************************************************/

#ifdef UNIX
EXTERN void Paste_Flare_Asm_88( )
{
   INT dH;
   USHORT *Where;
   PIXEL **Whats;
   INT *Offsets; 

   Where = (USHORT *)Sp_Dst;
   Sp_dHo--; Sp_dWo--;
   Whats = ((PIXEL**)Sp_Whats0) + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
   Where += Sp_dWo;
   for( dH=-Sp_dHo; dH<=0; ++dH )
   {
      PIXEL *What;
      INT dW;

      What = Whats[dH];
      for( dW=-Sp_dWo; dW<=0; ++dW )
      { 
         PIXEL c;
         c = What[ Offsets[dW] ];
         Where[dW] = (((BYTE*)Sp_CMap)[ (Where[dW]>>8) | (c<<8) ]<<8) | (Where[dW] & 0xFF );
      }
      Where += Sp_BpS;
   }
}
#endif   // UNIX

EXTERN void Paste_Flare_Bitmap_88( FLT xo, FLT yo, FLT R, 
   FLARE *Flare, FLARE_SPRITE *Sprite )
{
   Sp_CMap = Flare->Convert;
   if ( Setup_Paste_Flare( sizeof( USHORT ), xo, yo, R,
      Flare, Sprite )!=NULL ) Paste_Flare_Asm_88( );
}

/******************************************************************/
