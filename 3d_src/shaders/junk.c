/***********************************************
 *               shader.c                      *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/
/******************************************************************/
/*                           UV                                   */
/**********************  Alpha-hack *******************************/
/******************************************************************/

EXTERN void Shader_Draw_UV_Alpha( )
{
#if 0
   INT y, dy;
   PIXEL *Dst;

   y = Scan_Start;
   Dst = _RCst_.Base_Ptr + y*_RCst_.Pix_BpS;
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      INT U, V;
      PIXEL *Ptr;

      Len = Scan_Pt1[y];
      Ptr = Dst + Len;
      Len = Scan_Pt2[y] - Len;

      U = Scan_U[y];
      V = Scan_V[y];
      while( --Len>=0 )
      {
         INT Off;
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         Off = UV_Src[ Off ];
         /* Ptr[Len] = ( (PIXEL)Off + Ptr[Len] )/2; */
         Ptr[Len] ^= (PIXEL)Off;
         U -= dyU;
         V -= dyV;
      }
      Dst += _RCst_.Pix_BpS;
      ++y;
      dy--;
   }
#endif
}

   /* TODO: Hack! Remove. */

static void (*Shader_Deal_UV_Alpha( ))( )
{
   if ( Shader_Deal_UV( ) != Shader_Draw_UV )
      return( Shader_Draw_Flat );
   return( Shader_Draw_UV_Alpha );
}

/******************************************************************/
/******************************************************************/

EXTERN SHADER_METHODS UV_Alpha_Shader =
{
   Set_Param_UV,
   /* Transform_Vertices_No_Cull */ NULL,
   NULL,
   Shader_Shade_UV,
   Shader_Deal_UV_Alpha, NULL NULL,
};

/******************************************************************/
/*                        TODO: Hack! Remove.                     */
/******************************************************************/

EXTERN void Shader_Draw_UVl( )
{
#if 0
   INT y;
   PIXEL *Dst;
   INT DU, DV;

   DU = (INT)( DyU );
   DV = (INT)( DyV );

   y = Scan_Start;
   Dst = _RCst_.Base_Ptr + y*_RCst_.Pix_BpS;

   while( y<Scan_H )   
   {
      INT Len;
      INT U, V;
      PIXEL *Ptr;
      UINT So;

      FLT2INT( &Len, &Scan_Pt1[y] );
      Ptr = Dst + Len;
      FLT2INT( &U, &Scan_Pt2[y] ); 
      Len = U-Len;

      U = (INT)( Scan_U[y] );
      V = (INT)( Scan_V[y] );
      /* { _Clock( So ); } */
#if 1
      {
         INT Tmp;
         Tmp = Len;
         while( --Tmp>=0 )
         {
            INT Off;
            Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
            Ptr[Tmp] = Main_Tab[ UV_Src[Off] | Flat_Color ] ;
            U -= DU;
            V -= DV;
         }
      }
#else
      {
         Tmp = Len;
         while( --Tmp>=0 )
         {
            XBuffer[Tmp] = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
            U -= DU;
            V -= DV;
         }
         Tmp = Len;
         while( --Tmp>=0 )
         {
            XBuffer[Tmp] = (USHORT)(UV_Src[XBuffer[Tmp]]) | Flat_Color;
         }
         Tmp = Len;
         while( --Tmp>=0 )
            Ptr[Tmp] = Main_Tab[ XBuffer[Tmp] ];
      }

#endif

/* 
      {
         INT S1;
         _Clock( S1 ); So = S1-So;
         if ( Len>20 ) fprintf( stderr, " %f\r", (FLT)So/(FLT)Len );
      }
*/
      Dst += _RCst_.Pix_BpS;
      ++y;
   }
#endif
}

