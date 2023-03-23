/*
 * Caustics
 *
 * Skal 98
 ***************************************/

#include "demo.h"

#define BILX 64
#define BILY 64
#define BILX2 40
#define BILY2 25

EXTERN USHORT   Tab_I[2*CLAMP_OFF+256];
EXTERN USHORT  Tab_II[2*CLAMP_OFF+256];

PIXEL *Base_Plasma;
PIXEL *Base_Bil1 = NULL;
PIXEL *Caustics = NULL;

/********************************************************************/

void Pre_Cmp_Caustics( )
{
   INT i, j;
   PIXEL *Ptr;

   Base_Plasma = New_Fatal_Object( 256*256, PIXEL );
   Ptr = Base_Plasma;
   for( j=0; j<256; ++j )
      for( i=0; i<256; ++i )
      {
         FLT Norm;
#define RANGE  1.0            //  = MAGIK Constant!!!
#define Ho  64.0
#if 1
         Norm = Ho + (Ho/RANGE)*( 
            sin(i/21.0) + cos(j/11.0) + cos(i/20.0) + sin(j/19.0)
            + sin((i+j)/25.0 ) 
            + cos( sqrt( (128.0-i)*(128.0-i)+(128.0-j)*(128.0-j) )/10.0 )
#else
         Norm = Ho + (Ho/RANGE)*( 
            sin(i/30.0) + cos(j/16.0) + cos(i/27.0) + sin(j/23.0)
            + sin((i+j)/31.0 ) 
            + cos( sqrt( (128.0-i)*(128.0-i)+(128.0-j)*(128.0-j) )/16.0 )
#endif
         );
         *Ptr++ = ( PIXEL )( Norm );
      }
}

/********************************************************************/

static void Prepare_Caustics( FLT t, PIXEL *Caustics )
{
   BYTE U1, V1, U2, V2;
   FLT x;

   x = t*1.0f; 
   U1 = (BYTE)( 60.0f + 40.0f*cos( 2.12*x*M_PI_180 ) );
   V1 = (BYTE)( 120.0f + 70.0f*sin( 1.244*x*M_PI_180 ) );

   x = t*1.5f;
   U2 = (BYTE)( 110.0f - 90.0f*cos( 2.87*x*M_PI_180 ) );
   V2 = (BYTE)( 90.0f + 120.0f*sin( .985*x*M_PI_180 ) );

   Do_Plasma_256( Caustics, Base_Plasma, U1, V1, U2, V2 );
}

void Do_Da_Funky_Caustics( PIXEL *Caustics, INT W, INT H, PIXEL *Base )
{
   INT i, j;
   PIXEL *Ptr;
   USHORT Tmp[65*65];   // <= should be enough. Warning!!!

   Ptr = (PIXEL*)Caustics;
   memset( Tmp, 0, (W+1)*(H+1)*sizeof(USHORT) );
   for( j=0; j<255; ++j )
   {
      for( i=0; i<255; ++i )
      {
         INT dx, dy;
         dx = Ptr[i+1] - Ptr[i] + i;
         dx = Tab_I[CLAMP_OFF+dx];
         dy = Ptr[i+256] - Ptr[i] + j;
         dy = dx + Tab_II[CLAMP_OFF+dy];
         Tmp[dy] += 3;
      }
      Ptr += 256;
   }
   for( i=0; i<(W+1)*(H+1); ++i )
      if (Tmp[i]>=256 ) Base[i]=255;
      else Base[i]=(BYTE)Tmp[i];
}

/********************************************************************/

void Init_Caustics( INT Param )
{
   switch( Param )
   {
      case 0:           // CCCP + caustics...
         SELECT_CMAP(1);
         Check_CMap_VBuffer( &VB(1), 256 );
         Drv_Build_Ramp_16( VB(1).CMap, 0, 256, 0, 0, 0, 150, 180, 255, 0x4777 );

         Init_Saturation_Table3( Tab_I, 0, BILX2*2, 1.0, 1, 256 );
         Init_Saturation_Table3( Tab_II, 0, BILY2*2, 1.0, BILX2*2+1, 256 );
      break;
      case 1:
         Init_Saturation_Table3( Tab_I, 0, BILX, 1.0, 1, 256 );
         Init_Saturation_Table3( Tab_II, 0, BILY, 1.0, BILX+1, 256 );
         
         SELECT_CMAP(3);
         Check_CMap_VBuffer( &VB(3), 256 );
      break;
#if 0       // unused
      case 2:
         Init_Saturation_Table3( Tab_I, 0, BILX, 1.0, 1, 256 );
         Init_Saturation_Table3( Tab_II, 0, BILY, 1.0, BILX+1, 256 );
         SELECT_CMAP(3);
         Check_CMap_VBuffer( &VB(3), 256 );
         Init_Scroll( 0 );
      break;
#endif
   }
   if ( Base_Bil1==NULL ) Base_Bil1 = New_Fatal_Object( 257*257, BYTE );
   if ( Caustics==NULL) Caustics = New_Fatal_Object( 256*256, PIXEL );
}

/********************************************************************/

#define TRAD  10.0f
#define NX     (0.4f/(BILX2/2) )
#define NY     (0.4f/(BILY2/2) *0.625f )
EXTERN void Do_Ray_Tunnel( VBUFFER *Out1, PIXEL *Out2, 
   MATRIX2 M, PIXEL *Map )
{
   INT i, j;
   INT UV[51][81][2];
   INT *Dst;

   Dst = &UV[0][0][0];
   for( j=0; j<=BILY2; ++j )
   {
      FLT Ny;
      Ny = NY*(j-BILY2/2);
      for( i=0; i<=BILX2; ++i )
      {
         VECTOR P;
         FLT a,b,c,t;
         INT U, V, L;
         // FLT Nx;
         // t = 1.0 / sqrt( 1.0 + Nx*Nx + Ny*Ny );
         // P[0] = t*Nx; P[1] = t*Ny; P[2] = t;

         P[0] = NX*(i-BILX2/2);
         P[1] = Ny;
         P[2] = 1.0;
         nA_V_Eq( P, M );
#if 1
         P[0] = (FLT)fabs( P[0] );
         P[1] = (FLT)fabs( P[1] );
         P[2] = (FLT)fabs( P[2] );
         if ( P[1]>P[0] ) { FLT Tmp; Tmp = P[1]; P[1] = P[0]; P[0] = Tmp; }
#endif

         b = M[18]*P[0] + M[19]*P[1];
         a = P[0]*P[0] + P[1]*P[1];
         c = M[18]*M[18]+M[19]*M[19] - (TRAD*TRAD);
         t = 4.0f*( b*b - a*c );
         if ( (a>0.0) && (t>=0.0) )
         {
            t = (FLT)(-b+sqrt(t))/a;

            P[0] = t*P[0] + M[18];
            P[1] = t*P[1] + M[19];
            P[2] = t*P[2] + M[20];
            U = (INT)( P[2]*340.0f );
            t = (65536.0f/2.0f/M_PI)*(FLT)( M_PI-atan2( P[1], P[0] ) );
            V = (INT)t;
            // L = 65536.0*min(fabs(t*.25)+255*pow(cos(.5*Angle+PI/2),100.0),255.0);
         }
         else { U = V = 0; L = 0x0000; }
         *Dst++ = U; *Dst++ = V;
      }
   }
   Do_Bilinear_UV_Map_8( (PIXEL*)Out1->Bits, (INT*)UV, BILX2, BILY2, 320, Map );
}

static void Set_Tunnel_Position( FLT x, MATRIX2 M )
{
   VECTOR Dir, Po;

   Po[2] = Time_Scale*x;   
   Dir[2] = 70.0f*x-35.0f;

   x = (FLT)sin( 8.0*x*M_PI );
   Po[0] = 3.8f*(FLT)sin( 2.0*M_PI*x );
   Po[1] = 3.8f*(FLT)cos( 2.0*M_PI*x );
   Dir[0] = 5.0; 
   Dir[1] = 0.0; 
//   Dir[2] = 70.0*x-35.0;

   Camera_Matrix( Po, Dir, 0.0 /* x*M_PI */, M );
}

void Loop_Caustics_II( )
{
   MATRIX2 M;

   Prepare_Caustics( Global_x*500.0f, Caustics );
   Do_Da_Funky_Caustics( Caustics, BILX, BILY, Base_Bil1 );
   Do_Bilinear_4( Caustics, Base_Bil1, BILX, BILY, 256 );
   Set_Tunnel_Position( Global_x, M );
   Do_Ray_Tunnel( &VB(3), NULL, M, Caustics ); // Caustics=UV_Map here
}

/********************************************************************/

#if 0       // USELESS
static void Set_Tunnel_Position_II( FLT x, MATRIX2 M )
{
   VECTOR Dir, Po;

   Po[2] = Time_Scale*x;
   Po[0] = 1.6f;
   Po[1] = 0.0;
   Dir[0] = Po[0] + 0.1f;   // sin( x*6.0*M_PI );
   Dir[1] = Po[1] + 0.2f;
   Dir[2] = Po[2] + 1.0f;   // cos( x*6.0*M_PI );

   Camera_Matrix( Po, Dir, 0.0 /* x*M_PI */, M );
}
#endif

/********************************************************************/

#if 0
void Loop_Caustics_I( )
{
   Prepare_Caustics( Global_x*150.0, Caustics );
   Do_Da_Funky_Caustics( Caustics, BILX2*2, BILY2*2, Base_Bil1 );
   Do_Bilinear_4( VB(1).Bits, Base_Bil1, BILX2*2, BILY2*2, The_W );
//   VBuf_Map_8_Bits( &VB(VSCREEN), &VB(1) );
}
#endif

/********************************************************************/

#if 0          // DEBUG/TEST

void Loop_Caustics_III( )
{
   MATRIX2 M;

   Time_Scale = 1800.0;
   Drv_Build_Ramp_16( (UINT*)VB(3).CMap, 0, 256, 0, 0, 0, 190, 250, 190, 0x4565 );
   Prepare_Caustics( Global_x*50.0, Caustics );
   Do_Da_Funky_Caustics( Caustics, BILX, BILY, Base_Bil1 );
   Do_Bilinear_4( Caustics, Base_Bil1, BILX, BILY, 256 );
   Set_Tunnel_Position( Global_x, M );
   Do_Ray_Tunnel( &VB(3), NULL, M, Caustics ); //Caustics=UV_Map here...
   VBuf_Map_8_Bits( &VB(VSCREEN), &VB(3) );
}

/********************************************************************/

void Loop_Caustics_IV( )
{
   MATRIX2 M;

   Time_Scale = 1800.0;
   Drv_Build_Ramp_16( (UINT*)VB(3).CMap, 0, 256, 0, 0, 0, 190, 250, 190, 0x4565 );
   Prepare_Caustics( Global_x*50.0, Caustics );
   Do_Da_Funky_Caustics( Caustics, BILX, BILY, Base_Bil1 );
   Do_Bilinear_4( Caustics, Base_Bil1, BILX, BILY, 256 );
   Set_Tunnel_Position( Global_x, M );
   Do_Ray_Tunnel( &VB(3), NULL, M, Caustics ); // Caustics=UV_Map here
   VBuf_Map_8_Bits( &VB(VSCREEN), &VB(3) );
   Scroll_2( );   
}

#endif

/********************************************************************/

void Close_Caustics( )
{
   M_Free( Base_Plasma );
   M_Free( Base_Bil1 );
   M_Free( Caustics );
}

/********************************************************************/
