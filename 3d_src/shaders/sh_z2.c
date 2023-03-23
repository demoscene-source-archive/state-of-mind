/******************************************************************/
/*                     ZBuffer + Obj_ID + Poly_ID                 */
/***Skal97*********************************************************/

#include "main3d.h"

/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_zBuf_ID( )
{
   if ( Area>AREA_EPSILON2 ) goto Skip;

   dS1 = ( P_Edges[1]->Inv_Z - P_Edges[0]->Inv_Z ) * _RCst_.Z_Scale;
   dS2 = ( P_Edges[2]->Inv_Z - P_Edges[1]->Inv_Z ) * _RCst_.Z_Scale;

   DyS = ( dS1*dy1 - dS2*dy0 )*Area;
   DxS = ( dS2*dx0 - dS1*dx1 )*Area;

   Rasterize_iZ( );

   return( RENDER_USER1 );

Skip:
   Flat_Color = (USHORT)( (INT)( P_Edges[0]->Inv_Z*_RCst_.Z_Scale )>>16 );
   return( RENDER_FLAT_ZBUF );
}

/******************************************************************/
/******************************************************************/

EXTERN SHADER_METHODS ID_zBuf_Shader =
{
   NULL,
   NULL,
   Split_UVc,  
   NULL, 
   Shader_Deal_zBuf_ID, NULL,
   NULL
};

/******************************************************************/
/******************************************************************/

