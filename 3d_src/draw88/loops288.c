/***********************************************
 *   All loops in C, for UNIX...               *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"
#include "bump.h"

/******************************************************************/

#ifdef UNIX 

EXTERN void _Draw_Flat_88( )
{
   USHORT *Dst;
   INT y, dy;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )
   {
      USHORT *Ptr;
      INT Len, Tmp;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      while( Len<0 )
      {
         Ptr[Len] = (Ptr[Len]&0x00FF) | ((Flat_Color<<8)&0xFF00);
         Len++;
      }
      Dst += _RCst_.Pix_Width;
      y++;
      dy--;
   }
}
EXTERN void _Draw_Flat2_88( )
{
   USHORT *Dst;
   INT y, dy;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )
   {
      USHORT *Ptr;
      INT Len, Tmp;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      while( Len<0 )
      {
         Ptr[Len] = (Ptr[Len]&0xFF00) | (Flat_Color&0xFF);
         Len++;
      }
      Dst += _RCst_.Pix_Width;
      y++;
      dy--;
   }
}

#endif      // UNIX

/******************************************************************/

#ifdef UNIX

EXTERN void _Draw_Gouraud_88( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT*)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
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
      Dst += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}

EXTERN void _Draw_Gouraud_Ramp_88( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT*)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
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
      Dst += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}
#endif


/******************************************************************/

