/***********************************************
 *          octree manipulation                *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "ray.h"

EXTERN OCTREE *Cur_Octree = NULL;

/******************************************************************/
/******************************************************************/

EXTERN void Octree_Check_Parent( OCTREE_ID *Father, OCTREE_ID *Child )
{
      // set father's ID info according to the (new?) *Child
   Father->Size = Child->Size + 1;
   Father->x = (Child->x > 0) ? (Child->x >> 1) : (Child->x - 1) / 2;
   Father->y = (Child->y > 0) ? (Child->y >> 1) : (Child->y - 1) / 2;
   Father->z = (Child->z > 0) ? (Child->z >> 1) : (Child->z - 1) / 2;        
}

EXTERN void Octree_New_Root( OCTREE_NODE **Root )
{
   OCTREE_NODE *New;
   INT Index;

   // grow the root node twice

   New = Octree_New_Node( Cur_Octree );
   if ( New==NULL ) return;      // ?! error

   Octree_Check_Parent( &New->Id, &( (*Root)->Id ) );

      // find where to put the previous *Root in New node's children
   Index  = ( (*Root)->Id.x & 0x01 ) * 4;
   Index += ( (*Root)->Id.y & 0x01 ) * 2;
   Index += ( (*Root)->Id.z & 0x01 );
   (*Root)->Children[Index] = *Root;
   *Root = New;
}

EXTERN void Octree_Insert( OCTREE_NODE **Root, OCTREE_DUMMY_DATA *Data,
   OCTREE_ID *New_Id )
{
   OCTREE_ID Tmp_Id;
   OCTREE_NODE *Cur_Node;

      // New_Id must be properly scaled before getting here

   if ( *Root==NULL )
   {
      *Root = Octree_New_Node( Cur_Octree );
      (*Root)->Id = *New_Id;
   }

   while ( (*Root)->Id.Size<New_Id->Size )
      Octree_New_Root( Root );      //  Add new top

   Tmp_Id = *New_Id;
   while( Tmp_Id.Size<(*Root)->Id.Size )
      Octree_Check_Parent( &Tmp_Id, &Tmp_Id );

   while( ( Tmp_Id.x != (*Root)->Id.x )  ||
          ( Tmp_Id.y != (*Root)->Id.y )  ||
          ( Tmp_Id.z != (*Root)->Id.z ) )
   {
      Octree_New_Root( Root );
      Octree_Check_Parent( &Tmp_Id, &Tmp_Id );
   }

   Cur_Node = *Root;
   while( Cur_Node->Id.Size>New_Id->Size )
   {
      INT Final_Size, Index;

      Final_Size = Cur_Node->Id.Size - 1;
      Tmp_Id = *New_Id;
      while( Tmp_Id.Size<Final_Size )
         Octree_Check_Parent( &Tmp_Id, &Tmp_Id );
      Index  = ( Tmp_Id.x & 0x01 ) * 4;
      Index += ( Tmp_Id.y & 0x01 ) * 2;
      Index += ( Tmp_Id.z & 0x01 );
      if ( Cur_Node->Children[Index] == NULL )
      {
         Cur_Node->Children[Index] = Octree_New_Node( Cur_Octree );  // may fail
         Cur_Node->Children[Index]->Id = Tmp_Id;
      }
      Cur_Node = Cur_Node->Children[Index];
   }

      // Insert_Data
   Data->Next = Cur_Node->Data;
   Cur_Node->Data = Data;
}

/******************************************************************/

#define OCTREE_BIAS 10000000.0

EXTERN INT Octree_Point_In_Node( VECTOR Pt, OCTREE_ID *Id )
{
         // Warning: IEEE-format tricks!

   FLT Sized, Min, Test;
   union { float f; INT l; } Size;

   Size.l = Id->Size<<23;
   Sized = (FLT)( Size.f );

   Min = (FLT)( Id->x * Sized - OCTREE_BIAS );
   Test = Min - Sized*.5f; 
   if ( Pt[0]<Test ) return( FALSE );
   Test = Min + Sized*1.5f;
   if ( Pt[0]>=Test ) return( FALSE );

   Min = (FLT)( Id->y * Sized - OCTREE_BIAS );
   Test = Min - Sized*.5f; 
   if ( Pt[1]<Test ) return( FALSE );
   Test = Min + Sized*1.5f;
   if ( Pt[1]>=Test ) return( FALSE );

   Min = (FLT)( Id->z * Sized - OCTREE_BIAS );
   Test = Min - Sized*.5f; 
   if ( Pt[2]<Test ) return( FALSE );
   Test = Min + Sized*1.5f;
   if ( Pt[2]>=Test ) return( FALSE );

   return( TRUE );
}

/******************************************************************/

