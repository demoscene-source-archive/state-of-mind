/******************************************************************/
/*                      Sub-pixel UV-mapping                      */
/******************************************************************/

#include "main3d.h"

/******************************************************************/

EXTERN void Shader_Shade_UV( MATRIX2 M )
{
   return;
}

/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_UV( )
{
   if ( fabs(Area)>AREA_EPSILON2 ) 
      goto Skip1;

   if ( Constants_Computed ) goto Skip_UV;

#if 0
   dU1 = ( P_Edges[1]->UV[0]-P_Edges[0]->UV[0] );
   dU2 = ( P_Edges[2]->UV[0]-P_Edges[1]->UV[0] );

   dV1 = ( P_Edges[1]->UV[1]-P_Edges[0]->UV[1] );
   dV2 = ( P_Edges[2]->UV[1]-P_Edges[1]->UV[1] );
#endif

   if ( UV_Src==NULL )
      if ( Cache_Methods.Compute_MipMap( ) )
      {
Skip1:
         Constants_Computed = TRUE;
         return( RENDER_FLAT_I );
      }

Skip_UV:

   Area2 = Area * Mip_Scale;

   DyU = ( dU1*dy1 - dU2*dy0 )*Area2;
   dyU = (INT)DyU;
   DyV = ( dV1*dy1 - dV2*dy0 )*Area2;
   dyV = (INT)DyV;
   DxU = ( dU2*dx0 - dU1*dx1 )*Area2;
   DxV = ( dV2*dx0 - dV1*dx1 )*Area2;

   Rasterize_UV( );
   return( RENDER_UV );
}


EXTERN void Shader_Shade_UVl( MATRIX2 M )
{
   FLT A;
   VECTOR N, Pt;

   nA_V( N, M, (FLT *)( Orig_Poly->No ) );
   Sub_Vector( Pt, P_Vertex->N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( Norm_Squared( Pt )*Norm_Squared( N ) );
   A *= Dot_Product( Pt, N );
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Flat_Color = (A+Gouraud_Offset)*Gouraud_Amp/256.0f;   // light value in N[0]
}

EXTERN void Shader_Shade_UV_Gouraud( MATRIX2 M )
{
   VECTOR N, Pt;
   FLT A;

   nA_V( N, M, (FLT *)( Cur_Msh->Normals+Cur_Vtx_Nb ) );

      /* Cur_P_Vertex->N contains projected vertex */

   Sub_Vector( Pt, Cur_P_Vertex->N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( Norm_Squared( Pt )*Norm_Squared( N ) );
   A *= Dot_Product( Pt, N );
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Cur_P_Vertex->N[0] = (A+Gouraud_Offset)*Gouraud_Amp;   // light value in N[0]
}

/******************************************************************/

EXTERN void Set_Param_UV( void *Obj, ... )
{
   MESH *Msh;
   va_list Arg_List;
   MATERIAL *Mat;
   INT i;

   Msh = (MESH *)Obj;
   va_start( Arg_List, Obj );
   Mat = va_arg( Arg_List, MATERIAL * );
   for( i=0; i<Msh->Nb_Polys; ++i )          // XXX!!
   {
      if ( Mat!=NULL )
         if ( Msh->Polys[i].Ptr==NULL )
            Msh->Polys[i].Ptr = (void *)Mat;
      Msh->Polys[i].Flags |= POLY_MAP_UV;
   }
   va_end( Arg_List );
}

/******************************************************************/

EXTERN SHADER_METHODS UV_Shader =
{
   Set_Param_UV,
   NULL,
   Split_UV,
   NULL, // Shader_Shade_UV,
   Shader_Deal_UV, NULL, NULL,
   NULL
};


/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_UV_16b( )
{
      // Hack!!
   Shader_Deal_UV( );
   return( RENDER_UV_16b );
}

EXTERN SHADER_METHODS UV_16b_Shader =
{
   Set_Param_UV,
   NULL,
   Split_UV,
   NULL, // Shader_Shade_UV,
   Shader_Deal_UV_16b, NULL, NULL,
   NULL
};

/******************************************************************/

