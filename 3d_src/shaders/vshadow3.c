/***********************************************
 *              vshadow3.c                     *
 *        Volumic hard multi shadows           *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/
/******************************************************************/
/*                 V-SHADOWS III (la mission)                     */
/******************************************************************/
/******************************************************************/

   // ZBuffer_Front: Delta-z from Scene/Shadow poly
   // ZBuffer_Back: 

static void Draw_zBuf_CPos_Shadow( )
{
   INT y, dy;
   SHORT *ZBuf;
   SHORT *ZBuf_Back;
   PIXEL *Dst;

   y = Scan_Start;
   Dst = ((PIXEL *)ZBuffer_Shade) + (y+1)*_RCst_.Pix_Width;
   ZBuf_Back =  ZBuffer_Back + (y+1)*_RCst_.Pix_Width;
   ZBuf = ZBuffer + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )
   {
      INT Len;
      UINT S;
      SHORT *ZBuf2;
      SHORT *ZBuf_Back2;
      PIXEL *Dst2;

      Len = Scan_Pt1[y];
      Dst2 = Dst - Len;
      ZBuf2 = ZBuf - Len;
      ZBuf_Back2 = ZBuf_Back - Len;
      Len = Len - Scan_Pt2[y];

      S = Scan_S[y];
      while( Len<0 )
      {
         SHORT Tmp;
         Tmp = (SHORT)(S>>16) - ZBuf2[Len];
         if ( Tmp>0x02 ) Dst2[Len] += 0x01;
         // ZBuf_Back2[Len] += Tmp;
         S -= dyS;
         Len++;
      }
      Dst += _RCst_.Pix_Width;
      ZBuf += _RCst_.Pix_Width;
      ZBuf_Back += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}

static void Draw_zBuf_CNeg_Shadow( )
{
   INT y, dy;
   SHORT *ZBuf_Back;
   SHORT *ZBuf;
   PIXEL *Dst;

   y = Scan_Start;
   Dst =  ((PIXEL *)ZBuffer_Shade) + (y+1)*_RCst_.Pix_Width;
   ZBuf = ZBuffer + (y+1)*_RCst_.Pix_Width;
   ZBuf_Back = ZBuffer_Back + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )
   {
      INT Len;
      UINT S;
      SHORT *ZBuf2;
      SHORT *ZBuf_Back2;
      PIXEL *Dst2;

      Len = Scan_Pt1[y];
      Dst2 = Dst - Len;
      ZBuf2 = ZBuf - Len;
      ZBuf_Back2 = ZBuf_Back - Len;
      Len = Len - Scan_Pt2[y];

      S = Scan_S[y];
      while( Len<0 )
      {
         SHORT Tmp;
         Tmp = (SHORT)(S>>16) - ZBuf2[Len];
         if ( Tmp>0x02 ) Dst2[Len] += 0x10;
         // ZBuf_Back2[Len] -= Tmp;
         S -= dyS;
         Len++;
      }
      Dst += _RCst_.Pix_Width;
      ZBuf += _RCst_.Pix_Width;
      ZBuf_Back += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}

/******************************************************************/
/******************************************************************/

static void Shader_Transform_And_Draw_VShadow3( MATRIX2 M )
{
   INT i;

   Select_Sil_Edges3( M );
   Build_Shadow_Volume2( M );

//   Counting_Sort( Poly_Keys, Nb_Poly_Sorted );
//   fprintf( stderr, "Nb_Polys_Sorted: %d\r", Nb_Poly_Sorted );

   for( i=0; i<Nb_Poly_Sorted; ++i )
   {
      INT Start, End, Cap;
      MSH_EDGE *Edge;

      Edge = Cur_Msh->Edges + (Poly_Keys[i]>>16);
      Start = Edge->Start;
      End = Edge->End;

               // Cap vtx #1
      Out_Vtx[0] = P_Edges[0] = &P_Vertex[Start];
      Vtx_Flags[0] = Vertex_State[ Start ];  //  & ~0x03;

      Cap = *((INT *)&P_Vertex[Start].UV[1] );
      Out_Vtx[3] = P_Edges[3] = &Cap_Vtx[Cap];
      Vtx_Flags[3] = Cap_State[Cap];


               // Cap vtx #2
      Out_Vtx[1] = P_Edges[1] = &P_Vertex[End];
      Vtx_Flags[1] = Vertex_State[ End ]; // & ~0x03;

      Cap = *((INT *)&P_Vertex[End].UV[1] );
      Out_Vtx[2] = P_Edges[2] = &Cap_Vtx[Cap];
      Vtx_Flags[2] = Cap_State[Cap];

      Nb_Edges_To_Deal_With = Nb_Out_Edges = 4;
      if ( Shader_Emit_VShadow_Poly( ) ) continue;

      Shader_Deal_zBuf( );
      if ( Edge->State & EDGE_POS_SHADOW )
         Draw_zBuf_CPos_Shadow( );
      else Draw_zBuf_CNeg_Shadow( );

#if 0
         // debug
      if ( Edge->State & EDGE_POS_SHADOW )
         memset( AA_Table, 0x3F, 256 );
      else
         memset( AA_Table, 0x7F, 256 );
      {
         VECTOR Pi, Pf;
         Pi[0] = P_Vertex[ Edge->Start ].N[0];
         Pi[1] = P_Vertex[ Edge->Start ].N[1];
         Pi[2] = P_Vertex[ Edge->Start ].N[2];
         Pf[0] = P_Vertex[ Edge->End ].N[0];
         Pf[1] = P_Vertex[ Edge->End ].N[1];
         Pf[2] = P_Vertex[ Edge->End ].N[2];
         Emit_3D_Line( Pi, Pf );
      }
#endif
   }
}

/******************************************************************/
/******************************************************************/

EXTERN SHADER_METHODS Light_zBuf_Shader3 =
{
   NULL,          // Set parameters
   Shader_Transform_And_Draw_VShadow3,
   Split_Gouraud, NULL,
   NULL, NULL, NULL,
   NULL
};

/******************************************************************/
