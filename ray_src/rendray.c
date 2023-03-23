/***********************************************
 *          raytraced rendering                *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "ray.h"

EXTERN  RAYTRACER_INFO Ray_Info;

/******************************************************************/

   // Cur_Camera, _RCst_.The_Camera and _RCst_.The_Camera->Mo must be ok

EXTERN void Init_Camera_Ray( RAY *Ray, FLT u, FLT v )
{
   INIT_RAY( Ray );
   Ray->Orig[0] = Cur_Camera->Pos[0];
   Ray->Orig[1] = Cur_Camera->Pos[1];
   Ray->Orig[2] = Cur_Camera->Pos[2];
   Ray->Dir[0]  = Cur_Camera->D*_RCst_.The_Camera->Mo[6];
   Ray->Dir[1]  = Cur_Camera->D*_RCst_.The_Camera->Mo[7];
   Ray->Dir[2]  = Cur_Camera->D*_RCst_.The_Camera->Mo[8];
   Ray->Dir[0] -= u*_RCst_.The_Camera->Mo[0] + v*_RCst_.The_Camera->Mo[3];
   Ray->Dir[1] -= u*_RCst_.The_Camera->Mo[1] + v*_RCst_.The_Camera->Mo[4];
   Ray->Dir[2] -= u*_RCst_.The_Camera->Mo[2] + v*_RCst_.The_Camera->Mo[5];
   Normalize( Ray->Dir );
   Ray->Z_Unit = 1.0f;
   SET_FCOLOR( Ray->Col, 0.0, 0.0, 0.0, 1.0f );   // atmosphere?
}

EXTERN void Compute_Reflected_Ray( VECTOR Result, RAY *Ray )
{
   FLT Tmp;

   Tmp = 2.0f*Dot_Product( Ray->Dir, Ray->Normal );
   Result[0] = Ray->Dir[0] - Tmp*Ray->Normal[0];
   Result[1] = Ray->Dir[1] - Tmp*Ray->Normal[1];
   Result[2] = Ray->Dir[2] - Tmp*Ray->Normal[2];
}

/******************************************************************/

EXTERN void Ray_Check_Intersection( RAY *Ray )
{
      // hope Ray!=NULL and Ray->Node!=NULL, here...

   if ( !(Ray->Flags&RAY_HAS_NORMAL ) )
   {
      NODE_GET_NORMAL( Ray, Ray->Normal );
      Ray->Flags |= RAY_HAS_NORMAL;
   }
   if ( !(Ray->Flags&RAY_HAS_PT ) )
   {
      Add_Scaled_Vector( Ray->Pt, Ray->Orig,Ray->Dir, Ray->Z );
      Ray->Flags |= RAY_HAS_PT;      
   }
   Add_Scaled_Vector_Eq( Ray->Pt, Ray->Normal, _DEPTH_EPSILON_ );
   if ( !(Ray->Flags&RAY_HAS_COLOR ) )
   {
      RAY_GET_COLOR( Ray, Ray->Col );
   }
}

/******************************************************************/

EXTERN INT Intersect_Group_Union( RAY *Ray, OBJ_NODE *Node )
{
   OBJ_NODE *Cur;

   for( Cur=Node->Child; Cur!=NULL; Cur=Cur->Next )
   {
      RAY New_Ray;
      if ( Cur->Flags & Ray_Info.Filter_Flags ) continue;
      INIT_RAY( &New_Ray );
      Intersect_Node( &New_Ray, Node );
      if ( New_Ray.Id==-1 ) continue;
      if ( New_Ray.Z<Ray->Z ) { UPDATE_RAY( Ray, &New_Ray ); }
   }
   return( Ray->Id );
}

/******************************************************************/
/******************************************************************/

