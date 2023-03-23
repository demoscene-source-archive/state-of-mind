/*
 * - Displacement. Unix C version -
 *
 * Skal 98
 ***************************************/

#include "demo.h"

#ifdef UNIX

EXTERN void Mix_Dsp_Off( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, INT *Off )
{
   INT i, j;
   USHORT *Dst, *Src;
   Dst = (USHORT*)Out->Bits + Out->W;
   Src = (USHORT*)In->Bits + Out->W;
   Dsp += Out->W;
   for( j=Out->H; j>0; --j )
   {
      for( i=-Out->W; i<0; ++i )
      {
         INT k = (INT)Dsp[i];
         k = Off[k] + i;
         Dst[i] = Src[ k ];
      }         
      Dst = (USHORT*)((PIXEL*)Dst + Out->BpS);
      Src = (USHORT*)((PIXEL*)Src + In->BpS);
      Dsp += The_W;
   }
}

EXTERN void Mix_Dsp_Off_Safe_X( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, 
   INT *Off, INT *Sat )
{
   INT i, j;
   USHORT *Dst, *Src;
   Dst = (USHORT*)Out->Bits + Out->W;
   Src = (USHORT*)In->Bits;
   Dsp += Out->W;
   Sat += Out->W;
   for( j=Out->H; j>0; --j )
   {
      for( i=-Out->W; i<0; ++i )
      {
         INT k;
         k = (INT)Dsp[i];
         k = Off[k] + i;
         k = Sat[k];
         Dst[i] = Src[ k ];
      }
      Dst = (USHORT*)((PIXEL*)Dst + Out->BpS);
      Src = (USHORT*)((PIXEL*)Src + In->BpS);
      Dsp += The_W;
   }
}

EXTERN void Mix_Dsp_Off_Safe_Y( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, 
   INT *Off, INT *Sat )
{
   INT i, j;
   USHORT *Dst, *Src;
   Dst = (USHORT*)Out->Bits + Out->W;
   Src = (USHORT*)In->Bits + Out->W;
   Dsp += Out->W;
   for( j=Out->H-1; j>=0; --j )
   {
      for( i=-Out->W; i<0; ++i )
      {
         INT k;
         k = (INT)Dsp[i];
         k = Off[k];
         k = i + Sat[k];
         Dst[i] = Src[ k ];
      }
      Sat++;
      Dst = (USHORT*)((PIXEL*)Dst + Out->BpS);
      Dsp += The_W;
   }
}

#if 0    // USELESS NOW
EXTERN void Mix_Dsp_Off_Blur_X( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, 
   INT *Off, INT *Sat )
{
}
EXTERN void Mix_Dsp_Off_Blur_Y( VBUFFER *Out, VBUFFER *In, BYTE *Dsp, 
   INT *Off, INT *Sat )
{
}
#endif      // 0


#endif   // UNIX

/************************************************************/
