/***********************************************
 *         Poly rasterizer                     *
 *   + void calls for UNIX only                *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

#ifdef UNIX

EXTERN void Enter_Engine_Asm( )
{
}

EXTERN void Exit_Engine_Asm( )
{
}
#endif

/******************************************************************/

#ifdef UNIX    // For DOS and Linux: it's in raster.asm

/******************************************************************/
/******************************************************************/

// EXTERN void Format_Scanlines( )  { }

/******************************************************************/
/******************************************************************/

EXTERN void Rasterize_Edges( )
{
   FLT Slope, Adjust_y;
   INT Cur_S;

   FLT2INT( &Scan_Start, &_RCst_.Clips[3] );
   Scan_H = 0;

   Nb_Right_Edges = Nb_Left_Edges = 0;   
   for( Cur_S=Nb_Out_Edges-1; Cur_S>=0; --Cur_S )
   {
      if ( Edges_dy[Cur_S]>SLOPE_EPSILON ) // Right edge
      {
         FLT Xo;
         INT y, yf;
         INT _Xo_, _Slope_;

         if ( Edges_y[Cur_S]>=_RCst_.Clips[3] ) continue;
         if ( Edges_y[Cur_S+1]<=_RCst_.Clips[2] ) continue;

         Right_Edges[Nb_Right_Edges++] = Cur_S;

#if 0
         if ( Edges_dy[Cur_S]<SLOPE_EPSILON )
         {
            Edges_dy[Cur_S] = 0.0;  /* SLOPE_EPSILON; */
            Slope = Slope_Full[Cur_S] = Slopes[Cur_S] = 0.0;
         }
         else
#endif
         {
            Slopes[Cur_S] = 1.0/Edges_dy[Cur_S];
            Slope = Slope_Full[Cur_S] = Edges_dx[Cur_S] * Slopes[Cur_S];
         }
         {     /* Sub-pixel */
            FLT Yo;
            Yo = Edges_y[Cur_S];
            if ( _RCst_.Clips[2]>Yo ) FLT2INT( &y,&_RCst_.Clips[2] );
            else FLT2INT( &y,&Yo );
            Edges_ey[Cur_S] = Adjust_y = ( (FLT)y-Yo );
         }
 
         Xo = Edges_x[Cur_S];
         Xo += Adjust_y*Slope;

         if ( y<Scan_Start ) Scan_Start=y;
         Edges_yo[Cur_S] = y;
         {
            FLT Yf;
            Yf = Edges_y[Cur_S+1];
            if ( Yf>_RCst_.Clips[3] ) Yf = _RCst_.Clips[3];
            FLT2INT( &yf, &Yf );
         }

         _Xo_= (INT)ceil( Xo*65536.0 );
         _Slope_= (INT)ceil( Slope*65536.0 );
            /* Beware of signs ! */
         Edges_Xe[Cur_S] = _Xo_;
         Edges_dXe[Cur_S] = _Slope_;

         Scan_H += ( yf-y );
         while( y<yf )
         {
            Scan_Pt2[y] = _Xo_>>16;
            _Xo_ += _Slope_;
            y++;
         }
         Edges_yf[Cur_S] = yf;
      }
      else if ( Edges_dy[Cur_S]<-SLOPE_EPSILON ) /* Left edge */
      {
         FLT Xo;
         INT y, yf;
         INT _Xo_, _Slope_;

         if ( Edges_y[Cur_S]<=_RCst_.Clips[2] ) continue;
         if ( Edges_y[Cur_S+1]>=_RCst_.Clips[3] ) continue;

         Left_Edges[Nb_Left_Edges++] = Cur_S;
         Slopes[Cur_S] = 1.0/Edges_dy[Cur_S];
         Slope = Edges_dx[Cur_S] * Slopes[Cur_S];
         // Slope_Full[Cur_S] = Slope;

            /* Sub-pixel */
         {
            FLT Yo;
            Yo = Edges_y[Cur_S+1];
            if ( _RCst_.Clips[2]>Yo ) FLT2INT( &y,&_RCst_.Clips[2] );
            else FLT2INT( &y,&Yo );
            Edges_ey[Cur_S] = Adjust_y = ( (FLT)y-Yo );
         }
         Xo = Edges_x[Cur_S+1];
         Xo += Adjust_y*Slope;

         // if ( y<Scan_Start ) Scan_Start=y;
         Edges_yo[Cur_S] = y;
         {
            FLT Yf;
            Yf = Edges_y[Cur_S];
            if ( Yf>_RCst_.Clips[3] ) Yf = _RCst_.Clips[3];
            FLT2INT( &yf, &Yf );
         }
         _Xo_= (INT)ceil( Xo*65536.0 );
         _Slope_= (INT)ceil( Slope*65536.0 );
            /* Beware of signs ! */
         Edges_Xe[Cur_S] = _Xo_;
         Edges_dXe[Cur_S] = _Slope_;
         while( y<yf )
         {
            Scan_Pt1[y] = _Xo_>>16;
            _Xo_ += _Slope_;
            y++;
         }
         Edges_yf[Cur_S] = yf;
      }
   }
}

