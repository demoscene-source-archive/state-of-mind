/***********************************************
 *      Obj making                             *
 *                                             *
 * Skal 96                                     *
 ***********************************************/

#include "main3d.h"

#if 0

/*****************************************************************/

EXTERN OBJECT *Fill_Cyl5_Polys( OBJECT *Obj, int Nx, int Ny,
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius )
{
   int i, j, k;
   VERTICE *Vertice, *Base_V, *Base_V2;
   POLY    *Poly;
   FLT   R, Ty, Tz, Cy, Cz, Norm, Eps;

      /* Same as Fill_Cyl2, but with lighter caps,
         and without textures */

   Init_Cst_Spline( Axis_y, Axis_y->Nb_Pts, Axis_y->Pts );
   Init_Cst_Spline( Axis_z, Axis_z->Nb_Pts, Axis_z->Pts );
   Init_Cst_Spline( Radius, Radius->Nb_Pts, Radius->Pts );

   Eps = 1.0/(Ny-1);

   Cst_Spline_Pos_And_Drv( Axis_z, 0.0, &Cz, &Ty );
   Cst_Spline_Pos_And_Drv( Axis_y, 0.0, &Cy, &Tz );
   R = Cst_Spline_Pos( Radius, 0.0 );

   Norm = sqrt( Ty*Ty + Tz*Tz );
   Ty /= Norm; Tz /= Norm;

   Vertice = Obj->Vertices;
   Base_V = Vertice+Nx*Ny;
   Base_V->Base_Pt[_X] = 0;      /* Base Point */
   Base_V->Base_Pt[_Y] = (int)Cy;
   Base_V->Base_Pt[_Z] = (int)Cz;

   for( j=0; j<Ny; ++j )
   {
      for( i=0; i<Nx; ++i )
      {
         FLT x;
         
         x = ( FLT )( i )*2.0*M_PI/(Nx);

         Vertice->Base_Pt[ _X ]= ( int )( R*cos( x ) );
         Vertice->Base_Pt[ _Y ]= ( int )( Cy+Ty*R*sin( x ) );
         Vertice->Base_Pt[ _Z ]= ( int )( Cz-Tz*R*sin( x ) );

         Vertice++;
      }
      Advance_Cst_Spline_Pos_And_Drv( Axis_y, Eps, &Cy, &Tz );
      Advance_Cst_Spline_Pos_And_Drv( Axis_z, Eps, &Cz, &Ty );
      R = Advance_Cst_Spline( Radius, Eps );
      Norm = sqrt( Ty*Ty + Tz*Tz );
      Ty /= Norm; Tz /= Norm;
   }
   Base_V++;
   Base_V->Base_Pt[_X] = 0;      /* Top point */
   Base_V->Base_Pt[_Y] = (int)Cy;
   Base_V->Base_Pt[_Z] = (int)Cz;
   memcpy( Vertice+2, Obj->Vertices, Nx*sizeof(VERTICE) );
   memcpy( Vertice+2+Nx, Obj->Vertices+Nx*(Ny-1), Nx*sizeof(VERTICE) );
   Average_Normals( Obj );
   return( Obj );
}

EXTERN OBJECT *Make_Cyl5( int Nx, int Ny, 
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius )
{
   int i, j, k;
   OBJECT *Obj;
   VERTICE *Base_V, *Base_V2, *Apex_V;
   POLY *Poly;

   Obj = New_Object( 2*(Nx)*(Ny-1) + 2*(Nx), Nx*Ny+2*Nx+2 );

   Base_V = Obj->Vertices;
   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      for( i=0; i<Nx; ++i )
      {
         Poly->P3 = Base_V + ((i+1)%Nx) + j*Nx;
         Poly->P2 = Base_V + i + j*Nx;
         Poly->P1 = Base_V + ((i+1)%Nx) + (j+1)*Nx;
         Poly->Texture_Nb = 0;
         Poly++;

         Poly->P3 = Base_V + i + (j+1)*Nx;
         Poly->P2 = Base_V + ((i+1)%Nx) + (j+1)*Nx;
         Poly->P1 = Base_V + i + j*Nx;
         Poly->Texture_Nb = 0;
         Poly++;
      }
   }
   Apex_V = Base_V+Nx*Ny;
   Base_V = Apex_V+2;
   Base_V2 = Base_V+Nx;
   for( i=0; i<Nx; ++i )
   {
      Poly->P3 = Base_V + i;
      Poly->P2 = Base_V + ((i+1)%Nx);
      Poly->P1 = Apex_V;          /* Base point */
      Poly++;
      Poly->P1 = Base_V2 + i;
      Poly->P2 = Base_V2 + ((i+1)%Nx);
      Poly->P3 = Apex_V+1;        /* Top point */
      Poly++;
   }      
   Obj->Nb_Textures = 1;
   Fill_Cyl5_Polys( Obj, Nx, Ny, Axis_y, Axis_z, Radius );

   return( Obj );
}

