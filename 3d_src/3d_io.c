/************************************************
 *         3Ds Objects I/O                      *
 * heavily derivated from code from Jare/IGUANA *
 * and also from specs at:                      *
 * www.viewpoint.com/avalon/format_specs.html   *
 *                                              *
 * Skal 97.                                     *
 ************************************************/

#include "main3d.h"
#include "indy.h"

/******************************************************************/
/******************************************************************/

static STRING Current_Name;
static CHUNK_TYPE Current_Data;
static SHORT Current_ID;
static INT Chunk_Read( FILE *F, UINT Size );
static INT Cur_Color_Read = 0;
static INT Cur_Map_Nb = 0;
static OBJ_NODE *Cur_Dummy = NULL;

/******************************************************************/
/******************************************************************/

static void Set_Current_Name( STRING Name )
{
   Current_Name = Str_Dup( Name );
   if ( Current_Name==NULL ) Exit_Upon_Error( "Mem error" );
}

EXTERN int Read_String( FILE *In, STRING Name, int Max )
{
   int c, n;
   
   n = 0;
   do {
      c = fgetc( In );
      if ( c==EOF || c=='\0' ) break;
      Name[n++] = c;
   } while( n<Max-1 );
   Name[n++] = '\0';
   return( n );
}

/******************************************************************/
/******************************************************************/

static INT Instance_Name_Rd( FILE *F, UINT Size )
{
   char Name[255];

   Read_String( F, Name, 255 );
   DEBUG  ( Out_Message( "\n---- Instance name: %s ----\n", Name ) );

   if ( Cur_Node!=NULL )
   {
      Set_Current_Name( Name );
      M_Free( Cur_Node->Name );
      Cur_Node->Name = Current_Name;
      Cur_Node->Flags = (OBJ_FLAG)(Cur_Node->Flags | OBJ_OWNS_NAME);
      Cur_Node->Data->Name = NULL;
      Cur_Node->Data->Flags = (OBJ_FLAG)(Cur_Node->Data->Flags & ~OBJ_OWNS_NAME);
      Current_Name = NULL;
   }
   return( SKY_IS_BLUE );
}

static INT Obj_Block_Rd( FILE *F, UINT Size )
{
   char Name[255];
   INT Err;

   Read_String( F, Name, 255 );
   DEBUG( Out_Message( "\nReading object : %s", Name ) );

   Cur_Obj = NULL;
   Set_Current_Name( Name );

   Err = Chunk_Read( F, Size );
   if ( Err ) return( Err );

   return( SKY_IS_BLUE );
}

#ifndef _SKIP_HIDE_

static INT Obj_Hidden_Rd( FILE *F, UINT Size )
{
   DEBUG( Out_Message( "Hidden object" ) );
//   Obj_Hidden = TRUE;

   return( SKY_IS_BLUE );
}

#endif   // _SKIP_HIDE_

static INT Obj_No_Cast_Rd( FILE *F, UINT Size )
{
   Cur_Node->Flags |= OBJ_NO_SHADOW;
   return( SKY_IS_BLUE );
}

static INT Obj_No_Receive_Shadow_Rd( FILE *F, UINT Size )
{
//   fprintf( stderr, "No rcv: %s\n", Cur_Node->Name );
//   if ( strcmp( Cur_Node->Name, "roches" ) )
   Cur_Node->Flags |= OBJ_DONT_RECEIVE_SHDW;
   return( SKY_IS_BLUE );
}

static INT Obj_Tri_Mesh( FILE *F, UINT Size )
{
   Cur_Node = New_Object_By_Type( Current_Name, &_MESH_, _RCst_.The_World );
   if ( Cur_Node==NULL ) return( MEM_ERROR );
   Cur_Obj = Cur_Node->Data;
   Current_Name = NULL;
   return( Chunk_Read( F, Size ) );
}

static INT Vert_List_Rd( FILE *F, UINT Size )
{
   SHORT N, i;

      // Is vertex chunk always first ??

   if ( Cur_Obj==NULL ) return( UGLY_BUG );
   if ( Cur_Obj->Type != MESH_TYPE ) return( SKY_IS_BLUE );
   Cur_Msh = (MESH *)Cur_Obj;

   READ_ONE( N ); N = Indy_s( N );

   if ( Set_Mesh_Data( Cur_Msh, N, 0 )==NULL )   // This clears .Edges field!
      return( UGLY_BUG );
   DEBUG( Out_Message( "  %d vertices", N ) );
   for( i=0; i<N; ++i )
   {
      VECTOR V;

      READ_M(V);
      V[0] = Indy_f( V[0] );
      V[1] = Indy_f( V[1] );
      V[2] = Indy_f( V[2] );

      Cur_Msh->Vertex[i][0] = V[0];
      Cur_Msh->Vertex[i][1] = V[1];
      Cur_Msh->Vertex[i][2] = V[2];
      Set_Vector( Cur_Msh->Normals[i], 0.0, 0.0, 1.0 );
   }
   return( SKY_IS_BLUE );
}

static INT Mapping_Rd( FILE *F, UINT Size )
{
   SHORT N;
   F_MAPPING UV;
   INT i;

   if ( Cur_Obj->Type != MESH_TYPE ) return( SKY_IS_BLUE );
   Cur_Msh = (MESH *)Cur_Obj;

   READ_ONE(N); N = Indy_s( N );
   if ( Set_Mesh_Data( Cur_Msh, N, 0 )==NULL )   // This clears .Edges field!
      return( UGLY_BUG );
   DEBUG( Out_Message( "  %d Mapping coords", N ) );

   for( i=0; i<N; ++i )
   {
      READ_ONE( UV[0] ); UV[0] = Indy_f( UV[0] );
      READ_ONE( UV[1] ); UV[1] = Indy_f( UV[1] );
#if 0
      while( UV[0]<0.0 ) UV[0] += 1.0;
      while( UV[0]>1.0 ) UV[0] -= 1.0;
      while( UV[1]<0.0 ) UV[1] += 1.0;
      while( UV[1]>1.0 ) UV[1] -= 1.0;
#endif
      UV[1] = 1.0f - UV[1];
      Cur_Msh->UV[i][0] = UV[0]; // fmod( UV[0], 1.0 );
      Cur_Msh->UV[i][1] = UV[1]; // fmod( UV[1], 1.0 );
   }
   Cur_Obj->Flags = (OBJ_FLAG)(Cur_Obj->Flags | OBJ_HAS_UV);
   return( SKY_IS_BLUE );
}

#ifdef _SKIP_OPTIM_POLY_

static void Optimize_Poly_Pts( POLY *P, MESH *Msh )
{
   FLT Max;
   INT i, io;
   INT p1=0, p2=1, p3=2;

   Max = -1.0; io = 0;
   for( i=0; i<P->Nb_Pts; ++i )
   {
      FLT N1, N2, Dot;
      VECTOR V1, V2;
      Sub_Vector( V1, Msh->Vertex[P->Pt[p1]], Msh->Vertex[P->Pt[p2]] );
      N1 = Norm_Squared( V1 );
      if ( N1<=0.0 ) goto Skip;
      N1 = 1.0f / (FLT)sqrt( N1 );
      Sub_Vector( V2, Msh->Vertex[P->Pt[p2]], Msh->Vertex[P->Pt[p3]] );
      N2 = Norm_Squared( V2 );
      if ( N2<=0.0 ) goto Skip;
      N2 = 1.0f / (FLT)sqrt( N2 );
      Dot = Dot_Product( V1, V2 ) * N1*N2;
      if ( Dot>Max ) { Max = Dot; io = p2; }
Skip:
      p1=p2; p2=p3; p3++; if ( p3==P->Nb_Pts ) p3=0;
   }
   if ( io==0 ) return;
   p1 = io;
   p2 = p1+1; if ( p2==P->Nb_Pts ) p2=0;
   p3 = p2+1; if ( p3==P->Nb_Pts ) p3=0;
   p1 = P->Pt[p1];
   p2 = P->Pt[p2];
   p3 = P->Pt[p3];
   P->Pt[0] = (USHORT)p1;
   P->Pt[1] = (USHORT)p2;
   P->Pt[2] = (USHORT)p3;
   P->Flags |= 0x80;    // <= means 'optimized'. 
                        // Hope it won't conflict with some 3ds flags...
}

