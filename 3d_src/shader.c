/***********************************************
 *               shader.c                      *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/
/******************************************************************/

EXTERN INT Scan_U[MAX_SCANS], Scan_V[MAX_SCANS];
EXTERN INT Scan_S[MAX_SCANS];
// EXTERN INT Scan_T[MAX_SCANS];
EXTERN FLT Scan_UiZ[MAX_SCANS], Scan_ViZ[MAX_SCANS], Scan_iZ[MAX_SCANS];
EXTERN FLT Scan_UiZ_2[MAX_SCANS], Scan_ViZ_2[MAX_SCANS], Scan_iZ_2[MAX_SCANS];
EXTERN INT Scan_Pt1[MAX_SCANS], Scan_Pt2[MAX_SCANS];

/******************************************************************/

EXTERN FLT Edges_x[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Edges_dx[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
EXTERN FLT Edges_y[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Edges_dy[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
EXTERN FLT Edges_z[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Edges_dz[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
EXTERN FLT Slopes[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES], Slope_Full[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
EXTERN FLT Edges_ey[MAX_EDGES+1+MAX_ADDITIONAL_VERTICES];
EXTERN INT Edges_yf[MAX_EDGES+MAX_ADDITIONAL_VERTICES], Edges_yo[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
EXTERN INT Edges_Xe[MAX_EDGES+MAX_ADDITIONAL_VERTICES], Edges_dXe[MAX_EDGES+MAX_ADDITIONAL_VERTICES];

EXTERN INT Scan_Start, Scan_H;
EXTERN INT Span_Len, Span_Y;
EXTERN INT Nb_Right_Edges;
EXTERN USHORT Right_Edges[MAX_EDGES+MAX_ADDITIONAL_VERTICES];
EXTERN INT Nb_Left_Edges;
EXTERN USHORT Left_Edges[MAX_EDGES+MAX_ADDITIONAL_VERTICES];

/******************************************************************/

EXTERN FLT Area, Area2, aZ1, aZ2;

EXTERN FLT DxU, DyU, DzU;
EXTERN FLT DxV, DyV, DzV;
EXTERN FLT DxS, DyS, DxT, DyT;
EXTERN FLT U, dU1, dU2;
EXTERN FLT V, dV1, dV2;
EXTERN FLT S, dS1, dS2;
EXTERN FLT T, dT1, dT2;
EXTERN FLT diZ1, diZ2, iZ, DyiZ, DxiZ;

EXTERN FLT dy0, dy1, dx0, dx1;
EXTERN INT dyS, dyT, dyU, dyV, dyiZ;

   /* common data needed by Draw_*() and shaders funcs */

EXTERN PIXEL *UV_Src, *UV_Src2;
EXTERN USHORT Flat_Color, Flat_Color2;
EXTERN USHORT The_ID;
EXTERN POLY *Orig_Poly;
EXTERN P_POLY *Cur_P_Poly;
EXTERN UINT Cur_Vtx_Nb;
EXTERN UINT Cur_P_Vertex_Nb;
EXTERN UINT Cur_Poly_Pt;
EXTERN P_VERTICE *Cur_P_Vertex;
EXTERN PIXEL *Cur_Vertex_State;
EXTERN TEXTURE_MAP *Cur_Texture;
EXTERN PIXEL *Cur_Texture_Ptr;
EXTERN MATERIAL *Cur_Material;
EXTERN INT Constants_Computed;


// EXTERN INT  Scratch_Int;

/******************************************************************/
/******************************************************************/

#if 0
static INT Nb_Stats=0;
static INT _T1_[500];
static FLT _T2_[500];
EXTERN void Add_Stats( INT T1, INT T2, INT Len )
{
   if ( Len==0 ) return;
   T2 = T2-T1 - 16;
   if ( T2>3000 ) return;
   _T1_[Nb_Stats] = T2;
   _T2_[Nb_Stats] = 1.0*T2/Len;
   printf( "dT:%d  dT/L=%lf    ", Len, 1.0*T2/Len );
   Nb_Stats++;
   if ( Nb_Stats==500 )
   {
      INT i;
      FLT x, X;
      x = 0.0; X = 0.0;
      for( i=0; i<Nb_Stats; ++i )
      {
         x += _T2_[i]; X += (FLT)_T1_[i];
      }
      printf( "-dT-:%f -dT/L-:%f\n", x/(FLT)(Nb_Stats), X/(FLT)(Nb_Stats) );
      Nb_Stats = 0;
   }
}
#endif

/******************************************************************/

   // this one is called before calling Emit_Poly()...
   // sets up Out_Vtx[], Vtx_Flags[] and 
   // computes dx0, dy0, dx1, dy1, Area...

EXTERN void Setup_Poly_Vtx( )
{
   INT j;

   Nb_Out_Edges = j = Nb_Edges_To_Deal_With;
   Out_Vtx[j] = P_Edges[j] = P_Vertex + Orig_Poly->Pt[0];
      // keep only useful flags
   Vtx_Flags[j] = Vertex_State[ Orig_Poly->Pt[0] ] & ~FLAG_FOR_CLIPPING_II;

   for( --j; j>=0; --j )
   {
      Out_Vtx[j] = P_Edges[j] = P_Vertex + Orig_Poly->Pt[j];
      Vtx_Flags[j] = Vertex_State[ Orig_Poly->Pt[j] ] & ~FLAG_FOR_CLIPPING_II;
   }
   dy1 = P_Edges[2]->yp-P_Edges[1]->yp;
   dx0 = P_Edges[1]->xp-P_Edges[0]->xp;
   dy0 = P_Edges[1]->yp-P_Edges[0]->yp;
   dx1 = P_Edges[2]->xp-P_Edges[1]->xp,
//   Area  = dx0*dy1 - dx1*dy0;
   Area = P_Edges[0]->yp*dx1 + P_Edges[2]->yp*dx0;
   Area += P_Edges[1]->yp * ( P_Edges[0]->xp-P_Edges[2]->xp );

      // TODO: Need to compute Area from real 3d coords here,
      // becoz xp,yp can be negatively projected when its
      // z is <0.0 !!
}

EXTERN void Emit_Poly( )
{
   INT j;
   RENDERING_FUNCTION Draw;

   j = Orig_Poly->Clipped & FLAG_CLIP_USEFUL;   // Xo/X1/Zo/Z1
   if ( j )
   {
      SH_Clip( (USHORT)j );
      if ( !Nb_Out_Edges ) return;
   }
   // else: => Just emit

            /* Rasterize */

   aZ1 = 0.0; aZ2 = 0.0;   // (FLT)Nb_Out_Edges;

   for( j=Nb_Out_Edges-1; j>=0; --j )
   {
      Edges_y[j] = Out_Vtx[j]->yp;
      Edges_dy[j] = Out_Vtx[j+1]->yp - Edges_y[j];
      Edges_x[j] = Out_Vtx[j]->xp;
      Edges_dx[j] = Out_Vtx[j+1]->xp - Edges_x[j];
      Edges_z[j] = Out_Vtx[j]->Inv_Z;
      aZ2 += Edges_z[j];
      Edges_dz[j] = Out_Vtx[j+1]->Inv_Z - Edges_z[j];
      aZ1 += (FLT)fabs( Edges_dz[j] );
   }
   Edges_x[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->xp;
   Edges_y[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->yp;
   Edges_z[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->Inv_Z;

   aZ2 = 1.0f/aZ2;
   aZ1 = aZ1 * aZ2; // /(FLT)Nb_Out_Edges;
   aZ2 *= (FLT)Nb_Out_Edges;

   Area = 1.0f/Area;

   Rasterize_Edges( );

   if ( Scan_H<=0 ) return;

   /* Format_Scanlines( ); */

   Constants_Computed = FALSE;
   Draw = (*Cur_Shader->Deal_With_Poly)( );
   if ( Draw!=RENDER_NONE )
      (*Primitives[ Draw ])( );
   
   if ( Cur_Shader->Deal_With_Poly2 != NULL )
   { 
      Constants_Computed = FALSE;
      Draw = (*Cur_Shader->Deal_With_Poly2)( );
      if ( Draw!=RENDER_NONE )
         (*Primitives[ Draw ])( );
   }

   if ( Cur_Shader->Deal_With_Poly3 != NULL )
   { 
      Constants_Computed = FALSE;
      Draw = (*Cur_Shader->Deal_With_Poly3)( );
      if ( Draw!=RENDER_NONE )
         (*Primitives[ Draw ])( );
   }
}

/******************************************************************/
/******************************************************************/
