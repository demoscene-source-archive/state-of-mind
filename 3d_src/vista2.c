/***********************************************
 *            Vista building II                *
 *                                             *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"
#include "indy.h"

#ifndef _SKIP_VISTA_

/*********************************************************************/

static VISTA *Vista;
static VISTA_CHUNK *Prev_Chunk;

#define MAX_ACCUM 65536
static INT Nb_Total = 0, Prev_Total = 0;
static UINT Accum[MAX_ACCUM];
static UINT Prev_Accum[MAX_ACCUM];
static FLT Z_Min, Z_Max;

/*********************************************************************/

static SHADER_METHODS Shader1, Shader2;

/*********************************************************************/

#ifdef UNIX
static void _Draw_zBuf_ID( )
{
   INT y, dy;
   SHORT *Dst;
   USHORT *O_ID, *P_ID;

   y = Scan_Start;
   Dst = ZBuffer + (y+1)*_RCst_.Pix_BpS;
   O_ID = (USHORT*)ZBuffer_Front + (y+1)*_RCst_.Pix_BpS;
   P_ID = (USHORT*)ZBuffer_Back + (y+1)*_RCst_.Pix_BpS;
   dy = Scan_H;
   while( dy>0 )   
   {
      INT Len;
      UINT S;
      SHORT *Ptr;
      USHORT *O_ID2, *P_ID2;

      Len = Scan_Pt1[y];
      Ptr = Dst - Len;
      O_ID2 = O_ID - Len;
      P_ID2 = P_ID - Len;
      Len = Len-Scan_Pt2[y];

      S = Scan_S[y];
      while( Len<0 )
      {
         SHORT Z;
         Z = (SHORT)( S>>16 );
         if ( Z>Ptr[Len] )
         {
            Ptr[Len] = Z;
            P_ID2[Len] = Flat_Color;
            O_ID2[Len] = Flat_Color2;            
         }
         Len++;
         S -= dyS;
      }
      Dst += _RCst_.Pix_BpS;
      O_ID += _RCst_.Pix_BpS;
      P_ID += _RCst_.Pix_BpS;
      ++y;
      dy--;
   }
}
#else
extern void _Draw_zBuf_ID( );    // in vista3.asm
#endif

static RENDERING_FUNCTION Shader_Deal_ID( )
{
   FLT R;

   Flat_Color = Orig_Poly->ID;
   Flat_Color2 = (USHORT)Cur_Msh->ID;

   dS1 = ( P_Edges[1]->Inv_Z - P_Edges[0]->Inv_Z ) * _RCst_.Z_Scale;
   dS2 = ( P_Edges[2]->Inv_Z - P_Edges[1]->Inv_Z ) * _RCst_.Z_Scale;

   DyS = ( dS1*dy1 - dS2*dy0 )*Area;
   DxS = ( dS2*dx0 - dS1*dx1 )*Area;

   R = 1.0/fabs(Area)/65536.0;
   if ( R>Orig_Poly->N_Dot_C ) Orig_Poly->N_Dot_C = R;   

   Rasterize_iZ( );
   return( RENDER_USER1 );
}

static RENDERING_FUNCTION Shader_Deal_Light_ID( )
{
   Flat_Color = Orig_Poly->ID;
   Flat_Color2 = (USHORT)Cur_Msh->ID;

      // finish...

   return( RENDER_NONE );
}

/*********************************************************************/

static void Set_zBuf_ID_Func( OBJ_NODE *Obj )
{
   INT i;
   Cur_Msh = (MESH *)Obj->Data;
   Cur_Msh->Shader = &Shader1;
   Shader1.Set_Parameters( (void *)Obj->Data, 0x0000 );
   for( i=0; i<Cur_Msh->Nb_Polys; ++i )
      Cur_Msh->Polys[i].N_Dot_C = 0.0;
}

static void Set_zBuf_ID_Light_Func( OBJ_NODE *Obj )
{
   ((LIGHT *)Obj->Data)->Shader = &Shader2;
//   Shader2.Set_Parameters( (void *)Obj->Data, 0x0000 );
}