#endif   // _SKIP_OPTIM_POLY_


static INT Poly_List_Rd( FILE *F, UINT Size )
{
   SHORT N;
   SHORT Block[4];
   INT i;
   POLY *P;

   if ( Cur_Obj->Type != MESH_TYPE ) return( SKY_IS_BLUE );
   Cur_Msh = (MESH *)Cur_Obj;

   READ_ONE( N ); N = Indy_s( N );
   DEBUG( Out_Message("  %d Polys", N ) );

   if ( Set_Mesh_Data( Cur_Msh, 0, N )==NULL )   // This clears .Edges field!
      return( UGLY_BUG );

   P = Cur_Msh->Polys;
   for( i=0; i<N; ++i, P++ )
   {
      READ_M( Block );
      P->Pt[0] = Indy_s( Block[0] );
      P->Pt[1] = Indy_s( Block[1] );
      P->Pt[2] = Indy_s( Block[2] );
      P->Nb_Pts = 3;

      Block[3] = Indy_s( Block[3] );
      P->Flags = POLY_NO_FLAG;

#if 0
      if ( Block[3]&0x01 ) P->Flags |= POLY_CA_OK;
      if ( Block[3]&0x02 ) P->Flags |= POLY_BC_OK;
      if ( Block[3]&0x04 ) P->Flags |= POLY_AB_OK;
#else
      P->Flags |= POLY_AB_OK|POLY_BC_OK|POLY_CA_OK; // full..
#endif

      if ( Block[3]&0x08 )
         P->Flags |= POLY_U_WRAP;
      if ( Block[3]&0x10 )
         P->Flags |= POLY_V_WRAP;

#ifdef OPTIM_POLY
      if ( !(Block[3]&0x80) ) Optimize_Poly_Pts( P, Cur_Msh );
#endif   // OPTIM_POLY
   }
   Mesh_Store_Normals2( Cur_Msh );  // Don't call Mesh_Store_Normals() here!
   Average_Normals( Cur_Msh );
   return( Chunk_Read( F, Size ) );
}

static INT Face_Mat_Rd( FILE *F, UINT Size )
{
   char Name[255];
   SHORT N, Index;
   MATERIAL *Mat;

   if ( Cur_Obj->Type != MESH_TYPE ) return( SKY_IS_BLUE );
   Cur_Msh = (MESH *)Cur_Obj;

   Read_String( F, Name, 255 );
   
   READ_ONE( N ); N = Indy_s( N );
   DEBUG( Out_Message( " Mapping object 0x%x with '%s' (%d polys to be mapped)", 
      Cur_Msh, Name, N ) );
   Mat = (MATERIAL *)Search_Object_From_Name( _RCst_.The_World->Materials, Name );
   if ( Mat==NULL ) 
      return( UGLY_BUG );

   Mat->Users += 1;        // *NOT* N!!
   while( N-- > 0 )
   {
      READ_ONE( Index ); Index = Indy_s( Index );
//      if ( Cur_Msh->Polys[Index].Ptr != NULL )
//         fprintf( stderr, "**" );
      Cur_Msh->Polys[Index].Ptr = (void *)Mat;
   }

   return( SKY_IS_BLUE );
}

static INT Smooth_List_Rd( FILE *F, UINT Size )
{
#if 0
   INT Index;
   
   for( Index=0; Index<Cur_Msh->Nb_Vertex )
   {
      UINT Bits, i;

      READ_ONE( Bits ); Bits = Indy_l( Bits );
//      Out_Message( "Bits=%d Index=%d Size=%d", Bits, Index++, Size );
      for ( i=0; i<32; i++ )
         if ( Bits & (1<<i) )    // we've found group number i
         {
            Cur_Msh->Polys[Index].Flags |= POLY_SMOOTH;
         }
   }
#endif
   return( SKY_IS_BLUE );
}

/******************************************************************/

static INT Tr_Matrix_Rd( FILE *F, UINT Size )
{
   VECTOR Pivot;
   FLT M[9];
   QUATERNION Q;
   INT i;

   READ_ONE( M );
   for( i=0; i<9; ++i ) M[i] = Indy_f( M[i] );

   READ_ONE( Pivot );
   Pivot[0] = Indy_f( Pivot[0] );
   Pivot[1] = Indy_f( Pivot[1] );
   Pivot[2] = Indy_f( Pivot[2] );

   Matrix_To_Quaternion( M, Q );

   Quaternion_To_Matrix( Q, Cur_Node->Mo );

   Cur_Msh = (MESH *)Cur_Node->Data;

   Sub_Vertex_Eq( Cur_Msh, Pivot );
   Matrix_Vertex_Eq( Cur_Msh, Cur_Node->Mo );

   Cur_Node->Transform.Rot.QRot[0] = Q[0];
   Cur_Node->Transform.Rot.QRot[1] = Q[1];
   Cur_Node->Transform.Rot.QRot[2] = Q[2];
   Cur_Node->Transform.Rot.QRot[3] = Q[3];

   Cur_Node->Transform.Type = TRANSF_QUATERNION;
   Set_Vector( Cur_Node->Transform.Pivot, Pivot[0], Pivot[1], Pivot[2] );
   Set_Vector( Cur_Node->Transform.Pos, 0.0, 0.0, 0.0 );
   Set_Vector( Cur_Node->Transform.Scale, 1.0, 1.0, 1.0 );

   return( SKY_IS_BLUE );
}

static INT Camera_Rd( FILE *F, UINT Size )
{
   FLT Block[ 8 ];
   INT i;

   Cur_Node = New_Object_By_Type( Current_Name, &_CAMERA_, _RCst_.The_World );
   if ( Cur_Node==NULL ) return( MEM_ERROR );
   Current_Name = NULL;
   Cur_Obj = Cur_Node->Data;
   Cur_Camera = (CAMERA *)Cur_Obj;

   READ_M( Block );

   for( i=0; i<8; ++i ) Block[i] = Indy_f( Block[i] );

   Set_Vector( Cur_Camera->Pos, Block[0], Block[1], Block[2] );
   Set_Vector( Cur_Camera->Target, Block[3], Block[4], Block[5] );

   Cur_Camera->Bank = Block[6]*M_PI/180.0f;

   Block[7] *= M_PI/180.0f;                  // Fov = Block[7]
   Cur_Camera->D = Cur_Camera->Lens * (FLT)tan( Block[7]/2.0f );  
//   Cur_Camera->D2 = Cur_Camera->D*Cur_Camera->D;

   return( SKY_IS_BLUE );
}

/******************************************************************/

#ifndef _SKIP_LIGHT_

static INT Light_Rd( FILE *F, UINT Size )
{
   FLT Block[3];
   INT i;

   Cur_Node = New_Object_By_Type( Current_Name, &_LIGHT_, _RCst_.The_World );
   if ( Cur_Node == NULL ) return( MEM_ERROR );
   Current_Name = NULL;
   Cur_Obj = Cur_Node->Data;
   Cur_Light = (LIGHT *)Cur_Obj;
   Cur_Light->Light_Type = LIGHT_POINT;

   READ_M( Block );
   for( i=0; i<3; ++i ) Block[i] = Indy_f( Block[i] );

   Set_Vector( Cur_Light->Pos, Block[0], Block[1], Block[2] );
   Set_Vector( Cur_Light->Dir, 0.0, 0.0, 1.0 );

   Cur_Color_Read = 4;
   DEBUG( Out_Message( "  Light Pos: (%f,%f,%f)", Block[0], Block[1], Block[2] ) );

   return( Chunk_Read( F, Size ) );
}


#ifndef _SKIP_SPOT_

static INT Spot_Light_Rd( FILE *F, UINT Size )
{
   FLT Block[5];
   INT i;

   READ_M( Block );
   for( i=0; i<5; ++i ) Block[i] = Indy_f( Block[i] );

   if ( Cur_Light==NULL )
   {
      Cur_Node = New_Object_By_Type( Current_Name, &_LIGHT_, _RCst_.The_World );
      if ( Cur_Node == NULL ) return( MEM_ERROR );
      Current_Name = NULL;
      Cur_Obj = Cur_Node->Data;
      Cur_Light = (LIGHT *)Cur_Obj;
      Set_Vector( Cur_Light->Pos, 0.0, 0.0, 0.0 );
   }
   Cur_Light->Light_Type = LIGHT_SPOT;     // LIGHT_SPOT ?? 

   Set_Vector( Cur_Light->Dir, Block[0], Block[1], Block[2] );

   Cur_Color_Read = 4;

   Cur_Light->Hot_Spot = Block[3];
   Cur_Light->Fall_Off = Block[4];

   return( SKY_IS_BLUE );
}
#endif   //  _SKIP_SPOT_

