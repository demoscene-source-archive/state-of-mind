/***********************************************
 *              Sprites II                     *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#include "main3d.h"
#include "sprite.h"

#ifdef UNIX

EXTERN void Paste_Sprite_Asm_32( )
{
   INT dH;
   UINT **Whats;
   INT *Offsets;

   Sp_dHo--;
   Sp_dWo--;
   Whats = (UINT**)Sp_Whats0 + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
   Sp_Dst += Sp_dWo;
   for( dH=-Sp_dHo; dH<=0; ++dH )
   {
      UINT *What;
      INT dW;

      What = Whats[dH];
      for( dW=-Sp_dWo; dW<=0; ++dW )
      { 
         UINT c;
         c = What[ Offsets[dW] ];
         ((UINT*)Sp_Dst)[dW] = c&0xFFFFFF;
      }
      Sp_Dst =(UINT*)((PIXEL*)Sp_Dst + Sp_BpS);
   }
}

EXTERN void Paste_Sprite_Asm_Sat_32( )
{
   INT dH;
   UINT **Whats;
   INT *Offsets;

   Sp_dHo--;
   Sp_dWo--;
   Whats = (UINT**)Sp_Whats0 + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
   Sp_Dst += Sp_dWo;
   for( dH=-Sp_dHo; dH<=0; ++dH )
   {
      UINT *What;
      INT dW;

      What = Whats[dH];
      for( dW=-Sp_dWo; dW<=0; ++dW )
      {
         UINT S777;
         S777 = What[ Offsets[dW] ]; // & 0x00FFFFFF;
         S777 += ((UINT*)Sp_Dst)[dW];
         ((UINT*)Sp_Dst)[dW] = RGB_777_TO_777(S777);
      }
      Sp_Dst =(UINT*)((PIXEL*)Sp_Dst + Sp_BpS);
   }
}

EXTERN void Paste_Sprite_Asm_Raw_Mask_32( )
{
   INT dH;
   UINT **Whats;
   INT *Offsets;
   UINT *Where;

   Sp_dHo--; Sp_dWo--;
   Whats = (UINT**)Sp_Whats0 + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
   Where = (UINT *)Sp_Dst + Sp_dWo;

   for( dH=-Sp_dHo; dH<=0; ++dH )
   {
      UINT *What;
      INT dW;

      What = Whats[dH];
      for( dW=-Sp_dWo; dW<=0; ++dW )
      {
         UINT S777 = What[ Offsets[dW] ];
         UINT M = S777 >> 24;
         if ( M )
            ((UINT*)Sp_Dst)[dW] = S777;
      }
      Sp_Dst =(UINT*)((PIXEL*)Sp_Dst + Sp_BpS);
   }
}

EXTERN void Paste_Sprite_Asm_Mask_32( )
{
   INT dH;
   UINT **Whats;
   INT *Offsets;
   UINT *Where;

   Sp_dHo--; Sp_dWo--;
   Whats = (UINT**)Sp_Whats0 + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
   Where = (UINT *)Sp_Dst + Sp_dWo;

   for( dH=-Sp_dHo; dH<=0; ++dH )
   {
      UINT *What;
      INT dW;

      What = Whats[dH];
      for( dW=-Sp_dWo; dW<=0; ++dW )
      {
         UINT S777 = What[ Offsets[dW] ];
         UINT M = S777 >> 24;
         if ( M )
         {
            S777 += RGB_777_TO_777_BLEND( M, Sp_Dst[dW] );
            ((UINT*)Sp_Dst)[dW] = RGB_777_TO_777(S777);
         }
         else ((UINT*)Sp_Dst)[dW] = S777;
      }
      Sp_Dst = (UINT*)((PIXEL*)Sp_Dst + Sp_BpS);
   }
}

#endif      //    UNIX

/******************************************************************/
/******************************************************************/

