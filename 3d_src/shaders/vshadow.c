/***********************************************
 *              vshadow.c                      *
 *        Volumic/soft shadows                 *
 * Skal 98                                     *
 * Only with 8b renderer!!                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/
/******************************************************************/
/*                      V-SHADOWS v1.0                            */
/******************************************************************/
/******************************************************************/

static void Select_Sil_Edges2( MATRIX2 M )
{
   INT i;
   FLT Z_Scale2;
   MSH_EDGE *Edge;
   P_POLY *P_Poly;
   VECTOR *Vertex;
   P_VERTICE *P_Vtx;

      // Store all vertex position in .N[] field

   memset( Vertex_State, 0, Cur_Msh->Nb_Vertex );
   Vertex = Cur_Msh->Vertex;
   P_Vtx = P_Vertex;
   for( i=Cur_Msh->Nb_Vertex; i>0; --i )
   {
      A_V( P_Vtx->N, M, (FLT*)Vertex );
      P_Vtx++;
      Vertex++;
   }

   Orig_Poly = Cur_Msh->Polys;
   P_Poly = Poly_To_Sort;
   P_Vtx = P_Vertex;
   for( i=0; i<Cur_Msh->Nb_Polys; ++i )
   {
      INT j;
      VECTOR N;
      FLT Dot;

      nA_V( N, M, Orig_Poly->No );
      j = Orig_Poly->Pt[0];
//      P_Poly->N[0] = Dot = Dot_Product( N, P_Vertex[j].N );
      Dot = Dot_Product( N, P_Vertex[j].N );
      Dot -= Dot_Product( N, Cur_Light->P );
      if ( Dot>0.0 ) Orig_Poly->Clipped = 0x02;      // Shadowed edge
      else Orig_Poly->Clipped = 0x01;

      Orig_Poly++;
      P_Poly++;
   }

         // Retreive silhouette edges

   Nb_Poly_Sorted = 0;
   Z_Scale2 = 32767.0f/( _RCst_.Clips[6]-_RCst_.Clips[4] );
   Edge = Cur_Msh->Edges;
   for( i=0; i<Cur_Msh->Nb_Edges; ++i, Edge++ )
   {
      INT P1, P2, T1, T2;
      FLT Z;

      if ( Edge->Type & EDGE_TYPE_FLAT ) continue;
      if ( !(Edge->Type & EDGE_TYPE_SHARP) ) continue;

      P1 = Edge->Poly1;
      P2 = Edge->Poly2;
      if ( P2!=0xFFFF)        // <= -1  Warning!
      {
         T1 = Cur_Msh->Polys[P1].Clipped;
         T2 = Cur_Msh->Polys[P2].Clipped;
         if ( (T1|T2)!=0x03 ) continue;
      }
      else 
      {
         T1 = Cur_Msh->Polys[P1].Clipped;
         if ( T1!=0x02 ) continue;
         P2 = P1;
      }

//      Z = Poly_To_Sort[P1].N[0] + Poly_To_Sort[P2].N[0];
#if 0
      {
         INT j;
         VECTOR N;
         nA_V( N, M, Cur_Msh->Polys[P1].No );
         j = Edge->Start;
         Z = Dot_Product( N, P_Vertex[j].N );
         j = Edge->End;
         Z += Dot_Product( N, P_Vertex[j].N );
         nA_V( N, M, Cur_Msh->Polys[P2].No );
         j = Edge->Start;
         Z += Dot_Product( N, P_Vertex[j].N );
         j = Edge->End;
         Z += Dot_Product( N, P_Vertex[j].N );
      }
#endif
      // if ( Z<0.0 ) Edge->State = VTX_CLIP_Z1;    // BF-Cull in bit 7
      // else Edge->State = 0x00;

      Edge->State = 0x00;      
      if ( T1==0x02 )
      {
         if ( Poly_To_Sort[P1].N[0]<0.0 )
            Edge->State = VTX_CLIP_Z1;    // BF-Cull in bit 7
      }
      else
      {
         if ( Poly_To_Sort[P2].N[0]>0.0 )
            Edge->State = VTX_CLIP_Z1;    // BF-Cull in bit 7
      }

      Vertex_State[ Edge->Start ] = 0x03;
      Z = P_Vertex[ Edge->Start ].N[2];
      Vertex_State[ Edge->End ] = 0x03;
      Z += P_Vertex[ Edge->End ].N[2];
      Z /= 4.0;
      Z = ( _RCst_.Clips[6]-Z ) * Z_Scale2;
      Poly_Keys[ Nb_Poly_Sorted++ ] = (i<<16) | (USHORT)(Z);
   }
}

