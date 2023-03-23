/***********************************************
 *              Sprites                        *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"
#include "sprite.h"

EXTERN INT   Sp_Offsets0[ MAX_WIDTH ];
EXTERN void *Sp_Whats0[ MAX_SCANS ];
EXTERN UINT *Sp_Dst;
EXTERN INT   Sp_BpS;
EXTERN INT   Sp_dWo, Sp_dHo;
EXTERN void *Sp_CMap;

/******************************************************************/
/******************************************************************/

#ifdef UNIX

EXTERN void Paste_Sprite_Asm_8( )
//PIXEL *Dst, INT BpS, PIXEL **Whats, INT *Offsets, INT dWo, INT dHo )
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
         if ( c ) ((BYTE*)Where)[dW] = c;
      }
      Where += Sp_BpS;
   }
}

EXTERN void Paste_Sprite_Asm_8_Map16( )
//PIXEL *Dst, INT BpS, PIXEL **Whats, INT *Offsets, INT dWo, INT dHo )
{
   INT dH;
   PIXEL **Whats;
   USHORT *Where;
   INT *Offsets;

   Sp_dHo--; Sp_dWo--;
   Whats = ((PIXEL**)Sp_Whats0) + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
   Where = ((USHORT*)Sp_Dst) + Sp_dWo;

   for( dH=-Sp_dHo; dH<=0; ++dH )
   {
      PIXEL *What;
      INT dW;

      What = Whats[dH];
      for( dW=-Sp_dWo; dW<=0; ++dW )
      { 
         PIXEL c;
         c = What[ Offsets[dW] ];
         if ( c ) Where[dW] = ((USHORT*)Sp_CMap)[c];
      }
      Where = (USHORT*)( (BYTE*)Where + Sp_BpS );
   }
}

EXTERN void Paste_Sprite_Asm_8_Map16_Or( )
{
   INT dH;
   PIXEL **Whats;
   USHORT *Where;
   INT *Offsets;

   Sp_dHo--; Sp_dWo--;
   Whats = ((PIXEL**)Sp_Whats0) + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
   Where = ((USHORT*)Sp_Dst) + Sp_dWo;

   for( dH=-Sp_dHo; dH<=0; ++dH )
   {
      PIXEL *What;
      INT dW;

      What = Whats[dH];
      for( dW=-Sp_dWo; dW<=0; ++dW )
      { 
         PIXEL c;
         c = What[ Offsets[dW] ];
         if ( c ) Where[dW] |= ((USHORT*)Sp_CMap)[c];
      }
      Where = (USHORT*)( (BYTE*)Where + Sp_BpS );
   }
}

#endif      //    UNIX

/******************************************************************/

#ifdef UNIX

EXTERN void Paste_Sprite_Asm_16( )
// USHORT *Dst, INT BpS, PIXEL **Whats, INT *Offsets, INT dWo, INT dHo )
{
   INT dH;
   USHORT *Where;
   USHORT **Whats;
   INT *Offsets;   

   Where = (USHORT *)Sp_Dst;
   Sp_dHo--; Sp_dWo--;
   Whats = ((USHORT**)Sp_Whats0) + Sp_dHo;
   Offsets = Sp_Offsets0 + Sp_dWo;
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
         if ( c ) Where[dW] = c;
      }
      Where += Sp_BpS;
   }
}

#endif      //    UNIX

/******************************************************************/

