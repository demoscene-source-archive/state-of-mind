/***********************************************
 *      Obj making                             *
 *                                             *
 * Skal 96                                     *
 ***********************************************/

#include "main3d.h"

/*****************************************************************/

EXTERN MESH *Make_Sphere2( INT Nx, INT Ny, FLT Rx, FLT Ry, FLT Rz, 
   INT Wx, INT Wy )
{
   INT     j, N;
   MESH    *Obj;
   VECTOR  *Vertice;
   F_MAPPING *UV;
   POLY    *Poly;

   Obj = Set_Mesh_Data( NULL, Nx*Ny, 2*(Nx-1)*(Ny-1) );
   if ( Obj==NULL ) return( NULL );
   Vertice = Obj->Vertex;
   UV = Obj->UV;
   N = 0;
   for( j=0; j<Ny; ++j )
   {
      FLT y;
      INT i;
      y = ( FLT )( j )*M_PI/(Ny-1);
      for( i=0; i<Nx; ++i )
      {
         FLT   x, r;
         FLT   X, Y, Z;
         
         x = ( FLT )( i )*2.0*M_PI/(Nx-1);

         X = Rx*cos( x )*sin( y );
         Y = Ry*sin( x )*sin( y );
         Z = Rz*cos( y );

         Vertice[N][ _X ]= X;
         Vertice[N][ _Y ]= Y;
         Vertice[N][ _Z ]= Z;

//         x = fmod( 1.0*Wx*i/Nx, 2.0 );
//         if ( x>=1.0 ) x = 2.0-x;
         r = .5 + .5 * cos( x*Wx );
         UV[N][0] = r;
         // x = fmod( 1.0*Wy*j/Ny, 2.0 );
         // if ( x>=1.0 ) x = 2.0-x;
         r = .5 + .5 * cos( y*Wy );
         UV[N][1] = r;

         N++;
      }
   }

   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      INT i;
      for( i=0; i<Nx-1; ++i )
      {
            // normals inverted!!
         Poly->Pt[0] = (i+1) + (j+1)*Nx;
         Poly->Pt[2] = (i+1) + j*Nx;
         Poly->Pt[1] = i + j*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;

         Poly->Pt[0] = (i+1) + (j+1)*Nx;
         Poly->Pt[2] = i + j*Nx;
         Poly->Pt[1] = i + (j+1)*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;

      }
   }
   Average_Normals( Obj );
   Obj->Flags = (OBJ_FLAG)(Obj->Flags | OBJ_HAS_UV );
   return( Obj );
}

/*****************************************************************/

EXTERN MESH *Make_Torus2( INT Nx, INT Ny, FLT R1, FLT R2 )
{
   INT j, N;
   MESH    *Obj;
   VECTOR  *Vertice;
   POLY    *Poly;
   F_MAPPING *UV;


   Obj = Set_Mesh_Data( NULL, Nx*Ny, 2*Nx*Ny );
   if ( Obj==NULL ) return( NULL );
   Vertice = Obj->Vertex;
   UV = Obj->UV;
   N = 0;
   for( j=0; j<Ny; ++j )
   {
      INT i;
      FLT y;
      y = ( FLT )( j )*2.0*M_PI/(Ny);
      for( i=0; i<Nx; ++i )
      {
         FLT   x, X, Y, Z, R;
         x = ( FLT )( i )*2.0*M_PI/(Nx);

         R = R1 - R2*cos( y );
         X = R*cos( x );
         Y = R*sin( x );
         Z = R2*sin( y );

         Vertice[N][ _X ]= X;
         Vertice[N][ _Y ]= Y;
         Vertice[N][ _Z ]= Z;

         UV[N][0] = 1.0*i/Nx;
         UV[N][1] = 1.0*j/Ny;

         N++;
      }
   }

   Poly = Obj->Polys;
   for( j=0; j<Ny; ++j )
   {
      INT i;
      for( i=0; i<Nx; ++i )
      {
         Poly->Pt[2] = ((i+1)%Nx) + ((j+1)%Ny)*Nx;
         Poly->Pt[1] = ((i+1)%Nx) + (j%Ny)*Nx;
         Poly->Pt[0] = (i%Nx) + (j%Ny)*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;

         Poly->Pt[2] = ((i+1)%Nx) + ((j+1)%Ny)*Nx;
         Poly->Pt[1] = (i%Nx) + (j%Ny)*Nx;
         Poly->Pt[0] = (i%Nx) + ((j+1)%Ny)*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;
      }
   }
   Average_Normals( Obj );
   Obj->Flags = (OBJ_FLAG)(Obj->Flags | OBJ_HAS_UV );
   return( Obj );
}

/*****************************************************************/
