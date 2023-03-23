/***********************************************
 *        Light intersections ray              *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "ray.h"

static INT Nb_Lights;
static FCOLOR Light_Col;

/******************************************************************/

static FLT Init_Light_Ray_Dir( RAY *Ray, OBJ_NODE *L )
{
   LIGHT *Light;
   FLT Norm, Dist;

   Light = (LIGHT*)L->Data;

   Assign_Vector( Ray->Dir, Light->Pos );
   Sub_Vector_Eq( Ray->Dir, Ray->Orig );
   Dist = (FLT)sqrt( Norm_Squared( Ray->Dir ) );
//   if ( Dist>Light->Z_Max || Dist<Light->Z_Min ) return( -1.0 );
   if ( Dist>Light->Z_Max ) return( -1.0 );
   Ray->Z = Dist;    // sets range for this light...
   Ray->Z_Unit = 1.0f;
   Norm = 1.0f/Dist;
   Scale_Vector_Eq( Ray->Dir, Norm );
   Add_Scaled_Vector_Eq( Ray->Orig, Ray->Dir, _LIGHT_EPSILON_ );

   return( Dist );
}

EXTERN void Get_Light_Ray_Color( RAY *Ray, OBJ_NODE *Node, FCOLOR Col )
{
   LIGHT *Light;

   Light = (LIGHT*)Node->Data;
   Col[0] = Light->Color[0]*Light->Mult;
   Col[1] = Light->Color[1]*Light->Mult;
   Col[2] = Light->Color[2]*Light->Mult;
   Col[3] = Light->Color[3]*Light->Mult;
}

static FLT Do_Light_Fall( FLT D, LIGHT *Light )
{
   FLT t;
   t = ( Light->Z_Max - Light->Z_Min );
   if ( t<0.0000001 ) return( 1.0f );
   else t = ( D - Light->Z_Min) / t;
   if ( Ray_Info.Light_Fall!=1.0f ) t = (FLT)pow( t, Ray_Info.Light_Fall );
   return( 1.0f-t );
}
 
/******************************************************************/

static void Deal_With_Light( OBJ_NODE *L, RAY *Ray )
{
   RAY Light_Ray;
   LIGHT *Light;
   FLT Angle, C_Dist;

   Light = (LIGHT*)L->Data;

   INIT_RAY( &Light_Ray );
   Assign_Vector( Light_Ray.Orig, Ray->Pt ); // <= I_Pt here!!
   C_Dist = Init_Light_Ray_Dir( &Light_Ray, L );
   if ( C_Dist<0.0 ) return;   // too far

   Nb_Lights++;

   Angle = Dot_Product( Ray->Normal, Light_Ray.Dir );
   if ( Angle<0.0 ) return;    // Double sided?!?

   if ( Ray_Info.Light_Fall>0.0 )
      Angle *= Do_Light_Fall( C_Dist, Light );

   if ( Light->Light_Type == LIGHT_SPOT )
   {
      FLT Angle2;
      Angle2 = -Dot_Product( Light_Ray.Dir, Light->Dir );
      if ( Angle2<Light->Cos_Fall ) return;
      if ( Angle2<Light->Cos_Hot )
      {
         Angle2 = 1.0f - (Angle2-Light->Cos_Fall)*Light->Out_fall;
         Angle *= 1.0f - Angle2*Angle2;
      }
   }

   if ( Light->Last_Viewed==NULL )
      Intersect_All_Nodes( &Light_Ray, _RCst_.The_World->Root );
   else Intersect_All_Nodes_Cached( &Light_Ray, 
      _RCst_.The_World->Root, Light->Last_Viewed );

   if ( Light_Ray.Id!=-1 )
   {
         // TODO: Don't cache transparent objects here!!
      Light->Last_Viewed = Light_Ray.Node;
      return;  // shadowed
   }
   else Light->Last_Viewed = NULL;

   NODE_GET_COLOR( L, Light_Ray.Col );
   Angle *= Ray_Info.Diffuse;    // <= should be poly's diffuse here
   Add_Scaled_FColor_Eq( Light_Col, Light_Ray.Col, Angle );
}

/******************************************************************/
/******************************************************************/

#define MAX_LIGHT_SAMPLES  19
static FLT Light_Sample_Pos[MAX_LIGHT_SAMPLES][2] =
{
   { .750f, .433f}, { .000f, .866f}, {-.750f, .433f},
   {-.750f,-.433f}, { .000f,-.866f}, { .750f,-.433f},
   { .000f, .000f},
   { .750f, .000f}, { .375f, .650f}, {-.375f, .650f},
   {-.750f, .000f}, {-.375f,-.650f}, { .375f,-.650f},
   { .375f, .216f}, { .000f, .433f}, {-.375f, .217f},
   {-.375f,-.216f}, { .000f,-.433f}, { .375f,-.217f}
};