/*****************************************************************/

EXTERN OBJECT *Fill_Cyl4_Polys( OBJECT *Obj, int Nx, int Ny, 
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, 
   CST_SPLINE *Radius,
   CST_SPLINE *Shape, CST_SPLINE *Angle )
{
   int i, j, k;
   VERTICE *Vertice, *Base_V, *Base_V2;
   POLY    *Poly;
   FLT   R, Alpha;
   FLT   Ty, Tz, Cy, Cz, Norm;
   FLT   Eps, Eps2, Eps3;

   Init_Cst_Spline( Axis_y, Axis_y->Nb_Pts, Axis_y->Pts );
   Init_Cst_Spline( Axis_z, Axis_z->Nb_Pts, Axis_z->Pts );
   Init_Cst_Spline( Radius, Radius->Nb_Pts, Radius->Pts );
   Init_Cst_Spline( Angle, Angle->Nb_Pts, Angle->Pts );

   Eps = 1.0/(Ny-1.0);
   Eps2 = 1.0/(Nx-1.0);
   Eps3 = Eps2*2.0*M_PI;

   Cst_Spline_Pos_And_Drv( Axis_z, 0.0, &Cz, &Ty );
   Cst_Spline_Pos_And_Drv( Axis_y, 0.0, &Cy, &Tz );
   R = Cst_Spline_Pos( Radius, 0.0 );
   Alpha = Cst_Spline_Pos( Angle, 0.0 )*2.0*M_PI/256.0;

   Norm = sqrt( Ty*Ty + Tz*Tz );
   Ty /= Norm; Tz /= Norm;

   Vertice = Obj->Vertices;
   for( j=0; j<Ny; ++j )
   {
      FLT x, r;

      x = Alpha;
      Init_Cst_Spline( Shape, Shape->Nb_Pts, Shape->Pts );
      r = R * Cst_Spline_Pos( Shape, 0.0 );

      for( i=0; i<Nx; ++i )
      {
         FLT X, Y, Z;

         X = r*cos( x );
         Vertice->Base_Pt[ _Z ]= ( int )floor( X );
         r = r*sin(x );
         Y = Cy+Ty*r;
         Vertice->Base_Pt[ _X ]= ( int )floor( Y );
         Z = Cz-Tz*r;
         Vertice->Base_Pt[ _Y ]= ( int )floor( Z );

         Vertice++;
         x += Eps3;
         r = R*Advance_Cst_Spline( Shape, Eps2 );
      }
      Advance_Cst_Spline_Pos_And_Drv( Axis_y, Eps, &Cy, &Tz );
      Advance_Cst_Spline_Pos_And_Drv( Axis_z, Eps, &Cz, &Ty );
      R = Advance_Cst_Spline( Radius, Eps );
      Alpha = Advance_Cst_Spline( Angle, Eps )*2.0*M_PI/256.0;
      Norm = sqrt( Ty*Ty + Tz*Tz );
      Ty /= Norm; Tz /= Norm;
   }
   Average_Normals( Obj );
   return( Obj );
}

EXTERN OBJECT *Make_Cyl4( int Nx, int Ny,
   SHORT Uo, SHORT U, SHORT Vo, SHORT V,
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius,
   CST_SPLINE *Shape, CST_SPLINE *Angle )
{
   int i, j, k;
   OBJECT *Obj;
   VERTICE *Vertice, *Base_V, *Base_V2;
   POLY    *Poly;
   FLT   x, y;

   k = 2*Nx*(Ny-1);
   j = Nx*Ny;

   Obj = New_Object( k, j );

   Base_V = Obj->Vertices;
   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      for( i=0; i<Nx; ++i )
      {
         Poly->P3 = Base_V + ((i+1)%Nx) + (j)*Nx;
         Poly->P2 = Base_V + i + j*Nx;
         Poly->P1 = Base_V + ((i+1)%Nx) + (j+1)*Nx;
         Poly->Texture_Nb = 0;
         Poly++;

         Poly->P3 = Base_V + i + (j+1)*Nx;
         Poly->P2 = Base_V + ((i+1)%Nx) + (j+1)*Nx;
         Poly->P1 = Base_V + i + (j)*Nx;
         Poly->Texture_Nb = 0;
         Poly++;
      }
   }

   Fill_Cyl4_Polys( Obj, Nx, Ny, 
      Axis_y, Axis_z, Radius, Shape, Angle );

   Vertice = Obj->Vertices;
   for( j=0; j<Ny; ++j )
   {
      y = ( FLT )( j )/(Ny-1);
      for( i=0; i<Nx; ++i )
      {
         x = ( FLT )( i )/(Nx-1);

         Vertice->u = (USHORT)( ( x*(U-Uo)+Uo )*255.9 );
         Vertice->v = (USHORT)( ( y*(V-Vo)+Vo )*255.9 );
         Vertice++;
      }
   }

   Obj->Nb_Textures = 1;

   return( Obj );
}

