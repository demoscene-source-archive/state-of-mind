/******************************************************************/
/*                  Line & 'design' shaders                       */
/******************************************************************/

#include "main3d.h"

/******************************************************************/

EXTERN void Emit_3D_Line( VECTOR Pi, VECTOR Pf )
{
   FLT xi, yi, xf, yf, e;
   if ( Pi[2]<_RCst_.Clips[4] )
   {
      if ( Pf[2]<_RCst_.Clips[4] ) return;
      e = ( _RCst_.Clips[4]-Pf[2] ) / ( Pi[2]-Pf[2] );
      Pi[0] = Pf[0] + (Pi[0]-Pf[0])*e;
      Pi[1] = Pf[1] + (Pi[1]-Pf[1])*e;
      Pi[2] = _RCst_.Clips[4];
   }
   if ( Pf[2]<_RCst_.Clips[4] )
   {
      e = ( _RCst_.Clips[4]-Pi[2] ) / ( Pf[2]-Pi[2] );
      Pf[0] = Pi[0] + (Pf[0]-Pi[0])*e;
      Pf[1] = Pi[1] + (Pf[1]-Pi[1])*e;
      Pf[2] = _RCst_.Clips[4];
   }
   e = _RCst_._Lx_/Pi[2];
   xi = _RCst_._Cx_ + Pi[0]*e;
   yi = _RCst_._Cy_ - Pi[1]*e;
   e = _RCst_._Lx_/Pf[2];
   xf = _RCst_._Cx_ + Pf[0]*e;
   yf = _RCst_._Cy_ - Pf[1]*e;

         // Mega-cast :)
   ((void (*)(FLT,FLT,FLT,FLT))Primitives[RENDER_LINE])( xi, yi, xf, yf );
}

/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Raw_Lines( )
{
   INT i;

   for( i=0; i<Nb_Out_Edges; ++i )
   {
      FLT xi, yi, xf, yf;
      xi = Edges_x[i]; yi = Edges_y[i];
      xf = Edges_x[i+1]; yf = Edges_y[i+1];
      // Mega-cast :)
      ((void (*)(FLT,FLT,FLT,FLT))Primitives[RENDER_LINE])( xi, yi, xf, yf );
   }
   return( RENDER_NONE );
}

/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Draw_All_Lines( )
{
   INT i;

   for( i=0; i<Orig_Poly->Nb_Pts; ++i )
   {
      MSH_EDGE *Edge;

      Edge = &Cur_Msh->Edges[ Orig_Poly->Edges[i] ];
      if ( !(Edge->Type&EDGE_TYPE_FLAT) )
//      if ( Edge->Sharpness>0x7000 ) 
      {
         VECTOR Pi, Pf;
         Pi[0] = P_Vertex[ Edge->Start ].N[0];
         Pi[1] = P_Vertex[ Edge->Start ].N[1];
         Pi[2] = P_Vertex[ Edge->Start ].N[2];
         Pf[0] = P_Vertex[ Edge->End ].N[0];
         Pf[1] = P_Vertex[ Edge->End ].N[1];
         Pf[2] = P_Vertex[ Edge->End ].N[2];
         Emit_3D_Line( Pi, Pf );

         Edge->State |= EDGE_STATE_DRAWN;
      }
   }
   return( RENDER_NONE );
}

EXTERN RENDERING_FUNCTION Shader_Draw_Lines( )
{
   INT i;

   for( i=0; i<Orig_Poly->Nb_Pts; ++i )
   {
      MSH_EDGE *Edge;

      Edge = &Cur_Msh->Edges[ Orig_Poly->Edges[i] ];
      if ( Edge->State & EDGE_STATE_SEEN )
      {
         if ( (Edge->Type&(EDGE_TYPE_FLAT|EDGE_TYPE_SHARP))==EDGE_TYPE_SHARP )
         {
//            if ( !( Edge->State & EDGE_STATE_DRAWN ) )
            {
               VECTOR Pi, Pf;
               Pi[0] = P_Vertex[ Edge->Start ].N[0];
               Pi[1] = P_Vertex[ Edge->Start ].N[1];
               Pi[2] = P_Vertex[ Edge->Start ].N[2];
               Pf[0] = P_Vertex[ Edge->End ].N[0];
               Pf[1] = P_Vertex[ Edge->End ].N[1];
               Pf[2] = P_Vertex[ Edge->End ].N[2];
               Emit_3D_Line( Pi, Pf );

               Edge->State |= EDGE_STATE_DRAWN;
            }
         }
      }
   }
   return( RENDER_NONE );
}

