/***********************************************
 *          mesh ray-tracing funcs             *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "ray.h"

#ifndef UNIX
extern void ASM_Mesh_Intersect( MESH *Msh, RAY *Ray );
extern void ASM_Mesh_Intersect_Backface( MESH *Msh, RAY *Ray );
#endif

/******************************************************************/
/******************************************************************/

EXTERN INT Intersect_Mesh( RAY *Ray, OBJ_NODE *Node )
{
#ifdef UNIX
   INT i;
   MESH *Msh;
   POLY *Cur;
   POLY_RAYTRACE_CST *Cst;

   INIT_RAY( Ray );
   Msh = (MESH*)Node->Data;
   Cur = Msh->Polys;
   Cst = Msh->Cst;
   if ( !Ray_Info.Backface_Cull )
      for( i=0; i<Msh->Nb_Polys; ++i, Cur++, Cst++ )
      {
         FLT Dot, Z, x, y, UI, VI;
         VECTOR I;

         Dot = Dot_Product( Ray->Dir, Cur->No );
         if ( fabs(Dot)<_RAY_EPSILON_ ) continue;  // <= NO BACK_FACE CULL!
         Z = Cur->N_Dot_C;
         if ( Dot>0.0 ) Z = -Z;
         Z -= Dot_Product( Ray->Orig, Cur->No );
         Z = Z / Dot;
         if ( (Z>=Ray->Z) || (Z<=Ray->Z_Min) ) continue; // can't be a new intersection

         Add_Scaled_Vector( I, Ray->Orig, Ray->Dir, Z );
         UI = Dot_Product( Cst->PoP1, I );
         VI = Dot_Product( Cst->PoP2, I );

         x = Cst->V2 * UI - Cst->UV * VI + Cst->xo;
         if ( x<0.0 ) continue;
         y =-Cst->UV * UI + Cst->U2 * VI + Cst->yo;
         if ( y<0.0 ) continue;
         if ( x+y>1.0 ) continue;

         Ray->Z = Z; 
         Ray->Id = i;
         Ray->x = x;
         Ray->y = y;
      }
   else
      for( i=0; i<Msh->Nb_Polys; ++i, Cur++, Cst++ )
      {
         FLT Dot, Z, x, y, UI, VI;
         VECTOR I;

         Dot = Dot_Product( Ray->Dir, Cur->No );
         if ( Dot>-_RAY_EPSILON_ ) continue;  // <= BACK_FACE CULL!

         if ( fabs(Dot)<_RAY_EPSILON_ ) continue;  // <= NO BACK_FACE CULL!
         Z = Cur->N_Dot_C - Dot_Product( Ray->Orig, Cur->No );
         if ( Z>0.0 ) continue;     // Dot is <0.0 and final Z must be >0.0 
         Z = Z / Dot;
         if ( (Z>=Ray->Z) || (Z<=Ray->Z_Min) ) continue; // can't be a new intersection

         Add_Scaled_Vector( I, Ray->Orig, Ray->Dir, Z );
         UI = Dot_Product( Cst->PoP1, I );
         VI = Dot_Product( Cst->PoP2, I );

         x = Cst->V2 * UI - Cst->UV * VI + Cst->xo;
         if ( x<0.0 ) continue;
         y =-Cst->UV * UI + Cst->U2 * VI + Cst->yo;
         if ( y<0.0 ) continue;
         if ( x+y>1.0 ) continue;

         Ray->Z = Z; 
         Ray->Id = i;
         Ray->x = x;
         Ray->y = y;
      }
#else
   INIT_RAY( Ray );
   if ( !Ray_Info.Backface_Cull ) 
      ASM_Mesh_Intersect_Backface((MESH*)Node->Data, Ray );
   else ASM_Mesh_Intersect((MESH*)Node->Data, Ray );
#endif

   if ( Ray->Id == -1 ) return( -1 );
   Ray->Node = Node;
      // Init normal here??
   return( Ray->Id );
}

#define COL_RGB(C) { b = ((C)&0x001F)<<3; g = ((C)&0x07E0)>>3; r = ((C)&0xF800)>>8; }