/*****************************************************************/

EXTERN OBJECT *Fill_Cyl3_Polys( OBJECT *Obj, int Nx, int Ny, 
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius )
{
   int i, j, k;
   VERTICE *Vertice, *Base_V, *Base_V2;
   POLY    *Poly;
   FLT   r, R;
   FLT   Ty, Tz, Cy, Cz, Norm, Eps;

   Init_Cst_Spline( Axis_y, Axis_y->Nb_Pts, Axis_y->Pts );
   Init_Cst_Spline( Axis_z, Axis_z->Nb_Pts, Axis_z->Pts );
   Init_Cst_Spline( Radius, Radius->Nb_Pts, Radius->Pts );

   Eps = 1.0/(Ny-1.0);

   Cst_Spline_Pos_And_Drv( Axis_z, 0.0, &Cz, &Ty );
   Cst_Spline_Pos_And_Drv( Axis_y, 0.0, &Cy, &Tz );
   R = Cst_Spline_Pos( Radius, 0.0 );

   Norm = sqrt( Ty*Ty + Tz*Tz );
   Ty /= Norm; Tz /= Norm;

   Vertice = Obj->Vertices;
   for( j=0; j<Ny; ++j )
   {
      for( i=0; i<Nx; ++i )
      {
         FLT x;
         FLT X, Y, Z;

         x = ( FLT )( i )*2.0*M_PI/Nx;

         X = R*cos( x );
         Vertice->Base_Pt[ _X ]= ( int )floor( X );
         Y = Cy+Ty*R*sin( x );
         Vertice->Base_Pt[ _Y ]= ( int )floor( Y );
         Z = Cz-Tz*R*sin( x );
         Vertice->Base_Pt[ _Z ]= ( int )floor( Z );

         Vertice++;
      }
      Advance_Cst_Spline_Pos_And_Drv( Axis_y, Eps, &Cy, &Tz );
      Advance_Cst_Spline_Pos_And_Drv( Axis_z, Eps, &Cz, &Ty );
      R = Advance_Cst_Spline( Radius, Eps );
      Norm = sqrt( Ty*Ty + Tz*Tz );
      Ty /= Norm; Tz /= Norm;
   }

   Average_Normals( Obj );
   return( Obj );
}

EXTERN OBJECT *Make_Cyl3( int Nx, int Ny, 
   SHORT Uo, SHORT U, SHORT Vo, SHORT V,
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius )
{
   int i, j;
   OBJECT *Obj;
   VERTICE *Vertice, *Base_V;
   POLY    *Poly;

   Obj = New_Object( 2*Nx*(Ny-1), Nx*Ny );

   Base_V = Obj->Vertices;
   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      for( i=0; i<Nx; ++i )
      {
         Poly->P1 = Base_V + ((i+1)%Nx) + j*Nx;
         Poly->P3 = Base_V + i + j*Nx;
         Poly->P2 = Base_V + ((i+1)%Nx) + (j+1)*Nx;
         Poly->Texture_Nb = 0;
         Poly++;

         Poly->P1 = Base_V + i + (j+1)*Nx;
         Poly->P3 = Base_V + ((i+1)%Nx) + (j+1)*Nx;
         Poly->P2 = Base_V + i + j*Nx;
         Poly->Texture_Nb = 0;
         Poly++;

      }
   }
   Fill_Cyl3_Polys( Obj, Nx, Ny, Axis_y, Axis_z, Radius );

   Vertice = Obj->Vertices;
   for( j=0; j<Ny; ++j )
   {
      FLT y;
      y = ( FLT )( j )/(Ny-1);
      for( i=0; i<Nx; ++i )
      {
         FLT x;
         x = ( FLT )( i )/(Nx-1);

         Vertice->u = (USHORT)( ( x*(U-Uo)+Uo )*255.9 );
         Vertice->v = (USHORT)( ( y*(V-Vo)+Vo )*255.9 );
         Vertice++;
      }
   }
   Obj->Nb_Textures = 1;

   return( Obj );
}

/*****************************************************************/

