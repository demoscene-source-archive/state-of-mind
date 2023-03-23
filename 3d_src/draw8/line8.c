/***********************************************
 *    Lines drawing II                         *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

// Method for mixing colors
// #define MIX(A,B)  (A) = CLAMP_256( 0+ (A)+(B) )
#define MIX(A,B)  (A) = (B)
// #define MIX(A,B)  (A) |= (B)

/******************************************************************/
/******************************************************************/

EXTERN void _Draw_ALine_8( FLT xo, FLT yo, FLT x1, FLT y1 )
{
   FLT Tmp, dx, dy, S, T;
   INT yi, yf;

   if ( y1<yo ) { 
      Tmp = yo; yo=y1; y1=Tmp;
      Tmp = xo; xo=x1; x1=Tmp;
   }

   if ( (yo>=_RCst_.Clips[3]) || (y1<=_RCst_.Clips[2] ) ) return;

   dy = y1-yo; dx = x1-xo;

   if ( dy<.001 )     // Horizontal
   {
      INT y, x, xf;
      PIXEL *Ptr;
      PIXEL C1, C2, C3, Err;
      if ( dx<0.0 ) { Tmp = xo; xo=x1; x1=Tmp; }
      if ( (xo>=_RCst_.Clips[1]) || (x1<_RCst_.Clips[0]) ) return;
      if ( xo<_RCst_.Clips[0] ) xo = _RCst_.Clips[0];
      if ( x1>_RCst_.Clips[1] ) x1 = _RCst_.Clips[1];
      y = (INT)ceil(yo);
      Err = (PIXEL)ceil( 255.0*((FLT)y-yo) );
      C1 = AA_Table[ Err ];
      C2 = AA_Table[ 0xFF ];
      C3 = AA_Table[ 0xFF^Err ];
      x = (INT)ceil( xo ); xf = (INT)ceil( x1 );
      Ptr = ((PIXEL*)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_BpS - x;
      for( ; x<xf; ++x )
      { 
         MIX(Ptr[-_RCst_.Pix_BpS],C1);
         MIX(Ptr[_RCst_.Pix_BpS],C3);
         MIX(Ptr[0],C2);
         Ptr -= 1;
      }
      return;
   }


   S = (FLT)fabs( dx );
   if ( S<.001 )      // Vertical
   {
      INT x, y, yf;
      PIXEL *Ptr;
      PIXEL C1, C2, C3, Err;

      if ( (xo>=_RCst_.Clips[1]) || (xo<(_RCst_.Clips[0]+1.0)) ) return;
      x = (INT)ceil(xo);
      Err = (PIXEL)ceil( 255.0*((FLT)x-xo) );
      C1 = AA_Table[ 0xFF^Err ];
      C2 = AA_Table[ 0xFF ];
      C3 = AA_Table[ Err ];

      if ( y1>=_RCst_.Clips[3] ) y1 = _RCst_.Clips[3];
      if ( yo<_RCst_.Clips[2] ) yo = _RCst_.Clips[2];
      y = (INT)ceil( yo ); yf = (INT)ceil( y1 );
      Ptr = ((PIXEL*)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_BpS - x;

      for( ; y<yf; ++y ) 
      { 
         MIX(Ptr[-1],C1);
         MIX(Ptr[0],C2);
         MIX(Ptr[1],C3);
         Ptr+=_RCst_.Pix_BpS;
      }
      return;
   }

   if ( yo<_RCst_.Clips[2] ) { xo += (_RCst_.Clips[2]-yo)*dx/dy; yo = _RCst_.Clips[2]; }
   if ( y1>_RCst_.Clips[3] ) { x1 += (_RCst_.Clips[3]-y1)*dx/dy; y1 = _RCst_.Clips[3]; }

   if ( S<dy )    // >45'
   {
      INT xi;
      UINT Err, dErr, tErr;
      PIXEL *Ptr, C0;

      S = dx/dy;     // <1.0
      T = (FLT)( 65536.0/sqrt( 1.0 + (double)S*S ) );

         // Clip x
      if ( S<0.0 )
      {
         if ( (xo<_RCst_.Clips[0]) || (x1>=_RCst_.Clips[1]) ) return;
         if ( xo>_RCst_.Clips[1] ) { yo += (_RCst_.Clips[1]-xo)/S; xo = _RCst_.Clips[1]; }
         if ( x1<_RCst_.Clips[0] ) { y1 += (_RCst_.Clips[0]-x1)/S; x1 = _RCst_.Clips[0]; }
      }
      else
      {
         if ( (xo>=_RCst_.Clips[1]) || (x1<_RCst_.Clips[0]) ) return;
         if ( xo<_RCst_.Clips[0] ) { yo += (_RCst_.Clips[0]-xo)/S; xo = _RCst_.Clips[0]; }
         if ( x1>_RCst_.Clips[1] ) { y1 += (_RCst_.Clips[1]-x1)/S; x1 = _RCst_.Clips[1]; }
      }

      yi = (INT)ceil( yo );
      xo += S*( (FLT)yi-yo );
      xi = (INT)ceil( xo );      
      Ptr = ((PIXEL*)_RCst_.Base_Ptr) + (yi+1)*_RCst_.Pix_BpS - xi;
      Err = (UINT)ceil( T*( (FLT)xi-xo ) );

      yf = (INT)ceil( y1 );

      dErr = (UINT)ceil( fabs( T*S ) );
      tErr = (UINT)ceil( T );
      C0 = AA_Table[0xFF];

      if ( dx>0.0 )
      {
         Err = tErr - Err;
         yf -= yi;
         while( yf-- )
         {
            MIX( Ptr[-1], AA_Table[ (Err>>8) ] );
            MIX( Ptr[1], AA_Table[ 0xFF^(Err>>8) ] );
            MIX( Ptr[0], C0 );
            Ptr += _RCst_.Pix_BpS;
            Err += dErr;
            if ( Err>=tErr )
            {
               Err -= tErr;
               Ptr -= 1;
            }
         }
      }
      else
      {
         yf -= yi;
         while( yf-- )
         {
            MIX( Ptr[1], AA_Table[ (Err>>8) ] );
            MIX( Ptr[-1], AA_Table[ 0xFF^(Err>>8) ] );
            MIX( Ptr[0], C0 );
            Ptr += _RCst_.Pix_BpS;
            Err += dErr;
            if ( Err>=tErr )
            {
               Err -= tErr;
               Ptr += 1;
            }
         }
      }
   }
   else // <45'
   {
      INT xi, xf;
      UINT Err, dErr, tErr;
      PIXEL *Ptr, C0;

      S = dy/dx;     // <1.0
      T = (FLT)( 65536.0/sqrt( 1.0 + (double)S*S ) );

         // Clip x
      if ( S<0.0 )
      {
         if ( (xo<_RCst_.Clips[0]) || (x1>=_RCst_.Clips[1]) ) return;
         if ( xo>_RCst_.Clips[1] ) { yo += (_RCst_.Clips[1]-xo)*S; xo = _RCst_.Clips[1]; }
         if ( x1<_RCst_.Clips[0] ) { y1 += (_RCst_.Clips[0]-x1)*S; x1 = _RCst_.Clips[0]; }
      }
      else
      {
         if ( (xo>=_RCst_.Clips[1]) || (x1<_RCst_.Clips[0]) ) return;
         if ( xo<_RCst_.Clips[0] ) { yo += (_RCst_.Clips[0]-xo)*S; xo = _RCst_.Clips[0]; }
         if ( x1>_RCst_.Clips[1] ) { y1 += (_RCst_.Clips[1]-x1)*S; x1 = _RCst_.Clips[1]; }
      }

      xi = (INT)ceil( xo );
      yo += ( (FLT)xi-xo )*S;
      yi = (INT)ceil( yo );
      Ptr = ((PIXEL*)_RCst_.Base_Ptr) + (yi+1)*_RCst_.Pix_BpS - xi;
      Err = (UINT)ceil( T*( (FLT)yi-yo ) );

      xf = (INT)ceil( x1 );

      dErr = (UINT)ceil( fabs( T*S ) );
      tErr = (UINT)ceil( T );
      Err = tErr - Err;
      
      C0 = AA_Table[ 0xFF ];
      if ( dx>0.0 )
      {
         Ptr -= 1;
         xf -= xi;
         while( xf-- )
         {
            MIX( Ptr[_RCst_.Pix_BpS], AA_Table[ (Err>>8) ] );
            MIX( Ptr[-_RCst_.Pix_BpS], AA_Table[ 0xFF^(Err>>8) ] );
            MIX( Ptr[0], C0 );
            
            Ptr -= 1;
            Err += dErr;
            if ( Err>=tErr )
            {
               Err -= tErr;
               Ptr += _RCst_.Pix_BpS;
            }
         }
      }
      else
      {
         xi -= xf;
         while( xi-- )
         {
            MIX( Ptr[_RCst_.Pix_BpS], AA_Table[ (Err>>8) ] );
            MIX( Ptr[-_RCst_.Pix_BpS], AA_Table[ 0xFF^(Err>>8) ] );
            MIX( Ptr[0], C0 );
            Ptr += 1;
            Err += dErr;
            if ( Err>=tErr )
            {
               Err -= tErr;
               Ptr += _RCst_.Pix_BpS;
            }
         }
      }
   }
}

/******************************************************************/
/******************************************************************/
