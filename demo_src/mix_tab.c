/*
 * Misc. tables building
 *
 * Skal 98
 ***************************************/

#include "main.h"
extern double pow( double, double );

extern PIXEL Drv_Best_Match_RGB(INT,INT,INT, COLOR_ENTRY *,INT);

/*****************************************************************/

EXTERN PIXEL *Build_Mix_Tab( COLOR_ENTRY *Map, INT Nb, FLT x1, FLT x2 )
{
   INT i, n;
   FLT A;
   PIXEL *Tab;

   printf( "\n...Building MIX table (%.2f/%.2f)...\n", x1, x2 );
   Tab = New_Fatal_Object( Nb*256, PIXEL );
   A = 1.0f/(x1+x2);

   for( i=0; i<Nb; ++i )
   {
      for( n=0; n<Nb; ++n )
      {
         FLT x; INT R, G, B;
         x = (x1*Map[i][RED_F]  + x2*Map[n][RED_F])*A;
         R = (INT)x;
         x = (x1*Map[i][GREEN_F]+ x2*Map[n][GREEN_F])*A;
         G = (INT)x;
         x = (x1*Map[i][BLUE_F] + x2*Map[n][BLUE_F])*A;
         B = (INT)x;
         Tab[i+256*n] = Drv_Best_Match_RGB( R, G, B, Map, Nb );
      }
   }
   return( Tab );
}

EXTERN PIXEL *Build_Sat_Tab( COLOR_ENTRY *Map, INT Nb, FLT x1, FLT x2 )
{
   INT i, n;
   PIXEL *Tab;

   printf( "\n...Building SAT table (%.2f/%.2f)...\n", x1, x2 );
   Tab = New_Fatal_Object( Nb*256, PIXEL );

   for( i=0; i<Nb; ++i )
   {
      for( n=0; n<Nb; ++n )
      {
         FLT x; INT R, G, B;
         x = (x1*Map[i][RED_F]  + x2*Map[n][RED_F]); if (x>255.0) x=255.0;
         R = (INT)x;
         x = (x1*Map[i][GREEN_F]+ x2*Map[n][GREEN_F]); if (x>255.0) x=255.0;
         G = (INT)x;
         x = (x1*Map[i][BLUE_F] + x2*Map[n][BLUE_F]); if (x>255.0) x=255.0;
         B = (INT)x;
         Tab[i+256*n] = Drv_Best_Match_RGB( R, G, B, Map, Nb );
      }
   }
   return( Tab );
}

EXTERN PIXEL *Build_Sat_Tab_II( COLOR_ENTRY *Map, INT Nb, 
   COLOR_ENTRY *Map2, INT Nb2, FLT x1, FLT x2 )
{
   INT i, n;
   PIXEL *Tab;

   printf( "\n...Building SAT_II table (%.2f/%.2f)...\n", x1, x2 );

   Tab = New_Fatal_Object( Nb2*256, PIXEL );

   for( i=0; i<Nb; ++i )
   {
      for( n=0; n<Nb2; ++n )
      {
         FLT x; INT R, G, B;
         x = (x1*Map[i][RED_F]  + x2*Map2[n][RED_F]); if (x>255.0) x=255.0;
         R = (INT)x;
         x = (x1*Map[i][GREEN_F]+ x2*Map2[n][GREEN_F]); if (x>255.0) x=255.0;
         G = (INT)x;
         x = (x1*Map[i][BLUE_F] + x2*Map2[n][BLUE_F]); if (x>255.0) x=255.0;
         B = (INT)x;
         Tab[i+256*n] = Drv_Best_Match_RGB( R, G, B, Map, Nb );
      }
   }
   return( Tab );
}

EXTERN PIXEL *Build_Max_Tab( COLOR_ENTRY *Map, INT Nb )
{
   INT i, n;
   PIXEL *Tab;

   printf( "\n...Building MAX table...\n" );
   Tab = New_Fatal_Object( Nb*256, PIXEL );
   srandom( 1242 );

   for( i=0; i<Nb; ++i )
   {
      for( n=0; n<Nb; ++n )
      { 
         INT R, G, B;
         R = (Map[i][RED_F]>Map[n][RED_F])? (INT)Map[i][RED_F] : (INT)Map[n][RED_F];
         G = (Map[i][GREEN_F]>Map[n][GREEN_F])? (INT)Map[i][GREEN_F] : (INT)Map[n][GREEN_F];
         B = (Map[i][BLUE_F]>Map[n][BLUE_F])? (INT)Map[i][BLUE_F] : (INT)Map[n][BLUE_F];
         Tab[i+256*n] = Drv_Best_Match_RGB( R, G, B, Map, Nb );
      }
   }
   return( Tab );
}