EXTERN OBJECT *Fill_Cyl2_Polys( OBJECT *Obj, int Nx, int Ny, int Nx2, int Ny2,
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius )
{
   int i, j, k;
   VERTICE *Vertice, *Base_V, *Base_V2;
   POLY    *Poly;
   FLT   x, y, r, R;
   FLT   X, Y, Z;
   FLT   Ty, Tz, Cy, Cz, Norm, Eps;

   Init_Cst_Spline( Axis_y, Axis_y->Nb_Pts, Axis_y->Pts );
   Init_Cst_Spline( Axis_z, Axis_z->Nb_Pts, Axis_z->Pts );
   Init_Cst_Spline( Radius, Radius->Nb_Pts, Radius->Pts );

   Eps = 1.0/(Ny-1);

   Cst_Spline_Pos_And_Drv( Axis_z, 0.0, &Cz, &Ty );
   Cst_Spline_Pos_And_Drv( Axis_y, 0.0, &Cy, &Tz );
   R = Cst_Spline_Pos( Radius, 0.0 );

   Norm = sqrt( Ty*Ty + Tz*Tz );
   Ty /= Norm; Tz /= Norm;

   Vertice = Obj->Vertices + Nx*Ny;
   for( j=0; j<Ny2; ++j )
   {   
      y = ( FLT )( j )*2.0/(Ny2-1)-1.0;
      for( i=0; i<Nx2; ++i )
      {
         FLT r;

         x = ( FLT )( i )*2.0/(Nx2-1)-1.0;

         X = fabs( x ); Y = fabs( y );
         r = sqrt(x*x+y*y);
         if ( r==0.0 ) r = 1.0;
         if ( Y>X ) r = Y/r;
         else r = X/r;
         X = x*r;
         Y = y*r;

         Vertice->Base_Pt[ _X ]= ( int )floor( X*R );
         Vertice->Base_Pt[ _Y ]= ( int )floor( Cy - Y*Ty*R );
         Vertice->Base_Pt[ _Z ]= ( int )floor( Cz + Y*Tz*R );

         X = (X*r+1.0)*65535.9/2.0;
         Y = (Y*r+1.0)*65535.9/2.0;

         Vertice->u = (USHORT)( X );
         Vertice->v = (USHORT)( Y );

         Vertice++;
      }
   }

   Vertice = Obj->Vertices;
   for( j=0; j<Ny; ++j )
   {

      for( i=0; i<Nx; ++i )
      {
         x = ( FLT )( i )*2.0*M_PI/Nx;

         X = R*cos( x );
         Y = Cy+Ty*R*sin( x );
         Z = Cz-Tz*R*sin( x );

         Vertice->Base_Pt[ _X ]= ( int )floor( X );
         Vertice->Base_Pt[ _Y ]= ( int )floor( Y );
         Vertice->Base_Pt[ _Z ]= ( int )floor( Z );

         Vertice->u = (USHORT)( i*65535.9/(Nx-1) );
         Vertice->v = (USHORT)( j*65535.9/(Ny-1) );

         Vertice++;
      }
      Advance_Cst_Spline_Pos_And_Drv( Axis_y, Eps, &Cy, &Tz );
      Advance_Cst_Spline_Pos_And_Drv( Axis_z, Eps, &Cz, &Ty );
      R = Advance_Cst_Spline( Radius, Eps );
      Norm = sqrt( Ty*Ty + Tz*Tz );
      Ty /= Norm; Tz /= Norm;
   }
   Vertice = Obj->Vertices + Nx*Ny + Nx2*Ny2;
   for( j=0; j<Ny2; ++j )
   {
      y = ( FLT )( j )*2.0/(Ny2-1)-1.0;
      for( i=0; i<Nx2; ++i )
      {
         FLT r;

         x = ( FLT )( i )*2.0/(Nx2-1)-1.0;

         X = fabs( x ); Y = fabs( y );
         r = sqrt(x*x+y*y);
         if ( r==0.0 ) r = 1.0;
         if ( Y>X ) r = Y/r;
         else r = X/r;
         X = x*r;
         Y = y*r;

         Vertice->Base_Pt[ _X ]= ( int )floor( X*R );
         Vertice->Base_Pt[ _Y ]= ( int )floor( Cy - Y*Ty*R );
         Vertice->Base_Pt[ _Z ]= ( int )floor( Cz + Y*Tz*R );

         X = (X*r+1.0)*65535.9/2.0;
         Y = (Y*r+1.0)*65535.9/2.0;

         Vertice->u = (USHORT)( X );
         Vertice->v = (USHORT)( Y );

         Vertice++;
      }
   }

   Average_Normals( Obj );
   Obj->Flags |= OBJ_HAS_UV;
   return( Obj );
}

EXTERN OBJECT *Make_Cyl2( int Nx, int Ny, int Nx2, int Ny2,
   CST_SPLINE *Axis_y, CST_SPLINE *Axis_z, CST_SPLINE *Radius )
{
   int i, j, k;
   OBJECT *Obj;

   Obj = Make_Cyl( Nx, Ny, Nx2, Ny2, 500.0, 500.0 );
   Fill_Cyl2_Polys( Obj, Nx, Ny, Nx2, Ny2, Axis_y, Axis_z, Radius );

   Obj->Nb_Textures = 3;

   return( Obj );
}

/*****************************************************************/

#endif   // 0