EXTERN void Get_Mesh_Color( RAY *Ray, OBJ_NODE *Node, FCOLOR Col )
{
   FLT u, v;
   POLY *P;
   INT Off;
   double R,G,B;

   Ray->Flags |= RAY_HAS_COLOR;
   if ( _RCst_.The_World->Cache==NULL ) goto Default;
   P = & ((MESH*)Node->Data)->Polys[Ray->Id];
   if ( Cur_Ray_Mat==NULL || P!=Cur_Ray_Poly )
      NODE_GET_MATERIALS( Ray, Node );
   if ( Cur_Material == NULL ) goto Default;
//   if ( Ray_Info.Ambient==1.0 ) goto Default;      // debug

   {
      FLT x,y,w;
      x = Ray->x;
      y = Ray->y;
      w = 1.0f-x-y;
      u = x*P->UV[1][0] + y*P->UV[2][0] + w*P->UV[0][0];
      v = x*P->UV[1][1] + y*P->UV[2][1] + w*P->UV[0][1];
      u = fmod( u, 65536.0f ); v = fmod( v, 65536.0f );  // Hack!!
//      u = fmod( u*6, 65536.0f ); v = fmod( v*6, 65536.0f );  // Hack!!
   }
   u = ( Cur_Ray_Mat->Uo_Mip + u*Cur_Ray_Mat->Mip_Scale )/65536.0f;
   v = ( Cur_Ray_Mat->Vo_Mip + v*Cur_Ray_Mat->Mip_Scale )/65536.0f;
   Off = (INT)floor( u ) + 256*(INT)floor( v );

   if ( Cur_Ray_Mat->CMap==NULL )   // TXT_CACHE_II
   {
      USHORT *Ptr;
      Ptr = (USHORT*)Cur_Ray_Mat->Cur_Texture_Ptr;
      Ptr += Off;
      if ( Ray_Info.Do_UV_Antialias )
      {
         double ex, ey, fx, fy;
         USHORT r,g,b;
         ex = u - floor( u ); fx = 1.0-ex;
         ey = v - floor( v ); fy = 1.0-ey;
         COL_RGB( Ptr[0] );
         R = fx*fy*r;  G = fx*fy*g;  B = fx*fy*b;
         COL_RGB( Ptr[1] );
         R += ex*fy*r; G += ex*fy*g; B += ex*fy*b;
         COL_RGB( Ptr[256] );
         R += fx*ey*r; G += fx*ey*g; B += fx*ey*b;
         COL_RGB( Ptr[257] );
         R += ex*ey*r; G += ex*ey*g; B += ex*ey*b;
      }
      else
      {
         USHORT r,g,b;
         COL_RGB( Ptr[0] );
         R = 1.0f*r; G = 1.0f*r; B = 1.0f*r;
      }
   }
   else  // TXT_CACHE_I
   {
      PIXEL *Ptr;
      Ptr = Cur_Ray_Mat->Cur_Texture_Ptr;
      Ptr += Off;
      if ( Ray_Info.Do_UV_Antialias )
      {
         double ex, ey, fx, fy;
         INT Color;
         ex = u - floor( u ); fx = 1.0-ex;
         ey = v - floor( v ); fy = 1.0-ey;
         Color = 3*Ptr[0];
         R = fx*fy*Cur_Ray_Mat->CMap[Color];
         G = fx*fy*Cur_Ray_Mat->CMap[Color+1];
         B = fx*fy*Cur_Ray_Mat->CMap[Color+2];
         Color = 3*Ptr[1];
         R += ex*fy*Cur_Ray_Mat->CMap[Color];
         G += ex*fy*Cur_Ray_Mat->CMap[Color+1];
         B += ex*fy*Cur_Ray_Mat->CMap[Color+2];
         Color = 3*Ptr[256];
         R += fx*ey*Cur_Ray_Mat->CMap[Color];
         G += fx*ey*Cur_Ray_Mat->CMap[Color+1];
         B += fx*ey*Cur_Ray_Mat->CMap[Color+2];
         Color = 3*Ptr[257];
         R += ex*ey*Cur_Ray_Mat->CMap[Color];
         G += ex*ey*Cur_Ray_Mat->CMap[Color+1];
         B += ex*ey*Cur_Ray_Mat->CMap[Color+2];
      }
      else
      {
         INT Color;
         Color = 3*Ptr[0];
         R = 1.0*Cur_Ray_Mat->CMap[Color];
         G = 1.0*Cur_Ray_Mat->CMap[Color+1];
         B = 1.0*Cur_Ray_Mat->CMap[Color+2];
      }
   }
   SET_FCOLOR( Col, (FLT)( R/256.0 ), (FLT)( G/256.0 ), (FLT)( B/256.0 ), 1.0 );
   return;

Default:
   SET_FCOLOR( Col, Ray->x,Ray->y,0.0,1.0 );
   return;
}

