/*
 * Galaxy simulated with Particles...
 *
 *
 * P(x) = (1-x^2)/(1+a.x^2). Where x = r/R, r in [0,R], a>0
 * => P(r) = sqrt[ (1-y)/(1+b.y) ]. Where y in [0,1]
 * and b = a.R^2
 *
 * Cam matrix: [ Rx... ]
 *             [ Ry... ]
 *             [ Rz... ]
 * Particle a infinity pointing toward n = (nx,ny,nz)
 *  proj=>  _x_ = (Rx.n)/(Rz.n) * Lx + Cx
 *          _y_ = (Ry.n)/(Rz.n) * Ly + Cy
 *
 * Inverse Projection: UV coord in direction n=(x,y,1)
 * 
 *  [x',y',z'] = M^-1.n 
 *  puis:
 *     V = atan2( x'. Rzz, 1-x'.Rzx -y'Rzy )
 *     U = atan2( y'. sin(V), x' )
 *
 *
 * Skal 98
 ***************************************/

#include "demo.h"

typedef struct {
#pragma pack(1)

   FLT z;
   FLT t, dt;
   FLT r, dr;
   FLT x, y, s;
   SPRITE *Sp;
//   FLT T;

#pragma pack()
} PTCL;

static PTCL *Ptcl =  NULL;
static INT Nb_Ptcl;
#define MAX_PTCL 900    // 1300?
#define BETA 5.2f
#define R0 1.0f
#define Z0 0.26f
static TRANSFORM Trf;

static INT Ptcl_0 = 0, Ufo_x, Ufo_y;
static VBUFFER Ufo1, Ufo2, Ufo3, Ufo4, *Ufo=NULL;

EXTERN SPRITE *Sprs_Glxy[4] = { NULL };
EXTERN BITMAP *Sky = NULL;
EXTERN MATRIX2 Ptcl_M;

/********************************************************************/

static void Set_Ptcl_I( PTCL *Ptr )
{
   FLT z, r, t, dt;
   r = ( Random()&0xFFFF )/65536.0f;
   t = ( Random()&0xFFFF )/65536.0f;
   z = ( Random()&0xFFFF )/32768.0f  - 1.0f;
   if ( r<.3 ) 
   {
      z *= 1.0f-r*r/(0.3f*0.3f);
      t *= 2.0f*M_PI;
      r = (FLT)pow( r, 0.9f );
   }
   else
   {
      r = (FLT)pow( r, 1.4 );
      z *= (FLT)sqrt( (1.0f-r) / (1.0f+BETA*r) );
      t = t*t*t*M_PI;
      if ( Random()&0x01 ) t += M_PI;
      r = 0.1f + 0.5f*r*( 1.0f + (FLT)cos( t )*(FLT)cos( t ) );
   }
   dt = -r*2.0f*M_PI* ( 1.0f + .01f*((Random()&0xFFFF)/65536.0f) );
   Ptr->t = t;
   Ptr->dt = dt;
   Ptr->z = Z0 * z;
   Ptr->r = R0 * r;
   Ptr->dr = 0.5f + (Random()&0xFF)*(1.2f/256.0f);
   if ( (Random()&0x10) ) Ptr->Sp = Sprs_Glxy[ 0 ];
   else Ptr->Sp = Sprs_Glxy[ 1 + ( Random()%3 ) ];
}

static void Setup_All_Ptcls( )
{
   INT i;
   Ptcl = New_Fatal_Object( MAX_PTCL, PTCL );
   Nb_Ptcl = MAX_PTCL;

   for( i=0; i<Nb_Ptcl; ++i ) Set_Ptcl_I( Ptcl+i );

   Trf.Type = TRANSF_RZYX;
   Trf.Rot.Rot[_X] = M_PI/2.0f-.42f;
   Trf.Rot.Rot[_Y] = 0.25f;
   Trf.Rot.Rot[_Z] = 0.0;
   Trf.Scale[_X] = Trf.Scale[_Y] = Trf.Scale[_Z] = 1.0f;
   Trf.Pivot[0] = Trf.Pivot[1] = Trf.Pivot[2] = 0.0;
   Trf.Pos[0] =  0.0;
   Trf.Pos[1] =  0.0;
   Trf.Pos[2] =  1.0f;
}

/********************************************************************/

