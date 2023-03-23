/***********************************************
 *    Bilinear tables + Loop                   *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

/*********************************************************************/
/*********************************************************************/

extern void _Draw_UVc_Bilin_16( );
EXTERN float UV_Mult_Tab_I[64*256][2];
EXTERN float UV_Mult_Tab_II[64*256][2];
EXTERN float Cf_32_RB[65536];
EXTERN float Cf_32_G[65536];
// Cf  = (C3*Tab[3]);// & 0x7c1f83e0;  // try 0xec1f83e0!!
// 1.0 => +exp:0x3ff00000

/*********************************************************************/

EXTERN void Init_UV_Mult_Tab_16b( )
{
   UINT i, uu, u, vv, v;

   for( i=0; i<256*256; ++i )
   {
      UINT I;
      Cf_32_RB[i] = (USHORT)(i&0xf81f);
      Cf_32_G[i] = (USHORT)(i&0x07e0);
   }
   for( vv=0; vv<64; ++vv )
      for( uu=0; uu<256; ++uu )
      {
         u = uu/4;
         v = vv;
         UV_Mult_Tab_I[uu+vv*256][0] = (float)( (64-u)*(64-v)/64 )/64.0;
         UV_Mult_Tab_I[uu+vv*256][1] = (float)( u*(64-v)/64 )/64.0;
         UV_Mult_Tab_II[uu+vv*256][0] = (float)( v*(64-u)/64 )/64.0;
         UV_Mult_Tab_II[uu+vv*256][1] = (float)( u*v/64 )/64.0;
      }
}

/*********************************************************************/

#ifdef UNIX
EXTERN void _Draw_UVc_Bilin_16( )
{
   INT y, dy;
   USHORT *Dst;
   INT dU, dV; 

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*(_RCst_.Pix_BpS/2);
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      FLT uiz, viz, iz, Uf, Vf, z;
      INT U, V, dU, dV;
      USHORT *Ptr;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      if ( Len>=0 ) goto Skip;

      uiz = Scan_UiZ[y];
      viz = Scan_ViZ[y];
      iz  = Scan_iZ[y];
      z = 1.0/iz;
      Uf = uiz*z; Vf = viz*z;
      U = (INT)( Uf ); V = (INT)( Vf );
      iz += DyiZ*Len;
      z = 1.0/iz;
      uiz += DyU*Len;
      viz += DyV*Len;
      Uf = uiz*z - Uf;
      Vf = viz*z - Vf;
      dU = (INT)( -Uf/Len );
      dV = (INT)( -Vf/Len );

      while( Len<0 )
      {
         UINT Off, I, Tab;
         USHORT C;
         double C_RB, C_G;

         Tab  = (V>>2)&0x3f00;
         Tab |= (U>>8)&0x00ff;

         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         C = ((USHORT*)UV_Src)[ Off ];
         C_RB = Cf_32_RB[C]*UV_Mult_Tab_I[Tab][0];
         C_G  = Cf_32_G[C]*UV_Mult_Tab_I[Tab][0];
         C = ((USHORT*)UV_Src)[ Off+1 ];
         C_RB += Cf_32_RB[C]*UV_Mult_Tab_I[Tab][1];
         C_G  += Cf_32_G[C]*UV_Mult_Tab_I[Tab][1];
         C = ((USHORT*)UV_Src)[ Off+256 ];
         C_RB += Cf_32_RB[C]*UV_Mult_Tab_II[Tab][0];
         C_G  += Cf_32_G[C]*UV_Mult_Tab_II[Tab][0];
         C = ((USHORT*)UV_Src)[ Off+257 ];
         C_RB += Cf_32_RB[C]*UV_Mult_Tab_II[Tab][1];
         C_G  += Cf_32_G[C]*UV_Mult_Tab_II[Tab][1];

         C = (((UINT)C_RB) ) & 0xf81f;
         C |= (((UINT)C_G) ) & 0x07e0;
         Ptr[Len] = C;
         U += dU;
         V += dV;
         Len++;
      }

Skip:
      Dst += (_RCst_.Pix_BpS/2);
      ++y;
      dy--;
   }
}
#endif      // UNIX

/*********************************************************************/
