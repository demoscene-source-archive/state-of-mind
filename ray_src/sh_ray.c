/***********************************************
 *          mesh intersections funcs           *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "ray.h"

/******************************************************************/

/******************************************************************/

EXTERN void Shade_Ray( MATRIX2 M ) { }

EXTERN RENDERING_FUNCTION Deal_Ray( )
{
   FLT R1, R2;

   if ( fabs(Area)>AREA_EPSILON2 ) 
   {
      Flat_Color = Cache_Methods.Poly_Flat_Color( );
      goto Skip1;
   }

   if ( Cache_Methods.Compute_MipMap( ) )
   {
Skip1:
      Constants_Computed = TRUE;
      return( RENDER_FLAT_I );
   }

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

   return( RENDER_USER1 );
}

/******************************************************************/

EXTERN void Set_Param_Ray( void *Obj, ... )
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

EXTERN void Split_Ray(
   FLT x, P_VERTICE *From, P_VERTICE *To, P_VERTICE *Result )
{
   FLT Tmp, x1, x2;

   Result->Inv_Z = From->Inv_Z + x*( To->Inv_Z-From->Inv_Z );
   Tmp = Result->N[2] = 1.0f/Result->Inv_Z;
   x2 = To->UV[0]*To->Inv_Z;
   x1 = From->UV[0]*From->Inv_Z;
   Result->UV[0] = ( x1 + x*( x2-x1 ) ) * Tmp;
   x2 = To->UV[1]*To->Inv_Z;
   x1 = From->UV[1]*From->Inv_Z;
   Result->UV[1] = ( x1 + x*( x2-x1 ) ) * Tmp;

   Result->N[0] = From->N[0] + x*( To->N[0] - From->N[0] );
   Result->xp = From->xp + x*( To->xp - From->xp );
   Result->yp = From->yp + x*( To->yp - From->yp );
}

/******************************************************************/

EXTERN SHADER_METHODS Ray_Shader = 
{
   Set_Param_Ray,
   NULL,
   Split_Ray,
   Shade_Ray,
   Deal_Ray, NULL, NULL,
   NULL
};

/******************************************************************/
