/******************************************************************/
/*                      Env-Mapping                               */
/******************************************************************/

#include "main3d.h"

/******************************************************************/

EXTERN void Shader_Env_Mapper( MATRIX2 M )
{
   VECTOR N;

   nA_V( N, M, Cur_Msh->Normals[Cur_Vtx_Nb] );
   Normalize_Safe( N );

   Cur_P_Vertex->UV[0] = .5f*( 1.0f + N[0] );
   Cur_P_Vertex->UV[1] = .5f*( 1.0f + N[1] );
}

/******************************************************************/
/*                   Fake phong env-mapping (1rst pass)           */
/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_UV_Env( )
{
   if ( fabs(Area)>AREA_EPSILON2 ) goto Skip1;
   if ( Constants_Computed ) goto Skip_UV;

   if ( UV_Src==NULL )
      if ( Cache_Methods.Compute_MipMap_Env( ) )
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

/******************************************************************/

EXTERN void Set_Param_UV_Env( void *Obj, ... )
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
      // if ( Msh->Polys[i].Ptr==NULL )
         Msh->Polys[i].Ptr = (void *)Mat;
      Msh->Polys[i].Flags &= ~POLY_MAP_UV; // <= important
   }
   va_end( Arg_List );
}

/******************************************************************/

EXTERN SHADER_METHODS Env_Shader =
{
   Set_Param_UV_Env,
   NULL,
   Split_UV,
   Shader_Env_Mapper,
   Shader_Deal_UV_Env, NULL, NULL,
   NULL
};

/******************************************************************/
/*                   Fake phong env-mapping (2nd pass)            */
/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_UV_Env2( )
{
   if ( fabs(Area)>AREA_EPSILON2 ) 
      goto Skip1;

   if ( Constants_Computed ) goto Constants_Ok;

   dU1 = ( P_Edges[1]->N[0]-P_Edges[0]->N[0] );
   dU2 = ( P_Edges[2]->N[0]-P_Edges[1]->N[0] );

   dV1 = ( P_Edges[1]->N[1]-P_Edges[0]->N[1] );
   dV2 = ( P_Edges[2]->N[1]-P_Edges[1]->N[1] );


   if ( UV_Src==NULL )
      if ( Cache_Methods.Compute_MipMap_Env( ) )
      {
Skip1:
         Constants_Computed = TRUE;
         return( RENDER_FLAT_II );
      }

   Area2 = Area * Mip_Scale;

   DyU = ( dU1*dy1-dU2*dy0 )*Area2;
   dyU = (INT)DyU;
   DyV = ( dV1*dy1-dV2*dy0 )*Area2;
   dyV = (INT)DyV;
   DxU = ( dU2*dx0 - dU1*dx1 )*Area2;
   DxV = ( dV2*dx0 - dV1*dx1 )*Area2;

Constants_Ok:

   Rasterize_UV2( );

   return( RENDER_UV2 );
}


EXTERN void Shader_Env_Mapper2( MATRIX2 M )
{
   VECTOR N;

   nA_V( N, M, (FLT *)( Cur_Msh->Normals+Cur_Vtx_Nb ) );
   Normalize_Safe( N );

   Cur_P_Vertex->N[0] = .5f*( 1.0f + N[0] );
   Cur_P_Vertex->N[1] = .5f*( 1.0f + N[1] );
}

/******************************************************************/

EXTERN SHADER_METHODS UV_Env_Shader =
{
   Set_Param_UV,     // <= *not* Set_Param_UV_Env()
   NULL,
   Split_Shadow_UV,
   Shader_Env_Mapper2,
   Shader_Deal_UV_Env, Shader_Deal_UV_Env2, NULL,
   NULL
};

/******************************************************************/
