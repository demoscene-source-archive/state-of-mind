/*
 * MP3's Spectrum handling (unused (boring))
 *
 * Skal 98
 ***************************************/

#include "demo.h"
#include "mp3.h"

typedef struct {

   UINT Stamp;
   SHORT Amp[2][32];

} SPTR;

#define NB_FR  4

#define MAX_SP 16
static SPTR Sp[MAX_SP];
static INT Sp_Tail, Sp_Head;
static INT Cur_Frame = 0;
static INT Stamp = 1;
static FLT ms_Per_Frame;
static UINT S_Time = -1;

/********************************************************************/

EXTERN void Handle_Spectrum( INT Nb_Chn, FLT Spc[2][32][18] )
{
   INT i, j;
   Cur_Frame++;
   if ( (Cur_Frame&(NB_FR-1))==0x00 )
   {
      Sp_Head++;
      if ( Sp_Head==MAX_SP ) Sp_Head = 0;
      Sp[Sp_Head].Stamp = Stamp++;
      for( i=0; i<32; ++i )   // only mono
      {
         FLT x = 0.0;
         for( j=0; j<18; ++j ) x += fabs( Spc[0][i][j] );
         x = x*( 4000.0/18.0 );
         Sp[Sp_Head].Amp[0][i] = (SHORT)x; // += x/(NB_FR);
      }
   }
}

/********************************************************************/

EXTERN void Init_Spectrum( void *Dummy )
{
   MP3_STRM *Strm = (MP3_STRM*)Dummy;
   Sp_Tail = 0;
   memset( Sp, 0, sizeof( Sp ) );
   ms_Per_Frame = 1000.f*NB_FR*576.0f/Strm->FS;
   S_Time = Timer;
}

EXTERN void Close_Spectrum( )
{
   MP3_Handle_Spectrum = NULL;
}

/********************************************************************/

EXTERN void Do_Spectrum( VBUFFER *Out )
{
   INT i, j, k, n, T;
   USHORT CMap[256];
   USHORT *Dst, *Ptr;

   T = ( Timer-S_Time )/ms_Per_Frame-1;
   i = Sp_Tail;
   while( T>Sp[i].Stamp ) { 
      i++; if ( i==MAX_SP ) i = 0;
      if ( i==Sp_Tail || i==Sp_Head ) 
      { i = Sp_Tail; break; }
   }
   Sp_Tail = i;

   Drv_Build_Ramp_16( (UINT*)CMap, 0, 64, 247,228,0, 0,255,0, 0x2565 );
   Dst = (USHORT*)Out->Bits + The_W-64;
   for( i=0; i<32; ++i )
   {
      j = Sp[Sp_Tail].Amp[0][i];
      if ( j>63 ) j=63;
//      else if ( j<0 ) j=0;
      n = j = 64-j;
      for( k=0; j<64; ++k, ++j ) Dst[j] = CMap[k];
      Ptr = Dst + n;
      j = (64-n)*sizeof( USHORT );
      memcpy( (PIXEL*)Ptr+1*Out->BpS, Ptr, j );
      memcpy( (PIXEL*)Ptr+2*Out->BpS, Ptr, j );
      memcpy( (PIXEL*)Ptr+3*Out->BpS, Ptr, j );
      memcpy( (PIXEL*)Ptr+4*Out->BpS, Ptr, j );
//      memcpy( (PIXEL*)Dst+5*Out->BpS, Dst, j );
      (PIXEL*)Dst += Out->BpS*6;
   }
}

/********************************************************************/
