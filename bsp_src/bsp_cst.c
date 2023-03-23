/***********************************************
 *              BSP                            *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#include "main3d.h"
#include "bsp.h"

/******************************************************************/

EXTERN BSP_CST Bsp_Params;

EXTERN void BSP_Init_Params( )
{
   Mem_Clear( &Bsp_Params );
   Bsp_Params.Max_Try = 50;
   Bsp_Params.Split_Stop_Nb = 1;
   Bsp_Params.Test_Amp_Base = 0.1;
   Bsp_Params.Test_Amp = Bsp_Params.Test_Amp_Base;
}

/******************************************************************/
