/***********************************************
 *         3DS Animation                       *
 * heavily derived from spline_2.c code by     *
 * -Timur Davidenko (Adept/Esteem)-            *
 *  (adept@aquanet.co.il)                      *
 * and from the excellent doc written by       *
 *  -Jochen Wilhelmy (digisnap)-               *
 *  (digisnap@cs.tu-berlin.de)                 *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"

typedef enum { LEFT_T, RIGHT_T, PT } SPLINE_CASE;

/******************************************************************/

static double Do_Ease( double Val, double In, double Out )
{
   double S, k;

   S = In + Out;
   if ( S == 0.0 ) return( Val );
   if ( S>1.0 )
   {
      S = 1.0/S; In *= S; Out *= S; S = 1.0;
   }
   k = 1.0 / ( 2.0-S );
   if ( Val < In ) return( (k/In)*Val*Val );
   else
   {
      if ( Val < 1.0-Out )
      {
         return( k*( 2.0*Val - In ) );
      }
      else
      {
         Val = 1.0-Val;
         return( 1.0 - (k/Out)*Val*Val );
      }
   }
}

static INT Fetch_Spline_Key( FLT *T, SPLINE_3DS *Sp )
{
   INT n;
   FLT t, Frames;

   t = *T;

      // Sp->Nb_Keys shouldn't be <2, here...
      // use Void_Spline() if it's the case.

   if ( Sp->Flags & TRACK_LOOP )
   {
      t = (FLT)fmod( t, Sp->Keys[Sp->Nb_Keys-1].Frame );
   }

   if ( t>=Sp->Keys[Sp->Nb_Keys-1].Frame ) 
   { 
      n = Sp->Nb_Keys-1;
      t = Sp->Keys[n].Frame;
      goto Ok;
   }
   else if ( t<Sp->Keys[0].Frame )
   { 
      n = 0; 
      t = Sp->Keys[0].Frame; 
      goto Ok;
   }
   n=Sp->Cache;
   if ( Sp->Keys[n].Frame>t ) n=0;
   while( Sp->Keys[n+1].Frame < t ) n++;

   if ( n>Sp->Nb_Keys-2 )
   { n = Sp->Nb_Keys-2; t = Sp->Keys[n+1].Frame; }

Ok:
   Sp->Cache = n;
   if ( n!=Sp->Nb_Keys-1 )
   {
      Frames = Sp->Keys[n+1].Frame - Sp->Keys[n].Frame;
      if ( Frames>0.0 )
        t = ( t-Sp->Keys[n].Frame )/Frames;
      else t = 0.0;
      *T = (FLT)Do_Ease( t, Sp->Keys[n].Ease_Out, Sp->Keys[n+1].Ease_In );
      return( n );
   }
   else { *T = t; return( -1 ); }   
}

/******************************************************************/