static INT Light_ZMin_Rd( FILE *F, UINT Size )
{
   FLT ZMin;
   READ_ONE( ZMin ); ZMin = Indy_f( ZMin );
   if ( Cur_Light!=NULL ) Cur_Light->Z_Min = ZMin;
   return( SKY_IS_BLUE );
}
static INT Light_ZMax_Rd( FILE *F, UINT Size )
{
   FLT ZMax;
   READ_ONE( ZMax ); ZMax = Indy_f( ZMax );
   if ( Cur_Light!=NULL ) Cur_Light->Z_Max = ZMax;
   return( SKY_IS_BLUE );
}
static INT Light_Mult_Rd( FILE *F, UINT Size )
{
   FLT Mult;
   READ_ONE( Mult ); Mult = Indy_f( Mult );
   if ( Cur_Light!=NULL ) Cur_Light->Mult = Mult;
   return( SKY_IS_BLUE );
}
static INT Light_Off_Rd( FILE *F, UINT Size )
{
   if ( Cur_Light!=NULL ) 
    Cur_Light->Flags = (OBJ_FLAG)(Cur_Light->Flags | OBJ_DONT_RENDER);
   return( SKY_IS_BLUE );
}
static INT Light_Attenuation_On_Rd( FILE *F, UINT Size )
{
//   if ( Cur_Light!=NULL ) Cur_Light->Flags |= OBJ_DONT_RENDER;
   return( SKY_IS_BLUE );
}

#endif   // _SKIP_LIGHT_

/******************************************************************/

#ifndef _SKIP_COLORS_

static void Deal_With_Color( FLT *C )
{
   switch( Cur_Color_Read )
   {
      case 0: // nothing
      break;
      case 1: // Cur_Material->Ambient
         Cur_Material->Ambient[0] = C[0]; Cur_Material->Ambient[1] = C[1];
         Cur_Material->Ambient[2] = C[2]; Cur_Material->Ambient[3] = 1.0;
      break;
      case 2: // Cur_Material->Diffuse
         Cur_Material->Diffuse[0] = C[0]; Cur_Material->Diffuse[1] = C[1];
         Cur_Material->Diffuse[2] = C[2]; Cur_Material->Diffuse[3] = 1.0;
      break;
      case 3: // Cur_Material->Specular
         Cur_Material->Specular[0] = C[0]; Cur_Material->Specular[1] = C[1];
         Cur_Material->Specular[2] = C[2]; Cur_Material->Specular[3] = 1.0;
      break;
      case 4:  // Light->Color
         Cur_Light->Color[0] = C[0]; Cur_Light->Color[1] = C[1];
         Cur_Light->Color[2] = C[2]; Cur_Light->Color[3] = 1.0;
      break;
   }
   Cur_Color_Read = 0;
}

static INT RGB_F_Rd( FILE *F, UINT Size )
{
   FLT Block[3];
   INT i;

   READ_M( Block );
   for( i=0; i<3; ++i ) Block[i] = Indy_f( Block[i] );

   DEBUG( Out_Message( "  RGB: (%f,%f,%f)", Block[0], Block[1], Block[2] ) );

   Deal_With_Color( Block );
   return( SKY_IS_BLUE );
}

static INT RGB_B_Rd( FILE *F, UINT Size )
{
   PIXEL Block[3];
   FLT C[3];

   READ_M( Block );

   C[0] = Indy_f( Block[0] )/256.0f;
   C[1] = Indy_f( Block[1] )/256.0f;
   C[2] = Indy_f( Block[2] )/256.0f;

   DEBUG( Out_Message( "  RGB: (%f,%f,%f)", C[0], C[1], C[2] ) );

   Deal_With_Color( C );
   return( SKY_IS_BLUE );
}

static INT Mat_Ambient_Rd( FILE *F, UINT Size )
{
   Cur_Color_Read = 1;
   return( Chunk_Read( F, Size ) );
}
static INT Mat_Diffuse_Rd( FILE *F, UINT Size )
{
   Cur_Color_Read = 2;
   return( Chunk_Read( F, Size ) );
}
static INT Mat_Specular_Rd( FILE *F, UINT Size )
{
   Cur_Color_Read = 3;
   return( Chunk_Read( F, Size ) );
}

#endif // _SKIP_COLORS_


static INT Material_Name_Rd( FILE *F, UINT Size )
{
   char Name[255];

   Read_String( F, Name, 255 );
   DEBUG( Out_Message( "Material name: '%s'...(%d bytes)", Name, Size ) );

   Set_Current_Name( Name );
   Cur_Material = New_Material( );
   if ( Cur_Material==NULL ) return( MEM_ERROR );
   Insert_Object( (OBJECT *)Cur_Material, &_RCst_.The_World->Materials );
   Cur_Material->Name = Current_Name;   
   Cur_Material->Flags = (OBJ_FLAG)(Cur_Material->Flags | OBJ_OWNS_NAME);
   Current_Name = NULL;

   Cur_Material->Map_Nb1 = Cur_Material->Map_Nb2 =
      Cur_Material->Map_Nb3 = Cur_Map_Nb++; // Default
   return( Chunk_Read( F, Size ) );
}

static INT Map_Rd( FILE *F, UINT Size )
{
   char Name[255];

   Read_String( F, Name, 255 );
   DEBUG( Out_Message("Map file: '%s' (%d bytes)", Name, Size ) );

#if 1 // #ifdef DEBUG_CACHE
   Cur_Material->Txt1 = (TEXTURE_MAP *)Str_Dup( Name );
#endif
   return( SKY_IS_BLUE );
}

/******************************************************************/

static INT Frames_Rd( FILE *F, UINT Size )
{
   INT Frames[2];

   READ_M( Frames );
   _RCst_.The_World->Frames_Start = 1.0f*Indy_l( Frames[0] );
   _RCst_.The_World->Frames_End = 1.0f*Indy_l( Frames[1] );

   Cur_Dummy = NULL;   // starts keyframer

   return( SKY_IS_BLUE );
}

static INT Object_Number_Rd( FILE *F, UINT Size )
{
   SHORT n;

   READ_ONE( n );
   Current_ID = Indy_s( n );
   
   return( SKY_IS_BLUE );
}