static void Sort_Objects_Vista( OBJ_NODE *Cur )
{
   for( ; Cur!=NULL; Cur = Cur->Next )
   {
      if ( Cur->Flags & OBJ_DONT_RENDER ) continue;
      if ( Cur->Type==CAMERA_TYPE ) continue;
      if ( Cur->Type==MATERIAL_TYPE ) continue;

      Check_Node_Transform( Cur );        // Check hierarchy transform for Current Time

         // Transform object in camera's space
         // Result for this cam is in Cur->M

      STORE_NODE_MATRIX( Cur, _RCst_.The_Camera ); // <= Cur->M = Cam->Mo.Cur->Mo

      {
         FLT Clip[6];

         Project_Box( &Cur->Box, Cur->M, Clip );

         if ( Clip[1]<_RCst_.Clips[0] ) continue;
         if ( Clip[0]>_RCst_.Clips[1] ) continue;
         if ( Clip[1]-Clip[0] < 1.0 ) continue;
         if ( Clip[3]<_RCst_.Clips[2] ) continue;
         if ( Clip[2]>_RCst_.Clips[3] ) continue;
         if ( Clip[3]-Clip[2] < 1.0 ) continue;
      }

      if ( Cur->Flags&OBJ_NOT_ACTIVE ) goto Skip;
      if ( Cur->Methods->Render != NULL )
      {
         Sorted[Nb_Sorted++] = ((UINT)Nb_Obj_To_Sort)<<16;
         Obj_To_Sort[ Nb_Obj_To_Sort++ ] = (void *)Cur;
      }
Skip:
      if ( Cur->Child!=NULL )    // recurse
         Sort_Objects_Vista( Cur->Child );
   }      
}

EXTERN void Init_Vista_Building( WORLD *W, OBJ_NODE *Cam_Node )
{
   CAMERA *Cam;

   Shader1 = zBuf_Shader;
   Shader1.Deal_With_Poly = Shader_Deal_ID;
   Primitives[RENDER_USER1] = (void (*)( ))_Draw_zBuf_ID;
   Traverse_Typed_Nodes( W->Root, MESH_TYPE, Set_zBuf_ID_Func );
   Shader2 = zBuf_Shader;
   Shader2.Deal_With_Poly = Shader_Deal_Light_ID;
   Traverse_Typed_Nodes( W->Root, LIGHT_TYPE, Set_zBuf_ID_Light_Func );

   Renderer.Sort_Objects = Sort_Objects_Vista;

   Destroy_Vista( Vista );
   Vista = New_Fatal_Object( 1, VISTA );
   Mem_Clear( Vista );

   Cam = (CAMERA *)Cam_Node->Data;
   Vista->Camera = Cam_Node;
   Vista->Lens = Cam->Lens;
   Vista->Clip = _RCst_.Clips[4];
   Vista->Chunks = NULL;

   _RCst_.Z_Min = _RCst_.Clips[4];
   _RCst_.Z_Max = _RCst_.Clips[6];  // just in case...
   _RCst_.Z_Scale = _RCst_.Global_Z_Scale * _RCst_.Z_Min;
   _RCst_.Z_Vista_Scale = 65536.0f / ( _RCst_.Z_Max-_RCst_.Z_Min + .0001f );
   _RCst_.Z_Off = -_RCst_.Z_Scale/_RCst_.Z_Max;
   _RCst_.All_Z_Min = _HUGE_;
   _RCst_.All_Z_Max =-_HUGE_;

   Z_Min = _HUGE_;
   Z_Max =-_HUGE_;
   Prev_Total = Nb_Total = 0;
}


/*********************************************************************/

static int Cmp( const void *A, const void *B )
{
   UINT Id1, Id2;
   Id1 = *(UINT *)A;
   Id2 = *(UINT *)B;
   if ( Id1>Id2 ) return( 1 );
   else return( -1 );
}
 
EXTERN void Vista_Analysis( WORLD *W )
{
   INT i, j;
   USHORT *O_ID, *P_ID;
   UINT Last_Id;

   O_ID = (USHORT*)ZBuffer_Front;
   P_ID = (USHORT*)ZBuffer_Back;
   Last_Id = 0xFFFF;
   for( j=-_RCst_.Pix_Height; j<0; ++j )
   {
      O_ID += _RCst_.Pix_BpS;
      P_ID += _RCst_.Pix_BpS;
      for( i=-_RCst_.Pix_Width; i<0; ++i )
      {
         UINT Id;
         INT k;

         if ( O_ID[i]==0xFFFF ) continue;

         Id = ((UINT)O_ID[i])<<16;
         Id |= (UINT)P_ID[i];
         if ( Id==Last_Id ) continue;
         Last_Id = Id;         
         for( k=Nb_Total-1; k>=0; --k ) if ( Accum[k]==Id ) break;
         if ( k==-1 )
         {
            if ( Nb_Total<MAX_ACCUM ) Accum[Nb_Total++] = Last_Id = Id;
            else Exit_Upon_Error( "ACCUM overflow!! Recompile!:)" );
         }
      }
   }
   if ( Z_Max<_RCst_.All_Z_Max ) Z_Max = _RCst_.All_Z_Max;
   if ( Z_Min>_RCst_.All_Z_Min ) Z_Min = _RCst_.All_Z_Min;
}

