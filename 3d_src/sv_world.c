/***********************************************
 *              Dump world                     *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"
#include "indy.h"

/******************************************************************/

static INT N_Count = 0;

static WORLD *Da_World = NULL;
static INT T_Count = 0;
static FILE *Out;

static INT Write_Chunk( USHORT ID, void *Obj, INT Nb );
static INT Write_Mesh( void *M, INT );
static INT Write_Light( void *L, INT );
static INT Write_Spot_Light( void *L, INT );
static INT Write_Camera( void *Cam, INT );

static void Write_FaceMat( MESH *M );
static MESH *Da_Mesh = NULL;
static CHUNK_TYPE Current_Data;
// static SHORT Current_ID;

static OBJ_NODE *Cur_Node;
static OBJECT *Cur_Obj;
static MESH *Cur_Mesh;

EXTERN INT Dump_Objects_List( void *Dum );

static INT Emit_Byte( PIXEL d );
static INT Emit_Short( USHORT d );
static INT Emit_Int( INT d );
static INT Emit_Flt( FLT x );
static INT Emit_Vector( FLT *x, INT Len );
static INT Write_String_3ds( STRING Name );
static INT Emit_Sp_Flags( SPLINE_3DS *Sp );
static INT Emit_Sp_Key_Flags( A_KEY *K );

#define EMIT_TRACK(Sp,T) { if ( ((Sp)!=NULL) && (Sp)->Nb_Keys>0 ) \
   if ( Write_Chunk( (T), (void*)(Sp), 0 ) ) return(IO_ERROR); }

/******************************************************************/
/*                            Level 0                             */
/******************************************************************/

static INT Main_Wrt( void *Dum, INT Nb )
{
   WORLD *W;

   W = (WORLD *)Dum;
   if ( Write_Chunk( CHK_OBJMESH, (void *)W, 0 ) ) return( IO_ERROR );
   if ( Write_Chunk( CHK_KEYFRAMER, (void *)W, 0 ) ) return( IO_ERROR );
//   if ( Dump_Objects_List( (void *)W->Root ) ) return( IO_ERROR );
   return( SKY_IS_BLUE );
}   

/******************************************************************/
/*                            Level I                             */
/******************************************************************/

static void Obj_Dump_Func( OBJ_NODE *Node )
{
   fprintf( stderr, "\ndumping obj '%s'\n", Node->Name );
   if ( Node->Type!=GROUP_TYPE )
      Write_Chunk( CHK_OBJBLOCK, (void *)Node, 0 );
}

static INT Editor_3D_Wrt( void *Dum, INT Nb )
{
   OBJECT *Cur;
   WORLD *W;

   W = (WORLD *)Dum;

         // First, the materials. In reverse order...
         // Then, the objects (which includes Lights...)

   Cur = (OBJECT *)W->Materials;
   if ( Cur==NULL ) goto No_Material;

   while( Cur->Next != NULL ) Cur = Cur->Next;
   while( Cur!=NULL )
   {
      if ( Write_Chunk( CHK_MATERIAL, (void *)Cur, 0 ) )
         return( IO_ERROR );
      Cur = Cur->Prev;
   }

No_Material:

   Traverse_All_Nodes( W->Root, Obj_Dump_Func );

   return( SKY_IS_BLUE );
}

/******************************************************************/

static void Keyframer_Func( OBJ_NODE *Node )
{
   switch( Node->Type )
   {
      case MESH_TYPE:
         Write_Chunk( CHK_TRACKMESH, (void *)Node, 0 );
      break;
      case CAMERA_TYPE:
         Write_Chunk( CHK_TRACKCAMERA, (void *)Node, 0 );
         Write_Chunk( CHK_TRACKCAMTGT, (void *)Node, 0 );
      break;
      case LIGHT_TYPE:
         Write_Chunk( CHK_TRACKLIGHT, (void *)Node, 0 );
         if ( ((LIGHT*)Node->Data)->Light_Type == LIGHT_SPOT )
            Write_Chunk( CHK_TRACKLIGTGT, (void *)Node, 0 );
      break;
      default: break;
   }
}

static INT Keyframer_Wrt( void *Dum, INT Nb )
{
   WORLD *W;

   W = (WORLD *)Dum;

   if ( Write_Chunk( CHK_FRAMES, (void *)W, 0 ) )
      return( IO_ERROR );

   Traverse_All_Nodes( W->Root, Keyframer_Func );
   return( SKY_IS_BLUE );
}

/******************************************************************/
/*                            Level II                            */
/******************************************************************/

