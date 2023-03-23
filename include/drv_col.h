/*****************************************************
 *          Color utilities.
 *
 *                       Skal96
 *****************************************************/

#ifndef _DRV_COL_H_
#define _DRV_COL_H_

#include "main.h"
#include "lib_gfx.h"

/*****************************************************/

extern void Drv_RGB_To_CMap_Entry( COLOR_ENTRY *CMap, PIXEL *Ptr, INT Nb ) ;
extern void Drv_Transfert_CMap( COLOR_ENTRY *CMap, INT Start, PIXEL *Ptr, INT Nb );

extern void Drv_Fill_Pal( COLOR_ENTRY *Pal, INT I, PIXEL r, PIXEL g, PIXEL b );
extern void Drv_Copy_Pal( COLOR_ENTRY *Src, COLOR_ENTRY *Dst, INT );
extern void Drv_Black_Pal( COLOR_ENTRY *Pal, INT I );

extern COLOR_ENTRY *Drv_New_Colormap( INT Size );

/*****************************************************/

extern void Drv_Build_Ramp( COLOR_ENTRY *Pal, INT Start, INT End,
	INT r1, INT g1, INT b1, INT r2, INT g2, INT b2 );

extern void Drv_Build_RGB_Cube( COLOR_ENTRY *, FORMAT );

// extern INT Skl_Best_Match_RGB( PIXEL R, PIXEL G, PIXEL B, COLOR_ENTRY *C, INT n );

extern void Skl_Store_Formatted_CMap( UINT *Dst, COLOR_ENTRY *CMap, INT Nb, FORMAT Format );
extern void Skl_Store_CMap( COLOR_ENTRY *Dst, COLOR_ENTRY *Src, INT Nb );
extern void Skl_Match_CMaps( COLOR_ENTRY *Src, COLOR_ENTRY *Dst, INT Nb );

/*****************************************************/


#endif   // _DRV_COL_H_
