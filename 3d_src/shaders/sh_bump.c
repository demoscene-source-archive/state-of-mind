/******************************************************************/
/*                     Bump UV-mapping                            */
/******************************************************************/

#include "main3d.h"

/******************************************************************/

EXTERN void Shader_Shade_Bump( MATRIX2 M )
{
   VECTOR N;

   nA_V( N, M, (FLT *)( Cur_Msh->Normals+Cur_Vtx_Nb ) );
   Normalize_Safe( N );
   Cur_P_Vertex->N[0] = .5f*( 1.0f + N[0] );
   Cur_P_Vertex->N[1] = .5f*( 1.0f + N[1] );
}

/******************************************************************/

EXTERN void Set_Param_Bump( void *Obj, ... )
{
   MESH *Msh;
   va_list Arg_List;
   MATERIAL *Mat;
   INT i;

   Msh = (MESH *)Obj;
   va_start( Arg_List, Obj );
   Mat = va_arg( Arg_List, MATERIAL * );
   for( i=0; i<Msh->Nb_Polys; ++i )
   {
      if ( Mat!=NULL )
         if ( Msh->Polys[i].Ptr==NULL )
            Msh->Polys[i].Ptr = (void *)Mat;
      Msh->Polys[i].Flags |= POLY_MAP_UV;
   }
   va_end( Arg_List );
}

/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_Bump1( )
{
      // First pass: bump offset. 16bits
   UV_Src = Cur_Material->Txt1->Ptr;
   Cur_Mip_Slot = Cur_Material->Txt1->Slot;
   Shader_Deal_UV( );
   return( RENDER_UV_16b );
}

EXTERN RENDERING_FUNCTION Shader_Deal_Bump2( )
{
      // 2nd pass: Fake phong + previous offset
   UV_Src = Cur_Material->Txt2->Ptr;
   Cur_Mip_Slot = Cur_Material->Txt2->Slot;
   Shader_Deal_UV_Env2( );
   return( RENDER_UV_BUMP );
}

EXTERN RENDERING_FUNCTION Shader_Deal_Bump3( )
{
      // 3rd pass: UV mapping
   UV_Src = Cur_Material->Txt3->Ptr;
   Cur_Mip_Slot = Cur_Material->Txt3->Slot;
   Shader_Deal_UV( );
   return( RENDER_UV );
}

EXTERN SHADER_METHODS Bump_Shader =
{
   Set_Param_Bump,
   NULL,
   Split_UV,
   Shader_Shade_Bump,
   Shader_Deal_Bump1, Shader_Deal_Bump2, Shader_Deal_Bump3,
   NULL
};

/******************************************************************/


EXTERN RENDERING_FUNCTION Shader_Deal_Bump4( )
{
      // 2nd pass: bump offset
   Constants_Computed = FALSE;
   (*Cache_Methods.Init_From_Poly_II)( );
   if ( Shader_Deal_UV( )==RENDER_UV )
      return( RENDER_UV_BUMP_II );
   else return( RENDER_FLAT_I );
}

EXTERN RENDERING_FUNCTION Shader_Deal_Bump5( )
{
      // 1st pass: Fake phong offset

   dU1 = ( P_Edges[1]->N[0]-P_Edges[0]->N[0] );
   dU2 = ( P_Edges[2]->N[0]-P_Edges[1]->N[0] );

   dV1 = ( P_Edges[1]->N[1]-P_Edges[0]->N[1] );
   dV2 = ( P_Edges[2]->N[1]-P_Edges[1]->N[1] );

   Mip_Scale = 65536.0*256.0;
   Area2 = Area * Mip_Scale;
   Uo_Mip = Vo_Mip = 0.0;

   DyU = ( dU1*dy1-dU2*dy0 )*Area2;
   dyU = ( INT )DyU;
   DyV = ( dV1*dy1-dV2*dy0 )*Area2;
   dyV = ( INT )DyV;
   DxU = ( dU2*dx0 - dU1*dx1 )*Area2;
   DxV = ( dV2*dx0 - dV1*dx1 )*Area2;

   Rasterize_UV2( );

   return( RENDER_UV_OFFSET );
}

EXTERN SHADER_METHODS Bump_Shader2 =
{
   Set_Param_Bump,
   NULL,
   Split_UV,
   Shader_Shade_Bump,
   Shader_Deal_Bump5, Shader_Deal_Bump4, Shader_Deal_Bump3,
   NULL
};

/******************************************************************/