EXTERN MESH *Make_Cyl( INT Nx, INT Ny, INT Nx2, INT Ny2, FLT R, FLT H )
{
   INT i, j, k, N;
   MESH  *Obj;
   VECTOR *Vertice;
   INT Base_V, Base_V2;
   F_MAPPING *UV;
   POLY    *Poly;
   FLT   x, y, r;
   FLT   X, Y, Z;

   k = 2*Nx*(Ny-1) + 2*( 2*(Nx2-1)*(Ny2-1) );   /* nb Vertex */
   j = Nx*Ny + 2*Nx2*Ny2;                       /* nb Polys */

   Obj = Set_Mesh_Data( NULL, j, k );
   if ( Obj==NULL ) return( NULL );

   Vertice = Obj->Vertex;
   UV = Obj->UV;
   N = 0;
   for( j=0; j<Ny; ++j )
   {
      y = ( FLT )( j )*2.0f/(Ny-1)-1.0f;
      for( i=0; i<Nx; ++i )
      {
         x = ( FLT )( i )*2.0*M_PI/Nx;

         X = R*cos( x );
         Y = R*sin( x );
         Z = H*y;

         Vertice[N][ _X ]= X;
         Vertice[N][ _Y ]= Y;
         Vertice[N][ _Z ]= Z;

         UV[N][0] = 1.0*i/(Nx-1);
         UV[N][1] = 1.0*j/(Ny-1);

         N++;
      }
   }

   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      for( i=0; i<Nx; ++i )
      {
         Poly->Pt[0] = ((i+1)%Nx) + j*Nx;
         Poly->Pt[2] = i + j*Nx;
         Poly->Pt[1] = ((i+1)%Nx) + (j+1)*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;

         Poly->Pt[0] = i + (j+1)*Nx;
         Poly->Pt[2] = ((i+1)%Nx) + (j+1)*Nx;
         Poly->Pt[1] = i + j*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;
      }
   }

   k = Nx2*Ny2;
   for( j=0; j<Ny2; ++j )
   {
      y = ( FLT )( j )*2.0/(Ny2-1)-1.0;
      for( i=0; i<Nx2; ++i )
      {
         FLT r;

         x = ( FLT )( i )*2.0/(Nx2-1)-1.0;

         X = fabs( x ); Y = fabs( y );
         r = sqrt(x*x+y*y);
         if ( r==0.0 ) r = 1.0;
         if ( Y>X ) r = Y/r; 
         else r = X/r;
         X = x*r;
         Y = y*r;

         Vertice[N][ _X ]= X*R;
         Vertice[N][ _Y ]= Y*R;
         Vertice[N][ _Z ]= H;

         Vertice[N+k][ _X ]= X*R;
         Vertice[N+k][ _Y ]= Y*R;
         Vertice[N+k][ _Z ]= -H;

         X = (X+1.0)/2.0;
         Y = (Y+1.0)/2.0;

         UV[N][0] = X;
         UV[N][1] = Y;
         UV[N+k][0] = X;
         UV[N+k][1] = Y;

         N++;
      }
   }

   Base_V = Nx*Ny;
   Base_V2 = Base_V + Nx2*Ny2;
   Poly = Obj->Polys + 2*Nx*(Ny-1);
   k = 2*(Nx2-1)*(Ny2-1);
   for( j=0; j<Ny2-1; ++j )
   {
      for( i=0; i<Nx2-1; ++i )
      {
         Poly->Pt[0] = Base_V + (i+1) + j*Nx2;
         Poly->Pt[2] = Base_V + i + j*Nx2; 
         Poly->Pt[1] = Base_V + (i+1) + (j+1)*Nx2;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;

         Poly[k].Pt[0] = Base_V2 + (i+1) + j*Nx2; 
         Poly[k].Pt[1] = Base_V2 + i + j*Nx2;
         Poly[k].Pt[2] = Base_V2 + (i+1) + (j+1)*Nx2;
         Poly[k].Colors = 0;
         Poly[k].Nb_Pts = 3;

         Poly++;

         Poly->Pt[0] = Base_V + i + (j+1)*Nx2;
         Poly->Pt[2] = Base_V + i+1 + (j+1)*Nx2;
         Poly->Pt[1] = Base_V + i + j*Nx2;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;

         Poly[k].Pt[0] = Base_V2 + i + (j+1)*Nx2;
         Poly[k].Pt[1] = Base_V2 + i+1 + (j+1)*Nx2;
         Poly[k].Pt[2] = Base_V2 + i + j*Nx2;
         Poly[k].Colors = 0;
         Poly[k].Nb_Pts = 3;
         Poly++;
      }
   }

   Average_Normals( Obj );
   Obj->Flags = (OBJ_FLAG)(Obj->Flags | OBJ_HAS_UV );
   return( Obj );
}

