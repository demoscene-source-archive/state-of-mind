/***********************************************
 *              vshadow4.c                     *
 *        Volumic soft multi shadows           *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/
/******************************************************************/
/*                 V-SHADOWS IV (last part)                       */
/******************************************************************/
/******************************************************************/

EXTERN void Select_Sil_Edges3( MATRIX2 M )
{
   INT i;
   MSH_EDGE *Edge;
   FLT Z_Scale2;

      // Store all vertex position in .N[] field

   memset( Vertex_State, 0, Cur_Msh->Nb_Vertex );
   Edge = Cur_Msh->Edges;
   for( i=Cur_Msh->Nb_Edges; i>0; --i, Edge++ )
      Edge->State = 0x00;
   for( i=0; i<Cur_Msh->Nb_Polys; ++i )
      Cur_Msh->Polys[i].Clipped = 0x00;

   Nb_Poly_Sorted = 0;
   Z_Scale2 = 32767.0f/( _RCst_.Clips[6]-_RCst_.Clips[4] );
   Edge = Cur_Msh->Edges;
   for( i=0; i<Cur_Msh->Nb_Edges; ++i, Edge++ )
   {
      INT P1, P2, j;
      FLT Z, Dot;
      VECTOR V1, No;  // <= CP1 ^ CP2

      if ( Edge->Type & EDGE_TYPE_FLAT ) continue;
      if ( !(Edge->Type & EDGE_TYPE_SHARP) ) continue;

      P1 = Edge->Poly1;
      if ( Cur_Msh->Polys[P1].Clipped == 0x00 )
      {
         nA_V( Poly_To_Sort[P1].N, M, Cur_Msh->Polys[P1].No );
         Cur_Msh->Polys[P1].Clipped = 0x01;  // i.e: Normal is transformed
      }

      j = Edge->Start;
      if ( Vertex_State[j] == 0x00 )
      {
         A_V( P_Vertex[j].N, M, Cur_Msh->Vertex[j] );
         Vertex_State[j] = 0x08;  // i.e.: Vtx transformed
      }

      Dot  = Dot_Product( Poly_To_Sort[P1].N, P_Vertex[j].N );
      if ( Dot>0.0 ) Edge->State |= EDGE_P1_BACKCULL;
      Dot -= Dot_Product( Poly_To_Sort[P1].N, Cur_Light->P );
      if ( Dot>0.0 ) Edge->State |= EDGE_P1_SHADOWED;

      P2 = Edge->Poly2;
      if ( P2!=0xFFFF)
      {
         if ( Cur_Msh->Polys[P2].Clipped == 0x00 )
         {
            nA_V( Poly_To_Sort[P2].N, M, Cur_Msh->Polys[P2].No );
            Cur_Msh->Polys[P2].Clipped = 0x01;
         }

         Dot = Dot_Product( Poly_To_Sort[P2].N, P_Vertex[j].N );
         if ( Dot>0.0 ) Edge->State |= EDGE_P2_BACKCULL;
         Dot -= Dot_Product( Poly_To_Sort[P2].N, Cur_Light->P );
         if ( Dot>0.0 ) Edge->State |= EDGE_P2_SHADOWED;
      }
      else
      {
         if ( Edge->State & EDGE_P1_SHADOWED ) continue;
         Edge->State |= EDGE_P2_SHADOWED;
         goto Ok;
      }

            // test illumination of edge

      j = Edge->State & ( EDGE_P2_SHADOWED | EDGE_P1_SHADOWED );
      if ( (j!=EDGE_P2_SHADOWED)&&(j!=EDGE_P1_SHADOWED) ) continue;


            // Ok, potentially active edge. Finish.
Ok:
      if ( Vertex_State[Edge->End] == 0x00 )
      {
         A_V( P_Vertex[Edge->End].N, M, Cur_Msh->Vertex[Edge->End] );
         Vertex_State[Edge->End] = 0x08;  // i.e.: Vtx transformed
      }
  
#if 1
               // Easy cases
      j = Edge->State & ( EDGE_P2_BACKCULL | EDGE_P1_BACKCULL );
      if ( j==( EDGE_P2_BACKCULL | EDGE_P1_BACKCULL ) ) goto Skip;
      else if (j==0x00)
      {
         Edge->State |= EDGE_POS_SHADOW;
         goto Skip;
      }
      
#endif

Hard_Pos_Neg:
               // Hard cases
#if 1
      if ( P2!=0xFFFF )
      {
         Cross_Product( V1, P_Vertex[Edge->Start].N, P_Vertex[Edge->End].N );
         No[0] = Poly_To_Sort[P1].N[0];
         No[1] = Poly_To_Sort[P1].N[1];
         No[2] = Poly_To_Sort[P1].N[2];

         No[0] += Poly_To_Sort[P2].N[0];
         No[1] += Poly_To_Sort[P2].N[1];
         No[2] += Poly_To_Sort[P2].N[2];
         Dot = Dot_Product( V1, No );
         Dot *= Dot_Product( V1, Cur_Light->P );
         if ( Dot<0.0 ) Edge->State |= EDGE_POS_SHADOW;
      }
      // else Edge->State |= EDGE_POS_SHADOW;
#else
      Edge->State |= EDGE_POS_SHADOW;
#endif

Skip:
         // store silhouette edges

      Vertex_State[ Edge->Start ] |= 0x03;
      Z = P_Vertex[ Edge->Start ].N[2];
      Vertex_State[ Edge->End ] |= 0x03;
      Z += P_Vertex[ Edge->End ].N[2];
      Z /= 2.0;
      Z = ( _RCst_.Clips[6]-Z ) * Z_Scale2;
      Poly_Keys[ Nb_Poly_Sorted++ ] = (i<<16) | (USHORT)(Z);
   }
}

