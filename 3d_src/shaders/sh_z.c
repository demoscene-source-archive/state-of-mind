/******************************************************************/
/*                     ZBuffer                                    */
/***Skal97*********************************************************/

#include "main3d.h"

/******************************************************************/
/*          1/z-Gouraud setup in screen space                     */
/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_zBuf( )
{
   FLT dS1, dS2;
   if ( Area>AREA_EPSILON2 ) goto Skip;

   dS1 = ( P_Edges[1]->Inv_Z - P_Edges[0]->Inv_Z ) * _RCst_.Z_Scale;
   dS2 = ( P_Edges[2]->Inv_Z - P_Edges[1]->Inv_Z ) * _RCst_.Z_Scale;

   DyS = ( dS1*dy1 - dS2*dy0 )*Area;
   DxS = ( dS2*dx0 - dS1*dx1 )*Area;

   Rasterize_iZ( );

   return( RENDER_ZBUF );

Skip:
   dS1 = P_Edges[0]->Inv_Z*_RCst_.Z_Scale + _RCst_.Z_Off;
   Flat_Color = (USHORT)( (INT)( dS1 )>>16 );
   return( RENDER_FLAT_ZBUF );
}

/******************************************************************/
/******************************************************************/

EXTERN void Set_Param_UV_zBuf( void *Obj, ... )
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
         if ( Msh->Polys[i].Ptr == NULL )
            Msh->Polys[i].Ptr = (void *)Mat;
      Msh->Polys[i].Flags |= POLY_MAP_UV;
   }
   va_end( Arg_List );
}

/******************************************************************/
/******************************************************************/

EXTERN SHADER_METHODS zBuf_Shader =
{
   Set_Param_Flat,
   NULL,
   Split_Raw,
   NULL,
   Shader_Deal_zBuf, NULL, NULL,
   NULL
};

EXTERN SHADER_METHODS UVc_zBuf_Shader =
{
   Set_Param_UV_zBuf,
   NULL,
   Split_UVc,  
   NULL, 
   Shader_Deal_UVc, Shader_Deal_zBuf, NULL,
   NULL
};

/******************************************************************/
/******************************************************************/

EXTERN void Shader_Shade_UV_Gouraud_zBuf( MATRIX2 M )
{
   VECTOR N, Pt; FLT A;

   nA_V( N, M, (FLT *)( Cur_Msh->Normals+Cur_Vtx_Nb ) );

      /* Cur_P_Vertex->N contains projected vertex */

   Sub_Vector( Pt, Cur_P_Vertex->N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( Norm_Squared( Pt )*Norm_Squared( N ) );
   A *= Dot_Product( Pt, N );

   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Cur_P_Vertex->N[0] = (A+Gouraud_Offset)*Gouraud_Amp;   // Store light value in N[0]
}


EXTERN SHADER_METHODS UV_Gouraud_zBuf_Shader =
{
   Set_Param_UV_zBuf,
   NULL,
   Split_UV_Gouraud,  
   Shader_Shade_UV_Gouraud_zBuf,
   Shader_Deal_UV, Shader_Deal_Gouraud, Shader_Deal_zBuf,
   NULL
};

EXTERN SHADER_METHODS UVc_Gouraud_zBuf_Shader =
{
   Set_Param_UV_zBuf,
   NULL,
   Split_UVc_Gouraud,
   Shader_Shade_UV_Gouraud_zBuf,
   Shader_Deal_UVc, Shader_Deal_Gouraud, Shader_Deal_zBuf,
   NULL
};

/******************************************************************/

EXTERN void Shader_Shade_UV_Gouraud2_zBuf( MATRIX2 M )
{
   VECTOR Pt; FLT A;

      /* Cur_P_Vertex->N contains projected vertex */

   Sub_Vector( Pt, Cur_P_Vertex->N, Cur_Light->P );
   // A = -.5/(FLT)sqrt( (double)Norm_Squared( Pt ) );
   A = -20.0f/(FLT)Norm_Squared( Pt );
   A *= Dot_Product( Pt, Cur_P_Poly->N );

   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Cur_P_Vertex->N[0] = (A+Gouraud_Offset)*Gouraud_Amp;   // Store light value in N[0]
}


EXTERN SHADER_METHODS UVc_Flat3_zBuf_Shader =
{
   Set_Param_UV_zBuf,
   NULL,
   Split_UVc_Gouraud,
   NULL, // Shader_Shade_UV,
   Shader_Deal_UVc, Shader_Deal_Flat3, Shader_Deal_zBuf,
   NULL
};

EXTERN SHADER_METHODS UVc_Gouraud2_zBuf_Shader =
{
   Set_Param_UV_zBuf,
   NULL,
   Split_UVc_Gouraud,
   NULL, // Shader_Shade_UV_Gouraud2_zBuf,
   Shader_Deal_UVc, Shader_Deal_Gouraud, Shader_Deal_zBuf,
   NULL
};

/******************************************************************/

EXTERN void Shader_Shade_UV_Lighten_zBuf( MATRIX2 M )
{
}

EXTERN SHADER_METHODS UVc_Lighten_zBuf_Shader =
{
   Set_Param_UV_zBuf,
   NULL,
   Split_UVc_Gouraud,
   NULL, // Shader_Shade_UV_Lighten_zBuf,
   Shader_Deal_UVc, 
   Shader_Deal_Lighten_Flat2, // Shader_Deal_Gouraud,    
   Shader_Deal_zBuf,
   NULL
};

/******************************************************************/

EXTERN void Shader_Shade_UV_Env_Gouraud_zBuf( MATRIX2 M )
{
   VECTOR N, Pt; FLT A;

   nA_V( N, M, (FLT *)( Cur_Msh->Normals+Cur_Vtx_Nb ) );

      /* Cur_P_Vertex->N contains projected vertex */

   Sub_Vector( Pt, Cur_P_Vertex->N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( Norm_Squared( Pt )*Norm_Squared( N ) );
   A *= Dot_Product( Pt, N );

   
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Cur_P_Vertex->N[0] = (A+Gouraud_Offset)*Gouraud_Amp;   /* Store light value in N[0] */

//   Shader_Shade_UV( M, Vtx, P );
}

EXTERN SHADER_METHODS UV_Env_Gouraud_zBuf =
{
   Set_Param_UV,
   NULL,
   Split_UV_Gouraud,
   Shader_Shade_UV_Env_Gouraud_zBuf,
   Shader_Deal_UV, Shader_Deal_Gouraud, Shader_Deal_zBuf,
   NULL
};

/******************************************************************/
/******************************************************************/
