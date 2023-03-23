/***********************************************
 *              Lens flare 16bits + sat        *
 * unfinished...                               *
 * Skal 98.                                    *
 ***********************************************/

#include "demo.h"
#include "main3d.h"
#include "sprite.h"

/******************************************************************/

#ifdef UNIX

EXTERN void Paste_Flare_Asm_16_Sat( )
{
   INT dH;
   USHORT *Where;
   BYTE **Whats;
   INT *Offsets;

   Sp_dHo--; Sp_dWo--;
   Whats = ((BYTE**)Sp_Whats0) + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
   Where = ((USHORT *)Sp_Dst) + Sp_dWo;

   for( dH=-Sp_dHo; dH<=0; ++dH )
   {
      BYTE *What;
      INT dW;

      What = Whats[dH];
      for( dW=-Sp_dWo; dW<=0; ++dW )
      {
         INT S777;
         S777 = Offsets[dW];
         S777 = (UINT)What[ S777 ];
         if ( S777 )
         {
            UINT S777;
            S777 = ((UINT*)Sp_CMap)[ S777 ];
            S777 += RGB_16_TO_777( Where[dW] );
            Where[dW] = RGB_777_TO_16(S777);
         }
      }
      (BYTE*)Where += Sp_BpS;
   }
}
#endif   // UNIX

EXTERN void Paste_Flare_Bitmap_16_Sat( FLT xo, FLT yo, FLT R, 
   FLARE *Flare, FLARE_SPRITE *Sprite )
{
   Sp_CMap = Flare->Convert;
   if ( Setup_Paste_Flare( sizeof( USHORT ), xo, yo, R,
      Flare, Sprite )!=NULL ) Paste_Flare_Asm_16_Sat( );
}

/******************************************************************/
