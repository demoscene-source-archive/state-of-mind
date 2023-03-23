/***********************************************
 *       Virtual buffers (VBUF...)             *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
// #include "mem_map.h"
#include "vbuf.h"

EXTERN void Select_VBuffer( 
   VBUFFER *VBuf, VBUFFER_TYPE Type,
   INT W, INT H, INT BpS, INT Flags )
{
   INT Total, Quantum;

      // we want a virtual vbuf, and previous one wasn't
      // virtual. So => free previous malloc()

   if ( Flags & VBUF_VIRTUAL )
      if (!(VBuf->Flags&VBUF_VIRTUAL) )
      {
         M_Free( VBuf->Bits );
//         if ( VBuf->Flags&VBUF_OWN_CMAP ) M_Free( VBuf->CMap );
//         VBuf->Nb_Col = 0;
         VBuf->Flags &= ~VBUF_VIRTUAL;
         VBuf->Size = 0;
         VBuf->Type = VEMPTY;
         VBuf->Quantum = 0;
      }

   if ( Type==VSCR ) // special case for buffer #0  (=Screen buffer)
      return;        // should call Select_Screen_VBuffer() instead

   switch( Type )
   {
      case VEMPTY: Quantum = 0; break;
      case VCMAP: Quantum = 1; break;
      case V565: Quantum = 2; break;
      default: case V777: case V888: Quantum = 4; break;      
   }

   Total = H*BpS;
   if ( VBuf->Flags&VBUF_VIRTUAL )
   {
      VBuf->Bits = NULL;
      VBuf->Size = 0;
      VBuf->Type = VEMPTY;
      VBuf->Flags &= ~VBUF_VIRTUAL;
   }
   
   if ( Total>VBuf->Size )
   {
      M_Free( VBuf->Bits );
      if ( !(Flags&VBUF_VIRTUAL) )
      {
         VBuf->Bits = (void *)New_Fatal_Object( Total, BYTE );
         VBuf->Size = Total;
      }
   }
//   if ( VBuf->Flags&VBUF_OWN_CMAP ) M_Free( VBuf->CMap );
//   VBuf->Nb_Col = 0; // a priori.
   VBuf->Type = Type;
   VBuf->Flags = Flags | (VBuf->Flags&~VBUF_VIRTUAL);
   VBuf->W = W;
   VBuf->H = H;
   VBuf->BpS = BpS;
   VBuf->Quantum = (USHORT)Quantum;
   VBuf->Xo = VBuf->Yo = 0;
}

EXTERN void Check_CMap_VBuffer( VBUFFER *VBuf, INT N )
{
   if ( VBuf==NULL ) return;
   if ( (VBuf->Flags&VBUF_OWN_CMAP) && (VBuf->CMap!=NULL) && (N<=VBuf->Nb_Col) ) 
      return;
   if ( VBuf->Flags&VBUF_OWN_CMAP ) M_Free( VBuf->CMap );
   VBuf->CMap = New_Fatal_Object( N, UINT );
   VBuf->Nb_Col = N;
   VBuf->Flags |= VBUF_OWN_CMAP;
}

EXTERN void Duplicate_VBuffer( VBUFFER *Out, VBUFFER *In )
{
   if ( Out->Flags&VBUF_OWN_CMAP) M_Free( Out->CMap );
   if ( !(Out->Flags&VBUF_VIRTUAL) ) M_Free( Out->Bits );

   *Out = *In;

   Out->Flags &= ~VBUF_OWN_CMAP;
   Out->Flags |= VBUF_VIRTUAL;
}

EXTERN VBUFFER *Setup_Clipped_VBuffer( 
   VBUFFER *Out1, VBUFFER *Out2, VBUFFER *Out3,
   VBUFFER *In1, VBUFFER *In2, VBUFFER *In3,
   INT X2, INT Y2, INT X3, INT Y3 )
{
   Duplicate_VBuffer( Out1, In1 );
   Duplicate_VBuffer( Out2, In2 );
   Duplicate_VBuffer( Out3, In3 );
   return( Out1 );
}

EXTERN void Extract_Virtual_VBuffer( VBUFFER *Out, 
   VBUFFER *In, INT Xo, INT Yo, INT W, INT H )
{
   BYTE *Ptr;
   Duplicate_VBuffer( Out, In );
   Ptr = (BYTE*)In->Bits;
   Ptr += Yo*In->BpS + Xo*In->Quantum;
   Out->Bits = (void*)Ptr;
   Out->W = W;
   Out->H = H;
}

EXTERN void Virtualize_VBuffer( VBUFFER *VBuf, 
   VBUFFER *Src, VBUFFER *In, INT Xo, INT Yo )
{
   BYTE *Ptr;
   Select_VBuffer( VBuf, In->Type, 
      In->W, In->H, Src->BpS, VBUF_VIRTUAL );
   Ptr = (BYTE*)Src->Bits + Src->BpS * Yo;
   Ptr += Xo * Src->Quantum;
   VBuf->Bits = (void*)Ptr;
}

EXTERN void Dispose_VBuffer( VBUFFER *VBuf )
{
   if ( VBuf->Type==VSCR ) return;
//   VBuf->Flags = 0x00;
//   VBuf->Type = VEMPTY;
}

EXTERN void Destroy_VBuffer( VBUFFER *VBuf )
{
   if ( VBuf==NULL ) return;
   if ( VBuf->Type==VSCR ) return;

   if ( !(VBuf->Flags&VBUF_VIRTUAL) )
      M_Free( VBuf->Bits );
   VBuf->Bits = NULL;
   VBuf->Size = 0;
   if ( VBuf->Flags&VBUF_OWN_CMAP ) M_Free( VBuf->CMap );
   VBuf->Nb_Col = 0;
   VBuf->Flags = 0x00;
   VBuf->Type = VEMPTY;
}

/********************************************************************/

