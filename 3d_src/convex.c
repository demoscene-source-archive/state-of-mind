/***********************************************
 *              convex.c                       *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

#if 0     // DISABLED. NOT FINISHED.

/******************************************************************/

#define C_EPSILON 0.01

static INT Nb_Vertex; 
// static INT Nb_Polys;
static INT Total_Vertex,
Nb_CPart; static INT Nb_Polys_Done, Total_Polys;

   /* Isn't called at run-time. So, let's do it static...*/

#define MAX_POLYS 5000
#define MAX_VERTICES 10000
static POLY *Done[MAX_POLYS];
static INT Stack_Size;
static INT Stack[ MAX_POLYS ];

static VECTOR New_Vertex[MAX_VERTICES];
static USHORT New_Index[MAX_VERTICES];
static F_MAPPING New_UV[MAX_VERTICES];

/******************************************************************/
/******************************************************************/

static INT Test_Polys( POLY *P1, POLY *P2 )
{
   INT i, j;
   FLT Norm, Norm_0;

      /* Test if P2 lies on the plane defined by P1 */

   Norm_0 = (FLT)sqrt( (double)Norm_Squared( P1->No ) );

      /* Test if P2 lies on the plane defined by P1 */
   for( j=0; j<P1->Nb_Pts; ++j )
      for( i=0; i<P2->Nb_Pts; ++i ) 
      {
         VECTOR N;
         if ( P2->Pt[i]==P1->Pt[j] ) continue;
         N[0] = New_Vertex[ P2->Pt[i] ][0] - New_Vertex[ P1->Pt[j] ][0];
         N[1] = New_Vertex[ P2->Pt[i] ][1] - New_Vertex[ P1->Pt[j] ][1];
         N[2] = New_Vertex[ P2->Pt[i] ][2] - New_Vertex[ P1->Pt[j] ][2];
         Norm = Norm_0 * (FLT)sqrt( (double)Norm_Squared( N ) );
         if ( Dot_Product( N, P1->No )>C_EPSILON*Norm )
            return( -1 );
         if ( Dot_Product( N, P2->No )<-C_EPSILON*Norm )
            return( -1 );
      }
   return( 1 );   /* Ok */
}

/******************************************************************/

static INT Search_Nearby_Poly( INT P )
{
   INT i;
   INT Shuffle[MAX_EDGES] = { 0,1,2,3 };
   
      /* Find non-used vertex-sharing poly */

   for( i=Total_Polys-1; i>=0; --i )
   {
      INT j, k, Common;

      if ( i==P ) continue;
      if ( Done[i]->Colors != 0xFFFF ) continue; /* Already stored */
      if ( Done[i]->Clipped != 0x00 ) continue; /* Already tested */

      Common = 0;
      for( k=0; k<Done[P]->Nb_Pts; ++k )
      {         
         for( j=0; j<Done[i]->Nb_Pts; ++j )
            if ( Done[i]->Pt[j] == Done[P]->Pt[ Shuffle[k] ] ) 
            {
               Common++;   /* Sharing 2 vertex ? */
               if ( Common==2 ) return( i );
            }
      }
      {
         INT Tmp, k;
         k = 1 + ( random()%( Done[P]->Nb_Pts-1 ) );
         Tmp = Shuffle[0]; Shuffle[0] = Shuffle[k]; Shuffle[k] = Tmp;
      }
   }
   return( -1 );
}

/******************************************************************/

EXTERN void Build_CPart( INT Cur_Poly )
{
   INT i;

   for( i=Total_Polys-1; i>=0; --i ) Done[i]->Clipped = 0x00;

   Stack_Size = 0;
   Stack[Stack_Size++] = Cur_Poly;

   do
   {
      INT Test_Poly, Sign, k;

      Cur_Poly = Stack[--Stack_Size];

      while(1)
      {
         Test_Poly = Search_Nearby_Poly( Cur_Poly );
         if ( Test_Poly == -1 ) break; /* No more. Time to go on ... */

         Sign = 0; k = 0;
         for( i=Total_Polys-1; i>=0; --i )
         {
            if ( Done[i]->Colors != Nb_CPart ) continue;
            k++;
            Sign = Test_Polys( Done[i], Done[Test_Poly] );
            if ( Sign == -1 ) break;         
         }
         if ( Sign==-1 ) Done[Test_Poly]->Clipped = 0x01; /* Test failed. Reject. */
         else
         {
            Stack[Stack_Size++] = Test_Poly;
            Done[Test_Poly]->Colors = Nb_CPart; /* Add to the bunch */
            Nb_Polys_Done++;
            
            fprintf( stderr,
               "Nb_Polys_Done: %d  (%d cvx parts) (%d polys in current)    \r",
               Nb_Polys_Done, Nb_CPart, k );
            
         }
      }
   } while( Stack_Size>0 );
}

/******************************************************************/
/******************************************************************/