static INT Obj_Block_Wrt( void *Dum, INT Nb )
{
   OBJ_NODE *Node;
   OBJECT *Obj;

   Cur_Node = Node = (OBJ_NODE *)Dum;
   Cur_Obj = Obj = (OBJECT *)Node->Data;

   if ( Write_String_3ds( Node->Name ) )
      return( MEM_ERROR );

   switch( Node->Type )
   {
      case MESH_TYPE:
         if ( Write_Chunk( CHK_TRIMESH, Node, 0 ) ) return( IO_ERROR );
      break;
      case LIGHT_TYPE:
         if ( ((LIGHT*)Node)->Light_Type == LIGHT_SPOT )
         {
            if ( Write_Chunk( CHK_SPOTLIGHT, Node, 0 ) ) return( IO_ERROR );
         }
         else if ( Write_Chunk( CHK_LIGHT, Node, 0 ) ) return( IO_ERROR );
         if ( Write_Chunk( CHK_LGHTZMIN, Node, 0 ) ) return( IO_ERROR );
         if ( Write_Chunk( CHK_LGHTZMAX, Node, 0 ) ) return( IO_ERROR );
         if ( Write_Chunk( CHK_LGHTMULT, Node, 0 ) ) return( IO_ERROR );
         if ( Node->Flags & OBJ_DONT_RENDER )
            if ( Write_Chunk( CHK_LIGHT_OFF, Node, 0 ) ) return( IO_ERROR );
      break;
      case CAMERA_TYPE:
         if ( Write_Chunk( CHK_CAMERA, Node, 0 ) ) return( IO_ERROR );
      break;

      default: case GROUP_TYPE:   // Shouldn't happen
         Exit_Upon_Error( "Nasty bug." );
      break;
   }

   return( SKY_IS_BLUE );
}

static INT Material_Wrt( void *Dum, INT Nb )
{
   MATERIAL *Mat;
   Mat = (MATERIAL *)Dum;

   if ( Write_Chunk( CHK_MATNAME, (void *)Mat, 0 ) ) return( IO_ERROR );
   if ( Mat->Txt1!=NULL )
      if ( Write_Chunk( CHK_MAPFILE, (void *)Mat, 0 ) ) return( IO_ERROR );
//   if ( Write_Chunk( CHK_KEYFRAMER, (void *)W, 0 ) ) return( IO_ERROR );   
   return( SKY_IS_BLUE );
}

static INT Map_Wrt( void *Dum, INT Nb )
{
   MATERIAL *Mat;
   Mat = (MATERIAL *)Dum;
   if ( Write_String_3ds( (STRING)Mat->Txt1 ) ) return( MEM_ERROR );
   return( SKY_IS_BLUE );
}

/******************************************************************/

static INT Light_ZMin_Wrt( void *Dum, INT Nb )
{
   Emit_Flt( ((LIGHT*)Cur_Obj)->Z_Min );
   return( SKY_IS_BLUE );
}
static INT Light_ZMax_Wrt( void *Dum, INT Nb )
{
   Emit_Flt( ((LIGHT*)Cur_Obj)->Z_Max );
   return( SKY_IS_BLUE );
}
static INT Light_Mult_Wrt( void *Dum, INT Nb )
{
   Emit_Flt( ((LIGHT*)Cur_Obj)->Mult );
   return( SKY_IS_BLUE );
}
static INT Light_Off_Wrt( void *Dum, INT Nb )
{
   return( SKY_IS_BLUE );
}

/******************************************************************/

static INT Frames_Wrt( void *Dum, INT Nb )
{
   WORLD *W;

   W = (WORLD *)Dum;
   Emit_Int( (INT)W->Frames_Start );
   Emit_Int( (INT)W->Frames_End );
   return( SKY_IS_BLUE );
}

static INT ID_Wrt( void *Dum, INT Nb )
{
   OBJ_NODE *Node;

   Node = (OBJ_NODE *)Dum;
   Emit_Short( (USHORT)( Node->ID ) );
   return( SKY_IS_BLUE );
}

static INT HDR_Wrt( void *Dum, INT Nb )
{
   OBJ_NODE *Node;
   USHORT W[2];

   Node = (OBJ_NODE *)Dum;

   W[0] = 0x4000; W[1] = 0x0000;
   if ( Write_String_3ds( Node->Name ) ) return( MEM_ERROR );

   if ( Node->Flags & OBJ_NOT_ACTIVE ) W[0] |= 0x0800;   // Hidden obj
   Emit_Short( W[0] );   // Flags
   Emit_Short( W[1] );

   if ( Node->Parent==NULL )     // Parent ?
      Emit_Short( 0xFFFF );   // (none)
   else Emit_Short( (USHORT)( Node->Parent->ID ) ); 

   return( SKY_IS_BLUE );
}