static void GIF_To_VBuf( STRING Name, VBUFFER *V )
{
   BITMAP *Tmp;

   Tmp = Load_GIF( Name );

   V->Nb_Col = Tmp->Nb_Col;
   V->Type = VCMAP;
   V->Flags = 0x00;
   V->W = Tmp->Width;
   V->H = Tmp->Height;
   V->BpS = Tmp->Width;
   V->Size = V->H*V->BpS;
   V->Quantum = 1;
   V->Bits = (void*)New_Fatal_Object( V->Size, BYTE );
   memcpy( V->Bits, Tmp->Bits,V->Size );

   Check_CMap_VBuffer( V, Tmp->Nb_Col );
   CMap_To_16bits( (void*)V->CMap, Tmp->Pal, Tmp->Nb_Col, 0x4565 );
   Destroy_Bitmap( Tmp );
}

EXTERN void Pre_Cmp_Ptcl( )
{
      // Hack!!

   Sprs_Glxy[0] = Load_Sprite_777_JPEG( "sp.jpg" );
   Sprs_Glxy[0]->dV = 16;

   Sprs_Glxy[1] = New_Fatal_Object( 1, SPRITE );
   *Sprs_Glxy[1] = *Sprs_Glxy[0];
   Sprs_Glxy[1]->Vo = 1*16.0;

   Sprs_Glxy[2] = New_Fatal_Object( 1, SPRITE );
   *Sprs_Glxy[2] = *Sprs_Glxy[0];
   Sprs_Glxy[2]->Vo = 2*16.0;

   Sprs_Glxy[3] = New_Fatal_Object( 1, SPRITE );
   *Sprs_Glxy[3] = *Sprs_Glxy[0];
   Sprs_Glxy[3]->Vo = 3*16.0;

   GIF_To_VBuf( "mac6.gif", &Ufo1 );
   GIF_To_VBuf( "coca3.gif", &Ufo2 );
   GIF_To_VBuf( "dollar2.gif", &Ufo3 );
   GIF_To_VBuf( "yes.gif", &Ufo4 );

   Sky = Load_GIF( "sky4.gif" );
}

/********************************************************************/

EXTERN void Init_Ptcl( INT Param )
{
   switch ( Param )
   {
      case 0:
         Select_VBuffer( &VB(1), V777, The_W, The_H, The_W*4, 0x00 );
         memset( VB(1).Bits, 0, VB(1).Size );
         SELECT_CMAP( 3 );
         Check_CMap_VBuffer( &VB(3), 256 );
         Setup_All_Ptcls( );
      break;

      case 1:  // UFO #1
         Ufo = &Ufo1;
         Extract_Virtual_VBuffer( &VB(2), &VB(VSCREEN), 165, 5, Ufo->W, Ufo->H );
         Ptcl_0 = 1;
         Ufo_x = 165+20; Ufo_y = 5+23;
      break;
      case 2:      // nothing. Destroy last bitmap (i.e.: VBuf..)
         Destroy_VBuffer( Ufo ); Ufo = NULL;
      break;

      case 3:  // UFO #2
         Ufo = &Ufo2;
         Extract_Virtual_VBuffer( &VB(2), &VB(VSCREEN), 200, 0, Ufo->W, Ufo->H );
         Ptcl_0 = 4;
         Ufo_x = 200+6; Ufo_y = 5+15;
      break;
      case 4:      // nothing
         Destroy_VBuffer( Ufo ); Ufo = NULL;
      break;
      case 5:  // UFO #3
         Ufo = &Ufo3;
         Extract_Virtual_VBuffer( &VB(2), &VB(VSCREEN), 240, 5, Ufo->W, Ufo->H );
         Ptcl_0 = 7;
         Ufo_x = 240-4; Ufo_y = 5+10;
      break;
      case 6:      // nothing
         Destroy_VBuffer( Ufo ); Ufo = NULL;
      break;
      case 7:  // UFO #4
         Ufo = &Ufo4;
         Extract_Virtual_VBuffer( &VB(2), &VB(VSCREEN), 145, 10, Ufo->W, Ufo->H );
         Ptcl_0 = 15;
         Ufo_x = 145-4; Ufo_y = 10+10;
      break;
      case 8:      // nothing
         Destroy_VBuffer( Ufo ); Ufo = NULL;
      break;
   }
}

/********************************************************************/

#define UAMP   1.0f
#define VAMP   1.0f

