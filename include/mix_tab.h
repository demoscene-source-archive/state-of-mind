/***********************************************
 * Misc. tables builders                       *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#ifndef _MIX_TAB_H_
#define _MIX_TAB_H_

   // mix a cmap with itself
extern PIXEL *Build_Mix_Tab( COLOR_ENTRY *Map, INT Nb, FLT x1, FLT x2 );
   // mix two cmap
extern PIXEL *Build_Mix_Tab_II( COLOR_ENTRY *Map, INT Nb, 
   COLOR_ENTRY *Map2, INT Nb2, FLT x1, FLT x2 );

   // add with saturation
extern PIXEL *Build_Sat_Tab( COLOR_ENTRY *Map, INT Nb, FLT x1, FLT x2 );
   // add two cmap with saturation
extern PIXEL *Build_Sat_Tab_II( COLOR_ENTRY *Map, INT Nb, 
   COLOR_ENTRY *Map2, INT Nb2, FLT x1, FLT x2 );

   // self-max
extern PIXEL *Build_Max_Tab( COLOR_ENTRY *Map, INT Nb );

   // weight cmap with weights in [0,1.0]
extern PIXEL *Build_Weight_Tab( COLOR_ENTRY *Map, FLT *Wght, INT Nb_Col, INT W );

extern void Gamma_Saturate_Base( INT Nb, FCOLOR Base, FCOLOR Light, 
   FCOLOR Ambient, FLT Gamma, FLT Sat, FCOLOR *Dst );
   
#endif   // _MIX_TAB_H_

