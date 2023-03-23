/***********************************************
 *                 Radiosity                   *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "ray.h"
#include "rad_data.h"

#define RAD_SHOW_SAMPLES

/******************************************************************/

#define MAX_NEAREST_COUNT 10

typedef struct {
#pragma pack(1)

   INT    Wghts_Count;
   INT    Good_Count;
   VECTOR Pt, N;
   FLT    Cur_Error_Bound;

   FCOLOR Wght_Times_Illuminance[MAX_NEAREST_COUNT];
   FLT    Wght[MAX_NEAREST_COUNT];
   FLT    Distance[MAX_NEAREST_COUNT];
   INT    Close_Count;

} RAD_WGHT;

static RAD_INFO Rad_Info;

/******************************************************************/

static INT Rad_Average_Near( OCTREE_DATA *Node, void *Dummy )
{
   RAD_WGHT *W;
   VECTOR Delta, Delta_Unit;
   FLT D2, Dist, QRad, ri;
   FLT Error, Wght;
   FCOLOR Predict;
   INT Index;

   W = (RAD_WGHT *)Dummy;

   Sub_Vector( Delta, W->Pt, Node->Pt );
   D2 = Norm_Squared( Delta );
   ri = Node->Harmonic_Mean_Distance;
   QRad = ri*W->Cur_Error_Bound;
   if ( D2>QRad*QRad ) return( 1 );    // we're too far from sample point
   Dist = (FLT)sqrt( D2 );
   if ( Dist>0.000001 )
   {
      FLT iD, cos_diff;
      iD = 1.0f/Dist;
      Delta_Unit[0] = Delta[0]*iD;
      Delta_Unit[1] = Delta[1]*iD;
      Delta_Unit[2] = Delta[2]*iD;
      cos_diff = Dot_Product( Delta_Unit, Node->To_Nearest_Surface );
      if ( cos_diff>0.0 )
      {
         ri  = (1.0f-cos_diff)*ri;
         ri += cos_diff * Node->Nearest_Distance;
      }
   }
//   fprintf( stderr, "Dist=%f ri=%f  Error_Bound=%f\n", Dist, ri, W->Cur_Error_Bound );
   if ( Dist>ri*W->Cur_Error_Bound ) return( 1 );  // too far

   {
      FLT dir_diff;
      dir_diff = Dot_Product( W->N, Node->N );
      Error = 2.0f * (FLT)sqrt( fabs( 1.0-dir_diff ) ); // error rotate
      Error += Dist / ri;         // error translate
   }
   if ( Error > W->Cur_Error_Bound ) return( 1 );
   if ( Dist>0.000001 )
   {
      VECTOR TmpV;
      FLT In_Front;
      TmpV[0] = ( Node->N[0] + W->N[0] ) * 0.5f;
      TmpV[1] = ( Node->N[1] + W->N[1] ) * 0.5f;
      TmpV[2] = ( Node->N[2] + W->N[2] ) * 0.5f;
      Normalize( TmpV );
      In_Front = Dot_Product( TmpV, Delta_Unit );
      if ( In_Front<-0.05 ) return( 1 );
   }

      // saw method
   Wght = 1.0f - Error/W->Cur_Error_Bound;
   if ( Wght<0.001 ) return( 1 );

#if 1
   {
      Predict[0]  = Node->Illuminance[0];
      Predict[0] += Delta[0]*Node->drdx + Delta[1]*Node->drdy + Delta[2]*Node->drdz;
      Predict[1]  = Node->Illuminance[1];
      Predict[1] += Delta[0]*Node->dgdx + Delta[1]*Node->dgdy + Delta[2]*Node->dgdz;
      Predict[2]  = Node->Illuminance[2];
      Predict[2] += Delta[0]*Node->dbdx + Delta[1]*Node->dbdy + Delta[2]*Node->dbdz;
      Clamp_All_FColor( Predict );
   }   
#else
   Assign_FColor( Predict, Node->Illuminance );
#endif

#ifdef RAD_SHOW_SAMPLES
   if ( Rad_Info.Show_Rad_Smp )
      if ( Dist<Rad_Info.Max_Dist*.003 )
         { Predict[0] *= 7.0; Predict[1] *= 7.0; Predict[2] *= 7.0; }
#endif

   W->Good_Count++;
   if ( W->Close_Count<Rad_Info.Nearest_Count )
      Index = W->Close_Count++;
   else           // search for a lowest distance to discard
   {
      INT i;
      Index=-1;
      for( i=0; i<Rad_Info.Nearest_Count; i++ )
         if ( Dist<W->Distance[i] )
            { Index=i; break; }
      if ( Index==-1 ) return( 1 ); // no slot
   }

   W->Distance[Index] = Dist;
   W->Wght[Index] = Wght;
   Scale_FColor( W->Wght_Times_Illuminance[Index], Predict, Wght );

   return( 1 );
}