static void (*Shader_Deal_UVl( ))( )
{
   INT i;
   FLT R;

   dU1 = ( P_Edges[1]->UV[0]-P_Edges[0]->UV[0] );
   dU2 = ( P_Edges[2]->UV[0]-P_Edges[1]->UV[0] );

   dV1 = ( P_Edges[1]->UV[1]-P_Edges[0]->UV[1] );
   dV2 = ( P_Edges[2]->UV[1]-P_Edges[1]->UV[1] );

   if ( UV_Src==NULL )
      if ( Compute_MipMap( ) )
      {
Skip1:
         Constants_Computed = TRUE;
         return( RENDER_FLAT_II );
      }

   Area2 = Area * Mip_Scale;

   DyU = ( dU1*dy1-dU2*dy0 )*Area2;
   dyU = (INT)( DyU );  /* Used in inner-loop */
   DyV = ( dV1*dy1-dV2*dy0 )*Area2;
   dyV = (INT)( DyV );
   DxU = ( dU2*dx0 - dU1*dx1 )*Area2;
   DxV = ( dV2*dx0 - dV1*dx1 )*Area2;

   Rasterize_UV( );

   R = Dot_Product( Cur_Light->D, Cur_P_Poly->N );
   if ( R>0.0 ) Flat_Color = 0; /* return( Shader_Draw_UV ); */
   else Flat_Color = (USHORT)(-R*255.9 )<<8;

   return( Shader_Draw_UVl );
}

EXTERN SHADER_METHODS UVl2_Shader =
{
   Set_Param_UV,
   NULL,
   Shader_Emit_Polys,
   NULL, NULL,
   Shader_Shade_UV,
   Shader_Deal_UVl, NULL, NULL
};

/******************************************************************/
/*                    UV-Corrected #1                             */
/******************************************************************/

