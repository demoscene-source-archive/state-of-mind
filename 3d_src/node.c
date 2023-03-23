/***********************************************
 *                node.c                       *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

EXTERN INT Obj_Node_ID = 0xFFFE;

/******************************************************************/

EXTERN OBJ_NODE *New_Node( )
{
   OBJ_NODE *New;
   New = New_Object( 1, OBJ_NODE );
   if ( New==NULL ) return( NULL );
   Mem_Clear( New );

   New->Type    = VOID_TYPE;
   New->Prev    = NULL;
   New->Next    = NULL;
   New->Parent  = NULL;
   New->Child   = NULL;
   New->Top     = NULL;
   New->Flags   = OBJ_NO_FLAG;
   New->ID      = Obj_Node_ID--;          // <= won't conflict with 3ds assigned ID?
   New->Methods = NULL;
   New->Data    = NULL;
   New->Name    = NULL;

   Id_Transform( &New->Transform );
   New->Time_Stamp = 0;
   New->Cur_Time = -_HUGE_;

   return( New );
}

/******************************************************************/

EXTERN OBJ_NODE *Remove_Node( OBJ_NODE *Node )
{
   if ( Node->Prev!=NULL ) Node->Prev->Next = Node->Next;
   if ( Node->Next!=NULL ) Node->Next->Prev = Node->Prev;
   if ( Node->Parent!=NULL )
   {
      if ( Node->Parent->Child == Node )
      {
         if ( Node->Prev!=NULL ) Node->Parent->Child = Node->Prev;
         else Node->Parent->Child = Node->Next;
      }
   }
   if ( Node->Top!=NULL )
      if ( *Node->Top==Node)
      {
         if ( Node->Prev!=NULL ) *Node->Top = Node->Prev;
         else *Node->Top = Node->Next;
      }
   Node->Top = NULL;
   Node->Prev = Node->Next = NULL;
   Node->Parent = NULL;
   return( Node );
}

EXTERN void Remove_Object( OBJECT *Obj )
{
//   fprintf( stderr, " Obj=0x%x   Next=0x%x   Prev=0x%x\n", Obj, Obj->Next, Obj->Prev );
   if ( Obj->Next!=NULL ) Obj->Next->Prev = Obj->Prev;
   if ( Obj->Prev!=NULL ) Obj->Prev->Next = Obj->Next;
   if ( Obj->Top!=NULL )
      if ( *Obj->Top==Obj)
      {
         if ( Obj->Prev!=NULL ) *Obj->Top = Obj->Prev;
         else *Obj->Top = Obj->Next;
      }
   Obj->Top = NULL;
   Obj->Prev = Obj->Next = NULL;
}

EXTERN void Destroy_Object( OBJECT *Data, void (*Destroy)(OBJECT *) )
{
   if ( Data->Users<=1 )
   {
      Remove_Object( Data );
      if ( Data->Flags & OBJ_OWNS_NAME )
      {
         M_Free( Data->Name );
         Data->Flags = (OBJ_FLAG)( Data->Flags & ~OBJ_OWNS_NAME );
      }
      if ( Destroy!=NULL ) (*Destroy)( Data );
      M_Free( Data );
   }
   else Data->Users--;
}

EXTERN void Destroy_Objects( OBJECT *Cur, void (*Destroy)(OBJECT *) )
{
   while( Cur!=NULL )
   {
      OBJECT *Next;
      Next = Cur->Next;
      Cur->Users--;
//      if ( Cur->Users<=0 )              // <= TODO: Fix that!!
      {
         Remove_Object( Cur );
         if ( Cur->Flags & OBJ_OWNS_NAME )
         {
            M_Free( Cur->Name );
            Cur->Flags = (OBJ_FLAG)( Cur->Flags& ~OBJ_OWNS_NAME );
         }
         if ( Destroy!=NULL ) (*Destroy)( Cur );
         M_Free( Cur );
      }
      Cur = Next;
   }
}

EXTERN void Destroy_Node( OBJ_NODE *Node )
{
   if ( Node==NULL ) return;
   if ( Node->Data!=NULL )
      Destroy_Object( Node->Data, Node->Methods->Destroy );

   if ( Node->Flags & OBJ_OWNS_METHODS )
      M_Free( Node->Methods );

   Remove_Node( Node );
   if ( Node->Flags & OBJ_OWNS_NAME )
   {
      M_Free( Node->Name );
      Node->Flags &= ~OBJ_OWNS_NAME;
   }
   M_Free( Node );
}

EXTERN void Destroy_Nodes( OBJ_NODE *Cur )
{
   while( Cur!=NULL )
   {
      OBJ_NODE *Next;
      Next = Cur->Next;
      if ( Cur->Top!=NULL ) *(Cur->Top) = NULL;
      if ( Cur->Child!=NULL ) Destroy_Nodes( Cur->Child );
      Destroy_Node( Cur );
      Cur = Next;
   }
}