#endif         // UNIX

/******************************************************************/
/******************************************************************/

#ifdef UNIX

EXTERN void Rasterize_UV( )
{
   INT i;
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
         INT _U_, _dU_, _V_, _dV_, incU, incV;
         INT Xe, dXe; 

         dy = Edges_ey[j];
         dx = (FLT)Scan_Pt2[y] - Edges_x[j];

         dXe = Edges_dXe[j];
         if ( dXe>=0 ) Slope = (FLT)(dXe>>16);
         else Slope = -(FLT)( (-dXe)>>16 );

         Tmp = DxU + Slope*DyU;
         _dU_ = (INT)( Tmp );
         Tmp = Out_Vtx[j]->UV[0] * Mip_Scale;
         Tmp += dy*DxU + dx*DyU;
         _U_ = (INT)( Tmp + Uo_Mip );

         Tmp = DxV + Slope*DyV;
         _dV_ = (INT)( Tmp );
         Tmp = Out_Vtx[j]->UV[1] * Mip_Scale;
         Tmp += dy*DxV + dx*DyV;
         _V_ = (INT)( Tmp + Vo_Mip );

         Xe = Edges_Xe[j];
         if ( dXe<0 )
         {
            dXe = -dXe; Xe  = -Xe;
            incU = -dyU; incV = -dyV;
         }
         else { incU = dyU; incV = dyV; } // Hack !
         dXe &= 0xFFFF; Xe &= 0xFFFF;
         while( y<yf )
         {
            Scan_U[y] = _U_; _U_ += _dU_;
            Scan_V[y] = _V_; _V_ += _dV_;
            Xe += dXe;
            if ( Xe & 0x10000 )
            {
               Xe &= 0xFFFF;
               _U_ += incU;
               _V_ += incV;
            }
            y++;
         }
      }
   }
}
#endif   // UNIX

/******************************************************************/

#ifdef UNIX

   // Rasterize with N[0]/N[1] instead of UV[0]/UV[1]

EXTERN void Rasterize_UV2( )
{
   INT i;
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
         INT _U_, _dU_, _V_, _dV_, incU, incV;
         INT Xe, dXe; 

         dy = Edges_ey[j];
         dx = (FLT)Scan_Pt2[y] - Edges_x[j+1];

         dXe = Edges_dXe[j];
         if ( dXe>=0 ) Slope = (FLT)(dXe>>16);
         else Slope = -(FLT)( (-dXe)>>16 );

         Tmp = DxU + Slope*DyU;
         _dU_ = (INT)( Tmp );
         Tmp = Out_Vtx[j]->N[0] * Mip_Scale;
         Tmp += dy*DxU + dx*DyU;
         _U_ = (INT)( Tmp + Uo_Mip );

         Tmp = DxV + Slope*DyV;
         _dV_ = (INT)( Tmp );
         Tmp = Out_Vtx[j]->N[1] * Mip_Scale;
         Tmp += dy*DxV + dx*DyV;
         _V_ = (INT)( Tmp + Vo_Mip );

         Xe = Edges_Xe[j];
         if ( dXe<0 )
         {
            dXe = -dXe; Xe  = -Xe;
            incU = -dyU; incV = -dyV;
         }
         else { incU = dyU; incV = dyV; }    // Hack !
         dXe &= 0xFFFF; Xe &= 0xFFFF;
         while( y<yf )
         {
            Scan_U[y] = _U_; _U_ += _dU_;
            Scan_V[y] = _V_; _V_ += _dV_;
            Xe += dXe;
            if ( Xe & 0x10000 )
            {
               Xe &= 0xFFFF;
               _U_ += incU;
               _V_ += incV;
            }
            y++;
         }
      }
   }
}
#endif   // UNIX

