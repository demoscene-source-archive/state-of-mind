/***********************************************
 *              vshadow2.c                     *
 *        Volumic hard shadows                 *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/
/******************************************************************/
/*                 V-SHADOWS II (le retour)                       */
/******************************************************************/
/******************************************************************/

static void Select_Sil_Edges( MATRIX2 M )
{
   INT i;
   FLT Z_Scale2;

      // Store all vertex position in .N[] field
   for( i=Cur_Msh->Nb_Vertex-1; i>=0; --i )
   {
      Vertex_State[i] = 0;
      A_V( P_Vertex[i].N, M, Cur_Msh->Vertex[i] );
   }

   Orig_Poly = Cur_Msh->Polys;
   for( i=Cur_Msh->Nb_Polys; i>0; --i )
   {
      PIXEL Flag;
      VECTOR Pt, N;
      FLT Dot;

      nA_V( N, M, Orig_Poly->No );
      Dot = Dot_Product( N, P_Vertex[Orig_Poly->Pt[0]].N );
      if ( Dot>0.0 ) Flag = VTX_CLIP_Z1;     // BF-Cull in bit 7
      else Flag = 0x00;

      Sub_Vector( Pt, P_Vertex[Orig_Poly->Pt[0]].N, Cur_Light->P );
      Dot = Dot_Product( N, Pt );
      if ( Dot>0.0 ) 
      {
            /* Shadowed edge */
         Orig_Poly->Clipped = Flag|0x02;
      }
      else Orig_Poly->Clipped = Flag|0x01;
      Orig_Poly++;
   }

         // Retreive silhouette edges

   Nb_Poly_Sorted = 0;
   Z_Scale2 = 32767.0f/( _RCst_.Clips[6]-_RCst_.Clips[4] );
   for( i=Cur_Msh->Nb_Edges-1; i>=0; --i )
   {
      INT P1, P2, T;
      FLT Z;

      P1 = Cur_Msh->Edges[i].Poly1;
      P2 = Cur_Msh->Edges[i].Poly2;
      T = Cur_Msh->Polys[P1].Clipped | Cur_Msh->Polys[P2].Clipped;
      if ( (T&0x03)!=0x03 ) continue;
      Vertex_State[ Cur_Msh->Edges[i].Start ] |= T;
      Z = P_Vertex[ Cur_Msh->Edges[i].Start ].N[2];
      Vertex_State[ Cur_Msh->Edges[i].End ] |= T;
      Z += P_Vertex[ Cur_Msh->Edges[i].End ].N[2];
      Z /= 4.0;
      Z = ( _RCst_.Clips[6]-Z ) * Z_Scale2;
      Poly_Keys[ Nb_Poly_Sorted++ ] = (i<<16) | (USHORT)(Z);
   }
}

/******************************************************************/

static void Draw_zBuf_XOR_Shadow( )
{
   INT y, dy;
   SHORT *ZBuf;
   PIXEL *Dst;

   y = Scan_Start;
   Dst = ZBuffer_Shade + (y+1)*_RCst_.Pix_Width;
   ZBuf = ZBuffer + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )
   {
      INT Len;
      UINT S;
      SHORT *ZBuf2;
      PIXEL *Dst2;

      Len = Scan_Pt1[y];
      Dst2 = Dst - Len;
      ZBuf2 = ZBuf - Len;
      Len = Len - Scan_Pt2[y];

      S = Scan_S[y];
      while( Len<0 )
      {
         SHORT Tmp;
         Tmp = (SHORT)(S>>16) - ZBuf2[Len];
         if ( Tmp>0x10 ) Dst2[Len] ^= 0xFF;
         S -= dyS;
         Len++;
      }
      Dst += _RCst_.Pix_Width;
      ZBuf += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}

/******************************************************************/

static void Shader_Transform_And_Draw_VShadow2( MATRIX2 M )
{
   INT i;

   Select_Sil_Edges( M );
   // Counting_Sort( Poly_Keys, Nb_Poly_Sorted );

   Build_Shadow_Volume( M );

         /* Retreive front silhouette edges */

   for( i=0; i<Nb_Poly_Sorted; ++i )
   {
      INT Start, End, Cap, I;

      I = Poly_Keys[i]>>16;
      Start = Cur_Msh->Edges[I].Start;
      End = Cur_Msh->Edges[I].End;

      Out_Vtx[0] = P_Edges[0] = &P_Vertex[Start];
      Vtx_Flags[0] = Vertex_State[ Start ] & ~(VTX_CLIP_Z1|0x03);

      Out_Vtx[1] = P_Edges[1] = &P_Vertex[End];
      Vtx_Flags[1] = Vertex_State[ End ] & ~(VTX_CLIP_Z1|0x03);

               /* Cap vtx #1 */
      Cap = *((INT *)&P_Vertex[Start].UV[1] );
      Out_Vtx[3] = P_Edges[3] = &Cap_Vtx[Cap];
      Vtx_Flags[3] = Cap_State[Cap];

               /* Cap vtx #2 */
      Cap = *((INT *)&P_Vertex[End].UV[1] );
      Out_Vtx[2] = P_Edges[2] = &Cap_Vtx[Cap];
      Vtx_Flags[2] = Cap_State[Cap];

      Nb_Edges_To_Deal_With = Nb_Out_Edges = 4;
      if ( Shader_Emit_VShadow_Poly( ) ) continue;

      // Shader_Deal_zBuf_Sil( );
      // Draw_zBuf_XOR_Shadow( );
      if ( Shader_Deal_zBuf( ) != RENDER_FLAT_ZBUF )
         Draw_zBuf_XOR_Shadow( );
   }
}

/******************************************************************/
/******************************************************************/

EXTERN SHADER_METHODS Light_zBuf_Shader2 =
{
   NULL,                // Set parameters
   Shader_Transform_And_Draw_VShadow2,
   Split_Gouraud, NULL, // Split is only concerned with x,y, and Inv_Z...
   NULL, NULL, NULL,
   NULL
};

/******************************************************************/
/******************************************************************/