static void Get_Spline_Tangent( SPLINE_3DS *Sp, FLT *Result,
   INT Cur, SPLINE_CASE Case )
{
   INT i;
   A_KEY *V_Next, *V_Prev, *V_Cur;
   double Cont;

   V_Cur = &Sp->Keys[Cur];

   if ( Case==PT )
   {
      memcpy( Result, V_Cur->Values, Sp->Data_Size*sizeof( Result[0] ) );
      return;
   }

   if ( Cur==0 )  // First point
   {
      V_Next = &Sp->Keys[1];
      if ( Sp->Nb_Keys==2 )
      {
         for( i=Sp->Data_Size-1; i>=0; --i )
            Result[i] = ( V_Next->Values[i] - V_Cur->Values[i] )*( 1.0f-V_Cur->Tens );
         return;
      }
      if ( Sp->Flags&TRACK_LOOP )
      {
         V_Prev = &Sp->Keys[Sp->Nb_Keys-2];
      }
      else
      {
         FLT Tmp[4];
         Get_Spline_Tangent( Sp, Tmp, 1, LEFT_T );
         for( i=Sp->Data_Size-1; i>=0; --i )
            Result[i] = ( ( V_Next->Values[i] - V_Cur->Values[i] )*1.5f
               - Tmp[i]*0.5f ) * ( 1.0f-V_Cur->Tens );
         return;
      }
   }
   else if ( Cur==Sp->Nb_Keys-1 )   // Last Point
   {
      V_Prev = &Sp->Keys[Cur-1];
      if ( Sp->Nb_Keys==2 )
      {
         for( i=Sp->Data_Size-1; i>=0; --i )
            Result[i] = ( V_Cur->Values[i] - V_Prev->Values[i] )*( 1.0f-V_Cur->Tens );
         return;
      }
      if ( Sp->Flags&TRACK_LOOP )
      {
         V_Next = &Sp->Keys[1];
      }
      else
      {
         FLT Tmp[4];
         Get_Spline_Tangent( Sp, Tmp, Cur-1, RIGHT_T );
         for( i=Sp->Data_Size-1; i>=0; --i )
            Result[i] = ( ( V_Cur->Values[i] - V_Prev->Values[i] )*1.5f
               - Tmp[i]*0.5f ) * ( 1.0f-V_Cur->Tens );
         return;
      }
   }
   else
   {
      V_Prev = &Sp->Keys[Cur-1];
      V_Next = &Sp->Keys[Cur+1];
   }

   if ( Case==RIGHT_T ) Cont = 0.5*( 1.0-V_Cur->Cont );     // bn
   else Cont = 0.5*( 1.0+V_Cur->Cont );                     // an

   for( i=Sp->Data_Size-1; i>=0; --i )
   {
      double G1, G2;
      G1 = ( V_Cur->Values[i] - V_Prev->Values[i] )*( 1.0 + V_Cur->Bias );
      G2 = ( V_Next->Values[i] - V_Cur->Values[i] )*( 1.0 - V_Cur->Bias );
      G1 = ( G1+(G2-G1)*Cont ) * ( 1.0-V_Cur->Tens );
      Result[i] = (FLT)G1;
   }
}

EXTERN void Setup_Spline_3ds_Derivates( SPLINE_3DS *Sp )
{
   INT i;

   if ( Sp==NULL ) return;

   Sp->Cache = 0;
   if ( Sp->Nb_Keys<2 ) return;
   for( i=0; i<Sp->Nb_Keys; ++i )
   {
      Get_Spline_Tangent( Sp, Sp->Keys[i].Left, i, LEFT_T );
      Get_Spline_Tangent( Sp, Sp->Keys[i].Right, i, RIGHT_T );
   }
}

/******************************************************************/

EXTERN void Void_Spline( SPLINE_3DS *Sp, FLT *Result )
{
   INT i;

   if ( Sp->Nb_Keys==0 )
   {
      for( i=Sp->Data_Size-1; i>=0; --i ) Result[i] = 0.0;  // Hope it's ok.. 

   }
   else if ( Sp->Nb_Keys==1 )
   {
      for( i=Sp->Data_Size-1; i>=0; --i )
         Result[i] = Sp->Keys[0].Values[i];
   }
}

static void Hermite_Coeffs( double *h, FLT T )
{
   double T2, S, S2;

   T2 = T*T; S = 1.0-T; S2 = S*S;

   h[1] = T2*(3.0-2.0*T);
   h[0] = 1.0-h[1];       // h[0] = S2*(1.0+2.0*T);
   h[2] = T*S2;
   h[3] = -T2*S;
}