static INT Radiosity_ReUse( VECTOR Pt, VECTOR Normal, FCOLOR Result )
{
   INT i;
   RAD_WGHT W;
   FLT Total_Wght;

   if ( Rad_Info.Octree==NULL ) return( 0 );
   Cur_Octree = Rad_Info.Octree;
   if ( Cur_Octree->Root==NULL ) return( 0 );

   Assign_Vector( W.Pt, Pt );
   Assign_Vector( W.N, Normal );
   W.Wghts_Count = 0;
   W.Good_Count = 0;
   W.Close_Count = 0;
   W.Cur_Error_Bound = Rad_Info.Cur_Error_Bound;
   for( i=1; i<Rad_Info.Cur_Radiosity_Level; ++i )
      W.Cur_Error_Bound *= 1.4f;  // ?!?

         // we could start we last hit instead of Root, here...
   Octree_Distance_Traverse( Cur_Octree->Root, Pt, 
      Rad_Info.Cur_Radiosity_Level, Rad_Average_Near, (void*)&W );
   if ( W.Good_Count==0 ) return( 0 );    // no good values

   SET_FCOLOR( Result, 0.0, 0.0, 0.0, 1.0 );
   Total_Wght = 0.0;
   for( i=0; i<W.Close_Count; ++i )
   {
      Result[0] += W.Wght_Times_Illuminance[i][0];
      Result[1] += W.Wght_Times_Illuminance[i][1];
      Result[2] += W.Wght_Times_Illuminance[i][2];
      Total_Wght += W.Wght[i];
   }
   Total_Wght = 1.0f / Total_Wght;
   Scale_FColor_Eq( Result, Total_Wght );

   return( 1 );
}

/******************************************************************/

static void V_Unpack( VECTOR Dst, INT i )
{
   Dst[0] = 2.0f*Rad_Samples[i].x/255.0f - 1.0f;
   Dst[1] = 2.0f*Rad_Samples[i].y/255.0f - 1.0f;
   Dst[2] = 1.0f*Rad_Samples[i].z/255.0f;
}

