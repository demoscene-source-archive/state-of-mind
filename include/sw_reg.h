/*********************************************
 *                                           *
 *	   sw_region.h                            *
 *                                           *
 * Skal widgets v0.0 (c)1997                 *
 * Pascal.Massimino@ens.fr                   *
 *********************************************/

#ifndef _SW_REGION_H_
#define _SW_REGION_H_

#include "sw_main.h"

/********************************************************/

extern SW_ASPECT *SW_New_Aspect( );
extern void SW_Destroy_Aspect( SW_ASPECT *Aspect );

#if 0
extern void SW_Put_Bitmap_Into_Zone( MEM_ZONE *M, BITMAP *Btm,
   INT X, INT Y );
extern void SW_Tile_Bitmap_Into_Zone( MEM_ZONE *M, BITMAP *Btm,
   INT X, INT Y );
#endif

extern void SW_Fill_Zone_With_Color( MEM_ZONE *M, PIXEL Color );

/********************************************************/

#endif	/* _SW_REGION_H_ */

