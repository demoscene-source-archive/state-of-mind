/***********************************************
 *              group.h                        *
 * Skal 97                                     *
 ***********************************************/

#ifndef _GROUP_H_
#define _GROUP_H_

/******************************************************************/

struct GROUP {
#pragma pack(1)

   OBJECT_FIELDS

   SPLINE_3DS *Pos_k, *Rot_k, *Scale_k, *Hide_k;

};

struct OBJ_DUMMY {
#pragma pack(1)

   OBJECT_FIELDS

   SPLINE_3DS *Pos_k, *Rot_k, *Scale_k, *FOV_k;
   SPLINE_3DS *Roll_k, *Morph_k, *Hide_k;

};

extern NODE_METHODS _GROUP_;
extern NODE_METHODS _OBJ_DUMMY_;

/******************************************************************/

#endif   /* _GROUP_H_ */