EXTERN void Vista_Start_New_Bunch( )
{
   Nb_Total = 0;
   memset( Accum, 0, MAX_ACCUM*sizeof( Accum[0] ) );
   Z_Min = _HUGE_;
   Z_Max =-_HUGE_;
}

EXTERN void Fill_Vista_Chunk( VISTA_CHUNK *Cur, UINT *Accum, INT Nb )
{
   INT i;
   for( i=0; i<Nb; ++i )
   {
      Cur->Obj_ID[i] = (USHORT)( Accum[i]>>16 );
      Cur->Poly_ID[i] = (USHORT)( Accum[i]&0xFFFF );
   }
   Cur->Nb_Polys = Nb;
}

EXTERN void New_Init_Chunk( VISTA_CHUNK **Ptr, FLT Time )
{
   *Ptr = New_Vista_Chunk( Nb_Total );
   Fill_Vista_Chunk( *Ptr, Accum, Nb_Total );
   (*Ptr)->Type = VISTA_INIT;
   Vista->Nb_Chunks++;
   Prev_Chunk = *Ptr;
   Prev_Chunk->Time = Time;
   Prev_Chunk->Next = NULL;
   Prev_Chunk->Z_Max = Z_Max;
   Prev_Chunk->Z_Min = Z_Min;
}

EXTERN void Vista_Emit_Data( FLT Time, INT Nb_Thresh )
{
   Z_Max *= 1.1; Z_Min /= 1.1;
   qsort( Accum, Nb_Total, sizeof( Accum[0] ), Cmp );
   fprintf( stderr, "  T=%.2f [z=%.2f,%.2f] [%d->%d", Time, Z_Min, Z_Max, Prev_Total, Nb_Total );
   if ( Prev_Chunk==NULL ) // first one. INIT
   {
      fprintf( stderr, "] [First chunk]\n" );
      New_Init_Chunk( &Vista->Chunks, Time );
      memcpy( Prev_Accum, Accum, Nb_Total*sizeof( Accum[0] ) );
      Prev_Total = Nb_Total;
   }
   else
   {
      INT i,j, Keep;
      UINT Copy_Accum[MAX_ACCUM], Keep_Prev[MAX_ACCUM];
      UINT Copy_Prev[MAX_ACCUM];
      INT Nb_Minus, Nb_Plus;

      if ( Nb_Total ) memcpy( Copy_Accum, Accum, Nb_Total*sizeof( Accum[0] ) );
      if ( Prev_Total ) memcpy( Copy_Prev, Prev_Accum, Prev_Total*sizeof( Accum[0] ) );
      memset( Keep_Prev, 0, MAX_ACCUM*sizeof( Keep_Prev[0] ) );

      Keep = Nb_Plus = Nb_Minus = 0;
      for( j=0; j<Prev_Total; ++j )      
      {
         UINT Old_Id;
         Old_Id = Prev_Accum[j];
         for( i=0; i<Nb_Total; ++i )
         {
            INT Id;
            Id = Accum[i];
            if ( Id==0xFFFFFFFF ) continue;
            if ( Id==Old_Id )
            {
               Keep_Prev[Keep++] = Old_Id;
               Prev_Accum[j] = 0xFFFFFFFF;
               Accum[i]=0xFFFFFFFF;
            }
         }
      }
      for( i=0; i<Prev_Total; ++i )
         if ( Prev_Accum[i]!=0xFFFFFFFF )
            Prev_Accum[Nb_Minus++] = Prev_Accum[i];
      for( i=0; i<Nb_Total; ++i )
         if ( Accum[i]!=0xFFFFFFFF )
            Accum[Nb_Plus++] = Accum[i];

      fprintf( stderr, " (-:%d *:%d +:%d)] ", Nb_Minus, Keep, Nb_Plus );

            /* 
             * . Keep_Prev[]/Keep counts untouched IDs
             * . Prev_Accum[]/Nb_Minus counts those IDs who must vanish
             * . Accum[]/Nb_Plus counts new IDs
             *
             * Prev_Accum[]/Prev_Total must reflect the current polys state
             */

      if ( Nb_Minus+Nb_Plus>Nb_Total ) // too much swap. Re-init a new status
      {
         memcpy( Accum, Copy_Accum, Nb_Total*sizeof( Copy_Accum[0] ) );
         New_Init_Chunk( &Prev_Chunk->Next, Time );
         if ( Nb_Total ) memcpy( Prev_Accum, Accum, Nb_Total*sizeof( Accum[0] ) );
         Prev_Total = Nb_Total;
         fprintf( stderr, "[INIT]\n" );
      }
      else //  if ( Nb_Plus>Nb_Thresh )   // is there enough new polys?!      
      {                             // yes => perform the delta
         VISTA_CHUNK *New;

         if ( Nb_Minus ) memcpy( &Accum[Nb_Plus], Prev_Accum, Nb_Minus*sizeof( Prev_Accum[0] ) );
         Nb_Total = Nb_Plus+Nb_Minus;

            // build next accumalator
         if ( Keep ) memcpy( Prev_Accum, Keep_Prev, Keep*sizeof( Keep_Prev[0] ) );
         Prev_Total = Keep;
         if ( Nb_Plus ) memcpy( &Prev_Accum[Keep], Accum, Nb_Plus*sizeof( Accum[0] ) );
         Prev_Total += Nb_Plus;
         if ( Prev_Total ) qsort( Prev_Accum, Prev_Total, sizeof( UINT ), Cmp );

            // compute delta accumulator

         qsort( Accum, Nb_Total, sizeof( UINT ), Cmp );
         New = New_Vista_Chunk( Nb_Total );
         Fill_Vista_Chunk( New, Accum, Nb_Total );
         New->Nb_Polys = Nb_Total;
         New->Type = VISTA_DELTA;
         New->Time = Time;
         New->Z_Min = Z_Min;
         New->Z_Max = Z_Max;
         Prev_Chunk->Next = New;
         Prev_Chunk = New;

         Vista->Nb_Chunks++;
         fprintf( stderr, "[Delta(%d/%d)]\n", Nb_Total, Prev_Total );
      }
#if 0
      else  // add the new polys with the rest and do nothing...
      {
         if ( Prev_Total ) memcpy( Prev_Accum, Copy_Prev, Prev_Total*sizeof( Copy_Prev[0] ) );
         if ( Nb_Plus ) memcpy( &Prev_Accum[Prev_Total], Accum, Nb_Plus*sizeof( Accum[0] ) );
         Prev_Total += Nb_Plus;
         if ( Nb_Plus ) qsort( Prev_Accum, Prev_Total, sizeof( UINT ), Cmp );
         fprintf( stderr,"[merge]\n" );
      }
#endif
   }
   Nb_Total = 0;
   if ( Prev_Total>Vista->Max_Polys ) Vista->Max_Polys = Prev_Total;
}