static void PROJ(F_MAPPING UV, FLT x, FLT y, FLT z )
{
   VECTOR V;
   V[0] = x; V[1] = y; V[2] = z;
   nA_Inv_V_Eq( V, Ptcl_M );
   y = (FLT)atan2( V[0]*Ptcl_M[8], 1.0-V[0]*Ptcl_M[6] - V[1]*Ptcl_M[7] );
   if ( y<0.0 ) y = M_PI - y;
   x = (FLT)atan2( V[1]*sin(y), V[0] );
   UV[0] = 0.5f + (UAMP/M_PI)*x;
   UV[1] = (VAMP/M_PI)*y;
}

#define BILX2 40
#define BILY2 25
#define NX     ( 1.0f/(BILX2/2) )
#define NY     (-1.0f/(BILY2/2) )
#define DX  (2.0f*8.0f/320.0f)
#define DY  (2.0f*8.0f/200.0f)

  // sets up a bilinear-interpolated background

EXTERN void Set_Bck( PIXEL *Dst, PIXEL *Map )
{
   INT UV[(BILX2+1)][(BILY2+1)][2];
   INT i, j;
   INT *iDst;
   FLT y;

   iDst = &UV[0][0][0];
   y = 100.0/160.0;   // 1.0;
   for( j=0; j<=BILY2; ++j, y-=DX )
   {
      FLT x = -100.0f/160.0f;
      for( i=0; i<=BILX2; ++i, x+=DX )
      {
         F_MAPPING uv;
         PROJ( uv, x, y, 1.0 );
         *iDst++ = (SHORT)(uv[0]*65536.0f*2.0f);
         *iDst++ = (SHORT)(uv[1]*65536.0f*2.0f);
      }
   }
   Do_Bilinear_UV_Map_8( Dst, (INT*)UV, BILX2, BILY2, The_W, Map );
}

/********************************************************************/

static void Set_Ptcl_Matrix( FLT Phi, FLT Phi2, FLT Theta )
{
   Trf.Rot.Rot[_X] = M_PI/2.0f-.32f + .10f*(FLT)sin( Theta*8.0 );
   Trf.Rot.Rot[_Y] = 0.15f + .10f*(FLT)sin( Theta*5.0 );
   Trf.Rot.Rot[_X] *= Phi2;
   Trf.Rot.Rot[_Y] *= Phi2;
   Trf.Rot.Rot[_Z] = Theta*4.0f;    // 0.0;
   Trf.Pos[2] =  1.0f - 0.3f*Phi;
   Transform_To_Matrix( &Trf, Ptcl_M );
}

/********************************************************************/

static void Do_Ptcl_Anim( FLT Phi, FLT Phi2, FLT Theta )
{
   INT i;
   PTCL *Ptr;

   for( i=Nb_Ptcl, Ptr=Ptcl; i>0; --i, Ptr++ )
   {
      VECTOR V;
      FLT r, t;
      r = Ptr->r * Phi;
      t = Ptr->t + Theta*4.0f;
      V[0] = (FLT)cos( t ) * r;
      V[1] = (FLT)sin( t ) * r;
      V[2] = Ptr->z * Phi;
      Ptr->t += Global_dx * Ptr->dt;
      A_V_Eq( V, Ptcl_M );
      if ( V[2]<=0.0 || V[2]>2.0f ) goto Skip;
      Ptr->s = ( 2.0f-V[2] )/2.0f;
      V[2] = 1.6f / ( 1.0f + V[2] );
      Ptr->x = V[0]*V[2];
      Ptr->x = 100.0f + Ptr->x*95.0f;
      if ( (Ptr->x<-30.0f)||(Ptr->x>350.0f) )
      {
Skip:
         Ptr->s = 0.0; continue; 
      }
      Ptr->y = V[1]*V[2];
      Ptr->y = 100.0f - Ptr->y*95.0f;
      if ( (Ptr->y<-30.0f)||(Ptr->y>230.0f) ) goto Skip;

      r = Ptr->s*(2.0f*0.5f) * Ptr->dr;
      Paste_Sprite_Bitmap_Sat_32( (UINT*)VB(1).Bits, The_W*4,
         Ptr->x, Ptr->y, r, Ptr->Sp );

   }
}

/********************************************************************/