#if 0
static void (*Shader_Deal_UVc( ))( )
{
   INT i, y, yf;
   FLT R;

   if ( Constants_Computed ) goto Constants_Ok;

   dU1 = ( P_Edges[1]->UV[0]-P_Edges[0]->UV[0] );
   dU2 = ( P_Edges[2]->UV[0]-P_Edges[1]->UV[0] );

   dV1 = ( P_Edges[1]->UV[1]-P_Edges[0]->UV[1] );
   dV2 = ( P_Edges[2]->UV[1]-P_Edges[1]->UV[1] );

   diZ1 = P_Edges[1]->Inv_Z - P_Edges[0]->Inv_Z;
   diZ2 = P_Edges[2]->Inv_Z - P_Edges[1]->Inv_Z;

   R = (256.0*256.0) * Area * ( dU1*dV2-dU2*dV1 );
   if ( R<0.0 ) R = -R;

   if ( UV_Src==NULL )
   {
         /* Size=128 => Mip_Scale = 32767.9 */
      UV_Src = Cur_Texture_Ptr;
      if ( (*Cache_Methods.Retreive_Texture)( Cur_Mip_Slot, R ) < 4 )
      {
         Constants_Computed = TRUE;
         Flat_Color = (USHORT)UV_Src[1];
         return( Shader_Draw_Flat );
      }
   }

   Area3 = Area*iZ_SCALE;
   DyiZ = ( diZ1*Edges_dy[1]-diZ2*Edges_dy[0] )*Area3;

   Area2 = Area * Mip_Scale;

      /* Evaluate needs of correction... */

   R = 2.0 * DyiZ / ( P_Edges[0]->Inv_Z * Edges_dy[0] );
   if ( R<UVc_EPSILON && R>-UVc_EPSILON )
   {
      DyU = ( dU1*Edges_dy[1]-dU2*Edges_dy[0] )*Area2;
      DyV = ( dV1*Edges_dy[1]-dV2*Edges_dy[0] )*Area2;
      Constants_Computed = TRUE;
      return( Shader_Deal_UV( ) );
   }

   dU1 = ( P_Edges[1]->UV[0]*P_Edges[1]->Inv_Z-P_Edges[0]->UV[0]*P_Edges[0]->Inv_Z );
   dU2 = ( P_Edges[2]->UV[0]*P_Edges[2]->Inv_Z-P_Edges[1]->UV[0]*P_Edges[1]->Inv_Z );

   dV1 = ( P_Edges[1]->UV[1]*P_Edges[2]->Inv_Z-P_Edges[0]->UV[1]*P_Edges[0]->Inv_Z );
   dV2 = ( P_Edges[2]->UV[1]*P_Edges[2]->Inv_Z-P_Edges[1]->UV[1]*P_Edges[1]->Inv_Z );

   DyU = ( dU1*Edges_dy[1]-dU2*Edges_dy[0] )*Area2;
   DyV = ( dV1*Edges_dy[1]-dV2*Edges_dy[0] )*Area2;

   DxU = ( dU2*Edges_dx[0] - dU1*Edges_dx[1] )*Area2;
   DxV = ( dV2*Edges_dx[0] - dV1*Edges_dx[1] )*Area2;
   DxiZ = ( diZ2*Edges_dx[0] - diZ1*Edges_dx[1] )*Area3;


Constants_Ok:

   i=Right_Edges_Nb;
   while( --i>=0 )
   {
      INT j;

      j = Right_Edges_Used[i];
      y = Edges_yo[j]; 
      if ( y>Scan_H ) continue;
      if ( y<Scan_Start ) y=Scan_Start;
      yf = Edges_yf[j];
      if ( yf<Scan_Start ) continue;
      if ( yf>Scan_H ) yf = Scan_H;

      U = P_Edges[j]->UV[0]*P_Edges[j]->Inv_Z;
      dU12 =  ( P_Edges[j+1]->UV[0]*P_Edges[j+1]->Inv_Z - U )*Slopes[j];
      dU12 *= Mip_Scale;
      U *= Mip_Scale;

      V = P_Edges[j]->UV[1]*P_Edges[j]->Inv_Z;
      dV12 =  ( P_Edges[j+1]->UV[1]*P_Edges[j+1]->Inv_Z - V )*Slopes[j];
      dV12 *= Mip_Scale;
      V *= Mip_Scale;

      iZ = P_Edges[j]->Inv_Z;
      diZ12 =  ( P_Edges[j+1]->Inv_Z - iZ )*Slopes[j];
      diZ12 *= iZ_SCALE;
      iZ *= iZ_SCALE;

      {
         FLT dx, dy;
         dy = (FLT)y - Edges_y[j]; 
         dx = (FLT)Scan_Pt2[y] - Edges_x[j];
         U += dy*DxU + dx*DyU;
         V += dy*DxV + dx*DyV;
         iZ += dy*DxiZ + dx*DyiZ;
      }

      while( y<yf )
      {
         Scan_U[y] = U/iZ; U += dU12;
         Scan_V[y] = V/iZ; V += dV12;
         Scan_iZ[y] = iZ; iZ += diZ12;
         y++;
      }
   }

   if ( Must_Clip )
   {
      FLT dx, dy;

      y = Right_Start;
      dy = (FLT)Right_Start - Edges_y[0];
      dx = _RCst_.Clips[1] - Edges_x[0];
      U = P_Edges[0]->UV[0]*Mip_Scale;
      U += dy*DxU + dx*DyU;
      V = P_Edges[0]->UV[1]*Mip_Scale;
      V += dy*DxV + dx*DyV;
      iZ = P_Edges[0]->Inv_Z*iZ_SCALE;;
      iZ += dy*DxiZ + dx*DyiZ;

      while( y<Right_Stop )
      {
         Scan_U[y] = U/iZ; U += DxU; 
         Scan_V[y] = V/iZ; V += DxV; 
         Scan_iZ[y] = iZ; iZ += DxiZ; 
         y++;
      }
   }

   return( Shader_Draw_UVc );
}
#endif

