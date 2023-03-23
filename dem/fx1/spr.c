/***********************************************
 *              Sprites                        *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "demo.h"

/******************************************************************/

EXTERN INT Offsets0[ MAX_WIDTH ];
EXTERN void *Whats0[ MAX_SCANS ];
EXTERN UINT *Sp_Dst;
EXTERN INT Sp_BpS;
EXTERN INT dWo, dHo;

#ifdef UNIX

EXTERN void Paste_Sprite_Asm_32( )
{
   INT dH;
   UINT **Whats;
   INT *Offsets;

   dHo--; dWo--;
   Whats = (UINT**)Whats0 + dHo;
   Offsets = Offsets0 + dWo;
   Sp_Dst += dWo;
   for( dH=-dHo; dH<=0; ++dH )
   {
      UINT *What;
      INT dW;

      What = Whats[dH];
      for( dW=-dWo; dW<=0; ++dW )
      { 
         UINT c, co, f, r, g, b;
         UINT A;
         c = What[ Offsets[dW] ];
         Sp_Dst[dW] = c&0xFFFFFF;
      }
      (PIXEL*)Sp_Dst += Sp_BpS;
   }
}

EXTERN void Paste_Sprite_Asm_Sat_32( )
{
   INT dH;
   UINT **Whats;
   INT *Offsets;

   dHo--; dWo--;
   Whats = (UINT**)Whats0 + dHo;
   Offsets = Offsets0 + dWo;
   Sp_Dst += dWo;
   for( dH=-dHo; dH<=0; ++dH )
   {
      UINT *What;
      INT dW;

      What = Whats[dH];
      for( dW=-dWo; dW<=0; ++dW )
      {
         UINT S777;
         S777 = What[ Offsets[dW] ]; // & 0x00FFFFFF;
         S777 += Sp_Dst[dW];
         Sp_Dst[dW] = RGB_777_TO_777(S777);
      }
      (PIXEL*)Sp_Dst += Sp_BpS;
   }
}

EXTERN void Paste_Sprite_Asm_Mask_32( )
{
   INT dH;
   UINT **Whats;
   INT *Offsets;

   dHo--; dWo--;
   Whats = (UINT**)Whats0 + dHo;
   Offsets = Offsets0 + dWo;
   Sp_Dst += dWo;
   for( dH=-dHo; dH<=0; ++dH )
   {
      UINT *What;
      INT dW;

      What = Whats[dH];
      for( dW=-dWo; dW<=0; ++dW )
      {
         UINT S777, C, M;
         S777 = What[ Offsets[dW] ];
         M = S777 >> 24;
         if ( M )
         {
            S777 += RGB_777_TO_777_BLEND( M, Sp_Dst[dW] );
            Sp_Dst[dW] = RGB_777_TO_777(S777);
         }
         else Sp_Dst[dW] = S777;
      }
      (PIXEL*)Sp_Dst += Sp_BpS;
   }
}

#endif      //    UNIX

/******************************************************************/