EXTERN MESH *Make_Cone( INT Nx, INT Ny, FLT R1, FLT R2, FLT H )
{
   INT i, j, k, N;
   MESH  *Obj;
   VECTOR *Vertice;
   INT Base_V, Base_V2;
   F_MAPPING *UV;
   POLY    *Poly;
   FLT   x, y, r;
   FLT   X, Y, Z;

   k = 2*Nx*(Ny-1);   /* nb Vertex */
   j = Nx*Ny;        /* nb Polys */

   Obj = Set_Mesh_Data( NULL, j, k );
   if ( Obj==NULL ) return( NULL );

   Vertice = Obj->Vertex;
   UV = Obj->UV;
   N = 0;
   for( j=0; j<Ny; ++j )
   {
      FLT R;
      R = R1 + (R2-R1)*j/(Ny-1);
      y = ( FLT )( j )*2.0/(Ny-1)-1.0;
      for( i=0; i<Nx; ++i )
      {
         x = ( FLT )( i )*2.0*M_PI/Nx;

         X = R*cos( x );
         Y = R*sin( x );
         Z = H*y;

         Vertice[N][ _X ]= X;
         Vertice[N][ _Y ]= Y;
         Vertice[N][ _Z ]= Z;

         UV[N][0] = 1.0*i/(Nx-1);
         UV[N][1] = 1.0*j/(Ny-1);

         N++;
      }
   }

   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      for( i=0; i<Nx; ++i )
      {
         Poly->Pt[0] = ((i+1)%Nx) + j*Nx;
         Poly->Pt[2] = i + j*Nx;
         Poly->Pt[1] = ((i+1)%Nx) + (j+1)*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;

         Poly->Pt[0] = i + (j+1)*Nx;
         Poly->Pt[2] = ((i+1)%Nx) + (j+1)*Nx;
         Poly->Pt[1] = i + j*Nx;
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

EXTERN MESH *Make_Torus( INT Nx, INT Ny, FLT R1, FLT R2 )
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
         Poly->Pt[1] = ((i+1)%Nx) + ((j+1)%Ny)*Nx;
         Poly->Pt[2] = ((i+1)%Nx) + (j%Ny)*Nx;
         Poly->Pt[0] = (i%Nx) + (j%Ny)*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;

         Poly->Pt[1] = ((i+1)%Nx) + ((j+1)%Ny)*Nx;
         Poly->Pt[2] = (i%Nx) + (j%Ny)*Nx;
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

EXTERN MESH *Make_Sphere( INT Nx, INT Ny, FLT Rx, FLT Ry, FLT Rz )
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
         
         x = ( FLT )( i )*2.01*M_PI/(Nx-1);

         X = Rx*cos( x )*sin( y );
         Y = Ry*sin( x )*sin( y );
         Z = Rz*cos( y );

         Vertice[N][ _X ]= X;
         Vertice[N][ _Y ]= Y;
         Vertice[N][ _Z ]= Z;

         UV[N][0] = 1.0*i/Nx;
         UV[N][1] = 1.0*j/Ny;

         N++;
      }
   }

   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      INT i;
      for( i=0; i<Nx-1; ++i )
      {
         Poly->Pt[0] = (i+1) + (j+1)*Nx;
         Poly->Pt[1] = (i+1) + j*Nx;
         Poly->Pt[2] = i + j*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;

         Poly->Pt[0] = (i+1) + (j+1)*Nx;
         Poly->Pt[1] = i + j*Nx;
         Poly->Pt[2] = i + (j+1)*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;

      }
   }
   Average_Normals( Obj );
   Obj->Flags = (OBJ_FLAG)(Obj->Flags | OBJ_HAS_UV );
   return( Obj );
}


EXTERN MESH *Make_Plane_XY( INT Nx, INT Ny, FLT Rx, FLT Ry,
   FLT Uo, FLT U, FLT Vo, FLT V )
{
   INT     j, N;
   MESH    *Obj;
   VECTOR  *Normals, *Vertice;
   F_MAPPING *UV;
   POLY    *Poly;

   Obj = Set_Mesh_Data( NULL, Nx*Ny, 2*(Nx-1)*(Ny-1) );
   if ( Obj==NULL ) return( NULL );
   Vertice = Obj->Vertex;
   Normals = Obj->Normals;
   UV = Obj->UV;
   N = 0;

   for( j=Ny-1; j>=0; --j )
   {
      FLT y;
      INT i;

      y = Ry*( ( FLT )( j )*2.0/(Ny-1)-1.0 );

      for( i=Nx-1; i>=0; --i )
      {
         FLT x;
         x = Rx*( ( FLT )( i )*2.0/(Nx-1)-1.0 );

         Vertice[N][0] = x;
         Vertice[N][1] = y;
         Vertice[N][2] = 0.0;
         Normals[N][0] = 0.0;
         Normals[N][1] = 0.0;
         Normals[N][2] = -1.0;
         UV[N][0] = (U-Uo)*i/(Nx-1) + Uo;
         UV[N][1] = (V-Vo)*j/(Ny-1) + Vo;

         N++;
         
      }
   }

   Poly = Obj->Polys;
   for( j=Ny-2; j>=0; --j )
   {
      INT i;
      for( i=Nx-2; i>=0; --i )
      {
         Poly->Pt[0] = (i+1) + (j+1)*Nx;
         Poly->Pt[1] = (i+1) + j*Nx;
         Poly->Pt[2] = i + j*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;

         Poly->Pt[0] = (i+1) + (j+1)*Nx;
         Poly->Pt[1] = i + j*Nx;
         Poly->Pt[2] = i + (j+1)*Nx;
         Poly->Colors = 0;
         Poly->Nb_Pts = 3;
         Poly++;
      }
   }

   Obj->Flags = (OBJ_FLAG)(Obj->Flags | OBJ_HAS_UV ); /* | OBJ_HAS_NORMALS */   
   Average_Normals( Obj );
   return( Obj );
}


