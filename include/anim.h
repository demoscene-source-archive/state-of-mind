/***********************************************
 *             Animation                       *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _ANIM_H_
#define _ANIM_H_

/******************************************************************/
/******************************************************************/

typedef struct {
#pragma pack(1)

   FLT Frame;
   FLT Tens, Cont, Bias;    /* tension, continuity, bias */
   FLT Ease_In, Ease_Out;
   FLT Values[4];          // Max size: 4*sizeof(FLT) 
   FLT Left[4], Right[4];  /* Position & left/right tangents */

} A_KEY;

typedef struct {
#pragma pack(1)

   INT Flags; 
   INT Nb_Keys, Data_Size, Cache;
   A_KEY *Keys;

} SPLINE_3DS;

#define TRACK_NO_FLAG 0x0000
#define TRACK_REPEAT  0x0001
#define TRACK_LOOP    0x0002
#define TRACK_DUMMY_C 0x0004
#define TRACK_DUMMY_L 0x0008
#define TRACK_DUMMY_M 0x0010

      // TODO: Should be done included as Method
extern void Anim_Camera_3ds( void *Dummy, FLT Frame );
extern void Anim_Mesh_3ds( void *Dummy, FLT Frame );
extern void Anim_Light_3ds( void *Dummy, FLT Frame );
extern void Anim_Group_3ds( void *Dummy, FLT Frame );

extern void Destroy_Spline_3ds( SPLINE_3DS **Sp );
extern SPLINE_3DS *New_Spline_3ds( SPLINE_3DS **Sp, INT Size, INT Nb );
extern A_KEY *Insert_Key_In_Spline_3ds( SPLINE_3DS *Sp, FLT T );

extern void Void_Spline( SPLINE_3DS *Sp, FLT *Result );
extern void Setup_Spline_3ds_Derivates( SPLINE_3DS *Sp );
extern void Setup_Rot_Spline_3ds_Derivates( SPLINE_3DS *Sp );
// extern INT Setup_Rot_Spline_3ds_Derivates( SPLINE_3DS *Sp_Rot, SPLINE_3DS *Sp_Rot_SQuad );
extern void Interpolate_Spline_3ds_Values( FLT T, SPLINE_3DS *Sp, FLT *Result );
extern void Interpolate_Rot_Spline_3ds_Values( FLT T, SPLINE_3DS *Sp, FLT *Result );
extern INT Interpolate_Spline_3ds_Hidden( FLT T, SPLINE_3DS *Sp );

/******************************************************************/

#endif   /* _ANIM_H_ */