/******************************************************************/

EXTERN OBJECT *Insert_Object( OBJECT *Obj, OBJECT **Root )
{
   if ( Root==NULL ) return( NULL );

   Obj->Next = *Root;
   Obj->Prev = NULL;
   if ( (*Root)!=NULL ) (*Root)->Prev = Obj;
   Obj->Top = Root;
   (*Root) = Obj;
   return( *Root );
}

EXTERN OBJ_NODE *Insert_Node( OBJ_NODE *Node, OBJ_NODE **Head, OBJECT **Root )
{
   if ( (Root!=NULL) && (Node->Data!=NULL) )
      Insert_Object( Node->Data, Root );
   if ( Head==NULL )  return( NULL );   // error?!
   Node->Next = *Head;
   if ( (*Head)!=NULL )
   {
      Node->Parent = (*Head)->Parent;
      Node->Top = (*Head)->Top;
      (*Head)->Prev = Node;
   }
   else
   {
      Node->Top = Head;
      Node->Parent = NULL;    // <= TODO: this is wrong!!
   }
   *Head = Node;
   Node->Prev = NULL;
   return( Node );
}

EXTERN OBJ_NODE *Insert_Child_Node( OBJ_NODE *Node, OBJ_NODE *Parent, OBJECT **Root )
{
   if ( (Root!=NULL) && (Node->Data!=NULL) )
      Insert_Object( Node->Data, Root );
   if ( Parent==NULL )  return( NULL );   // error?!
   Node->Next = Parent->Child;
   if ( Parent->Child!=NULL )
   {
      Node->Top = Parent->Child->Top;
      Parent->Child->Prev = Node;
   }
   else Node->Top = &Parent->Child;
   Parent->Child = Node;
   Node->Prev = NULL;
   Node->Parent = Parent;
   return( Node );
}

EXTERN OBJ_NODE *Nodify( OBJECT *Data, NODE_METHODS *Methods )
{
   OBJ_NODE *New;

   if ( Data==NULL ) return( NULL );
   New = New_Node( );
   if ( New==NULL ) return( NULL );
   New->Methods = Methods;
   New->Data = Data;
   New->Type = Data->Type;
   return( New );
}

/******************************************************************/

EXTERN void Default_Obj_Fields( OBJECT *Obj )
{
   Obj->Flags = OBJ_NO_FLAG;
   Obj->Prev  = Obj->Next = NULL;
   Obj->Top   = NULL;
   Obj->Name  = NULL;
   Obj->Users = 0;
   Obj->ID    = 0xFFFF;  /* => none */
   Obj->Type  = VOID_TYPE;
}

EXTERN OBJECT *New_Node_Data( NODE_METHODS *Methods )
{
   OBJECT *Data; 
   Data = (OBJECT *)New_Object( Methods->Data_Size, PIXEL );
   if ( Data==NULL ) return( NULL );
   memset( (void *)Data, 0, Methods->Data_Size*sizeof( PIXEL ) );
   Default_Obj_Fields( Data );
   if ( Methods->Init!=NULL ) (*Methods->Init)( Data );
   return( Data );
}

EXTERN OBJ_NODE *Instantiate( NODE_METHODS *Methods )
{
   OBJ_NODE *New;

   New = New_Node( );
   if ( New==NULL ) return( NULL );
   New->Methods = Methods;
   if ( Methods==NULL ) return( New );
   New->Data = New_Node_Data( Methods );
   if ( New->Data==NULL ) { M_Free( New ); return( NULL ); }
   New->Type = New->Data->Type;
   return( New );
}

EXTERN OBJ_NODE *Attach_Node_Data( OBJ_NODE *Node, OBJECT *Data, NODE_METHODS *Methods )
{
   if ( Node==NULL ) return( NULL );
   Node->Methods = Methods;
   Node->Data = Data;
   Node->Type = Node->Data->Type;
   return( Node );
}

EXTERN OBJ_NODE *Duplicate_Node( OBJ_NODE *Node )
{
   OBJ_NODE *New;
   INT Id;

   New = New_Node( );
   if ( New==NULL ) return( NULL );
   Id = New->ID;
   *New = *Node;
   New->ID = Id;
   if ( Node->Methods==NULL ) return( New );
   New->Methods = Node->Methods;
   New->Flags &= OBJ_OWNS_METHODS | OBJ_OWNS_NAME;
   if ( New->Data!=NULL ) New->Data->Users++;
   return( New );
}

EXTERN OBJ_NODE *Copy_Node( OBJ_NODE *Node )
{
   OBJ_NODE *New;
   if ( Node==NULL ) return( NULL );
   if ( Node->Data==NULL )
   {
      New = Duplicate_Node( Node );
      return( New );
   }
   else 
   {
      New = Instantiate( Node->Methods );
      New->Data = NODE_COPY(New, Node);
      return( New );
   }
}

/******************************************************************/