EXTERN void Choose_Ortho_Basis( VECTOR N, VECTOR n2, VECTOR n3 )
{
   VECTOR up;
   if ( fabs( fabs(N[2])-1.0 ) < 0.1 )
      { Set_Vector( up, 0.0, 1.0, 0.0 ); }
   else { Set_Vector( up, 0.0, 0.0, 1.0 ); }
   Cross_Product( n2, N, up ); Normalize( n2 );
   Cross_Product( n3, N, n2 ); Normalize( n3 );
}

EXTERN void Local_Ray( RAY *Out, RAY *In, MATRIX2 M )
{
   A_Inv_V( Out->Orig, M, In->Orig );
   nA_Inv_V( Out->Dir, M, In->Dir );
   Out->Z_Unit = (FLT)sqrt( Norm_Squared( Out->Dir ) );
//   Out->Z_Unit = 1.0f;
//   Normalize( Out->Dir );
}

/******************************************************************/

EXTERN INT Intersect_Node( RAY *Ray, OBJ_NODE *Node )
{
   RAY Ref_Ray;

   if ( !(Node->Flags&OBJ_USE_RAYTRACE) ) return( FALSE );

   if ( Node->Time_Stamp<_RCst_.Frame_Stamp )
   {
      Check_Node_Transform( Node );
      if ( Cur_Camera!=NULL )
         Node->Camera_ID = (USHORT)Cur_Camera->ID; // <= this isn't done yet
      else Node->Camera_ID++;
   }

   Ref_Ray = *Ray;
   switch( Node->Type )
   {
      case MESH_TYPE:
      {
         FLT Z_Min, Z_Max;
         Local_Ray( &Ref_Ray, Ray, Node->Mo );
         if ( Intersect_Box( &Ref_Ray, &Node->Box, &Z_Min, &Z_Max )==FALSE )
            return( FALSE );
         if ( Z_Min*Ref_Ray.Z_Unit > Ray->Z*Ray->Z_Unit ) return( FALSE );
         Ref_Ray.Z = Z_Max;
         if ( Z_Min*Ref_Ray.Z_Unit > Ray->Z_Min*Ray->Z_Unit )
            Z_Min = Ray->Z_Min*Ray->Z_Unit/Ref_Ray.Z_Unit;
         Ref_Ray.Z_Min = Z_Min;
         NODE_INTERSECT( &Ref_Ray, Node );
      }
      break;
      case CAMERA_TYPE: case LIGHT_TYPE: case MATERIAL_TYPE:
         return( FALSE );
      break;
      default: return( FALSE ); break;
   }
   if ( Ref_Ray.Id==-1 ) return( -1 ); // <= means: box intersect only
   UPDATE_RAY( Ray, &Ref_Ray )
   Ray->Z *= Ref_Ray.Z_Unit;
   Ray->Z /= Ray->Z_Unit;
   return( 1 );         // <= means: node intersect
}

/******************************************************************/
/******************************************************************/

      // Main recursive entries

EXTERN INT Compute_Reflected_Color( RAY *Ray, FCOLOR Result )
{
   RAY R_Ray;

   Assign_Vector( R_Ray.Orig, Ray->Pt );
   Compute_Reflected_Ray( R_Ray.Dir, Ray );
   Normalize( R_Ray.Dir );
   R_Ray.Z_Unit = 1.0f;

   if ( Trace_Ray( &R_Ray, _RCst_.The_World->Root ) != -1 )
   {
      Assign_Vector( Result, R_Ray.Col );
      return( TRUE );
   }
   else return( FALSE );
}

EXTERN INT Intersect_All_Nodes( RAY *Ray, OBJ_NODE *Node )
{
   for( ; Node!=NULL; Node=Node->Next )
   {
      RAY New_Ray;
      if ( Node->Flags & Ray_Info.Filter_Flags ) continue;
      New_Ray = *Ray;
      if ( Intersect_Node( &New_Ray, Node )!=FALSE )
      {
         if ( New_Ray.Z<Ray->Z ) { UPDATE_RAY( Ray, &New_Ray ); }
         if ( Node->Child!=NULL )
            Intersect_All_Nodes( Ray, Node->Child );
      }
   }
   return( Ray->Id );
}

