/***********************************************
 *      Data array                             *
 *  Constant-sized objects handling...         *
 *                                             *
 * Skal 96                                     *
 ***********************************************/
    
#include "main3d.h"

/******************************************************************/

EXTERN INLINE void Clear_Array( DATA_ARRAY *Array )
{
   M_Free( Array->Data );
   Array->Nb_Elm = 0;
   Array->Max_Elm = 0;
}

EXTERN void *Init_Array( INT Nb_Elm, INT Size, DATA_ARRAY *Array )
{
   Clear_Array( Array );
   Array->Elm_Size = Size;
   Size *= Nb_Elm;
   if ( Nb_Elm )
   {
      Array->Data = ( void *)New_Object( Size, PIXEL );
      if ( Array->Data == NULL ) Exit_Upon_Mem_Error( "Array data", Size );
      memset( Array->Data, 0, Size );
   }
   else Array->Data = NULL;
   Array->Nb_Elm = 0;
   Array->Max_Elm = Nb_Elm;
   return( Array->Data );
}

EXTERN void *Add_Array_Element( DATA_ARRAY *Array )
{
   void *New;

   if ( Array->Data == NULL ) Init_Array( 1, Array->Elm_Size, Array );
   if ( Array->Nb_Elm == Array->Max_Elm )
   {
      New = My_Realloc( Array->Data, ( Array->Max_Elm*2 )*Array->Elm_Size );
      if ( New == NULL ) return( NULL );
      Array->Data = New;
      Array->Max_Elm *= 2;
   }
   New = (void *)( (UINT)Array->Data + Array->Nb_Elm*Array->Elm_Size );
   Array->Nb_Elm++;
   return( New );
}

EXTERN void *Remove_Array_Elm_Nb( DATA_ARRAY *Array, INT Pos )
{
   INT Where, What;
   void *New;
   if ( Pos<0 || Pos>= Array->Nb_Elm )
   {
      DEBUG( fprintf( stderr, "Remove_Array_Elm_Nb() failed (%d)", Pos ) );
      return( NULL );
   }

   Where = (INT)Array->Data + Pos*Array->Elm_Size;
   What = (INT)Array->Data + (Array->Nb_Elm-1)*Array->Elm_Size;
   memcpy( (void *)Where, (void *)What, Array->Elm_Size );
   Array->Nb_Elm--;
   if ( Array->Nb_Elm<Array->Max_Elm/2 )
   {
      New = My_Realloc( Array->Data, (Array->Max_Elm/2)*Array->Elm_Size );
      if ( New == NULL ) return( Array->Data );
      Array->Data = New;
      Array->Max_Elm = Array->Max_Elm/2;
   }
   return( (void *)( (INT)Array->Data + Pos*Array->Elm_Size ) );
}

EXTERN void *Remove_Array_Elm( DATA_ARRAY *Array, void *Elm )
{
   INT Pos;

   if ( Array->Nb_Elm == 0 || Array->Elm_Size == 0 || Array->Data == NULL )
      return( NULL );
   Pos = ( (int)Elm - (int)Array->Data )/Array->Elm_Size;
   return( Remove_Array_Elm_Nb( Array, Pos ) );
}

EXTERN void *Clean_Up_Array( DATA_ARRAY *Array )
{
   void *New;

   if ( Array->Nb_Elm == Array->Max_Elm ) return( Array->Data );
   New = My_Realloc( Array->Data, Array->Nb_Elm*Array->Elm_Size );
   if ( New == NULL ) return( Array->Data );
   Array->Data = New;
   Array->Max_Elm = Array->Nb_Elm;
   return( New );
}

/******************************************************************/
