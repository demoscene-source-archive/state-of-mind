/*
 * - Bilinear -   (/ray-tracing)
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

#define MAX_W  512+4

EXTERN void Do_Bilinear_4( PIXEL *Out, PIXEL *In, 
   INT W, INT H, INT Dst_BpS )
{
   INT i, j;
   USHORT V[MAX_W];
   USHORT dV[MAX_W];

   for( i=0; i<=W; ++i ) V[i] = In[i]<<8;
   for( j=H; j>0; --j )
   {
      In += i;
      for( i=0; i<=W; ++i ) dV[i] = ( (In[i]<<8)-V[i] ) / 4;
      Do_Bilin_Line_4( Out, (SHORT*)V, (SHORT*)dV, W ); Out += Dst_BpS;
      Do_Bilin_Line_4( Out, (SHORT*)V, (SHORT*)dV, W ); Out += Dst_BpS;
      Do_Bilin_Line_4( Out, (SHORT*)V, (SHORT*)dV, W ); Out += Dst_BpS;
      Do_Bilin_Line_4( Out, (SHORT*)V, (SHORT*)dV, W ); Out += Dst_BpS;
   }
}

EXTERN void Do_Bilinear_UV_Map_8( PIXEL *Out, INT *In,
   INT W, INT H, INT Dst_BpS, PIXEL *Map )
{
   INT i, j;
   USHORT UV[MAX_W*2];
   SHORT dUV[MAX_W*2];

   for( i=0; i<=2*W+1; ++i ) UV[i] = (USHORT)( In[i]&0xFFFF );
   for( j=H; j>0; --j )
   {
      In += i;
      for( i=0; i<=2*W+1; ++i )
      {
         SHORT C = (USHORT)( In[i]-UV[i] );
         dUV[i] = C>>3;
      }
      Do_Bilin_Line_UV_Map_8( Out, UV, (USHORT*)dUV, W, Map ); Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_8( Out, UV, (USHORT*)dUV, W, Map ); Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_8( Out, UV, (USHORT*)dUV, W, Map ); Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_8( Out, UV, (USHORT*)dUV, W, Map ); Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_8( Out, UV, (USHORT*)dUV, W, Map ); Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_8( Out, UV, (USHORT*)dUV, W, Map ); Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_8( Out, UV, (USHORT*)dUV, W, Map ); Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_8( Out, UV, (USHORT*)dUV, W, Map ); Out += Dst_BpS;
   }
}

/********************************************************************/

#if 0       // USELESS NOW

