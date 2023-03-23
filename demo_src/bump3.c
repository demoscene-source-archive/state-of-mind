/***********************************************
 *      Bump map utilities                     *
 *                                             *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "btm.h"
#include "bump.h"
#include <math.h>

/*****************************************************************/

#if 1  // #ifdef UNIX
EXTERN void Bumpify_I( PIXEL *Dst, PIXEL *Src, INT W, INT H,
   INT Dst_BpS, INT Src_BpS, INT xo, INT yo, PIXEL *Env_Map )
{
   INT j;
   for( j=0; j<H; ++j )
   {
      INT i, jo;

      jo = yo-j;
      for( i=0; i<W; ++i )
      {
         INT x, y;
         x = Src[i+1]-Src[i];
         x += ( xo-i );
         x = CLAMP_256(x);
         y = Src[i+Src_BpS]-Src[i];
         y += jo;         
         y = CLAMP_256(y);
         Dst[i] = Env_Map[x|(y<<8)];
      }
      Src += Src_BpS;
      Dst += Dst_BpS;
   }
}

#endif   // UNIX

/*****************************************************************/

#if 1  // #ifdef UNIX
EXTERN void Bumpify_II( PIXEL *Dst, PIXEL *Src, INT W, INT H,
   INT Dst_BpS, INT Src_BpS, INT xo, INT yo, PIXEL *Env_Map )
{
   INT j;
   for( j=0; j<H; ++j )
   {
      INT i, jo;

      jo = yo-j;
      for( i=0; i<W; ++i )
      {
         INT x, y;
         x = Src[i+1]-Src[i];
         x += ( xo-i );
         x = CLAMP_256(x);
         y = Src[i+Src_BpS]-Src[i];
         y += jo;         
         y = CLAMP_256(y);
         Dst[i] = Env_Map[x|(y<<8)];
      }
      Src += Src_BpS;
      Dst += Dst_BpS;
   }
}

#endif   // UNIX

/*****************************************************************/