EXTERN INT Intersect_All_Nodes_Except_Cached( RAY *Ray, OBJ_NODE *Cur, OBJ_NODE *First )
{
   for( ; Cur!=NULL; Cur=Cur->Next )
   {
      if ( Cur->Flags & Ray_Info.Filter_Flags ) continue;
      if ( Cur!=First )
      {
         RAY New_Ray;
         New_Ray = *Ray;
         if ( Intersect_Node( &New_Ray, Cur )!=FALSE )
         {
            if ( New_Ray.Z<Ray->Z )
               UPDATE_RAY( Ray, &New_Ray );
            if ( Cur->Child!=NULL )
            {
               New_Ray = *Ray;
               if ( Intersect_All_Nodes_Except_Cached( &New_Ray, Cur->Child, First )!= -1 )
               {
                  if ( New_Ray.Z<Ray->Z )
                     UPDATE_RAY( Ray, &New_Ray );
               }
            }
         }
      }
   } 
   return( Ray->Id );
}

EXTERN INT Intersect_All_Nodes_Cached( RAY *Ray, OBJ_NODE *Cur, OBJ_NODE *First )
{
   if ( (First!=NULL)&& !(First->Flags & Ray_Info.Filter_Flags) )
   {
      RAY New_Ray;      
      New_Ray = *Ray;
      if ( Intersect_Node( &New_Ray, First )==TRUE )
      {
         if ( New_Ray.Z<Ray->Z )
            UPDATE_RAY( Ray, &New_Ray );
         return( Ray->Id );
      }
   }
   for( ;Cur!=NULL; Cur=Cur->Next )
   {
      if ( Cur->Flags & Ray_Info.Filter_Flags ) continue;
      if ( Cur!=First )
      {
         RAY New_Ray;
         New_Ray = *Ray;
         if ( Intersect_Node( &New_Ray, Cur )!=FALSE )
         {
            if ( New_Ray.Z<Ray->Z )
               UPDATE_RAY( Ray, &New_Ray );
            if ( Cur->Child!=NULL )
            {
               New_Ray = *Ray;
               if ( Intersect_All_Nodes_Except_Cached( &New_Ray, Cur->Child, First ) != -1 )
               {
                  if ( New_Ray.Z<Ray->Z )
                     UPDATE_RAY( Ray, &New_Ray );
               }
            }
         }
      }
   } 
   return( Ray->Id );
}

/******************************************************************/

EXTERN void Compute_Intersection_Color( RAY *Ray )
{
   FLT Total_W;

      // collect colors
      // + constant ambient color. Weight: Ambient

   if ( Cur_Ray_Mat!=NULL && Ray_Info.Cst_Ambient>0.0 )
   {
      Scale_FColor_Eq( Ray->Col, 1.0f-Ray_Info.Cst_Ambient );
      Add_Scaled_FColor_Eq( Ray->Col, Cur_Ray_Mat->Diffuse, Ray_Info.Cst_Ambient );
   }
   if ( !(Ray->Node->Flags&OBJ_DONT_RECEIVE_SHDW) )
   {
      Scale_FColor_Eq( Ray->Col, Ray_Info.Ambient );
      Total_W = Ray_Info.Ambient;
   }
   else
   {
      Scale_FColor_Eq( Ray->Col, Ray_Info.Ambient+Ray_Info.Diffuse );
      Total_W = Ray_Info.Ambient+Ray_Info.Diffuse;
      goto Skip_Light;
   }

      // + light color

   if ( Ray_Info.Diffuse>0.0 )
   {
      FCOLOR Light_Col;

      if ( Ray_Info.Do_Light )
      {
         if ( Ray_Trace_Lights( Ray, Light_Col ) )
         {
            Add_FColor_Eq( Ray->Col, Light_Col );
            Total_W += Ray_Info.Diffuse;
         }
      }
      else
      {
         Compute_Light_Diffuse( Ray, Light_Col );
         Add_FColor_Eq( Ray->Col, Light_Col );
         Total_W += Ray_Info.Diffuse;
      }
   }
Skip_Light:

      // + ambient radiosity color

   if ( Ray_Info.Do_Radiosity && Ray_Info.Radiosity>0.0 )
   {
      FCOLOR Rad_Color;
      // Scale_FColor_Eq( Ray->Col, 1.0-Ray_Info.Radiosity );
      if ( Compute_Ambient_Radiosity( Ray->Pt, Ray->Normal, Rad_Color ) )
      {
         Add_Scaled_FColor_Eq( Ray->Col, Rad_Color, Ray_Info.Radiosity );
      }
      Total_W += Ray_Info.Radiosity;
   }

      // Reflection
   if ( Ray_Info.Reflect>0.0 )
   {
      FCOLOR R_Color;
      if ( Compute_Reflected_Color( Ray, R_Color )==TRUE )
      {
         Add_Scaled_FColor_Eq( Ray->Col, R_Color, Ray_Info.Reflect );            
      }
      Total_W +=  Ray_Info.Reflect;
   }      
//      if ( Total_W>0.0 )
//         Scale_FColor_Eq( Ray->Col, 1.0/Total_W );
   Clamp_All_FColor( Ray->Col );
}

