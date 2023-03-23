/***********************************************
 *      Blur utilities                         *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#ifndef _BLUR_H_
#define _BLUR_H_

extern void Init_Blur_Map_I( PIXEL *Tab, INT Size, INT Levels, 
   INT Amp, INT Offset );
extern void Init_Blur_Map_II( PIXEL *Tab, INT Size, INT Levels, 
   INT Amp, INT Offset );

#endif   // _BLUR_H_