#define MAX(a,b)  ( (a)>(b)?(a):(b) )
#define MAX3(a,b,c) ( ((a)>(b)) ? MAX((a),(c)) : MAX((b),(c)) )

EXTERN void Octree_Find_Box( VECTOR Min, VECTOR Max, OCTREE_ID *Id )
{
      // Warning: Uses IEEE format trick when clearing float mantissa!!
   float dx, dy, dz, Max_Size;     // <= do NOT use FLT here
   union { float f; INT l; } convert;
   FLT BSized, Inv;
   OCTREE_ID Base_Id, Test_Id;

   dx = (float)( Max[0]-Min[0] );
   dy = (float)( Max[1]-Min[1] );
   dz = (float)( Max[2]-Min[2] );
   Max_Size = MAX3( dx, dy, dz );
   convert.f = Max_Size;
   convert.l &= 0xff800000;      // <= clear mantissa
   BSized = (FLT)convert.f;

     // find smallest possible node to stash stuff in

   Inv = 1.0f/BSized;
   Base_Id.x = (INT)floor( (Min[0] + OCTREE_BIAS ) * Inv );
   Base_Id.y = (INT)floor( (Min[1] + OCTREE_BIAS ) * Inv );
   Base_Id.z = (INT)floor( (Min[2] + OCTREE_BIAS ) * Inv );

   convert.f = (float)BSized;
   Base_Id.Size = ( convert.l & 0x7f800000) >> 23;

   while ( 1 )
   {
      INT idx;
      Test_Id.Size = Base_Id. Size;
      for( idx=0; idx<2; ++idx )
      {
         INT idy;
         Test_Id.x = Base_Id.x + idx;
         for( idy=0; idy<2; ++idy )
         {
            INT idz;
            Test_Id.y = Base_Id.y + idy;
            for( idz=0; idz<2; ++idz )
            {
               Test_Id.z = Base_Id.z + idz;
               if ( Octree_Point_In_Node( Min, &Test_Id ) &&
                    Octree_Point_In_Node( Max, &Test_Id ) )
                  goto Found;
            }
         }
      }
      Octree_Check_Parent( &Base_Id, &Base_Id );
   }
Found:
   *Id = Test_Id;
}

EXTERN void Octree_Find_Sphere( VECTOR C, FLT R, OCTREE_ID *Id )
{
   VECTOR Min, Max;
   Set_Vector( Min, C[0]-R, C[1]-R, C[2]-R );
   Set_Vector( Max, C[0]+R, C[1]+R, C[2]+R );
   Octree_Find_Box( Min, Max, Id );
}

/******************************************************************/

   // call (*Func) for each sub nodes' data.
   // returns: 0 to stop, 1 to keep going

EXTERN INT Octree_Traverse( OCTREE_NODE *Node,
   INT (*Func)( OCTREE_DUMMY_DATA *, void *Handle1 ),
   void *Handle )
{
   INT i, Keep_On;
   OCTREE_DUMMY_DATA *Cur_Data;

   Keep_On = TRUE;
   for( i=0; i<8; ++i )
   {
      OCTREE_NODE *Cur_Node;
      Cur_Node = Node->Children[i];
      if ( Cur_Node!=NULL )
      {
         Keep_On = Octree_Traverse( Cur_Node, Func, Handle );
         if ( Keep_On==FALSE ) break;
      }
   }
   Cur_Data = Node->Data;
   while( Cur_Data!=NULL )
   {
      Keep_On = (*Func)( Cur_Data, Handle );
      if ( Keep_On==FALSE ) break;
      Cur_Data=Cur_Data->Next;
   }
   return( Keep_On );
}

/******************************************************************
 *                  Object's maintainance                         *
 ******************************************************************/


EXTERN OCTREE_NODE *Octree_New_Node( OCTREE *Octree )
{
   OCTREE_NODE *New;
   New = New_Fatal_Object( 1, OCTREE_NODE );
   if ( New==NULL ) return( NULL );
   Mem_Clear( New );
   Octree->Nb_Nodes++;
   return( New );
}

EXTERN OCTREE *Create_Base_Octree( INT Data_Size )
{
   OCTREE *New;
   New = New_Fatal_Object( 1, OCTREE );
   if ( New==NULL ) return( NULL );
   New->Root = NULL;
   Set_Vector( New->Center, 0.0, 0.0, 0.0 );
   Set_Vector( New->Scale, 1.0, 1.0, 1.0 );
   New->Last_Hit = NULL;
   New->Nb_Nodes = 0;
   New->Nb_Data = 0;
   New->Data_Size = Data_Size;
   New->Init_Data_Block = NULL;
   New->Destroy_Data_Block = NULL;
   return( New );
}