static void Jitter_Light_Ray( VECTOR Result, INT Nb, VECTOR L, FLT C_Dist, FLT C_Rad )
{
   FLT dx, dy, mu;
   VECTOR Ray;

   Ray[0] = C_Rad*Light_Sample_Pos[Nb][0];
   Ray[1] = C_Rad*Light_Sample_Pos[Nb][1];
   mu = C_Rad*Ray_Info.Light_Jitter;
   if ( mu>0.0 )
   {
      Ray[0] += mu*( 2.0f*(random()&0xFF)/255.0f - 1.0f );
      Ray[1] += mu*( 2.0f*(random()&0xFF)/255.0f - 1.0f );
   }   
   Ray[2] = C_Dist;
   mu = L[1]*L[1] + L[2]*L[2];
   if ( mu>0.0 )
   {
      mu = (FLT)sqrt( mu );
      Result[0] = mu*Ray[0] + L[0]*Ray[2];
      mu = 1.0f/mu;
      dx = Ray[2] - L[0]*mu*Ray[0];
      dy = mu*Ray[1];
      Result[1] = dx*L[1] + L[2]*dy;
      Result[2] =-dy*L[1] + L[2]*dx;
      Normalize( Result );
   }
   else
   {
      Result[1] = Result[2] = 0.0;
      Result[0] = 1.0;
   }
}

static void Deal_With_Extended_Light( OBJ_NODE *L, RAY *Ray )
{
   RAY Light_Ray;
   LIGHT *Light;
   FLT Visible, Theta, C_Rad, C_Dist;
   INT i, Count;
   VECTOR Base_Dir;
   OBJ_NODE *Last_Cached;

   Light = (LIGHT*)L->Data;

   INIT_RAY( &Light_Ray );
   Assign_Vector( Light_Ray.Orig, Ray->Pt ); // <= I_Pt here!!
   C_Dist = Init_Light_Ray_Dir( &Light_Ray, L );
   if ( C_Dist<=0.0 ) return; // too far
   if ( Ray_Info.Light_Fall>0.0 )
      Visible = Do_Light_Fall( C_Dist, Light );
   else Visible = 1.0;

   Assign_Vector( Base_Dir, Light_Ray.Dir );
   Theta = (FLT)asin( (double)Ray_Info.Light_Extend/C_Dist );
   C_Rad = Ray_Info.Light_Extend*(FLT)cos( (double)Theta );
   C_Dist = C_Dist - Ray_Info.Light_Extend*(FLT)sin( (double)Theta );

   Nb_Lights++;

   Visible *= Dot_Product( Ray->Normal, Light_Ray.Dir );
   if ( Visible<0.0 ) return;    // Double sided?!?

   if ( Light->Light_Type == LIGHT_SPOT )
   {
      FLT Visible2;
      Visible2 = -Dot_Product( Light_Ray.Dir, Light->Dir );
      if ( Visible2<Light->Cos_Fall ) return;
      if ( Visible2<Light->Cos_Hot )
      {
         Visible2 = 1.0f - (Visible2-Light->Cos_Fall)*Light->Out_fall;
         Visible *= 1.0f - Visible2*Visible2;
      }
   }

   Last_Cached = NULL; // Light->Last_Viewed;

   Count=MAX_LIGHT_SAMPLES;
   for( i=0; i<MAX_LIGHT_SAMPLES; ++i )
   {
      Light_Ray.Z_Min = 0.0;
      Light_Ray.Id = -1;
      Light_Ray.Z = C_Dist;
      Jitter_Light_Ray( Light_Ray.Dir, i, Base_Dir, C_Dist, C_Rad );

         // TODO: Fix this mess!!
#if 0
      if ( Light->Last_Viewed!=NULL )
         Intersect_All_Nodes_Cached( &Light_Ray, 
            _RCst_.The_World->Root, Light->Last_Viewed );
      else if ( Last_Cached!=NULL )
         Intersect_All_Nodes_Cached( &Light_Ray, 
            _RCst_.The_World->Root, Last_Cached );
      else Intersect_All_Nodes( &Light_Ray, _RCst_.The_World->Root );
#endif
      if ( Last_Cached!=NULL )
         Intersect_All_Nodes_Cached( &Light_Ray, 
            _RCst_.The_World->Root, Last_Cached );
      else Intersect_All_Nodes( &Light_Ray, _RCst_.The_World->Root );

      if ( Light_Ray.Id!=-1 ) 
      {
         Count--; // shadowed
         Last_Cached = Light_Ray.Node;
      }
      else Last_Cached = NULL; // Light->Last_Viewed;

      if ( i==6 )
      {
         if ( Count==12 )
         {
            Count = 0;
            break;
         }
         else if ( Count==19 ) break;
      }
   }
   Light->Last_Viewed = NULL; // Last_Cached;

   if ( Count )
   {
      FCOLOR Sum_Light_Col;
      NODE_GET_COLOR( L, Sum_Light_Col );
      Visible *= (1.0f/19.0f)*Count;
      Visible *= Ray_Info.Diffuse;    // should be poly's diffuse here
      Add_Scaled_FColor_Eq( Light_Col, Sum_Light_Col, Visible );
   }
}

/******************************************************************/
/******************************************************************/

