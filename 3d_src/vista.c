/***********************************************
 *       Vista  (hasta la..)                   *
 *                                             *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

#ifndef _SKIP_VISTA_

#include "indy.h"

/*********************************************************************/

EXTERN void Destroy_Vista_Chunk( VISTA_CHUNK *V )
{
   if ( V==NULL ) return;
   M_Free( V->Obj_ID );
   M_Free( V->Poly_ID );
   V->Nb_Polys = 0;
}

EXTERN VISTA_CHUNK *New_Vista_Chunk( INT Nb_Polys )
{
   VISTA_CHUNK *New;
   New = New_Fatal_Object( 1, VISTA_CHUNK );
   if ( New==NULL ) goto Failed;
   New->Time = 0.0;
   New->Type = VISTA_UNKNOWN;
   New->Next = NULL;
   New->Z_Min = _HUGE_;
   New->Z_Max =-_HUGE_;
   if ( Nb_Polys )
   {
      New->Obj_ID = New_Fatal_Object( Nb_Polys, USHORT );
      New->Poly_ID = New_Fatal_Object( Nb_Polys, USHORT );
      if ( New->Poly_ID==NULL || New->Obj_ID==NULL ) goto Failed;
   }
   New->Nb_Polys = Nb_Polys;

   return( New );

Failed:
   Destroy_Vista_Chunk( New );
   M_Free( New );
   return( NULL );
}

/*********************************************************************/

EXTERN void Destroy_Vista( VISTA *V )
{
   VISTA_CHUNK *Next;
   if ( V==NULL ) return;
   M_Free( V->Poly_ID );
   M_Free( V->Obj_ID );
   Next = V->Chunks;
   while ( Next!=NULL )
   {
      VISTA_CHUNK *Cur;
      Cur = Next;
      Next = Cur->Next;
      Destroy_Vista_Chunk( Cur );
      M_Free( Cur );
   }
} 

/*********************************************************************/

EXTERN void Select_Vista( VISTA *Vista )
{
   WORLD_REAL_RSC Rsc;

   Mem_Clear( &Rsc );
   Rsc.Poly_Keys_Sz = Vista->Max_Polys;
   Rsc.P_Vertex_Sz = Vista->Max_Polys*MAX_POLY_PTS;   // ?!?!
   Rsc.Sorted_Sz = Vista->Max_Polys*MAX_POLY_PTS;
   Rsc.Vertex_State_Sz = Vista->Max_Polys*MAX_POLY_PTS;
   if ( World_Check_Resources( &Rsc ) == NULL )
   { 
      Cur_Vista = NULL; 
      return;
   }
   M_Free( Vista->Poly_ID );
   Vista->Poly_ID = New_Fatal_Object( Vista->Max_Polys, USHORT );
   M_Free( Vista->Obj_ID );
   Vista->Obj_ID = New_Fatal_Object( Vista->Max_Polys, USHORT );
   Cur_Vista = Vista;
}

EXTERN VISTA *Select_Vista_World( WORLD *W, OBJ_NODE *Cam_Node )
{
   CAMERA *Cam;

   if ( W->State == 0x00 ) Set_Up_World( W );
   World_Clean_Resources( );

   if ( Cam_Node==NULL ) return( NULL );
   Cam = (CAMERA *)Cam_Node->Data;
   if ( Cam->Vista==NULL ) return( NULL );

   Select_Vista( Cam->Vista );
   Init_Node_Ptr( W );
   Cam->Vista->Cur_Time = _HUGE_;   // will force chunk rewind

   Finish_World_Init( W );

   return( Cur_Vista );
}

/*********************************************************************/

   // only needed for endian order swapping  (=>UNIX)

#ifdef UNIX
EXTERN void Swap_Vista_Chunk( VISTA_CHUNK *Cur )
{
   INT i;
   for( i=0; i<Cur->Nb_Polys; ++i )
   {
      Cur->Obj_ID[i] = Indy_s( Cur->Obj_ID[i] );
      Cur->Poly_ID[i] = Indy_s( Cur->Poly_ID[i] );
   }
}
#endif   // UNIX