EXTERN void Get_Mesh_Normal( RAY *Ray, OBJ_NODE *Node, VECTOR Normal )
{
   POLY *P;
   P = &((MESH*)Node->Data)->Polys[Ray->Id];
   nA_V( Normal, Node->Mo, P->No );
   Normalize( Normal );
}

/******************************************************************/

EXTERN INT Compute_Raytrace_Poly_Constants( OBJ_NODE *Node )
{
   MESH *Msh;

      // needs being called every time vertex are changed (morphing, for example)

   INT i;
   POLY *Poly;
   POLY_RAYTRACE_CST *Cst;

   if ( Node==NULL ) return( 0 );
   Msh = (MESH*)Node->Data;
   if ( Msh==NULL || Msh->Cst==NULL ) return( 0 );   // error ?!
   Poly = Msh->Polys;
   Cst = Msh->Cst;
   if ( Poly==NULL || Cst==NULL ) return( 0 );
   for( i=0; i<Msh->Nb_Polys; ++i, Poly++, Cst++ )
   {
      FLT Tmp, u1, v1;

      Poly->Flags |= POLY_RAYTRACE;
      Sub_Vector( Cst->PoP1, Msh->Vertex[ Poly->Pt[1] ], Msh->Vertex[ Poly->Pt[0] ] );
      Sub_Vector( Cst->PoP2, Msh->Vertex[ Poly->Pt[2] ], Msh->Vertex[ Poly->Pt[0] ] );
      Cst->U2 = Norm_Squared( Cst->PoP1 );
      Cst->V2 = Norm_Squared( Cst->PoP2 );
      Cst->UV = Dot_Product( Cst->PoP1, Cst->PoP2 );
      Tmp = Cst->U2*Cst->V2 - Cst->UV*Cst->UV;
      if ( Tmp!=0.0 ) Tmp = 1.0f/Tmp;
      Cst->U2 *= Tmp; Cst->V2 *= Tmp; Cst->UV *= Tmp;
      u1 = Dot_Product( Cst->PoP1, Msh->Vertex[ Poly->Pt[0] ] );
      v1 = Dot_Product( Cst->PoP2, Msh->Vertex[ Poly->Pt[0] ] );
      Cst->xo = -Cst->V2*u1 + Cst->UV*v1;
      Cst->yo =  Cst->UV*u1 - Cst->U2*v1;
   }
   return( 1 );
}

EXTERN void Set_Ray_Tracing_Constants( OBJ_NODE *Obj )
{
   switch ( Obj->Type )
   {
      case MESH_TYPE:
      {
         MESH *Msh;
         Msh = (MESH*)Obj->Data;
         Obj->Flags = (OBJ_FLAG)( Obj->Flags | OBJ_USE_RAYTRACE );
         Msh->Flags = (OBJ_FLAG)( Msh->Flags | OBJ_USE_RAYTRACE );
         M_Free( Msh->Cst );
         Msh->Cst = New_Fatal_Object( Msh->Nb_Polys, POLY_RAYTRACE_CST );
         Compute_Raytrace_Poly_Constants( Obj );
      }
      break;
      default:
         Obj->Flags |= OBJ_USE_RAYTRACE;
      break;
   }
}

/******************************************************************/
