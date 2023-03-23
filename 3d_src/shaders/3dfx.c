/******************************************************************/
/*                     3DFX interfacing                           */
/******************************************************************/

#include "main3d.h"

#ifdef USE_3DFX

#include <glide.h>

EXTERN GrHwConfiguration hwconfig;
EXTERN GrScreenResolution_t resolution = GR_RESOLUTION_640x480;
EXTERN GrVertex Vtx_3dfx[ MAX_EDGES+1+6 ];

/******************************************************************/

EXTERN RENDER_UV2 Shader_Deal_UV_3dfx( )
{
   INT i;
   i=Nb_Out_Edges-2;
   if ( i<2 ) return;
   for( ; i>=0; --i )
   {
      float Inv_Z;
      Vtx_3dfx[i].x = Out_Vtx[i]->xp;
      Vtx_3dfx[i].y = Out_Vtx[i]->xp;

      Inv_Z = Vtx_3dfx[i].oow = Out_Vtx[i]->Inv_Z;
      Vtx_3dfx[i].tmuvtx[0].oow = Inv_Z;
      Inv_Z *= 255.0;
      Vtx_3dfx[i].tmuvtx[0].sow = Out_Vtx[i]->UV[0]*Inv_Z;
      Vtx_3dfx[i].tmuvtx[0].tow = Out_Vtx[i]->UV[1]*Inv_Z;

      Vtx_3dfx[i].ooz = 256.0*Inv_Z;
   }
   grDrawPolygonVertexList( Nb_Out_Edges-1, Vtx_3dfx );
   return( RENDER_NONE );
}

/******************************************************************/

EXTERN INT Init_3dfx( )
{
   char version[80];

   grGlideGetVersion( version );

   if ( geteuid() )
   {
      Out_Message( "Init_3dfx error: Not suid root" );
      return( -1 );
   }

   grGlideInit();

   setuid( getuid() ); // Give up root privileges

   if ( grSstQueryHardware( &hwconfig ) == 0 ) return( -1 );
   grSstSelect( 0 );
   if ( grSstWinOpen( 0,
         resolution,
         GR_REFRESH_60Hz,
         GR_COLORFORMAT_ABGR,
         GR_ORIGIN_UPPER_LEFT, 2, 1 ) == 0 )
      return( -1 );

      // Plug new rendering functions

   UV_Shader.Deal_With_Poly = Shader_Deal_UV_3dfx;
   UVc_Shader.Deal_With_Poly = Shader_Deal_UV_3dfx;
   
   return( 0 );
}

EXTERN INT Refresh_3dfx( )
{
   grBufferClear( 0x00000000, 0, GR_WDEPTHVALUE_FARTHEST );
   grBufferSwap( 1 );
}

EXTERN INT Un_Init_3dfx( )
{
    grGlideShutdown();
}    

/******************************************************************/

#endif   // USE_3DFX