EXTERN SPRITE *Load_Sprite_777_JPEG( STRING Name )
{
   BITMAP_16 *Src;
   SPRITE *Sprite = NULL;
   BYTE *RGB;
   UINT *Dst;
   INT i, j, Nb_Cmp;

   Src = Load_JPEG( Name, &Nb_Cmp );
   if ( Src==NULL ) return( NULL );

   Sprite = New_Fatal_Object( 1, SPRITE );
   if ( Sprite==NULL ) return( NULL );

   Sprite->BpS = Src->Width*sizeof(UINT);
   Sprite->dU = Src->Width;
   Sprite->dV = Src->Height;
   Sprite->Uo = Sprite->Vo = 0;

   Sprite->Bits = (BYTE *)New_Fatal_Object( Src->Width*Src->Height, UINT );
   RGB = (BYTE*)Src->Bits;
   Dst = (UINT *)Sprite->Bits;
   if ( Nb_Cmp==3 )
   {
      for ( j=Src->Height; j>0; --j )
      {
         for( i=0; i<Src->Width; ++i )
         {
            UINT R, G, B;
            B = RGB[3*i+0]; G = RGB[3*i+1]; R = RGB[3*i+2];
            Dst[i] = RGB_TO_777(R,G,B);
            Dst[i] &= 0x00FFFFFF;
         }
         Dst += Src->Width; 
         RGB += 3*Src->Width;
      }
   }
   else if ( Nb_Cmp==1 )
   {
      for ( j=Src->Height; j>0; --j )
      {
         for( i=0; i<Src->Width; ++i )
         {
            UINT R;
            R = RGB[i];
            Dst[i] = RGB_TO_777(R,R,R);
            Dst[i] &= 0x00FFFFFF;
         }
         Dst += Src->Width; 
         RGB += Src->Width;
      }
   }
   else M_Free( Sprite );  // => error
// Failed:
   Destroy_16b( Src );
   return( Sprite );
}

/******************************************************************/
/******************************************************************/

static void Mask_Sprite_777_Raw( SPRITE *Sprite, BYTE *Src, INT Mix_Levels )
{
   UINT *Dst;
   INT i, j;

   Dst = (UINT *)Sprite->Bits;
   for ( j=Sprite->dV; j>0; --j )
   {
      for( i=0; i<Sprite->dU; ++i )
      {
         Dst[i] |= (UINT)(Src[i]*Mix_Levels/256)<<24;
      }
      Dst += Sprite->dU; 
      Src += Sprite->dU;
   }
}

EXTERN SPRITE *Mask_Sprite_777_JPEG( STRING Name, SPRITE *Sprite, INT Mix_Levels )
{
   BITMAP_16 *Src;
   INT Nb_Cmp;

   Src = Load_JPEG( Name, &Nb_Cmp );
   if ( Src==NULL ) return( NULL );
   if ( Nb_Cmp!=1 ) return( NULL );
   Mask_Sprite_777_Raw( Sprite, (BYTE*)Src->Bits, Mix_Levels );
   Destroy_16b( Src );
   return( Sprite );
}

EXTERN SPRITE *Mask_Sprite_777_GIF( STRING Name, SPRITE *Sprite, INT Mix_Levels )
{
   BITMAP *Src;
   INT i;

   Src = Load_GIF( Name );
   if ( Src==NULL ) return( NULL );
   for( i=0; i<Src->Width*Src->Height; ++i )
      Src->Bits[i] = Src->Pal[ 3*Src->Bits[i] ];
   Mask_Sprite_777_Raw( Sprite, Src->Bits, Mix_Levels );
   Destroy_Bitmap( Src );
   return( Sprite );
}

EXTERN void Destroy_Sprite_777( SPRITE *S )
{
   if ( S==NULL ) return;
   M_Free( S->Bits );
   M_Free( S );
}

/******************************************************************/
/******************************************************************/

EXTERN void Paste_Sprite_Bitmap_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   if ( Setup_Paste_Sprite( (void*)Dst, BpS, sizeof(UINT),
      xo, yo, R, Sprite )!=NULL ) Paste_Sprite_Asm_32( );
}

EXTERN void Paste_Sprite_Bitmap_Sat_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   if ( Setup_Paste_Sprite( (void*)Dst, BpS, sizeof(UINT),
      xo, yo, R, Sprite )!=NULL ) Paste_Sprite_Asm_Sat_32( );
}

EXTERN void Paste_Sprite_Bitmap_Mask_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   if ( Setup_Paste_Sprite( (void*)Dst, BpS, sizeof(UINT),
      xo, yo, R, Sprite )!=NULL ) Paste_Sprite_Asm_Mask_32( );
}

EXTERN void Paste_Sprite_Bitmap_Raw_Mask_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   if ( Setup_Paste_Sprite( (void*)Dst, BpS, sizeof(UINT),
      xo, yo, R, Sprite )!=NULL ) Paste_Sprite_Asm_Raw_Mask_32( );
}

/******************************************************************/
/******************************************************************/

