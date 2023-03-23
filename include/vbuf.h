/***********************************************
 *       Virtual buffers (VBUF...)             *
 * Skal 98                                     *
 ***********************************************/

#ifndef _VBUF_H_
#define _VBUF_H_

/********************************************************************/

typedef enum { VEMPTY, V565, V777, V888, VCMAP, VSCR } VBUFFER_TYPE;

#define VBUF_VIRTUAL    0x01
#define VBUF_COMPRESSED 0x02
#define VBUF_OWN_CMAP   0x04

typedef struct 
{
#pragma pack(1)

   VBUFFER_TYPE Type;
   INT W, H, BpS, Size;
   USHORT Flags;
   USHORT Quantum;
   void *Bits;
   INT Nb_Col;
   void *CMap;
   INT Xo, Yo;

} VBUFFER;

/********************************************************************/

extern void Select_VBuffer( 
   VBUFFER *VBuf, VBUFFER_TYPE Type,
   INT W, INT H, INT BpS, INT Flags );
extern void Check_CMap_VBuffer( VBUFFER *VBuf, INT N );
extern void Duplicate_VBuffer( VBUFFER *Out, VBUFFER *In );
extern VBUFFER *Setup_Clipped_VBuffer( 
   VBUFFER *Out1, VBUFFER *Out2, VBUFFER *Out3,
   VBUFFER *In1, VBUFFER *In2, VBUFFER *In3,
   INT X2, INT Y2, INT X3, INT Y3 );
extern void Virtualize_VBuffer( VBUFFER *VBuf, 
   VBUFFER *Src, VBUFFER *In, INT Xo, INT Yo );
extern void Extract_Virtual_VBuffer( VBUFFER *Out, 
   VBUFFER *In, INT Xo, INT Yo, INT W, INT H );
extern void Dispose_VBuffer( VBUFFER *VBuf );
extern void Destroy_VBuffer( VBUFFER *VBuf );

extern void VBuffer_Clear( VBUFFER *V );
extern void Copy_Buffer_Short( VBUFFER *Out, VBUFFER *In );
extern void Copy_Buffer_Short_Rev( VBUFFER *Out, VBUFFER *In );
extern void Map_16_Bits_Rev( VBUFFER *Out, VBUFFER *In );
extern void Copy_Buffer_Byte( VBUFFER *Out, VBUFFER *In );
extern void Copy_Buffer_Masked_Byte( VBUFFER *Out, VBUFFER *In, BYTE Mask );
extern void VBuf_Map_8_Bits( VBUFFER *Out, VBUFFER *In );
extern void VBuf_Map_8_Bits_Zero( VBUFFER *Out, VBUFFER *In );
extern void VBuf_Map_8_Bits_Transp( VBUFFER *Out, VBUFFER *In );
extern void Map_32_Bits( VBUFFER *Out, VBUFFER *In );

extern void Map_Scale_Up_8_Bits( VBUFFER *Out, VBUFFER *In );

/********************************************************************/

#endif   // _VBUF_H_

