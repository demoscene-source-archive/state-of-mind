/******************************************************************/
/*                     LIGHTEN Shadow                             */
/******************************************************************/

#include "main3d.h"

#ifdef USE_OLD_SHADOWZ

EXTERN USHORT The_ID;

/******************************************************************/

#if defined( DEBUG_SHADOW )
static void Debug_Shadow( INT y, USHORT Col )
{
   INT X;
   INT Off;
   INT _U_, _V_;
   X = (INT)( Scan_Pt2[y] ) - (INT)( Scan_Pt1[y] );
   _U_ = Scan_U[y];
   _V_ = Scan_V[y];

   Off = ( (_U_>>16)&0x00FF ) | ( (_V_>>8)&0xFF00 );
   Light_Buf[Off] = (Col<<4);
   return;
   --X;
   _U_ -= dyU;
   _V_ -= dyV;
   while( --X>=0 )
   {
      Off = ( (_U_>>16)&0x00FF ) | ( (_V_>>8)&0xFF00 );
      if ( X==0 ) Light_Buf[Off] = (Col + 3)<<4 ;
      else Light_Buf[Off] = (0xFF<<4);
      _U_ -= dyU;
      _V_ -= dyV;
   }
}
#endif

/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_UV_Shadow( )
{
   INT i, y, yf;
   FLT R;

#if 0
   R = Dot_Product( Cur_Light->D, Cur_P_Poly->N );
   if ( R>0.0 /* || Area<1.0/256.0 */ ) {
Not_Good:
      Flat_Color = 0x20;   /* full shadow */
      return( RENDER_FLAT_I );
   }
#endif

   /* Could be retreived here => R = Cur_P_Poly->C; ? */
   R = Dot_Product( Cur_Light->D, Cur_P_Poly->N );
   if ( R>-1.0/256.0 ) 
   {
Not_Good:
      Flat_Color = 0x20;   /* full shadow */
      return( RENDER_FLAT_I );
   }
   Flat_Color = (USHORT)(-R*255.9 );

   The_ID = Orig_Poly->ID;

   {
      FLT dU1, dU2, dV1, dV2;

      dU1 = ( P_Edges[1]->N[0]-P_Edges[0]->N[0] );
      dU2 = ( P_Edges[2]->N[0]-P_Edges[1]->N[0] );

      dV1 = ( P_Edges[1]->N[1]-P_Edges[0]->N[1] );
      dV2 = ( P_Edges[2]->N[1]-P_Edges[1]->N[1] );

      Area2 = Area * SHADOW_SCALE;

      DyU = ( dU1*dy1-dU2*dy0 )*Area2;
      DyV = ( dV1*dy1-dV2*dy0 )*Area2;
      DxU = ( dU2*dx0 - dU1*dx1 )*Area2;
      DxV = ( dV2*dx0 - dV1*dx1 )*Area2;
   }

   i=Nb_Right_Edges;
   while( --i>=0 )
   {
      INT j;
      INT y, yf;
      

      j = Right_Edges[i];

      y = Edges_yo[j];
      yf = Edges_yf[j];

      {
         FLT dx, dy, Tmp, Slope;
         FLT dU12, dV12;
         INT Xe, dXe;
         INT _U_, _dU_, _V_, _dV_, _Uo_, _Vo_;

         dy = Edges_ey[j];
         dx = (FLT)Scan_Pt2[y] - Edges_x[j];

         dyU = (INT)( DyU );
         dyV = (INT)( DyV );
         /* printf( "%f %f  ", DyU, DyV ); */

         dXe = Edges_dXe[j];
         if ( dXe>=0 ) Slope = (FLT)(dXe>>16);
         else Slope = -(FLT)( (-dXe)>>16 );

         dV12 = Out_Vtx[j+1]->N[1] - Out_Vtx[j]->N[1];
         dU12 = Out_Vtx[j+1]->N[0] - Out_Vtx[j]->N[0];
         if ( ( (dU12>=0.0) && ( dV12>dU12 || dV12<-dU12 ) ) ||
              ( (dU12<=0.0) && ( dV12<dU12 || dV12>-dU12 ) ) )
         {
            INT eV, incV;

            Tmp = DxV + Slope*DyV;
            _dV_ = (INT)( Tmp );
            Tmp = Out_Vtx[j]->N[1] * SHADOW_SCALE;
            _Vo_ = (INT)( Tmp );
            Tmp += dy*DxV + dx*DyV;
            _V_ = (INT)( Tmp );
            eV = _V_-_Vo_;

            _Uo_ = (INT)( Out_Vtx[j]->N[0] * SHADOW_SCALE );

            Slope = dU12/dV12*SHADOW_SCALE;
            /* printf( "S:%f\n", Slope ); */
            if ( dU12>0.0 ) _Uo_ += 0x10000;

            Xe = Edges_Xe[j];
            if ( dXe<0 )
            {
               dXe = -dXe; Xe  = -Xe;
               incV = -dyV;
            }
            else { incV = dyV; } /* Hack ! Remove. */
            dXe &= 0xFFFF; Xe &= 0xFFFF;

            while( y<yf )
            {
               INT Delta_V;
               FLT DU; 
               Scan_V[y] = _V_ | 0xFFFF;
               DU = (FLT)( eV>>16 )*Slope;
               Scan_U[y] = ( _Uo_ + (INT)(DU) ) | 0xFFFF;

#if defined( DEBUG_SHADOW )
               if (y==yf-1) Debug_Shadow( y, 0x02 );
               else Debug_Shadow( y, 0x03 );
#endif

               Delta_V = _dV_;
               Xe += dXe;
               if ( Xe & 0x10000 )
               {
                  Xe &= 0xFFFF;
                  Delta_V += incV;
               }
               _V_ += Delta_V;
               eV += Delta_V;
               y++;
            }
         }
         else
         {
            INT eU, incU;

            Tmp = DxU + Slope*DyU;
            _dU_ = (INT)( Tmp );
            Tmp = Out_Vtx[j]->N[0] * SHADOW_SCALE;
            _Uo_ = (INT)( Tmp );
            Tmp += dy*DxU + dx*DyU;
            _U_ = (INT)( Tmp );
            eU = _U_-_Uo_;

            _Vo_ = (INT)( Out_Vtx[j]->N[1] * SHADOW_SCALE );

            Slope = dV12/dU12*SHADOW_SCALE;
            if ( dU12<0.0 ) _Vo_ += 0x10000;

            Xe = Edges_Xe[j];
            if ( dXe<0 )
            {
               dXe = -dXe; Xe  = -Xe;
               incU = -dyU;
            }
            else { incU = dyU; } /* Hack ! Remove. */
            dXe &= 0xFFFF; Xe &= 0xFFFF;

            while( y<yf )
            {
               INT Delta_U;
               FLT DV; 
               Scan_U[y] = _U_ | 0xFFFF;
               DV = (FLT)( eU>>16 )*Slope;
               Scan_V[y] = ( _Vo_ + (INT)(DV) ) | 0xFFFF;

#if defined( DEBUG_SHADOW )
               if (y==yf-1) Debug_Shadow( y, 0x12 );
               else Debug_Shadow( y, 0x13 );
#endif

               Delta_U = _dU_;
               Xe += dXe;
               if ( Xe & 0x10000 )
               {
                  Xe &= 0xFFFF;
                  Delta_U += incU;
               }
               _U_ += Delta_U;
               eU += Delta_U;
               y++;
            }
         }
      }
   }
   return( _Draw_Flat_Shadow );
}

