/*****************************************************
 *          Color utilities.
 *
 *                       Skal96
 *****************************************************/

#ifndef _COLOR_H_
#define _COLOR_H_

extern PIXEL *New_Colormap( INT );

extern void Build_Ramp( PIXEL *Pal, int Start, int End, 
	PIXEL r1, PIXEL g1, PIXEL b1, PIXEL r2, PIXEL g2, PIXEL b2 );
extern void Saturate_Pal( PIXEL *Src, PIXEL *Dst,
	PIXEL r1, PIXEL g1, PIXEL b1, PIXEL r2, PIXEL g2, PIXEL b2 );
extern void Saturate_Pal2( PIXEL *Src, PIXEL *Dst, int, int,
   PIXEL r2, PIXEL g2, PIXEL b2, double Gamma );
extern void Saturate_Color( PIXEL *Pal, 
   int How_Many, int Bunch, int Nb, 
   float Ambient, float Gamma, float Per,
   PIXEL rf, PIXEL gf, PIXEL bf );

extern INT Match_Best_RGB( PIXEL *Pal, INT Size, INT R, INT G, INT B );
extern PIXEL *Prepare_Blur_Pal( BITMAP *Btm );
extern PIXEL *Prepare_Blur_Pal_2( BITMAP *Btm );

extern void Gamma_Saturate( FCOLOR *CMap, INT Nb, INT Skip,
   FCOLOR Base, FCOLOR Light, double Gamma, double Sat );

/*****************************************************/


#endif   /* _COLOR_H_ */
