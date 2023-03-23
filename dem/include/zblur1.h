/*
 * ZBlur
 *
 * Skal 98 (skal.planet-d.net)
***************************************/

/*******************************************************************/

extern void Do_ZBlur_II_Asm( PIXEL *Dst, PIXEL *Src, USHORT *ZMap, INT BpS );
extern void Do_ZBlur_III_Asm( PIXEL *Dst, PIXEL *Src, INT BpS );

extern void Do_ZBlur_II( PIXEL *Dst, PIXEL *Src, USHORT *ZMap, INT BpS );
extern void Init_ZBlur_Index_I( INT Z_Max, FLT Focal, FLT Gamma, INT Levels );
extern void Do_ZBlur_III( PIXEL *Dst, PIXEL *Src, INT BpS );

/*******************************************************************/