/******************************************************************/
 
static INT Track_Mesh_Wrt( void *Dum, INT Nb )
{
   OBJ_NODE *Node;
   MESH *Mesh;

   Cur_Node = Node = (OBJ_NODE *)Dum;
   Cur_Obj = (OBJECT *)Node->Data;
   Cur_Mesh = Mesh = (MESH *)Node->Data;

   DEBUG( Out_Message( "\n Current_Data set to: CHK_TRACKMESH" ) );
   Current_Data = CHK_TRACKMESH;

   if ( Write_Chunk( CHK_NODE_ID, (void *)Node, 0 ) )
      return( IO_ERROR );

   if ( Write_Chunk( CHK_NODE_HDR, (void *)Node, 0 ) )
      return( IO_ERROR );

   if ( Write_Chunk( CHK_PIVOT, (void *)Node, 0 ) )
      return( IO_ERROR );

   EMIT_TRACK( Mesh->Scale_k, CHK_TRACKSCALE );
   EMIT_TRACK( Mesh->Pos_k, CHK_TRACKPOS );
   EMIT_TRACK( Mesh->Rot_k, CHK_TRACKROTATE );
   EMIT_TRACK( Mesh->Hide_k, CHK_TRACKHIDE );
   EMIT_TRACK( Mesh->Morph_k, CHK_TRACKMORPH );

   return( SKY_IS_BLUE );
}

static INT Track_Camera_Wrt( void *Dum, INT Nb )
{
   OBJ_NODE *Node;
   CAMERA *Camera;

   Cur_Node = Node = (OBJ_NODE *)Dum;
   Camera = (CAMERA *)Node->Data;

   DEBUG( Out_Message( "\n Current_Data set to: CHK_TRACKCAMERA" ) );
   Current_Data = CHK_TRACKCAMERA;

   if ( Write_Chunk( CHK_NODE_ID, (void *)Node, 0 ) )
      return( IO_ERROR );

   if ( Write_Chunk( CHK_NODE_HDR, (void *)Node, 0 ) )
      return( IO_ERROR );

      // Pos + Fov + Roll
   EMIT_TRACK( Camera->Pos_k, CHK_TRACKPOS );
   EMIT_TRACK( Camera->FOV_k, CHK_TRACKFOV );
   EMIT_TRACK( Camera->Roll_k, CHK_TRACKROLL );

   return( SKY_IS_BLUE );
}

static INT Track_Camera_Target_Wrt( void *Dum, INT Nb )
{
   OBJ_NODE *Node;
   CAMERA *Camera;

   Cur_Node = Node = (OBJ_NODE *)Dum;
   Camera = (CAMERA *)Node->Data;

   DEBUG( Out_Message( "\n Current_Data set to: CHK_TRACKCAMTGT" ) );
   Current_Data = CHK_TRACKCAMTGT;

   if ( Write_Chunk( CHK_NODE_ID, (void *)Node, 0 ) )
      return( IO_ERROR );

   if ( Write_Chunk( CHK_NODE_HDR, (void *)Node, 0 ) )
      return( IO_ERROR );

         // Target
   EMIT_TRACK( Camera->Target_k, CHK_TRACKPOS );

   return( SKY_IS_BLUE );
}

static INT Track_Light_Wrt( void *Dum, INT Nb )
{
   OBJ_NODE *Node;
   LIGHT *Light;

   Cur_Node = Node = (OBJ_NODE *)Dum;
   Light = (LIGHT *)Node->Data;

   DEBUG( Out_Message( "\n Current_Data set to: CHK_TRACKLIGHT" ) );
   Current_Data = CHK_TRACKLIGHT;

   if ( Write_Chunk( CHK_NODE_ID, (void *)Node, 0 ) )
      return( IO_ERROR );

   if ( Write_Chunk( CHK_NODE_HDR, (void *)Node, 0 ) )
      return( IO_ERROR );

      // Pos
   EMIT_TRACK( Light->Pos_k, CHK_TRACKPOS );
   EMIT_TRACK( Light->Color_k, CHK_TRACKCOLOR );
   EMIT_TRACK( Light->Hide_k, CHK_TRACKHIDE );

   return( SKY_IS_BLUE );
}