EXTERN void Destroy_Octree_Node( OCTREE *Octree, OCTREE_NODE **Base )
{
   INT i;
   OCTREE_DUMMY_DATA *Data;

   if ( Base==NULL || (*Base)==NULL ) return;
   for( i=0; i<8; ++i )
   {
      if ( (*Base)->Children[i]!=NULL )
         Destroy_Octree_Node( Octree, &(*Base)->Children[i] );
   }   

   Data = (OCTREE_DUMMY_DATA *)(*Base)->Data;
   while( Data!=NULL )
   {
      OCTREE_DUMMY_DATA *Next;
      Next = Data->Next;
      if ( Octree->Destroy_Data_Block!=NULL )
         (*Octree->Destroy_Data_Block)( (void*)Data );
      M_Free( Data );
      Cur_Octree->Nb_Data--;
      Data=Next;
   }
   M_Free( *Base );     // *Base=NULL;
   Cur_Octree->Nb_Nodes--;
}

EXTERN void Destroy_Octree( OCTREE *Octree )
{
   if ( Octree==NULL ) return;
   Destroy_Octree_Node( Octree, &Octree->Root );
   M_Free( Octree );
}


/******************************************************************
 ******************************************************************/

EXTERN INT Octree_Distance_Traverse( OCTREE_NODE *Node,
   VECTOR Pt, INT Bounce,
   INT (*Func)(  OCTREE_DATA *, void *Handle1 ),
   void *Handle )
{
   INT i, Keep_On;
   OCTREE_DATA *Cur_Data;

   Keep_On = TRUE;
   for( i=0; i<8; ++i )
   {
      OCTREE_NODE *Cur_Node;
      Cur_Node = Node->Children[i];
      if ( Cur_Node!=NULL )
      {
         if ( Octree_Point_In_Node( Pt, &Cur_Node->Id ) )
         {
            Keep_On = Octree_Distance_Traverse( 
               Cur_Node, Pt, Bounce, Func, Handle );
            if ( Keep_On==FALSE ) break;
         }
      }
   }

   Cur_Data = (OCTREE_DATA *)Node->Data;
   while( Cur_Data!=NULL )
   {
      if ( Cur_Data->Bounce_Level == Bounce )
      {
         Keep_On = (*Func)( Cur_Data, Handle );
         if ( Keep_On==FALSE ) break;
      }
      Cur_Data = Cur_Data->Next;
   }
   return( Keep_On );
}

static void *Init_Radiosity_Block( )
{
   OCTREE_DATA *New;
   New = New_Fatal_Object( 1, OCTREE_DATA );
   if ( New==NULL ) return( NULL );
   Mem_Clear( New );    // <= will zero the gradients
   SET_FCOLOR( New->Illuminance, 0.0, 0.0, 0.0, 1.0 );
   New->Bounce_Level = 0;
   New->Next = NULL;
   return( (void*)New );
}

static void Destroy_Radiosity_Block( void *Block )
{
   // nothing special to do
}

/******************************************************************/

EXTERN OCTREE *Create_Radiosity_Octree( )
{
   OCTREE *New;
   New = Create_Base_Octree( sizeof( OCTREE_DATA ) );
   if ( New==NULL ) return( NULL );
   New->Init_Data_Block = Init_Radiosity_Block;
   New->Destroy_Data_Block = Destroy_Radiosity_Block;
   return( New );
}

/******************************************************************/
/******************************************************************/

#if 0
EXTERN void Print_Octree_Node( OCTREE_NODE *Node, INT Level )
{
   OCTREE_DATA *Data;
   INT i;
   if ( Node==NULL ) return;
   fprintf( stderr, "%*s Node 0x%x: Id=(%d,%d,%d / %d)\n",
      Level*2, "",
      Node, Node->Id.x, Node->Id.y, Node->Id.z, Node->Id.Size );
   Data=(OCTREE_DATA*)Node->Data;
   if ( Data!=NULL ) Level++;
   for ( Data=(OCTREE_DATA*)Node->Data; Data!=NULL; Data=Data->Next )
   {
      fprintf( stderr, "%*s   Data 0x%x:  l=%d  Mean_D=%.3f Near_D=%.3f Col=%.2f,%.2f,%.2f\n", 
         Level*2, "",
        Data, Data->Bounce_Level, Data->Harmonic_Mean_Distance,
        Data->Nearest_Distance,
        Data->Illuminance[0],Data->Illuminance[1],Data->Illuminance[2] );
   }
   for( i=0; i<8; ++i )
      if ( Node->Children[i]!=NULL )
         Print_Octree_Node( Node->Children[i], Level );
   fprintf( stderr, "---------------------------------\n" );
   return;
}
#endif

/******************************************************************/