#if 0
EXTERN void Shader_Draw_UVc( )
{
   INT y;
   PIXEL *Dst;
   INT DUo, DVo, DU, DV, D2U, D2V, DiZ;

   DUo = (INT)( DyU );
   DVo = (INT)( DyV );
   DiZ = (INT)( DyiZ );

      /* Now, draw! Here lies the Mythical Inner-Loop :) */

   y = Scan_Start;
   Dst = _RCst_.Base_Ptr + y*_RCst_.Pix_BpS;

   while( y<Scan_H )   
   {
      INT Len;
      INT U, V;
      FLT A;
      PIXEL *Ptr;

      FLT2INT( &Len, &Scan_Pt1[y] );
      Ptr = Dst + Len;
      FLT2INT( &U, &Scan_Pt2[y] ); 
      Len = U-Len;

      U = (INT)( Scan_U[y] );
      V = (INT)( Scan_V[y] );
      if ( Len<8 ) goto Basic_UV;

      A = DyiZ * (FLT)Len;
      A = 2.0*A / ( Scan_iZ[y] + A );
      if ( A<UVc_EPSILON && A>-UVc_EPSILON )
      {
Basic_UV:
         while( --Len>=0 )
         {
            INT Off;
            Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
            Ptr[Len] = UV_Src[ Off ];
            U -= DUo;
            V -= DVo;
         }
         goto End;
      }
      else  /* parabolic correction */
      {
         D2U = (INT)( 2.0*A*DyU );
         DU = (INT)( DyU*(1-A) );
         D2V = (INT)( 2.0*A*DyV );
         DV = (INT)( DyV*(1-A) );
         while( --Len>=0 )
         {
            INT Off;
            Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
            Ptr[Len] = UV_Src[ Off ];
            U -= DU; DU += D2U;
            V -= DV; DV += D2V;
         }
      }
End:
      Dst += _RCst_.Pix_BpS;
      ++y;
   }

}
#endif

/******************************************************************/
#if 0
EXTERN SHADER_METHODS UVc_Shader =
{
   Set_Param_UV,
   NULL,
   Shader_Emit_Polys,
   NULL, NULL,
   Shader_Shade_UV,
   Shader_Deal_UVc, NULL, NULL
};
#endif

/******************************************************************/
/******************************************************************/
/*              constant-z UV drawing                             */
/******************************************************************/
/******************************************************************/

