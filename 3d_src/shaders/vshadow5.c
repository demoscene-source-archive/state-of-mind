/***********************************************
 *              vshadow4.c                     *
 *        Volumic soft multi shadows           *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

#define DEBUG_EDGES

/******************************************************************/
/******************************************************************/
/*                   V-SHADOWS V (last try)                       */
/******************************************************************/
/******************************************************************/

static void Select_Sil_Edges4( MATRIX2 M )
{
   INT i;
   MSH_EDGE *Edge;

      // reset state

   memset( Vertex_State, 0, Cur_Msh->Nb_Vertex );
   Edge = Cur_Msh->Edges;
   for( i=Cur_Msh->Nb_Edges; i>0; --i, Edge++ )
      Edge->State = 0x00;
   for( i=0; i<Cur_Msh->Nb_Polys; ++i )
      Cur_Msh->Polys[i].Clipped = 0x00;

      // Store all vertex position in .N[] field

   Nb_Poly_Sorted = 0;
   Edge = Cur_Msh->Edges;
   for( i=0; i<Cur_Msh->Nb_Edges; ++i, Edge++ )
   {
      INT P1, P2, j, k;
      FLT Sign, Dot;
      VECTOR Do;  // <= CP1 ^ CP2
      VECTOR AB, AL, AC;

      if ( Edge->Type & EDGE_TYPE_FLAT ) continue;
      if ( !(Edge->Type & EDGE_TYPE_SHARP) ) continue;

      P1 = Edge->Poly1;
      if ( Cur_Msh->Polys[P1].Clipped == 0x00 )
      {
         nA_V( Poly_To_Sort[P1].N, M, Cur_Msh->Polys[P1].No );
         Cur_Msh->Polys[P1].Clipped = 0x01;  // i.e: Normal is transformed
      }

      j = Edge->Start;
      if ( !(Vertex_State[j]&0x02) )
      {
         A_V( P_Vertex[j].N, M, Cur_Msh->Vertex[j] );
         Vertex_State[j] |= 0x02;  // i.e.: Vtx transformed
      }

            // AL.N1 = OL.N1 - OA.N1
            // Camera is located in O
            // AL.N1>0  =>   EDGE_REGION_N1 (region I ou III )
            // AL.N2>0  =>   EDGE_REGION_N2 (region II ou III )

      Dot  = Dot_Product( Poly_To_Sort[P1].N, P_Vertex[j].N );
      if ( Dot>0.0 ) Edge->State |= EDGE_P1_BACKCULL;
      Dot -= Dot_Product( Poly_To_Sort[P1].N, Cur_Light->P );
      if ( Dot<0.0 ) Edge->State |= EDGE_REGION_N1; // AL.N1<=0...

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
         if ( Dot<0.0 ) Edge->State |= EDGE_REGION_N2;

            // EDGE_REGION_N1, !EDGE_REGION_N2 => region I   (*)
            //!EDGE_REGION_N1,  EDGE_REGION_N2 => region II  (*)
            // EDGE_REGION_N1,  EDGE_REGION_N2 => region III
            //!EDGE_REGION_N1, !EDGE_REGION_N2 => region IV

         j = Edge->State & ( EDGE_REGION_N1 | EDGE_REGION_N2 );
         if ( (j!=EDGE_REGION_N1)&&(j!=EDGE_REGION_N2) ) continue;
         if ( j==EDGE_REGION_N2 ) { Sign = -1.0; k = P2; }
         else { Sign = 1.0; k = P1; }
      }
      else     // only region I or II. regions III or IV are void.
      {
         if ( Edge->State & EDGE_REGION_N1 ) Sign = 1.0;
         else Sign = -1.0;
         k = P1;
      }

      if ( !(Vertex_State[Edge->End]&0x02) )
      {
         A_V( P_Vertex[Edge->End].N, M, Cur_Msh->Vertex[Edge->End] );
         Vertex_State[Edge->End] |= 0x02;  // i.e.: Vtx transformed
      }

      Sub_Vector( AB, P_Vertex[Edge->End].N, P_Vertex[Edge->Start].N );
      Sub_Vector( AL, Cur_Light->P, P_Vertex[Edge->Start].N );
      Cross_Product( Do, AL, AB );
      if ( Dot_Product( AL, Poly_To_Sort[k].N )>0.0 )
         Sign = -Sign;

      Dot = Sign*Dot_Product( Do, P_Vertex[Edge->Start].N );
      if ( Dot<0.0 ) Edge->State |= EDGE_POS_SHADOW;

      Vertex_State[ Edge->Start ] |= 0x01;
      Vertex_State[ Edge->End ] |= 0x01;
      Poly_Keys[ Nb_Poly_Sorted++ ] = i;
   }
}

