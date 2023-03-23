/***********************************************
 *   All loops in C, for UNIX...               *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"
#include "bump.h"

/******************************************************************/

#ifdef UNIX 

EXTERN void _Draw_Flat_16( )
{
   USHORT *Dst;
   INT y, dy;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*(_RCst_.Pix_BpS/2);
   dy = Scan_H;
   while( dy>0 )
   {
      USHORT *Ptr;
      INT Len;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      while( Len<0 )
      {
         Ptr[Len] = Flat_Color&0xffff;
         Len++;
      }
      Dst = (USHORT*)((BYTE*)Dst+_RCst_.Pix_BpS);
      y++;
      dy--;
   }
}

EXTERN void _Draw_Flat2_16( )
{
   USHORT *Dst;
   INT y, dy;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*(_RCst_.Pix_BpS/2);
   dy = Scan_H;
   while( dy>0 )
   {
      USHORT *Ptr;
      INT Len;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      while( Len<0 )
      {
         Ptr[Len] = Flat_Color;
         Len++;
      }
      Dst = (USHORT*)((BYTE*)Dst+_RCst_.Pix_BpS);
      y++;
      dy--;
   }
}

#endif      // UNIX

/******************************************************************/

#ifdef UNIX

EXTERN void _Draw_Gouraud_16( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT*)_RCst_.Base_Ptr) + (y+1)*(_RCst_.Pix_BpS/2);
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      INT S;
      USHORT *Ptr;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];

      S = Scan_S[y];
      while( Len<0 )
      {
         Ptr[Len] = (Ptr[Len]&0xFF00) | (PIXEL)( S>>16 );
         S -= dyS;
         Len++;
      }
      Dst += (_RCst_.Pix_BpS/2);
      ++y;
      dy--;
   }
}

EXTERN void _Draw_Gouraud_Ramp_16( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT*)_RCst_.Base_Ptr) + (y+1)*(_RCst_.Pix_BpS/2);
   dy = Scan_H;

   while( dy>0 )   
   {
      INT Len;
      INT S;
      USHORT *Ptr;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      S = Scan_S[y]>>16;
      while( Len<0 )
      {
         Ptr[Len] = (Ptr[Len]&0xFF00)|(PIXEL)( S );
         Len++;
      }
      Dst = (USHORT*)((BYTE*)Dst+_RCst_.Pix_BpS);
      ++y;
      dy--;
   }
}
#endif


/******************************************************************/
/******************************************************************/

#ifdef UNIX
EXTERN void _Draw_UV_16b( )
{
   INT y, dy;
   USHORT *Dst;

      /* Now, draw! Here lies the Mythical Inner-Loop :) */

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*(_RCst_.Pix_BpS/2);
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
         USHORT Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         Ptr[Len++] = ((USHORT*)UV_Src)[ Off ]; // <= *NO* Mip_Mask HERE!!
         U -= dyU; V -= dyV;
      }
      Dst = (USHORT*)((BYTE*)Dst+_RCst_.Pix_BpS);
      ++y;
      dy--;
   }
}
#endif

/******************************************************************/
/******************************************************************/

#ifdef UNIX
EXTERN void _Draw_UVc_16( )
{
   INT y, dy;
   USHORT *Dst;

      /* Now, draw! Here lies the Mythical Inner-Loop :) */

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*(_RCst_.Pix_BpS/2);
   dy = Scan_H;
   while( dy>0 )   
   {
      USHORT *Ptr;
      INT Len;
      FLT uiz, viz, iz, z;
      INT dU, dV;
      INT U, V;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      if ( Len>=0 ) goto Skip;

      uiz = Scan_UiZ[y];
      viz = Scan_ViZ[y];
      iz  = Scan_iZ[y];
      z = 1.0f/iz;
      U = (INT)floor(uiz*z); V = (INT)floor(viz*z);
      iz  += Len*DyiZ;
      uiz += Len*DyU;
      viz += Len*DyV;
      z = 1.0f/iz;
      dU = (INT)floor(uiz*z); dV = (INT)floor(viz*z);
      z = 1.0f/Len;
      dU = (U-dU)*z; dV = (V-dV)*z;
      while( Len<0 )
      {
         USHORT Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         Ptr[Len++] = ((USHORT*)UV_Src)[ Off&Mip_Mask ];
         U += dU; V += dV;
      }
Skip:
      Dst = (USHORT*)((BYTE*)Dst+_RCst_.Pix_BpS);
      y++;
      dy--;
   }
}
#endif

/******************************************************************/
/******************************************************************/

#ifdef UNIX 
#define _FUNC_ _Draw_UVc_64_16
#define LOG2_PIX_PER_SPANS 6
#define DIV_TAB _Div_Tab_64_
#include "./loops216.h"
#undef _FUNC_
#undef LOG2_PIX_PER_SPANS
#undef DIV_TAB
#undef PIX_PER_SPANS

#define _FUNC_ _Draw_UVc_32_16
#define LOG2_PIX_PER_SPANS 5
#define DIV_TAB _Div_Tab_32_
#include "./loops216.h"
#undef _FUNC_
#undef LOG2_PIX_PER_SPANS
#undef DIV_TAB
#undef PIX_PER_SPANS

#define _FUNC_ _Draw_UVc_16_16
#define LOG2_PIX_PER_SPANS 4
#define DIV_TAB _Div_Tab_16_
#include "./loops216.h"
#undef _FUNC_
#undef LOG2_PIX_PER_SPANS
#undef DIV_TAB
#undef PIX_PER_SPANS

#define _FUNC_ _Draw_UVc_8_16
#define LOG2_PIX_PER_SPANS 3
#define DIV_TAB _Div_Tab_8_
#include "./loops216.h"
#undef _FUNC_
#undef LOG2_PIX_PER_SPANS
#undef DIV_TAB
#undef PIX_PER_SPANS

#define _FUNC_ _Draw_UVc_4_16
#define LOG2_PIX_PER_SPANS 2
#define DIV_TAB _Div_Tab_4_
#include "./loops216.h"
#undef _FUNC_
#undef LOG2_PIX_PER_SPANS
#undef DIV_TAB
#undef PIX_PER_SPANS

#endif   // UNIX

/******************************************************************/
/******************************************************************/