EXTERN void VBuffer_Clear( VBUFFER *V )
{
   INT j;
   BYTE *Dst = (BYTE*)V->Bits;
   for( j=V->H; j>0; --j )
   {
      memset( Dst, 0, V->BpS ); 
      Dst += V->BpS;
   }
}

EXTERN void Copy_Buffer_Short( VBUFFER *Out, VBUFFER *In )
{
   INT j;
   USHORT *Dst, *Src;
   Dst = (USHORT*)Out->Bits;
   Src = (USHORT*)In->Bits;
   for( j=Out->H; j>0; --j )
   {
      memcpy( Dst, Src, Out->W*sizeof( USHORT ) );
      Dst = (USHORT*)((BYTE*)Dst + Out->BpS);
      Src = (USHORT*)((BYTE*)Src + In->BpS);
   }
}

EXTERN void Copy_Buffer_Short_Rev( VBUFFER *Out, VBUFFER *In )
{
   INT j;
   USHORT *Dst, *Src;
   Dst = (USHORT*)Out->Bits;
   Src = (USHORT*)In->Bits;
   for( j=In->H; j>0; --j )
   {
      memcpy( Dst, Src, In->W*sizeof( USHORT ) );
      Dst = (USHORT*)((BYTE*)Dst + Out->BpS);
      Src = (USHORT*)((BYTE*)Src + In->BpS);
   }
}

EXTERN void Copy_Buffer_Byte( VBUFFER *Out, VBUFFER *In )
{
   INT j;
   BYTE *Dst, *Src;
   Dst = (BYTE*)Out->Bits;
   Src = (BYTE*)In->Bits;
   for( j=Out->H; j>0; --j )
   {
      memcpy( Dst, Src, Out->W );
      Dst = (BYTE*)((BYTE*)Dst + Out->BpS);
      Src = (BYTE*)((BYTE*)Src + In->BpS);
   }
}

EXTERN void Copy_Buffer_Masked_Byte( VBUFFER *Out, VBUFFER *In, BYTE Mask )
{
   INT i, j;
   BYTE *Dst, *Src;
   Dst = (BYTE*)Out->Bits + Out->W;
   Src = (BYTE*)In->Bits + Out->W;
   for( j=Out->H; j>0; --j )
   {
      for( i=-Out->W; i<0; ++i )
         Dst[i] = Src[i] & Mask;
      Dst = (BYTE*)((BYTE*)Dst + Out->BpS);
      Src = (BYTE*)((BYTE*)Src + In->BpS);
   }
}

EXTERN void VBuf_Map_8_Bits( VBUFFER *Out, VBUFFER *In )
{
   INT i, j;
   USHORT *Dst; PIXEL *Src;
   UINT *CMap;
   Dst = (USHORT*)Out->Bits + Out->W;
   Src = (PIXEL*)In->Bits + Out->W;
   CMap = (UINT*)In->CMap;
   for( j=Out->H; j>0; --j )
   {
      for( i=-Out->W; i<0; ++i ) Dst[i] = (USHORT)CMap[ Src[i] ];
      Dst = (USHORT*)((BYTE*)Dst + Out->BpS);
      Src = (PIXEL*)((BYTE*)Src + In->BpS);
   }
}