/******************************************************************/
/******************************************************************/

static void Build_Shadow_Volume4( MATRIX2 M )
{
   INT i, Nb_Sil_Vtx;

   Nb_Sil_Vtx = 0;
   for( i=Cur_Msh->Nb_Vertex-1; i>=0; --i )
   {
      VECTOR Pt;
      FLT Inv_Z;
      P_VERTICE *Cap;

      if ( (Vertex_State[i]&0x01)!=0x01 ) 
      {
         Vertex_State[i] = 0x00; // clear clip flags
         continue;
      }
      Vertex_State[i] = 0x00; // clear clip flags

               // Project
      Pt[0] = P_Vertex[i].N[0]; 
      Pt[1] = P_Vertex[i].N[1];
      Pt[2] = P_Vertex[i].N[2];
      if ( fabs(Pt[2])<Z_EPSILON ) Pt[2] = Z_EPSILON;

      P_Vertex[i].Inv_Z = Inv_Z = 1.0f/Pt[2];
      Inv_Z *= _RCst_._Lx_;

      P_Vertex[i].xp = _RCst_._Cx_ + Pt[0]*Inv_Z;
      P_Vertex[i].yp = _RCst_._Cy_ - Pt[1]*Inv_Z;

      if ( Pt[2]>_RCst_.Clips[6] ) Vertex_State[i] |= VTX_CLIP_Z1;
      if ( Pt[2]<=_RCst_.Clips[4] ) Vertex_State[i] |= VTX_CLIP_Zo;
//      else  // .xp & .xp are correct, since Inv_Z>0.0. => set flags...
      {
         if ( P_Vertex[i].xp<=_RCst_.Clips[0] ) Vertex_State[i] |= VTX_CLIP_Xo;
         if ( P_Vertex[i].xp> _RCst_.Clips[1] ) Vertex_State[i] |= VTX_CLIP_X1;
         if ( P_Vertex[i].yp<=_RCst_.Clips[2] ) Vertex_State[i] |= VTX_CLIP_Yo;
         if ( P_Vertex[i].yp> _RCst_.Clips[3] ) Vertex_State[i] |= VTX_CLIP_Y1;
      }

      *((INT*)&P_Vertex[i].UV[1] ) = Nb_Sil_Vtx;  // Store obj's vertex in UV[1] field, as an int

         //       -=[ Cap vtx ]=-
         //
         // Cap = (1+A)*(Vtx-Light) + Light
         //     = Vtx + A.( Vtx-Light )
         //
         // [ A = Z_Max / ||(Vtx-Light)|| ]
         //

      Sub_Vector_Eq( Pt, Cur_Light->P );

      Cap_State[Nb_Sil_Vtx] = 0x00;
      Cap = &Cap_Vtx[Nb_Sil_Vtx];

      Inv_Z = 1.0f + Cur_Light->Z_Max / (FLT)sqrt( (double)Norm_Squared( Pt ) );
      Pt[0] *= Inv_Z; Pt[1] *= Inv_Z; Pt[2] *= Inv_Z;
      Add_Vector( Cap->N, Pt, Cur_Light->P );

      if ( fabs(Cap->N[2])<Z_EPSILON ) Cap->N[2] = Z_EPSILON;

      Cap->Inv_Z = Inv_Z = 1.0f/Cap->N[2];      
      Inv_Z *= _RCst_._Lx_;

      Cap->xp = _RCst_._Cx_ + Cap->N[0]*Inv_Z;
      Cap->yp = _RCst_._Cy_ - Cap->N[1]*Inv_Z;

      if ( Cap->N[2]>_RCst_.Clips[6] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_Z1;
      if ( Cap->N[2]<=_RCst_.Clips[4] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_Zo;
//      else  // .xp & .xp are correct, since Inv_Z>0.0. => set flags...
      {
         if ( Cap->xp<=_RCst_.Clips[0] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_Xo;
         if ( Cap->xp> _RCst_.Clips[1] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_X1;
         if ( Cap->yp<=_RCst_.Clips[2] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_Yo;
         if ( Cap->yp> _RCst_.Clips[3] ) Cap_State[Nb_Sil_Vtx] |= VTX_CLIP_Y1;
      }
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
         Tmp = (SHORT)( (S>>16)-ZBuf2[Len] );
         if ( Tmp>0 ) Dst2[Len]+=Tmp;
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
         Tmp = (SHORT)( (S>>16)-ZBuf2[Len] );
         if ( Tmp>0 ) Dst2[Len] += Tmp;
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

#ifdef DEBUG_EDGES
static void Draw_Edge( MSH_EDGE *Edge, void *CMap1, void *CMap2 )
{
//  if ( Edge->Poly2 == 0xFFFF )
//  if ( !(Edge->State & EDGE_POS_SHADOW) )
   {
      VECTOR Pi, Pf;
      if ( Edge->State & EDGE_POS_SHADOW ) Line_16_CMap = (USHORT*)CMap1;
      else Line_16_CMap = (USHORT*)CMap2;
      Pi[0] = P_Edges[0]->N[0];
      Pi[1] = P_Edges[0]->N[1];
      Pi[2] = P_Edges[0]->N[2];
      Pf[0] = P_Edges[1]->N[0];
      Pf[1] = P_Edges[1]->N[1];
      Pf[2] = P_Edges[1]->N[2];
      Emit_3D_Line( Pi, Pf );
      Pi[0] = P_Edges[0]->N[0];
      Pi[1] = P_Edges[0]->N[1];
      Pi[2] = P_Edges[0]->N[2];
      Pf[0] = P_Edges[3]->N[0];
      Pf[1] = P_Edges[3]->N[1];
      Pf[2] = P_Edges[3]->N[2];
      Emit_3D_Line( Pi, Pf );
      Pi[0] = P_Edges[1]->N[0];
      Pi[1] = P_Edges[1]->N[1];
      Pi[2] = P_Edges[1]->N[2];
      Pf[0] = P_Edges[2]->N[0];
      Pf[1] = P_Edges[2]->N[1];
      Pf[2] = P_Edges[2]->N[2];
      Emit_3D_Line( Pi, Pf );
      Pi[0] = P_Edges[2]->N[0];
      Pi[1] = P_Edges[2]->N[1];
      Pi[2] = P_Edges[2]->N[2];
      Pf[0] = P_Edges[3]->N[0];
      Pf[1] = P_Edges[3]->N[1];
      Pf[2] = P_Edges[3]->N[2];
      Emit_3D_Line( Pi, Pf );

   }
}
#endif


EXTERN void Shader_Transform_And_Draw_VShadow5( MATRIX2 M )
{
   INT i;
#ifdef DEBUG_EDGES
   USHORT CMap1[256];
   USHORT CMap2[256];
   Drv_Build_Ramp_16( (void *)CMap1, 0, 256, 0,0,0, 100,255,255, 0x2565 );
   Drv_Build_Ramp_16( (void *)CMap2, 0, 256, 0,0,0, 255,100,100, 0x2565 );
   Init_Anti_Alias_Table( 0, 256, 1.0 );
#endif

   Select_Sil_Edges4( M );
   Build_Shadow_Volume4( M );

   for( i=0; i<Nb_Poly_Sorted; ++i )
   {
      INT Vtx;
      MSH_EDGE *Edge;

      Edge = &Cur_Msh->Edges[ Poly_Keys[i] ];

               // Cap vtx #1
      Vtx = Edge->Start;
      Out_Vtx[0] = P_Edges[0] = &P_Vertex[Vtx];
      Vtx_Flags[0] = Vertex_State[Vtx];  //  & ~0x03;

      Vtx = *((INT *)&P_Vertex[Vtx].UV[1] );
      Out_Vtx[1] = P_Edges[1] = &Cap_Vtx[Vtx];
      Vtx_Flags[1] = Cap_State[Vtx];

               // Cap vtx #2
      Vtx = Edge->End;
      Out_Vtx[3] = P_Edges[3] = &P_Vertex[Vtx];
      Vtx_Flags[3] = Vertex_State[Vtx]; // & ~0x03;

      Vtx = *((INT *)&P_Vertex[Vtx].UV[1] );
      Out_Vtx[2] = P_Edges[2] = &Cap_Vtx[Vtx];
      Vtx_Flags[2] = Cap_State[Vtx];

#ifdef DEBUG_EDGES
      Draw_Edge( Edge, CMap1, CMap2 );
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

EXTERN SHADER_METHODS Light_zBuf_Shader5 =
{
   NULL,
   Shader_Transform_And_Draw_VShadow5,
   Split_Raw, NULL,
   NULL, NULL, NULL,
   NULL
};

/******************************************************************/