EXTERN void _Draw_Flat_Shadow( )
{
   INT y, dy;
   PIXEL *Dst;

#if defined( DEBUG_SHADOW )
   /* return; */
#endif

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

      while( Len>0 )
      {
         INT Off;
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
/* #if !defined( DEBUG_SHADOW ) */

         if ( Light_Buf[Off] == The_ID ) 
         {
            Ptr[Len] = Flat_Color;
         }
         else Ptr[Len] = 0x20;  /* The shadow */
/* #else */
/*
         if ( Len==0 ) Light_Buf[Off] = 0x02;
         else Light_Buf[Off] = 0x03;
*/
/* #endif */

         U -= dyU;
         V -= dyV;
         Len--;
      }
      Dst += _RCst_.Pix_BpS;
      ++y;
      dy--;
   }
}

/******************************************************************/

EXTERN void Shader_Deal_Lighten_Shadow( MATRIX2 M )
{
      /* Needed by UV+Shadow mapper. Oh well... */
   Shader_Shade_UV( M );
}

/******************************************************************/
/******************************************************************/

EXTERN SHADER_METHODS Flat_Shadow_Lighten_Shader =
{
   Set_Param_Flat,
   NULL,
   Split_Shadow,
   Shader_Deal_Lighten_Shadow,
   Shader_Deal_UV_Shadow, NULL, NULL,
   NULL
};

