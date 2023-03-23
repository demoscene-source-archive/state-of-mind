/*
 * JPEG 'anims' and masks
 *
 * Skal 98
 ***************************************/

#include "demo.h"
#include "rand.h"
#include "truecol.h"

/********************************************************************/

static INT A_Cnt = 0;
EXTERN INT JA_Parity = 0;

EXTERN LINT Timer0;
EXTERN INT Next_Beat = 0;

/********************************************************************/

   // soft/strange interpolation. beware of edge effects...

#ifdef UNIX 
static void JPaste_I( VBUFFER *V, BYTE *Bits, INT W, INT H )
{
   BYTE *Dst = (BYTE*)V->Bits;
   Dst += W;
   W /= 2;
   H /= 2;
   Bits += W;
   for( ; H>0; --H )
   {
      INT i;
      for( i=-W; i<0; ++i ) Dst[2*i] = Bits[i];
      Dst[-1] = Dst[-2];
      Dst += 2*V->BpS;
      Bits += W;
   }
   Dst -= V->BpS + 2*W;
   memcpy( Dst, Dst-V->BpS, 2*W );
}

static void JPaste_II( VBUFFER *V, BYTE *Bits, INT W, INT H )
{
   BYTE *Dst = (BYTE*)V->Bits;
   Dst += W;
   W /= 2;
   H /= 2;
   Bits += W;
   for( ; H>0; --H )
   {
      INT i;
      for( i=-W; i<0; ++i )
      {
         Dst[2*i+1] = ( Dst[2*i]+Dst[2*i+2] ) / 2;
         Dst[2*i] = Bits[i];
      }
      Dst += 2*V->BpS;
      Bits += W;
   }
}
static void JPaste_III( VBUFFER *V, BYTE *Bits, INT W, INT H )
{
   BYTE *Dst = (BYTE*)V->Bits;
   Dst += W;
   W = W/2-1; Dst-=2; 
   H /= 2;
   for( ; H>0; --H )
   {
      INT i;
      for( i=-W; i<0; ++i )
      {
         Dst[2*i+321] = ( Dst[2*i+320]+Dst[2*i+322] ) / 2;
         Dst[2*i] = *Bits++;
      }
      Dst[321] = ( Dst[320]+Dst[321] ) / 2;
      Dst[0] = *Bits++;
      Dst += 2*V->BpS;
   }
}
static void JPaste_IV( VBUFFER *V, BYTE *Bits, INT W, INT H )
{
   BYTE *Dst = (BYTE*)V->Bits;
   Dst += W;
   W /= 2;
   H /= 2;
   Bits += W;
   for( --H; H>0; --H )
   {
      INT i;
      for( i=-W; i<0; ++i )
      {
         Dst[2*i+320] = ( Dst[2*i+640]+Dst[2*i+0] ) / 2;
         Dst[2*i] = Bits[i];
      }
      Dst += 2*V->BpS;
      Bits += W;
   }
}

#else

extern void JPaste_I( VBUFFER *V, BYTE *Bits, INT W, INT H );
extern void JPaste_II( VBUFFER *V, BYTE *Bits, INT W, INT H );
extern void JPaste_III( VBUFFER *V, BYTE *Bits, INT W, INT H );
extern void JPaste_IV( VBUFFER *V, BYTE *Bits, INT W, INT H );

#endif

static void (*A_Paste[4])( VBUFFER *, BYTE *, INT, INT ) 
 = { JPaste_I, JPaste_II, JPaste_IV, JPaste_III };

/********************************************************************/

EXTERN void Paste_JPEG_Anim( )
{
   if ( Cur_Anim_Bits==NULL ) return;
   (*A_Paste[A_Cnt])( &VB(JANIM),
      (PIXEL*)Cur_Anim_Bits + Cur_Anim_Size*Anim_Cnt, 
      Cur_Anim_Width*2, Cur_Anim_Height*2 );
   A_Cnt++;
   A_Cnt = A_Cnt&0x03;
}

EXTERN void Init_JPEG_Anim( )
{
   SELECT_CMAP( JANIM );
   A_Cnt = 0;
}

/********************************************************************/