/*********************************************************************/

EXTERN void Dump_Vista( STRING Out_Name )
{
   FILE *Out;
   VISTA_CHUNK *Cur;
   VISTA_IO Head;

   if ( Vista==NULL ) return;

   Out = Access_File( Out_Name, WRITE_SWITCH );
   if ( Out==NULL ) return;

   Head.Max_Polys = Indy_l( Vista->Max_Polys );
   Head.Camera_ID = (USHORT)Indy_s( Vista->Camera->ID );
   Head.Lens = Indy_f( Vista->Lens );
   Head.Clip = Indy_f( Vista->Clip );
   Head.Nb_Chunks = Indy_l( Vista->Nb_Chunks );
   F_WRITE( &Head, sizeof(Head), 1, Out );

   for( Cur=Vista->Chunks; Cur!=NULL; Cur=Cur->Next )
   {
      VISTA_CHUNK_IO Tmp;
      Tmp.Time = Indy_f( Cur->Time );
      Tmp.Type = Indy_l( Cur->Type );
      Tmp.Nb_Polys = Indy_l( Cur->Nb_Polys );
      Tmp.Z_Min = Indy_f( Cur->Z_Min );
      Tmp.Z_Max = Indy_f( Cur->Z_Max );
      F_WRITE( &Tmp, sizeof(Tmp), 1, Out );
      if ( Cur->Nb_Polys )
      {
         if ( Indy_s( 0x1234 )!=0x1234 ) Swap_Vista_Chunk( Cur );
         Save_LZW_Block_II( (BYTE*)Cur->Obj_ID, Out, Cur->Nb_Polys*sizeof(USHORT), 8 );
         Save_LZW_Block_II( (BYTE*)Cur->Poly_ID, Out, Cur->Nb_Polys*sizeof(USHORT), 8 );
         if ( Indy_s( 0x1234 )!=0x1234 ) Swap_Vista_Chunk( Cur );    // restore state
      }
   }
   fclose( Out );   
}

/*********************************************************************/

#endif   // _SKIP_VISTA_
