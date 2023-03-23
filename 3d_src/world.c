/***********************************************
 *              world.c                        *
 * Skal 96                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/
/******************************************************************/

EXTERN UINT    Nb_Obj_To_Sort = 0;
EXTERN INT     Nb_Sorted = 0;
EXTERN INT     Nb_Poly_Sorted = 0;
EXTERN INT     Total_Poly_Sorted = 0;

/******************************************************************/
/******************************************************************/

   // Pointer to current objects being processed

EXTERN MESH           *Cur_Msh = NULL;
EXTERN SHADER_METHODS *Cur_Shader = NULL;
EXTERN OBJECT         *Cur_Obj = NULL;
EXTERN OBJ_NODE       *Cur_Node = NULL;
EXTERN LIGHT          *Cur_Light = NULL;
EXTERN CAMERA         *Cur_Camera = NULL;
EXTERN VISTA          *Cur_Vista = NULL;

/******************************************************************/
/******************************************************************/

EXTERN WORLD *Create_World( ) // <= God knows I like this function's name :) 
{
   WORLD *W;

   W = New_Fatal_Object( 1, WORLD );
   Mem_Clear( W );

   W->Cameras   = NULL;
   W->Lights    = NULL;
   W->Planes    = NULL;
   W->Materials = NULL;
   W->Objects   = NULL;

   W->Root = NULL;   // New_Group_Object( );

   W->Time = W->Frames_Start = 0.0;
   W->Frames_End = 1.0;
   W->Cache = NULL;

   W->State = 0x00;
   W->Octree = NULL;   

   return( W );
}

/******************************************************************/

EXTERN void Clear_World( WORLD *W )
{
   if ( W==NULL ) return;
   Destroy_Nodes( W->Root );
   W->Cameras   = NULL;
   W->Lights    = NULL;
   W->Planes    = NULL;
   W->Objects   = NULL;

   M_Free( W->Node_Ptr ); 
   W->Node_Ptr = 0;

   Destroy_Objects( (OBJECT *)W->Materials, _MATERIAL_.Destroy );
   W->Materials = NULL;

   if ( Cache_Methods.Clear_Cache!=NULL )
      (*Cache_Methods.Clear_Cache)( W->Cache );
   M_Free( W->Cache );
   W->State = 0x00;
}

EXTERN void Destroy_World( WORLD *W )     // <= And this one even more!! :))
{
   Clear_World( W );
   if ( W==_RCst_.The_World ) World_Clean_Resources( );
   M_Free( W );
}

/******************************************************************/

EXTERN OBJ_NODE *New_Object_By_Type( STRING Name, NODE_METHODS *Methods, WORLD *W )
{
   OBJ_NODE *Cur_Node;
   OBJECT **Where;

   Cur_Node = Instantiate( Methods );
   if ( Cur_Node==NULL ) return( NULL );
   Cur_Node->Name = Name;
   Cur_Node->Flags |= OBJ_OWNS_NAME;

   switch( Cur_Node->Type )
   {
      default: case MESH_TYPE: case GROUP_TYPE: case OBJ_DUMMY_TYPE:
         Where = &W->Objects;
      break;
      case CAMERA_TYPE: Where = &W->Cameras; break;
      case LIGHT_TYPE: Where = &W->Lights; break;
      case MATERIAL_TYPE: Where = &W->Materials; break;
   }
   Insert_Node( Cur_Node, &W->Root, Where );
   return( Cur_Node );
}

/******************************************************************/

   // compute resources needed...