EXTERN void Do_Bilinear_UV_Map_32b_8( UINT *Out, INT *In,
   INT W, INT H, INT Dst_BpS, UINT *Map )
{
   INT i, j;
   USHORT UV[MAX_W*2];
   SHORT dUV[MAX_W*2];

   for( i=0; i<=2*W+1; ++i ) UV[i] = (USHORT)( In[i]&0xFFFF );
   for( j=H; j>0; --j )
   {
      In += i;
      for( i=0; i<=2*W+1; ++i )
      {
         SHORT C = (SHORT)( In[i]-UV[i] );
         dUV[i] = C>>3;
      }
      Do_Bilin_Line_UV_Map_32b_8( Out, UV, dUV, W, Map ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_32b_8( Out, UV, dUV, W, Map ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_32b_8( Out, UV, dUV, W, Map ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_32b_8( Out, UV, dUV, W, Map ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_32b_8( Out, UV, dUV, W, Map ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_32b_8( Out, UV, dUV, W, Map ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_32b_8( Out, UV, dUV, W, Map ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_Map_32b_8( Out, UV, dUV, W, Map ); (BYTE*)Out += Dst_BpS;
   }
}

EXTERN void Do_Bilinear_8( PIXEL *Out, PIXEL *In,
   INT W, INT H, INT Dst_BpS )
{
   INT i, j;
   USHORT V[MAX_W];
   SHORT dV[MAX_W];

   for( i=0; i<=W; ++i ) V[i] = In[i]<<8;
   for( j=H; j>0; --j )
   {
      In += i;
      for( i=0; i<=W; ++i ) dV[i] = ( (In[i]<<8)-V[i] )/8;
      Do_Bilin_Line_8( Out, V, dV, W ); Out += Dst_BpS;
      Do_Bilin_Line_8( Out, V, dV, W ); Out += Dst_BpS;
      Do_Bilin_Line_8( Out, V, dV, W ); Out += Dst_BpS;
      Do_Bilin_Line_8( Out, V, dV, W ); Out += Dst_BpS;
      Do_Bilin_Line_8( Out, V, dV, W ); Out += Dst_BpS;
      Do_Bilin_Line_8( Out, V, dV, W ); Out += Dst_BpS;
      Do_Bilin_Line_8( Out, V, dV, W ); Out += Dst_BpS;
      Do_Bilin_Line_8( Out, V, dV, W ); Out += Dst_BpS;
   }
}

EXTERN void Do_Bilinear_UV_8( USHORT *Out, INT *In,
   INT W, INT H, INT Dst_BpS )
{
   INT i, j;
   INT UV[MAX_W*2];
   INT dUV[MAX_W*2];

   for( i=0; i<(W+1)*2; ++i ) UV[i] = In[i];
   for( j=H; j>0; --j )
   {
      In += i;
      for( i=0; i<(W+1)*2; ++i ) dUV[i] = ( In[i]-UV[i] ) / 8;
      Do_Bilin_Line_UV_8( Out, UV, dUV, W ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_8( Out, UV, dUV, W ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_8( Out, UV, dUV, W ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_8( Out, UV, dUV, W ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_8( Out, UV, dUV, W ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_8( Out, UV, dUV, W ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_8( Out, UV, dUV, W ); (BYTE*)Out += Dst_BpS;
      Do_Bilin_Line_UV_8( Out, UV, dUV, W ); (BYTE*)Out += Dst_BpS;
   }
}


#endif

/********************************************************************/
/********************************************************************/

#ifdef UNIX

EXTERN void Do_Bilin_Line_UV_Map_8( PIXEL *Out, USHORT *UV, USHORT *dUV, 
   INT W, PIXEL *Map )
{
   USHORT Uo, U, dU, Vo, V, dV;
   U = UV[0]; UV[0] = U+dUV[0];
   V = UV[1]; UV[1] = V+dUV[1];
   UV += 2; dUV += 2;
   for( ; W>0; --W )
   {
      Uo = U; Vo = V;
      U = UV[0]; UV[0] += dUV[0]; dU = (USHORT)( (SHORT)( (U-Uo)&0xffff )>>3 );
      V = UV[1]; UV[1] += dUV[1]; dV = (USHORT)( (SHORT)( (V-Vo)&0xffff )>>3 );
      UV += 2; dUV += 2;
      Out[0] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[1] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[2] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[3] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[4] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[5] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[6] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[7] = Map[(Vo&0xff00)|(Uo>>8)]; 
      Out += 8;
   }
}

EXTERN void Do_Bilin_Line_4( PIXEL *Out, SHORT *C, SHORT *dC, INT W )
{
   USHORT Vo, V, dV;
   V = C[0]; C[0] += dC[0]; C++; dC++;
   for( ; W>0; --W )
   {
      Vo = V;
      V = C[0]; 
      dV = (USHORT)( (SHORT)( (V-Vo)&0xffff )>>2 );
      C[0] += dC[0]; C++; dC++;
      Out[0] = Vo>>8; Vo += dV;
      Out[1] = Vo>>8; Vo += dV;
      Out[2] = Vo>>8; Vo += dV;
      Out[3] = Vo>>8; Vo += dV;
      Out += 4;
   }
}

/********************************************************************/

#if 0       // USELESS NOW

EXTERN void Do_Bilin_Line_UV_8( USHORT *Out, UINT *UV, UINT *dUV, INT W )
{
   USHORT Uo, U, dU, Vo, V, dV;
   U = UV[0]; V = UV[1];
   UV[0] += dUV[0]; UV[1] += dUV[1]; 
   UV+=2; dUV+=2;
   for( ; W>0; --W )
   {
      Uo = U; Vo = V;
      U = UV[0]; UV[0] += dUV[0]; dU = (USHORT)( (SHORT)( (U-Uo)&0xffff )>>3 );
      V = UV[1]; UV[1] += dUV[1]; dV = (USHORT)( (SHORT)( (V-Vo)&0xffff )>>3 );
      UV+=2; dUV+=2;
      Out[0] = (Vo&0xff00)|(Uo>>8); Uo += dU; Vo += dV;
      Out[1] = (Vo&0xff00)|(Uo>>8); Uo += dU; Vo += dV;
      Out[2] = (Vo&0xff00)|(Uo>>8); Uo += dU; Vo += dV;
      Out[3] = (Vo&0xff00)|(Uo>>8); Uo += dU; Vo += dV;
      Out[4] = (Vo&0xff00)|(Uo>>8); Uo += dU; Vo += dV;
      Out[5] = (Vo&0xff00)|(Uo>>8); Uo += dU; Vo += dV;
      Out[6] = (Vo&0xff00)|(Uo>>8); Uo += dU; Vo += dV;
      Out[7] = (Vo&0xff00)|(Uo>>8); 
      Out += 8;
   }
}

EXTERN void Do_Bilin_Line_UV_Map_32b_8( UINT *Out, SHORT *UV, SHORT *dUV, 
   INT W, UINT *Map )
{
   USHORT Uo, U, dU, Vo, V, dV;
   U = UV[0]; UV[0] += dUV[0];
   V = UV[1]; UV[1] += dUV[1];
   UV += 2; dUV += 2;
   for( ; W>0; --W )
   {
      Uo = U; Vo = V;
      U = UV[0]; UV[0] += dUV[0]; dU = (USHORT)( (SHORT)( (U-Uo)&0xffff )>>3 );
      V = UV[1]; UV[1] += dUV[1]; dV = (USHORT)( (SHORT)( (V-Vo)&0xffff )>>3 );
      UV += 2; dUV += 2;
      Out[0] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[1] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[2] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[3] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[4] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[5] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[6] = Map[(Vo&0xff00)|(Uo>>8)]; Uo += dU; Vo += dV;
      Out[7] = Map[(Vo&0xff00)|(Uo>>8)]; 
      Out += 8;
   }
}

EXTERN void Do_Bilin_Line_8( PIXEL *Out, SHORT *C, SHORT *dC, INT W )
{
   SHORT Vo, V, dV;
   V = C[0]; C[0] += dC[0]; C++; dC++;
   for( ; W>0; --W )
   {
      Vo = V;
      V = C[0]; C[0] += dC[0]; C++; dC++; dV = (V-Vo)/8;
      Out[0] = Vo>>8; Vo += dV;
      Out[1] = Vo>>8; Vo += dV;
      Out[2] = Vo>>8; Vo += dV;
      Out[3] = Vo>>8; Vo += dV;
      Out[4] = Vo>>8; Vo += dV;
      Out[5] = Vo>>8; Vo += dV;
      Out[6] = Vo>>8; Vo += dV;
      Out[7] = Vo>>8; 
      Out += 8;
   }
}

#endif   // 0

#endif   // UNIX

/********************************************************************/

#ifdef UNIX

EXTERN void Do_Plasma_256( PIXEL *Dst, PIXEL *In,
   PIXEL U1, PIXEL V1, PIXEL U2, PIXEL V2 )
{
   INT j;

   for( j=256; j>0; --j )
   {
      INT i;
      char *Src1, *Src2;
      Dst += 256;
      Src1 = (char*)In + (V1<<8); V1++;
      Src2 = (char*)In + (V2<<8); V2++;
      for( i=-256; i<0; ++i ) Dst[i] = Src1[ U1++ ] + Src2[ U2++ ];
   }
}

#endif   // UNIX

/********************************************************************/

