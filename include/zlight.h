/***********************************************
 *              z-Lights                       *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _ZLIGHT_H_
#define _ZLIGHT_H_

// #define USE_OLD_ZLIGHT_CODE

/******************************************************************/

#ifdef USE_OLD_ZLIGHT_CODE

// extern PIXEL Shdw_Attenuation_Table[16*16*256]; // Remove from static !!
extern PIXEL Shdw_Attenuation_Table[16*16]; // Remove from static !!
extern void Render_zBuf_Light( );
extern void Render_zBuf_Light2( );
extern void Init_Attenuation_Table( );
extern void Init_Volumic_Attenuation_Table( );

#endif   // USE_OLD_ZLIGHT_CODE

/******************************************************************/

#endif   // _ZLIGHT_H_
