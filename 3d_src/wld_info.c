/************************************************
 *          World info printing                 *
 * Skal 98.                                     *
 ************************************************/

#include "main3d.h"

/******************************************************************/

static INT Tab;

static void Print_Object( OBJECT *Obj )
{
   switch( Obj->Type )
   {
      case LIGHT_TYPE:
         if ( ((LIGHT*)Obj)->Light_Type == LIGHT_SPOT )
            Out_Message( "%*s 0x%x  [LIGHT_SPOT] -%s- \tID=%d Tgt=%s", Tab, "", 
               Obj, Obj->Name, Obj->ID, ((LIGHT*)Obj)->Dummy==NULL?
               "[ok]" : ((LIGHT*)Obj)->Dummy->Name );
         else Out_Message( "%*s 0x%x  [LIGHT %d] -%s- \tID=%d Tgt=0x%x", Tab, "", 
               Obj, ((LIGHT*)Obj)->Light_Type, Obj->Name, Obj->ID, ((LIGHT*)Obj)->Dummy );
      break;
      case CAMERA_TYPE:
         Out_Message( "%*s 0x%x  [CAMERA] -%s- \tID=%d Tgt=%s", Tab, "", 
            Obj, Obj->Name, Obj->ID, ((CAMERA*)Obj)->Dummy==NULL?
            "[ok]" : ((CAMERA*)Obj)->Dummy->Name );
      break;
      case OBJ_DUMMY_TYPE:
         Out_Message( "%*s 0x%x  [DUMMY] -%s- \tID=%d", Tab, "", 
            Obj, Obj->Name, Obj->ID );
      break;
      case MESH_TYPE:
         Out_Message( "%*s 0x%x  [MESH] -%s- \tID=%d  %d polys", Tab, "", 
            Obj, Obj->Name, Obj->ID, ((MESH*)Obj)->Nb_Polys );
      break;
      case MATERIAL_TYPE:
         Out_Message( "%*s 0x%x  [MATERIAL] -%s- ID=%d Nb1=%d (0x%x)", Tab, "", 
            Obj, Obj->Name, Obj->ID,
            ((MATERIAL*)Obj)->Map_Nb1, ((MATERIAL*)Obj)->Txt1 );
      break;
      default:
         Out_Message( "%*s 0x%x  [Type=%d] -%s- \tID=%d", Tab, "", 
            Obj, Obj->Type, Obj->Name, Obj->ID );
      break;
   }
}

static void Print_Node( OBJ_NODE *Node )
{
   Out_Message( "%*s 0x%x  [%s] \tID=%d, Data=0x%x (%s) Parent:%s", 
      Tab, "", Node, Node->Name, Node->ID,
      Node->Data,
      Node->Data==NULL? "-" : 
         (Node->Data->Name)==NULL? "-" : Node->Data->Name,
      Node->Parent==NULL? "-" : 
         (Node->Parent->Name==NULL) ? "-" : Node->Parent->Name
   );
}

EXTERN void Traverse_Nodes_II( OBJ_NODE *Cur, void (*Func)( OBJ_NODE *) )
{
   while( Cur!=NULL )   
   {
      (*Func)( Cur );
      if ( Cur->Child!=NULL )
      {
         Tab++;
         Traverse_Nodes_II( Cur->Child, Func );
         Tab--;
      }      
      Cur=Cur->Next;
   }
}

EXTERN void Print_World_Info( WORLD *W )
{
   Tab = 1;
   Out_Message( " Objects: " );
   Traverse_Objects( W->Objects, Print_Object );
   Out_Message( "\n Lights: " );
   Traverse_Objects( W->Lights, Print_Object );
   Out_Message( "\n Cameras: " );
   Traverse_Objects( W->Cameras, Print_Object );
   Out_Message( "\n Material: " );
   Traverse_Objects( W->Materials, Print_Object );

   Tab = 0;
   Out_Message( "\n Tree: " );
   Traverse_Nodes_II( W->Root, Print_Node );
}

/******************************************************************/