EXTERN VISTA *Load_Vista( STRING In_Name, WORLD *W )
{
   FILE *In;
   VISTA_CHUNK **Cur, *Chunk;
   VISTA *New;
   VISTA_IO Head;
   SHORT Cam_Id;
   INT i;

   In = Access_File( In_Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

   New = New_Fatal_Object( 1, VISTA );
   Mem_Clear( New );
   F_READ( &Head, In, sizeof( Head ) );
   New->Max_Polys = Indy_l( Head.Max_Polys );
      //    soupape de securité!!
      // Lors de l'insertion/removal du DELTA
      // on genere temporairement des ID supplementaires...
   New->Max_Polys *= 2;
   New->Max_Polys += 20;  

   Cam_Id = Indy_s( Head.Camera_ID );
   New->Camera = Search_Node_From_ID( W->Root, Cam_Id );
   if ( New->Camera==NULL ) goto Failed;  // ?!?
   if ( New->Camera->Type != CAMERA_TYPE ) goto Failed;

   New->Lens = Indy_f( Head.Lens );
   New->Clip = Indy_f( Head.Clip );
   New->Nb_Chunks = Indy_l( Head.Nb_Chunks );
   New->Cur_Time = _HUGE_;    // will force chunk rewind
   New->Cur_Chunk = NULL;
   New->Cur_Nb = 0;

   Cur = &New->Chunks;
   for( i=0; i<New->Nb_Chunks; ++i )
   {
      VISTA_CHUNK_IO Tmp;

      if ( F_READ( &Tmp, In, sizeof( Tmp ) ) != 1 ) goto Failed;

      Chunk = New_Vista_Chunk( 0 );
      (*Cur) = Chunk;
      Cur = &Chunk->Next;
      Chunk->Nb_Polys = Indy_l( Tmp.Nb_Polys );
      Chunk->Time = Indy_f( Tmp.Time );
      Chunk->Type = Indy_l( Tmp.Type );
      Chunk->Z_Min = Indy_f( Tmp.Z_Min );
      Chunk->Z_Max = Indy_f( Tmp.Z_Max );
      if ( Chunk->Nb_Polys )
      {
         Chunk->Obj_ID = (USHORT*)Load_LZW_Block_II( In );
         Chunk->Poly_ID = (USHORT*)Load_LZW_Block_II( In );
         if ( Indy_s( 0x1234 )!=0x1234 ) Swap_Vista_Chunk( Chunk );    // restore state
      }
      else { Chunk->Obj_ID = NULL; Chunk->Poly_ID = NULL; }
   }
   F_CLOSE( In );   

   Destroy_Vista( ((CAMERA *)New->Camera->Data)->Vista );   // Clear previous if needed
   ((CAMERA *)New->Camera->Data)->Vista = New;  // install new

   return( New );

Failed:
   Destroy_Vista( New );
   return( NULL );
}

/*********************************************************************
 *                                                                   *
 *               RENDERING USING VISTA                               *
 *                                                                   *
 * _RCst_.The_Camera->Mo must be set before getting here...          *
 *********************************************************************/

      //             -= replaces Sort_Objects() =-
      // . build visible poly list corresponding to current World->Time
      // . Check in hierarchy if concerned objects are transformed yet
      // . computes visible polys data

#if 0   // (debug)
static void Print_Vista_State( VISTA_CHUNK *Cur_Chunk )
{
   INT i;
   fprintf( stderr, "Src (%d):", Cur_Chunk->Nb_Polys );
   for( i=0; i<Cur_Chunk->Nb_Polys; ++i ) fprintf( stderr, "%.4x%.4x ",
      Cur_Chunk->Obj_ID[i], Cur_Chunk->Poly_ID[i] );
   fprintf( stderr, "\n\nDst (%d):", Cur_Vista->Cur_Nb );
   for( i=0; i<Cur_Vista->Cur_Nb; ++i ) fprintf( stderr, "%.4x%.4x ",
      Cur_Vista->Obj_ID[i], Cur_Vista->Poly_ID[i] );            
   fprintf( stderr, "\n\n" );
}
#endif

EXTERN void Sort_Vista_Polys( OBJ_NODE *Root )
{
   VISTA_CHUNK *Cur_Chunk;
   INT i;

         // search for correct chunk time

   if ( Cur_Vista->Cur_Time>_RCst_.The_World->Time )
   {
      Cur_Chunk = Cur_Vista->Cur_Chunk = Cur_Vista->Chunks;
      Cur_Vista->Cur_Time = -_HUGE_;
      goto Init;
   }
   
         // Perform_Chunk_Update

   Cur_Chunk = Cur_Vista->Cur_Chunk;
   if ( Cur_Chunk==NULL ) return;      // hope Cur_Vista->Cur_Nb is ok!
   if ( Cur_Chunk->Next==NULL ) 
   {
      if ( Cur_Vista->Cur_Time!=Cur_Chunk->Time ) goto Init;
      else goto Ok;
   }
   if ( Cur_Chunk->Next->Time>_RCst_.The_World->Time ) goto Ok;

            // search. this is a mess, with all the GOTOs!! well...

   while( 1 )
   {
      if ( Cur_Chunk->Next==NULL ) break; // last one
      if ( Cur_Chunk->Next->Time>_RCst_.The_World->Time )
         break; // found
      Cur_Chunk = Cur_Chunk->Next;

Init:
      if ( Cur_Chunk->Type == VISTA_INIT )
      {
         memcpy( Cur_Vista->Poly_ID, Cur_Chunk->Poly_ID, Cur_Chunk->Nb_Polys*sizeof( USHORT ) );
         memcpy( Cur_Vista->Obj_ID, Cur_Chunk->Obj_ID, Cur_Chunk->Nb_Polys*sizeof( USHORT ) );
         Nb_Poly_Sorted = Cur_Vista->Cur_Nb = Cur_Chunk->Nb_Polys;

//         fprintf( stderr, "[t=%.2f. \t[INIT]. %d polys]\n", _RCst_.The_World->Time, Cur_Chunk->Nb_Polys );
      }
      else  // if ( Cur_Chunk->Type == VISTA_DELTA )
      {
         INT i, j, Min;

            // to perform insertion/removal without to many searches,
            // we must keep the ->Obj_ID and ->Poly_ID lexically
            // ordered.

         Min = 0;
         for( j=0; j<Cur_Chunk->Nb_Polys; ++j )
         {
            UINT Src_ID;      // ID to insert/remove

//            fprintf( stderr, "<=[Node_Ptr[0]=0x%x\n", _RCst_.The_World->Node_Ptr[0] );

            Src_ID = Cur_Chunk->Obj_ID[j] << 16;
            Src_ID |= Cur_Chunk->Poly_ID[j];
            for( i=Min; i<=Cur_Vista->Cur_Nb; ++i )
            {
               UINT Dst_ID;      // ID scanned for
               INT k;

               if ( i==Cur_Vista->Cur_Nb ) goto Insert;  // force
               Dst_ID = Cur_Vista->Obj_ID[i] << 16;
               Dst_ID |= Cur_Vista->Poly_ID[i];
               if ( Dst_ID==Src_ID )
               {
                     // this is a minus

//                  fprintf( stderr, "Removing ID 0x%.8x from order %d. Total Nb=%d\n", Dst_ID, i, Cur_Vista->Cur_Nb );
                  Cur_Vista->Cur_Nb--;
                  for( k=i; k<Cur_Vista->Cur_Nb; ++k )
                  {
                     Cur_Vista->Obj_ID[k] = Cur_Vista->Obj_ID[k+1];
                     Cur_Vista->Poly_ID[k] = Cur_Vista->Poly_ID[k+1];
                  }
                  Min=i;
                  break;   // next plus/minus
               }
//               else if ( Dst_ID<Src_ID ) Min = i+1; // not found yet
               else if ( Dst_ID>Src_ID ) 
               {  // we've just skipped to good position. => Insert
Insert:
                     // this is a plus

//                  fprintf( stderr, "Inserting ID 0x%.8x at order %d (=ID 0x%.8x). Total Nb=%d\n",
//                     Src_ID, Min, (Cur_Vista->Obj_ID[Min]<<16) | Cur_Vista->Poly_ID[Min], Cur_Vista->Cur_Nb );

//                  if ( Cur_Vista->Cur_Nb>=Cur_Vista->Max_Polys )
//                     fprintf( stderr, "!!!Cur_Nb/Max_Polys:%d/%d  \n", Cur_Vista->Cur_Nb, Cur_Vista->Max_Polys );
                  for( k=Cur_Vista->Cur_Nb-1; k>=i; --k )
                  {
                     Cur_Vista->Obj_ID[k+1] = Cur_Vista->Obj_ID[k];
                     Cur_Vista->Poly_ID[k+1] = Cur_Vista->Poly_ID[k];
                  }
                  Cur_Vista->Obj_ID[i] = (USHORT)( Src_ID>>16 );
                  Cur_Vista->Poly_ID[i] = (USHORT)( Src_ID & 0xFFFF );
                  Cur_Vista->Cur_Nb++;
                  Min=i+1;
                  break;   // next plus/minus
               }
            }
//            Print_Vista_State( Cur_Chunk );
         }
//         fprintf( stderr, "[t=%.2f. \t[DELTA]. %d/%d polys]\n", _RCst_.The_World->Time, Cur_Chunk->Nb_Polys, Cur_Vista->Cur_Nb );
//         for( i=0; i<=Cur_Vista->Cur_Nb; ++i )
//            fprintf( stderr, "0x%x/%x  ", Cur_Vista->Obj_ID[i], Cur_Vista->Poly_ID[i] );
//         fprintf( stderr, "\n" );
      }
      // else: this is an error
   }
   Cur_Vista->Cur_Time = Cur_Chunk->Time;
   Cur_Vista->Cur_Chunk = Cur_Chunk;

         // Zscreen = K*Zmin*(1/Z - 1/ZMax)
         //         = (K*Zmin) / Z  + ( -K*Zmin/ZMax )
         //         = Z_Scale * Inv_Z + Z_Off...
   _RCst_.Z_Min = Cur_Chunk->Z_Min;
   _RCst_.Z_Max = Cur_Chunk->Z_Max;
   _RCst_.Z_Scale = _RCst_.Global_Z_Scale * _RCst_.Z_Min;
   _RCst_.Z_Vista_Scale = 65536.0f / ( _RCst_.Z_Max-_RCst_.Z_Min + .0001f );
   _RCst_.Z_Off = -_RCst_.Z_Scale/_RCst_.Z_Max;

Ok:

   Nb_Poly_Sorted = 0;     // <= active objects
   Nb_Sorted = 0;          // <= entry of polys in P_Vertex[] array

   for( i=0; i<Cur_Vista->Cur_Nb; ++i )
   {
      USHORT Obj_ID;

      Poly_Keys[i] = Nb_Poly_Sorted<<16;

      Obj_ID = Cur_Vista->Obj_ID[i];
      Cur_Node = _RCst_.The_World->Node_Ptr[Obj_ID];

      if ( Cur_Node->Flags & OBJ_DONT_RENDER ) continue;
      if ( Cur_Node->Type==CAMERA_TYPE ) continue;
      if ( Cur_Node->Type==MATERIAL_TYPE ) continue;

      Check_Node_Transform( Cur_Node );

      if ( Cur_Node->Flags&OBJ_NOT_ACTIVE ) continue;

         // Transform object in camera's space
         // Result for this cam is in Cur->M

      STORE_NODE_MATRIX( Cur_Node, _RCst_.The_Camera );     // <= Cur->M = Cam->Mo.Cur->Mo

      Cur_Obj = (OBJECT *)Cur_Node->Data;
      if ( Cur_Node->Type==MESH_TYPE )
      {
         USHORT Poly_ID;
         P_POLY Tmp;
         Cur_Msh = (MESH *)Cur_Obj;
         Cur_Shader = Cur_Msh->Shader;
         Cur_P_Poly = &Tmp;
         Poly_ID = Cur_Vista->Poly_ID[i];
         Orig_Poly = &Cur_Msh->Polys[Poly_ID];
         Vista_Select_Poly( i );
      }
      else if ( Cur_Node->Type==LIGHT_TYPE )
         Vista_Select_Light( Cur_Node, i );
   }
   if ( Nb_Poly_Sorted ) Renderer.Sort_Polys( Poly_Keys, Nb_Poly_Sorted );
}

/*********************************************************************/
/*********************************************************************/

static void Vista_Setup_Poly_Vtx( )
{
   INT j;

   Nb_Out_Edges = j = Nb_Edges_To_Deal_With;         
   Out_Vtx[j] = P_Edges[j] = Cur_P_Vertex;
   Vtx_Flags[j] = Cur_Vertex_State[ 0 ] & ~FLAG_FOR_CLIPPING_II;

   for( --j; j>=0; --j )
   {
      Out_Vtx[j] = P_Edges[j] = Cur_P_Vertex + j;
      Vtx_Flags[j] = Cur_Vertex_State[j] & ~FLAG_FOR_CLIPPING_II;
   }
   dy1 = P_Edges[2]->yp-P_Edges[1]->yp;
   dx0 = P_Edges[1]->xp-P_Edges[0]->xp;
   dy0 = P_Edges[1]->yp-P_Edges[0]->yp;
   dx1 = P_Edges[2]->xp-P_Edges[1]->xp,
   Area  = dx0*dy1 - dx1*dy0;
//   Area = P_Edges[0]->yp*dx1 + P_Edges[2]->yp*dx0;
//   Area += P_Edges[1]->yp * ( P_Edges[0]->xp-P_Edges[2]->xp );
}

EXTERN void Render_Vista( OBJ_NODE *Root )  
{
   INT i;

   if ( Cur_Vista==NULL ) return;

   Renderer.Sort_Objects( Root );   // will build visibility buffer
   if ( !Nb_Poly_Sorted ) return;

         // Render every objects

   for( i=0; i<Nb_Poly_Sorted; ++i )
   {
      USHORT Obj_ID, Key, ID;

      Key = (USHORT)( Poly_Keys[i]>>16 );
      ID = (USHORT)( Sorted[Key]&0xFFFF );
      Key = (USHORT)( Sorted[Key]>>16 );

      Obj_ID = Cur_Vista->Obj_ID[Key];
      Cur_Node = _RCst_.The_World->Node_Ptr[Obj_ID];
      Cur_Obj = (OBJECT *)Cur_Node->Data;

         // Render object

      if ( Cur_Node->Type==MESH_TYPE )
      {
         USHORT Poly_ID;
         INT j;
         
         Cur_Msh = (MESH*)Cur_Obj;
         Poly_ID = Cur_Vista->Poly_ID[Key];
         Orig_Poly = Cur_Msh->Polys + Poly_ID;
         Nb_Edges_To_Deal_With = Orig_Poly->Nb_Pts;
         Cur_P_Vertex = P_Vertex + ID;
         Cur_Vertex_State = Vertex_State + ID;

            ///// Vista_Render_Poly /////

         Cache_Methods.Init_From_Poly( );
         Vista_Setup_Poly_Vtx( );

         if ( fabs(Area)<=AREA_EPSILON3 ) continue;

         if ( Orig_Poly->Flags & POLY_MAP_UV )
         {
            for( j=Orig_Poly->Nb_Pts-1; j>=0; --j )
            {
               Out_Vtx[j]->UV[0] = (1.0f/UV_SCALE)*Orig_Poly->UV[j][0];
               Out_Vtx[j]->UV[1] = (1.0f/UV_SCALE)*Orig_Poly->UV[j][1];
            }
         }
         Renderer.Emit_Poly( );  // <= use global emitter
      }
      else if ( Cur_Node->Type==LIGHT_TYPE )
      {
      }
      else
      {
      }
   }
}

/*********************************************************************/
/*********************************************************************/

#endif   // _SKIP_VISTA_