EXTERN PIXEL *Build_Mix_Tab_II( COLOR_ENTRY *Map, INT Nb, 
   COLOR_ENTRY *Map2, INT Nb2, FLT x1, FLT x2 )
{
   INT i, n;
   FLT A;
   PIXEL *Tab;

   printf( "\n...Building MIX_II table (%.2f/%.2f)...\n", x1, x2 );
   Tab = New_Fatal_Object( Nb2*256, PIXEL );
   A = 1.0f/(x1+x2);

   for( i=0; i<Nb; ++i )
   {
      for( n=0; n<Nb2; ++n )
      {
         FLT x; INT R, G, B;
         x = (x1*Map[i][RED_F]  + x2*Map2[n][RED_F])*A;
         R = (INT)x;
         x = (x1*Map[i][GREEN_F]+ x2*Map2[n][GREEN_F])*A;
         G = (INT)x;
         x = (x1*Map[i][BLUE_F] + x2*Map2[n][BLUE_F])*A;
         B = (INT)x;
         Tab[i+256*n] = Drv_Best_Match_RGB( R, G, B, Map2, Nb2 );
      }
   }
   return( Tab );
}

EXTERN PIXEL *Build_Weight_Tab( COLOR_ENTRY *Map, FLT *Wght, INT Nb_Col, INT W )
{
   INT i, n;
   PIXEL *Tab;

   printf( "\n...Building WGHT table...\n" );
   Tab = New_Fatal_Object( W*256, PIXEL );

   for( i=0; i<W; ++i )
      for( n=0; n<Nb_Col; ++n )
      {
         INT R, G, B;
         R = (INT)( Wght[i]*Map[n][RED_F] );
         G = (INT)( Wght[i]*Map[n][GREEN_F] );
         B = (INT)( Wght[i]*Map[n][BLUE_F] );
         Tab[n+256*i] = Drv_Best_Match_RGB( R, G, B, Map, Nb_Col );
//         fprintf( stderr, "i=%d n=%d    Wght[]=%f  R=%d G=%d B=%d\n",
//            i, n, Wght[i], R,G,B );
      }
   return( Tab );
}

/*****************************************************************/

EXTERN void Gamma_Saturate_Base(
   INT Nb, FCOLOR Base, FCOLOR Light, FCOLOR Ambient, 
   FLT Gamma, FLT Sat, FCOLOR *Dst )
{
   INT i;
   FCOLOR Light2;

   Light2[0] = Base[0]*Sat + (1.0f-Sat)*Ambient[0];
   Light2[1] = Base[1]*Sat + (1.0f-Sat)*Ambient[1];
   Light2[2] = Base[2]*Sat + (1.0f-Sat)*Ambient[2];
   Light2[3] = Base[3]*Sat + (1.0f-Sat)*Ambient[3];

   for( i=0; i<128; ++i )
   {
      FLT x;
      x = (FLT)(i)/128.0f;
      if ( (Gamma>0.0) && (x!=0.0) ) x = (FLT)pow( x, Gamma );
      // y = Sat + x*(1.0-Sat);
      Dst[i][0] = Base[0]*x + (1.0f-x)*Light2[0];
      Dst[i][1] = Base[1]*x + (1.0f-x)*Light2[1];
      Dst[i][2] = Base[2]*x + (1.0f-x)*Light2[2];
      Dst[i][3] = Base[3]*x + (1.0f-x)*Light2[3];
   }

   Light2[0] = Base[0]*Sat + (1.0f-Sat)*Light[0];
   Light2[1] = Base[1]*Sat + (1.0f-Sat)*Light[1];
   Light2[2] = Base[2]*Sat + (1.0f-Sat)*Light[2];
   Light2[3] = Base[3]*Sat + (1.0f-Sat)*Light[3];

   for( i=128; i<Nb; ++i )
   {
      FLT x;
      x = (FLT)(i-128)/(FLT)(Nb-1-128);
//      x = (FLT)(i)/(FLT)(Nb-1);
      if ( Gamma != 1.0 && x!=0.0 ) x = (FLT)pow( (double)x, Gamma );
      Dst[i][0] = ( Base[0]*(1.0f-x)+Light2[0]*x );
      Dst[i][1] = ( Base[1]*(1.0f-x)+Light2[1]*x );
      Dst[i][2] = ( Base[2]*(1.0f-x)+Light2[2]*x );
      Dst[i][3] = ( Base[3]*(1.0f-x)+Light2[3]*x );
   }
}

/*********************************************************************/
