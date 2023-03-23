/***********************************************
 *      BSP tree1. Handling BSP as arrays.     *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/
    
#include "main3d.h"
#include "bsp.h"

EXTERN VECTOR BSP_Planes[3] = {
   { 1.0, 0.0, 0.0 },
   { 0.0, 1.0, 0.0 },
   { 0.0, 0.0, 1.0 }
};

EXTERN OBJ_NODE **BSP_Obj_To_Split = NULL;
EXTERN OBJ_NODE **BSP_New_Objs = NULL;
EXTERN INT BSP_Node_Start, BSP_Node_End, BSP_Max_Nodes;
EXTERN USHORT BSP_Box_Points_Flags[8] = 
{
      // points:
      //   7---4       z
      //  /|  /|       |
      // 3---0 |       .--y
      // | 6-|-5      /
      // |/  1/      x
      // 2---1
   0x0001, 0x0002, 0x0004, 0x0008,
   0x0010, 0x0020, 0x0040, 0x0080
}; 

EXTERN VECTOR BSP_Cur_Box_Pt[8];

static INT Left, Right, Cross;
static INT Left_Polys, Right_Polys, Cross_Polys;

/******************************************************************/

EXTERN USHORT BSP_Check_Box( OBJ_NODE *Node, BSP_NODE *Bsp_Node )
{
   USHORT Flag = 0x0000;
   INT i;
   BBOX Box;

   BBox_Transform_Min_Max( &Node->Box, Node->Mo );
   BBox_TPlanes_To_Points( BSP_Cur_Box_Pt, &Node->Box );
   for( i=0; i<8; ++i )
   {
      FLT Dot;
      Dot = Dot_Product( BSP_Cur_Box_Pt[i], Bsp_Node->Plane );
      if ( Dot<Bsp_Node->Plane[3] )
         Flag |= BSP_Box_Points_Flags[i];   // right side
   }   
   return( Flag );
}

EXTERN void BSP_Split_Node( BSP_TREE *Tree, INT Node )
{
   BSP_NODE *Cur;
   OBJ_NODE *Obj;
   INT i;

   Cur = &Tree->Nodes[Node];

   Left = 0; Right = 2*BSP_Max_Nodes-1;
   for( i=BSP_Node_Start; i<BSP_Node_End; ++i )
   {
      OBJ_NODE *Obj;
      USHORT Flag;

      Obj = BSP_Obj_To_Split[i];
      if ( Obj==NULL ) continue;

      Flag = BSP_Check_Box( Obj, Cur );
      if ( Flag == 0x0000 ) // full left  (Dot>=0.0)
         BSP_New_Objs[Left++] = Obj;
      else if ( Flag == 0x00ff ) // full right (Dot<0.0)
         BSP_New_Objs[Right--] = Obj;
      else  // crossing
      {
         OBJ_NODE *Obj_Left, *Obj_Right;
         BSP_New_Objs[Left++] = Obj_Left;
         BSP_New_Objs[Right--] = Obj_Right;         
      }
   }
      // rebuild objs array
}

/******************************************************************/

static FLT BSP_Evaluate_Split( BSP_NODE *Cur, INT Plane, FLT Alpha )
{
   INT i;
   FLT Value;

   Assign_Vector( Cur->Plane, BSP_Planes[Plane] );
   Cur->Plane[3] = (1.0-Alpha)*Cur->Mins[Plane] + Alpha*Cur->Maxs[Plane];
   if ( Cur->Plane[3]<0.0 )
   {
      Cur->Plane[0] = -Cur->Plane[0];
      Cur->Plane[1] = -Cur->Plane[1];
      Cur->Plane[2] = -Cur->Plane[2];
      Cur->Plane[3] = -Cur->Plane[3];
   }

   Left = 0; Right = 0; Cross = 0;
   Left_Polys = 0; Right_Polys = 0; Cross_Polys = 0;

   for( i=BSP_Node_Start; i<BSP_Node_End; ++i )
   {
      OBJ_NODE *Obj;
      USHORT Flag;

      Obj = BSP_Obj_To_Split[i];
      if ( Obj==NULL ) continue;

      Flag = BSP_Check_Box( Obj, Cur );
      if ( Flag == 0x0000 ) // full left  (Dot>=0.0)
      {
         Left++;
         if ( Obj->Type==MESH_TYPE )
            Left_Polys += ((MESH*)Obj->Data)->Nb_Polys;
      }
      else if ( Flag == 0x00ff ) // full right (Dot<0.0) 
      {
         Right++;
         if ( Obj->Type==MESH_TYPE )
            Right_Polys += ((MESH*)Obj->Data)->Nb_Polys;
      }
      else
      {
         Cross++;
         if ( Obj->Type==MESH_TYPE )
            Cross_Polys += ((MESH*)Obj->Data)->Nb_Polys;
      }
   }

   Value = fabs( Right_Polys-Left_Polys ) + Cross_Polys;

   printf( "Node ID %d -> Alpha=%f Left=%d Cross=%d Right=%d\n", Cur->ID, Alpha, Left, Cross, Right );
   printf( "    Left_Polys=%d Cross_Polys=%d Right_Polys=%d\n", Left_Polys, Cross_Polys, Right_Polys );
   printf( "    Alpha = %lf     Value = %lf\n", Alpha, Value );

   return( Value );
} 

