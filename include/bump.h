/***********************************************
 *      Bump map utilities                     *
 *                                             *
 * Skal 98                                     *
 ***********************************************/

#ifndef _BUMP_H_
#define _BUMP_H_

   // in bump.c

extern SHORT *Build_Mult_Tab( );
extern BITMAP *Do_The_Emboss( BITMAP *BTM, PIXEL Mask );

   // in bump2.c

extern PIXEL *Build_Bump_Light_Tab( PIXEL *Ptr, FLT Gamma );
extern void Height_To_Bump( char *Dst, PIXEL *Src, INT W, INT H );
extern void Do_Da_Bump_I( PIXEL *Dst, INT Dst_BpS, PIXEL *Src, PIXEL *Env, 
   INT W, INT H, INT xo, INT yo );
extern void Do_Da_Bump_II( PIXEL *Dst, INT Dst_BpS, PIXEL *Src, PIXEL *Env, 
   INT W, INT H, INT xo, INT yo );

   // in bump3.c or bump3.asm

extern void Bumpify_I( PIXEL *Dst, PIXEL *Src, INT W, INT H,
   INT Dst_BpS, INT Src_BpS, INT xo, INT yo, PIXEL *Env_Map );
extern void Bumpify_II( PIXEL *Dst, PIXEL *Src, INT W, INT H,
   INT Dst_BpS, INT Src_BpS, INT xo, INT yo, PIXEL *Env_Map );

  // in clamp.c

#define CLAMP_OFF 320
#define CLAMP_256(x) (I_Table[CLAMP_OFF+(x)])
#define CLAMP2_256(x) (I_Table2[CLAMP_OFF+(x)])
extern PIXEL I_Table[256+2*CLAMP_OFF]; 
extern USHORT I_Table2[256+2*CLAMP_OFF]; 
extern void Init_Saturation_Table( INT C0, INT C1, FLT Gamma );
extern void Init_Saturation_Table2( INT C0, INT C1, FLT Gamma, INT Scale );
extern void Init_Saturation_Table3( USHORT *,INT C0, INT C1, FLT Gamma, INT W, INT Amp );

extern void Init_Saturation_Table4( 
   INT *Tab, INT C0, INT C1, FLT Gamma, INT W, INT Amp );
extern void Init_Saturation_Table5( 
   INT *Tab, INT C0, INT C1, FLT Gamma, INT W, INT Off1, INT Amp, INT Off2 );

#endif /* _BUMP_H_ */