static INT Track_Obj_Name_Rd( FILE *F, UINT Size )
{
   USHORT W[2], Parent;
   char Name[255];

   Read_String( F, Name, 255 );
   DEBUG( Out_Message("Track Obj name: %s", Name ) );
   READ_M( W );
   W[0] = Indy_s( W[0] );
   W[1] = Indy_s( W[1] );

   READ_ONE( Parent );
   Parent = Indy_s( Parent );
   DEBUG( Out_Message( "   Flags: 0x%X, 0x%X. Parent: %d", W[0], W[1], Parent ) );

   Cur_Obj = NULL;

         // TODO: Do we really need to set Current_ID to Node or Obj!?

   switch( Current_Data )
   {
      case CHK_TRACKMESH:      
         Cur_Node = Search_Node_From_Name( _RCst_.The_World->Root, Name );
         if ( Cur_Node==NULL ) // Dummy object
         {
            DEBUG( Out_Message( "Dummy mesh object '%s'!!!\n", Name ) );
            Set_Current_Name( Name );
            Cur_Node = New_Object_By_Type( Current_Name, &_OBJ_DUMMY_, _RCst_.The_World );
            if ( Cur_Node==NULL ) return( MEM_ERROR );
            Current_Name = NULL;
            Cur_Dummy = Cur_Node;
                // hidden, a priori
            Cur_Node->Flags = (OBJ_FLAG)(Cur_Node->Flags | OBJ_NOT_ACTIVE);
            Cur_Node->Data->Flags = (OBJ_FLAG)(Cur_Node->Data->Flags | OBJ_NOT_ACTIVE);
         }
         else Cur_Dummy = NULL;
      break;

      case CHK_TRACKCAMERA:
         Cur_Node = Search_Node_From_Name( _RCst_.The_World->Root, Name );
         if ( Cur_Node==NULL ) return( UGLY_BUG );
         if ( Cur_Node->Type != CAMERA_TYPE )
         {
            CAMERA *Cam;
            OBJ_NODE *New_Node;
            if ( Cur_Node->Type!=OBJ_DUMMY_TYPE ) return( UGLY_BUG );
            Cur_Obj = Cur_Node->Data;

               // we've got to promote a DUMMY object to camera-type

            New_Node = New_Object_By_Type( Cur_Node->Name, &_CAMERA_, _RCst_.The_World );
            if ( New_Node==NULL ) return( MEM_ERROR );
            Cur_Node->Name = NULL;
            Cam = (CAMERA*)New_Node->Data; 
            Cam->Name = Cur_Obj->Name; Cur_Obj->Name = NULL;
            Cam->Target_k = ((OBJ_DUMMY*)Cur_Obj)->Pos_k;
            ((OBJ_DUMMY*)Cur_Obj)->Pos_k = NULL;
            New_Node->ID = Cur_Node->ID;
            Cam->ID = Cur_Obj->ID;

            Destroy_Node( Cur_Node );
            Cur_Node = New_Node;
         }
         else if ( Cur_Node!=NULL ) ((CAMERA*)Cur_Node->Data)->Dummy = Cur_Dummy;
         Cur_Dummy = NULL;
      break;

      case CHK_TRACKCAMTGT:
         Cur_Node = Search_Node_From_Name( _RCst_.The_World->Root, Name );
         if ( Cur_Node==NULL ) // Dummy object
         {
            DEBUG( Out_Message( "Dummy cam tgt '%s'!!!\n", Name ) );
            Set_Current_Name( Name );
            Cur_Node = New_Object_By_Type( Current_Name, &_OBJ_DUMMY_, _RCst_.The_World );
            if ( Cur_Node==NULL ) return( MEM_ERROR );
            Current_Name = NULL;
            Cur_Dummy = Cur_Node;
            Cur_Node->Flags = (OBJ_FLAG)(Cur_Node->Flags | OBJ_NOT_ACTIVE);
            Cur_Node->Data->Flags = (OBJ_FLAG)(Cur_Node->Data->Flags | OBJ_NOT_ACTIVE);

         }
         else Cur_Dummy = NULL;
      break;

#ifndef _SKIP_LIGHT_

      case CHK_TRACKLIGHT:         
         Cur_Node = Search_Node_From_Name( _RCst_.The_World->Root, Name );
         if ( Cur_Node==NULL ) return( UGLY_BUG );
         if ( Cur_Node->Type != LIGHT_TYPE )
         {
            LIGHT *Light;
            OBJ_NODE *New_Node;
            if ( Cur_Node->Type!=OBJ_DUMMY_TYPE ) return( UGLY_BUG );
            Cur_Obj = Cur_Node->Data;

               // we've got to promote a DUMMY object to light-type

            New_Node = New_Object_By_Type( Cur_Node->Name, &_LIGHT_, _RCst_.The_World );
            if ( New_Node==NULL ) return( MEM_ERROR );
            Cur_Node->Name = NULL;
            Light = (LIGHT*)New_Node->Data; 
            Light->Name = Cur_Obj->Name; Cur_Obj->Name = NULL;
            Light->Target_k = ((OBJ_DUMMY*)Cur_Obj)->Pos_k;
            ((OBJ_DUMMY*)Cur_Obj)->Pos_k = NULL;
            New_Node->ID = Cur_Node->ID;
            Light->ID = Cur_Obj->ID;

            Destroy_Node( Cur_Node );
            Cur_Node = New_Node;
         }
         else if ( Cur_Node!=NULL ) ((LIGHT*)Cur_Node->Data)->Dummy = Cur_Dummy;
         Cur_Dummy = NULL;
      break;

      case CHK_TRACKLIGTGT:
         Cur_Node = Search_Node_From_Name( _RCst_.The_World->Root, Name );
         if ( Cur_Node==NULL ) // Dummy object
         {
            DEBUG( Out_Message( "Dummy light tgt '%s'!!!\n", Name ) );
            Set_Current_Name( Name );
            Cur_Node = New_Object_By_Type( Current_Name, &_OBJ_DUMMY_, _RCst_.The_World );
            if ( Cur_Node==NULL ) return( MEM_ERROR );
            Current_Name = NULL;
            Cur_Dummy = Cur_Node;
            Cur_Node->Flags = (OBJ_FLAG)(Cur_Node->Flags | OBJ_NOT_ACTIVE);
            Cur_Node->Data->Flags = (OBJ_FLAG)(Cur_Node->Data->Flags | OBJ_NOT_ACTIVE);
         }
         else Cur_Dummy = NULL;
      break;

#endif   // _SKIP_LIGHT_

      default: Cur_Node = NULL; return( SKY_IS_BLUE ); 
   }
   if ( Cur_Node==NULL ) return( UGLY_BUG );
   Cur_Node->Transform.Type = TRANSF_QUATERNION;
   Cur_Node->ID = Current_ID;
   Cur_Obj = (OBJECT *)Cur_Node->Data;
   if ( W[0]&0x0800 )   // Hidden obj
   {
      Cur_Node->Flags = (OBJ_FLAG)(Cur_Node->Flags | OBJ_NOT_ACTIVE);
      Cur_Obj->Flags = (OBJ_FLAG)(Cur_Obj->Flags | OBJ_NOT_ACTIVE);
   }

   if ( Parent != 0xFFFF ) // 0xFFFF means none
   {
      OBJ_NODE *Upper;
      Upper = Search_Node_From_ID( _RCst_.The_World->Root, (UINT)Parent );
      if ( (Upper!=NULL) && (Upper!=Cur_Node->Parent) )
      {
         Remove_Node( Cur_Node );
//         Insert_Node( Cur_Node, &Upper->Child, NULL );
//         Cur_Node->Parent = Upper;
         Insert_Child_Node( Cur_Node, Upper, NULL );
         Upper->Flags &= ~OBJ_DONT_RENDER;
         DEBUG ( Out_Message( "Parent's name: '%s'", Upper->Name ) );
      }
   }
   return( SKY_IS_BLUE );
}

static INT Pivot_Rd( FILE *F, UINT Size )
{
   VECTOR Pv;
   INT i;

   READ_M( Pv );
   for( i=0; i<3; ++i ) Pv[i] = Indy_f( Pv[i] );

   if ( Cur_Node!=NULL )
   {
      Add_Vector_Eq( Cur_Node->Transform.Pivot, Pv );
      if ( Cur_Node->Type==MESH_TYPE )
         Sub_Vertex_Eq( (MESH *)Cur_Node->Data, Pv );
      if ( Cur_Node->Parent != NULL )
         Sub_Vector_Eq( Cur_Node->Transform.Pivot, Cur_Node->Parent->Transform.Pivot );
   }
   else DEBUG( Out_Message( "PIVOT for dummy object!!!" ) );

   return( SKY_IS_BLUE );
}

/******************************************************************/

static INT Spline_Flags( FILE *F, USHORT Flags, A_KEY *K )
{
   INT i;
   FLT Data;

   K->Tens = K->Cont = K->Bias = 0.0;
   K->Ease_In = K->Ease_Out = 0.0;

   for( i=0; i<16; ++i )
   {
      if ( !(Flags&(1<<i) ) ) continue;
      READ_ONE( Data ); Data = Indy_f( Data );
      switch( i )
      {
         case 0: K->Tens = Data; break;
         case 1: K->Cont = Data; break;
         case 2: K->Bias = Data; break;
         case 3: K->Ease_In = Data; break;
         case 4: K->Ease_Out = Data; break;
      }
   }
   return( SKY_IS_BLUE );
}

static void Track_Decode_Flags( USHORT *Flags, SPLINE_3DS *Sp )
{
//   Sp->Flags &= (TRACK_DUMMY_C|TRACK_DUMMY_L|TRACK_DUMMY_M);    // Clear all 
   switch ( Flags[0]&0x03 )
   {
      case 0x02: Sp->Flags |= TRACK_REPEAT; break;
      case 0x03: Sp->Flags |= TRACK_LOOP; break;
   }
}