EXTERN void VBuf_Map_8_Bits_Transp( VBUFFER *Out, VBUFFER *In )
{
   INT i, j;
   USHORT *Dst; PIXEL *Src;
   UINT *CMap;
   Dst = (USHORT*)Out->Bits + Out->W;
   Src = (PIXEL*)In->Bits + Out->W;
   CMap = (UINT*)In->CMap;
   for( j=Out->H; j>0; --j )
   {
      for( i=-Out->W; i<0; ++i )
         if ( Src[i] ) Dst[i] = (USHORT)CMap[ Src[i] ];
      Dst = (USHORT*)((BYTE*)Dst + Out->BpS);
      Src = (PIXEL*)((BYTE*)Src + In->BpS);
   }
}

EXTERN void VBuf_Map_8_Bits_Zero( VBUFFER *Out, VBUFFER *In )
{
   INT i, j;
   USHORT *Dst; PIXEL *Src;
   USHORT *CMap;
   Dst = (USHORT*)Out->Bits + Out->W;
   Src = (PIXEL*)In->Bits + Out->W;
   CMap = (USHORT*)In->CMap;
   for( j=Out->H; j>0; --j )
   {
      for( i=-Out->W; i<0; ++i ) {
         Dst[i] = CMap[ Src[i] ];
         Src[i] = 0x00;
      }
      Dst = (USHORT*)((BYTE*)Dst + Out->BpS);
      Src = (PIXEL*)((BYTE*)Src + In->BpS);
   }
}

EXTERN void Map_16_Bits_Rev( VBUFFER *Out, VBUFFER *In )
{
   INT i, j;
   USHORT *Dst; PIXEL *Src;
   USHORT *CMap;
   Dst = (USHORT*)Out->Bits + In->W;
   Src = (PIXEL*)In->Bits + In->W;
   CMap = (USHORT*)In->CMap;
   for( j=In->H; j>0; --j )
   {
      for( i=-In->W; i<0; ++i ) Dst[i] = CMap[ Src[i] ];
      Dst = (USHORT*)((BYTE*)Dst + Out->BpS);
      Src = (PIXEL*)((BYTE*)Src + In->BpS);
   }
}

EXTERN void Map_32_Bits( VBUFFER *Out, VBUFFER *In )
{
   INT i, j;
   UINT *Dst; PIXEL *Src;
   UINT *CMap;
   Dst = (UINT*)Out->Bits + Out->W;
   Src = (PIXEL*)In->Bits + Out->W;
   CMap = (UINT*)In->CMap;
   for( j=Out->H; j>0; --j )
   {
      for( i=-Out->W; i<0; ++i ) Dst[i] = CMap[ Src[i] ];
      Dst = (UINT*)((BYTE*)Dst + Out->BpS);
      Src = (PIXEL*)((BYTE*)Src + In->BpS);
   }
}

/********************************************************************/

#ifdef UNIX

EXTERN void Map_Scale_Up_8_Bits( VBUFFER *Out, VBUFFER *In )
{
   INT i, j, ii, ki, kj;
   INT di, dj;
   USHORT *Dst;
   PIXEL *Src;
   USHORT *CMap;

   Dst = (USHORT*)Out->Bits + Out->W;
   Src = (PIXEL*)In->Bits; //  + Out->W;
   CMap = (USHORT*)In->CMap;

   di = ( In->W<<16 ) / Out->W;
   dj = ( In->H<<16 ) / Out->H;

   kj = 0; 
   for( j=Out->H; j>0; j-- )
   {
      ii = ki = 0;
      for( i=-Out->W; i<0; i++ )
      {
         if ( Src[ii] ) Dst[i] = CMap[ Src[ii] ];
         ki+=di; if ( ki>0xFFFF ) { ki &= 0xFFFF; ii++; }
      }
      kj += dj; if ( kj>0xFFFF ) { 
         kj &= 0xFFFF; 
         Src = (PIXEL*)((BYTE*)Src + In->BpS);
      }
      Dst = (USHORT*)((BYTE*)Dst + Out->BpS);
   }
}
#endif

/********************************************************************/