static INT Radiosity_Gather( VECTOR Pt, VECTOR Normal, 
   FCOLOR Result )
{
   FLT iD_Sum, Save_Min_ReUse, Min_Dist, Mean_Dist;
   INT i, Cur_Radiosity_Count;
   INT Grad_Count, Nb_Samples;
   FLT Sum_Of_iD, Sum_Of_D, Tmp;
   FLT drdxs, dgdxs, dbdxs, drdys, dgdys, dbdys, drdzs, dgdzs, dbdzs;
   VECTOR n2, n3, Min_Dist_Dir;

   Cur_Radiosity_Count = Rad_Info.Max_Count;

   Save_Min_ReUse = Rad_Info.Min_ReUse;
   for( i=0 ;i<Rad_Info.Cur_Radiosity_Level; ++i )
   {
      Cur_Radiosity_Count /= 3;
      Rad_Info.Min_ReUse *= 2.0;
   }
   Choose_Ortho_Basis( Normal, n2, n3 );

   SET_FCOLOR( Result, 0.0, 0.0, 0.0, 1.0 );
   iD_Sum = 0.0;
   Min_Dist = _HUGE_;

   Grad_Count = Nb_Samples = 0;
   Sum_Of_iD = 0.0;
   Sum_Of_D = 0.0;
   drdxs = dgdxs = dbdxs = 0.0;
   drdys = dgdys = dbdys = 0.0;
   drdzs = dgdzs = dbdzs = 0.0;

   Rad_Info.Cur_Radiosity_Level++;

   for( i=0; i<Cur_Radiosity_Count; ++i )
   {
      VECTOR Rand_V;
      RAY New_Ray;
      FLT Depth, iD;

      INIT_RAY( &New_Ray );
      Assign_Vector( New_Ray.Orig, Pt );

      V_Unpack( Rand_V, i );
      if ( fabs( Normal[2]-1.0)<0.001 )
         { Assign_Vector( New_Ray.Dir, Rand_V ); }
      else
      {
         New_Ray.Dir[0] = n2[0]*Rand_V[0] + n3[0]*Rand_V[1] + Normal[0]*Rand_V[2];
         New_Ray.Dir[1] = n2[1]*Rand_V[0] + n3[1]*Rand_V[1] + Normal[1]*Rand_V[2];
         New_Ray.Dir[2] = n2[2]*Rand_V[0] + n3[2]*Rand_V[1] + Normal[2]*Rand_V[2];
      }
      Normalize( New_Ray.Dir );
      New_Ray.Z_Unit = 1.0;

      if ( Trace_Ray( &New_Ray, _RCst_.The_World->Root )==-1 ) continue;

      Depth = New_Ray.Z;
      if ( Depth<_DEPTH_EPSILON_ )
         continue;   // meaningless (self-)intersection?!

      Nb_Samples++;
      if ( Depth>Rad_Info.Max_Dist*10.0 )
         continue;      // point too far => contribution=0.0

      iD = 1.0f/Depth;
//      if ( Depth<Rad_Info.Max_Dist*10.0f )
      {
         FLT Tmp, d;
         Tmp = New_Ray.Dir[0]; d = iD*Tmp*Tmp; if ( Tmp<0.0 ) d = -d;
         drdxs += d*New_Ray.Col[0];
         dgdxs += d*New_Ray.Col[1];
         dbdxs += d*New_Ray.Col[2];
         Tmp = New_Ray.Dir[1]; d = iD*Tmp*Tmp; if ( Tmp<0.0 ) d = -d;
         drdys += d*New_Ray.Col[0];
         dgdys += d*New_Ray.Col[1];
         dbdys += d*New_Ray.Col[2];
         Tmp = New_Ray.Dir[2]; d = iD*Tmp*Tmp; if ( Tmp<0.0 ) d = -d;
         drdzs += d*New_Ray.Col[0];
         dgdzs += d*New_Ray.Col[1];
         dbdzs += d*New_Ray.Col[2];         
         Sum_Of_iD += iD;
         Sum_Of_D += Depth;
         Grad_Count++;
      }
      if ( Depth>Rad_Info.Max_Dist ) Depth = Rad_Info.Max_Dist;

      Add_FColor_Eq( Result, New_Ray.Col );
      iD_Sum += 1.0f/Depth;;
 
      if ( Depth<Min_Dist )
      {
         Min_Dist = Depth;
         Assign_Vector( Min_Dist_Dir, New_Ray.Dir );
      }
   }
         // restore state
   Rad_Info.Cur_Radiosity_Level--;
   Rad_Info.Min_ReUse = Save_Min_ReUse;
   if ( (Nb_Samples==0) || (iD_Sum<0.00001) )
      return( 0 );      // no intersection found...

      // collect result

   Mean_Dist = 1.0f*Nb_Samples/iD_Sum;
   Tmp = 1.0f/Nb_Samples;      // 1.0/Mean_Dist; ?!?
   Scale_FColor_Eq( Result, Tmp );
   Result[3] = 1.0;
//   Clamp_All_FColor( Result );

   if ( Mean_Dist>Rad_Info.Max_Dist*0.0001 ) // cache result in octree
   {
      OCTREE_ID Id;
      OCTREE_DATA *Data;

      if ( Mean_Dist<Rad_Info.Min_ReUse )
         Mean_Dist = Rad_Info.Min_ReUse;

      Octree_Find_Sphere( Pt, Mean_Dist*Rad_Info.Error_Bound, &Id );
      Data = (OCTREE_DATA*)(*Cur_Octree->Init_Data_Block)( );
      if ( Data==NULL ) return( 1 );
      Cur_Octree->Nb_Data++;

//      fprintf( stderr, "New: C=%.2f,%.2f,%.2f  Pt=%.3f,%.3f,%.3f  Mean_Dist = %f\n",
//         Result[0], Result[1], Result[2], Pt[0], Pt[1], Pt[2], Mean_Dist );
//      fprintf( stderr, "*" );
      if ( Grad_Count>10 ) // else the gradient are zeroed in Init_Data_Block()
      {
         FLT Tmp;
         Tmp = 1.0f * Grad_Count/( Sum_Of_D*Sum_Of_iD );
         Data->drdx = Tmp*drdxs;
         Data->dgdx = Tmp*dgdxs;
         Data->dbdx = Tmp*dbdxs;
         Data->drdy = Tmp*drdys;
         Data->dgdy = Tmp*dgdys;
         Data->dbdy = Tmp*dbdys;
         Data->drdz = Tmp*drdzs;
         Data->dgdz = Tmp*dgdzs;
         Data->dbdz = Tmp*dbdzs;
      }

      Assign_FColor( Data->Illuminance, Result );
      Assign_Vector( Data->To_Nearest_Surface, Min_Dist_Dir );
      Data->Harmonic_Mean_Distance = Mean_Dist;
      Data->Nearest_Distance = Min_Dist;
      Data->Bounce_Level = Rad_Info.Cur_Radiosity_Level;
      Assign_Vector( Data->Pt, Pt );
      Assign_Vector( Data->N, Normal );
      Data->Next = NULL;
      Octree_Insert( &Cur_Octree->Root, (OCTREE_DUMMY_DATA*)Data, &Id );      
   }
   return( 1 );
}