EXTERN void Setup_Anim( )
{
   Anim_Cnt = 0;
   Cur_Anim_Size = Cur_Anim_Width*Cur_Anim_Height;
   if (Cur_Anim!=NULL )
   {
      Cur_Anim_Bits = (BYTE *)Cur_Anim->Bits;
	  Anim_Max_Cnt = Cur_Anim->Width*Cur_Anim->Height/Cur_Anim_Size;
   }
   else Cur_Anim_Bits = NULL;
   
}

EXTERN void Set_Anim_CMap( VBUFFER *VBuf, INT N, 
   INT Ro, INT Go, INT Bo, 
   INT R, INT G, INT B, 
   UINT Format )
{
   Check_CMap_VBuffer( VBuf, N );
   Drv_Build_Ramp_16( VBuf->CMap, 0, N, Ro, Go, Bo, R, G, B, Format );
}

EXTERN void Paste_JPEG_Anim_Move( )
{
   PIXEL *Bits;

   Bits = (PIXEL*)Cur_Anim_Bits + Cur_Anim_Size*Anim_Cnt;
   if ( JA_Parity )
   {
      if ( Anim_Cnt!=0 )
         Bits -= Cur_Anim_Width*(TRandom(0)&0x03);
      JA_Parity--;
   }
   (*A_Paste[A_Cnt])( &VB(JANIM),
      Bits, Cur_Anim_Width*2, Cur_Anim_Height*2 );
   A_Cnt++;
   A_Cnt = A_Cnt&0x03;
}

/********************************************************************/

      // these should be elsewhere...

EXTERN FLT Get_Beat( )
{
   FLT x;
   if ( Timer>Next_Beat ) { Timer0 = Next_Beat; Next_Beat += BEAT_TIMER_III; }
   x = 1.0f*(Timer-Timer0)/BEAT_TIMER_III;
   Anim_Cnt = (INT)( x*Anim_Max_Cnt);
   if ( Anim_Cnt>=Anim_Max_Cnt ) Anim_Cnt = Anim_Max_Cnt-1;
   else if ( Anim_Cnt<0 ) Anim_Cnt = 0;
   return( x );
}

EXTERN FLT Get_Beat_2( )   // returns 1.0 to 0.0 on every pulse
{
   FLT x;
   if ( Timer>Next_Beat ) { Timer0 = Next_Beat; Next_Beat += BEAT_TIMER_III; }
   x = 1.0f*(Timer-Timer0)/(BEAT_TIMER_III/2);
   x = (FLT)fmod( x, 1.0 );
   if ( (x>0.0) && (x<0.2f) ) x = (0.2f-x)/0.2f;
   else if ( (x>0.6)&&(x<0.8) ) x = (x-0.6f)/0.2f;
   else x=0.0;
   return( x );
}

EXTERN FLT Get_Beat_3( )   // returns 1.0 to 0.0 on every pulse
{
   FLT x;
   if ( Timer>Next_Beat ) { Timer0 = Next_Beat; Next_Beat += BEAT_TIMER_III; }
   x = 1.0f*(Timer-Timer0)/BEAT_TIMER_III;
   x = (FLT)fmod( 4.0*x, 1.0 );
   if ( (x>0.0) && (x<0.3) ) return( x = 1.0f - x/0.3f );
   else return( 0.0 );
}

EXTERN FLT Get_Beat_4( )
{
   FLT x;
   if ( Timer>Next_Beat ) { Timer0 = Next_Beat; Next_Beat += 2*BEAT_TIMER_III; }
   x = 1.0f*(Timer-Timer0)/(2.0f*BEAT_TIMER_III);
   x = (FLT)fmod( x, 1.0 );
   Anim_Cnt = (INT)( x*Anim_Max_Cnt);
   if ( Anim_Cnt>=Anim_Max_Cnt ) Anim_Cnt = Anim_Max_Cnt-1;
   else if ( Anim_Cnt<0 ) Anim_Cnt = 0;
   return( x );
}

/********************************************************************/

EXTERN void Destroy_Screen( VBUFFER *Out, FLT x )
{
   INT Off, i, j;
   USHORT *Dst = (USHORT*)Out->Bits;

   Off = (INT)(x*0x100);
   for( j=Out->H/2; j>0; j-- )
   {
      INT U;
      U = (The_W-1)*Off;
      for( i=The_W-1; i>=0; --i )
      {
         Dst[i] = Dst[U>>8];
         U -= Off;
      }
      Dst = (USHORT*)((PIXEL*)Dst + Out->BpS);
      U = (The_W-1)*0x100 - (The_W-1)*Off;
      for( i=0; i<The_W; ++i )
      {
         Dst[i] = Dst[U>>8];
         U += Off;
      }
      Dst = (USHORT*)((PIXEL*)Dst + Out->BpS);
   }
}