/******************************************************************/
/******************************************************************/

EXTERN void Build_Shadow_Volume2( MATRIX2 M )
{
   INT i, Nb_Sil_Vtx;

   Nb_Sil_Vtx = 0;
   for( i=Cur_Msh->Nb_Vertex-1; i>=0; --i )
   {
      VECTOR Pt;
      FLT Inv_Z;

      if ( (Vertex_State[i]&0x03)!=0x03 ) continue;

      Vertex_State[i] = 0x00; // clear clip flags

               // Project

      // A_V( Pt, M, Cur_Msh->Vertex[i] );
      Pt[0] = P_Vertex[i].N[0];
      Pt[1] = P_Vertex[i].N[1];
      Pt[2] = P_Vertex[i].N[2];
      P_Vertex[i].Inv_Z = Inv_Z = 1.0f/Pt[2];

      Inv_Z *= _RCst_._Lx_;

      P_Vertex[i].xp = _RCst_._Cx_ + Pt[0]*Inv_Z;
      if ( P_Vertex[i].xp<_RCst_.Clips[0] ) Vertex_State[i] |= VTX_CLIP_Xo;
      if ( P_Vertex[i].xp>_RCst_.Clips[1] ) Vertex_State[i] |= VTX_CLIP_X1;

      P_Vertex[i].yp = _RCst_._Cy_ - Pt[1]*Inv_Z;
      if ( P_Vertex[i].yp<_RCst_.Clips[2] ) Vertex_State[i] |= VTX_CLIP_Yo;
      if ( P_Vertex[i].yp>_RCst_.Clips[3] ) Vertex_State[i] |= VTX_CLIP_Y1;
      if ( Pt[2]<=_RCst_.Clips[4] ) Vertex_State[i] |= VTX_CLIP_Zo;

      *((INT*)&P_Vertex[i].UV[1] ) = Nb_Sil_Vtx;  // Store obj's vertex in UV[1] field, as an int

               // Cap vtx

      Sub_Vector_Eq( Pt, Cur_Light->P );

      Inv_Z = 1.0f + Cur_Light->Z_Max / (FLT)sqrt( (double)Norm_Squared( Pt ) );
      Pt[0] *= Inv_Z; Pt[1] *= Inv_Z; Pt[2] *= Inv_Z;
      Add_Vector_Eq( Pt, Cur_Light->P ); 

      Cap_State[Nb_Sil_Vtx] = 0x00;
      Cap_Vtx[Nb_Sil_Vtx].Inv_Z = Inv_Z = 1.0f/Pt[2];
      Inv_Z *= _RCst_._Lx_;

      Cap_Vtx[Nb_Sil_Vtx].xp = _RCst_._Cx_ + Pt[0]*Inv_Z;
      if ( Cap_Vtx[Nb_Sil_Vtx].xp<_RCst_.Clips[0] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_Xo;
      if ( Cap_Vtx[Nb_Sil_Vtx].xp>_RCst_.Clips[1] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_X1;

      Cap_Vtx[Nb_Sil_Vtx].yp = _RCst_._Cy_ - Pt[1]*Inv_Z;
      if ( Cap_Vtx[Nb_Sil_Vtx].yp<_RCst_.Clips[2] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_Yo;
      if ( Cap_Vtx[Nb_Sil_Vtx].yp>_RCst_.Clips[3] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_Y1;
      if ( Pt[2]<=_RCst_.Clips[4] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_Zo;

      Nb_Sil_Vtx++;
   }
}

/******************************************************************/
/******************************************************************/

   // ZBuffer_Front: Delta-z from Scene/Shadow poly
   // ZBuffer_Back: 

static void Draw_zBuf_SPos_Shadow( )
{
   INT y, dy;
   SHORT *ZBuf;
   SHORT *Dst;

   y = Scan_Start;
   Dst = ZBuffer_Front + (y+1)*_RCst_.Pix_Width;
   ZBuf = ZBuffer + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )
   {
      INT Len;
      UINT S;
      SHORT *ZBuf2;
      SHORT *Dst2;

      Len = Scan_Pt1[y];
      Dst2 = Dst - Len;
      ZBuf2 = ZBuf - Len;
      Len = Len - Scan_Pt2[y];

      S = Scan_S[y];
      while( Len<0 )
      {
         SHORT Tmp;
         Tmp = (SHORT)( (S>>16) - (INT)ZBuf2[Len] );
         if ( Tmp>=0x00) Dst2[Len] += Tmp + 0x0001;
         S -= dyS;
         Len++;
      }
      Dst += _RCst_.Pix_Width;
      ZBuf += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
}

static void Draw_zBuf_SNeg_Shadow( )
{
   INT y, dy;
   SHORT *ZBuf;
   SHORT *Dst;

   y = Scan_Start;
   Dst =  ZBuffer_Back + (y+1)*_RCst_.Pix_Width;
   ZBuf = ZBuffer + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )
   {
      INT Len;
      UINT S;
      SHORT *ZBuf2;
      SHORT *Dst2;

      Len = Scan_Pt1[y];
      Dst2 = Dst - Len;
      ZBuf2 = ZBuf - Len;
      Len = Len - Scan_Pt2[y];

      S = Scan_S[y];
      while( Len<0 )
      {
         SHORT Tmp;
         Tmp = (SHORT)( (S>>16) - (INT)ZBuf2[Len] );
         if ( Tmp>=0x00 ) Dst2[Len] += Tmp + 0x0001;
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
/******************************************************************/

EXTERN void Shader_Transform_And_Draw_VShadow4( MATRIX2 M )
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

#if 0
      if ( Edge->Poly2 == 0xFFFF )
      {
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
      }
#endif

      Nb_Edges_To_Deal_With = Nb_Out_Edges = 4;
      if ( Shader_Emit_VShadow_Poly( ) ) continue;

      // if ( Shader_Deal_zBuf( ) != RENDER_FLAT_ZBUF )
      Shader_Deal_zBuf( );
      if ( Edge->State & EDGE_POS_SHADOW )
         Draw_zBuf_SPos_Shadow( );
      else Draw_zBuf_SNeg_Shadow( );
   }
}

/******************************************************************/

EXTERN SHADER_METHODS Light_zBuf_Shader4 =
{
   NULL,
   Shader_Transform_And_Draw_VShadow4,
   Split_Gouraud, NULL,
   NULL, NULL, NULL,
   NULL
};

/******************************************************************/
