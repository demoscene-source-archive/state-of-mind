/***********************************************
 *                tree.c                       *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

EXTERN INT Count_Nodes( OBJ_NODE *Cur, INT *Nb, INT *P, INT *E )
{
   while( Cur!=NULL )
   {
      if ( Cur->Child!=NULL )
         Count_Nodes( Cur->Child, Nb, P, E );
      (*Nb)++;
      if ( Cur->Data!=NULL && Cur->Data->Type==MESH_TYPE )
      {
         *P += ((MESH*)Cur->Data)->Nb_Polys;
         *E += ((MESH*)Cur->Data)->Nb_Edges;
      }
      Cur=Cur->Next;
   }
   return( *Nb );
}

EXTERN INT Count_Objects( OBJECT *Cur )
{
   INT Nb;
   
   Nb = 0;
   while( Cur!=NULL ) { Nb++; Cur=Cur->Next; }
   return( Nb );
}


EXTERN INT Finish_Object_Data( OBJECT *Data, INT (*Finish)(OBJECT *) )
{
   if ( !(Data->Flags&OBJ_FINISHED) )
      return (*Finish)( Data );
   else return 0;
}

EXTERN INT Finish_All_Data( OBJECT *Cur, INT (*Finish)(OBJECT *) )
{
   while( Cur!=NULL )
   {
      OBJECT *Next = Cur->Next;
      if ( Finish_Object_Data( Cur, Finish ) )
         return( -1 );
      Cur = Next;
   }
   return 0;
}

EXTERN INT Finish_Objects( OBJ_NODE *Cur )
{
   while( Cur!=NULL )
   {
      if ( Cur->Data!=NULL )
      {
         Cur->Data->ID = Cur->ID;
            if ( NODE_FINISH( Cur ) )
               return( -1 );
        // else => Unkown object. Should set Err to -1 ??
      }

      if ( Cur->Parent != NULL ) 
         Insert_In_Upper_Box( &Cur->Parent->Box, &Cur->Box );

      if ( Cur->Flags&OBJ_OWNS_NAME )
      {
         M_Free( Cur->Name );
         Cur->Flags = (OBJ_FLAG)( Cur->Flags & ~OBJ_OWNS_NAME );
      }
      if ( (Cur->Data!=NULL) && (Cur->Data->Flags&OBJ_OWNS_NAME) )
      {
         M_Free( Cur->Data->Name );
         Cur->Data->Flags = (OBJ_FLAG)( Cur->Data->Flags & ~OBJ_OWNS_NAME );
      }
      if ( Cur->Child!=NULL ) Finish_Objects( Cur->Child );
      Cur = Cur->Next;
   }
   return( 0 );
}

/******************************************************************/

EXTERN void Check_Node_Transform( OBJ_NODE *Cur )
{
   if ( Cur->Parent!=NULL )
   {
      if ( (_RCst_.The_Camera==NULL) || (Cur->Parent->Camera_ID!=_RCst_.The_Camera->ID) )
         Check_Node_Transform( Cur->Parent );
   }
   if ( Cur->Time_Stamp<_RCst_.Frame_Stamp )
   {
      if ( Cur->Flags & OBJ_DO_ANIM )
      {
            // TODO: Warning! If more than 1 cam, no need to call it again!
         NODE_ANIM( Cur, _RCst_.The_World->Time );
      }
      STORE_TRANSFORM_MATRIX( Cur );      // <= updates Cur->Mo
      if ( Cur->Parent!=NULL )
         Mult_Matrix2_AB( Cur->Parent->Mo, Cur->Mo );   // M.Mo => Mo     (hierarchy updating)
   }   
}

EXTERN void Traverse_All_Nodes( OBJ_NODE *Cur, void (*Func)( OBJ_NODE *) )
{
   while( Cur!=NULL )   
   {
      OBJ_NODE *Next, *Child;
      Next = Cur->Next; Child = Cur->Child;
      (*Func)( Cur );   // first, process the parent
      if ( Child!=NULL ) Traverse_All_Nodes( Child, Func );
      Cur = Next;
   }
}

EXTERN void Traverse_Objects( OBJECT *Cur, void (*Func)( OBJECT *) )
{
   while( Cur!=NULL )   
   {
      (*Func)( Cur );
      Cur=Cur->Next;
   }
}

#if 0       // deprecated?

