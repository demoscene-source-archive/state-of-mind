/*
 * Scratch funcs
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

EXTERN void Init_Mask_VBuf( )
{
   SELECT_565_V( VMASK );
   SELECT_565_V( VMASK2 );
}

EXTERN void Setup_Mask( INT VMask, DMASK *Mask, USHORT Format )
{
   if ( Mask==NULL ) return;
   SELECT_565_V( VMask );
   Check_CMap_VBuffer( &VB(VMask), Mask->Nb_Col );
   CMap_To_16bits( (USHORT*)VB(VMask).CMap, Mask->Pal, Mask->Nb_Col, Format );
   VB(VMask).Bits = (void*)Mask->Bits;
}

/********************************************************************/

EXTERN DMASK *Load_DMask( STRING Name )
{
   DMASK *New;
   BITMAP *Tmp;
   INT Mask_Bits, Nb_Col;

   Tmp = Load_GIF( Name );
   Nb_Col = Tmp->Nb_Col;
   if ( Nb_Col<=8 ) Mask_Bits = 5;
   else if ( Nb_Col<=16 ) Mask_Bits = 4;
   else if ( Nb_Col<=32 ) Mask_Bits = 3;
   else if ( Nb_Col<=64 ) Mask_Bits = 2;
   else if ( Nb_Col<=128 ) Mask_Bits = 1;
   else Mask_Bits = 0;

   New = New_Fatal_Object( 1, DMASK );
   New->Bits = Split_Mask( Tmp->Bits, Tmp->Width*Tmp->Height,  Mask_Bits );
   New->Pal = New_Fatal_Object( Nb_Col*3, BYTE );
   memcpy( New->Pal, Tmp->Pal, 3*Nb_Col );
   New->Nb_Col = Nb_Col;
   New->Width = Tmp->Width;
   New->Height = Tmp->Height;
   New->Size = New->Width * New->Height;
   New->Off_X = ( The_W-New->Width ) / 2;
   New->Off_Y = ( The_H-New->Height ) / 2;
   Destroy_Bitmap( Tmp );
   return( New );
}


EXTERN void Paste_DMask( VBUFFER *Out, VBUFFER *Mask, VBUFFER *In )
{
   Mixer.Mix_8_8_Blend_Mask( Out, Mask, In );
}

EXTERN void Paste_DMask_16( VBUFFER *Out, VBUFFER *Mask, VBUFFER *In )
{
   Mixer.Mix_16_8_Blend_Mask( Out, Mask, In );
}

EXTERN void Paste_DMask_Move( VBUFFER *Out, VBUFFER *Mask, VBUFFER *In )
{
   Mixer.Mix_8_8_Blend_Mask( Out, Mask, In );
#if 0
   UINT Off;

   Off = Mask->Off_Y + ( random()&0x07 );
   Dst += Mask->Off_X + Off*Dst_BpS;
   Src += Mask->Off_X + Off*Dst_BpS;

   Mixer.Mix_8_8_Blend_Mask( 
      Dst, Dst_BpS,
      Mask->Bits, Src,
      Mask->Width, Mask->Height-Off, Src_BpS, CMap1, CMap2 );
#endif
}

EXTERN void Paste_DMask_Move_II( VBUFFER *Out, VBUFFER *Mask, VBUFFER *In )
{
   Mixer.Mix_8_8_Blend_Mask( Out, Mask, In );

#if 0
   UINT Off_X, Off_Y, Off;
   USHORT *Bits;

   Bits = Mask->Bits;
   Off_X = Mask->Off_X + ( random()&0x07 ) - 4;
   Off_Y = Mask->Off_Y + ( random()&0x07 ) - 4;
   if ( Off_X>0 ) Bits += Off_X;
   else Dst -= Off_X;
   if ( Off_Y>0 ) Bits += Off_Y*Src_BpS;
   else Dst -= Off_Y*Dst_BpS;

   Mixer.Mix_8_8_Blend_Mask( 
      Dst, Dst_BpS, Bits, Src,
      Mask->Width-8, Mask->Height-8, 
      Src_BpS, CMap1, CMap2 );
#endif
}

EXTERN void Saturate_DMask_Pal( VBUFFER *V, FLT x, DMASK *Mask, UINT Color, UINT Format )
{
   if ( x>=0.4 )
      CMap_To_16bits( (USHORT*)V->CMap, Mask->Pal, Mask->Nb_Col, Format );
   else
   {
      INT L;
      if ( x<0.1 )
      {
         L = (INT)( 255.0*x/0.1 );
         CMap_To_16bits_With_Fx( (USHORT*)V->CMap, Mask->Pal, Mask->Nb_Col, Format,
            0x000000, NULL, L, 0 );
      }
      else
      {
         L = (INT)( 255*(0.4-x)/0.3 );
         CMap_To_16bits_With_Fx( (USHORT*)V->CMap, Mask->Pal, Mask->Nb_Col, Format,
            Color, NULL, L, 0 );
      }
   }
}

EXTERN void Destroy_DMask( DMASK *Mask )
{
   if ( Mask==NULL ) return;
   M_Free( Mask->Pal );
   M_Free( Mask->Bits );
   M_Free( Mask );
}

EXTERN void Clear_DMask( )
{
   Destroy_DMask( Cur_Mask );
   Cur_Mask = NULL;
}

EXTERN void Clear_DMask_And_Anim( )
{
   if ( Cur_Anim!=NULL ) Destroy_16b( Cur_Anim );
   Cur_Anim = NULL;
   Destroy_DMask( Cur_Mask );
   Cur_Mask = NULL;
}

EXTERN void Extract_DMask_Raw_Col( DMASK *M, BYTE *Dst )
{
   INT i;
   for( i=0; i<M->Size; ++i )
      if ( ((USHORT*)M->Bits)[i]&0xFF )
         Dst[i] = 0xFF;
      else Dst[i] = 0x00;
}

EXTERN void Extract_DMask_Blend( DMASK *M, BYTE *Dst )
{
   INT i;
   for( i=0; i<M->Size; ++i )
      Dst[i] = ((USHORT*)M->Bits)[i]>>4;
}

/********************************************************************/