/******************************************************************/

static INT Read_Track_Vector( FILE *F, SPLINE_3DS *Sp )
{
   INT i;

   for( i=0; i<Sp->Nb_Keys; ++i )
   {
      INT Frame, j;
      USHORT Flags;

      READ_ONE( Frame );
      Sp->Keys[i].Frame = (FLT)( Indy_l( Frame ) );

      READ_ONE( Flags ); Flags = Indy_s( Flags );
      Spline_Flags( F, Flags, &Sp->Keys[i] );
      for( j=0; j<Sp->Data_Size; ++j )
      {
         FLT Tmp;
         READ_ONE( Tmp );
         Sp->Keys[i].Values[j] = Indy_f( Tmp );
      }
   }
   Cur_Node->Flags &= ~OBJ_TRACK_OK;
   Cur_Node->Flags |= OBJ_DO_ANIM;
   return( SKY_IS_BLUE );
}

static INT Read_Track_Quat( FILE *F, SPLINE_3DS *Sp )
{
   INT i;
//   FLT Fact, dA;

   // Fact = 1.0; dA = 0;
   for( i=0; i<Sp->Nb_Keys; ++i )
   {
      INT Frame;
      USHORT Flags;
      FLT Rot[4];

      READ_ONE( Frame ); 
      Sp->Keys[i].Frame = (FLT)( Indy_l( Frame ) );

      READ_ONE( Flags ); Flags = Indy_s( Flags );
      Spline_Flags( F, Flags, &Sp->Keys[i] );

      READ_M( Rot );
      Rot[0] = Indy_f( Rot[0] );
      Rot[0] = -Rot[0];       // <= ?!??? 3ds suxxXXXX

      Rot[1] = Indy_f( Rot[1] );
      Rot[2] = Indy_f( Rot[2] );
      Rot[3] = Indy_f( Rot[3] );

//      dA = Rot[0]-dA;
      if ( Theta_V_To_Quaternion( Sp->Keys[i].Values, Rot ) != 0.0 )
      {
//         if ( fabs(dA)>M_PI ) Fact *= -1;
         if ( i>0 ) Mult_Quat_Eq( Sp->Keys[i].Values, Sp->Keys[i-1].Values );
//         Sp->Keys[i].Values[0] *= Fact;
//         Sp->Keys[i].Values[1] *= Fact;
//         Sp->Keys[i].Values[2] *= Fact;
//         Sp->Keys[i].Values[3] *= Fact;         
      }
      else if ( i>0 )
      {
         Sp->Keys[i].Values[0] = Sp->Keys[i-1].Values[0];
         Sp->Keys[i].Values[1] = Sp->Keys[i-1].Values[1];
         Sp->Keys[i].Values[2] = Sp->Keys[i-1].Values[2];
         Sp->Keys[i].Values[3] = Sp->Keys[i-1].Values[3];
      }
//      dA = Rot[0];
   }
   Cur_Node->Flags &= ~OBJ_TRACK_OK;
   Cur_Node->Flags |= OBJ_DO_ANIM;
   return( SKY_IS_BLUE );
}

/******************************************************************/

static INT Track_Pos_Rd( FILE *F, UINT Size )
{
   INT n, i;
   USHORT Unknown[5];
   SPLINE_3DS *Sp, **Sp_Ptr;

   // if ( Cur_Node==NULL ) return( SKY_IS_BLUE );

   READ_M( Unknown );
   for( i=0; i<5; ++i ) Unknown[i] = Indy_s( Unknown[i] );
   READ_ONE( n ); n = Indy_l( n );        // Short instead ?!

   switch( Current_Data )
   {
      case CHK_TRACKMESH:
         if ( Cur_Node->Type == OBJ_DUMMY_TYPE )
            Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->Pos_k;
         else Sp_Ptr = &((MESH*)Cur_Obj)->Pos_k;
      break;

      case CHK_TRACKCAMERA:
         Sp_Ptr = &((CAMERA*)Cur_Obj)->Pos_k;
         ((CAMERA*)Cur_Obj)->Dummy = Cur_Dummy;
      break;
      case CHK_TRACKCAMTGT:
         if ( Cur_Node->Type == OBJ_DUMMY_TYPE )
            Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->Pos_k;
         else Sp_Ptr = &((CAMERA*)Cur_Obj)->Target_k;
      break;

      case CHK_TRACKLIGHT:
         if ( Cur_Node->Type == OBJ_DUMMY_TYPE )
            return( SKY_IS_BLUE );  // !!
//            Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->Pos_k;
         else
         {
            Sp_Ptr = &((LIGHT*)Cur_Obj)->Pos_k;
            ((LIGHT*)Cur_Obj)->Dummy = Cur_Dummy;
         }
      break;
      case CHK_TRACKLIGTGT:
         if ( Cur_Node->Type == OBJ_DUMMY_TYPE )
            Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->Pos_k;
         else Sp_Ptr = &((LIGHT*)Cur_Obj)->Target_k;
      break;
      default: return( UGLY_BUG );
   }

   Sp = New_Spline_3ds( Sp_Ptr, 3, n );
   if ( Sp==NULL ) return( MEM_ERROR );
   Track_Decode_Flags( Unknown, Sp );

   if ( Read_Track_Vector( F, Sp ) ) return( IO_ERROR );

   for( i=0; i<n; ++i )    // Sub-pivot offset
   {
      Sub_Vector_Eq( Sp->Keys[i].Values, Cur_Node->Transform.Pivot );
   }

   return( SKY_IS_BLUE );   
}

static INT Track_Rot_Rd( FILE *F, UINT Size )
{
   INT n, i;
   USHORT Unknown[5];

   SPLINE_3DS *Sp, **Sp_Ptr;

   if ( Cur_Node==NULL ) return( SKY_IS_BLUE );

   READ_M( Unknown );
   for( i=0; i<5; ++i ) Unknown[i] = Indy_s( Unknown[i] );
   READ_ONE( n ); n = Indy_l( n );  /* Short instead ?! */

   switch( Cur_Node->Type )
   {
      case MESH_TYPE:
         Sp_Ptr = &((MESH*)Cur_Obj)->Rot_k;
      break;
      case GROUP_TYPE:
         Sp_Ptr = &((GROUP*)Cur_Obj)->Rot_k;
      break;
      case OBJ_DUMMY_TYPE:
         Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->Rot_k;
      break;
      case LIGHT_TYPE:
         Sp_Ptr = &((LIGHT*)Cur_Obj)->Rot_k;
      break;
      default: return( UGLY_BUG );
   }

   Sp = New_Spline_3ds( Sp_Ptr, 4, n );
   if ( Sp==NULL ) return( MEM_ERROR );

   Track_Decode_Flags( Unknown, Sp );

   if ( Read_Track_Quat( F, Sp ) ) return( IO_ERROR );

   Cur_Node->Transform.Type = TRANSF_QUATERNION;
   return( SKY_IS_BLUE );   
}

static INT Track_Scale_Rd( FILE *F, UINT Size )
{
   INT n, i;
   USHORT Unknown[5];
   SPLINE_3DS *Sp, **Sp_Ptr;

   if ( Cur_Node==NULL ) return( SKY_IS_BLUE );
   READ_M( Unknown );
   for( i=0; i<5; ++i ) Unknown[i] = Indy_s( Unknown[i] );
   READ_ONE( n ); n = Indy_l( n );  /* Short instead ?! */
   DEBUG( Out_Message("Scale keys:%d", n ) );

   switch( Cur_Node->Type )
   {
      case MESH_TYPE:
         Sp_Ptr = &((MESH*)Cur_Obj)->Scale_k;
      break;
      case GROUP_TYPE:
         Sp_Ptr = &((GROUP*)Cur_Obj)->Scale_k;
      break;
      case OBJ_DUMMY_TYPE:
         Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->Scale_k;
      break;
      case CAMERA_TYPE: return( UGLY_BUG ); 
      default: return( UGLY_BUG );  
   }

   Sp = New_Spline_3ds( Sp_Ptr, 3, n );
   if ( Sp==NULL ) return( MEM_ERROR );

   Track_Decode_Flags( Unknown, Sp );

   if ( Read_Track_Vector( F, Sp ) ) return( IO_ERROR );

   return( SKY_IS_BLUE );   
}