static INT Track_Light_Target_Wrt( void *Dum, INT Nb )
{
   OBJ_NODE *Node;
   LIGHT *Light;

   Cur_Node = Node = (OBJ_NODE *)Dum;
   Light = (LIGHT *)Node->Data;

   if ( Light->Light_Type != LIGHT_SPOT ) return( SKY_IS_BLUE );

   DEBUG( Out_Message( "\n Current_Data set to: CHK_TRACKLIGTGT" ) );
   Current_Data = CHK_TRACKLIGTGT;

   if ( Write_Chunk( CHK_NODE_ID, (void *)Node, 0 ) )
      return( IO_ERROR );

   if ( Write_Chunk( CHK_NODE_HDR, (void *)Node, 0 ) )
      return( IO_ERROR );

      // Target + Rot + Roll ?
   EMIT_TRACK( Light->Target_k, CHK_TRACKPOS );
   EMIT_TRACK( Light->Rot_k, CHK_TRACKROTATE );
   EMIT_TRACK( Light->Roll_k, CHK_TRACKROTATE );
   EMIT_TRACK( Light->Falloff_k, CHK_TRACKFALLOFF );
   EMIT_TRACK( Light->Hotspot_k, CHK_TRACKHOTSPOT );
   return( SKY_IS_BLUE );
}

/******************************************************************/
/*                            Level III                           */
/******************************************************************/

static INT Write_Mesh( void *Dum, INT Nb )
{
   MESH *M;
   OBJ_NODE *Node;

   Node = (OBJ_NODE *)Dum;
   M = (MESH *)Node->Data;

   if ( Write_Chunk( CHK_VERTLIST, (void *)M->Vertex, M->Nb_Vertex ) )
      return( IO_ERROR );

   if ( M->UV!=NULL )
      if ( Write_Chunk( CHK_MAPLIST, (void *)M->UV, M->Nb_Vertex ) )
         return( IO_ERROR );

   if ( Write_Chunk( CHK_TRMATRIX, (void *)Node, 0 ) )
      return( IO_ERROR );

   if ( Write_Chunk( CHK_FACELIST, (void *)M->Polys, M->Nb_Polys ) )
      return( IO_ERROR );

#if 0    // proprietary chunks
   if ( M->Flags & OBJ_HAS_POLY_UV )   // save UV in each polys
      if ( Write_Chunk( CHK_POLY_UV, (void *)M->Polys, M->Nb_Polys ) )
         return( IO_ERROR );

   if ( M->Flags & OBJ_HAS_EDGES_OK )     // save Edges_ID in each polys
      if ( Write_Chunk( CHK_POLY_EDGES, (void *)M->Polys, M->Nb_Polys ) )
         return( IO_ERROR );

   if ( M->Flags & OBJ_EDGES_OK )   // save Edges
      if ( Write_Chunk( CHK_EDGES, (void *)M->Edges, M->Nb_Edges ) )
         return( IO_ERROR );
#endif

   Write_FaceMat( M );

   return( SKY_IS_BLUE );
} 

static INT Write_Light( void *Dum, INT Nb )
{
   LIGHT *L;
   OBJ_NODE *Node;

   Node = (OBJ_NODE *)Dum;
   L = (LIGHT *)Node->Data;

   Emit_Vector( L->Pos, 3 );  // Position.
   return( SKY_IS_BLUE );
}

static INT Write_Spot_Light( void *Dum, INT Nb )
{
   LIGHT *L;
   OBJ_NODE *Node;

   Node = (OBJ_NODE *)Dum;
   L = (LIGHT *)Node->Data;

   Emit_Vector( L->Dir, 3 );  // Direction.
   Emit_Flt( 30.0f );         // Fall-off
   Emit_Flt( 0.0 );           // Hotspot

   return( SKY_IS_BLUE );
}

static INT Write_Camera( void *Dum, INT Nb )
{
   CAMERA *Cam;
   OBJ_NODE *Node;

   Node = (OBJ_NODE *)Dum;
   Cam = (CAMERA *)Node->Data;

   Emit_Vector( Cam->Pos, 3 );          //  Position
   Emit_Vector( Cam->Target, 3 );       // +Target
   Emit_Flt( Cam->Bank*180.0f/M_PI );   // +Bank
   Emit_Flt( (FLT)atan( Cam->D/Cam->Lens )*360.0/M_PI );   // +~Fov
   return( SKY_IS_BLUE );
}

/******************************************************************/

static INT MatName_Wrt( void *Dum, INT Nb )
{
   MATERIAL *Mat;
   Mat = (MATERIAL *)Dum;

   if ( Write_String_3ds( Mat->Name ) ) return( MEM_ERROR );
   return( SKY_IS_BLUE );
}

/******************************************************************/

static INT Pivot_Wrt( void *Dum, INT Nb )
{
   OBJ_NODE *Node;

   Node = (OBJ_NODE *)Dum;
   Emit_Vector( Node->Transform.Pivot, 3 );      // Pivot[] = 0.0
   return( SKY_IS_BLUE );
}

