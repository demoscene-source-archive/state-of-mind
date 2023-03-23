/***************************************
 *         mixers header               *
 *                                     *
 *                   Skal 96           *
 ***************************************/

#ifndef _MIXER_H_
#define _MIXER_H_

extern void Mix_Raw( PIXEL *, PIXEL *, INT );
extern void Mix_With_Background( PIXEL *, PIXEL *, INT, UINT );
extern void Mix_With_BTM_Background( PIXEL *, PIXEL *, PIXEL *, INT );

extern void Mix_With_Blur( PIXEL *, PIXEL *, INT );
extern void Mix_With_Blur2( PIXEL *, PIXEL *, INT );
extern void Mix_With_Blur3( PIXEL *, PIXEL *, PIXEL *, INT );
extern void Mix_With_Blur4( PIXEL *, PIXEL *, PIXEL *, INT );
extern void Mix_With_Blur4_Final( PIXEL *, PIXEL *, PIXEL *, PIXEL *, INT );

extern void Mix_With_Dsp_1( PIXEL *, PIXEL *, PIXEL *, PIXEL *, INT );
extern void Mix_With_Dsp_2( PIXEL *, PIXEL *, PIXEL *, PIXEL *, INT );
extern void Mix_With_Dsp_3( PIXEL *, PIXEL *, PIXEL *, PIXEL *, INT );

extern void Mix_With_Thresh( PIXEL *, PIXEL *, PIXEL *, INT, INT, INT );
extern void Mix_With_Thresh2( PIXEL *, PIXEL *, PIXEL *, INT, INT );
extern void Mix_With_Thresh3( PIXEL *, PIXEL *, PIXEL *, PIXEL *, INT, INT );

extern void Mix_Dsp_Map( PIXEL *, PIXEL *, PIXEL *, INT );

#endif   /* _MIXER_H_ */
