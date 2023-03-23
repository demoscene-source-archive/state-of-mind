/***********************************************
 *   All loops in C, for UNIX...               *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"
#include "bump.h"

/******************************************************************/
/******************************************************************/

EXTERN void _Draw_UV_Bump( )
{
   INT y, dy;
   USHORT *Dst;

      /* Now, draw! Here lies the Mythical Inner-Loop :) */

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      INT U, V;
      USHORT *Ptr;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];

      U = Scan_U[y];
      V = Scan_V[y];
      while( Len<0 )
      {
         USHORT Off;
         // char u, v;
         // u = (char)((U>>16)&0x00FF) + (Ptr[Len]&0xFF);
         // v = (char)((V>>16)&0x00FF) + ((Ptr[Len]>>8)&0xFF);
         // Off = (INT)( ((PIXEL)u) | ((PIXEL)v)<<8 );
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         Off += Ptr[Len];
         Ptr[Len] = ( ( UV_Src[ Off ]&0xFF ) ) | 0x80;
         U -= dyU;
         V -= dyV;
         Len++;
      }
      Dst += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}

EXTERN void _Draw_Bump2( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      INT U, V;
      USHORT *Ptr;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];

      U = Scan_U[y];
      V = Scan_V[y];
      while( Len<0 )
      {
         USHORT Off;
         INT x, y;
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );

         x = UV_Src[Off+1] - UV_Src[Off];
         x += (INT)( Ptr[Len] & 0xFF );
         x = CLAMP_256( x );

         y = UV_Src[Off+256] - UV_Src[Off];
         y += (INT)( Ptr[Len]>>8 );
         y = CLAMP_256( y );

         Ptr[Len] = ( UV_Src2[ x|(y<<8) ] | 0x80 );
         U -= dyU;
         V -= dyV;
         Len++;
      }
      Dst += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}

/******************************************************************/

