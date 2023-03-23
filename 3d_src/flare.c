/***********************************************
 *              Lens flare                     *
 * Coded during the Saturne 97 :)              *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"
#include "color.h"
#include "sprite.h"  // for flares..

/******************************************************************/

static FLT Flare_Factor[8] = {
   .1f, .15f, .3f, .45f, .6f, .85f, .65f, 1.0f
};

/******************************************************************/

static void Draw_Light_Flares( LIGHT *Light, FLT Base_Ratio )
{
   INT i;
   for( i=0; i<Light->Flare->Nb_Sprites; ++i )
   {
      FLT A, R, x, y;
      A = Light->Flare->Sprites[i].Alpha;
      x = A*_RCst_._Cx_ + (1.0f-A)*Light->Xp;
      y = A*_RCst_._Cy_ + (1.0f-A)*Light->Yp;
      R = Base_Ratio*Light->Flare->Sprites[i].Scale;

         // Mega-cast :)
      ((void (*)( FLT,FLT,FLT,FLARE*,FLARE_SPRITE*))
         Primitives[RENDER_PASTE_FLARE])
            ( x, y, R, Light->Flare, &Light->Flare->Sprites[i] );
   }
}

static FLT Compute_Flare_Visibility( LIGHT *Light )
{
   USHORT Z_Light;
   INT Pts_Seen, X, Y, dX, dY, Where;
   FLT Inv_Z;

//   Z_Light = (USHORT)( 32767.0*_RCst_.Clips[6] * Light->Inv_Z );
   Inv_Z = ( Light->Inv_Z*_RCst_.Z_Scale + _RCst_.Z_Off )/65536.0f;
   Z_Light = (USHORT)( Inv_Z );
   X = (INT)Light->Xp; Y = (INT)Light->Yp;

   Where = Light->Where;
   if ( ZBuffer[Where]<Z_Light ) Pts_Seen = 4;
   else Pts_Seen = 0;

   dX = dY = 1;
   if ( X>dX )
   {
      if ( ZBuffer[Where-dX]<Z_Light ) Pts_Seen+=1;
   }
   if ( X<_RCst_.Pix_Width-dX )
   {
      if ( ZBuffer[Where+dX]<Z_Light ) Pts_Seen+=1;
   }
   if ( Y>dY )
   {
      if ( ZBuffer[Where-_RCst_.Pix_Width*dY]<Z_Light ) Pts_Seen+=1;
   }
   if ( Y<_RCst_.Pix_Height-dY )
   {
      if ( ZBuffer[Where+_RCst_.Pix_Width*dY]<Z_Light ) Pts_Seen+=1;
   }
   if ( Pts_Seen==0 ) return( 0.0 );
   return( Flare_Factor[ Pts_Seen-1 ]*Light->Inv_Z*_RCst_.Clips[4] ); // *(Inv_Z/65536.0) );
}

/******************************************************************/

static void Do_Light_Flare( LIGHT *Light )
{
   FLT Base_Ratio;

   if ( Light->Inv_Z>=_RCst_.Clips[5] ) return;
   Base_Ratio = Light->Flare_Scale;
   Base_Ratio *= Compute_Flare_Visibility( Light );
   if ( Base_Ratio>0.0 ) 
      Draw_Light_Flares( Light, Base_Ratio );
}

EXTERN void Render_Light_Flare( WORLD *W )
{
   for( Cur_Light = (LIGHT*)W->Lights; 
         Cur_Light!=NULL; Cur_Light=(LIGHT*)Cur_Light->Next )
   {
      if ( Cur_Light->Flare==NULL ) continue;
      if ( Cur_Light->Where<0 ) continue;
      Do_Light_Flare( Cur_Light );
   }
}

/******************************************************************/

#if 0
EXTERN PIXEL *Promote_Flare_Btm( BITMAP *Btm, FCOLOR *Big_Tab, INT Total_Col )
{
   PIXEL *New;
   INT i;

   New = New_Fatal_Object( Btm->Nb_Col*256, PIXEL );
   if ( New==NULL ) return( NULL );
   for( i=0; i<Btm->Nb_Col; ++i )
   {
      INT j, k;
      FLT R, G, B;
      R = Btm->Pal[i*3]/256.0;
      G = Btm->Pal[i*3+1]/256.0;
      B = Btm->Pal[i*3+2]/256.0;
      for( k=0; k<Total_Col; ++k )
      {
         FLT R2, G2, B2;
         FLT Best;
         INT N;

            // Lighten col
         R2 = R + Big_Tab[128+k*256][0]; if ( R2>1.0 ) R2 = 1.0;
         G2 = G + Big_Tab[128+k*256][1]; if ( G2>1.0 ) G2 = 1.0;
         B2 = B + Big_Tab[128+k*256][2]; if ( B2>1.0 ) B2 = 1.0;

         Best = _HUGE_; N = -1;
         for( j=0; j<256; ++j )
         {
            FLT D;
            D  = fabs(R2-Big_Tab[j*256+128][0]);
            D += fabs(G2-Big_Tab[j*256+128][1]);
            D += fabs(B2-Big_Tab[j*256+128][2]);
            if ( D<Best ) { Best = D; N = j; }
         }
         New[i*256+k] = N;
      }
      fprintf( stderr, "%d   \r", i );
   }
   return( New );
}
#endif

/******************************************************************/

EXTERN void *Setup_Paste_Flare( INT Dst_Quantum, FLT xo, FLT yo, FLT R, 
   FLARE *Flare, FLARE_SPRITE *Sprite )
{
   BYTE *What;
   FLT U, V;
   FLT x, xi, xf, y, yi, yf;
   FLT Tmp;
   INT F, dF, Err, dErr, i;      // DDA
   INT Wo, X, Y;

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
   Sp_Dst = (UINT*)_RCst_.Base_Ptr;
   Sp_Dst = (UINT*)( (BYTE*)Sp_Dst+ X*Dst_Quantum );

   U = (FLT)Sprite->Uo;
   U += ( (FLT)X - xi )*R;

   dF = (INT)(R*65536.0);
   dErr = dF & 0xFFFF;
   dF >>= 16;
   F = (INT)(U*65536.0);
   Err = F & 0xFFFF;
   F >>= 16;

   Sp_dWo = (INT)ceil(xf) - X;
   for( i=0; i<Sp_dWo; ++i )
   {
      Sp_Offsets0[i] = F; F += dF; Err += dErr;
      if ( Err&0x10000 ) { Err &= 0xFFFF; F++; }
   }

   Y = (INT)ceil( y );
   Sp_BpS = _RCst_.Pix_BpS;
   Sp_Dst = (UINT*)( (BYTE*)Sp_Dst+ Y*Sp_BpS );

   V = (FLT)Sprite->Vo;
   V += ( (FLT)Y - yi )*R;
   F = (INT)(V*65536.0);
   Err = F & 0xFFFF;
   F >>= 16;

   Wo = Flare->BpS;
   dF *= Wo;
   What = (BYTE*)Flare->Bits;
   What += F*Wo;

   Sp_dHo = (INT)ceil(yf) - Y;
   for( i=0; i<Sp_dHo; ++i )
   {
      Sp_Whats0[i] = What; What += dF; Err += dErr;
      if ( Err&0x10000 ) { Err &= 0xFFFF; What += Wo; }
   }
   return( Sp_Dst );
}


/******************************************************************/
