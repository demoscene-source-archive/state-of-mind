/******************************************************************/
/*                      FLAT                                      */
/******************************************************************/

#include "main3d.h"

/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_Lighten_Flat( )
{
   FLT A;
   INT i;
   VECTOR N, Pt;

//   nA_V( N, Cur_Node->Mo, (FLT *)( Cur_P_Poly->N ) );
   i = Orig_Poly->Pt[0];
   Sub_Vector( Pt, P_Vertex[i].N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( Norm_Squared( Pt )*Norm_Squared( Cur_P_Poly->N ) );
   A *= Dot_Product( Pt, Cur_P_Poly->N );
//   A = -.5f/(FLT)sqrt( Norm_Squared( Pt )*Norm_Squared( N ) );
//   A *= Dot_Product( Pt, N );
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Flat_Color = (A+Gouraud_Offset)*Gouraud_Amp/65536.0f;   // light value in N[0]

   return( RENDER_FLAT_I );
}

EXTERN RENDERING_FUNCTION Shader_Deal_Lighten_Flat2( )
{
   FLT A;
   INT i;
   VECTOR Pt;

   i = Orig_Poly->Pt[0];
   Sub_Vector( Pt, P_Vertex[i].N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( (double)Norm_Squared( Pt ) );
   A *= Dot_Product( Pt, Cur_P_Poly->N );
   if ( A<GOURAUD_EPSILON ) A = GOURAUD_EPSILON;
   Flat_Color = (A+Gouraud_Offset)*Gouraud_Amp/65536.0f;   // light value in N[0]

   return( RENDER_FLAT_II );
}

EXTERN RENDERING_FUNCTION Shader_Deal_Flat3( )
{
   FLT A;
   INT i;
   VECTOR Pt;

   i = Orig_Poly->Pt[0];
   Sub_Vector( Pt, P_Vertex[i].N, Cur_Light->P );
   A = -.5f/(FLT)sqrt( (double)Norm_Squared( Pt ) );
   A *= Dot_Product( Pt, Cur_P_Poly->N );
   if ( A<GOURAUD_EPSILON ) Flat_Color = (INT)( Gouraud_Base*256.0f );
   else Flat_Color = // (INT)( Gouraud_Offset*256.0f );
      (A+Gouraud_Offset)*Gouraud_Amp/65536.0f;   // light value in N[0]

   return( RENDER_FLAT_II );
}


EXTERN RENDERING_FUNCTION Shader_Deal_Flat( )
{
   Flat_Color = Orig_Poly->Colors;
//   Flat_Color2 = Orig_Poly->Colors >>8;

   return( RENDER_FLAT_I );
}
EXTERN RENDERING_FUNCTION Shader_Deal_Flat2( )
{
   Flat_Color = Orig_Poly->Colors & 0xFF;
//   Flat_Color2 = Orig_Poly->Colors >>8;

   return( RENDER_FLAT_II );
}

/******************************************************************/
/******************************************************************/

EXTERN void Set_Param_Flat( void *Obj, ... )
{
   MESH *Msh;
   va_list Arg_List;
   USHORT Colors;
   INT i;

   Msh = (MESH *)Obj;
   va_start( Arg_List, Obj );
//   Colors = va_arg( Arg_List, USHORT );
   Colors = va_arg( Arg_List, int );
   for( i=0; i<Msh->Nb_Polys; ++i )
   {
      Msh->Polys[i].Colors = Colors;
      Msh->Polys[i].Flags &= ~POLY_MAP_UV;
   }
   va_end( Arg_List );
}

/******************************************************************/
/******************************************************************/

EXTERN SHADER_METHODS Flat_Shader =
{
   Set_Param_Flat,   /* Set parameters */
   NULL, /* transform */
   Split_Raw,
   NULL, /* Shaders setup */
   Shader_Deal_Flat, NULL, NULL,
   NULL
};

EXTERN SHADER_METHODS Flat_Lighten_Shader =
{
   Set_Param_Flat,
   NULL,
   Split_Raw,
   NULL,
   Shader_Deal_Lighten_Flat, NULL, NULL,
   NULL
};

EXTERN SHADER_METHODS UVl_Shader =
{
   Set_Param_UV,
   NULL,
   Split_Raw,
   Shader_Shade_UVl,
   Shader_Deal_UV, Shader_Deal_Lighten_Flat, NULL,
   NULL
};

/******************************************************************/
/******************************************************************/


