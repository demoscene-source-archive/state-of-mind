/***********************************************
 *               shadow.c                      *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

#ifdef USE_OLD_SHADOWZ

EXTERN USHORT Light_Buffer2[256*256];

/******************************************************************/
/******************************************************************/

EXTERN void Shader_Emit_Shadow_Polys( )
{
   INT i;

   for( i=Nb_Poly_Sorted-1; i>=0; --i )
   {
      INT j;

      Cur_P_Poly = &Poly_To_Sort[ Poly_Keys[i]>>16 ];
      Orig_Poly = Cur_P_Poly->Poly_Orig;
      Nb_Edges_To_Deal_With = Orig_Poly->Nb_Pts;

               /* No clip, Just emit */
      Nb_Out_Edges = j = Nb_Edges_To_Deal_With;
      for( --j; j>=0; --j )
         P_Edges[j] = Out_Vtx[j] = P_Vertex + Orig_Poly->Pt[j];
      dy1 = P_Edges[2]->yp-P_Edges[1]->yp;
      dx0 = P_Edges[1]->xp-P_Edges[0]->xp;
      Area  = dx0*dy1;
      dy0 = P_Edges[1]->yp-P_Edges[0]->yp;
      dx1 = P_Edges[2]->xp-P_Edges[1]->xp,
      Area -= dx1*dy0;
      if ( Area<=AREA_EPSILON3 ) continue;
      Out_Vtx[Nb_Out_Edges] = Out_Vtx[0];

               /* Rasterize */

      for( j=Nb_Out_Edges-1; j>=0; --j )
      {
         Edges_y[j] = Out_Vtx[j]->yp;
         Edges_dy[j] = Out_Vtx[j+1]->yp - Edges_y[j];
         Edges_x[j] = Out_Vtx[j]->xp;
         Edges_dx[j] = Out_Vtx[j+1]->xp - Edges_x[j];
/*
         Edges_z[j] = Out_Vtx[j]->Inv_Z;
         Edges_dz[j] = Out_Vtx[j+1]->Inv_Z - Edges_z[j];
*/
      }
      Edges_y[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->yp;
      Edges_x[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->xp;
/*
      Edges_z[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->Inv_Z;
*/
      /* Area = 1.0/Area; */

      Scan_Start = LIGHT_BUF_H; Scan_H = 0;

      Rasterize_Edges( );
      // Scan_H -= Scan_Start;
      if ( Scan_H<= 0 ) continue;
      /* Format_Scanlines( ); */

      {
         RENDERING_FUNCTION Draw;
         Constants_Computed = FALSE;
         Draw = (*Cur_Shader->Deal_With_Poly)( );
         if ( Draw!=RENDER_NONE )
            (*Primitives[ Draw ])( );

         /* No 2nd or third pass, yet... */
      }
   }
}

/******************************************************************/
/*                      SHADOWS                                   */
/******************************************************************/

static void Shader_Transform_Shadow( MATRIX2 M )
{
   INT i;
   FLT ZScale, Mesh_Max, Mesh_Min;

   Mesh_Max = -_HUGE_;
   Mesh_Min =  _HUGE_;

      /* TODO: FiX ThAt !! */
   bzero( Vertex_State, Cur_Msh->Nb_Vertex*sizeof(PIXEL) );

   Orig_Poly = Cur_Msh->Polys;
   Nb_Poly_Sorted = 0;
   Cur_P_Poly = Poly_To_Sort;
   for( i=Cur_Msh->Nb_Polys; i>0; --i )
   {
      INT j;
      FLT Key;

#if 0
         /* Can't be used as it: Orig_Poly->No isn't transformed
            in parent's frame here... */

      if ( Dot_Product( Orig_Poly->No, Cur_Light->Dir ) > 0.1 )
         goto End;
#endif

/*
      nA_V( Cur_P_Poly->N, M, Orig_Poly->No );
      if ( Cur_P_Poly->N[2]>0.01 ) goto End;

         No need for more than Cur_P_Poly->N[2].
          => Unrolled:
*/
      Key  = M[15]*Orig_Poly->No[0];
      Key += M[16]*Orig_Poly->No[1];
      Key += M[17]*Orig_Poly->No[2];
      Key *= -256.0;
      if ( Key<1.0 )
      {
         // Cur_P_Poly->C = 0;
         goto End;  /* this poly WILL be flat-rendered in real space */
      }
         /* else => Store light intensity in .C field... */      
      // Cur_P_Poly->C = (USHORT)Key;

      Key = 0.0;
      for( j=2; j>=0; --j )
      {
         INT K;

         K = Orig_Poly->Pt[j];
         if ( Vertex_State[K]==0x01 ) goto End;
         else if ( Vertex_State[K]==0x00 )
         {
            VECTOR Pt;
            FLT Inv_Z;

            A_V( Pt, M, (FLT *)( Cur_Msh->Vertex+K ) );

            if ( Pt[2]<0.01 ) goto Reject;   /* Wrong orientation */            
            P_Vertex[K].Inv_Z = Pt[2]; /* .Inv_Z contains Z, actually */
            Key += Pt[2];      /* Inv_Z is used as key => ajust Scale !! */

            Inv_Z = 127.9 / (FLT)sqrt( (double)Norm_Squared( Pt ) );            
            Pt[0] *= Inv_Z; Pt[1] *= Inv_Z; Pt[2] *= Inv_Z; /* Normalize */
            Inv_Z = Cur_Light->cos_Alpha/Pt[2];            
            if ( Inv_Z>1.0 ) goto Reject;    /* Wrong hemisphere */
            {
               PIXEL U, V;
               U = (PIXEL)( Pt[0]*Inv_Z ) + 128;
               P_Vertex[K].xp = (FLT)U;
               V = 127 - (PIXEL)( Pt[1]*Inv_Z );
               P_Vertex[K].yp = (FLT)V;
               // UV_Vertex[ Shadow_IDs[K] ] = U | (V<<8);
            }
            Vertex_State[K] = 0x02;
            continue;
Reject:
            Vertex_State[K] = 0x01;
            goto End;
         }
         else Key += P_Vertex[K].Inv_Z;
      }
      Nb_Poly_Sorted++;
      Cur_P_Poly->Poly_Orig = Orig_Poly;
      /* Key /= Orig_Poly->Nb_Pts */
      Cur_P_Poly->Key = Key;
      if ( Key>Mesh_Max ) Mesh_Max = Key;
      if ( Key<Mesh_Min ) Mesh_Min = Key;

      Cur_P_Poly++;
End:
      Orig_Poly++;
   }
   if ( !Nb_Poly_Sorted ) return;

   ZScale = 65536.0 / ( Mesh_Max-Mesh_Min + .0001 );
   for( i=0; i<Nb_Poly_Sorted; ++i )
   {
      USHORT Key;
      Key = (USHORT)( ( Poly_To_Sort[i].Key - Mesh_Min )*ZScale );
      Poly_Keys[ i ] = (i<<16) | Key;
   }
   Counting_Sort( Poly_Keys, Nb_Poly_Sorted );
}

/******************************************************************/
/******************************************************************/

static void (*Shader_Deal_Shadow( ))( )
{
         /* TODO: INLINE that !!! */
   Flat_Color = (USHORT)( Orig_Poly->ID & 0xFFFF );
   return( Shader_Draw_16b_256 );
}

/******************************************************************/
/******************************************************************/
/*                      z-SHADOWS                                 */
/******************************************************************/

static void Shader_Emit_zShadow_Polys( )
{
   INT i;

   BZero( Light_Buffer2, 256*256*sizeof(USHORT) );

   /* for( i=Nb_Poly_Sorted-1; i>=0; --i ) */
   for( i=0; i<Nb_Poly_Sorted; i++ )
   {
      INT j;

      Cur_P_Poly = &Poly_To_Sort[ Poly_Keys[i]>>16 ];
      Orig_Poly = Cur_P_Poly->Poly_Orig;
      Nb_Edges_To_Deal_With = Orig_Poly->Nb_Pts;

               /* No clip, Just emit */
      Nb_Out_Edges = j = Nb_Edges_To_Deal_With;
      for( --j; j>=0; --j )
         P_Edges[j] = Out_Vtx[j] = P_Vertex + Orig_Poly->Pt[j];
      dy1 = P_Edges[2]->yp-P_Edges[1]->yp;
      dx0 = P_Edges[1]->xp-P_Edges[0]->xp;
      Area  = dx0*dy1;
      dy0 = P_Edges[1]->yp-P_Edges[0]->yp;
      dx1 = P_Edges[2]->xp-P_Edges[1]->xp,
      Area -= dx1*dy0;
      if ( Area<=AREA_EPSILON3 ) continue;
      Out_Vtx[Nb_Out_Edges] = Out_Vtx[0];

               /* Rasterize */

      for( j=Nb_Out_Edges-1; j>=0; --j )
      {
         Edges_y[j] = Out_Vtx[j]->yp;
         Edges_dy[j] = Out_Vtx[j+1]->yp - Edges_y[j];
         Edges_x[j] = Out_Vtx[j]->xp;
         Edges_dx[j] = Out_Vtx[j+1]->xp - Edges_x[j];
/*
         Edges_z[j] = Out_Vtx[j]->Inv_Z;
         Edges_dz[j] = Out_Vtx[j+1]->Inv_Z - Edges_z[j];
*/
      }
      Edges_y[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->yp;
      Edges_x[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->xp;
/*
      Edges_z[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->Inv_Z;
*/
      Area = 1.0/Area;

      Scan_Start = LIGHT_BUF_H; Scan_H = 0;

      Rasterize_Edges( );
      // Scan_H -= Scan_Start;
      if ( Scan_H<=0 ) continue;
      /* Format_Scanlines( ); */

      {
         RENDERING_FUNCTION Draw;
         Constants_Computed = FALSE;
         Draw = (*Cur_Shader->Deal_With_Poly)( );
         if ( Draw!=RENDER_NONE )
            (*Primitives[ Draw ])( );
      }
   }
}

static void Shader_Transform_zShadow( MATRIX2 M )
{
   INT i;
   FLT ZScale, Mesh_Min, Mesh_Max;

   Mesh_Max = -_HUGE_;
   Mesh_Min =  _HUGE_;

      /* TODO: FiX ThAt !! */
   bzero( Vertex_State, Cur_Msh->Nb_Vertex*sizeof(PIXEL) );

   Orig_Poly = Cur_Msh->Polys;
   Nb_Poly_Sorted = 0;
   Cur_P_Poly = Poly_To_Sort;
   for( i=Cur_Msh->Nb_Polys; i>0; --i )
   {
      INT j;
      FLT Key;

      Key  = M[15]*Orig_Poly->No[0];
      Key += M[16]*Orig_Poly->No[1];
      Key += M[17]*Orig_Poly->No[2];
      Key *= -256.0;
      if ( Key<1.0 )
      {
         Cur_P_Poly->C = 0;
         goto End;  /* this poly WILL be flat-rendered in real space */
      }
         /* else => Store light intensity in .C field... */      
      Cur_P_Poly->C = (USHORT)Key;

      Key = 0.0;
      j = Nb_Edges_To_Deal_With = Orig_Poly->Nb_Pts;
      for( j--; j>=0; --j )
      {
         INT K;
         FLT Tmp;

         K = Orig_Poly->Pt[j];
         if ( Vertex_State[K]==0x01 ) goto End;
         else if ( Vertex_State[K]==0x00 )
         {
            VECTOR Pt;
            FLT Inv_Z;

            A_V( Pt, M, (FLT *)( Cur_Msh->Vertex+K ) );

            if ( Pt[2]<0.01 ) goto Reject;   /* Wrong orientation */            

            Inv_Z = 127.9 / (FLT)sqrt( (double)Norm_Squared( Pt ) );            
            Pt[0] *= Inv_Z; Pt[1] *= Inv_Z; Pt[2] *= Inv_Z; /* Normalize */
            P_Vertex[K].Inv_Z = Inv_Z;
            Key += Inv_Z;
            Tmp = Cur_Light->cos_Alpha/Pt[2];            
            if ( Tmp>1.0 ) goto Reject;    /* Wrong hemisphere */
            {
               PIXEL U, V;
               U = (PIXEL)( Pt[0]*Tmp ) + 128;
               P_Vertex[K].xp = (FLT)U;
               V = 127 - (PIXEL)( Pt[1]*Tmp );
               P_Vertex[K].yp = (FLT)V;
               // UV_Vertex[ Shadow_IDs[K] ] = U | (V<<8);
               // Inv_Z_Vertex[ Shadow_IDs[K] ] = Inv_Z; /* Clamp to 16bits ?! */
            }
            Vertex_State[K] = 0x02;
            continue;
Reject:
            Vertex_State[K] = 0x01;
            goto End;
         }
         else Key += P_Vertex[K].Inv_Z;
      }
      Nb_Poly_Sorted++;

      Cur_P_Poly->Poly_Orig = Orig_Poly;
      /* Key /= Orig_Poly->Nb_Pts */
      Cur_P_Poly->Key = Key;
      if ( Key>Mesh_Max ) Mesh_Max = Key;
      if ( Key<Mesh_Min ) Mesh_Min = Key;
      Cur_P_Poly++;
End:
      Orig_Poly++;
   }

   if ( !Nb_Poly_Sorted ) return;

   ZScale = 65536.0 / ( Mesh_Max-Mesh_Min + .0001 );
   for( i=0; i<Nb_Poly_Sorted; ++i )
   {
      USHORT Key;
      Key = (USHORT)( ( Poly_To_Sort[i].Key-Mesh_Min )*ZScale );
      Poly_Keys[ i ] = (i<<16) | Key;
   }

   ZScale = 32768.0/Mesh_Max;
   i=Cur_Msh->Nb_Vertex;
   while( --i>=0 )
   {
      if ( !( Vertex_State[i] & 0x02 ) ) continue;
      // P_Vertex[i].Inv_Z = Inv_Z_Vertex[ Shadow_IDs[i] ] =
      //   ( Inv_Z_Vertex[ Shadow_IDs[i] ] )*ZScale;
   }
   Counting_Sort( Poly_Keys, Nb_Poly_Sorted );
}

/******************************************************************/
/******************************************************************/

static void Set_Param_Shadow( void *Obj, ... )
{
}

/******************************************************************/

EXTERN SHADER_METHODS Shadow_Shader =
{
   Set_Param_Shadow,   /* Set parameters */
   Shader_Transform_Shadow, /* transform */
   Shader_Emit_Shadow_Polys, /* Emit */
   NULL, NULL,
   NULL, /* Shaders setup */
   Shader_Deal_Shadow, NULL, NULL,
   NULL
};

EXTERN SHADER_METHODS zShadow_Shader =
{
   Set_Param_Shadow,   /* Set parameters */
   Shader_Transform_zShadow, /* transform */
   Shader_Emit_zShadow_Polys,
   NULL, NULL,
   NULL, /* Shaders setup */
   Shader_Deal_zShadow, NULL, NULL,
   NULL
};

/******************************************************************/
/******************************************************************/

#endif   // USE_OLD_SHADOWZ