static void Setup_Paste_Sprite_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   BYTE *What;
   FLT U, V;
   FLT x, xi, xf;
   FLT y, yi, yf;
   FLT Tmp;
   INT F, dF, Err, dErr, Steps_X, Steps_Y, i;      // DDA
   INT Wo;
   INT X, Y;

   Sp_Dst = NULL;

   Tmp = (0.5*R)*Sprite->dU;
   x = xi = xo - Tmp;
   xf = xo + Tmp;
   Tmp = (0.5*R)*Sprite->dV;
   y = yi = yo - Tmp;
   yf = yo + Tmp;

   if ( xf<=_RCst_.Clips[0] ) return;
   if ( xf>_RCst_.Clips[1] ) { xf = _RCst_.Clips[1]; }
   if ( yf<=_RCst_.Clips[2] ) return;
   if ( yf>_RCst_.Clips[3] ) { yf = _RCst_.Clips[3]; }
   if ( x>_RCst_.Clips[1] ) return;
   if ( x<_RCst_.Clips[0] ) { x = _RCst_.Clips[0]; }
   if ( y>_RCst_.Clips[3] ) return;
   if ( y<_RCst_.Clips[2] ) { y = _RCst_.Clips[2]; }

   R = 1.0 / R;

      // This should be in ASM. Oh, well...

         // X offsets DDA

   X = (INT)ceil( x );
   Dst += X;

   U = (FLT)Sprite->Uo;
   U += ( (FLT)X - xi )*R;

   dF = (INT)(R*65536.0);
   dErr = dF & 0xFFFF;
   dF >>= 16;
   F = (INT)(U*65536.0);
   Err = F & 0xFFFF;
   F >>= 16;

   Steps_X = (INT)ceil(xf) - X;
   for( i=0; i<Steps_X; ++i )
   {
      Offsets0[i] = F;
      F += dF; Err += dErr;
      if ( Err&0x10000 )
      {
         Err &= 0xFFFF;
         F++;
      }
   }

         // Y offsets DDA

   Y = (INT)ceil( y );
   (PIXEL*)Dst += Y*BpS;

   V = (FLT)Sprite->Vo;
   V += ( (FLT)Y - yi )*R;

      // dF, dErr unchanged...
   F = (INT)(V*65536.0);
   Err = F & 0xFFFF;
   F >>= 16;

   dF *= Sprite->BpS;
   What = Sprite->Bits + F*Sprite->BpS;

   Steps_Y = (INT)ceil(yf) - Y;
   for( i=0; i<Steps_Y; ++i )
   {
      Whats0[i] = (void*)What;
      What += dF; Err += dErr;
      if ( Err&0x10000 )
      {
         Err &= 0xFFFF;
         What += Sprite->BpS;
      }
   }

   Sp_Dst = Dst; Sp_BpS = BpS;
   dWo = Steps_X; dHo = Steps_Y;
}

EXTERN void Paste_Sprite_Bitmap_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   Setup_Paste_Sprite_32( Dst, BpS, xo, yo, R, Sprite );
   if ( Sp_Dst!=NULL ) Paste_Sprite_Asm_32( );
}

EXTERN void Paste_Sprite_Bitmap_Sat_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   Setup_Paste_Sprite_32( Dst, BpS, xo, yo, R, Sprite );
   if ( Sp_Dst!=NULL ) Paste_Sprite_Asm_Sat_32( );
}

EXTERN void Paste_Sprite_Bitmap_Mask_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   Setup_Paste_Sprite_32( Dst, BpS, xo, yo, R, Sprite );
   if ( Sp_Dst!=NULL ) Paste_Sprite_Asm_Mask_32( );
}

EXTERN void Paste_Sprite_Bitmap_Raw_Mask_32( UINT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   Setup_Paste_Sprite_32( Dst, BpS, xo, yo, R, Sprite );
   if ( Sp_Dst!=NULL ) Paste_Sprite_Asm_Raw_Mask_32( );
}

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
Failed:
   Destroy_16b( Src );
   return( Sprite );
}

static void Mask_Sprite_777_Raw( SPRITE *Sprite, BYTE *Src )
{
   UINT *Dst;
   INT i, j;

   Dst = (UINT *)Sprite->Bits;
   for ( j=Sprite->dV; j>0; --j )
   {
      for( i=0; i<Sprite->dU; ++i )
      {
         Dst[i] |= (UINT)(Src[i]*(MIX_LEVELS)/256)<<24;
      }
      Dst += Sprite->dU; 
      Src += Sprite->dU;
   }
}

EXTERN SPRITE *Mask_Sprite_777_JPEG( STRING Name, SPRITE *Sprite )
{
   BITMAP_16 *Src;
   INT Nb_Cmp;

   Src = Load_JPEG( Name, &Nb_Cmp );
   if ( Src==NULL ) return( NULL );
   if ( Nb_Cmp!=1 ) return( NULL );
   Mask_Sprite_777_Raw( Sprite, (BYTE*)Src->Bits );
   Destroy_16b( Src );
   return( Sprite );
}

EXTERN SPRITE *Mask_Sprite_777_GIF( STRING Name, SPRITE *Sprite )
{
   BITMAP *Src;
   INT i;

   Src = Load_GIF( Name );
   if ( Src==NULL ) return( NULL );
   for( i=0; i<Src->Width*Src->Height; ++i )
      Src->Bits[i] = Src->Pal[ 3*Src->Bits[i] ];
   Mask_Sprite_777_Raw( Sprite, Src->Bits );
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