EXTERN RENDERING_FUNCTION Shader_Draw_Visible_Lines( )
{
   INT i;

   for( i=0; i<Orig_Poly->Nb_Pts; ++i )
   {
      MSH_EDGE *Edge;

      Edge = &Cur_Msh->Edges[ Orig_Poly->Edges[i] ];
      if ( Edge->State & EDGE_STATE_SEEN )
      {
//         if ( (Edge->Type&(EDGE_TYPE_FLAT|EDGE_TYPE_SHARP))==EDGE_TYPE_SHARP )
         {
            if ( Edge->Poly2 != 0xFFFF ) continue;
#if 0
            {
               PIXEL T1, T2;
               T1 = Cur_Msh->Polys[Edge->Poly1].Clipped & FLAG_POLY_VISIBLE;
               T2 = Cur_Msh->Polys[Edge->Poly2].Clipped & FLAG_POLY_VISIBLE;
               if ( (T1==FLAG_POLY_VISIBLE) && 
                    (T2==FLAG_POLY_VISIBLE) ) 
                   continue; // both poly seen
            }
#endif
//            if ( !( Edge->State & EDGE_STATE_DRAWN ) )
            {
               VECTOR Pi, Pf;
               Pi[0] = P_Vertex[ Edge->Start ].N[0];
               Pi[1] = P_Vertex[ Edge->Start ].N[1];
               Pi[2] = P_Vertex[ Edge->Start ].N[2];
               Pf[0] = P_Vertex[ Edge->End ].N[0];
               Pf[1] = P_Vertex[ Edge->End ].N[1];
               Pf[2] = P_Vertex[ Edge->End ].N[2];
               Emit_3D_Line( Pi, Pf );

               Edge->State |= EDGE_STATE_DRAWN;
            }
         }
      }
   }
   return( RENDER_NONE );
}

/******************************************************************/

EXTERN void Transform_Outlined_Vertices( MATRIX2 M )
{
   INT i;
   if ( Cur_Msh->Edges!=NULL )
      for( i=0; i<Cur_Msh->Nb_Edges; ++i )
         Cur_Msh->Edges[i].State = EDGE_STATE_VOID;

   Transform_Vertices_I( M );
}

EXTERN void Shader_Shade_Edges( MATRIX2 M )
{
   INT i;
   for( i=0; i<Orig_Poly->Nb_Pts; ++i )
   {
      Cur_Msh->Edges[ Orig_Poly->Edges[i] ].State |= EDGE_STATE_SEEN;
   }
}

EXTERN SHADER_METHODS Flat_Lighten_Anti_Shader =
{
   Set_Param_Flat,
   Transform_Outlined_Vertices,
   Split_Raw,
   Shader_Shade_Edges,
   Shader_Deal_Lighten_Flat, Shader_Draw_Visible_Lines, NULL,
   NULL
};

EXTERN SHADER_METHODS Flat_Anti_Shader =
{
   Set_Param_Flat,
   Transform_Outlined_Vertices,
   Split_Raw,
   Shader_Shade_Edges,
   Shader_Deal_Flat, Shader_Draw_Lines, NULL,
   NULL
};

EXTERN SHADER_METHODS Raw_Line_Shader =
{
   Set_Param_Flat,
   NULL,
   Split_Raw,
   NULL,
   Shader_Deal_Flat, Shader_Raw_Lines, NULL,
   NULL
};

EXTERN SHADER_METHODS UVc_Line_Shader =
{
   Set_Param_UV,
   NULL,
   Split_UVc,
   NULL,
   Shader_Deal_UVc, Shader_Raw_Lines, NULL,
   NULL
};

EXTERN SHADER_METHODS Flat_All_Anti_Shader =
{
   Set_Param_Flat,
   Transform_Outlined_Vertices,
   Split_Raw,
   Shader_Shade_Edges,
   Shader_Deal_Flat, Shader_Draw_All_Lines, NULL,
   NULL
};

EXTERN SHADER_METHODS UV_Anti_Shader =
{
   Set_Param_UV,
   Transform_Outlined_Vertices,
   Split_UVc,
   Shader_Shade_Edges,
   Shader_Deal_UVc, Shader_Draw_Lines, NULL,
   NULL
};

EXTERN SHADER_METHODS UV_zBuf_Anti_Shader =
{
   Set_Param_UV,
   Transform_Outlined_Vertices,
   Split_UVc,
   Shader_Shade_Edges,
   Shader_Deal_UVc, Shader_Draw_Lines, Shader_Deal_zBuf,
   NULL
};

EXTERN SHADER_METHODS Only_Line_Shader =
{
   Set_Param_Flat,
   Transform_Outlined_Vertices,
   Split_Raw,
   Shader_Shade_Edges,
   Shader_Draw_Lines, NULL, NULL,
   NULL
};
EXTERN SHADER_METHODS Only_All_Line_Shader =
{
   Set_Param_Flat,
   Transform_Outlined_Vertices,
   Split_Raw,
   Shader_Shade_Edges,
   Shader_Draw_All_Lines, NULL, NULL,
   NULL
};

/******************************************************************/

