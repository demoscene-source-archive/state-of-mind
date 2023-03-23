/*****************************************************
 *          Color utilities. Old.
 *
 *                       Skal97
 *****************************************************/

#include "main.h"
#include "btm.h"
#include "color.h"

extern double pow( double, double );

/***************************************************************************/

EXTERN void Saturate_Pal( PIXEL *Src, PIXEL *Dst,
	PIXEL r1, PIXEL g1, PIXEL b1, PIXEL r2, PIXEL g2, PIXEL b2 )
{
   int i, j, n;
   PIXEL r, g, b;

   if ( Dst == NULL || Src == NULL ) return;

   for( j=0; j<16; ++j )
   {
      r = Src[ j*3   ];
      g = Src[ j*3+1 ];
      b = Src[ j*3+2 ];

      for( i=0; i<16; ++i )
      {
         n = (i+0)*16 + j;
         Dst[ n*3     ] = ( r + (r2-r)*i/15 );
         Dst[ n*3 + 1 ] = ( g + (g2-g)*i/15 );
         Dst[ n*3 + 2 ] = ( b + (b2-b)*i/15 );
      }
   }
}

EXTERN void Saturate_Pal2( PIXEL *Src, PIXEL *Dst, int Nb_Col, int Nb,
	PIXEL R, PIXEL G, PIXEL B, double Gamma )
{
   int i, j, n;
   PIXEL r, g, b;

   if ( Dst == NULL || Src == NULL ) return;

      /* Saturate Nb continuous colors with Nb shades */
      /* Light color = (R,G,B). + Gamma exp */

   if ( Nb<2 ) return;

   for( j=0; j<Nb_Col; ++j )
   {
      r = *Src++;
      g = *Src++;
      b = *Src++;

      for( i=0; i<Nb; ++i )
      {
         double x;
         x = (double)i/(Nb-1);
         if ( x!=0.0 && Gamma!=1.0 ) x = pow( x, Gamma );
         n = i*Nb_Col + j;
         Dst[ n*3     ] = (BYTE)( (1.0-x)*r + (R-r)*x );
         Dst[ n*3 + 1 ] = (BYTE)( (1.0-x)*g + (G-g)*x );
         Dst[ n*3 + 2 ] = (BYTE)( (1.0-x)*b + (B-b)*x );
      }
   }
}

EXTERN void Saturate_Color( PIXEL *Pal, 
   int How_Many, int Bunch, int Nb, 
   float Ambient, float Gamma, float Per,
   PIXEL rf, PIXEL gf, PIXEL bf )
{
   int i, j;
   float ri, gi, bi;
   float Dr, Dg, Db;

   if ( Pal == NULL ) return;

   /*  Saturate Per% of color (Ambient*Col) with color (rf,gf,bf)  */

   for( j=0; j<How_Many; ++j )
   {
      ri = Ambient*(float)Pal[ 0 ]; 
      gi = Ambient*(float)Pal[ 1 ]; 
      bi = Ambient*(float)Pal[ 2 ];
      /* Final col is Per% of (rf,gf,bf) and (1-Per)% of initial Col */
      Dr = Per*(float)(rf) + (1.0f-Per)*(float)Pal[ 0 ];
      Dg = Per*(float)(gf) + (1.0f-Per)*(float)Pal[ 1 ];
      Db = Per*(float)(bf) + (1.0f-Per)*(float)Pal[ 2 ];

      for( i=0; i<Nb; ++i )
      {
         double x;
         x = (double)(i)/(double)(Nb-1);
         if ( Gamma!=1.0 && x!=0.0 ) x = pow( x, (double)Gamma );
         Pal[ 3*i*Bunch ] =   ( PIXEL )( ri*(1.0-x)+Dr*x );
         Pal[ 3*i*Bunch+1 ] = ( PIXEL )( gi*(1.0-x)+Dg*x );
         Pal[ 3*i*Bunch+2 ] = ( PIXEL )( bi*(1.0-x)+Db*x );
      }
      Pal += 3;      
   }
}

EXTERN void Gamma_Saturate( FCOLOR *CMap, INT Nb, INT Skip,
   FCOLOR Base, FCOLOR Light, double Gamma, double Sat )
{
   INT i;
   FCOLOR Light2;

   Light2[0] = (FLT)( Base[0]*Sat + (1.0-Sat)*Light[0] );
   Light2[1] = (FLT)( Base[1]*Sat + (1.0-Sat)*Light[1] );
   Light2[2] = (FLT)( Base[2]*Sat + (1.0-Sat)*Light[2] );
   Light2[3] = (FLT)( Base[3]*Sat + (1.0-Sat)*Light[3] );

   if ( Skip<=0 ) Skip = 1;
   for( i=0; i<Nb; ++i )
   {
      FLT x;
      x = (FLT)(i)/(FLT)(Nb-1);
      if ( Gamma!=1.0 && x!=0.0 ) x = (FLT)pow( (double)x, Gamma );
      (*CMap)[0] = (FLT)( Base[0]*(1.0-x)+Light2[0]*x );
      (*CMap)[1] = (FLT)( Base[1]*(1.0-x)+Light2[1]*x );
      (*CMap)[2] = (FLT)( Base[2]*(1.0-x)+Light2[2]*x );
      (*CMap)[3] = (FLT)( Base[3]*(1.0-x)+Light2[3]*x );
      CMap += Skip;
   }
}