EXTERN void Set_Up_World( WORLD *W )   
{
   W->State = 0x00;

      // Will set IDs, too. In mesh.c
      // and will set Nb_Edges!

//   Cur_Poly_ID = 0x0000; useless...
#ifdef USE_OLD_SHADOWZ
   Vertex_ID = 0x0000;
#endif

   Finish_Objects( W->Root );

   W->Max_Obj = 0; W->Max_Polys = 0; W->Max_Edges = 0;

   Count_Nodes( W->Root, &W->Max_Obj, &W->Max_Polys, &W->Max_Edges );

   W->Max_Vertex = Find_Max_Vertex( W->Objects );

   if ( W->Max_Edges<W->Max_Polys )    // some edges are missing...
      W->Max_Edges = W->Max_Polys + W->Max_Vertex + 10; // !! Genus?

   W->State = 0x01;

      // Init material
   if ( W->Cache!=NULL && Cache_Methods.Setup_Material!=NULL )
   {
      OBJECT *Cur;
      Cur = W->Materials;
      while( Cur!=NULL )   
      {
         Cache_Methods.Setup_Material( W->Cache, Cur );
         Cur=Cur->Next;
      }
   }

         // Materials are separated objects. Finish'em!
   Finish_All_Data( W->Materials, Finish_Material_Data );

//   DEBUG( Out_Message( "Removed %d unused material(s)", j ) );
   DEBUG( Out_Message( "%d object(s) in world", W->Max_Obj ) );
   DEBUG( Out_Message( "Max %d vertex", W->Max_Vertex ) );
   DEBUG( Out_Message( "Max %d edges", W->Max_Edges ) );
   DEBUG( Out_Message( "%d poly(s) in world", W->Max_Polys ) );
   DEBUG( Out_Message( "%d camera(s).", Count_Objects(W->Cameras) ) );
   DEBUG( Out_Message( "%d light(s).", Count_Objects(W->Lights) ) );
}

/******************************************************************/
/******************************************************************/

static INT ASM_OK = FALSE;

#if defined( LNX ) || defined( UNIX )

#include <signal.h>

static INT SIGFPE_Count = -1;

static void SIGFPE_Catch( int Signal )
{
   SIGFPE_Count++;
   // fprintf( stderr, "Number of FPE: %d\n", SIGFPE_Count );
}

#endif

/******************************************************************/

EXTERN void Finish_World_Init( WORLD *W )
{
   Nb_Sorted = Nb_Obj_To_Sort = 0;
   _RCst_.The_World   = W;

   if ( !ASM_OK ) 
   {
#ifndef UNIX
   Enter_Engine_Asm( ); 
#endif
      ASM_OK = TRUE;
   }
#if defined( LNX ) || defined( UNIX )
   if ( SIGFPE_Count==-1 ) signal( SIGFPE, SIGFPE_Catch );
#endif
}

static void Find_Max_ID( WORLD *W, OBJ_NODE *Cur )
{
   while( Cur!=NULL )   
   {
      if ( Cur->Child!=NULL ) Find_Max_ID( W, Cur->Child );
      if ( Cur->ID > W->Max_Nodes ) W->Max_Nodes = Cur->ID;
      if ( W->Node_Ptr!=NULL ) W->Node_Ptr[Cur->ID] = Cur;
      Cur=Cur->Next;
   }
}

EXTERN OBJ_NODE **Init_Node_Ptr( WORLD *W )
{
   if ( W==NULL ) return( NULL );

   M_Free( W->Node_Ptr );
   W->Max_Nodes = 0;
   Find_Max_ID( W, W->Root );
   W->Max_Nodes++;
   W->Node_Ptr = New_Fatal_Object( W->Max_Nodes, OBJ_NODE * );
   Find_Max_ID( W, W->Root );

   return( W->Node_Ptr );
}

EXTERN void Select_World( WORLD *W )
{
   WORLD_REAL_RSC Rsc;

   if ( W->State == 0x00 ) Set_Up_World( W );

   Mem_Clear( &Rsc );
   Rsc.Poly_Keys_Sz = W->Max_Edges;
   Rsc.Poly_To_Sort_Sz = W->Max_Edges;
   Rsc.P_Vertex_Sz = W->Max_Vertex;
   Rsc.Vertex_State_Sz = W->Max_Vertex;
   Rsc.Sorted_Sz = W->Max_Obj;
   Rsc.Obj_To_Sort_Sz = W->Max_Obj;

   if ( World_Check_Resources( &Rsc ) == NULL )
   { 
      _RCst_.The_World   = NULL;
      return;
   }

   M_Free( W->Node_Ptr );
   Finish_World_Init( W );
}

#if 0       // unused
EXTERN void Un_Init_Renderer( )
{
   if ( SIGFPE_Count>=0 )
   {
      fprintf( stderr, "Number of FPE: %d\n", SIGFPE_Count );
      SIGFPE_Count = 0;
   }
}
#endif

/******************************************************************/
/******************************************************************/