EXTERN void Traverse_Nodes( OBJ_NODE *Cur, void (*Func)( OBJ_NODE *) )
{
   while( Cur!=NULL )   
   {
      if ( Cur->Child!=NULL )
         Traverse_Nodes( Cur->Child, Func );
      (*Func)( Cur );
      Cur=Cur->Next;
   }
}

EXTERN void Traverse_Nodes_With_Transform( OBJ_NODE *Cur, 
   MATRIX2 Base_Matrix, void (*Func)( OBJ_NODE *, MATRIX2 ) )
{
   while( Cur!=NULL )
   {
      MATRIX2 M;
      Transform_To_Matrix( &Cur->Transform, M );
      if ( Base_Matrix != NULL )
         Mult_Matrix2_AB( Base_Matrix, M );
      (*Func)( Cur, M );         // first, process the parent
      if ( Cur->Child!=NULL )
         Traverse_Nodes_With_Transform( Cur->Child, M, Func );      
      Cur = Cur->Next;
   }
}
#endif

EXTERN void Traverse_Typed_Nodes( OBJ_NODE *Cur, OBJ_TYPE Type,
   void (*Func)( OBJ_NODE *) )
{
   for( ;Cur!=NULL; Cur=Cur->Next )   
   {
//      if ( Cur->Flags & OBJ_NOT_ACTIVE ) continue;
      if ( Cur->Flags & OBJ_DONT_RENDER ) continue;
      if ( Cur->Type==Type ) (*Func)( Cur );    // first, process the parent
      if ( Cur->Child!=NULL )
         Traverse_Typed_Nodes( Cur->Child, Type, Func );      
   }
}

/******************************************************************/

EXTERN OBJ_NODE *Search_Node_From_Type( OBJ_NODE *Cur, OBJ_TYPE Type )
{
   while( Cur!=NULL )   
   {
      if ( Cur->Type == Type ) return( Cur );
      if ( Cur->Child!=NULL ) 
      {
         OBJ_NODE *Ok;
         Ok = Search_Node_From_Type( Cur->Child, Type );
         if ( Ok != NULL ) return( Ok );
      }
      Cur=Cur->Next;
   }
   return( NULL );
}

EXTERN OBJ_NODE *Search_Node_From_ID( OBJ_NODE *Cur, UINT ID )
{
   while( Cur!=NULL )   
   {
      if ( Cur->ID == ID ) return( Cur );
      if ( Cur->Child!=NULL ) 
      {
         OBJ_NODE *Ok;
         Ok = Search_Node_From_ID( Cur->Child, ID );
         if ( Ok != NULL ) return( Ok );
      }
      Cur=Cur->Next;
   }
   return( NULL );
}

EXTERN OBJ_NODE *Search_Node_From_Name( OBJ_NODE *Cur, STRING Name )
{
   while( Cur!=NULL )
   {
      if ( Cur->Child!=NULL ) 
      {
         OBJ_NODE *Ok;
         Ok = Search_Node_From_Name( Cur->Child, Name );
         if ( Ok != NULL ) return( Ok );
      }
      if ( Cur->Name!=NULL )
         if ( !strcmp(Cur->Name, Name) ) return( Cur );
      Cur=Cur->Next;
   }
   return( NULL );
}

EXTERN OBJECT *Search_Object_From_ID( OBJECT *Cur, UINT ID )
{
   while( Cur!=NULL )   
   {
      if ( Cur->ID == ID ) return( Cur );
      Cur=Cur->Next;
   }
   return( NULL );
}

EXTERN OBJECT *Search_Object_From_Name( OBJECT *Cur, STRING Name )
{
   while( Cur!=NULL )
   {
      if ( Cur->Name!=NULL )
         if ( !strcmp(Cur->Name, Name) )
            return( Cur );
      Cur=Cur->Next;
   }
   return( NULL );
}

/******************************************************************/

#if 0

EXTERN INT Find_Max_Tree_Length( OBJ_NODE *List )
{
   OBJECT *Cur;
   INT Max, i;

   Max = -1;
   i = 0;
   for( Cur = List; Cur !=NULL; Cur = Cur->Next )
   {
      if ( Cur->Child!=NULL )
      {
         INT n;
         n = Find_Max_Tree_Length( Cur->Child );
         if ( n>Max ) Max = n;
      }
      i++;      
   }
   return( Max );
}

#endif
 
/******************************************************************/