/******************************************************************/

#ifdef UNIX

EXTERN void Rasterize_Grd( )
{
   INT i;
   i=Nb_Right_Edges;
   while( --i>=0 )
   {
      INT j;
      INT y, yf;

      j = Right_Edges[i];
      /* fprintf( stderr, "Right_Edges: 0x%x i=%d  j=%d\n",
         Right_Edges, i, j ); */

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
         Tmp = Out_Vtx[j]->N[0];
         Tmp += dy*DxS + dx*DyS;     // Sub-pixel
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
}

#endif   // UNIX

/******************************************************************/

#ifdef UNIX

EXTERN void Rasterize_iZ( )
{
   INT i;
   i=Nb_Right_Edges;
   while( --i>=0 )
   {
      INT j;
      INT y, yf;

      j = Right_Edges[i];
      /* fprintf( stderr, "Right_Edges: 0x%x i=%d  j=%d\n",
         Right_Edges, i, j ); */

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
         Tmp = Out_Vtx[j]->Inv_Z*_RCst_.Z_Scale;
         Tmp += _RCst_.Z_Off;
         Tmp += dy*DxS + dx*DyS;       // Sub-pixel
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
}

#endif   // UNIX

/******************************************************************/

#if 1 // #ifdef UNIX       // ASM version still UNFINISHED!!

EXTERN void Rasterize_UVc( )
{
   INT i,j;

   for( i=Nb_Right_Edges-1; i>=0; --i )
   {
      INT y, yf;
      FLT dx, dy;
      FLT Slope;
      FLT _U_, _dU_, incU;
      FLT _V_, _dV_, incV;
      FLT _iZ_, _diZ_, inc_iZ;
      INT Xe, dXe; 

      j = Right_Edges[i];
      y = Edges_yo[j];
      yf = Edges_yf[j];

      dy = Edges_ey[j];
      dx = (FLT)Scan_Pt2[y] - Edges_x[j];

      dXe = Edges_dXe[j];
      if ( dXe>=0 ) Slope = (FLT)(dXe>>16);
      else Slope = -(FLT)( (-dXe)>>16 );

      _iZ_ = aZ2*Out_Vtx[j]->Inv_Z;

      _dU_ = DxU + Slope*DyU;
      _U_ = Out_Vtx[j]->UV[0]*Mip_Scale + Uo_Mip;
      _U_ *= _iZ_;
      _U_ += dy*DxU + dx*DyU;

      _dV_ = DxV + Slope*DyV;
      _V_ = Out_Vtx[j]->UV[1]*Mip_Scale + Vo_Mip;
      _V_ *= _iZ_;
      _V_ += dy*DxV + dx*DyV;

      _iZ_ += dy*DxiZ + dx*DyiZ;
      _diZ_ = DxiZ + Slope*DyiZ;

      Xe = Edges_Xe[j];
      if ( dXe<0 )
      {
         dXe = -dXe; Xe  = -Xe;
         incU = -DyU; incV = -DyV; inc_iZ = -DyiZ;
      }
      else { incU = DyU; incV = DyV; inc_iZ = DyiZ; }
      dXe &= 0xFFFF; Xe &= 0xFFFF;
      while( y<yf )
      {
         Scan_UiZ[y] = _U_;
         _U_ += _dU_;
         Scan_ViZ[y] = _V_;
         _V_ += _dV_;
         Scan_iZ[y] = _iZ_; _iZ_ += _diZ_;
         Xe += dXe;
         if ( Xe & 0x10000 )
         {
            Xe &= 0xFFFF;
            _U_  += incU;
            _V_  += incV;
            _iZ_ += inc_iZ;
         }
         y++;
      }
   }

   for( i=Nb_Left_Edges-1; i>=0; --i )
   {
      INT y, yf;
      FLT dx, dy;
      FLT Slope;
      FLT _U_, _dU_, incU;
      FLT _V_, _dV_, incV;
      FLT _iZ_, _diZ_, inc_iZ;
      INT Xe, dXe; 

      j = Left_Edges[i];

      y = Edges_yo[j];
      yf = Edges_yf[j];

      dy =  (FLT)y-Edges_y[j];  // Edges_ey[j];
      dx = ( (FLT)Scan_Pt1[y] - Edges_x[j] );

      dXe = Edges_dXe[j];
      if ( dXe>=0 ) Slope = (FLT)(dXe>>16);
      else Slope = -(FLT)( (-dXe)>>16 );

      _iZ_ = aZ2*Out_Vtx[j]->Inv_Z;

      _dU_ = DxU + Slope*DyU;
      _U_ = Out_Vtx[j]->UV[0]*Mip_Scale + Uo_Mip;
      _U_ *= _iZ_;
      _U_ += dy*DxU + dx*DyU;

      _dV_ = DxV + Slope*DyV;
      _V_ = Out_Vtx[j]->UV[1]*Mip_Scale + Vo_Mip;
      _V_ *= _iZ_;
      _V_ += dy*DxV + dx*DyV;

      _iZ_ += dy*DxiZ + dx*DyiZ;
      _diZ_ = DxiZ + Slope*DyiZ;

      Xe = Edges_Xe[j];
//      fprintf( stderr, "Xe=%d dXe=%d\n", Xe, dXe );
      if ( dXe<0 )
      {
         dXe = -dXe; Xe  = -Xe;
         incU = -DyU; incV = -DyV; inc_iZ = -DyiZ;
      }
      else { incU = DyU; incV = DyV; inc_iZ = DyiZ; }
      dXe &= 0xFFFF; Xe &= 0xFFFF;

      while( y<yf )
      {
         Scan_UiZ_2[y] = _U_; _U_ += _dU_;
         Scan_ViZ_2[y] = _V_; _V_ += _dV_;
         Scan_iZ_2[y] = _iZ_; _iZ_ += _diZ_;
         Xe += dXe;
         if ( Xe & 0x10000 )
         {
            Xe &= 0xFFFF;
            _U_  += incU;
            _V_  += incV;
            _iZ_ += inc_iZ;
         }
         y++;
      }
   }
}
#endif   // 1

#if 0    // OLD version

EXTERN void Rasterize_UVc( )
{
   INT i;

   i=Nb_Right_Edges;
   while( --i>=0 )
   {
      INT j, y, yf;

      j = Right_Edges[i];

      y = Edges_yo[j];
      yf = Edges_yf[j];

      {
         FLT dx, dy, Slope;
         FLT _U_, _dU_, incU;
         FLT _V_, _dV_, incV;
         FLT _iZ_, _diZ_, inc_iZ;
         INT Xe, dXe; 

         dy = Edges_ey[j];
         dx = (FLT)Scan_Pt2[y] - Edges_x[j];

         dXe = Edges_dXe[j];
         if ( dXe>=0 ) Slope = (FLT)(dXe>>16);
         else Slope = -(FLT)( (-dXe)>>16 );

         _iZ_ = aZ2*Out_Vtx[j]->Inv_Z;

         _dU_ = DxU + Slope*DyU;
         _U_ = Out_Vtx[j]->UV[0]*Mip_Scale + Uo_Mip;
         _U_ *= _iZ_;
         _U_ += dy*DxU + dx*DyU;

         _dV_ = DxV + Slope*DyV;
         _V_ = Out_Vtx[j]->UV[1]*Mip_Scale + Vo_Mip;
         _V_ *= _iZ_;
         _V_ += dy*DxV + dx*DyV;

         _iZ_ += dy*DxiZ + dx*DyiZ;
         _diZ_ = DxiZ + Slope*DyiZ;

         Xe = Edges_Xe[j];
         if ( dXe<0 )
         {
            dXe = -dXe; Xe  = -Xe;
            incU = -DyU; incV = -DyV; inc_iZ = -DyiZ;
         }
         else { incU = DyU; incV = DyV; inc_iZ = DyiZ; }
         dXe &= 0xFFFF; Xe &= 0xFFFF;
         while( y<yf )
         {
//            if ( _U_>0.0 )
                Scan_UiZ[y] = _U_;
//            else Scan_UiZ[y] = 0.0; 
            _U_ += _dU_;
//            if ( _V_>0.0 ) 
               Scan_ViZ[y] = _V_;
//            else Scan_ViZ[y] = 0.0;
            _V_ += _dV_;
            Scan_iZ[y] = _iZ_; _iZ_ += _diZ_;
            Xe += dXe;
            if ( Xe & 0x10000 )
            {
               Xe &= 0xFFFF;
               _U_  += incU;
               _V_  += incV;
               _iZ_ += inc_iZ;
            }
            y++;
         }
      }
   }
}

#endif    // UNIX

/******************************************************************/

