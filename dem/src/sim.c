/*
 * Simulation (unused)
 *
 * Skal98 (skal.planet-d.net)
 ***************************************/

#include "demo.h"

/********************************************************************/

typedef struct {

   FLT x, y;
   INT n[3];
   USHORT Flag, Stamp;
} VTX;

#define VTX_FIX   0x01
#define VTX_OK    0x02

static VTX *Vtx = NULL;
static INT Nb_Vtx = 0;
static INT Top_Vtx = 0;
static USHORT Stamp = 0;
#define MAX_VTX 300

static FLT Pm_Amp = 1.0, P1_Amp = 1.0, P2_Amp = 1.0;
static FLT Prb_m = 1.0, mT = 2.1, mT2 = 1.1;
static FLT Prb_mI = 0.04;

/********************************************************************/

EXTERN void Pre_Cmp_Vtx( )
{
}

EXTERN void Init_Vtx( INT Param )
{
   switch( Param )
   {
      case 0:
         Vtx = New_Fatal_Object( MAX_VTX, VTX );
         Vtx[0].x = -1.0; Vtx[0].y = 1.0; Vtx[0].Flag = VTX_FIX;
         Vtx[0].n[0] = 4; Vtx[0].n[1] = Vtx[0].n[2] = -1;

         Vtx[1].x =  1.0; Vtx[1].y = 1.0; Vtx[1].Flag = VTX_FIX;
         Vtx[1].n[0] = 5; Vtx[1].n[1] = Vtx[1].n[2] = -1;

         Vtx[2].x = -1.0; Vtx[2].y =-1.0; Vtx[2].Flag = VTX_FIX;
         Vtx[2].n[0] = 6; Vtx[2].n[1] = Vtx[2].n[2] = -1;

         Vtx[3].x =  1.0; Vtx[3].y =-1.0; Vtx[3].Flag = VTX_FIX;
         Vtx[3].n[0] = 7; Vtx[3].n[1] = Vtx[3].n[2] = -1;

         Vtx[4].x = -0.6; Vtx[4].y = 0.6; Vtx[4].Flag = VTX_OK;
         Vtx[4].n[0] = 0; Vtx[4].n[1] = 5; Vtx[4].n[2] = 6;

         Vtx[5].x =  0.6; Vtx[5].y = 0.6; Vtx[5].Flag = VTX_OK;
         Vtx[5].n[0] = 1; Vtx[5].n[1] = 7; Vtx[5].n[2] = 4;

         Vtx[6].x = -0.6; Vtx[6].y =-0.6; Vtx[6].Flag = VTX_OK;
         Vtx[6].n[0] = 2; Vtx[6].n[1] = 4; Vtx[6].n[2] = 7;

         Vtx[7].x =  0.6; Vtx[7].y =-0.6; Vtx[7].Flag = VTX_OK;
         Vtx[7].n[0] = 3; Vtx[7].n[1] = 6; Vtx[7].n[2] = 5;

         Nb_Vtx = 8; Top_Vtx = 7;
         Stamp = 0;
         Pm_Amp = 1.0; P1_Amp = 1.0; P2_Amp = 1.0;
      break;
   }
}

/********************************************************************/

static INT New_Vtx( )
{
   INT i;
   for( i=0; i<=Top_Vtx; ++i )
      if ( Vtx[i].Flag == 0x00 )
         break;
   if (i==MAX_VTX) return( -1 );
   Vtx[i].Flag = VTX_OK;
   Vtx[i].Stamp = Stamp;
   Nb_Vtx++;
   if ( i>Top_Vtx ) Top_Vtx = i;
   return( i );
}

static void Remove_Vtx( INT n )
{
   Vtx[n].Flag = 0x00;
   Nb_Vtx--;
   if ( Nb_Vtx==0 ) { Top_Vtx=0; return; }
   if ( Top_Vtx==n ) while( Vtx[Top_Vtx].Flag == 0x00 ) Top_Vtx--;
}

static FLT r[3], a[3], rm, am;
static FLT px[3], py[3];
static INT vm;
static void Do_Dst( FLT x, FLT y, VTX *b )
{
   FLT t;
   VTX *v;

   v = Vtx + b->n[0];
   px[0] = ( v->x-x ); r[0] = px[0]*px[0];
   py[0] = ( v->y-y ); r[0]+= py[0]*py[0]; r[0] = sqrt(r[0]);
   rm = r[0]; vm = 0;
   v = Vtx + b->n[1];
   px[1] = ( v->x-x ); r[1] = px[1]*px[1];
   py[1] = ( v->y-y ); r[1]+= py[1]*py[1]; r[1] = sqrt(r[1]);
   if (r[1]<rm ) { rm = r[1]; vm = 1; }
   v = Vtx + b->n[2];
   px[2] = ( v->x-x ); r[2] = px[2]*px[2];
   py[2] = ( v->y-y ); r[2]+= py[2]*py[2]; r[2] = sqrt(r[2]);
   if (r[2]<rm ) { rm = r[2]; vm = 2; }
   t = r[0]*r[1]; if ( t=0.0 ) a[0] = 0.0;
   else a[0] = (px[0]*px[1] + py[0]*py[1])/t;
   t = r[1]*r[2]; if ( t=0.0 ) a[1] = 0.0;
   else a[1] = (px[1]*px[2] + py[1]*py[2])/t;
   t = r[2]*r[0]; if ( t=0.0 ) a[2] = 0.0;
   else a[2] = (px[2]*px[0] + py[2]*py[0])/t;

#if 0
   am = a[0];
   if ( a[1]<am ) am = a[1];
   if ( a[2]<am ) am = a[2];
#else
   am = ( a[0] + a[1] + a[2] ) / 3.0;
#endif
   am -= cos( M_PI/3.0 );
}

