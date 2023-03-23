/*
 * Scratch funcs
 *
 * Skal 98
 ***************************************/

#include "demo.h"
#include "dmask.h"

/********************************************************************/

#if 0 // UNUSED
EXTERN void Setup_Mask( INT VMask, DMASK *Mask, UINT Format )
{
   if ( Mask==NULL ) return;
   SELECT_565_V( VMask );
   Check_CMap_VBuffer( &VB(VMask), Mask->Nb_Col );
   CMap_To_16bits( (void*)VB(VMask).CMap, Mask->Pal, Mask->Nb_Col, Format );
   Uncompress_DMask( Mask, NULL );
   VB(VMask).Bits = (void*)Mask->Bits;
}
#endif

EXTERN void Setup_Mask_II( INT VMask, DMASK *DMask )
{
   UINT Pal[256];

   if ( DMask==NULL ) return;
   SELECT_CMAP_V( VMask );
   Check_CMap_VBuffer( &VB(VMask), 256 );
   Uncompress_DMask_II( DMask, NULL );
   VB(VMask).Bits = (void*)DMask->Bits;
   CMap_To_16bits( (void*)Pal, DMask->Pal, DMask->Nb_Col, 0x777 );
   DMask_Setup_Pal( (UINT*)VB(VMask).CMap, DMask, (PIXEL*)Pal );
}

/********************************************************************/

#if 0

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
   Mixer.Mix_8_8_Blend_Mask_II( Out, Mask, In );
}

#endif

/********************************************************************/

EXTERN void Paste_DMask_Move_II( VBUFFER *Out, VBUFFER *Mask, VBUFFER *In )
{
   VBUFFER Tmp_O, Tmp_M, Tmp_I;
   INT Off_X, Off_Y;

   Mem_Clear( &Tmp_O ); Duplicate_VBuffer( &Tmp_O, Out );
   Mem_Clear( &Tmp_M ); Duplicate_VBuffer( &Tmp_M, Mask );
   Mem_Clear( &Tmp_I ); Duplicate_VBuffer( &Tmp_I, In );
   Off_X = (TRandom(1)&0x03) - 2;
   Off_Y = (TRandom(0)&0x03) - 2;
   if ( Off_Y>0 ) 
   {
      Tmp_O.Bits = (void*)((PIXEL*)Tmp_O.Bits + Off_Y*Tmp_O.BpS);
      Tmp_O.H -= Off_Y;
      Tmp_M.H -= Off_Y;
      Tmp_I.H -= Off_Y;
   }      
   else 
   {
      Tmp_M.Bits = (void*)((PIXEL*)Tmp_M.Bits - Off_Y*Tmp_M.BpS);
      Tmp_I.Bits = (void*)((PIXEL*)Tmp_I.Bits - Off_Y*Tmp_I.BpS);
      Tmp_O.H += Off_Y;
      Tmp_M.H += Off_Y;
      Tmp_I.H += Off_Y;
   }
   Mixer.Mix_8_8_Blend_Mask_II( &Tmp_O, &Tmp_M, &Tmp_I );
}

EXTERN void Saturate_DMask_Pal( VBUFFER *V, FLT x, DMASK *Mask, UINT Color )
{
   UINT Pal[256];

   if ( x>=0.4 )
      CMap_To_16bits( (void*)Pal, Mask->Pal, Mask->Nb_Col, 0x777 );
   else
   {
      INT L;
      if ( x<0.1 )
      {
         L = (INT)( 255.0*x/0.1 );
         CMap_To_16bits_With_Fx( (void*)Pal, Mask->Pal, Mask->Nb_Col, 0x777,
            0x000000, NULL, L, 0 );
      }
      else
      {
         L = (INT)( 255*(0.4-x)/0.3 );
         CMap_To_16bits_With_Fx( (void*)Pal, Mask->Pal, Mask->Nb_Col, 0x777,
            Color, NULL, L, 0 );
      }
   }
   DMask_Setup_Pal( (UINT*)V->CMap, Mask, (PIXEL*)Pal );
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
   if ( Cur_Mask!=NULL ) Destroy_DMask( Cur_Mask );
   Cur_Mask = NULL;
}

EXTERN void Extract_DMask_Raw_Col( DMASK *M, BYTE *Dst )
{
   INT i;
   INT Mask = (1<<(8-M->MBits))-1;
// INT Val = (1<<(7-M->MBits))-1;
   for( i=0; i<M->Size; ++i )
//      if ( (((PIXEL*)M->Bits)[i]&Mask)<Mask ) Dst[i] = 0xFF;
//      else Dst[i] = 0x00;
      if ( (((PIXEL*)M->Bits)[i]&Mask) ) Dst[i] = 0xFF;
      else Dst[i] = 0x00;
}

EXTERN void Extract_DMask_Blend( DMASK *M, BYTE *Dst )
{
   INT i;
   BYTE Mask;
   Mask = (1<<(8-M->MBits))-1;
   for( i=0; i<M->Size; ++i )
      Dst[i] = ((BYTE*)M->Bits)[i]&Mask;
}

/********************************************************************/