/******************************************************************/

static INT Read_Degree_Track( SPLINE_3DS **Sp_Ptr, INT Nb, USHORT *Unknown, FILE *F )
{
   SPLINE_3DS *Sp;
   INT i;

   Sp = New_Spline_3ds( Sp_Ptr, 1, Nb );
   if ( Sp==NULL ) return( MEM_ERROR );

   Track_Decode_Flags( Unknown, Sp );

   if ( Read_Track_Vector( F, Sp ) ) return( IO_ERROR );

   for( i=0; i<Nb; ++i ) Sp->Keys[i].Values[0] *= M_PI/180.0f;

   return( SKY_IS_BLUE );
}

static INT Track_FOV_Rd( FILE *F, UINT Size )
{
   INT n, i;
   USHORT Unknown[5];
   SPLINE_3DS **Sp_Ptr;

   if ( Cur_Node==NULL ) return( SKY_IS_BLUE );
   READ_M( Unknown );
   for( i=0; i<5; ++i ) Unknown[i] = Indy_s( Unknown[i] );
   READ_ONE( n ); n = Indy_l( n );  // Short instead ?!

   switch( Cur_Node->Type )
   {
      case CAMERA_TYPE:
         Sp_Ptr = &((CAMERA*)Cur_Obj)->FOV_k;
      break;
      case OBJ_DUMMY_TYPE:
         Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->FOV_k;   // Hack!!
      break;
      default: return( UGLY_BUG );  
   }
   
   return( Read_Degree_Track( Sp_Ptr, n, Unknown, F ) );
}


static INT Track_Roll_Rd( FILE *F, UINT Size )
{
   INT n;
   USHORT Unknown[5];
   SPLINE_3DS **Sp_Ptr;

   if ( Cur_Node==NULL ) return( SKY_IS_BLUE );
   READ_M( Unknown );
   for( n=0; n<5; ++n ) Unknown[n] = Indy_s( Unknown[n] );
   READ_ONE( n ); n = Indy_l( n );  /* Short instead ?! */

   switch( Cur_Node->Type )
   {
      case CAMERA_TYPE:
         Sp_Ptr = &((CAMERA*)Cur_Obj)->Roll_k;
      break;
      case LIGHT_TYPE:
         Sp_Ptr = &((LIGHT*)Cur_Obj)->Roll_k;
      break;
      case OBJ_DUMMY_TYPE:
         Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->Roll_k; // Hack!!!
      break;
      case MESH_TYPE: return( UGLY_BUG ); 
      default: return( UGLY_BUG );  
   }

   return( Read_Degree_Track( Sp_Ptr, n, Unknown, F ) );
}

#ifndef _SKIP_SPOT_

static INT Track_Fall_Off_Rd( FILE *F, UINT Size )
{
   INT n;
   USHORT Unknown[5];
   SPLINE_3DS **Sp_Ptr;

   if ( Cur_Node==NULL ) return( SKY_IS_BLUE );
   READ_M( Unknown );
   for( n=0; n<5; ++n ) Unknown[n] = Indy_s( Unknown[n] );
   READ_ONE( n ); n = Indy_l( n );  /* Short instead ?! */

   switch( Cur_Node->Type )
   {
      case LIGHT_TYPE:
         Sp_Ptr = &((LIGHT*)Cur_Obj)->Falloff_k;
         ((LIGHT*)Cur_Obj)->Light_Type = LIGHT_SPOT;
      break;
      case OBJ_DUMMY_TYPE: return( SKY_IS_BLUE );
      default: return( UGLY_BUG ); 
   }
   return( Read_Degree_Track( Sp_Ptr, n, Unknown, F ) );
}

#endif   // _SKIP_SPOT_

#ifndef _SKIP_SPOT_

static INT Track_Hot_Spot_Rd( FILE *F, UINT Size )
{
   INT n;
   USHORT Unknown[5];
   SPLINE_3DS **Sp_Ptr;

   if ( Cur_Node==NULL ) return( SKY_IS_BLUE );
   READ_M( Unknown );
   for( n=0; n<5; ++n ) Unknown[n] = Indy_s( Unknown[n] );
   READ_ONE( n ); n = Indy_l( n );  /* Short instead ?! */

   switch( Cur_Node->Type )
   {
      case LIGHT_TYPE:
         Sp_Ptr = &((LIGHT*)Cur_Obj)->Hotspot_k;
         ((LIGHT*)Cur_Obj)->Light_Type = LIGHT_SPOT;
      break;
      case OBJ_DUMMY_TYPE: return( SKY_IS_BLUE );
      default: return( UGLY_BUG ); 
   }
   return( Read_Degree_Track( Sp_Ptr, n, Unknown, F ) );
}
#endif   //  _SKIP_SPOT_

#ifndef _SKIP_COLORS_

static INT Track_Color_Rd( FILE *F, UINT Size )
{
   INT n;
   USHORT Unknown[5];
   SPLINE_3DS *Sp, **Sp_Ptr;

   if ( Cur_Node==NULL ) return( SKY_IS_BLUE );

   READ_M( Unknown );
   for( n=0; n<5; ++n ) Unknown[n] = Indy_s( Unknown[n] );
   READ_ONE( n ); n = Indy_l( n );        // Short instead ?!

   switch( Current_Data )
   {
      case CHK_TRACKLIGHT: case CHK_TRACKLIGTGT:
         if ( Cur_Node->Type == OBJ_DUMMY_TYPE )
            return( SKY_IS_BLUE );
            // Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->Color_k;
         else Sp_Ptr = &((LIGHT*)Cur_Obj)->Color_k;
      break;
      default: return( UGLY_BUG );
   }

   Sp = New_Spline_3ds( Sp_Ptr, 3, n );
   if ( Sp==NULL ) return( MEM_ERROR );
   Track_Decode_Flags( Unknown, Sp );

      // read 3 floats between 0.0 and 1.0 per color

   if ( Read_Track_Vector( F, Sp ) ) return( IO_ERROR );
   return( SKY_IS_BLUE );   
}

#endif   // _SKIP_COLORS_

/******************************************************************/

#ifndef _SKIP_MORPH_

static INT Track_Morph_Rd( FILE *F, UINT Size )
{
   INT n, i, Need_Copy;
   USHORT Unknown[5];
   SPLINE_3DS *Sp, **Sp_Ptr;

   if ( Cur_Node==NULL ) return( SKY_IS_BLUE );
   READ_M( Unknown );
   for( n=0; n<5; ++n ) Unknown[n] = Indy_s( Unknown[n] );
   READ_ONE( n ); n = Indy_l( n );  /* Short instead ?! */
   DEBUG(  Out_Message("Morph keys:%d", n ) );

   switch( Cur_Node->Type )
   {
      case MESH_TYPE:
         Sp_Ptr = &((MESH*)Cur_Obj)->Morph_k;
      break;
      case OBJ_DUMMY_TYPE:
         Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->Morph_k; // Hack!!!
      break;
      default: return( UGLY_BUG );  
   }
   
   Sp = New_Spline_3ds( Sp_Ptr, 1, n );
   if ( Sp==NULL ) return( MEM_ERROR );

   Track_Decode_Flags( Unknown, Sp );

   Cur_Msh = (MESH *)Cur_Obj;
   M_Free( Cur_Msh->Morph_Node );
   M_Free( Cur_Msh->Vertex_Copy );
   M_Free( Cur_Msh->Normals_Copy );

   Cur_Msh->Morph_Node = New_Fatal_Object( n, OBJ_NODE* );
   Need_Copy = FALSE;

   for( i=0; i<n; ++i )
   {
      INT Frame;
      USHORT Flags;
      char Name[256];
      OBJ_NODE *Node;

      READ_ONE( Frame );
      Sp->Keys[i].Frame = (FLT)( Indy_l( Frame ) );

      READ_ONE( Flags ); Flags = Indy_s( Flags );
      Spline_Flags( F, Flags, &Sp->Keys[i] );

      Sp->Keys[i].Values[0] = 1.0f*i;
      if ( Read_String( F, Name, 256 )==0 ) return( UGLY_BUG );
      Node = Search_Node_From_Name( _RCst_.The_World->Root, Name );
      if ( Node==NULL || Node->Type != MESH_TYPE ) return( UGLY_BUG );
      Cur_Msh->Morph_Node[i] = Node;

      if ( Node==Cur_Node ) Need_Copy = TRUE;
      else
      {
         MESH *Tmp;
         Node->Flags |= OBJ_DONT_RENDER;
         Tmp = (MESH*)Node->Data;
         M_Free( Tmp->UV );
         M_Free( Tmp->Edges );
         M_Free( Tmp->Polys );
         Tmp->Nb_Polys = Tmp->Nb_Edges = 0;
         Tmp->Flags = (OBJ_FLAG)( Tmp->Flags | OBJ_HAS_UV );    // Fake!!
         Tmp->Flags = (OBJ_FLAG)( Tmp->Flags | OBJ_DONT_OPTIMIZ );
      }
      Node->Flags |= OBJ_DONT_OPTIMIZ;
   }
   Cur_Msh->Flags = (OBJ_FLAG)( Cur_Msh->Flags | OBJ_DONT_OPTIMIZ );
   Cur_Msh->Morph_Flags = MESH_MORPH_DEFAULT_FLAGS;

   if ( Need_Copy )
   {
      Cur_Msh->Vertex_Copy = New_Fatal_Object( Cur_Msh->Nb_Vertex, VECTOR );
      Cur_Msh->Normals_Copy = New_Fatal_Object( Cur_Msh->Nb_Vertex, VECTOR );
      memcpy( Cur_Msh->Vertex_Copy, Cur_Msh->Vertex, Cur_Msh->Nb_Vertex*sizeof( VECTOR ) );
      memcpy( Cur_Msh->Normals_Copy, Cur_Msh->Normals, Cur_Msh->Nb_Vertex*sizeof( VECTOR ) );
   }
   Cur_Node->Flags &= ~OBJ_TRACK_OK;
   Cur_Node->Flags |= OBJ_DO_ANIM;

   return( SKY_IS_BLUE );   
}

