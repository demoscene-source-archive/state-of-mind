/***********************************************
 *      Data array                             *
 *  Constant-sized objects handling...         *
 *                                             *
 * Skal 97                                     *
 ***********************************************/ 

#ifndef _ARRAY_H_
#define _ARRAY_H_

/******************************************************************/

typedef struct {
#pragma pack(1)

   INT Nb_Elm, Max_Elm, Elm_Size;
   void *Data;

} DATA_ARRAY;

extern void *Remove_Array_Elm( DATA_ARRAY *Array, void *Elm );
extern void *Add_Array_Element( DATA_ARRAY *Array );
extern void *Init_Array( INT Nb_Elm, INT Size, DATA_ARRAY *Array );
extern void Clear_Array( DATA_ARRAY *Array );
extern void *Clean_Up_Array( DATA_ARRAY *Array );

/******************************************************************/

#endif   /* _ARRAY_H_ */