/******************************************************************/
/******************************************************************/

/******************************************************************/
/*          UV-1/z setup for Camera-Space                         */
/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_UV_zShadow( )
{
   INT i;
   FLT R;

/*   R = Cur_P_Poly->C;  Retreive light intensity ?! */
   R = Dot_Product( Cur_Light->D, Cur_P_Poly->N );
   if ( R>-1.0/256.0 ) 
   {
Not_Good:
      Flat_Color = 0x20;   /* full shadow */
      return( _Draw_Flat );
   }
   Flat_Color = (USHORT)(-R*255.9 );

   Area2 = Area * SHADOW_SCALE;

   dU1 = ( P_Edges[1]->N[0] - P_Edges[0]->N[0] );
   dU2 = ( P_Edges[2]->N[0] - P_Edges[1]->N[0] );
   DyU = ( dU1*dy1 - dU2*dy0 )*Area2;
   DxU = ( dU2*dx0 - dU1*dx1 )*Area2;

   dV1 = ( P_Edges[1]->N[1] - P_Edges[0]->N[1] );
   dV2 = ( P_Edges[2]->N[1] - P_Edges[1]->N[1] );
   DyV = ( dV1*dy1 - dV2*dy0 )*Area2;
   DxV = ( dV2*dx0 - dV1*dx1 )*Area2;

   dS1 = ( P_Edges[1]->N[2] - P_Edges[0]->N[2] )*INV_Z_SCALE;
   dS2 = ( P_Edges[2]->N[2] - P_Edges[1]->N[2] )*INV_Z_SCALE;
   DyS = ( dS1*dy1 - dS2*dy0 )*Area;
   DxS = ( dS2*dx0 - dS1*dx1 )*Area;

   i=Nb_Right_Edges;
   while( --i>=0 )
   {
      INT j;
      INT y, yf;
      
      j = Right_Edges[i];

      y = Edges_yo[j];
      yf = Edges_yf[j];

      {
         FLT dx, dy, Tmp, Slope;
         INT _S_, _U_, _dS_, _dU_, _V_, _dV_;
         FLT dU12, dV12;
         INT _Uo_, _Vo_;
         INT Xe, dXe; 

         dy = Edges_ey[j];
         dx = (FLT)Scan_Pt2[y] - Edges_x[j];

         dXe = Edges_dXe[j];
         if ( dXe>=0 ) Slope = (FLT)(dXe>>16);
         else Slope = -(FLT)( (-dXe)>>16 );

         Tmp = DxS + Slope*DyS;
         _dS_ = (INT)( Tmp );
         Tmp = Out_Vtx[j]->N[2] * INV_Z_SCALE;
         Tmp += dy*DxS + dx*DyS;
         _S_ = (INT)( Tmp );

         dyS = (INT)( DyS );
         dyU = (INT)( DyU );
         dyV = (INT)( DyV );
   
         dU12 = Out_Vtx[j+1]->N[0] - Out_Vtx[j]->N[0];
         dV12 = Out_Vtx[j+1]->N[1] - Out_Vtx[j]->N[1];

         if ( ( ( dU12>0.0 ) && ( dV12>dU12 || dV12<-dU12 ) ) ||
            ( ( dU12<0.0 ) && ( dV12<dU12 || dV12>-dU12 ) ) )
         {
            INT incV, incS, _Uo_;
            INT eV;

            Tmp = DxV + Slope*DyV;
            _dV_ = (INT)( Tmp );
            Tmp = Out_Vtx[j]->N[1] * SHADOW_SCALE;
            _Vo_ = (INT)( Tmp );
            Tmp += dy*DxV + dx*DyV;
            _V_ = (INT)( Tmp );
            eV = _V_-_Vo_;
            _Uo_ = (INT)( Out_Vtx[j]->N[0] * SHADOW_SCALE );

            if ( dV12>0.0 ) _Uo_ -= 0x10000;
            Slope = dU12/dV12*SHADOW_SCALE;


            Xe = Edges_Xe[j];
            if ( dXe<0 )
            {
               dXe = -dXe; Xe  = -Xe;
               incV = -dyV;
               incS = -dyS;
            }
            else { incV = dyV; incS = dyS; } /* Hack ! Remove. */
            dXe &= 0xFFFF; Xe &= 0xFFFF;

            while( y<yf )
            {
               INT Delta_V;
               FLT DU;
               Scan_S[y] = _S_;_S_ += _dS_;
               Scan_V[y] = _V_;
               DU = (FLT)( eV>>16 )*Slope;
               Scan_U[y] = ( _Uo_ + (INT)(DU) );

/*
#if defined( DEBUG_SHADOW )
               if (y==yf-1) Debug_Shadow( y, 0x12 );
               else Debug_Shadow( y, 0x13 );
#endif
*/

               Delta_V = _dV_;
               Xe += dXe;
               if ( Xe & 0x10000 )
               {
                  Xe &= 0xFFFF;
                  Delta_V += incV;
                  _S_ += incS;
               }
               _V_ += Delta_V;
               eV += Delta_V;
               y++;
            }
         }
         else
         {
            INT incU, incS, _Vo_;
            INT eU;

            Tmp = DxU + Slope*DyU;
            _dU_ = (INT)( Tmp );
            Tmp = Out_Vtx[j]->N[0] * SHADOW_SCALE;
            _Uo_ = (INT)( Tmp );
            _Uo_ -= 0x10000;

            Tmp += dy*DxU + dx*DyU;
            _U_ = (INT)( Tmp );
            eU = _U_-_Uo_;
            _Vo_ = (INT)( Out_Vtx[j]->N[1] * SHADOW_SCALE );

            /* if ( dV12>0.0 ) _Vo_ -= 0x10000; */
            Slope = dV12/dU12*SHADOW_SCALE;

            Xe = Edges_Xe[j];
            if ( dXe<0 )
            {
               dXe = -dXe; Xe  = -Xe;
               incU = -dyU;
               incS = -dyS;
            }
            else { incU = dyU; incS = dyS; } /* Hack ! Remove. */
            dXe &= 0xFFFF; Xe &= 0xFFFF;

            while( y<yf )
            {
               INT Delta_U;
               FLT DV;
               Scan_S[y] = _S_; _S_ += _dS_;
               Scan_U[y] = _U_;
               DV = (FLT)( eU>>16 )*Slope;
               Scan_V[y] = ( _Vo_ + (INT)(DV) );

#if defined( DEBUG_SHADOW )
               if (dU12>0.0) Debug_Shadow( y, 0x42 );
               else Debug_Shadow( y, 0x53 );
#endif

               Delta_U = _dU_;
               Xe += dXe;
               if ( Xe & 0x10000 )
               {
                  Xe &= 0xFFFF;
                  Delta_U += incU;
                  _S_ += incS;
               }
               _U_ += Delta_U;
               eU += Delta_U;
               y++;
            }
         }
      }
   }
   return( _Draw_Flat_zShadow );
}

         /* 1/Z-buffer comparaison/drawing in camera space */