static void (*Shader_Deal_UVc2( ))( )
{
#if 0
   INT i, y, yf;
   FLT R, Dx, Dy;

   dU1 = ( P_Edges[1]->UV[0]-P_Edges[0]->UV[0] );
   dU2 = ( P_Edges[2]->UV[0]-P_Edges[1]->UV[0] );

   dV1 = ( P_Edges[1]->UV[1]-P_Edges[0]->UV[1] );
   dV2 = ( P_Edges[2]->UV[1]-P_Edges[1]->UV[1] );

   R = (256.0*256.0) * Area * ( dU1*dV2-dU2*dV1 );
   if ( R<0.0 ) R = -R;

   if ( UV_Src==NULL )
   {
         /* Size=128 => Mip_Scale = 32767.9 */
      UV_Src = Cur_Texture_Ptr;
      if ( (*Cache_Methods.Retreive_Texture)( Cur_Mip_Slot, R ) < 4 )
      {
         Constants_Computed = TRUE;
         Flat_Color = (USHORT)UV_Src[1];
         return( Shader_Draw_Flat );
      }
   }

   Area2 = Area * Mip_Scale;

   DyU = ( dU1*Edges_dy[1]-dU2*Edges_dy[0] )*Area2;
   DyV = ( dV1*Edges_dy[1]-dV2*Edges_dy[0] )*Area2;

   DxU = ( dU2*Edges_dx[0] - dU1*Edges_dx[1] )*Area2;
   DxV = ( dV2*Edges_dx[0] - dV1*Edges_dx[1] )*Area2;

      /* Evaluate needs of correction... */
   if ( Area>AREA_EPSILON || Cur_P_Poly->N[2]<(-1.0+NZc_EPSILON) )
   {
      Constants_Computed = TRUE;
      return( Shader_Deal_UV( ) );
   }
   Dx = Cur_P_Poly->N[1];
   Dy = Cur_P_Poly->N[0];
   if ( Dx<0.0 ) { Dx = -Dx; Dy = -Dy; }
   DzU = Dx*DyU - Dy*DxU;
   DzV = Dx*DyV - Dy*DxV;

   i=Right_Edges_Nb;
   while( --i>=0 )
   {
      INT j;

      j = Right_Edges_Used[i];

      y = Edges_yo[j];
      if ( y>Scan_H ) continue;
      if ( y<Scan_Start ) y=Scan_Start;
      yf = Edges_yf[j];
      if ( yf<Scan_Start ) continue;
      if ( yf>Scan_H ) yf = Scan_H;

      U = P_Edges[j]->UV[0]*P_Edges[j]->Inv_Z;
      dU12 =  ( P_Edges[j+1]->UV[0]*P_Edges[j+1]->Inv_Z - U )*Slopes[j];
      dU12 *= Mip_Scale;
      U *= Mip_Scale;

      V = P_Edges[j]->UV[1]*P_Edges[j]->Inv_Z;
      dV12 =  ( P_Edges[j+1]->UV[1]*P_Edges[j+1]->Inv_Z - V )*Slopes[j];
      dV12 *= Mip_Scale;
      V *= Mip_Scale;

         /* TODO: U & V needs being z-corrected, here! */
         /* ... */
      {
         FLT dx, dy;
         dy = (FLT)y - Edges_y[j]; 
         dx = (FLT)Scan_Pt2[y] - Edges_x[j];
         U += dy*DxU + dx*DyU;
         V += dy*DxV + dx*DyV;
      }

      while( y<yf )
      {
         Scan_U[y] = U; U += dU12;
         Scan_V[y] = V; V += dV12;
         y++;
      }
   }

   if ( Must_Clip )
   {
      FLT dx, dy;

      y = Right_Start;
      dy = (FLT)Right_Start - Edges_y[0];
      dx = _RCst_.Clips[1] - Edges_x[0];

      U = P_Edges[0]->UV[0]*Mip_Scale;
      U += dy*DxU + dx*DyU;
      V = P_Edges[0]->UV[1]*Mip_Scale;
      V += dy*DxV + dx*DyV;

      while( y<Right_Stop )
      {
         Scan_U[y] = U; U += DxU; 
         Scan_V[y] = V; V += DxV; 
         y++;
      }
   }


      /* Dx is >0.0 at this point... */
      /* Alpha = Dy/Dx <=> Screen slope. */
   if ( Dy>0.0 && Dy<Dx )  goto Alpha_Small; /* downward small */
   if ( Dy<0.0 && Dy>-Dx ) goto Alpha_Small;  /* upward small */
      
         /* case: Dy>0, |Alpha|>1   */
         /* and case Dy<0, |Alpha|>1 */

   _A_ = (INT)( Dx/Dy*65536.0 ); /* _A_<1 or _A_>-1 */
   R = 1.0/Dy;
   DzU *= R; DzV *= R;
   if ( R>0.0 ) return( Shader_Draw_UVc2_Ap ); /* screenwise downward. A>0 */
   else return( Shader_Draw_UVc2_An );          /* screenwise upward. A<0.   */

Alpha_Small:
   _A_ = (INT)( Dy/Dx*65536.0 );
   R = 1.0/Dx;
   DzU *= R; DzV *= R;

   return( Shader_Draw_UVc2 );
#endif
   return( NULL );
}

