/************************************************
 *           JPEG internal header               *
 *                                              *
 * Skal 98                                      *
 ************************************************/

#ifndef _JPEG_H_
#define _JPEG_H_

/*******************************************************/

#define BLOCK_X   8
#define BLOCK_Y   8
#define BLOCKSIZE 64       // BLOCK_X*BLOCK_Y
#define MAX_DEVS 16
#define MAX_SOURCES 16
#define MAX_COMPONENTS 256

/*******************************************************/

typedef struct {
  INT bits[36];
  INT huffval[257];
} XHUFF;

   // Decoder tables

typedef struct {
   INT ml;
   INT maxcode[36], mincode[36];
   INT valptr[36];
} DHUFF;

typedef struct {
   INT ehufco[257], ehufsi[257];
} EHUFF;

/*******************************************************/

typedef struct {
   SHORT *Ycc;
   void *Where;
   BYTE dx, dy;
   SHORT Comp;
} MDU_BLOCK;

typedef struct {

   UINT Marker;   // <= typedef enum...
   INT    Image_Sequence;
   INT    End_Of_Image;
   INT    Nb_MDU, Cur_MDU;
   INT    NbQuantMatrices;
   INT   *QuantMatrices[MAX_DEVS];

   INT    Nb_AC_Tab;
   DHUFF *ACDHuff[MAX_DEVS];
   XHUFF *ACXHuff[MAX_DEVS];
   INT    Nb_DC_Tab;
   DHUFF *DCDHuff[MAX_DEVS];
   XHUFF *DCXHuff[MAX_DEVS];

      // Frame
   INT Type;
   INT Q;
   INT Data_Precision;
   FLT Scale;
   INT Global_Height;
   INT Global_Width;
   INT Global_Nb_Comp;
   INT cn[MAX_COMPONENTS];
   INT xf[MAX_COMPONENTS];
   INT yf[MAX_COMPONENTS];
   INT tq[MAX_COMPONENTS];
   INT Width[MAX_COMPONENTS];
   INT Height[MAX_COMPONENTS];
   INT Max_X, Max_Y;
   INT MDUWide, MDUHigh;

   SHORT *Base;
   void *Dst;
   INT Unit, Size;
   USHORT Format, Quantum;
   MDU_BLOCK *Blocks;

} JPG_INFO;

typedef struct {

   INT Nb_Comp;
   INT SSS, SSE, SAH, SAL;
   INT MDUWide, MDUHigh;
   INT ci[MAX_SOURCES];
   INT ta[MAX_SOURCES];
   INT td[MAX_SOURCES];
   INT Last_DC[MAX_SOURCES];

} JPG_SCAN;

/*******************************************************/
/*******************************************************/

   // macros

#define MAX(x,y) ( (x>y) ? x:y )
#define MIN(x,y) ( (x>y) ? y:x )

#define HI15(c)   ( ((c)>>4)&0x0F )
#define LO15(c)   ( (c)&0x0F )

/*******************************************************/

   // in ld_jpg.c
extern void  JPG_Init_Info( JPG_INFO *Info );
extern void  JPG_Make_Scan( JPG_SCAN *Scan );
extern void  JPG_Clean_Up_Info( JPG_INFO *Info );

   // in markers.c
extern void *JPG_Decode( JPG_INFO *Info, INT *Width, INT *Height );

   // in huff.c
extern void JPG_ReadHuffman( XHUFF *XHuff, DHUFF *DHuff );
extern void JPG_Specified_Huffman( INT *bts, INT *hvls, XHUFF *XHuff, EHUFF *EHuff );
extern INT  JPG_Decode_DC( INT *Matrix,  XHUFF *XHuff, DHUFF *DHuff );
extern void JPG_Decode_AC( INT *Matrix,  XHUFF *XHuff, DHUFF *DHuff );

   // in adct.c
extern void Do_Chen_IDCT( FLT Matrix[64] );
extern void Do_Chen_DCT( FLT Matrix[64] );

   // in ycc.c
extern void JPG_Allocate_MDU_Blocks( JPG_INFO *Info );
extern void JPG_Write_Matrix( JPG_INFO *Info, FLT Matrix[BLOCKSIZE],
   MDU_BLOCK *Block, 
   INT x, INT y, INT nx, INT ny,
   FLT Shift );

/*******************************************************/
/*******************************************************/

#endif   // _JPEG_H_
