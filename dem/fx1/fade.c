/*
 * Fade funcs
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

static void Init_Fade_Tab_16b( USHORT *Lo, USHORT *Hi, FLT x )
{
   INT i;
   for( i=0; i<256; ++i )
   {
      UINT R, G1, G2, B;

      R  = (UINT)((i&0xF8)*x );
      G2 = (UINT)(((i&0x07)<<8)*x );
      G1 = (UINT)((i&0xE0)*x );
      B  = (UINT)((i&0x1F)*x );
      Hi[i] = (USHORT)( ((R&0xF8)<<8) | (G2&0x07e0) );
      Lo[i] = (USHORT)((G1&0xE0) | (B&0x1F));
   }
}

#if 0       // USELESS now

static void Init_Fade_Tab_16b_White( USHORT *Lo, USHORT *Hi, FLT x )
{
   INT i;
   for( i=0; i<256; ++i )
   {
      UINT R, G1, G2, B;

      R  = (UINT)ceil((i&0xF8)*x ); if ( R>0xF8 ) R = 0xF8;
      G2 = (UINT)ceil((i&0x07)*x ); if ( G2>0x07 ) G2 = 0x07;
      G1 = (UINT)ceil((i&0xE0)*x ); if ( G1>0xE0 ) G1 = 0xE0;
      B  = (UINT)ceil((i&0x1F)*x ); if ( B>0x1F ) B = 0x1F;
      Hi[i] = (USHORT)(( (R&0xF8) | (G2&0x07) )<<8);
      Lo[i] = (USHORT)((G1&0xE0) | (B&0x1F));
   }
}

#endif


EXTERN void Loop_Fade( )
{
   USHORT Lo_Fade[256], Hi_Fade[256];
   Init_Fade_Tab_16b( Lo_Fade, Hi_Fade, 0.98f );
   Mixer.Mix_To_Fade_Feedback( &VB(VSCREEN), &VB(VSCREEN), 
      (USHORT*)Lo_Fade, (USHORT*)Hi_Fade );
}

/********************************************************************/

#if 0          //  UNUSED NOW

EXTERN USHORT Lo16_To_16_White[MIX_LEVELS][256];
EXTERN USHORT Hi16_To_16_White[MIX_LEVELS][256];
EXTERN UINT Lo16_To_777_White[MIX_LEVELS][256];
EXTERN UINT Hi16_To_777_White[MIX_LEVELS][256];

EXTERN INT Cnt_Fade = 0, Cnt_Fade2 = 0;
EXTERN INT Fade[16] =
{
   1,3,5,7,  6,5,4,4,
   4,3,3,3,  2,2,1,0
};
EXTERN INT Fade_II[16] =
{
   1,6,10,16,  20,25,28,31,
   29,26,23,21,  16,5,2,0
};

EXTERN void Init_Fade_White( INT Param )
{
   SELECT_565( 1 );
   Copy_Buffer_Short( &VB(1), &VB(VSCREEN) );      // copy screen in VB #1
   Cnt_Fade = 0;
}

EXTERN void Loop_Fade_White( )
{
   if ( Cnt_Fade<MIX_LEVELS-1 ) Cnt_Fade++;
   Mixer.Mix_To_Fade( &VB(VSCREEN), &VB(1), 
      (USHORT*)Lo16_To_16_White[Cnt_Fade],
      (USHORT*)Hi16_To_16_White[Cnt_Fade] );
}
#endif

////////////////////////////////////////////////////////////////////////

