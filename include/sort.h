/***********************************************
 *              Fast sort                      *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#ifndef _SORT_H_
#define _SORT_H_

/******************************************************************/

/* #define MAX_SORT_OBJ   (65536) */
#define MAX_SORT_OBJ   (32767)
extern void Counting_Sort( UINT *A, INT Nb );
// extern void Counting_Sort2( UINT *A, INT Nb );

#if 0
typedef struct {

  UINT Key;
  void *Data;
  void *Next;
  void *Dummy;

} SORT_ELM;

typedef struct {

  UINT Key;
  void *Data;

} SORT_ELM2;

extern void Counting_Sort2( SORT_ELM2 *A, INT Nb );

#endif

/******************************************************************/

/*
extern void *Stack_Low[];
extern void Radix_Sort( void *Objects, INT Nb );
extern void **Extract_Sorted_List( void **, INT Nb );
extern void Radix_Sort( SORT_ELM *Obj, INT Nb );
extern void Extract_Sorted_List( SORT_ELM *A, SORT_ELM **Stack, INT Nb );
*/

/******************************************************************/

#endif   /* _SORT_H_ */
    