static USHORT XOr_Tab[32] = { 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8410, 0x8410,
 0xf79e, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf79e, 0xe71c, 0x8410, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
// 0xffff, 0xffff, 0xffff, 0x7bef, 0x18e3, 0x0861, 0x0000, 0x0000
};

EXTERN void Destroy_Screen_XOr( VBUFFER *Out, FLT x )
{
   INT i, j;
   USHORT Or;
   USHORT *Dst = (USHORT*)Out->Bits + Out->W;

   Or = XOr_Tab[ (INT)floor( x*31.9 ) ];
   if ( Or==0x0000 ) return;
   for( j=Out->H; j>0; j-- )
   {
      for( i=-The_W; i<0; ++i ) Dst[i] ^= Or;
      Dst = (USHORT*)((PIXEL*)Dst + Out->BpS);
   }
}

/********************************************************************/

#if 0

static INT Phase_Off[16] = 
{
   0,1,3,2,    5,4,7,5,
   7,6,8,7,    8,3,5,1
};

EXTERN void Destroy_Screen_II( VBUFFER *Out )
{
   INT Off, i, j;
   USHORT *Dst = (USHORT*)Out->Bits + Out->W;
   Off = 0;
   for( j=Out->H; j>0; --j )
   {
      INT k;
      k = Phase_Off[Off];
      for( i=The_W-1-k; i>=0; --i )
         Dst[i+k] = Dst[i] & 0x07e0;
      for( i=0; i<k; ++i ) Dst[i] = 0x0000;
      (PIXEL*)Dst += Out->BpS;
      Off += TRandom(0)&0x01;
      Off = Off&0x0F;
   }
}
#endif

/********************************************************************/

static PIXEL HTab_I[256] =
{
   3, 2, 3, 3, 3, 4, 4, 3, 3, 4, 29, 32, 16,
   15, 14, 11, 8, 5, 3, 1, 0,
   0
};
static USHORT Or_Tab[128];

EXTERN void Destroy_Screen_III( VBUFFER *Out )
{
   INT j;
   USHORT *Dst = (USHORT*)Out->Bits + Out->W;
   static PIXEL Phase0 = 0;
   PIXEL Phase;

   Phase = Phase0 + (TRandom(2)&0x03);
   Phase0 += 3;
   for( j=Out->H; j>0; --j )
   {
      INT i, ko;
      ko = HTab_I[Phase] + 8 + (TRandom(1)&0x03);
      for( i=-Out->W; i<0; ++i )
      {
         PIXEL k;
         k = ko + Rand_Tab[Hash_Seed];
         Dst[i] = Or_Tab[ ((Dst[i]>>5)&0x003f) + k ];
         Hash_Seed = Hash1[Hash_Seed];
      }
      Dst = (USHORT*)((PIXEL*)Dst + Out->BpS);
      Phase++;
   }
}

EXTERN void Pre_Cmp_Destroy_Screen( )
{
   INT i;
   Drv_Build_Ramp_16( Or_Tab, 0, 64, 0,0,0, 20,200,100,0x2565 );
   for( i=64; i<128; ++i ) Or_Tab[i] = Or_Tab[63];
   Init_Hash1( 10, 126 );
}

/********************************************************************/

#include "sprs2.h"    // BYTE Sprite_Data[_T_][_H_*_W_]
// #include "sprs3.h"    // BYTE Sprite_Data2[_T_][_H_*_W_]

