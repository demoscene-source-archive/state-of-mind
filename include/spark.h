/***********************************************
 *      Spark effect                           *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#ifndef _SPARK_H_
#define _SPARK_H_

   // in spark.c

extern PIXEL Spark_Tab[256];
extern void Build_Spark_Tab( INT MapX, INT MapY, INT Amp, INT Offset, FLT Gam );

extern void Do_Spark_Blur( FLT Xo, FLT dXo, FLT *Len, FLT *dLen, 
   INT Nb, PIXEL *Dst, INT MapX, INT MapY );
extern void Do_Spark_Blur_II( FLT Xo, FLT dXo, FLT *Len, FLT *dLen, 
   INT Nb, PIXEL *Dst, INT MapX, INT MapY );

extern USHORT *Setup_Spark( USHORT *Map, PIXEL *Bits, INT W, INT H,
  FLT Xo, FLT Yo, INT MapX, INT MapY );
extern void Paste_Spark( USHORT *Src, PIXEL *Map, PIXEL *Dst, INT BpS,
   INT W, INT H, PIXEL *Conv );

#endif   // _SPARK_H_
