/***********************************************
 *              Fake rotozoomer                *
 *  uses a 3d (big) poly for the mapping :)    *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

static F_MAPPING Base_uv[5] = {
  {0.0,0.0}, {1.0,0.0}, {1.0,1.0}, {0.0,1.0}, {0.0,0.0} 
};

static POLY The_Poly = 
{
   0x00, 0x00, 0xF0,
   { 0, 1, 2, 3 },
   4,
   0x3c, /* Full x-y clipped */
   { 0.0, 0.0, 1.0 },
   NULL,
   { 0, 1, 2, 0 },
   1.0,
   0.0,
   { {0,0},{0,0},{0,0},{0,0} }
};
static P_POLY The_P_Poly;

static P_VERTICE The_P_Vertex[4];
static F_MAPPING Base_Vertex[5] = {
   { -1.0, -1.0 },
   {  1.0, -1.0 },
   {  1.0,  1.0 },
   { -1.0,  1.0 }
 };

/*********************************************************************/

EXTERN void RotoZoom_3D( FLT Rot, FLT Scale, FLT Cx, FLT Cy, void *Ptr )
{
   INT i, j;
   RENDERING_FUNCTION Draw;
   
   Cx = 1.0f*_RCst_.Pix_Width - Cx;
   for( i=0; i<4; ++i )
   {
      FLT x, y;
      x = Base_Vertex[i][0]*(FLT)cos(Rot);
      x +=  -Base_Vertex[i][1]*(FLT)sin(Rot);
      The_P_Vertex[i].xp = Cx + x*Scale;
      y = Base_Vertex[i][0]*(FLT)sin(Rot);
      y += Base_Vertex[i][1]*(FLT)cos(Rot);
      The_P_Vertex[i].yp = y*Scale + Cy;

      The_P_Vertex[i].UV[0] = Base_uv[i][0];
      The_P_Vertex[i].UV[1] = Base_uv[i][1];
      The_Poly.UV[i][0] = (USHORT)(Base_uv[i][0]*65535.5f);
      The_Poly.UV[i][1] = (USHORT)(Base_uv[i][1]*65535.5f);
      The_P_Vertex[i].N[0] = 0.0;
      The_P_Vertex[i].N[1] = 1.0;   // z
      The_P_Vertex[i].N[2] = 1.0;   // Inv_z
      The_P_Vertex[i].Inv_Z = 1.0;      
   }

   dU1 = 1.0; dU2 = 0.0; dV1 = 0.0; dV2 = 1.0;
   The_Poly.U1V2_U2V1 = 1.0;

   Cur_P_Poly = &The_P_Poly;
   Orig_Poly = Cur_P_Poly->Poly_Orig = &The_Poly;;
   Nb_Edges_To_Deal_With = Orig_Poly->Nb_Pts;
   Cur_Shader = &UV_Shader;

            /* Clip */

   if ( 1 ) /* Orig_Poly->Clipped & 0xFCFC */
   {
      Nb_Out_Edges = j = Nb_Edges_To_Deal_With;
      Out_Vtx[j] = P_Edges[j] = The_P_Vertex + Orig_Poly->Pt[0];
      Vtx_Flags[j] = 0x0;

      if ( P_Edges[j]->xp < _RCst_.Clips[0] ) Vtx_Flags[j] |= VTX_CLIP_Xo;
      if ( P_Edges[j]->xp > _RCst_.Clips[1] ) Vtx_Flags[j] |= VTX_CLIP_X1;
      if ( P_Edges[j]->yp < _RCst_.Clips[2] ) Vtx_Flags[j] |= VTX_CLIP_Yo;
      if ( P_Edges[j]->yp > _RCst_.Clips[3] ) Vtx_Flags[j] |= VTX_CLIP_Y1;
      Orig_Poly->Clipped = 0x00;
      for( --j; j>=0; --j )
      {
         Out_Vtx[j] = P_Edges[j] = The_P_Vertex + Orig_Poly->Pt[j];
         Vtx_Flags[j] = 0x0;
         if ( P_Edges[j]->xp < _RCst_.Clips[0] ) Vtx_Flags[j] |= VTX_CLIP_Xo;
         if ( P_Edges[j]->xp > _RCst_.Clips[1] ) Vtx_Flags[j] |= VTX_CLIP_X1;
         if ( P_Edges[j]->yp < _RCst_.Clips[2] ) Vtx_Flags[j] |= VTX_CLIP_Yo;
         if ( P_Edges[j]->yp > _RCst_.Clips[3] ) Vtx_Flags[j] |= VTX_CLIP_Y1;
         Orig_Poly->Clipped |= Vtx_Flags[j];
      }
      dy1 = P_Edges[2]->yp-P_Edges[1]->yp;
      dx0 = P_Edges[1]->xp-P_Edges[0]->xp;
      dy0 = P_Edges[1]->yp-P_Edges[0]->yp;
      dx1 = P_Edges[2]->xp-P_Edges[1]->xp,
      Area = P_Edges[0]->yp*dx1 + P_Edges[2]->yp*dx0;
      Area += P_Edges[1]->yp * ( P_Edges[0]->xp-P_Edges[2]->xp );
   
      if ( Area<=AREA_EPSILON3 ) return;

      SH_Clip( Orig_Poly->Clipped );  // !!!
      if ( !Nb_Out_Edges ) return;
   }

            /* Rasterize */

   aZ1 = 0.0; aZ2 = 0.0;

   for( j=Nb_Out_Edges-1; j>=0; --j )
   {
      Edges_y[j] = Out_Vtx[j]->yp;
      Edges_dy[j] = Out_Vtx[j+1]->yp - Edges_y[j];
      Edges_x[j] = Out_Vtx[j]->xp;
      Edges_dx[j] = Out_Vtx[j+1]->xp - Edges_x[j];
      Edges_z[j] = Out_Vtx[j]->Inv_Z;
      aZ2 *= Edges_z[j];
      Edges_dz[j] = Out_Vtx[j+1]->Inv_Z - Edges_z[j];
      aZ1 *= Edges_dz[j];
   }
   Edges_y[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->yp;
   Edges_x[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->xp;
   Edges_z[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->Inv_Z;

   aZ2 = 1.0f/aZ2;
   aZ1 = aZ1 * aZ2; // /(FLT)Nb_Out_Edges;
   aZ2 *= (FLT)Nb_Out_Edges;

   Area = 1.0f/Area;

   UV_Src = (BYTE*)Ptr;
   Cur_Mip = 8;
   Mip_Mask = Mip_Masks[Cur_Mip];
   Uo_Mip = Vo_Mip = 0.0;
   Mip_Scale = 256.0*65536.0;

   Rasterize_Edges( );
   Constants_Computed = TRUE;
   Draw = UV_Shader.Deal_With_Poly( );
   if ( Draw!=RENDER_NONE ) (*Primitives[ Draw ])( );
}

/*********************************************************************/

EXTERN void Spherical_3D( F_MAPPING Uo, F_MAPPING U1, F_MAPPING U2, void *Ptr )
{
   INT j;
   RENDERING_FUNCTION Draw;

   The_P_Vertex[0].xp = _RCst_.Clips[0];
   The_P_Vertex[0].yp = _RCst_.Clips[2];
   The_P_Vertex[0].UV[0] = Uo[0];
   The_P_Vertex[0].UV[1] = Uo[1];
   The_Poly.UV[0][0] = (USHORT)( The_P_Vertex[0].UV[0]*65535.5f );
   The_Poly.UV[0][1] = (USHORT)( The_P_Vertex[0].UV[1]*65535.5f );
   The_P_Vertex[0].N[0] = 0.0;
   The_P_Vertex[0].N[1] = The_P_Vertex[0].N[2] = 1.0f;
   The_P_Vertex[0].Inv_Z = 1.0f;

   The_P_Vertex[1].xp = _RCst_.Clips[1];
   The_P_Vertex[1].yp = _RCst_.Clips[2];
   The_P_Vertex[1].UV[0] = U1[0];
   The_P_Vertex[1].UV[1] = U1[1];
   The_Poly.UV[1][0] = (USHORT)( The_P_Vertex[1].UV[0]*65535.5f );
   The_Poly.UV[1][1] = (USHORT)( The_P_Vertex[1].UV[1]*65535.5f );
   The_P_Vertex[1].N[0] = 0.0;
   The_P_Vertex[1].N[1] = The_P_Vertex[1].N[2] = 1.0f;
   The_P_Vertex[1].Inv_Z = 1.0f;

   The_P_Vertex[2].xp = _RCst_.Clips[1];
   The_P_Vertex[2].yp = _RCst_.Clips[3];
   The_P_Vertex[2].UV[0] = U2[0] + U1[0] - Uo[0];
   The_P_Vertex[2].UV[1] = U2[1] + U1[1] - Uo[1];
   The_Poly.UV[2][0] = (USHORT)( The_P_Vertex[2].UV[0]*65535.5f );
   The_Poly.UV[2][1] = (USHORT)( The_P_Vertex[2].UV[1]*65535.5f );
   The_P_Vertex[2].N[0] = 0.0;
   The_P_Vertex[2].N[1] = The_P_Vertex[2].N[2] = 1.0f;
   The_P_Vertex[2].Inv_Z = 1.0f;

   The_P_Vertex[3].xp = _RCst_.Clips[0];
   The_P_Vertex[3].yp = _RCst_.Clips[3];
   The_P_Vertex[3].UV[0] = U2[0];
   The_P_Vertex[3].UV[1] = U2[1];
   The_Poly.UV[3][0] = (USHORT)( The_P_Vertex[3].UV[0]*65535.5f );
   The_Poly.UV[3][1] = (USHORT)( The_P_Vertex[3].UV[1]*65535.5f );
   The_P_Vertex[3].N[0] = 0.0;
   The_P_Vertex[3].N[1] = The_P_Vertex[3].N[2] = 1.0f;
   The_P_Vertex[3].Inv_Z = 1.0f;

   dU1 = U1[0]-Uo[0]; dU2 = U2[0]-Uo[0]; 
   dV1 = U1[1]-Uo[1]; dV2 = U2[1]-Uo[1];
   The_Poly.U1V2_U2V1 = dU1*dV2 - dU2*dV1;

   Cur_P_Poly = &The_P_Poly;
   Orig_Poly = Cur_P_Poly->Poly_Orig = &The_Poly;;
   Nb_Edges_To_Deal_With = Orig_Poly->Nb_Pts;
   Cur_Shader = &UV_Shader;

   Out_Vtx[0] = P_Edges[0] = The_P_Vertex;
   Out_Vtx[1] = P_Edges[1] = The_P_Vertex + 1;
   Out_Vtx[2] = P_Edges[2] = The_P_Vertex + 2;
   Out_Vtx[3] = P_Edges[3] = The_P_Vertex + 3;
   Out_Vtx[4] = P_Edges[4] = The_P_Vertex;

   Nb_Out_Edges = Nb_Edges_To_Deal_With = 4;

   dy1 = P_Edges[2]->yp-P_Edges[1]->yp;
   dx0 = P_Edges[1]->xp-P_Edges[0]->xp;
   dy0 = P_Edges[1]->yp-P_Edges[0]->yp;
   dx1 = P_Edges[2]->xp-P_Edges[1]->xp,
   Area = P_Edges[0]->yp*dx1 + P_Edges[2]->yp*dx0;
   Area += P_Edges[1]->yp * ( P_Edges[0]->xp-P_Edges[2]->xp );
   Area = 1.0f/Area;
   aZ1 = 0.0; aZ2 = 1.0f;

   for( j=Nb_Out_Edges-1; j>=0; --j )
   {
      Edges_y[j]  = P_Edges[j]->yp;
      Edges_dy[j] = P_Edges[j+1]->yp - Edges_y[j];
      Edges_x[j]  = P_Edges[j]->xp;
      Edges_dx[j] = P_Edges[j+1]->xp - Edges_x[j];
      Edges_z[j]  = P_Edges[j]->Inv_Z;
      Edges_dz[j] = P_Edges[j+1]->Inv_Z - Edges_z[j];
   }
   Edges_y[Nb_Out_Edges] = P_Edges[Nb_Out_Edges]->yp;
   Edges_x[Nb_Out_Edges] = P_Edges[Nb_Out_Edges]->xp;
   Edges_z[Nb_Out_Edges] = P_Edges[Nb_Out_Edges]->Inv_Z;

   UV_Src = (BYTE*)Ptr;
   Cur_Mip = 8;
   Uo_Mip = Vo_Mip = 0.0;
   Mip_Scale = 256.0f*65536.0f;

   Rasterize_Edges( );
   Constants_Computed = TRUE;
   Draw = UV_Shader.Deal_With_Poly( );
   if ( Draw!=RENDER_NONE ) (*Primitives[ Draw ])( );
}

/******************************************************************/