#endif   //  _SKIP_MORPH_

/******************************************************************/

#ifndef _SKIP_HIDE_

static INT Track_Hide_Rd( FILE *F, UINT Size )
{
   INT n, i, Hide;
   USHORT Unknown[5];
   SPLINE_3DS *Sp, **Sp_Ptr;

   if ( Cur_Node==NULL ) return( SKY_IS_BLUE );
   READ_M( Unknown );
   for( n=0; n<5; ++n ) Unknown[n] = Indy_s( Unknown[n] );
   READ_ONE( n ); n = Indy_l( n );  /* Short instead ?! */
   DEBUG(  Out_Message("Hide keys:%d", n ) );

   switch( Cur_Node->Type )
   {
      case MESH_TYPE:
         Sp_Ptr = &((MESH*)Cur_Obj)->Hide_k;
      break;
      case OBJ_DUMMY_TYPE:
         Sp_Ptr = &((OBJ_DUMMY*)Cur_Obj)->Hide_k;
      break;
      case GROUP_TYPE:
         Sp_Ptr = &((GROUP*)Cur_Obj)->Hide_k;
      break;
      case LIGHT_TYPE:
         Sp_Ptr = &((LIGHT*)Cur_Obj)->Hide_k;
      break;
      default: return( UGLY_BUG ); 
   }
   
   Sp = New_Spline_3ds( Sp_Ptr, 1, n );
   if ( Sp==NULL ) return( MEM_ERROR );

   Track_Decode_Flags( Unknown, Sp );

   if ( Cur_Node->Flags & OBJ_NOT_ACTIVE ) Hide = TRUE;
   else Hide = FALSE;

   for( i=0; i<n; ++i )
   {
      INT Frame;
      USHORT Flags;

      READ_ONE( Frame );
      Sp->Keys[i].Frame = (FLT)( Indy_l( Frame ) );

      READ_ONE( Flags ); Flags = Indy_s( Flags );
      Spline_Flags( F, Flags, &Sp->Keys[i] );

      Sp->Keys[i].Values[0] = 1.0f*Hide;
      Hide ^= 0x01;
   }
   Cur_Node->Flags &= ~OBJ_TRACK_OK;
   Cur_Node->Flags |= OBJ_DO_ANIM;

   return( SKY_IS_BLUE );   
}

#endif   // _SKIP_HIDE_

/******************************************************************/

static INT Track_Mesh( FILE *F, UINT Size )
{
   DEBUG( Out_Message( "\n Current_Data set to: CHK_TRACKMESH" ) );
   Current_Data = CHK_TRACKMESH;
   return( Chunk_Read( F, Size ) );
}
static INT Track_Camera( FILE *F, UINT Size )
{
   DEBUG( Out_Message( "\n Current_Data set to: CHK_TRACKCAMERA" ) );
   Current_Data = CHK_TRACKCAMERA;
   return( Chunk_Read( F, Size ) );
}
static INT Track_Camera_Target( FILE *F, UINT Size )
{
   DEBUG( Out_Message( "\n Current_Data set to: CHK_TRACKCAMTGT" ) );
   Current_Data = CHK_TRACKCAMTGT;
   return( Chunk_Read( F, Size ) );
}

#ifndef _SKIP_LIGHT_

static INT Track_Light( FILE *F, UINT Size )
{
   DEBUG( Out_Message( "\n Current_Data set to: CHK_TRACKLIGHT" ) );
   Current_Data = CHK_TRACKLIGHT;
   return( Chunk_Read( F, Size ) );
}
static INT Track_Light_Target( FILE *F, UINT Size )
{
   DEBUG( Out_Message( "\n Current_Data set to: CHK_TRACKLIGTGT" ) );
   Current_Data = CHK_TRACKLIGTGT;
   return( Chunk_Read( F, Size ) );
}
#endif   //_SKIP_LIGHT_

#ifndef _SKIP_COLORS_

static INT Track_Ambient_Rd( FILE *F, UINT Size )
{
   DEBUG ( "\n Current_Data set to: CHK_TRACKAMB\n" );
   Current_Data = CHK_TRACKAMB;
   return( Chunk_Read( F, Size ) );
}

#endif   // _SKIP_COLORS_

static INT Track_Unknown( FILE *F, UINT Size )
{
   DEBUG( Out_Message( "\n Current_Data set to: CHK_UNKNOWN" ) );
   Current_Data = CHK_UNKNOWN;
   return( Chunk_Read( F, Size ) );
}

/******************************************************************/

#ifndef _SKIP_MISC_

static INT Morph_Smooth_Rd( FILE *F, UINT Size )
{
   FLT Smooth;

   READ_ONE( Smooth ); Smooth = Indy_f( Smooth );
//   DEBUG( Out_Message( "  Morph Smooth: %f", Smooth ) );
   return( SKY_IS_BLUE );
}

static INT One_Unit_Rd( FILE *F, UINT Size )
{
   FLT Unit;

   READ_ONE( Unit ); Unit = Indy_f( Unit );
   DEBUG( Out_Message( "  One unit: %f", Unit ) );
   return( SKY_IS_BLUE );
}

static INT U_Scale_Rd( FILE *F, UINT Size )
{
   FLT U_Scale;

   READ_ONE( U_Scale ); U_Scale = Indy_f( U_Scale );
   DEBUG( Out_Message( "  U_Scale: %f", U_Scale ) );
   return( SKY_IS_BLUE );
}
static INT V_Scale_Rd( FILE *F, UINT Size )
{
   FLT V_Scale;

   READ_ONE( V_Scale ); V_Scale = Indy_f( V_Scale );
   DEBUG( Out_Message( "  V_Scale: %f", V_Scale ) );
   return( SKY_IS_BLUE );
}
static INT U_Offset_Rd( FILE *F, UINT Size )
{
   FLT U_Offset;

   READ_ONE( U_Offset ); U_Offset = Indy_f( U_Offset );
   DEBUG( Out_Message( "  U_Offset: %f", U_Offset ) );
   return( SKY_IS_BLUE );
}
static INT V_Offset_Rd( FILE *F, UINT Size )
{
   FLT V_Offset;

   READ_ONE( V_Offset ); V_Offset = Indy_f( V_Offset );
   DEBUG( Out_Message( "  V_Offset: %f", V_Offset ) );
   return( SKY_IS_BLUE );
}