static void Traverse_Light_Nodes( OBJ_NODE *Cur, RAY *Ray )
{
   for( ;Cur!=NULL; Cur=Cur->Next )   
   {
      if ( Cur->Flags & OBJ_DONT_RENDER ) continue;
      if ( Cur->Type==LIGHT_TYPE )
      {
         if ( Ray_Info.Light_Extend>0.0 )
            Deal_With_Extended_Light( Cur, Ray );
         else Deal_With_Light( Cur, Ray );
      }
      if ( Cur->Child!=NULL ) Traverse_Light_Nodes( Cur->Child, Ray );
   }
}

EXTERN INT Ray_Trace_Lights( RAY *Ray, FCOLOR Result )
{
   Nb_Lights = 0;
   SET_FCOLOR( Light_Col, 0.0, 0.0, 0.0, 1.0 );  // <= Ambiant base color

   // PLug filter flags
   Ray_Info.Filter_Flags = OBJ_NO_SHADOW;
   Traverse_Light_Nodes( _RCst_.The_World->Root, Ray );
   Ray_Info.Filter_Flags = 0x00000000;

   if ( Nb_Lights>0 )
   {
      Assign_FColor( Result, Light_Col );
      Result[3] = 1.0;
//      Scale_FColor( Result, Light_Col, 1.0/Nb_Lights );
   }
   else { SET_FCOLOR( Result, 0.0, 0.0, 0.0, 1.0 ); } // <= Ambiant base color?

   return( Nb_Lights );
}

/******************************************************************/
/******************************************************************/

static void Deal_With_Diffuse_Light( OBJ_NODE *L, RAY *Ray )
{
   RAY Light_Ray;
   LIGHT *Light;
   FLT Angle, C_Dist;

   Light = (LIGHT*)L->Data;

   INIT_RAY( &Light_Ray );
   Assign_Vector( Light_Ray.Orig, Ray->Pt ); // <= I_Pt here!!
   C_Dist = Init_Light_Ray_Dir( &Light_Ray, L );
   if ( C_Dist<0.0 ) return;   // too far

   Nb_Lights++;

   Angle = Dot_Product( Ray->Normal, Light_Ray.Dir );
   if ( Angle<0.0 ) return;    // Double sided?!?

   if ( Ray_Info.Light_Fall>0.0 )
      Angle *= Do_Light_Fall( C_Dist, Light );

   if ( Light->Light_Type == LIGHT_SPOT )
   {
      FLT Angle2;
      Angle2 = -Dot_Product( Light_Ray.Dir, Light->Dir );
      if ( Angle2<Light->Cos_Fall ) return;
      if ( Angle2<Light->Cos_Hot )
      {
         Angle2 = 1.0f - (Angle2-Light->Cos_Fall)*Light->Out_fall;
         Angle *= 1.0f - Angle2*Angle2;
      }
   }
   NODE_GET_COLOR( L, Light_Ray.Col );

   Angle *= Ray_Info.Diffuse;    // <= should be poly's diffuse here
   Light_Col[0] += Light_Ray.Col[0]*Angle;
   Light_Col[1] += Light_Ray.Col[1]*Angle;
   Light_Col[2] += Light_Ray.Col[2]*Angle;
   Light_Col[3] *= Light_Ray.Col[3];
}


static void Traverse_Diffuse_Light_Nodes( OBJ_NODE *Cur, RAY *Ray )
{
   for( ;Cur!=NULL; Cur=Cur->Next )   
   {
      if ( Cur->Flags & OBJ_DONT_RENDER ) continue;
      if ( Cur->Type==LIGHT_TYPE )
      {
         Deal_With_Diffuse_Light( Cur, Ray );
      }
      if ( Cur->Child!=NULL ) Traverse_Light_Nodes( Cur->Child, Ray );
   }
}

EXTERN INT Compute_Light_Diffuse( RAY *Ray, FCOLOR Result )
{
   Nb_Lights = 0;
   SET_FCOLOR( Light_Col, 0.0, 0.0, 0.0, 1.0 );  // <= Ambiant base color

   Traverse_Diffuse_Light_Nodes( _RCst_.The_World->Root, Ray );

   if ( Nb_Lights>0 )
   {
      Assign_FColor( Result, Light_Col );
      Result[3] = 1.0;
//      Scale_FColor( Result, Light_Col, 1.0/Nb_Lights );
   }
   else { SET_FCOLOR( Result, 0.0, 0.0, 0.0, 1.0 ); } // <= Ambiant base color?

   return( Nb_Lights );
}

/******************************************************************/

EXTERN INT Ray_Trace_One_Light( RAY *Ray, FCOLOR Result, OBJ_NODE *L )
{
   Nb_Lights = 0;
   SET_FCOLOR( Light_Col, 0.0, 0.0, 0.0, 1.0 );  // <= Ambiant base color

   if ( Ray_Info.Light_Extend>0.0 )
      Deal_With_Extended_Light( L, Ray );
   else Deal_With_Light( L, Ray );
   Assign_FColor( Result, Light_Col );
   return( Nb_Lights );
}

/******************************************************************/