EXTERN void Interpolate_Spline_3ds_Values( FLT T, SPLINE_3DS *Sp, FLT *Result )
{
   double h[4];
   INT i, n;

   if ( Sp==NULL ) return;

   if ( Sp->Nb_Keys<=1 ) { Void_Spline( Sp, Result ); return; }

   n = Fetch_Spline_Key( &T, Sp );
   if ( n==-1 )
   {
      for( i=0; i<Sp->Data_Size; ++i )
         Result[i] = Sp->Keys[Sp->Nb_Keys-1].Values[i];
      return;
   }

   Hermite_Coeffs( h, T );

   for( i=0; i<Sp->Data_Size; ++i )
   {
      double T;
      T  = h[0]*Sp->Keys[n].Values[i];
      T += h[1]*Sp->Keys[n+1].Values[i];
      T += h[2]*Sp->Keys[n].Right[i];
      T += h[3]*Sp->Keys[n+1].Left[i];
      Result[i] = (FLT)T;
   }
}

#ifndef _SKIP_HIDE_

      // very special track for hidden objects
EXTERN INT Interpolate_Spline_3ds_Hidden( FLT T, SPLINE_3DS *Sp )
{
   INT n;

   if ( Sp==NULL ) return( FALSE );

   if ( Sp->Nb_Keys<=1 ) return( (INT)Sp->Keys[0].Values[0] );

   n = Fetch_Spline_Key( &T, Sp );
   if ( n==-1 ) return( (INT)Sp->Keys[Sp->Nb_Keys-1].Values[0] );

   return( (INT)Sp->Keys[n].Values[0] );
}

#endif   // _SKIP_HIDE_

/******************************************************************/

static void Get_Rot_Spline_Tangent( SPLINE_3DS *Sp, FLT *Result,
   INT Cur, SPLINE_CASE Case )
{
   INT i;
   A_KEY *V_Next, *V_Prev, *V_Cur;
   FLT Cont, Tens;
   QUATERNION G1, G2, G3;

   V_Cur = &Sp->Keys[Cur];

   if ( Case==PT )
   {
      for( i=Sp->Data_Size-1; i>=0; --i )
         Result[i] = V_Cur->Values[i];
      return;
   }

   if ( Cur==0 )  // First point
   {
      V_Next = &Sp->Keys[1];
      if ( !(Sp->Flags&TRACK_LOOP) || (Sp->Nb_Keys<=2) )
      {
         Quaternion_Linear_Interp( Result, 
            V_Cur->Values, V_Next->Values, 
               (1.0f-V_Cur->Tens)*(1.0f-V_Cur->Cont*V_Cur->Bias)/3.0f );
         return;
      }
      else
      {
         V_Prev = &Sp->Keys[Sp->Nb_Keys-2];
      }
   }
   else if ( Cur==Sp->Nb_Keys-1 )   // Last Point
   {
      V_Prev = &Sp->Keys[Cur-1];
      if ( !(Sp->Flags&TRACK_LOOP) || (Sp->Nb_Keys<=2) )
      {
         Quaternion_Linear_Interp( Result, 
            V_Cur->Values, V_Prev->Values, 
               (1.0f-V_Cur->Tens)*(1.0f-V_Cur->Cont*V_Cur->Bias)/3.0f ); 
         return;
      }
      else
      {
         V_Next = &Sp->Keys[1];
      }
   }
   else
   {
      V_Prev = &Sp->Keys[Cur-1];
      V_Next = &Sp->Keys[Cur+1];
   }

   Quaternion_Linear_Interp( G1, V_Cur->Values, V_Prev->Values, -(1.0f+V_Cur->Bias)/3.0f );
   Quaternion_Linear_Interp( G2, V_Cur->Values, V_Next->Values,  (1.0f-V_Cur->Bias)/3.0f );
   if ( Case==RIGHT_T )
   {
      Cont = 0.5f - 0.5f*V_Cur->Cont;
      Tens = 1.0f-V_Cur->Tens;
   }
   else {
      Cont = 0.5f + 0.5f*V_Cur->Cont;
      Tens = V_Cur->Tens - 1.0f;
   }
   Quaternion_Linear_Interp( G3, G1, G2, Cont );
   Quaternion_Linear_Interp( Result, V_Cur->Values, G3, Tens );
}