/******************************************************************/

static INT Track_3D_Wrt( void *Dum, INT Nb )
{
   INT i, N;
   SPLINE_3DS *Sp;
   VECTOR V;

   Sp = (SPLINE_3DS *)Dum;

   N = Emit_Sp_Flags( Sp );
   Emit_Int( N );   // nb keys

   for( i=0; i<N; ++i )
   {
      Emit_Sp_Key_Flags( &Sp->Keys[i] );     // flags
      if ( (Cur_Node!=NULL) && (Cur_Node->Type==MESH_TYPE) )
      {
         Add_Vector( V, Sp->Keys[i].Values, Cur_Node->Transform.Pivot );
         Emit_Vector( V, 3 );  // position
//         Emit_Vector( Sp->Keys[i].Values, 3 );
      }
      else Emit_Vector( Sp->Keys[i].Values, 3 );
   }
   return( SKY_IS_BLUE );
}

static INT Track_3D_Vector_Wrt( void *Dum, INT Nb )
{
   INT i, N;
   SPLINE_3DS *Sp;

   Sp = (SPLINE_3DS *)Dum;

   N = Emit_Sp_Flags( Sp );
   Emit_Int( N );   // nb keys

   for( i=0; i<N; ++i )
   {
      Emit_Sp_Key_Flags( &Sp->Keys[i] );     // flags
      Emit_Vector( Sp->Keys[i].Values, 3 );  // Scale, or Color
   }
   return( SKY_IS_BLUE );
}

#if 0       // UNUSED
static INT Track_1D_Raw_Wrt( void *Dum, INT Nb )
{
   INT i, N;
   SPLINE_3DS *Sp;

   Sp = (SPLINE_3DS *)Dum;

   N = Emit_Sp_Flags( Sp );
   Emit_Int( N );   // nb keys

   for( i=0; i<N; ++i )
   {
      Emit_Sp_Key_Flags( &Sp->Keys[i] );  // flags
      Emit_Flt( Sp->Keys[i].Values[0] );  // Fov, 
   }
   return( SKY_IS_BLUE );
}
#endif

static INT Track_1D_Degree_Wrt( void *Dum, INT Nb )
{
   INT i, N;
   SPLINE_3DS *Sp;

   Sp = (SPLINE_3DS *)Dum;

   N = Emit_Sp_Flags( Sp );
   Emit_Int( N );   // nb keys

   for( i=0; i<N; ++i )
   {
      FLT Tmp;
      Emit_Sp_Key_Flags( &Sp->Keys[i] );  // flags
      Tmp = Sp->Keys[i].Values[0]*180.0f/M_PI;
      Emit_Flt( Tmp );     // roll, falloff or Hotspot
   }
   return( SKY_IS_BLUE );
}

static INT Track_1D_Hide_Wrt( void *Dum, INT Nb )
{
   INT i, N;
   SPLINE_3DS *Sp;

   Sp = (SPLINE_3DS *)Dum;

   N = Emit_Sp_Flags( Sp );
   Emit_Int( N );   // nb keys

   for( i=0; i<N; ++i )
      Emit_Sp_Key_Flags( &Sp->Keys[i] );  // flags

   return( SKY_IS_BLUE );
}

static INT Track_Morph_Wrt( void *Dum, INT Nb )
{
   INT i, N;
   SPLINE_3DS *Sp;

   Sp = (SPLINE_3DS *)Dum;

   N = Emit_Sp_Flags( Sp );
   Emit_Int( N );   // nb keys

   for( i=0; i<N; ++i )
   {
      OBJ_NODE *Node;
      Emit_Sp_Key_Flags( &Sp->Keys[i] );  // flags
      Node = Cur_Mesh->Morph_Node[ (INT)Sp->Keys[i].Values[0] ];
      if ( Write_String_3ds( Node->Name ) )
         return( MEM_ERROR );
   }
   return( SKY_IS_BLUE );
}