EXTERN void *Setup_Paste_Sprite( void *Dst, INT BpS, INT Dst_Quantum, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   BYTE *What;
   FLT U, V;
   FLT x, xi, xf;
   FLT y, yi, yf;
   FLT Tmp;
   INT F, dF, Err, dErr, Steps_X, Steps_Y, i;      // DDA
   INT X, Y;

   Sp_Dst = NULL;

   Tmp = (0.5f*R)*Sprite->dU;
   x = xi = xo - Tmp;
   xf = xo + Tmp;
   Tmp = (0.5f*R)*Sprite->dV;
   y = yi = yo - Tmp;
   yf = yo + Tmp;

   if ( xf<=_RCst_.Clips[0] ) return( NULL );
   if ( xf>_RCst_.Clips[1] ) { xf = _RCst_.Clips[1]; }
   if ( yf<=_RCst_.Clips[2] ) return( NULL );
   if ( yf>_RCst_.Clips[3] ) { yf = _RCst_.Clips[3]; }
   if ( x>_RCst_.Clips[1] ) return( NULL );
   if ( x<_RCst_.Clips[0] ) { x = _RCst_.Clips[0]; }
   if ( y>_RCst_.Clips[3] ) return( NULL );
   if ( y<_RCst_.Clips[2] ) { y = _RCst_.Clips[2]; }

   R = 1.0f / R;

      // This should be in ASM. Oh, well...

         // X offsets DDA

   X = (INT)ceil( x );
   Dst = (void*)((BYTE*)Dst + X*Dst_Quantum);

   U = (FLT)Sprite->Uo;
   U += ( (FLT)X - xi )*R;

   dF = (INT)(R*65536.0);
   dErr = dF & 0xFFFF;
   dF >>= 16;
   F = (INT)(U*65536.0);
   Err = F & 0xFFFF;
   F >>= 16;

   Steps_X = (INT)ceil(xf) - X;
   Sp_dWo = Steps_X;
   for( i=0; i<Steps_X; ++i )
   {
      Sp_Offsets0[i] = F;
      F += dF; Err += dErr;
      if ( Err&0x10000 ) { Err &= 0xFFFF; F++; }
   }

         // Y offsets DDA

   Y = (INT)ceil( y );
   Dst = (void*)((BYTE*)Dst + Y*BpS);
   Sp_Dst = (UINT*)Dst;
   Sp_BpS = BpS;

   V = (FLT)Sprite->Vo;
   V += ( (FLT)Y - yi )*R;

      // dF, dErr unchanged...
   F = (INT)(V*65536.0);
   Err = F & 0xFFFF;
   F >>= 16;

   dF *= Sprite->BpS;
   What = Sprite->Bits + F*Sprite->BpS;

   Steps_Y = (INT)ceil(yf) - Y;
   Sp_dHo = Steps_Y;
   for( i=0; i<Steps_Y; ++i )
   {
      Sp_Whats0[i] = (void*)What;
      What += dF;
      Err += dErr;
      if ( Err&0x10000 ) { Err &= 0xFFFF; What += Sprite->BpS; }
   }
   return( Sp_Dst );
}

/******************************************************************/

EXTERN void Paste_Sprite_Bitmap_8( PIXEL *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   if ( Setup_Paste_Sprite( (void*)Dst, BpS, sizeof(BYTE), 
      xo, yo, R, Sprite )!=NULL ) Paste_Sprite_Asm_8( );
}

/******************************************************************/

EXTERN void Paste_Sprite_Bitmap_8_Map16_Or( USHORT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite, USHORT *CMap )
{
   Sp_CMap = CMap;
   if ( Setup_Paste_Sprite( (void*)Dst, BpS, sizeof(USHORT), 
      xo, yo, R, Sprite )!=NULL ) Paste_Sprite_Asm_8_Map16_Or( );
}

EXTERN void Paste_Sprite_Bitmap_8_Map16( USHORT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite, USHORT *CMap )
{
   Sp_CMap = CMap;
   if ( Setup_Paste_Sprite( (void*)Dst, BpS, sizeof(USHORT), 
      xo, yo, R, Sprite )!=NULL ) Paste_Sprite_Asm_8_Map16( );
}

/******************************************************************/

EXTERN void Paste_Sprite_Bitmap_16( USHORT *Dst, INT BpS, 
   FLT xo, FLT yo, FLT R, SPRITE *Sprite )
{
   if ( Setup_Paste_Sprite( (void*)Dst, BpS, sizeof(USHORT),
      xo, yo, R, Sprite )!=NULL ) Paste_Sprite_Asm_16( );
}

/******************************************************************/
/******************************************************************/

EXTERN SPRITE *Btm_16_To_Sprite( BITMAP_16 *Src )
{
   SPRITE *Sprite;

   if ( Src==NULL ) return( NULL );
   Sprite = New_Fatal_Object( 1, SPRITE );
   if ( Sprite==NULL ) return( NULL );

   Sprite->Bits = (PIXEL *)Src->Bits;
   Sprite->dU = Src->Width;
   Sprite->dV = Src->Height;
   Sprite->Uo = Sprite->Vo = 0;
   Sprite->BpS = Src->Width*sizeof(USHORT);
   Src->Bits = NULL;
   Destroy_16b( Src );
   return( Sprite );
}

EXTERN void Destroy_Sprite( SPRITE *S )
{
   if ( S==NULL ) return;
   M_Free( S->Bits );
   M_Free( S );
}

/******************************************************************/