/******************************************************************/

EXTERN INT Compute_Ambient_Radiosity( VECTOR Pt, VECTOR Normal, FCOLOR
   Result )
{
   INT Ok_To_ReUse;
//   INT Save_Bound;

   if ( Rad_Info.Cur_Radiosity_Level>=Rad_Info.Max_Radiosity_Level )
      return( 0 );
   Cur_Octree = Rad_Info.Octree;
   if ( Rad_Info.Cur_Radiosity_Level==1 )
   {
      Rad_Info.Cur_Error_Bound = Rad_Info.Error_Bound;
   }

//   Save_Bound = Rad_Info.Cur_Error_Bound;
//   if ( Weight<0.25 )
//      Rad_Info.Cur_Error_Bound += ( 0.25-Weight );
    Ok_To_ReUse = Radiosity_ReUse( Pt, Normal, Result );

//   Rad_Info.Cur_Error_Bound = Save_Bound;
   if ( !Ok_To_ReUse )
   {
      if ( !Radiosity_Gather( Pt, Normal, Result ) )
         return( 0 );
   }

#if 0
   if ( Rad_Info.Cur_Radiosity_Level==1 )
   {
         // time to collect
      FLT Gray, x;

      Gray = ( Result[0] + Result[1] + Result[2] )/3.0;     // *Result[3] ?!?
      x = 1.0-Gray;
      x *= Rad_Info.Brightness;
      Gray *= Rad_Info.Brightness;
      Result[0] *= x; Result[1] *= x; Result[2] *= x;
      Gray *= Rad_Info.Gray_Level;
      if ( Gray>0.0 )
      {
         Result[0] += Gray; Result[1] += Gray; Result[2] += Gray;
      }
      Result[3] = 1.0;
         // Clamp Result[] ?!?
   }
#endif
   return( 1 );
}

/******************************************************************/

EXTERN RAD_INFO *Init_Radiosity( )
{
      // Defaults should be:
      //   * Max_Dist:   ||Camera_Pos - Target|| * .2
      //   * Brightness: 3.3
      //   * Max_Count: 100
      //   * Error_Bound: 0.4
      //   * Low_Error_Factor: 0.8     <= pour les previews
      //   * Min_Reuse: 0.015
      //   * Nearest_Count : 6      (au lieu de MAX=15?)
      //   * Recursion_Limit: 1

   Rad_Info.Max_Radiosity_Level = 3;    // default: 4?
   Rad_Info.Cur_Radiosity_Level = 1;
   Rad_Info.Brightness = 2.3f;
   Rad_Info.Gray_Level = 0.0;
   Rad_Info.Error_Bound = 0.06f; // <= default 0.6 ??
   Rad_Info.Cur_Error_Bound = 0.0;     // useless here
   Rad_Info.Min_ReUse = 0.05f;     // <= Default0.4  ?!
   Rad_Info.Max_Dist = 10;
   Rad_Info.Max_Count = 80;     // <= default: 100?
   Rad_Info.Nearest_Count = MAX_NEAREST_COUNT;
   Rad_Info.World = _RCst_.The_World;
   Rad_Info.Show_Rad_Smp = FALSE;

      // we could re-load an octree file here...

   Destroy_Octree( Rad_Info.Octree ); Rad_Info.Octree = NULL;
   Rad_Info.Octree = Create_Radiosity_Octree( /*sizeof( OCTREE_DATA )*/ );
   if ( Rad_Info.Octree==NULL ) return( FALSE );

   return( &Rad_Info );
}

EXTERN INT Close_Radiosity( )
{
      // we could save an octree file here...
   Destroy_Octree( Rad_Info.Octree ); Rad_Info.Octree = NULL;
   Rad_Info.World = NULL;
   return( TRUE );
}

/******************************************************************/
