/******************************************************************/
/*            Sub-pixel corrected UV-mapping                      */
/******************************************************************/

#include "main3d.h"

#ifdef DO_MAP_STATS
EXTERN INT St_Lin=0, St_64p=0, St_32p=0, St_16p=0;
EXTERN INT St_8p=0, St_4p=0, St_2p=0;
#endif

/******************************************************************/

#ifdef DO_MAP_STATS
EXTERN void P_St_Stats( )
{
   INT Total;
   FLT F;

   Total = St_Lin + St_32p + St_64p + St_16p + St_8p + St_4p + St_2p;
   if (Total==0) return;
   F = 100.0/(FLT)(Total);
   fprintf( stderr, "Total: %d Lin:%.1f%% 64p:%.1f%% 32p:%.1f%% 16p:%.1f%% 8p:%.1f%% 4p:%.1f%% 2p:%.1f%%\n",
      Total, F*St_Lin, F*St_64p, F*St_32p, F*St_16p, F*St_8p, F*St_4p, F*St_2p );
}
#endif

/******************************************************************/

EXTERN void Shader_Shade_UVc_Lighten( MATRIX2 M )
{
   VECTOR N, Pt;
   FLT A;

   nA_V( N, M, (FLT *)( Orig_Poly->No ) );

      /* Cur_P_Vertex->N contains projected vertex */

   Sub_Vector( Pt, Cur_P_Vertex->N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( Norm_Squared( Pt )*Norm_Squared( N ) );
   A *= Dot_Product( Pt, N );
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Cur_P_Vertex->N[0] = (A+Gouraud_Offset)*Gouraud_Amp;   /* Store light value in N[0] */
}

EXTERN void Shader_Shade_UVc_Gouraud( MATRIX2 M )
{
   VECTOR N, Pt;
   FLT A;

   nA_V( N, M, (FLT *)( Cur_Msh->Normals+Cur_Vtx_Nb ) );

      /* Cur_P_Vertex->N contains projected vertex */

   Sub_Vector( Pt, Cur_P_Vertex->N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( Norm_Squared( Pt )*Norm_Squared( N ) );
   A *= Dot_Product( Pt, N );
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Cur_P_Vertex->N[0] = (A+Gouraud_Offset)*Gouraud_Amp;   /* Store light value in N[0] */
}

EXTERN void Shader_Shade_UVc_Gouraud2( MATRIX2 M )
{
   VECTOR Pt;
   FLT A;

//   nA_V( N, M, (FLT *)( Cur_Msh->Normals+Cur_Vtx_Nb ) );

      /* Cur_P_Vertex->N contains projected vertex */

   Sub_Vector( Pt, Cur_P_Vertex->N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( Norm_Squared( Pt )*Norm_Squared( Cur_P_Poly->N ) );
   A *= Dot_Product( Pt, Cur_P_Poly->N );
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Cur_P_Vertex->N[0] = (A+Gouraud_Offset)*Gouraud_Amp;   /* Store light value in N[0] */
}

/******************************************************************/
/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_UVc( )
{
   FLT R1, R2;

   if ( fabs(Area)>AREA_EPSILON2 ) 
   {
      Flat_Color = Cache_Methods.Poly_Flat_Color( );
      goto Skip1;
   }

   if ( UV_Src==NULL )
      if ( Cache_Methods.Compute_MipMap( ) )
      {
Skip1:
         Constants_Computed = TRUE;
         return( RENDER_FLAT_I );
      }
//   fprintf( stderr, "UV_Src = 0x%x,   Slot=%d    Mip=%d\n",
//      UV_Src, Cur_Mip_Slot, Cur_Mip );

#if 0
   if ( (fabs(Area)>AREA_EPSILON) || (Cur_Mip>4) || (aZ1<UVc_EPSILON1) )
   {
      ADD_STAT( St_Lin++ );
      Constants_Computed = TRUE;
      return( Shader_Deal_UV( ) );
   }
#endif

   diZ1 = P_Edges[1]->Inv_Z - P_Edges[0]->Inv_Z;
   R1 = diZ1 * P_Edges[0]->N[2];    //  <= Alpha = (zo-z1)/z1
   diZ2 = P_Edges[2]->Inv_Z - P_Edges[1]->Inv_Z;
   R2 = diZ2 * P_Edges[1]->N[2];    //  <= Alpha = (z1-z2)/z2

   Area2 = Area * aZ2;
   DxiZ = ( diZ2*dx0 - diZ1*dx1 )*Area2;
   DyiZ = ( diZ1*dy1 - diZ2*dy0 )*Area2;

   Area2 *= Mip_Scale;

   dU1 = ( R1*P_Edges[1]->UV[0] + dU1 ) *  P_Edges[0]->Inv_Z;
   dU2 = ( R2*P_Edges[2]->UV[0] + dU2 ) *  P_Edges[1]->Inv_Z;
   DxU = ( dU2*dx0 - dU1*dx1 )*Area2;
   DxU += DxiZ*Uo_Mip;
   DyU = ( dU1*dy1 - dU2*dy0 )*Area2;
   DyU += DyiZ*Uo_Mip;

   dV1 = ( R1*P_Edges[1]->UV[1] + dV1 ) *  P_Edges[0]->Inv_Z;
   dV2 = ( R2*P_Edges[2]->UV[1] + dV2 ) *  P_Edges[1]->Inv_Z;
   DxV = ( dV2*dx0 - dV1*dx1 )*Area2;
   DxV += DxiZ*Vo_Mip;
   DyV = ( dV1*dy1 - dV2*dy0 )*Area2;
   DyV += DyiZ*Vo_Mip;

   Rasterize_UVc( );

//   return( RENDER_UVC_32 );

   if ( aZ1<UVc_EPSILON_LIN )
   {
      ADD_STAT( St_Lin++ );
      return( RENDER_UVC );
   }
      // correction every 64/32/16/8/4 or 2 pixels

#if 0 // DISABLE for now, becoz of pb in loopt_16.asm...
   else if ( aZ1<UVc_EPSILON64 )  
   {
      ADD_STAT( St_64p++ );
      return( RENDER_UVC_64 );
   }
#endif
   else if ( aZ1<UVc_EPSILON32 )
   {
      ADD_STAT( St_32p++ );
      return( RENDER_UVC_32 );
   }
   else if ( aZ1<UVc_EPSILON16 )
   {
      ADD_STAT( St_16p++ );
      return( RENDER_UVC_16 );
   }
   else if ( aZ1<UVc_EPSILON8 )
   {
      ADD_STAT( St_8p++ );
      return( RENDER_UVC_8 );
   }
   else if ( aZ1<UVc_EPSILON4 )
   {
      ADD_STAT( St_4p++ );
      return( RENDER_UVC_4 );
   }
   else 
   {
      ADD_STAT( St_2p++ );
      return( RENDER_UVC_2 );
   }
}

/******************************************************************/
/******************************************************************/

EXTERN SHADER_METHODS UVc_Shader =
{
   Set_Param_UV,
   NULL,
   Split_UVc,    // <= will split N[2] (=Z) too!
   NULL, // Shader_Shade_UV,
   Shader_Deal_UVc, NULL, NULL,
   NULL
};

EXTERN SHADER_METHODS UVc_Gouraud =
{
   Set_Param_UV,
   NULL,
   Split_UVc_Gouraud,    // <= will split N[2] (=Z) and N[0] !!
   Shader_Shade_UVc_Gouraud,  // store gouraud in N[0]. NOT N[2]!!
   Shader_Deal_UVc, Shader_Deal_Gouraud, NULL,
   NULL
};

EXTERN SHADER_METHODS UVc_Gouraud2 =
{
   Set_Param_UV,
   NULL,
   Split_UVc_Gouraud,    // <= will split N[2] (=Z) and N[0] !!
   Shader_Shade_UVc_Gouraud2,  // store gouraud in N[0]. NOT N[2]!!
   Shader_Deal_UVc, Shader_Deal_Gouraud, NULL,
   NULL
};

EXTERN SHADER_METHODS UVc_Lighten_Shader =
{
   Set_Param_UV,
   NULL,
   Split_UVc,
   Shader_Shade_UVc_Lighten,
   Shader_Deal_UVc, Shader_Deal_Lighten_Flat2, NULL,
   NULL
};

/******************************************************************/