/******************************************************************/

EXTERN INT Trace_Ray( RAY *Ray, OBJ_NODE *Node )
{
   if ( Ray_Info.Cur_Trace_Level >= Ray_Info.Max_Trace_Level )
      return( -1 );

   INIT_RAY( Ray );
   Cur_Ray_Mat = NULL;

   Ray_Info.Cur_Trace_Level++;
   Intersect_All_Nodes( Ray, Node );

   if ( Ray->Node!=NULL )
   {
         // Jump to intersection point once for all.
         // Ray->Col is updated from texture map only, here.
         // => Base ambient (=texture)

      Ray_Check_Intersection( Ray );
      Compute_Intersection_Color( Ray );
   }

   Ray_Info.Cur_Trace_Level--;
   return( Ray->Id );
}

/******************************************************************/

EXTERN  RAYTRACER_INFO *Init_Raytracer( )
{
   Ray_Info.Do_Light = FALSE;
   Ray_Info.Light_Node = NULL;
   Ray_Info.Do_Radiosity = FALSE;
   Ray_Info.Do_UV_Antialias = FALSE;
   Ray_Info.Antialias = 0.0;   
   Ray_Info.Max_Trace_Level = 4;
   Ray_Info.Cur_Trace_Level = 1;
   Ray_Info.Ambient = .5f;
   Ray_Info.Cst_Ambient = 0.2f;
   Ray_Info.Radiosity = 1.7f;     // ?!
   Ray_Info.Diffuse = 0.4f;
   Ray_Info.Reflect = 0.0;
   Ray_Info.Transmit = 0.0;

   Ray_Info.Light_Extend = 0.0;
   Ray_Info.Light_Jitter = 0.05f;
   Ray_Info.Light_Error_Thresh = 0.00;
   Ray_Info.Light_Fall = 0.00;

   Ray_Info.Filter_Flags = 0x00000000; // <= OBJ_NO_SHADOW
   Ray_Info.Backface_Cull = FALSE;

      // plug methods in objects

   _GROUP_.Intersect = Intersect_Group_Union;

   _MESH_.Intersect = Intersect_Mesh;
   _MESH_.Get_Color = Get_Mesh_Color;
   _MESH_.Get_Normal = Get_Mesh_Normal;
   _MESH_.Get_Materials = Get_Mesh_Ray_Materials;
   _MESH_.Init_Ray_Constants = Compute_Raytrace_Poly_Constants;

   _LIGHT_.Get_Color = Get_Light_Ray_Color;   

   return( &Ray_Info );
}

/******************************************************************/
/******************************************************************/