/*****************************************************************/

#if 0

EXTERN OBJECT *Make_Plane2( INT Nx, INT Ny, FLT Rx, FLT Ry,
   SHORT Uo, SHORT U, SHORT Vo, SHORT V )
{
   INT i, j, k;
   OBJECT  *Obj;
   VERTICE *Vertice, *Base_V, *Base_V2;
   POLY    *Poly;
   FLT x, y;

   Obj = New_Object( (Nx-1)*(Ny-1), Nx*Ny );
   Vertice = Obj->Vertices;
   for( j=0; j<Ny; ++j )
   {
      y = ( FLT )( j )*2.0/(Ny-1)-1.0;
      for( i=0; i<Nx; ++i )
      {
         x = ( FLT )( i )*2.0/(Nx-1)-1.0;

         Vertice->Base_Pt[ _X ]= ( int )floor( Rx*x );
         Vertice->Base_Pt[ _Y ]= ( int )floor( Ry*y );
         Vertice->Base_Pt[ _Z ]= ( int )floor( 0 );
         Vertice->Base_N[_X] = 0;
         Vertice->Base_N[_Y] = 0;
         Vertice->Base_N[_Z] = 32767;

         Vertice->u = (USHORT)( ( i*(U-Uo)/(Nx-1)+Uo )*255.9 );
         Vertice->v = (USHORT)( ( j*(V-Vo)/(Ny-1)+Vo )*255.9 );
                           
         Vertice++;
      }
   }

   Base_V = Obj->Vertices;
   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      for( i=0; i<Nx-1; ++i )
      {
         int o1, o2, o3, Base;

         Base = i+j*Nx;

         switch ( (i+j)&0x03 ) {
            case 0: o1 = Base+1; o2 = Base; o3 = Base+Nx; break;
            case 1: o1 = Base+1; o2 = Base; o3 = Base+Nx+1; break;
            case 2: o1 = Base; o2 = Base+Nx; o3 = Base+Nx+1; break;
            case 3: o1 = Base+1; o2 = Base+Nx; o3 = Base+Nx+1; break;
         }

         Poly->P1 = Base_V + o1;
         Poly->P2 = Base_V + o3;
         Poly->P3 = Base_V + o2;
         Poly->Texture_Nb = 0;
         Poly++;

      }
   }
   return( Obj );
}

EXTERN OBJECT *Make_Plane_XY( int Nx, int Ny, FLT Rx, FLT Ry,
   SHORT Uo, SHORT U, SHORT Vo, SHORT V )
{
   int i, j, k;
   OBJECT  *Obj;
   VERTICE *Vertice, *Base_V, *Base_V2;
   POLY    *Poly;
   FLT x, y;

   Obj = New_Object( 2*(Nx-1)*(Ny-1), Nx*Ny );
   Vertice = Obj->Vertices;
   for( j=0; j<Ny; ++j )
   {
      y = ( FLT )( j )*2.0/(Ny-1)-1.0;
      for( i=0; i<Nx; ++i )
      {
         x = ( FLT )( i )*2.0/(Nx-1)-1.0;

         Vertice->Base_Pt[ _X ]= ( int )floor( Rx*x );
         Vertice->Base_Pt[ _Y ]= ( int )floor( Ry*y );
         Vertice->Base_Pt[ _Z ]= 0;
         Vertice->Base_N[_X] = 0;
         Vertice->Base_N[_Y] = 0;
         Vertice->Base_N[_Z] = 32000;
         Vertice->u = (USHORT)( ( i*(U-Uo)/(Nx-1)+Uo )*255.9 );
         Vertice->v = (USHORT)( ( j*(V-Vo)/(Ny-1)+Vo )*255.9 );

         Vertice++;
      }
   }

   Vertice = Obj->Vertices;
   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      for( i=0; i<Nx-1; ++i )
      {
         Poly->P1 = Vertice + (i+1) + (j+1)*Nx;
         Poly->P3 = Vertice + (i+1) + j*Nx;
         Poly->P2 = Vertice + i + j*Nx;
         Poly->Texture_Nb = 0;
         Poly++;

         Poly->P1 = Vertice + (i+1) + (j+1)*Nx;
         Poly->P3 = Vertice + i + j*Nx;
         Poly->P2 = Vertice + i + (j+1)*Nx;
         Poly->Texture_Nb = 0;
         Poly++;

      }
   }
   return( Obj );
}