EXTERN void _Draw_UVc_Test( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   DyU *= 4.0; DyV *= 4.0; DyiZ *= 4.0;
   while( dy>0 )   
   {
      INT Len, Left_Over;
      FLT uiz, viz, iz, z;
      FLT zo;
      INT U, V;
      USHORT *Ptr;

      iz  = Scan_iZ[y]; z = 1.0f/iz;

      Len = Scan_Pt2[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt1[y];
      if ( Len<=0 ) goto Skip;
      Left_Over = (Len)&0x03;
      Len = Len>>2;

      uiz = Scan_UiZ[y]; U = (INT)( uiz*z );
      viz = Scan_ViZ[y]; V = (INT)( viz*z );

      iz -= DyiZ; uiz -= DyU; viz -= DyV;
      z = 1.0f/iz;

      zo = z;

      while( --Len>=0 )
      {
         INT Span;
         INT U2, V2, dU, dV;

         // U2 = (INT)( uiz*z ); V2 = (INT)( viz*z );
         U2 = (INT)( uiz*zo ); V2 = (INT)( viz*zo );
         dU = (U2-U)>>2; dV = (V2-V)>>2;

         uiz -= DyU; viz -= DyV; iz -= DyiZ;
         // z = 1.0/iz;

         zo *= (2.0f-iz*zo);
         // zo = zo*(2.0-iz*zo);
         // fprintf( stderr, "iz=%e  => z=%e  zo=%e\n", iz, z, zo );

         Span = -4; Ptr -= Span;
         while( Span<0 )
         {
            INT Off;
            Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
            Ptr[Span] = ( Ptr[Span] &0x00FF ) | ( UV_Src[ Off ]<<8 );
            U += dU; V += dV;
            Span++;
         }
         // U = U2; V = V2;
      }

      if ( Left_Over>0 )
      {
         INT dU, dV, U2, V2;
         // U2 = (INT)( uiz*z ); V2 = (INT)( viz*z );
         U2 = (INT)( uiz*zo ); V2 = (INT)( viz*zo );
         dU = (U2-U)>>2; dV = (V2-V)>>2;
         while( --Left_Over>=0 )
         {
            INT Off;
            Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
            Ptr[0] = ( Ptr[0] &0x00FF ) | ( UV_Src[ Off ]<<8 );
            Ptr++;
            U += dU; V += dV;
         }
      }
Skip:
      Dst += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}

/******************************************************************/
/******************************************************************/

#if 0 //#ifdef UNIX
EXTERN void _Draw_16b_256( )
{
   USHORT *Dst;
   INT y, dy;

   y = Scan_Start;
   Dst = Light_Buf + (y+1)*LIGHT_BUF_W - 1;
   dy = Scan_H;
   while( dy>0 )
   {
      USHORT *Ptr;
      INT Len, Tmp;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      while( Len<0 ) Ptr[Len++] = Flat_Color;
      Dst += LIGHT_BUF_W;
      y++;
      dy--;
   }
}
#endif

/******************************************************************/
/******************************************************************/
/*                            UNIX section                        */
/******************************************************************/
/******************************************************************/

#ifdef UNIX
EXTERN void _Draw_UV_Offset( )
{
   INT y, dy;
   USHORT *Dst;

      /* Now, draw! Here lies the Mythical Inner-Loop :) */

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      INT U, V;
      USHORT *Ptr;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];

      U = Scan_U[y];
      V = Scan_V[y];
      while( Len<0 )
      {
         Ptr[Len] = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         U -= dyU;
         V -= dyV;
         Len++;
      }
      Dst += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}
#endif

/******************************************************************/

/******************************************************************/
/*          1/z-GOURAUD drawing in screen space                    */
/******************************************************************/

#ifdef UNIX

EXTERN void _Draw_zBuf( )
{
   INT y, dy;
   SHORT *Dst;

   y = Scan_Start;
   Dst = ZBuffer + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      UINT S;
      SHORT *Ptr;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len-Scan_Pt2[y];

      S = Scan_S[y];
      while( Len<0 )
      {
         Ptr[Len++] = (SHORT)( S>>16 );
         S -= dyS;
      }
      Dst += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}

EXTERN void _Draw_Flat_zBuf( )
{
   USHORT *Dst;
   INT y, dy;

   y = Scan_Start;
   Dst = (USHORT*)ZBuffer;
   Dst += (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )
   {
      INT Len = Scan_Pt1[y];
      USHORT *Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      while( Len<0 )
      {
         Ptr[Len++] = (USHORT)Flat_Color;
      }
      Dst += _RCst_.Pix_Width;
      y++;
      dy--;
   }
}
#endif

/******************************************************************/

#ifdef USE_OLD_SHADOWZ

/******************************************************************/
/*          1/z-GOURAUD drawing in Light Space                    */
/******************************************************************/

#if 1
/* #ifdef UNIX */

EXTERN void _Draw_zGouraud( )
{
   INT y, dy;
   USHORT *Dst, *Dst2;

   y = Scan_Start;
   Dst = Light_Buf + y*256;
   Dst2 = Light_Buffer2 + y*256;
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      UINT S;
      USHORT *Ptr, *Ptr2;

      Len = Scan_Pt1[y];
      Ptr = Dst + Len;
      Ptr2 = Dst2 + Len;
      Len = Scan_Pt2[y] - Len;

      S = Scan_S[y];
      while( --Len>=0 )
      {
         USHORT Zo;
         Zo = (S>>16);
         Ptr2[Len] = ( Zo+Ptr[Len] )/2;
         Ptr[Len] = Zo;
         S -= dyS;
      }
      Dst += 256;
      Dst2 += 256;
      ++y;
      dy--;
   }
}

EXTERN void _Draw_16b_zShadow_256( )
{
   USHORT *Dst, *Dst2;
   INT y, dy;

   y = Scan_Start;
   Dst = Light_Buf + y*256;
   Dst2 = Light_Buffer2 + y*256;
   dy = Scan_H;
   while( dy>0 )
   {
      USHORT *Ptr2, *Ptr;
      INT Len, Tmp;

      Len = Scan_Pt1[y];
      Ptr = Dst + Len;
      Ptr2 = Dst2 + Len;
      Len = Scan_Pt2[y] - Len;
      while( --Len>=0 )
      {
         Ptr2[Len] = ( Flat_Color+Ptr[Len] )/2;
         Ptr[Len] = Flat_Color;
      }
      Dst += 256;
      Dst2 += 256;
      y++;
      dy--;
   }
}
#endif

#endif   // USE_OLD_SHADOWZ

/******************************************************************/
