/***********************************************
 *              Lens flare                     *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#include "main3d.h"
#include "sprite.h"

/******************************************************************/

#ifdef UNIX

EXTERN void Paste_Flare_Asm_16( )
{
   INT dH;
   USHORT *Where;
   USHORT **Whats;
   INT *Offsets;


   Sp_dHo--; Sp_dWo--;
   Whats = ((USHORT**)Sp_Whats0) + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
   Where = (USHORT *)Sp_Dst;
   Where += Sp_dWo;

   for( dH=-Sp_dHo; dH<=0; ++dH )
   {
      USHORT *What;
      INT dW;

      What = Whats[dH];
      for( dW=-Sp_dWo; dW<=0; ++dW )
      { 
         USHORT c;
         c = What[ Offsets[dW] ];
         /*if ( c )*/ Where[dW] = c;
      }
      Where = (USHORT*)((BYTE*)Where+Sp_BpS);
   }
}
#endif   // UNIX

EXTERN void Paste_Flare_Bitmap_16( FLT xo, FLT yo, FLT R, 
   FLARE *Flare, FLARE_SPRITE *Sprite )
{
   Sp_CMap = Flare->Convert;
   if ( Setup_Paste_Flare( sizeof( USHORT ), xo, yo, R,
      Flare, Sprite )!=NULL ) Paste_Flare_Asm_16( );
}

/******************************************************************/