EXTERN void Setup_Rot_Spline_3ds_Derivates( SPLINE_3DS *Sp )
{
   INT i;

   if ( Sp==NULL ) return;

   Sp->Cache = 0;
   if ( Sp->Nb_Keys<2 ) return;
   for( i=0; i<Sp->Nb_Keys; ++i )
   {
      Get_Rot_Spline_Tangent( Sp, Sp->Keys[i].Left, i, LEFT_T );
      Get_Rot_Spline_Tangent( Sp, Sp->Keys[i].Right, i, RIGHT_T );
   }
}

/******************************************************************/

static void Bezier_Coeffs( double *h, FLT T )
{
   double S, T2, S2;
   S = 1.0-T; S2 = S*S; T2 = T*T;

   h[0] = S2*S;
   h[2] = 3.0*S2*T;
   h[3] = 3.0*S*T2;
   h[1] = T2*T;
}

EXTERN void Interpolate_Rot_Spline_3ds_Values( 
   FLT T, SPLINE_3DS *Sp, FLT *Result )
{
   INT i, n;
   double h[4];

   if ( Sp==NULL ) return;

   if ( Sp->Nb_Keys==0 ) {    // hope it's ok...
      Result[0] = 1.0; 
      Result[1]=Result[2]=Result[3]=  0.0;
      return;
   }
   if ( Sp->Nb_Keys==1 ) { Void_Spline( Sp, Result ); return; }

   n = Fetch_Spline_Key( &T, Sp );
   if ( n==-1 )
   {
      for( i=0; i<Sp->Data_Size; ++i )
         Result[i] = Sp->Keys[Sp->Nb_Keys-1].Values[i];
      return;
   }

   Bezier_Coeffs( h, T );
   for( i=0; i<Sp->Data_Size; ++i )
   {
      double T;
      T  = h[0]*Sp->Keys[n].Values[i];
      T += h[1]*Sp->Keys[n+1].Values[i];
      T += h[2]*Sp->Keys[n].Right[i];
      T += h[3]*Sp->Keys[n+1].Left[i];
      Result[i] = (FLT)T;
   }
}

/******************************************************************/

EXTERN void Destroy_Spline_3ds( SPLINE_3DS **Sp )
{
   if ( Sp==NULL || (*Sp)==NULL ) return;
   M_Free( (*Sp)->Keys );
   M_Free( (*Sp) );
}

EXTERN SPLINE_3DS *New_Spline_3ds( SPLINE_3DS **Sp, INT Size, INT Nb )
{
   Destroy_Spline_3ds( Sp );
   *Sp = New_Fatal_Object( 1, SPLINE_3DS );
   if ( *Sp==NULL ) return( NULL );
   (*Sp)->Keys = New_Fatal_Object( Nb, A_KEY );
   if ( (*Sp)->Keys==NULL ) { M_Free( (*Sp) ); return( NULL ); }
   (*Sp)->Data_Size = Size;
   (*Sp)->Nb_Keys = Nb;
   (*Sp)->Cache = 0;
   (*Sp)->Flags = 0;
   return( *Sp );
}

EXTERN A_KEY *Insert_Key_In_Spline_3ds( SPLINE_3DS *Sp, FLT T )
{
   A_KEY *New;
   INT i, j;
   New = New_Fatal_Object( Sp->Nb_Keys+1, A_KEY );
   if ( New==NULL ) return( NULL );
   for( i=0; i<Sp->Nb_Keys; ++i )
      if ( Sp->Keys[i].Frame<=T ) continue;
   if ( i ) memcpy( &New[0], &Sp->Keys[0], i*sizeof( Sp->Keys[0] ) );
   j = Sp->Nb_Keys-i;
   New[i].Frame = T;
   if ( j ) memcpy( &New[i+1], &Sp->Keys[i], j*sizeof( Sp->Keys[0] ) );
   M_Free( Sp->Keys );
   Sp->Keys = New;
   Sp->Nb_Keys++;
   return( &New[i] );
}

/******************************************************************/