/********************************************************************/

EXTERN INT Match_Best_RGB( PIXEL *Pal, INT Size, INT R, INT G, INT B )
{
   INT Dist, D, Index;
   INT n, n3;

   Dist = 0x7FFFFFFF;
   Index = 0;
   n=Size-1;
   n3=n*3;

   for( ; n>=0; n-- )
   {
      INT r;
      r = R-Pal[n3]; D = r*r;
      r = G-Pal[n3+1]; D += r*r;
      r = B-Pal[n3+2]; D += r*r;
      if ( D<Dist ) { Dist = D; Index = n; }
      n3 -= 3;
   }
   return( Index );
}

/********************************************************************/

   // old junk

#if   0

EXTERN PIXEL *Prepare_Blur_Pal( BITMAP *Btm )
{
   int i, j;

      /* 50% - 50% mixing */
   M_Free( Btm->Blur_Pal );
   Btm->Nb_Blur_Col = Btm->Nb_Col*256;
   Btm->Blur_Pal = New_Fatal_Object( Btm->Nb_Blur_Col, PIXEL );
   BZero( Btm->Blur_Pal, Btm->Nb_Blur_Col );
   for( i=0; i<Btm->Nb_Col; ++i )
   {
      int r0, b0, g0;
      r0 = Btm->Pal[i*3];
      g0 = Btm->Pal[i*3+1];
      b0 = Btm->Pal[i*3+2];
      for( j=0; j<Btm->Nb_Col; ++j )
      {
         int n, Best, Best_Dist;
         int r1, b1, g1;
         r1 = ( Btm->Pal[j*3] + r0 ) / 2;
         g1 = ( Btm->Pal[j*3+1] +g0 ) / 2;
         b1 = ( Btm->Pal[j*3+2] + b0 ) / 2;
         Best_Dist = 0x7FFFFFFF;
         Best = j;
         for( n=0; n<Btm->Nb_Col; ++n )
         {
            int Dist, r, g, b;
            r = Btm->Pal[n*3] - r1;
            g = Btm->Pal[n*3+1] - g1;
            b = Btm->Pal[n*3+2] - b1;
            Dist = r*r + g*g + b*b;
            if ( Dist<Best_Dist ) { Best = n; Best_Dist=Dist; }
         }
         Btm->Blur_Pal[ i+(j<<8) ] = Best;
      }
   }

   return( Btm->Blur_Pal );
}

EXTERN PIXEL *Prepare_Blur_Pal_2( BITMAP *Btm )
{
   int i, j;

      /* 5/8 - 3/8 mixing */
   M_Free( Btm->Blur_Pal );
   Btm->Nb_Blur_Col = Btm->Nb_Col*256;
   Btm->Blur_Pal = New_Fatal_Object( Btm->Nb_Blur_Col, PIXEL );
   BZero( Btm->Blur_Pal, Btm->Nb_Blur_Col );
   for( i=0; i<Btm->Nb_Col; ++i )
   {
      int r0, b0, g0;

      r0 = Btm->Pal[i*3];
      g0 = Btm->Pal[i*3+1];
      b0 = Btm->Pal[i*3+2];
      for( j=0; j<Btm->Nb_Col; ++j )
      {
         int n, Best, Best_Dist;
         int r1, b1, g1;

         r1 = ( 5*Btm->Pal[j*3] + 3*r0 ) / 8;
         g1 = ( 5*Btm->Pal[j*3+1] + 3*g0 ) / 8;
         b1 = ( 5*Btm->Pal[j*3+2] + 3*b0 ) / 8;
         Best_Dist = 0x7FFFFFFF;
         Best = j;
         for( n=0; n<Btm->Nb_Col; ++n )
         {
            int r, g, b, Dist;
            r = Btm->Pal[n*3] - r1;
            g = Btm->Pal[n*3+1] - g1;
            b = Btm->Pal[n*3+2] - b1;
            Dist = r*r + g*g + b*b;
            if ( Dist<Best_Dist ) { Best = n; Best_Dist=Dist; }
         }
         Btm->Blur_Pal[ i+(j<<8) ] = Best;
      }
   }

   return( Btm->Blur_Pal );
}

#endif

/********************************************************************/