static INT UV_Rot_Rd( FILE *F, UINT Size )
{
   FLT UV_Rot;

   READ_ONE( UV_Rot ); UV_Rot = Indy_f( UV_Rot );
   DEBUG( Out_Message( "  UV_Rot: %f", UV_Rot ) );
   return( SKY_IS_BLUE );
}

#endif   // _SKIP_MISC_

/******************************************************************/

static struct {

    CHUNK_TYPE ID;
    INT (*Reader)(FILE *, UINT);

} Chunk_Names[] = {

#ifndef _SKIP_MISC_
   { CHK_ONE_UNIT,    One_Unit_Rd },
   { CHK_VSCALE,      V_Scale_Rd },
   { CHK_USCALE,      U_Scale_Rd },
   { CHK_UOFFSET,     U_Offset_Rd },
   { CHK_VOFFSET,     V_Offset_Rd },
   { CHK_UV_ROT,      UV_Rot_Rd },
//   { CHK_MESHVRSN,   Mesh_Version_Rd },
#endif _SKIP_MISC_

   { CHK_MAIN,        NULL },
   { CHK_OBJMESH,     NULL },
   { CHK_OBJBLOCK,    Obj_Block_Rd },
#ifndef _SKIP_HIDE_
   { CHK_OBJHIDDEN,   Obj_Hidden_Rd },
#endif

#ifndef _SKIP_SHADOWS_
   { CHK_OBJNOCAST,   Obj_No_Cast_Rd },
//   { CHK_OBJ_MATTE, Obj_Matte_Rd },
   { CHK_OBJNORCV,    Obj_No_Receive_Shadow_Rd },
#endif   // _SKIP_SHADOWS_

   { CHK_TRIMESH,     Obj_Tri_Mesh },
   { CHK_VERTLIST,    Vert_List_Rd },
   { CHK_FACELIST,    Poly_List_Rd },
   { CHK_FACEMAT,     Face_Mat_Rd },
   { CHK_MAPLIST,     Mapping_Rd },
   { CHK_TRMATRIX,    Tr_Matrix_Rd },
   { CHK_CAMERA,      Camera_Rd },
   { CHK_SMOOLIST,    Smooth_List_Rd },
#ifndef _SKIP_LIGHT_
   { CHK_LIGHT,       Light_Rd },
#ifndef _SKIP_SPOT_
   { CHK_SPOTLIGHT,   Spot_Light_Rd },
#endif
   { CHK_LGHTZMIN,    Light_ZMin_Rd },
   { CHK_LGHTZMAX,    Light_ZMax_Rd },
   { CHK_LGHTMULT,    Light_Mult_Rd },
   { CHK_LIGHT_OFF,   Light_Off_Rd },
   { CHK_LGHT_ATTN,   Light_Attenuation_On_Rd },
#endif   // _SKIP_LIGHT_
#ifndef _SKIP_COLORS_
   { CHK_RGB_B,       RGB_B_Rd },
   { CHK_RGB_F,       RGB_F_Rd },
#endif   // _SKIP_COLORS_
   { CHK_TEXTURE,     NULL },
   { CHK_MATERIAL,    NULL },
   { CHK_MATNAME,     Material_Name_Rd },
   { CHK_MAPFILE,     Map_Rd },
#ifndef _SKIP_COLORS_
   { CHK_AMBIENT,     Mat_Ambient_Rd },
   { CHK_DIFFUSE,     Mat_Diffuse_Rd },
   { CHK_SPECULAR,    Mat_Specular_Rd },
#endif   // _SKIP_COLORS_

   { CHK_NODE_ID,     Object_Number_Rd },
   { CHK_KEYFRAMER,   NULL },
   { CHK_PIVOT,       Pivot_Rd },

   { CHK_FRAMES,       Frames_Rd },
   { CHK_NODE_HDR,     Track_Obj_Name_Rd },
   { CHK_INSTANCE_NAME, Instance_Name_Rd },
#ifndef _SKIP_MISC_
   { CHK_MORPH_SMOOTH, Morph_Smooth_Rd },
#endif   // _SKIP_MISC_
   { CHK_TRACKMESH,    Track_Mesh },
   { CHK_TRACKCAMERA,  Track_Camera },
   { CHK_TRACKCAMTGT,  Track_Camera_Target },
#ifndef _SKIP_LIGHT_
   { CHK_TRACKLIGHT,   Track_Light },
   { CHK_TRACKLIGTGT,  Track_Light_Target },
   { CHK_TRACKSPOTL,   Track_Light },
#endif   //_SKIP_LIGHT_

#ifndef _SKIP_MORPH_
   { CHK_TRACKMORPH,   Track_Morph_Rd },
#endif
#ifndef _SKIP_HIDE_
   { CHK_TRACKHIDE,    Track_Hide_Rd },
#endif
#ifndef _SKIP_COLORS_
   { CHK_TRACKAMB,     Track_Ambient_Rd },
#endif   // _SKIP_COLORS_

#ifndef _SKIP_SPOT_
   { CHK_TRACKFALLOFF, Track_Fall_Off_Rd },
   { CHK_TRACKHOTSPOT, Track_Hot_Spot_Rd },
#endif
   { CHK_TRACKPOS,     Track_Pos_Rd },
   { CHK_TRACKROTATE,  Track_Rot_Rd },
   { CHK_TRACKSCALE,   Track_Scale_Rd },
   { CHK_TRACKFOV,     Track_FOV_Rd },
   { CHK_TRACKROLL,    Track_Roll_Rd },
#ifndef _SKIP_COLORS_
   { CHK_TRACKCOLOR,   Track_Color_Rd }
#endif   // _SKIP_COLORS_
};

/******************************************************************/

static INT Find_Chunk_From_ID( CHUNK_TYPE ID )
{
   int i;

   for( i=0; i<sizeof( Chunk_Names )/sizeof( Chunk_Names[0] ); i++ )
      if ( ID == Chunk_Names[i].ID )
         return i;
   return( -1 );
}

EXTERN INT Chunk_Read( FILE *F, UINT Final_Pos )
{
   USHORT ID;
   INT    Len;
   INT    Err, N;
   UINT   Pos;

   // while( ( Pos=ftell( F ) )<Final_Pos )
   while( ( Pos=F_TELL( F ) )<Final_Pos )
   {
      READ_ONE( ID ); ID  = Indy_s( ID );
      READ_ONE( Len ); Len = Indy_l( Len );

      N = Find_Chunk_From_ID( (CHUNK_TYPE)ID );
      if ( N==-1 )
      {
         DEBUG( Out_Message("Bad chunk ID #0x%.4x (Pos:%d, Len:%d)",
            ID, Pos, Len ) );
         if ( Len<2 ) return( SKY_IS_BLUE );
         F_SEEK( F, Pos + Len, SEEK_SET );
         continue;
      }
      Pos += Len;
      if ( Chunk_Names[N].Reader != NULL )
         Err = (*Chunk_Names[N].Reader)( F, Pos );
      else Err = Chunk_Read( F, Pos );
      F_SEEK( F, Pos, SEEK_SET );
      if ( Err )
         return( Err );
      if ( ferror( F ) ) return( IO_ERROR );
   }
   return( SKY_IS_BLUE );                           
}

/******************************************************************/

EXTERN WORLD *Load_World( STRING Name )
{
   FILE *In;
   UINT FSize;
   INT Err;

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

   _RCst_.The_World = Create_World( );

   Obj_Node_ID = 0xFFFF;     // so IDs are reconstructed right
   Cur_Node = NULL;
   Cur_Obj = NULL;
   Cur_Dummy = NULL;
   Cur_Light = NULL;
   Current_Data = CHK_MAIN;
   Cur_Material = NULL;
   Cur_Map_Nb = 0;

   FSize = F_SIZE( In );
   DEBUG( Out_Message("File: %s (size:%d)", Name, FSize ) );

   Err = Chunk_Read( In, FSize );

   DEBUG( Out_Message( "\n=> Ok - Err=%d\n", Err ) );

   if ( Err )
   {
      Destroy_World( _RCst_.The_World );
      return( NULL );
   }

   DEBUG( Print_World_Info( _RCst_.The_World ) );
   // Set_Up_World( _RCst_.The_World );
   return( _RCst_.The_World );
}

/******************************************************************/