extern double sqrt( double );
static INT Track_4D_Wrt( void *Dum, INT Nb )
{
   INT i, N;
   SPLINE_3DS *Sp;
   QUATERNION Q, Q_Prev;

   Sp = (SPLINE_3DS *)Dum;

   N = Emit_Sp_Flags( Sp );
   Emit_Int( N );   // nb keys

#define _X_ 1
#define _Y_ 2
#define _Z_ 3
#define _W_ 0

   for( i=0; i<N; ++i )
   {
      Emit_Sp_Key_Flags( &Sp->Keys[i] );  // flags
      Q[0] = Sp->Keys[i].Values[0];
      Q[1] = Sp->Keys[i].Values[1];
      Q[2] = Sp->Keys[i].Values[2];
      Q[3] = Sp->Keys[i].Values[3];

      if ( i>0 )
      {
         Q[1] = -Q[1]; Q[2] = -Q[2]; Q[3] = -Q[3];
         Mult_Quat_Eq( Q, Q_Prev ); // Q = (Q^-1).Q_Prev
      }
      Q_Prev[0] = Q[0]; Q_Prev[1] = Q[1]; Q_Prev[2] = Q[2]; Q_Prev[3] = Q[3];

      Q[1] = -Q[1]; Q[2] = -Q[2]; Q[3] = -Q[3];
      Quaternion_To_Theta_V( Q, Q );
      Emit_Flt( Q[0] ); Emit_Flt( Q[1] ); Emit_Flt( Q[2] ); Emit_Flt( Q[3] );
   }
   return( SKY_IS_BLUE );
}

/******************************************************************/
/*                            Level IV                            */
/******************************************************************/

static INT Vert_List_Wrt( void *Dum, INT Nb )
{
   VECTOR *V, Tmp;
   INT i;
   V = (VECTOR *)Dum;

   Emit_Short( (SHORT)Nb );

   for( i=0; i<Nb; ++i )
   {
      Set_Vector( Tmp, V[i][0], V[i][1], V[i][2] );
      Add_Vector_Eq( Tmp, Cur_Node->Transform.Pivot );
      if ( Emit_Vector( Tmp, 3 ) ) return( IO_ERROR );
//        if ( Emit_Vector( V[i], 3 ) ) return( IO_ERROR );
   }
   return( SKY_IS_BLUE );
}

static INT Poly_List_Wrt( void *Dum, INT Nb )
{
   POLY *P;
   INT i;

   P = (POLY *)Dum;

   Emit_Short( (SHORT)Nb );

   for( i=0; i<Nb; ++i )
   {
      SHORT Flags;
      if ( Emit_Short( P[i].Pt[0] ) ) return( IO_ERROR );
      if ( Emit_Short( P[i].Pt[1] ) ) return( IO_ERROR );
      if ( Emit_Short( P[i].Pt[2] ) ) return( IO_ERROR );
      Flags = 0x80;  // <= means 'optimized'
      if ( P[i].Flags & POLY_AB_OK ) Flags|=0x04;
      if ( P[i].Flags & POLY_BC_OK ) Flags|=0x02;
      if ( P[i].Flags & POLY_CA_OK ) Flags|=0x01;
      if ( P[i].Flags & POLY_U_WRAP ) Flags|=0x08;
      if ( P[i].Flags & POLY_V_WRAP ) Flags|=0x10;
      if ( Emit_Short( Flags ) ) return( IO_ERROR );   // Flags...
   }
   return( SKY_IS_BLUE );
}

static INT Tr_Matrix_Wrt( void *Dum, INT Nb )
{
   MATRIX Mtx;

   Id_Matrix( Mtx );
   if ( Emit_Vector( Mtx, 3*3 ) ) return( IO_ERROR ); // Node->Mo instead ?!
   Emit_Flt( 0.0 ); Emit_Flt( 0.0 ); Emit_Flt( 0.0 );    // Pivot

   return( SKY_IS_BLUE );
}

static INT Mapping_Wrt( void *Dum, INT Nb )
{
   F_MAPPING *UV;
   INT i;

   UV = (F_MAPPING *)Dum;

   Emit_Short( (SHORT)Nb );

   for( i=0; i<Nb; ++i )
   {
      if ( Emit_Flt( UV[i][0] ) ) return( IO_ERROR );
      if ( Emit_Flt( 1.0f - UV[i][1] ) ) return( IO_ERROR );
   }
   return( SKY_IS_BLUE );
}

static INT FaceMat_Wrt( void *Dum, INT Count )
{
   INT j;
   MATERIAL *Da_Mat = (MATERIAL *)Dum;

   Write_String_3ds( Da_Mat->Name );
   Emit_Short( (USHORT)Count );
   for( j=0; j<Da_Mesh->Nb_Polys; ++j )
   {
      if ( Da_Mesh->Polys[j].Ptr == Da_Mat )
         Emit_Short( (USHORT)j );
   }
   return( SKY_IS_BLUE );
}