/******************************************************************/
/******************************************************************/

EXTERN void Build_Shadow_Volume( MATRIX2 M )
{
   INT i, Nb_Sil_Vtx;

   Nb_Sil_Vtx = 0;
   for( i=Cur_Msh->Nb_Vertex-1; i>=0; --i )
   {
      VECTOR Pt;
      FLT Inv_Z;

      if ( Vertex_State[i]!=0x03 ) continue;

               // Project

      // A_V( Pt, M, Cur_Msh->Vertex[i] );
      Pt[0] = P_Vertex[i].N[0];
      Pt[1] = P_Vertex[i].N[1];
      Pt[2] = P_Vertex[i].N[2];
      Inv_Z = 1.0f/Pt[2];
      P_Vertex[i].Inv_Z = Inv_Z;

      Inv_Z *= _RCst_._Lx_;

      P_Vertex[i].xp = _RCst_._Cx_ + Pt[0]*Inv_Z;
      if ( P_Vertex[i].xp<_RCst_.Clips[0] ) Vertex_State[i] |= VTX_CLIP_Xo;
      if ( P_Vertex[i].xp>_RCst_.Clips[1] ) Vertex_State[i] |= VTX_CLIP_X1;

      P_Vertex[i].yp = _RCst_._Cy_ - Pt[1]*Inv_Z;
      if ( P_Vertex[i].yp<_RCst_.Clips[2] ) Vertex_State[i] |= VTX_CLIP_Yo;
      if ( P_Vertex[i].yp>_RCst_.Clips[3] ) Vertex_State[i] |= VTX_CLIP_Y1;
      if ( Pt[2]<=_RCst_.Clips[4] ) Vertex_State[i] |= VTX_CLIP_Zo;

      *((INT*)&P_Vertex[i].UV[1] ) = Nb_Sil_Vtx;  // Store obj's vertex in UV[1] field, as an int

               /* Cap vtx */

      Sub_Vector_Eq( Pt, Cur_Light->P );
#if 0
      if ( Pt[2]>Dz_EPSILON ) Inv_Z = _TMAX_;
      else
      {
         Inv_Z = (_RCst_.Clips[6]-Cur_Light->P[2])/Pt[2];
         if ( Inv_Z<1.0f ) Inv_Z = 1.0f;
      }
      Pt[0] = Pt[0]*Inv_Z+Cur_Light->P[0];
      Pt[1] = Pt[1]*Inv_Z+Cur_Light->P[1];
      Pt[2] = Pt[2]*Inv_Z+Cur_Light->P[2];
#else
            // Light distance: 18.0
      Inv_Z = 1.0f + Cur_Light->Z_Max / (FLT)sqrt( (double)Norm_Squared( Pt ) );
      Pt[0] *= Inv_Z; Pt[1] *= Inv_Z; Pt[2] *= Inv_Z;
      Add_Vector_Eq( Pt, Cur_Light->P ); 
#endif

      Cap_State[Nb_Sil_Vtx] = 0x00;
      Inv_Z = 1.0f/Pt[2];
      Cap_Vtx[Nb_Sil_Vtx].Inv_Z = Inv_Z;
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

   // ZBuffer_Front: Delta z from Scane/Shadow poly
   // ZBuffer_Back: Intensity ?

static void Draw_zBuf_Pos_Shadow( )
{
   INT y, dy;
   SHORT *ZBuf;
   SHORT *Dst;

   y = Scan_Start;
   Dst = ((SHORT*)ZBuffer_Front) + (y+1)*_RCst_.Pix_Width;
   ZBuf = ((SHORT*)ZBuffer) + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )
   {
      INT Len;
      UINT S;
      SHORT *Dst2, *ZBuf2;

      Len = Scan_Pt1[y];
      Dst2 = Dst - Len;
      ZBuf2 = ZBuf - Len;
      Len = Len - Scan_Pt2[y];

      S = Scan_S[y];
      while( Len<0 )
      {
         // SHORT Tmp;
         Dst2[Len] = (SHORT)(S>>16) - ZBuf2[Len];
         //Tmp = (SHORT)(S>>16) - ZBuf2[Len] ;
         //if ( Tmp<0 ) Dst2[Len] = 0x7FFF;
         //else Dst2[Len] = 0x3F00;
         S -= dyS;
         Len++;
      }
      Dst += _RCst_.Pix_Width;
      ZBuf += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
   // Flat_Color = 0x80; Shader_Draw_Flat( );
}

static void Draw_zBuf_Neg_Shadow( )
{
   INT y, dy;
   SHORT *ZBuf, *Front, *Toto;
   PIXEL *Screen;

   y = Scan_Start;
   Front = ((SHORT*)ZBuffer_Front) + (y+1)*_RCst_.Pix_Width;
   ZBuf = ((SHORT*)ZBuffer) + (y+1)*_RCst_.Pix_Width;
   Screen = ZBuffer_Shade + (y+1)*_RCst_.Pix_Width;
   Toto = ZBuffer_Back + (y+1)*_RCst_.Pix_Width;
   dy = Scan_H;
   while( dy>0 )
   {
      INT Len;
      UINT S;
      SHORT *ZBuf2, *Front2, *Toto2;
      PIXEL *Screen2;

      Len = Scan_Pt1[y];
      Front2 = Front - Len;
      ZBuf2 = ZBuf - Len;
      Screen2 = Screen - Len;
      Toto2 = Toto - Len;
      Len = Len - Scan_Pt2[y];

      S = Scan_S[y];
      while( Len<0 )
      {
         SHORT Tmp;
         Tmp = (SHORT)(S>>16) - ZBuf2[Len];
         Toto2[Len] = Tmp;
         // if ( Tmp<0 ) Toto2[Len] = 0xFFFF;
         // else Toto2[Len] = 0x7FFF;
         //Toto2[Len] = Tmp-Front2[Len];


         if ( Front2[Len]>0 )
            if ( Tmp<0 )
               Screen2[Len] = ((Tmp-Front2[Len])>>7 )&0xFF; // 0x3f;
                    // ( Screen2[Len]&0xFF00 ) | 0x3F;
#if 0
         if ( Front2[Len]>0 )
            if ( Tmp>0 ) 
               Screen2[Len] -= 0xFF; 
               // Screen2[Len] -= ( ( ( Tmp-Front2[Len] )>>7 )&0xFF );
#endif

         S -= dyS;
         Len++;
      }
      ZBuf += _RCst_.Pix_Width;
      Front += _RCst_.Pix_Width;
      Screen += _RCst_.Pix_Width;
      Toto += _RCst_.Pix_Width;
      ++y;
      dy--;
   }
   // Flat_Color = 0x40; Shader_Draw_Flat( );
}

/******************************************************************/
/******************************************************************/

static void Shader_Transform_And_Draw_VShadow( MATRIX2 M )
{
   INT i;

   Select_Sil_Edges2( M );
   Counting_Sort( Poly_Keys, Nb_Poly_Sorted );

   Build_Shadow_Volume( M );

         // Retreive front silhouette edges

   // for( i=Nb_Poly_Sorted-1; i>=0; --i )
   for( i=0; i<Nb_Poly_Sorted; ++i )
   {
      INT Start, End, Cap;
      MSH_EDGE *Edge;

      Edge = Cur_Msh->Edges + (Poly_Keys[i]>>16);
      Start = Edge->Start;
      End = Edge->End;

      Out_Vtx[0] = P_Edges[0] = &P_Vertex[Start];
      Vtx_Flags[0] = 0x00; // Vertex_State[ Start ] & ~(VTX_CLIP_Z1|0x03);

      Out_Vtx[1] = P_Edges[1] = &P_Vertex[End];
      Vtx_Flags[1] = 0x00; // Vertex_State[ End ] & ~(VTX_CLIP_Z1|0x03);

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

      if ( Shader_Deal_zBuf( ) != RENDER_FLAT_ZBUF )
      {
         if ( Edge->State & VTX_CLIP_Z1 )
            Draw_zBuf_Neg_Shadow( );
         else Draw_zBuf_Pos_Shadow( );
      }
   }
}

/******************************************************************/
/******************************************************************/

EXTERN SHADER_METHODS Light_zBuf_Shader =
{
   NULL,                                  // Set parameters
   Shader_Transform_And_Draw_VShadow,     // <= .Transform
   Split_Gouraud, NULL, // Split is only concerned with x,y, and Inv_Z...
   NULL, NULL, NULL,
   NULL
};

/******************************************************************/
/******************************************************************/