EXTERN void Loop_Ptcl_I( )
{
   FLT Phi, Phi2, Amp;

   Setup_Clipping( 1.0, 319.0, 1.0, 199.0, 1.0, 0.0 );

   if ( Global_x<0.4f ) Phi = (Global_x-0.0f)/(0.4f-0.0f);
   else if ( Global_x<0.8f ) Phi = 1.0f;
   else if ( Global_x<0.85f ) Phi = 1.0f-(Global_x-0.80f)/(0.85f-0.80f);
   else Phi = 10.0f*(Global_x-0.85f)/(1.0f-0.85f);

   if ( Global_x<0.3f ) Phi2 = 0.0;
   else if ( Global_x<0.5f ) Phi2 = ( Global_x-0.3f ) / ( 0.5f-0.3f );
   else Phi2 = 1.0f;

   Set_Ptcl_Matrix( Phi, Phi2, Global_x*(2.0f*M_PI) );

   Set_Bck( (PIXEL*)VB(3).Bits, Sky->Bits );

   Amp = 1.0f;
   if ( Global_x>=0.35f ) Amp -= (Global_x-0.35f)/(1.0f-0.35f);
   CMap_To_16bits_With_Fx( (void*)VB(3).CMap, Sky->Pal, Sky->Nb_Col, 0x777,
      0x000000, NULL, (BYTE)(Amp*255.0f), 0 );
   Map_32_Bits( &VB(1), &VB(3) );

   Do_Ptcl_Anim( Phi, Phi2, Global_x*(2.0f*M_PI) );

   Mixer.Mix_777_To_16( &VB(VSCREEN), &VB(1) );
   Destroy_Screen_XOr( &VB(VSCREEN), Get_Beat( ) );
}

EXTERN void Loop_Ptcl_II( )
{   
   Loop_Ptcl_I( );
   VBuf_Map_8_Bits_Transp( &VB(2), Ufo );

   if ( Ptcl[Ptcl_0].s>0.0 )
   {
      INT a, BpS, xo, yo;
      USHORT *Dst;

      BpS = VB(VSCREEN).BpS / 2;
      Dst = (USHORT*)VB(VSCREEN).Bits + Ufo_y*BpS + Ufo_x;
      Dst[-BpS+0] = Dst[-BpS+1] = Dst[-BpS+2] = Dst[-BpS+3] = 0xf800;
      Dst[0] = Dst[1] = Dst[2] = Dst[3] = 0xf800;
      Dst[BpS+0] = Dst[BpS+1] = Dst[BpS+2] = Dst[BpS+3] = 0xf800;
      Dst[2*BpS+0] = Dst[2*BpS+1] = Dst[2*BpS+2] = Dst[2*BpS+3] = 0xf800;

      xo = (INT)Ptcl[Ptcl_0].x;
      if ( xo<5 ) xo = 5; 
      else if (xo>=Ufo_x) xo = Ufo_x;
      Ptcl[Ptcl_0].x = 1.0f*xo;
      a = xo; xo = Ufo_x;
      Dst = (USHORT*)VB(VSCREEN).Bits + Ufo_y*BpS + a;
      a = xo-a;
      while( a-- ) { Dst[a] = Dst[a+BpS] = 0xf800; }
      yo = (INT)Ptcl[Ptcl_0].y;
      if ( yo<5 ) yo = 5; 
      else if (yo>The_H-5) yo = The_H-5;
      Ptcl[Ptcl_0].y = 1.0f*yo;
      if ( Ufo_y<yo ) { a = Ufo_y; }
      else { a=yo; yo=Ufo_y; }
      Dst = (USHORT*)VB(VSCREEN).Bits + a*BpS + (INT)Ptcl[Ptcl_0].x;
      a = yo-a;
      while( a-- ) { Dst[0] = Dst[1] = 0xf800; Dst+=BpS;  }
      Dst = (USHORT*)VB(VSCREEN).Bits + ((INT)Ptcl[Ptcl_0].y)*BpS + (INT)Ptcl[Ptcl_0].x;
      Dst[-BpS-1] = Dst[-BpS+0] = Dst[-BpS+1] = Dst[-BpS+2] = 0xf800;
      Dst[-1] = Dst[0] = Dst[1] = Dst[2] = 0xf800;
      Dst[BpS-1] = Dst[BpS+0] = Dst[BpS+1] = Dst[BpS+2] = 0xf800;
//      Dst[2*BpS-1] = Dst[2*BpS+0] = Dst[2*BpS+1] = Dst[2*BpS+2] = 0xf800;
   }
}

/********************************************************************/

EXTERN void Close_Ptcl( )
{
   M_Free( Ptcl );
   Nb_Ptcl = 0;

   Destroy_Sprite_777( Sprs_Glxy[0] );
   M_Free( Sprs_Glxy[1] );
   M_Free( Sprs_Glxy[2] );
   M_Free( Sprs_Glxy[3] );
   Destroy_Bitmap( Sky );
}

/********************************************************************/
