/************************************************
 *           JPEG internal header               *
 *                                              *
 * Skal 98                                      *
 ************************************************/

#ifndef _IJPEG_H_
#define _IJPEG_H_

/*******************************************************/

   // in iycc.c

extern void RGB_To_Ycc( FLT In[3][64], FLT Out[3][64], FLT DCTShift );

   // in ihuff.c

extern void JPG_Encode_AC( INT *Matrix, EHUFF *EHuff );
extern void JPG_Encode_DC( INT Coef, INT *Last_DC, EHUFF *EHuff );

extern FILE *JPG_F_Out;
extern INT   JPG_Out_Bits;
extern INT   JPG_Bit_Pos;

   // Type = 0 => full Ycc
   // Type = 1 => Y only (grayscale)

extern BYTE *JPEG_Out( STRING Name, BYTE *Bits, 
   INT Width, INT Height, INT Type );
extern BYTE *JPEG_Out_Raw( FILE *Out, BYTE *Bits, INT Width, INT Height,
   INT Type );

/*******************************************************/

#endif   // _IJPEG_H_
