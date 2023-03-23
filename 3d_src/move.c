/***********************************************
 *              Move                           *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"

#if 0

/******************************************************************/

EXTERN void Insert_Point( VECTOR P, QUATERNION Q, FLT t, MOVE_SPLINE *Spl )
{
}

EXTERN void Add_Point( VECTOR P, QUATERNION Q, FLT t, MOVE_SPLINE *Spl )
{
   MOVE_KEY *Key;
   Key = (MOVE_KEY *)Add_Array_Element( &Spl->Keys );
   Key->P[0] = P[0]; Key->P[1] = P[1]; Key->P[2] = P[2];
   Key->Q[0] = Q[0]; Key->Q[1] = Q[1];
   Key->Q[2] = Q[2]; Key->Q[3] = Q[3];
   Key->t = t;
}

EXTERN MOVE_SPLINE *New_Move_Spline( INT Nb )
{
   MOVE_SPLINE *New;

   New = New_Fatal_Object( 1, MOVE_SPLINE );
   if ( New==NULL ) return( NULL );
   Mem_Clear( New );
   if ( Nb<=0 ) Nb = 4;
   if ( Init_Array( 4, sizeof( MOVE_KEY ), &New->Keys )==NULL ) goto Failed;
   New->Cache = 0;
   New->Last_Pt = 0;
   return( New );

Failed:
   Clear_Array( &New->Keys );
   M_Free( New );
   return( NULL );
}

EXTERN void Set_Up_Move( MOVE_SPLINE *Spl )
{
   INT i, Last_Pt;
   MOVE_KEY *Keys;

   Clean_Up_Array( &Spl->Keys );
   Keys = (MOVE_KEY *)Spl->Keys.Data;
   Last_Pt = (Spl->Keys.Nb_Elm-2) & ~0x01;
   Spl->Last_Pt = Last_Pt;
   Spl->Cache = 0;

   Keys[1].Q[0] = Keys[0].Q[0];
   Keys[1].Q[1] = Keys[0].Q[1];
   Keys[1].Q[2] = Keys[0].Q[2];
   Keys[1].Q[3] = Keys[0].Q[3];

   Keys[Last_Pt+1].Q[0] = Keys[Last_Pt].Q[0];
   Keys[Last_Pt+1].Q[1] = Keys[Last_Pt].Q[1];
   Keys[Last_Pt+1].Q[2] = Keys[Last_Pt].Q[2];
   Keys[Last_Pt+1].Q[3] = Keys[Last_Pt].Q[3];

   for( i=2; i<Last_Pt; i+=2 )
      Setup_SQuad( Keys[i+1].Q, Keys[i-2].Q, Keys[i].Q, Keys[i+2].Q );
}

EXTERN void Move_Matrix( FLT t, MOVE_SPLINE *Spl, MATRIX2 M )
{
   MOVE_KEY *Keys;
   INT i;
   VECTOR P, Q;
   FLT u, ub, u2, ub2, u3, ub3;

   Keys = (MOVE_KEY *)Spl->Keys.Data;

   if ( t<Keys[0].t ) { 
      Spl->Cache = 0;
      M[18] = Keys[0].P[0]; 
      M[19] = Keys[0].P[1]; 
      M[20] = Keys[0].P[2];
      Quaternion_To_Matrix( Keys[0].Q, M );
      return;
   }
   if ( t>Keys[Spl->Last_Pt].t )
   {
      Spl->Cache = Spl->Last_Pt-2;
      M[18] = Keys[Spl->Last_Pt].P[0]; 
      M[19] = Keys[Spl->Last_Pt].P[1]; 
      M[20] = Keys[Spl->Last_Pt].P[2];
      Quaternion_To_Matrix( Keys[Spl->Last_Pt].Q, M );
      return;
   }

   i = Spl->Cache;
   if ( t<Keys[i].t ) i=0;

   while ( t>=Keys[i+2].t ) i+=2;

   Spl->Cache = i;
   u = (t-Keys[i].t)/( Keys[i+2].t - Keys[i].t );
   u2 = u*u;
   u3 = u2*u;
   ub = 1.0-u;
   ub2 = ub*ub;
   ub3 = ub2*ub;

   {
      QUATERNION Q1, Q2;
      Quaternion_Linear_Interp( Q1, Keys[i].Q, Keys[i+2].Q, u );
      Quaternion_Linear_Interp( Q2, Keys[i+1].Q, Keys[i+3].Q, u );      
      Quaternion_Linear_Interp( Q, Q1, Q2, 2.0*u*(1.0-u) );
   }
   Quaternion_To_Matrix( Q, M );

   M[18] = ub3*Keys[i].P[0] + 3.0*ub2*u*Keys[i+1].P[0] +
      (6.0*ub+u)*u2*Keys[i+2].P[0] - 3.0*ub*u2*Keys[i+3].P[0];
   M[19] = ub3*Keys[i].P[1] + 3.0*ub2*u*Keys[i+1].P[1] +
      (6.0*ub+u)*u2*Keys[i+2].P[1] - 3.0*ub*u2*Keys[i+3].P[1];
   M[20] = ub3*Keys[i].P[2] + 3.0*ub2*u*Keys[i+1].P[2] +
      (6.0*ub+u)*u2*Keys[i+2].P[2] - 3.0*ub*u2*Keys[i+3].P[2];

/*
   Out_Message( "Key #%d/%d (#Elm: %d)",
      i, Spl->Last_Pt, Spl->Keys.Nb_Elm );
*/

   return;
}