EXTERN void BSP_Split_Node_Check( BSP_TREE *Tree, INT Node, INT Plane )
{
   BSP_NODE *Cur;
   OBJ_NODE *Obj;
   INT i, Try, Min;
   FLT Alpha, Alpha_Test;
   FLT Value, Value_Test;
   FLT Beta;

   Try = Bsp_Params.Max_Try;
   Bsp_Params.Test_Amp = Bsp_Params.Test_Amp_Base;
   Cur = &Tree->Nodes[Node];
   Min = BSP_Node_End-BSP_Node_Start;

         // starting plane in middle of box

   Alpha = 0.5;
   Value = BSP_Evaluate_Split( Cur, Plane, Alpha );
   Min = Value;
   Beta = Alpha;

Restart:

   Alpha_Test = Alpha + (random()&0xff)/256.0*Bsp_Params.Test_Amp;
   if ( Alpha_Test<0.0 ) Alpha_Test = 0.0;
   else if ( Alpha_Test>1.0 ) Alpha_Test = 1.0;

   Value_Test = BSP_Evaluate_Split( Cur, Plane, Alpha_Test );

   if ( Value_Test==Value )
   {
      Alpha = Alpha_Test; // keep on searching farther
      Bsp_Params.Test_Amp *= 2.0;
   }
   else if ( Value_Test<Value ) // keep this try
   {
      if ( Value_Test<Min )
      {
         Min = Value_Test;
         Beta = Alpha_Test;
      }
      Alpha = Alpha_Test;

   }
   Value = Value_Test;

   Try--;
   if ( Try==0 ) { Alpha = Beta; goto Restart; } // final choice
   else if ( Try==-1 ) goto End;
         
#if 0
   if ( Left_Polys>Right_Polys )
      Alpha = 0.5f * ( 1.0f+1.0f*Right_Polys/(Left_Polys+Right_Polys+Cross_Polys) );
//      Alpha -= Alpha/2.0;
   else if ( Right_Polys>Left_Polys )
      Alpha = 0.5f * ( 1.0f+1.0f*Left_Polys/(Left_Polys+Right_Polys+Cross_Polys) );
//      Alpha -= Alpha/2.0;
   else Alpha = 0.5f * ( 1.0f+1.0f*Cross_Polys/(Left_Polys+Right_Polys+Cross_Polys) );
#endif

   goto Restart;      

End:
   printf( "Node ID %d -> Alpha=%f Left=%d Cross=%d Right=%d\n", Cur->ID, Alpha, Left, Cross, Right );
   printf( "    Left_Polys=%d Cross_Polys=%d Right_Polys=%d\n", Left_Polys, Cross_Polys, Right_Polys );
//   BSP_Split_Node( Tree, Node );
}

/******************************************************************/

EXTERN INT Insert_In_Upper_BSP_Box( VECTOR UMins, VECTOR UMaxs,
   VECTOR Mins, VECTOR Maxs )
{
   INT Flag = 0;

   if ( UMaxs[0]<Maxs[0] ) { Flag++; UMaxs[0] = Maxs[0]; }
   if ( UMaxs[1]<Maxs[1] ) { Flag++; UMaxs[1] = Maxs[1]; }
   if ( UMaxs[2]<Maxs[2] ) { Flag++; UMaxs[2] = Maxs[2]; }
   if ( UMins[0]>Mins[0] ) { Flag++; UMins[0] = Mins[0]; }
   if ( UMins[1]>Mins[1] ) { Flag++; UMins[1] = Mins[1]; }
   if ( UMins[2]>Mins[2] ) { Flag++; UMins[2] = Mins[2]; }

   return( Flag );
}

EXTERN BSP_TREE *BSP_Setup_Tree( WORLD *W )
{
   BSP_TREE *New;
   BSP_NODE *Root_Node;
   INT n;
   BBOX R_Box;

   New = BSP_New_Tree( 0, 0 );
   if ( New==NULL ) return( NULL );
   Root_Node = BSP_New_Node( New );
   if ( Root_Node==NULL ) return( NULL );

   Set_Up_World( W );
   Init_Node_Ptr( W );
//   printf( "Max_Nodes=%d\n", W->Max_Nodes );

   Assign_Vector( R_Box.Mins, Root_Node->Mins );
   Assign_Vector( R_Box.Maxs, Root_Node->Maxs );

   _RCst_.Frame_Stamp++;
   for( n=0; n<W->Max_Nodes; ++n )
   {
      OBJ_NODE *Cur;
      BBOX Box;
      Cur = W->Node_Ptr[n];
      if ( Cur==NULL ) continue;
      Check_Node_Transform( Cur );
      BBox_Transform_Min_Max( &Cur->Box, Cur->Mo );
      Insert_In_Upper_BSP_Box( R_Box.Mins, R_Box.Maxs, 
         Cur->Box.TMins, Cur->Box.TMaxs );
      // R_Box.Radius = Set_BBox_From_Min_Max( R_Box, R_Box.Mins, R_Box.Maxs );
   }
   Assign_Vector( Root_Node->Mins, R_Box.Mins );
   Assign_Vector( Root_Node->Maxs ,R_Box.Maxs );
   _RCst_.Frame_Stamp = 0;

   BSP_Obj_To_Split = New_Fatal_Object( W->Max_Nodes, OBJ_NODE * );
   BSP_New_Objs = New_Fatal_Object( 2*W->Max_Nodes, OBJ_NODE * );
   memcpy( BSP_Obj_To_Split, W->Node_Ptr, W->Max_Nodes*sizeof(OBJ_NODE *) );   
   BSP_Node_Start = 0;
   BSP_Node_End = W->Max_Nodes;
   BSP_Max_Nodes = W->Max_Nodes;

   BSP_Split_Node_Check( New, 0, 0 );

   return( New );
}

/******************************************************************/