EXTERN void _Draw_Flat_zShadow( )
{
   INT y, dy;
   PIXEL *Dst;

   y = Scan_Start;
   Dst = _RCst_.Base_Ptr + y*_RCst_.Pix_BpS + 1;
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      INT U, V, S;
      PIXEL *Ptr;

      Len = Scan_Pt1[y];
      Ptr = Dst + Len;
      Len = Scan_Pt2[y] - Len;

      U = Scan_U[y];
      V = Scan_V[y];
      S = Scan_S[y];

      while( Len>0 )
      {
         INT Off;
         Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
         if ( Light_Buffer2[Off] < (S>>16) )
         {
            Ptr[Len] = Flat_Color;
         }
         else Ptr[Len] = 0x20;  /* The shadow */
         /* Light_Buffer2[Off] = 0xFFFF; */
         U -= dyU;
         V -= dyV;
         S -= dyS;
         Len--;
      }
      Dst += _RCst_.Pix_BpS;
      ++y;
      dy--;
   }
}

/******************************************************************/
/*         Light-1/z setup in Camera-Space                        */
/******************************************************************/

EXTERN void Shader_Deal_Lighten_zShadow( MATRIX2 M )
{
//   USHORT UV;
//   Cur_P_Vertex->N[0] = (FLT)( UV&0xFF );
//   Cur_P_Vertex->N[1] = (FLT)( ( UV>>8 )&0xFF );

      /* Needed by UV+Shadow mapper. Oh well... */
   Shader_Shade_UV( M );
}