EXTERN void Shader_Draw_UVc2_Ap( )
{
#if 0
   INT y;
   PIXEL *Dst;
   INT DZU, DZV;

   DZU = (INT)( DzU );
   DZV = (INT)( DzV );

      /* Now, draw! Here lies the Mythical Inner-Loop :) */

   y = (INT)Scan_Start;
   Dst = _RCst_.Base_Ptr + y*_RCst_.Pix_BpS;

   while( y<Scan_H )   
   {
      INT X, Y, x;
      INT U, V;
      PIXEL *Ptr;

      Ptr = Dst;
      x = (INT)Scan_Pt2[y];

      X = x<<16;
      U = (INT)( Scan_U[y] );
      V = (INT)( Scan_V[y] );
      Y = y;
      while( x>(INT)Scan_Pt1[Y] && x<=(INT)Scan_Pt2[Y]
             && Y<Scan_H )   /* put a sentinel !! */
      {
         INT Off;
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         x = (X>>16);
         Ptr[x] = UV_Src[ Off ];
         /* Ptr[x] = 0x01; */
         U -= DZU;
         V -= DZV;
         X -= _A_;
         Ptr += _RCst_.Pix_BpS;
         Y++;
      }
      Dst += _RCst_.Pix_BpS;
      ++y;
   }
#endif
}

EXTERN void Shader_Draw_UVc2_An( )
{
#if 0
   INT y;
   PIXEL *Dst;
   INT DZU, DZV;

   DZU = (INT)( DzU );
   DZV = (INT)( DzV );

      /* Now, draw! Here lies the Mythical Inner-Loop :) */

   y = Scan_Start;
   Dst = _RCst_.Base_Ptr + y*_RCst_.Pix_BpS;

   while( y<Scan_H )   
   {
      INT X, Y, x;
      INT U, V;
      PIXEL *Ptr;

      Ptr = Dst;
      x = (INT)Scan_Pt2[y];

      X = x<<16;
      U = (INT)( Scan_U[y] );
      V = (INT)( Scan_V[y] );
      Y = y;

            /* TODO: put a sentinel !! */
            /* TODO: Don't emit edges with Alpha>dx/dy!! */
      while(  x>(INT)Scan_Pt1[Y] && x<=(INT)Scan_Pt2[Y]
             && Y>=Scan_Start )
      {
         INT Off;
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         x = (X>>16);
         Ptr[x] = UV_Src[ Off ];
         U -= DZU;
         V -= DZV;
         X += _A_;
         Ptr -= _RCst_.Pix_BpS;
         Y--;
      }
      Dst += _RCst_.Pix_BpS;
      ++y;
   }
#endif
}

EXTERN void Shader_Draw_UVc2( )
{
#if 0
   INT y;
   INT DZU, DZV;

   DZU = (INT)( DzU );
   DZV = (INT)( DzV );

      /* Now, draw! Here lies the Mythical Inner-Loop :) */

   y = Scan_Start;

   while( y<Scan_H )   
   {
      INT Y, yo, x;
      INT U, V;
      PIXEL *Ptr;

      x = (INT)Scan_Pt2[y];
      Ptr = _RCst_.Base_Ptr + x;

      U = (INT)( Scan_U[y] );
      V = (INT)( Scan_V[y] );
      yo = y;
      Y = (yo<<16);

            /* put a sentinel !! */
      while( x>(INT)Scan_Pt1[yo] && x<=(INT)Scan_Pt2[yo] 
          && yo<Scan_H && yo>=Scan_Start )
      {
         INT Off;
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         yo = (Y>>16);
         Ptr[yo*_RCst_.Pix_BpS] = UV_Src[ Off ];
         Y -= _A_;
         U -= DZU;
         V -= DZV;
         x--;
         Ptr--;
      }
      ++y;
   }
#endif
}

/******************************************************************/

EXTERN SHADER_METHODS UVc2_Shader =
{
   Set_Param_UV,
   NULL,
   Shader_Emit_Polys,
   NULL, NULL,
   Shader_Shade_UV,
   Shader_Deal_UVc2, NULL, NULL
};

/******************************************************************/
/******************************************************************/