static void Write_FaceMat( MESH *M )
{
   INT i, j, N;
   INT Da_Count[65536];
   MATERIAL *Mats[65536];
   MATERIAL *Da_Mat;

   Da_Mesh = M;
   N = 0;
//   bzero( Mats, 65536*sizeof(Mats[0]) );
//   bzero( Da_Count, 65536*sizeof(Da_Count[0]) );
   for( i=0; i<Da_Mesh->Nb_Polys; ++i )
   {
      Da_Mat = (MATERIAL *)Da_Mesh->Polys[i].Ptr;
      if ( Da_Mat==NULL ) continue;
      for( j=0; j<N; ++j ) 
         if ( Da_Mat==Mats[j] )
         {
            Da_Count[j]++;
            break;
         }
      if ( j<N ) continue;
      Mats[N] = Da_Mat;
      Da_Count[N] = 1;
      N++;
   }
   for( i=0; i<N; ++i )
      if ( Write_Chunk( CHK_FACEMAT, (void *)Mats[i], Da_Count[i] ) )
         return;
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

static struct {

    CHUNK_TYPE ID;
    INT (*Writer)( void *, INT );

} Chunk_Writers[] = {

   { CHK_MAIN,         Main_Wrt },
   { CHK_OBJMESH,      Editor_3D_Wrt },
   { CHK_OBJBLOCK,     Obj_Block_Wrt },
   { CHK_OBJHIDDEN,    NULL },
   { CHK_TRIMESH,      Write_Mesh },
   { CHK_VERTLIST,     Vert_List_Wrt },
   { CHK_FACELIST,     Poly_List_Wrt },
   { CHK_FACEMAT,      FaceMat_Wrt },
   { CHK_MAPLIST,      Mapping_Wrt },
   { CHK_TRMATRIX,     Tr_Matrix_Wrt },

   { CHK_CAMERA,       Write_Camera },
   { CHK_SMOOLIST,     NULL },
   { CHK_LIGHT,        Write_Light },
   { CHK_SPOTLIGHT,    Write_Spot_Light },
   { CHK_LGHTZMIN,     Light_ZMin_Wrt },
   { CHK_LGHTZMAX,     Light_ZMax_Wrt },
   { CHK_LGHTMULT,     Light_Mult_Wrt },
   { CHK_LIGHT_OFF,    Light_Off_Wrt },

   { CHK_MATERIAL,     Material_Wrt },
   { CHK_MATNAME,      MatName_Wrt },
   { CHK_MAPFILE,      Map_Wrt },

   { CHK_KEYFRAMER,    Keyframer_Wrt },
   { CHK_NODE_ID,      ID_Wrt },
   { CHK_NODE_HDR,     HDR_Wrt },
   
   { CHK_FRAMES,       Frames_Wrt },
   { CHK_PIVOT,        Pivot_Wrt },  
   
   { CHK_TRACKMESH,    Track_Mesh_Wrt },
   { CHK_TRACKCAMERA,  Track_Camera_Wrt },
   { CHK_TRACKCAMTGT,  Track_Camera_Target_Wrt },
   { CHK_TRACKLIGHT,   Track_Light_Wrt },
   { CHK_TRACKLIGTGT,  Track_Light_Target_Wrt },
   { CHK_TRACKSPOTL,   NULL },

   { CHK_TRACKAMB,     NULL },
   { CHK_TRACKPOS,     Track_3D_Wrt },
   { CHK_TRACKROTATE,  Track_4D_Wrt },
   { CHK_TRACKSCALE,   Track_3D_Vector_Wrt },
   { CHK_TRACKFOV,     Track_1D_Degree_Wrt },
   { CHK_TRACKROLL,    Track_1D_Degree_Wrt },
   { CHK_TRACKHIDE,    Track_1D_Hide_Wrt },
   { CHK_TRACKFALLOFF, Track_1D_Degree_Wrt },
   { CHK_TRACKHOTSPOT, Track_1D_Degree_Wrt },
   { CHK_TRACKCOLOR,   Track_3D_Vector_Wrt },
   { CHK_TRACKMORPH,   Track_Morph_Wrt },

};

/******************************************************************/

static INT Find_Chunk_From_ID( CHUNK_TYPE ID )
{
   int i;

   for( i=0; i<sizeof( Chunk_Writers )/sizeof( Chunk_Writers[0] ); i++ )
      if ( ID == Chunk_Writers[i].ID )
         return i;
   return( -1 );
}

static INT Write_Chunk( USHORT ID, void *Obj, INT Nb )
{
   INT N, Pos, E;
   BYTE Buf[6] = { 0 };

   Pos = ftell( Out );
   if ( fwrite( (void*)Buf, 6, 1, Out )!=1 ) return( IO_ERROR );

      // Process...

   N = Find_Chunk_From_ID( (CHUNK_TYPE) ID );
   if ( N==-1 || Chunk_Writers[N].Writer==NULL ) goto Skip; /* ?!? */

   fprintf( stderr, "[ID=0x%.4x] ", ID );
   T_Count++;
   if ( T_Count==6 ) { fprintf( stderr, "\n" ); T_Count=0; }
   if ( ( *Chunk_Writers[N].Writer )( Obj, Nb ) ) return( IO_ERROR );

Skip:
   E = ftell( Out ); 
   fseek( Out, Pos, SEEK_SET );
   *(USHORT *)( &Buf[ 0 ] ) = Indy_s( ID );
   *(INT*)( &Buf[ 2 ] ) = Indy_l( E-Pos );
   if ( fwrite( (void*)Buf, 6, 1, Out )!=1 ) return( IO_ERROR );
   fseek( Out, E, SEEK_SET );
   return( SKY_IS_BLUE );
};

/******************************************************************/
/******************************************************************/

EXTERN INT Dump_World( STRING Name, WORLD *W )
{
   INT Err;

   N_Count = 1;
   T_Count = 0;

   Da_World = W;
   Out = Access_File( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( IO_ERROR );

   Err = Write_Chunk( CHK_MAIN, (void *)W, 0 );

   fclose( Out );
   Da_World = NULL;
   if ( Err )
   {
      fprintf( stderr, "Err=%d!\n", Err );
      return( IO_ERROR );
   }
   else return( SKY_IS_BLUE );
}

/******************************************************************/
/******************************************************************/

static INT Emit_Byte( PIXEL d )
{
   if ( fwrite( (void*)&d, sizeof( d ), 1, Out )!=1 ) return( IO_ERROR );
   return( SKY_IS_BLUE );
} 

static INT Emit_Short( USHORT d )
{
   d = Indy_s( d );
   if ( fwrite( (void*)&d, sizeof( d ), 1, Out )!=1 ) return( IO_ERROR );
   return( SKY_IS_BLUE );
} 

static INT Emit_Int( INT d )
{
   d = Indy_l( d );
   if ( fwrite( (void*)&d, sizeof( d ), 1, Out )!=1 ) return( IO_ERROR );
   return( SKY_IS_BLUE );
} 

static INT Emit_Flt( FLT x )
{
   x = Indy_f( x );
   if ( fwrite( (void*)&x, sizeof( x ), 1, Out )!=1 ) return( IO_ERROR );
   return( SKY_IS_BLUE );
}

static INT Emit_Vector( FLT *x, INT Len )
{
   INT i;
   for( i=0; i<Len; ++i )
      if ( Emit_Flt( x[i] )!=SKY_IS_BLUE ) return( IO_ERROR );
   return( SKY_IS_BLUE );
}

static INT Write_String_3ds( STRING Name )
{
   INT Len;
   char Buf[20];
   if ( Name == NULL )
   {
      Name = Buf;
      sprintf( Buf, "N%.3d", N_Count++ );
   }
   Len = strlen( Name ) + 1;
   if ( fwrite( (void*)Name, Len, 1, Out )!=1 ) return( IO_ERROR );
   return( SKY_IS_BLUE );
}

static INT Emit_Sp_Flags( SPLINE_3DS *Sp )
{
   USHORT Flag_0;

   Flag_0 = 0x00;
   if ( Sp->Flags & TRACK_REPEAT ) Flag_0 |= 0x02;
   if ( Sp->Flags & TRACK_LOOP ) Flag_0 |= 0x03;
   Emit_Short( Flag_0 );
   Emit_Short( 0 );
   Emit_Short( 0 );
   Emit_Short( 0 );
   Emit_Short( 0 );

   return( Sp->Nb_Keys );
}

static INT Emit_Sp_Key_Flags( A_KEY *K )
{
   INT Flags;

   Emit_Int( (INT)K->Frame );
   Flags = 0x0;

   if ( K->Tens!=0.0 ) Flags |= 0x01;
   if ( K->Cont!=0.0 ) Flags |= 0x02;
   if ( K->Bias!=0.0 ) Flags |= 0x04;
   if ( K->Ease_In!=0.0 ) Flags |= 0x08;
   if ( K->Ease_Out!=0.0 ) Flags |= 0x10;

   Emit_Short( (USHORT)Flags );

   if ( Flags&0x01 ) Emit_Flt( K->Tens );
   if ( Flags&0x02 ) Emit_Flt( K->Cont );
   if ( Flags&0x04 ) Emit_Flt( K->Bias );
   if ( Flags&0x08 ) Emit_Flt( K->Ease_In );
   if ( Flags&0x10 ) Emit_Flt( K->Ease_Out );

   return( SKY_IS_BLUE );  
}

/******************************************************************/
/******************************************************************/

