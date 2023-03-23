/***********************************************
 *              vshadow.c                      *
 *        Volumic/soft shadows                 *
 * Skal 98                                     *
 * Only with 8b renderer!!                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

EXTERN P_VERTICE *Cap_Vtx;
EXTERN BYTE *Cap_State;

/******************************************************************/
/*                      V-SHADOWS                                 */
/******************************************************************/

EXTERN INT Shader_Emit_VShadow_Poly( )
{
   INT i, f;

      // ALWAYS CLIP !

   Out_Vtx[4] = P_Edges[4] = Out_Vtx[0];
   Vtx_Flags[4] = Vtx_Flags[0];

   f = Vtx_Flags[0] | Vtx_Flags[1] | Vtx_Flags[2] | Vtx_Flags[3];
   f &= FLAG_CLIP_USEFUL;

Redo:

   dy1 = P_Edges[2]->yp-P_Edges[1]->yp;
   dx0 = P_Edges[1]->xp-P_Edges[0]->xp;
   dy0 = P_Edges[1]->yp-P_Edges[0]->yp;
   dx1 = P_Edges[2]->xp-P_Edges[1]->xp,
//   Area  = dx0*dy1 - dx1*dy0;  
   Area = P_Edges[0]->yp*dx1 + P_Edges[2]->yp*dx0;
   Area += P_Edges[1]->yp * ( P_Edges[0]->xp-P_Edges[2]->xp );

   if ( fabs(Area)<=AREA_EPSILON3 ) return(-1);      

   if ( (Area<=-AREA_EPSILON3)&&!(f&VTX_CLIP_Zo) )
   {
      P_VERTICE *Tmp;
      USHORT Tmp2;
      INT i, j;
      for( i=0,j=Nb_Out_Edges-1; i<j; ++i,--j )
      {
         Tmp = Out_Vtx[i]; Out_Vtx[i] = Out_Vtx[j]; Out_Vtx[j] = Tmp;
         Tmp = P_Edges[i]; P_Edges[i] = P_Edges[j]; P_Edges[j] = Tmp;
         Tmp2 = Vtx_Flags[i]; Vtx_Flags[i] = Vtx_Flags[j]; Vtx_Flags[j] = Tmp2;
      }
      Out_Vtx[Nb_Out_Edges] = P_Edges[Nb_Out_Edges] = Out_Vtx[0];
      Vtx_Flags[Nb_Out_Edges] = Vtx_Flags[0];
      goto Redo;
   }

   if ( f )
   {
      SH_Clip( f );  
      if ( !Nb_Out_Edges ) return(-1);
   }

            // Rasterize

   for( i=Nb_Out_Edges-1; i>=0; --i )
   {
      Edges_y[i] = Out_Vtx[i]->yp;
      Edges_dy[i] = Out_Vtx[i+1]->yp - Edges_y[i];
      Edges_x[i] = Out_Vtx[i]->xp;
      Edges_dx[i] = Out_Vtx[i+1]->xp - Edges_x[i];
      Edges_z[i] = Out_Vtx[i]->Inv_Z;
//      Edges_dz[i] = Out_Vtx[i+1]->Inv_Z - Edges_z[i];
   }
   Edges_y[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->yp;
   Edges_x[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->xp;
   Edges_z[Nb_Out_Edges] = Out_Vtx[Nb_Out_Edges]->Inv_Z;

   Area = 1.0f/Area;

   Rasterize_Edges( );
   if ( Scan_H<=0 ) return( -1 );
   return( 0 );
}

/******************************************************************/
/******************************************************************/

EXTERN void Render_zBuf_Shade( )
{
   INT i;
   P_VERTICE Cap_Vtx_Stck[MAX_SIL_VTX];
   BYTE Cap_State_Stck[MAX_SIL_VTX];

   Cap_Vtx = Cap_Vtx_Stck;
   Cap_State = Cap_State_Stck;

            // Render every objects
            // do they need being previously sorted??    (=>no)

   for( i=0; i<Nb_Sorted; ++i )
   {
      Cur_Node = (OBJ_NODE *)Obj_To_Sort[ Sorted[i]>>16 ];
      Cur_Obj = (OBJECT *)Cur_Node->Data;
      Cur_Msh = (MESH *)Cur_Obj;
      if ( Cur_Msh->Flags & ( OBJ_NO_SHADOW | OBJ_DONT_RENDER ) )
         continue;
      // if ( Cur_Msh->Methods->Render == NULL ) continue;

         // Transform object in camera's space
      STORE_NODE_MATRIX( Cur_Node, _RCst_.The_Camera );

         // Cur_Shader is Light's one, not mesh's...
      (*Cur_Shader->Transform)( Cur_Node->M );   
   }
}

/******************************************************************/
/******************************************************************/
