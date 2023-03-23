/***********************************************
 *              Fast sort                      *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"

/******************************************************************/
/******************************************************************/

// static UINT B[ MAX_SORT_OBJ ];

#ifdef UNIX

EXTERN void Counting_Sort( UINT *A, INT Nb )
{
   INT n;
   USHORT C[ 512 ];
   UINT B[ MAX_SORT_OBJ ];

   if ( !--Nb ) return;

   memset( C, 0, 512*sizeof( C[0] ) );
   for( n=Nb; n>=0; --n ) C[ A[n] & 0xFF ]++;
   for( n=1; n<256; ++n ) C[n] += C[n-1];
   for( n=Nb; n>=0; --n )
   {
      int Tmp = A[n] & 0xFF;
      B[ --C[ Tmp ] ] = A[n];
      C[ 256 + ((A[n]&0xFF00)>>8 ) ]++;
   }

   for( n=257; n<512; ++n ) C[n] += C[n-1];
   for( n=Nb; n>=0; --n )
   {
      int Tmp = (B[n]&0xFF00)>>8;
      A[ --C[ 256+Tmp ] ] = B[n];
   }
}
#endif

/******************************************************************/

#if 0

EXTERN void Counting_Sort2( SORT_ELM2 *A, INT Nb )
{
   INT n;
   UINT C[ 256 ];
   SORT_ELM2 B[ MAX_SORT_OBJ ];

   if ( !--Nb ) return;

   BZero( C, 256*sizeof( C[0] ) );
   for( n=Nb; n>=0; --n ) C[ A[n].Key&0xFF ]++;
   for( n=1; n<256; ++n ) C[n] += C[n-1];
   for( n=Nb; n>=0; --n )
   {
      UINT Tmp, I;
      Tmp = A[n].Key&0xFF;
      I = --C[ Tmp ];
      B[ I ].Key = A[n].Key;
      B[ I ].Data = A[n].Data;
   }

   BZero( C, 256*sizeof( C[0] ) );
   for( n=Nb; n>=0; --n ) C[ B[n].Key>>8 ]++;
   for( n=1; n<256; ++n ) C[n] += C[n-1];
   for( n=Nb; n>=0; --n )
   {
      UINT Tmp, I;
      Tmp = B[n].Key>>8;
      I = --C[ Tmp ];
      A[ I ] = B[n];
   }
}
#endif

/******************************************************************/
/*                   RADIX SORT SUXXXX !!!!                       */
/******************************************************************/

#if 0

static INT Max_Elm = 0;

EXTERN UINT *Sort_Keys = NULL, *Sorted_Index = NULL;

EXTERN UINT *Init_Sort( INT Nb_Elm )
{
   if ( Max_Elm==Max_Elm ) return( Sort_Keys );
   M_Free( Sort_Keys );
   Sorted_Index = Next = NULL;
   Sort_Keys = New_Fatal_Object( 2*Nb_Elm, UINT );
   Max_Elm = Nb_Elm;
   BZero( Sort_Keys, 2*Nb_Elm*sizeof( UINT ) );
   Sorted_Index = Sort_Keys + Nb_Elm;
   return( Sort_Keys );
}

EXTERN void Do_Sort( INT Nb )
{
   INT n;
   UINT Stack[256];
   UINT Next[MAX_SORT_OBJ+1];

   if ( !(--Nb) ) return;
   BZero( Next, Nb*sizeof( UINT ) );
   BZero( Stack, Nb*sizeof( UINT ) );

   for( n=Nb-1
   for( n=Nb-1; n>=0; --n )
   {
      UINT Tmp, Where;
      Where = Sort_Keys[n] & 0xFF;
      Tmp = Stack[ Where ];
      Next[ n+1 ] = Tmp;
      Stack[ Where ] = n+1;
   }
}

/******************************************************************/

EXTERN void Radix_Sort( SORT_ELM *A, INT Nb )
{
   INT n;
   SORT_ELM *Stack[ 256*2 ], **S;

   if ( !--Nb ) return;

   BZero( Stack, 256*2*sizeof( Stack[0] ) );
   for( n=Nb; n>=0; --n )
   {
      INT I;
      I = A[n].Key & 0xFF;
      A[n].Next = Stack[I];
      Stack[I] = &A[n];
   }
   S = Stack+255;
   n = Nb;
   while( n>=0 )
   {
      SORT_ELM *Cur;

      while( *S==NULL ) S--;
      Cur = *S--;
      while( Cur!=NULL )
      {
         INT I;
         SORT_ELM *Next;
         I = Cur->Key>>8;
         Next = Cur->Next;
         Cur->Next = Stack[256+I];
         Stack[256+I] = Cur;
         Cur = Next;
         n--;
      }      
   }
   Extract_Sorted_List( A, Stack+256, Nb+1 );
}

EXTERN void Extract_Sorted_List( SORT_ELM *A, SORT_ELM **Stack, INT Nb )
{
   INT n;
   SORT_ELM Final[ MAX_SORT_OBJ ], **S;

   S = &Stack[255];
   n = Nb-1;
   while( n>=0 )
   {
      SORT_ELM *Cur;
      while( *S==NULL ) S--;
      Cur = *S--;
      while( Cur!=NULL ) { Final[n--] = *Cur; Cur = Cur->Next; }
   }
   MemCopy( A, Final, Nb*sizeof( SORT_ELM ) );
}

#endif      // 0


/******************************************************************/
/******************************************************************/