/******************************************************************/

         /* LIGHT-Shader */

EXTERN SHADER_METHODS Flat_zShadow_Lighten_Shader =
{
   Set_Param_Flat,
   NULL,
   Split_Shadow,
   Shader_Deal_Lighten_zShadow,
   Shader_Deal_UV_zShadow,NULL, NULL,
   NULL
};

/******************************************************************/
/******************************************************************/
/*          1/z-Gouraud setup in Light Space                      */
/******************************************************************/

EXTERN RENDERING_FUNCTION Shader_Deal_zShadow( )
{
   INT i;
   FLT R;

   /* if ( Area>AREA_EPSILON2 ) goto Skip; */

   dS1 = ( P_Edges[1]->Inv_Z - P_Edges[0]->Inv_Z )*INV_Z_SCALE;
   dS2 = ( P_Edges[2]->Inv_Z - P_Edges[1]->Inv_Z )*INV_Z_SCALE;

   DyS = ( dS1*dy1 - dS2*dy0 )*Area;
   DxS = ( dS2*dx0 - dS1*dx1 )*Area;

   i=Nb_Right_Edges;
   while( --i>=0 )
   {
      INT j;
      INT y, yf;

      j = Right_Edges[i];

      y = Edges_yo[j];
      yf = Edges_yf[j];

      {
         FLT dx, dy, Tmp, Slope;
         INT _S_, _dS_, incS;
         INT Xe, dXe; 

         dy = Edges_ey[j];
         dx = (FLT)Scan_Pt2[y] - Edges_x[j];

         dXe = Edges_dXe[j];
         if ( dXe>=0 ) Slope = (FLT)(dXe>>16);
         else Slope = -(FLT)( (-dXe)>>16 );

         Tmp = DxS + Slope*DyS;
         _dS_ = (INT)( Tmp );
         Tmp = Out_Vtx[j]->Inv_Z * INV_Z_SCALE;
         Tmp += dy*DxS + dx*DyS;    /* Sub-pixel */
         _S_ = (INT)( Tmp );
         dyS = (INT)DyS;

         Xe = Edges_Xe[j];
         if ( dXe<0 )
         {
            dXe = -dXe; Xe  = -Xe;
            incS = -dyS;
         }
         else incS = dyS;
         dXe &= 0xFFFF; Xe &= 0xFFFF;
         while( y<yf )
         {
            Scan_S[y] = _S_; _S_ += _dS_;
            Xe += dXe;
            if ( Xe & 0x10000 )
            {
               Xe &= 0xFFFF;
               _S_ += incS;
            }
            y++;
         }
      }
   }

#if defined( DEBUG_SHADOW )
   Flat_Color = (USHORT)( Orig_Poly->ID<<8 );
   return( _Draw_16b_zShadow_256 );
#else
   return( RENDER_Z_GOURAUD );
#endif
Skip:
   Flat_Color = (USHORT)( P_Edges[0]->Inv_Z*INV_Z_SCALE );
   return( _Draw_16b_zShadow_256 );
}

/******************************************************************/
/******************************************************************/

   /* TODO: Hack! Remove. */

EXTERN RENDERING_FUNCTION Shader_Deal_UV_Shadow2( )
{
   if ( Shader_Deal_UV( ) != _Draw_UV )
      return( _Draw_Flat_Shadow );
   UV_Src2 = UV_Src;
   return( RENDER_UV2 );
}

EXTERN SHADER_METHODS UV_Shadow_Lighten_Shader =
{
   Set_Param_Flat,
   NULL,
   Split_Shadow_UV,
   Shader_Deal_Lighten_Shadow,
   Shader_Deal_UV_Shadow, Shader_Deal_UV_Shadow2, NULL,
   NULL
};

EXTERN SHADER_METHODS UV_zShadow_Lighten_Shader =
{
   Set_Param_Flat,
   NULL,
   Split_Shadow_UV,
   Shader_Deal_Lighten_Shadow,
   Shader_Deal_UV_zShadow, Shader_Deal_UV_Shadow2, NULL,
   NULL
};

/******************************************************************/
/******************************************************************/

#endif   // USE_OLD_SHADOWZ