static FLT Rand_I( )
{
   return( 2.0*(Random()&0xFF)/256.0-1.0 );
}

static FLT Rand_II( )
{
   return( (Random()&0xFF)/256.0 );
}
static INT Search_Branch( VTX *v, INT k )
{
   if ( v->n[0]==k ) return(0);
   if ( v->n[1]==k ) return(1);
   return(2);
}
static void Update_Vtx( INT Nb )
{
   while( Nb-- )
   {
      INT n;
      FLT x, y, t, p;
      VTX *v;
      VTX *n1, *n2, *n3;

      do { n = Random() % (Top_Vtx+1); } 
      while( Vtx[n].Flag!=VTX_OK );
      v = &Vtx[n];
      x = v->x + Pm_Amp*Rand_I( )/320.0;
      y = v->y + Pm_Amp*Rand_I( )/320.0;
      Do_Dst( x, y, v );

      n1 = Vtx + v->n[0];
      n2 = Vtx + v->n[1];
      n3 = Vtx + v->n[2];

         // Mute?
      if ( (rm>0.1) && (Rand_II( )<Prb_mI) )
      {
         INT m1, m2;
         INT k;
         m1 = New_Vtx(); if ( m1==-1 ) continue;
         m2 = New_Vtx(); if ( m2==-1 ) { Remove_Vtx(m1); continue; }
         k = Search_Branch( n2, n ); n2->n[k] = m1;
         k = Search_Branch( n3, n ); n3->n[k] = m2;
         Vtx[m1].n[0] = n; Vtx[m1].n[1] = m2; Vtx[m1].n[2] = v->n[1]; //(=n2)
         Vtx[m2].n[0] = m1; Vtx[m2].n[1] = n; Vtx[m2].n[2] = v->n[2]; //(=n3)
         v->n[1] = m2; v->n[2] = m1;   // update *v last...
         Vtx[m1].x = v->x + px[1]*( .4 + .4*Rand_II( ) );
         Vtx[m1].y = v->y + py[1]*( .4 + .4*Rand_II( ) );
         Vtx[m2].x = v->x + px[2]*( .4 + .4*Rand_II( ) );
         Vtx[m2].y = v->y + py[2]*( .4 + .4*Rand_II( ) );
         v->x = v->x + px[0]*( .4 + .4*Rand_II( ) );
         v->y = v->y + py[0]*( .4 + .4*Rand_II( ) );
         continue;
      }
          // Move?
      t = exp( -am*am*mT );
      p = Prb_m*Rand_II( );
      if ( p<t ) { Vtx[n].x = x; Vtx[n].y = y; continue; }
      t = ( rm-0.08 );
      t = exp( -t*t*mT2 );
      p = Prb_m*Rand_II( );
      if ( p<t ) { Vtx[n].x = x; Vtx[n].y = y; continue; }
   }
}

/********************************************************************/

static void Join( FLT x, FLT y, VTX *v )
{
   if ( v->Stamp>=Stamp ) return;
   ((void (*)(FLT,FLT,FLT,FLT))Primitives[RENDER_LINE])( 
      x*150+160.0, y*80+100.0,
      v->x*150+160.0, v->y*80+100.0 );
}

static void Draw_All_Vtx( )
{
   INT i;
   Stamp++;
   for( i=0; i<=Top_Vtx; ++i )
   {
      FLT xi, yi;
      VTX *v;
      Vtx[i].Stamp = Stamp;
      if ( Vtx[i].Flag==0x00 ) continue;
      xi = Vtx[i].x; yi = Vtx[i].y;
      if ( Vtx[i].Flag==VTX_FIX )
      {
         // draw only n0-n[0]
         v = Vtx + Vtx[i].n[0];
         Join( xi, yi, v );
      }
      else
      {
         v = Vtx + Vtx[i].n[0]; Join( xi, yi, v );
         v = Vtx + Vtx[i].n[1]; Join( xi, yi, v );
         v = Vtx + Vtx[i].n[2]; Join( xi, yi, v );         
      }      
   }
}

/********************************************************************/

EXTERN void Loop_Vtx( )
{
   memset( VB(VSCREEN).Bits, 0, VB(VSCREEN).Size );
   Update_Vtx( Nb_Vtx );
   Draw_All_Vtx( );
}

/********************************************************************/

EXTERN void Close_Vtx( )
{
   M_Free( Vtx );
   Nb_Vtx = Top_Vtx = 0;
}

/********************************************************************/