static void Reset_Vertex( OBJECT *Dum, MATRIX2 Matrix )
{
   MESH *Msh;
   INT i;

   Msh = (MESH *)Dum;

   if ( Msh->Type != MESH_TYPE || Msh->Nb_Polys >MAX_POLYS )
      Exit_Upon_Error( "Error in Convexify()" );

   Average_Normals( Msh );

   for( i=0; i<Msh->Nb_Polys; ++i )
   {
      VECTOR N;
      Msh->Polys[i].Colors = 0xFFFF;
      Msh->Polys[i].Clipped = 0x00;

      Msh->Polys[i].Pt[0] += Total_Vertex;
      Msh->Polys[i].Pt[1] += Total_Vertex;
      Msh->Polys[i].Pt[2] += Total_Vertex;

      Done[ Total_Polys + i ] = Msh->Polys + i;
      nA_V( N, Matrix, (FLT *)( Msh->Polys[i].No ) );
      Set_Vector( Msh->Polys[i].No, N[0], N[1], N[2] );
   }
   Total_Polys += i;

   for( i=0; i<Msh->Nb_Vertex; ++i )
   {
      VECTOR Pt;
      A_V( Pt, Matrix, (FLT *)( Msh->Vertex+i ) );
      New_Vertex[Total_Vertex+i][0] = Pt[0];
      New_Vertex[Total_Vertex+i][1] = Pt[1];
      New_Vertex[Total_Vertex+i][2] = Pt[2];
      New_UV[Total_Vertex+i][0] = Msh->UV[i][0];
      New_UV[Total_Vertex+i][1] = Msh->UV[i][1];
   }
   Total_Vertex += i;
}


EXTERN OBJECT *Convexify( OBJECT *Obj )
{
   OBJECT *New;
   INT i;

   Out_Message( "Convexifying objects with Depth-first algo..." );


   for( i=0; i<MAX_POLYS; ++i ) Done[i] = NULL; /* Debug only */

   Total_Polys = 0; Total_Vertex = 0;
   Traverse_Tree_With_Transform( Obj, NULL, Reset_Vertex );

   Out_Message( "Total polys to deal with: %d", Total_Polys );
   Out_Message( "Total vertex to deal with: %d", Total_Vertex );


   Nb_CPart = 0;
   while( Nb_Polys_Done<Total_Polys )
   {
      INT First_Poly;

           /* find first un-tested Poly to start with */
      First_Poly = 0;
      while( Done[ First_Poly ]->Colors != 0xFFFF )
         First_Poly++; 

      Done[First_Poly]->Colors = Nb_CPart;   /* Mark it */
      Nb_Polys_Done++;      


      Build_CPart( First_Poly ); /* Start new bunch with First_Poly */

      Nb_CPart++;
   }

   Out_Message( "\n   ...done. Found %d convex parts", Nb_CPart );

   Out_Message( " Rebuilding mesh..." );
   if ( Nb_CPart==1 )   /* Nothing to do. Keep it. */
   {
      New = Obj;
      goto Finish;
   }

   New = (OBJECT *)New_Group_Object( );
   if ( New==NULL ) return( NULL );


   for( i=0; i<Nb_CPart; ++i )
   {
      MESH *Msh;
      INT Nb_Polys, j, k, n;

      for( j=0; j<Total_Vertex; ++j ) New_Index[j] = 0xFFFF;

      Nb_Polys = 0;

      for( j=0; j<Total_Polys; ++j )
         if ( Done[j]->Colors == i )
         {
            for( k=0; k<Done[j]->Nb_Pts; ++k )
               New_Index[ Done[j]->Pt[k] ] = 1;
            Nb_Polys++;
         }

      Nb_Vertex = 0;
      for( j=0; j<Total_Vertex; ++j )
         if ( New_Index[j]!=0xFFFF )
            Nb_Vertex++;

      Out_Message( "Part #%d: %d polys, %d vertex.", i, Nb_Polys, Nb_Vertex );

      Msh = Set_Mesh_Data( NULL, Nb_Vertex, Nb_Polys );
      if ( Msh==NULL ) return( NULL );         

      k=0;  /* Final index */
      for( j=0; j<Total_Vertex; ++j )
      {
         if ( New_Index[j] == 0xFFFF ) continue;
         New_Index[j] = k;
         Msh->Vertex[k][0] = New_Vertex[ j ][0];
         Msh->Vertex[k][1] = New_Vertex[ j ][1];
         Msh->Vertex[k][2] = New_Vertex[ j ][2];
         Msh->UV[k][0] = New_UV[j][0];
         Msh->UV[k][1] = New_UV[j][1];
         k++;
      }
      n = 0;
      for( j=0; j<Total_Polys; ++j )
      {
         if ( Done[j]->Colors != i ) continue;
         for( k=0; k<Done[j]->Nb_Pts; ++k )
            Msh->Polys[n].Pt[k] = New_Index[ Done[j]->Pt[k] ];
         Msh->Polys[n].Nb_Pts = k;
         Msh->Polys[n].Clipped = 0x00;
         Msh->Polys[n].Colors = 0x0000;
         n++;
      }
      Average_Normals( Msh );      
//      TODO: Fix/finish that...
//      Insert_Object( (OBJECT *)Msh, (OBJECT *)New );
   }

Finish:
   Out_Message( "    ....ok" );
   return( New );
}

/******************************************************************/
/******************************************************************/

#endif  // 0