EXTERN OBJECT *Make_Plane_XZ( int Nx, int Ny, FLT Rx, FLT Ry,
   SHORT Uo, SHORT U, SHORT Vo, SHORT V )
{
   int i, j, k;
   OBJECT  *Obj;
   VERTICE *Vertice, *Base_V, *Base_V2;
   POLY    *Poly;
   FLT x, y;

   Obj = New_Object( 2*(Nx-1)*(Ny-1), Nx*Ny );
   Vertice = Obj->Vertices;
   for( j=0; j<Ny; ++j )
   {
      y = ( FLT )( j )*2.0/(Ny-1)-1.0;
      for( i=0; i<Nx; ++i )
      {
         x = ( FLT )( i )*2.0/(Nx-1)-1.0;

         Vertice->Base_Pt[ _X ]= ( int )floor( Rx*x );
         Vertice->Base_Pt[ _Y ]= 0;
         Vertice->Base_Pt[ _Z ]= ( int )floor( Ry*y );
         Vertice->Base_N[_X] = 0;
         Vertice->Base_N[_Y] = -32000;
         Vertice->Base_N[_Z] = 0;
         Vertice->u = (USHORT)( ( i*(U-Uo)/(Nx-1)+Uo )*255.9 );
         Vertice->v = (USHORT)( ( j*(V-Vo)/(Ny-1)+Vo )*255.9 );

         Vertice++;
      }
   }

   Vertice = Obj->Vertices;
   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      for( i=0; i<Nx-1; ++i )
      {
         Poly->P1 = Vertice + (i+1) + (j+1)*Nx;
         Poly->P3 = Vertice + (i+1) + j*Nx;
         Poly->P2 = Vertice + i + j*Nx;
         Poly->Texture_Nb = 0;
         Poly++;

         Poly->P1 = Vertice + (i+1) + (j+1)*Nx;
         Poly->P3 = Vertice + i + j*Nx;
         Poly->P2 = Vertice + i + (j+1)*Nx;
         Poly->Texture_Nb = 0;
         Poly++;

      }
   }
   return( Obj );
}

/*****************************************************************/

EXTERN OBJECT *Make_Patch_uv( int Nx, int Ny, PATCH Patch, 
   SHORT Uo, SHORT U, SHORT Vo, SHORT V )
{
   int i, j, k;
   OBJECT  *Obj;
   VERTICE *Vertice, *Base_V, *Base_V2;
   POLY    *Poly;
   FLT x, y, z;

   Obj = New_Object( 2*(Nx-1)*(Ny-1), Nx*Ny );
   Vertice = Obj->Vertices;
   for( j=0; j<Ny; ++j )
   {
      y = ( FLT )( j )/(Ny-1);
      for( i=0; i<Nx; ++i )
      {
         FLT u, v;
         x = ( FLT )( i )/(Nx-1);
         Patch_Point( Vertice->Base_Pt, x, y, Patch );
         Patch_Normal( Vertice->Base_N, x, y, Patch );
/*
         printf( "#%d %d: X: %d   Y: %d   Z:%d\n", i,j,
            Vertice->Base_Pt[_X],
            Vertice->Base_Pt[_Y],
            Vertice->Base_Pt[_Z] );
*/
         Vertice->u = (USHORT)( ( x*(U-Uo)+Uo )*255.9 );
         Vertice->v = (USHORT)( ( y*(V-Vo)+Vo )*255.9 );
         Vertice++;
      }
   }

   Vertice = Obj->Vertices;
   Poly = Obj->Polys;
   for( j=0; j<Ny-1; ++j )
   {
      for( i=0; i<Nx-1; ++i )
      {
         Poly->P1 = Vertice + (i+1) + (j+1)*Nx;
         Poly->P3 = Vertice + (i+1) + j*Nx;
         Poly->P2 = Vertice + i + j*Nx;
         Poly->Texture_Nb = 0;
         Poly++;

         Poly->P1 = Vertice + (i+1) + (j+1)*Nx;
         Poly->P3 = Vertice + i + j*Nx;
         Poly->P2 = Vertice + i + (j+1)*Nx;
         Poly->Texture_Nb = 0;
         Poly++;

      }
   }
   Obj->Nb_Textures = 1;
   Obj->Flags |= OBJ_HAS_UV;
   return( Obj );
}

EXTERN OBJECT *Make_Patch( int Nx, int Ny, PATCH Patch )
{
   return( Make_Patch_uv( Nx, Ny, Patch, 0, 256,  0, 256 ) );
}


/*****************************************************************/

#endif   // 0