EXTERN void Paste_JPEG_Anim_Sprites( )
{
   PIXEL *Bits;
   BYTE *Dst1, *Source_Ptr;
   INT W, H;

   if ( Cur_Anim_Bits==NULL ) return;
   W = Cur_Anim_Width;     // 160
   H = Cur_Anim_Height;    // 100
   Bits = (PIXEL*)Cur_Anim_Bits + Cur_Anim_Size*Anim_Cnt;
   Bits += W;

   Dst1 = (BYTE*)VB(JANIM).Bits;
   Dst1 += 2*W;
   Source_Ptr = &Sprite_Data[0][0];

   for( ; H>0; H-=4 )
   {
      INT i;
      for( i=-W; i<0; i+=4 )
      {
         BYTE *Src, *Dst;
         UINT C;
         C  = Bits[i] + Bits[i+1] + Bits[i+2] + Bits[i+3];
         C += Bits[W+i] + Bits[W+i+1] + Bits[W+i+2] + Bits[W+i+3];
         C += Bits[2*W+i] + Bits[2*W+i+1] + Bits[2*W+i+2] + Bits[2*W+i+3];
         C += Bits[3*W+i] + Bits[3*W+i+1] + Bits[3*W+i+2] + Bits[3*W+i+3];
         C /= (16*16);
         Src = &Source_Ptr[C*(8*8)];
         Dst = Dst1;
         Dst[2*i] = *Src++; Dst[2*i+1] = *Src++; 
         Dst[2*i+2] = *Src++; Dst[2*i+3] = *Src++;
         Dst[2*i+4] = *Src++; Dst[2*i+5] = *Src++;
         Dst[2*i+6] = *Src++; Dst[2*i+7] = *Src++;
         Dst += VB(JANIM).BpS;
         Dst[2*i] = *Src++; Dst[2*i+1] = *Src++; 
         Dst[2*i+2] = *Src++; Dst[2*i+3] = *Src++;
         Dst[2*i+4] = *Src++; Dst[2*i+5] = *Src++;
         Dst[2*i+6] = *Src++; Dst[2*i+7] = *Src++;
         Dst += VB(JANIM).BpS;
         Dst[2*i] = *Src++; Dst[2*i+1] = *Src++; 
         Dst[2*i+2] = *Src++; Dst[2*i+3] = *Src++;
         Dst[2*i+4] = *Src++; Dst[2*i+5] = *Src++;
         Dst[2*i+6] = *Src++; Dst[2*i+7] = *Src++;
         Dst += VB(JANIM).BpS;
         Dst[2*i] = *Src++; Dst[2*i+1] = *Src++; 
         Dst[2*i+2] = *Src++; Dst[2*i+3] = *Src++;
         Dst[2*i+4] = *Src++; Dst[2*i+5] = *Src++;
         Dst[2*i+6] = *Src++; Dst[2*i+7] = *Src++;
         Dst += VB(JANIM).BpS;
         Dst[2*i] = *Src++; Dst[2*i+1] = *Src++; 
         Dst[2*i+2] = *Src++; Dst[2*i+3] = *Src++;
         Dst[2*i+4] = *Src++; Dst[2*i+5] = *Src++;
         Dst[2*i+6] = *Src++; Dst[2*i+7] = *Src++;
         Dst += VB(JANIM).BpS;
         Dst[2*i] = *Src++; Dst[2*i+1] = *Src++; 
         Dst[2*i+2] = *Src++; Dst[2*i+3] = *Src++;
         Dst[2*i+4] = *Src++; Dst[2*i+5] = *Src++;
         Dst[2*i+6] = *Src++; Dst[2*i+7] = *Src++;
         Dst += VB(JANIM).BpS;
         Dst[2*i] = *Src++; Dst[2*i+1] = *Src++; 
         Dst[2*i+2] = *Src++; Dst[2*i+3] = *Src++;
         Dst[2*i+4] = *Src++; Dst[2*i+5] = *Src++;
         Dst[2*i+6] = *Src++; Dst[2*i+7] = *Src++;
         Dst += VB(JANIM).BpS;
         Dst[2*i] = *Src++; Dst[2*i+1] = *Src++; 
         Dst[2*i+2] = *Src++; Dst[2*i+3] = *Src++;
         Dst[2*i+4] = *Src++; Dst[2*i+5] = *Src++;
         Dst[2*i+6] = *Src++; Dst[2*i+7] = *Src++;
      }
      Dst1 += 8*VB(JANIM).BpS;
      Bits += 4*W;
   }
}

/********************************************************************/

static INT RSeed = 1257;
EXTERN INT Random() { return ((RSeed=RSeed*205+29573)>>5)&0xffff; }
EXTERN INT TRandom( INT Slot )
{
   static INT Last_Timer[4] = {-1, -1, -1, -1};
   static INT Last_Rand[4] = { 0 };
   if ( (Timer-Last_Timer[Slot])>=30 )
   {
      Last_Rand[Slot] = Random();
      Last_Timer[Slot] = Timer;
   }
   return( Last_Rand[Slot] );
}

/********************************************************************/
