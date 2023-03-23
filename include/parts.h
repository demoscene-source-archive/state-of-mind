/***********************************************
 *                particles                    *
 * Skal 98                                     *
 ***********************************************/

#ifndef _PARTICLES_H_
#define _PARTICLES_H_

/******************************************************************/

struct PARTICLE {
#pragma pack(1)

   OBJECT_FIELDS

   VECTOR     *P;   // pos
   VECTOR     *V;       // speed
   INT         Nb_Parts;

   SHADER_METHODS *Shader;
   SPLINE_3DS *Pos_k, *Rot_k, *Scale_k, *Morph_k, *Hide_k;
};

extern INT New_Particles( OBJ_NODE *Node, INT Nb );

extern NODE_METHODS _PARTICLE_;

/******************************************************************/

#endif   // _PARTICLES_H_