#ifdef UNIX
EXTERN void _Draw_UV2_88( )
{
   INT y, dy;
   USHORT *Dst;

      // Now, draw! Here lies the Mythical Inner-Loop :)

   y = Scan_Start;
   Dst = ((USHORT*)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
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
         INT Off;
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         Ptr[Len] = (Ptr[Len]&0xFF00) | (UV_Src[Off]&0xFF);
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

#ifdef UNIX
EXTERN void _Draw_UV_88( )
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
         INT Off;
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         Ptr[Len] = (Ptr[Len]&0xFF) | ( UV_Src[ Off ]<<8 );
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

#if 0    // not useful
EXTERN void _Draw_UVc( )
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
      FLT uiz, viz, iz;
      INT _U_, _V_;
      USHORT *Ptr;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      if ( Len>=0 ) goto Skip;

      uiz = Scan_UiZ[y];
      viz = Scan_ViZ[y];
      iz  = Scan_iZ[y];
      while( Len<0 )
      {
         INT Off; INT U, V; FLT z;
         z = 1.0/iz;
         U = (INT)( uiz*z ); V = (INT)( viz*z );
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         Ptr[Len] = ( Ptr[Len] &0x00FF ) | ( UV_Src[ Off ]<<8 );
         uiz -= DyU;
         viz -= DyV;
         iz  -= DyiZ;
         Len++;
      }
Skip:
      Dst += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}
#endif

/******************************************************************/

#if 0    // not useful
EXTERN void _Draw_UVc_1( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      FLT uiz, viz, z, iz;
      USHORT *Ptr;
      INT U, V, dU, dV;

      iz  = Scan_iZ[y];
      z = 1.0/iz;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt2[y];
      if ( Len>=0 ) goto Skip;

      uiz = Scan_UiZ[y]; U = (INT)( uiz*z );
      viz = Scan_ViZ[y]; V = (INT)( viz*z );

      uiz += DyU*(FLT)Len; viz += DyV*(FLT)Len;
      iz += DyiZ*(FLT)Len;
      z = 1.0/iz;

      iz = -1.0/(FLT)Len;
      dU = (INT)( ( uiz*z - (FLT)U )*iz );
      dV = (INT)( ( viz*z - (FLT)V )*iz );

      while( Len<0 )
      {
         INT Off; 
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         Ptr[Len] = ( Ptr[Len] &0x00FF ) | ( UV_Src[ Off ]<<8 );
         U += dU; V += dV;
         Len++;
      }

Skip:
      Dst += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}
#endif

/******************************************************************/

#ifdef UNIX 

EXTERN void _Draw_UVc_32_88( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   DyU *= 32.0; DyV *= 32.0; DyiZ *= 32.0;
   while( dy>0 )   
   {
      INT Len, Left_Over;
      FLT uiz, viz, iz, z;
      INT U, V;
      USHORT *Ptr;

      iz  = Scan_iZ[y]; z = 1.0/iz;

      Len = Scan_Pt2[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt1[y];
      if ( Len<=0 ) goto Skip;
      Left_Over = (Len)&0x1F;
      Len = Len>>5;

      uiz = Scan_UiZ[y]; U = (INT)( uiz*z );
      viz = Scan_ViZ[y]; V = (INT)( viz*z );

      if ( Len==0 )
      {
         uiz -= DyU*_Div_Tab_32_[Left_Over];
         viz -= DyV*_Div_Tab_32_[Left_Over];
         iz  -= DyiZ*_Div_Tab_32_[Left_Over];
         z = 1.0/iz;
         goto Finish_32;
      }
      iz -= DyiZ; uiz -= DyU; viz -= DyV;
      z = 1.0/iz;

      while( --Len>=0 )
      {
         INT Span;
         INT U2, V2, dU, dV;

         U2 = (INT)( uiz*z ); V2 = (INT)( viz*z );
         dU = (U2-U)>>5; dV = (V2-V)>>5;
         if ( Len )
         {
            uiz -= DyU; viz -= DyV; iz -= DyiZ;
            z = 1.0/iz;
         }
         else if ( Left_Over )
         {
            uiz -= DyU*_Div_Tab_16_[Left_Over];
            viz -= DyV*_Div_Tab_16_[Left_Over];
            iz  -= DyiZ*_Div_Tab_16_[Left_Over];
            z = 1.0/iz;
         }
         Span = -32; Ptr -= Span;
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

Finish_32:
      if ( Left_Over>0 )
      {
         INT dU, dV;
         FLT U2, V2;
         U2 = uiz*z; V2 = viz*z;
         dU = (INT)( (U2-(FLT)U)*_Div_Tab_1_[Left_Over] );
         dV = (INT)( (V2-(FLT)V)*_Div_Tab_1_[Left_Over] );
         // dV = (V2-V)>>4;
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

EXTERN void _Draw_UVc_16_88( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   DyU *= 16.0; DyV *= 16.0; DyiZ *= 16.0;
   while( dy>0 )   
   {
      INT Len, Left_Over;
      FLT uiz, viz, iz, z;
      INT U, V;
      USHORT *Ptr;

      iz  = Scan_iZ[y]; z = 1.0/iz;

      Len = Scan_Pt2[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt1[y];
      if ( Len<=0 ) goto Skip;
      Left_Over = (Len)&0x0F;
      Len = Len>>4;

      uiz = Scan_UiZ[y]; U = (INT)( uiz*z );
      viz = Scan_ViZ[y]; V = (INT)( viz*z );

      if ( Len==0 )
      {
         uiz -= DyU*_Div_Tab_16_[Left_Over];
         viz -= DyV*_Div_Tab_16_[Left_Over];
         iz  -= DyiZ*_Div_Tab_16_[Left_Over];
         z = 1.0/iz;
         goto Finish_16;
      }
      iz -= DyiZ; uiz -= DyU; viz -= DyV;
      z = 1.0/iz;

      while( --Len>=0 )
      {
         INT Span;
         INT U2, V2, dU, dV;

         U2 = (INT)( uiz*z ); V2 = (INT)( viz*z );
         dU = (U2-U)>>4; dV = (V2-V)>>4;
         if ( Len )
         {
            uiz -= DyU; viz -= DyV; iz -= DyiZ;
            z = 1.0/iz;
         }
         else if ( Left_Over )
         {
            uiz -= DyU*_Div_Tab_16_[Left_Over];
            viz -= DyV*_Div_Tab_16_[Left_Over];
            iz  -= DyiZ*_Div_Tab_16_[Left_Over];
            z = 1.0/iz;
         }
         Span = -16; Ptr -= Span;
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

Finish_16:
      if ( Left_Over>0 )
      {
         INT dU, dV;
         FLT U2, V2;
         U2 = uiz*z; V2 = viz*z;
         dU = (INT)( (U2-(FLT)U)*_Div_Tab_1_[Left_Over] );
         dV = (INT)( (V2-(FLT)V)*_Div_Tab_1_[Left_Over] );
         // dV = (V2-V)>>4;
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

EXTERN void _Draw_UVc_8_88( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   DyU *= 8.0; DyV *= 8.0; DyiZ *= 8.0;
   while( dy>0 )   
   {
      INT Len, Left_Over;
      FLT uiz, viz, iz, z;
      INT U, V;
      USHORT *Ptr;

      iz  = Scan_iZ[y]; z = 1.0/iz;

      Len = Scan_Pt2[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt1[y];
      if ( Len<=0 ) goto Skip;
      Left_Over = (Len)&0x07;
      Len = Len>>3;

      uiz = Scan_UiZ[y]; U = (INT)( uiz*z );
      viz = Scan_ViZ[y]; V = (INT)( viz*z );

      iz -= DyiZ; uiz -= DyU; viz -= DyV;
      z = 1.0/iz;

      while( --Len>=0 )
      {
         INT Span;
         INT U2, V2, dU, dV;

         U2 = (INT)( uiz*z ); V2 = (INT)( viz*z );
         dU = (U2-U)>>3; dV = (V2-V)>>3;

         uiz -= DyU; viz -= DyV; iz -= DyiZ;
         z = 1.0/iz;
         Span = -8; Ptr -= Span;
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
         U2 = (INT)( uiz*z ); V2 = (INT)( viz*z );
         dU = (U2-U)>>3; dV = (V2-V)>>3;
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

EXTERN void _Draw_UVc_4_88( )
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
      INT U, V;
      USHORT *Ptr;

      iz  = Scan_iZ[y]; z = 1.0/iz;

      Len = Scan_Pt2[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt1[y];
      if ( Len<=0 ) goto Skip;
      Left_Over = (Len)&0x03;
      Len = Len>>2;

      uiz = Scan_UiZ[y]; U = (INT)( uiz*z );
      viz = Scan_ViZ[y]; V = (INT)( viz*z );

      iz -= DyiZ; uiz -= DyU; viz -= DyV;
      z = 1.0/iz;

      while( --Len>=0 )
      {
         INT Span;
         INT U2, V2, dU, dV;

         U2 = (INT)( uiz*z ); V2 = (INT)( viz*z );
         dU = (U2-U)>>2; dV = (V2-V)>>2;

         uiz -= DyU; viz -= DyV; iz -= DyiZ;
         z = 1.0/iz;
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
         U2 = (INT)( uiz*z ); V2 = (INT)( viz*z );
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

EXTERN void _Draw_UVc_2_88( )
{
   INT y, dy;
   USHORT *Dst;

   y = Scan_Start;
   Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   DyU *= 2.0; DyV *= 2.0; DyiZ *= 2.0;
   while( dy>0 )   
   {
      INT Len, Left_Over;
      FLT uiz, viz, iz, z;
      INT U, V;
      USHORT *Ptr;

      iz  = Scan_iZ[y]; z = 1.0/iz;

      Len = Scan_Pt2[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt1[y];
      if ( Len<=0 ) goto Skip;
      Left_Over = (Len)&0x01;
      Len = Len>>1;

      uiz = Scan_UiZ[y]; U = (INT)( uiz*z );
      viz = Scan_ViZ[y]; V = (INT)( viz*z );

      iz -= DyiZ; uiz -= DyU; viz -= DyV;
      z = 1.0/iz;

      while( --Len>=0 )
      {
         INT Span;
         INT U2, V2, dU, dV;

         U2 = (INT)( uiz*z ); V2 = (INT)( viz*z );
         dU = (U2-U)>>1; dV = (V2-V)>>1;

         uiz -= DyU; viz -= DyV; iz -= DyiZ;
         z = 1.0/iz;
         Span = -1; Ptr -= Span;
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
         U2 = (INT)( uiz*z ); V2 = (INT)( viz*z );
         dU = (U2-U)>>1; dV = (V2-V)>>1;
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

#endif   // UNIX

/******************************************************************/
/******************************************************************/
