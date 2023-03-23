/******************************************************************/
/*                         GOURAUD                                */
/**Skal97**********************************************************/

#include "main3d.h"

EXTERN FLT Gouraud_Offset = .5f;
EXTERN FLT Gouraud_Base = .1f;
EXTERN FLT Gouraud_Amp = GOURAUD_SCALE;
EXTERN VECTOR Total_Dir;
EXTERN VECTOR Total_Pos;

/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_Gouraud( )
{
   if ( Area>AREA_EPSILON2 ) goto Skip;

   if ( Constants_Computed ) goto Constants_Ok;

   {
      dS1 = ( P_Edges[1]->N[0] - P_Edges[0]->N[0] );
      dS2 = ( P_Edges[2]->N[0] - P_Edges[1]->N[0] );

      DyS = ( dS1*dy1 - dS2*dy0 )*Area;
      DxS = ( dS2*dx0 - dS1*dx1 )*Area;
   }

Constants_Ok:

   Rasterize_Grd( );

   if ( (DyS<GOURAUD_EPSILON2) && (DyS>-GOURAUD_EPSILON2) ) 
      return( RENDER_GOURAUD_RAMP );
   else return( RENDER_GOURAUD );

Skip:
   Flat_Color = (USHORT)( P_Edges[0]->N[0]/65536.0 );
   return( RENDER_FLAT_I );
}

EXTERN void Shader_Shade_Gouraud( MATRIX2 M )
{
   FLT A;
   VECTOR N, Pt;

   nA_V( N, M, (FLT *)( Cur_Msh->Normals+Cur_Vtx_Nb ) ); // N isn't normalized!

      /* Cur_P_Vertex[].N contains projected vertex */

   Sub_Vector( Pt, Cur_P_Vertex->N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( Norm_Squared( Pt )*Norm_Squared( N ) );
   A *= Dot_Product( Pt, N );
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Cur_P_Vertex->N[0] = (A+Gouraud_Offset)*Gouraud_Amp;   // light value in N[0]

#if 0
   A = -.5*Dot_Product( Cur_Light->D, N );
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Cur_P_Vertex->N[0] = (A+Gouraud_Offset)*Gouraud_Amp;   // light value in N[0]
#endif
}

/******************************************************************/

EXTERN SHADER_METHODS Gouraud_Shader =
{
   Set_Param_Flat,
   NULL,
   Split_Gouraud,
   Shader_Shade_Gouraud,
   Shader_Deal_Gouraud, NULL, NULL,
   NULL
};


EXTERN SHADER_METHODS UVg_Shader =
{
   Set_Param_UV,
   NULL,
   Split_UV_Gouraud,
   Shader_Shade_UV_Gouraud,
   Shader_Deal_UV, Shader_Deal_Gouraud, NULL,
   NULL
};

/******************************************************************/

EXTERN void Shader_Shade_UV_Env_Gouraud( MATRIX2 M )
{
   VECTOR N, Pt; 
   FLT A;

   nA_V( N, M, (FLT *)( Cur_Msh->Normals+Cur_Vtx_Nb ) );
   Normalize_Safe( N );
   Cur_P_Vertex->UV[0] = .5f + N[0]/2.0f;
   Cur_P_Vertex->UV[1] = .5f + N[1]/2.0f;

      /* Cur_P_Vertex[].N contains projected vertex */

   Sub_Vector( Pt, Cur_P_Vertex->N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( (double)Norm_Squared( Pt ) );
   A *= Dot_Product( Pt, N );
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Cur_P_Vertex->N[0] = (A+Gouraud_Offset)*Gouraud_Amp;   // light value in N[0]
}

EXTERN SHADER_METHODS UV_Env_Gouraud_Shader =
{
   Set_Param_UV,
   NULL,
   Split_UV_Gouraud,
   Shader_Shade_UV_Env_Gouraud,
   Shader_Deal_UV, Shader_Deal_Gouraud, NULL,
   NULL
};

/******************************************************************/