/******************************************************************/

EXTERN void Move_Matrix2( FLT t, MOVE_SPLINE *Spl, MATRIX2 M )
{
   MOVE_KEY *Keys;
   INT i;
   VECTOR P, Q;
   FLT b0, b1, b2, b3;
   FLT u, s, tens;

   Keys = (MOVE_KEY *)Spl->Keys.Data;

   if ( t<Keys[0].t )
   { 
      Spl->Cache = 0;
      M[18] = Keys[0].P[0]; 
      M[19] = Keys[0].P[1]; 
      M[20] = Keys[0].P[2];
      Quaternion_To_Matrix( Keys[0].Q, M );
      return;
   }
   if ( t>Keys[Spl->Last_Pt].t )
   {
      Spl->Cache = Spl->Last_Pt-2;
      M[18] = Keys[Spl->Last_Pt].P[0]; 
      M[19] = Keys[Spl->Last_Pt].P[1]; 
      M[20] = Keys[Spl->Last_Pt].P[2];
      Quaternion_To_Matrix( Keys[Spl->Last_Pt].Q, M );
      return;
   }

   i = Spl->Cache;
   if ( t<Keys[i].t ) i=0;

   while ( t>=Keys[i+2].t ) i+=2;

   Spl->Cache = i;
   u = (t-Keys[i].t)/( Keys[i+2].t - Keys[i].t );
   {
      FLT u, u2, u3, ub, d, Tmp;
      FLT s2, s3;

      d = 2*s + 4;
      d = d*s + 4;
      d = d*s + tens + 2;
      d = 1.0/d;
      u2 = u*u;
      u3 = u2*u;
      ub = 1.0-u;
      Tmp = ub*s;
      s2 = s*s;
      s3 = s2*s;

      b0 = 2.0*d*(Tmp*Tmp*Tmp);     //    2/d.[s.(1-u)]^3

      b1 = 2.0*s3*u*(u2+3.0*ub);    //    2.s^3.u.[u^2 + 3(1-u)]
      b1 += 2.0*s2*(u3-3*u2+2.0);   //  += 2.s^2.(u^3-3u^2+2)
      b1 += 2.0*s*(u3 -3.0*u+2.0);  //  += 2.s.(u^3-3.u+2)
      b1 += tens*(2.0*u3-3*u2+1.0);    //  += t.(2u^3-3u^2+1)
      b1 *= d;

      b2 = 2.0*s2*u2*(3.0-u);
      b2 += 2.0*s*u*(3.0-u2);
      b2 += tens*u2*(3.0-2.0*u) + 2.0*(1.0-u3);
      b2 *= d;

      b3 = 2.0*u3*d;                //    2.u^3/d
   }


   {
      QUATERNION Q1, Q2;
      Quaternion_Linear_Interp( Q1, Keys[i].Q, Keys[i+2].Q, u );
      Quaternion_Linear_Interp( Q2, Keys[i+1].Q, Keys[i+3].Q, u );      
      Quaternion_Linear_Interp( Q, Q1, Q2, 2.0*u*(1.0-u) );
   }
   Quaternion_To_Matrix( Q, M );

   M[18] = b0*Keys[i].P[0] + b1*Keys[i+1].P[0] +
      b2*Keys[i+2].P[0] + b3*Keys[i+3].P[0];
   M[19] = b0*Keys[i].P[1] + b1*Keys[i+1].P[1] +
      b2*Keys[i+2].P[1] + b3*Keys[i+3].P[1];
   M[20] = b0*Keys[i].P[2] + b1*Keys[i+1].P[2] +
      b2*Keys[i+2].P[2] + b3*Keys[i+3].P[2];

/*
   Out_Message( "Key #%d/%d (#Elm: %d)",
      i, Spl->Last_Pt, Spl->Keys.Nb_Elm );
*/

   return;
}

/******************************************************************/

#endif
