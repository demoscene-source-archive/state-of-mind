/***********************************************
 *      Z-Blur utilities                       *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#ifndef _ZBLUR_H_
#define _ZBLUR_H_

extern UINT *Init_ZBlur_Index( INT Z_Max, FLT Focal, FLT Gamma,
   UINT **Z_Index, INT Levels );

extern PIXEL *Init_ZBlur_Tables( 
   INT Z_Max, INT Levels,
   PIXEL **W_Tab, COLOR_ENTRY *Col_Tab, INT Nb );

#endif   // _ZBLUR_H_

