/***********************************************
 *    Marching cubes (for blobs)               *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

// #define DEBUG_MC_NORMAL

#ifdef DO_MCUBE2

EXTERN FLT *Iso_Z;

/******************************************************************/
/******************************************************************/
/******************************************************************/

#ifdef MC_SAFE_BOUNDS

static inline void Fill_X_Y_Edges( )
{
   INT Off, i;

   Off = 0;
   for( i=0; i<MCX-1; ++i )
   {
      INT j;
      for( j=0; j<MCY-1; ++j )
      {
         if ( !(Raw_Flags[Off]^Raw_Flags[Off+MCY]) )
         {
            INT Vtx_ID;
            Vtx_ID = MC_Blob->Nb_Vertex++;
            X_Edge[Off] = Vtx_ID;
            MC_Blob->Vertex[Vtx_ID][1] = j*MC_SCALE_Y + MC_OFF_Y;
            MC_Blob->Vertex[Vtx_ID][2] = Slice[2];
            {
               FLT x;
               x = (FLT)i;
               x += Raw_Slice_A[Off]/( Raw_Slice_A[Off] - Raw_Slice_A[Off+MCY] );
               MC_Blob->Vertex[Vtx_ID][0] = MC_OFF_X + x*MC_SCALE_X;
            }
         }

         if ( !(Raw_Flags[Off]^Raw_Flags[Off+1]) )
         {
            INT Vtx_ID;
            Vtx_ID = MC_Blob->Nb_Vertex++;
            Y_Edge[Off] = Vtx_ID;
            MC_Blob->Vertex[Vtx_ID][0] = i*MC_SCALE_X + MC_OFF_X;
            MC_Blob->Vertex[Vtx_ID][2] = Slice[2];
            {
               FLT y;
               y = (FLT)j;
               y += Raw_Slice_A[Off]/( Raw_Slice_A[Off] - Raw_Slice_A[Off+1] );
               MC_Blob->Vertex[Vtx_ID][1] = MC_OFF_Y + y*MC_SCALE_Y;
            }
         }
         Off++;
      }
      Off++;
   }
}

static inline void Fill_Z_yTop_Edge( )
{
   INT j;

   for( j=MCY-2; j>=0; --j )
   {
      if ( !(Flags_A[j]^Flags_B[j]) )
      {
         INT Vtx_ID;
         Vtx_ID = MC_Blob->Nb_Vertex++;
         Z_Edge[j] = Vtx_ID;
         MC_Blob->Vertex[Vtx_ID][0] = MC_OFF_X;
         MC_Blob->Vertex[Vtx_ID][1] = j*MC_SCALE_Y + MC_OFF_Y;
         {
            FLT z;
            z = Slice_B[j]/( Slice_A[j] - Slice_B[j] );
            MC_Blob->Vertex[Vtx_ID][2] = Slice[2] + z*MC_SCALE_Z;
         }
      }
      if ( !(Flags_B[j]^Flags_B[j+1]) )
      {
         INT Vtx_ID;
         Vtx_ID = MC_Blob->Nb_Vertex++;
         Top_yEdge[j] = Vtx_ID;
         MC_Blob->Vertex[Vtx_ID][0] = MC_OFF_X;
         MC_Blob->Vertex[Vtx_ID][2] = Slice[2] + MC_SCALE_Z;
         {
            FLT y;
            y = (FLT)j;
            y += Slice_B[j]/( Slice_B[j] - Slice_B[j+1] );
            MC_Blob->Vertex[Vtx_ID][1] = MC_OFF_Y + y*MC_SCALE_Y;
         }
      }
   }
}

static inline void Fill_Vtx_6_10( INT Off, INT i )
{
   if ( !(Flags_B[Off+1]^Flags_B[Off+1+MCY]) )
   {
      Vtx_6 = MC_Blob->Nb_Vertex++;
      MC_Blob->Vertex[Vtx_6][1] = MC_OFF_Y;
      MC_Blob->Vertex[Vtx_6][2] = Slice[2];
      {
         FLT x;
         x = (FLT)i;
         x += Slice_B[Off]/( Slice_B[Off] - Slice_B[Off+MCY] );
         MC_Blob->Vertex[Vtx_6][0] = MC_OFF_X + x*MC_SCALE_X;
      }
   }

   if ( !(Flags_A[Off+1+MCY]^Flags_B[Off+1+MCY]) )
   {
      Vtx_10 = MC_Blob->Nb_Vertex++;
      MC_Blob->Vertex[Vtx_10][0] = MC_SCALE_X*i + MC_OFF_X + MC_SCALE_X;
      MC_Blob->Vertex[Vtx_10][1] = MC_OFF_Y;
      {
         FLT z;
         z = Slice_B[Off+1+MCY]/( Slice_A[Off+1+MCY] - Slice_B[Off+1+MCY] );
         MC_Blob->Vertex[Vtx_10][2] = Slice[2] + z*MC_SCALE_Z;
      }
   }   
}

#endif   // MC_SAFE_BOUNDS

/******************************************************************/

#if 0

static inline FLT Do_V( VECTOR P )
{
   INT k;
   FLT V, R2, Inv_R2;

   V = -O_Blob->K;

   for( k=O_Blob->Nb_Centers-1; k>=0; --k )
   {
      FLT x, Tmp;

      Tmp = P[0] - O_Blob->G[k][0];
      x  = Tmp*Tmp;
      if ( x>O_Blob->Radius2[k] ) continue;

      Tmp = P[1] - O_Blob->G[k][1];
      x += Tmp*Tmp;
      if ( x>O_Blob->Radius2[k] ) continue;

      Tmp = P[2] - O_Blob->G[k][2];
      x += Tmp*Tmp;
      if ( x>O_Blob->Radius2[k] ) continue;

      x *= O_Blob->Inv_Radius2[k];
      V += (1-x)*O_Blob->Strength[k];
   }
   return( V );
}

static inline void Fill_Slice( FLT *Ptr, PIXEL *Flags )
{
   INT j, Off;

   j = MCY-1;
   Slice[1] = j*MC_SCALE_Y + MC_OFF_Y;
   Off = MCX*MCY-1;
   for( ; j>=0; --j )
   {
      INT i;

      i=MCX-1;
      Slice[0] = i*MC_SCALE_X + MC_OFF_X;
      for( ; i>=0; --i )
      {
         Ptr[ Off ] = Do_V( Slice, Blob );
         if ( Ptr[Off]<0.0 ) Flags[Off] = 0x00;
         else Flags[Off] = 0x01;
         Slice[0] -= MC_SCALE_X;
         Off--;
      }
      Slice[1] -= MC_SCALE_Y;
   }
}
#endif

/******************************************************************/

EXTERN void Do_MCube( MC_BLOB *Blob )
{
   INT i, j, k;

   O_Blob = Blob;

   MC_Blob = Blob->Blob;
   MC_Blob->Nb_Vertex = 0;
   MC_Blob->Nb_Polys = 0;

   Slice[2] = MC_OFF_Z;
   Slice_A = Raw_Slice; Slice_B = Raw_Slice + MCX*MCY;
   Flags_A = Raw_Flags; Flags_B = Raw_Flags + MCX*MCY;

   Clear_Slice_A( -Blob->K );
   Fill_Slice3( Raw_Slice, Raw_Flags, Blob );
#ifdef MC_SAFE_BOUNDS
   Fill_X_Y_Edges( );
#endif

   Emit_All_MC( Blob );

   MC_Blob->Flags &= ~OBJ_HAS_NORMALS;
   Mesh_Store_Normals2( MC_Blob );
   // Average_Normals( MC_Blob );
}

/******************************************************************/
/******************************************************************/

EXTERN void MC_Blobify( MC_BLOB *Blob, FLT Radius, FLT Strength )
{
   INT i;

   if ( Blob==NULL ) return;
   if ( Blob->Nb_Centers==MAX_CENTERS ) return;

   i = Blob->Nb_Centers++;
   Blob->Radius[i]      = Radius;
   Blob->Radius2[i]     = (Radius*Radius);
   Blob->Inv_Radius2[i] = BLOB_TABLE_SIZE/(Radius*Radius);
   Blob->Strength[i]    = Strength;
   Set_Vector( Blob->G[i], 0.0, 0.0, 0.0 );
}


EXTERN MESH *MC_Finish_Blob( MC_BLOB *Blob, INT Max_Vtx, INT Max_Polys )
{
   INT i, j;

   Build_MCube_Tab( );
   /* Dump_MCube_Tab( ); */
   Build_Default_Blob_Table( );

   Blob->Blob = Set_Mesh_Data( Blob->Blob, Max_Vtx, Max_Polys );
   if ( Blob->Blob == NULL )
      goto Failed;

   for( j=0; j<Blob->Blob->Nb_Polys; ++j )
   {
      Blob->Blob->Polys[j].Nb_Pts = 3;
   }
   Blob->Blob->Flags |= OBJ_DONT_OPTIMIZ;

      /* XXX Hack so box is set up */
   Blob->Blob->Vertex[0][0] = MC_OFF_X;
   Blob->Blob->Vertex[0][1] = MC_OFF_Y;
   Blob->Blob->Vertex[0][2] = MC_OFF_Z;
   Blob->Blob->Vertex[1][0] = MC_OFF_X + MC_SCALE_X*MCX;
   Blob->Blob->Vertex[1][1] = MC_OFF_Y + MC_SCALE_Y*MCY;
   Blob->Blob->Vertex[1][2] = MC_OFF_Z + MC_SCALE_Z*MCZ;

   return( Blob->Blob );

Failed:

   if ( Blob->Blob != NULL )
   {
      Destroy_Object( (OBJECT *)Blob->Blob, _MESH_.Destroy );
      M_Free( Blob->Blob );
   }
   return( NULL );
}

/******************************************************************/
/******************************************************************/

#if 0

EXTERN FLT *Install_Iso_Mesh( MC_BLOB *Blob, OBJ_NODE *Node, INT Slot )
{
   FLT    *Ho = NULL;
   VECTOR *No = NULL;
   FLT    *Tab = NULL;
   INT i;
   MESH *Mesh;

   Mesh = (MESH *)Node->Data;

              /* Huggh@!! */

   Tab = New_Object( MCX*MCY*MCZ, FLT );
   if ( Tab==NULL ) goto Failed;

   Ho = New_Object( Mesh->Nb_Vertex, FLT );
   if ( Ho==NULL ) goto Failed;

   No = New_Object( Mesh->Nb_Vertex, VECTOR );
   if ( No==NULL ) goto Failed;

      /* Recompute fixed data */

   Average_Normals( Mesh );

   Transform_To_Matrix( &Node->Transform, Node->Mo );
   for( i=Mesh->Nb_Vertex-1; i>=0; --i )
   {
      VECTOR Po;
      nA_V( No[i], (FLT *)&Node->Mo[9], Mesh->Normals[i] );
      AT_V( Po, Node->Mo, Mesh->Vertex[i] );
      Ho[i] = Dot_Product( Po, No[i] );
   }

   Blob->Ho[Slot] = Ho;
   Blob->No[Slot] = No;
   Blob->Iso_Mesh[Slot] = Tab;

   return( Tab );

Failed:
   M_Free( Tab );
   M_Free( Ho );
   M_Free( No );
   return( NULL );
}

/******************************************************************/

EXTERN FLT *Mesh_To_Iso( MC_BLOB *Blob, OBJ_NODE *Node )
{
   FLT *Tab, *Ho;
   VECTOR *No;
   USHORT *Prox;
   INT i, j, k;
   FLT z, Max, Min;
   INT N;
   MESH *Mesh;

   if ( Blob->Nb_Iso_Mesh==MAX_ISO_MESH ) return( NULL );

   Mesh = (MESH *)Node->Data;

   if ( Install_Iso_Mesh( Blob, Node, Blob->Nb_Iso_Mesh )==NULL )
      return( NULL );

   Ho = Blob->Ho[ Blob->Nb_Iso_Mesh ];
   No = Blob->No[ Blob->Nb_Iso_Mesh ];
   Tab = Blob->Iso_Mesh[ Blob->Nb_Iso_Mesh ];

   Prox = New_Object( MCX*MCY*MCZ, USHORT );     /* Huggh@!! */
   if ( Prox==NULL ) goto Failed;
   Blob->Prox[ Blob->Nb_Iso_Mesh ] = Prox;

   Blob->Nb_Iso_Mesh++;

   fprintf( stderr, " 3D-bluring mesh...\n" );

   Max = -_HUGE_;
   Min = _HUGE_;
   N = MCZ*MCY*MCX-1;
   z = MC_OFF_Z + (MCZ-1)*MC_SCALE_Z;
   for( k=MCZ-1; k>=0; --k )
   {
      FLT x;
      x = MC_OFF_X + (MCX-1)*MC_SCALE_X;         
      for( i=MCX-1; i>=0; --i )
      {
         FLT y;
         y = MC_OFF_Y + (MCY-1)*MC_SCALE_Y;
         for( j=MCY-1; j>=0; --j )
         {
            FLT d2_Best, d;
            INT n, n_Best;

            d2_Best = _HUGE_;

            for( n=Mesh->Nb_Vertex-1; n>=0; --n )
            {
               VECTOR Po;
               FLT d2, d;

               AT_V( Po, Node->Mo, Mesh->Vertex[n] );

               d = (Po[0]-x); d2 = d*d;
               d = (Po[1]-y); d2 += d*d;
               d = (Po[2]-z); d2 += d*d;
               if ( d2<d2_Best )
               { 
                  d2_Best = d2; 
                  n_Best = n;
               }
            }
            d  = No[n_Best][0]*x + No[n_Best][1]*y + No[n_Best][2]*z;
            d -= Ho[n_Best];
            d = -atan( d/Blob->Tension )*(2.0/M_PI); /* in [-1,1] */
            Tab[N] = d;
            if ( d>Max ) Max = d;
            if ( d<Min ) Min = d;
            Prox[N] = n_Best;
            fprintf( stderr, "Point #%d   \r", N );
            N--;
            y -= MC_SCALE_Y;
         }
         x -= MC_SCALE_X;
      }
      z -= MC_SCALE_Z;
   }

   if ( Max==Min ) Max += 0.01;
   for( i=MCZ*MCY*MCX-1; i>=0; --i )
   {
      Tab[i] = 2.0 * ( Tab[i]-Min ) / ( Max-Min ) - 1.0;
      /* fprintf( stderr, "%f ", Tab[i] ); */
   }

   fprintf( stderr, "\n ...done\n" );

   return( Tab );

Failed:
   M_Free( Tab );
   M_Free( Ho );
   M_Free( No );
   M_Free( Prox );
   return( NULL );
}

#endif   // 0

/******************************************************************/
/******************************************************************/

EXTERN void Do_MCube2( MC_BLOB *Blob, FLT *Iso, FLT Blend )
{
   INT i, j, k;

   O_Blob = Blob;

   MC_Blob = Blob->Blob;
   MC_Blob->Nb_Vertex = 0;
   MC_Blob->Nb_Polys = 0;
   Blob->Blend0 = Blend;
   Blob->Blend1 = 1.0 - Blend;

   Slice[2] = MC_OFF_Z;
   Slice_A = Raw_Slice; Slice_B = Raw_Slice + MCX*MCY;
   Flags_A = Raw_Flags; Flags_B = Raw_Flags + MCX*MCY;

   Iso_Z = Iso;
   for( i = MCX*MCY-1; i>=0; --i ) 
      Slice_A[i] = Blob->Blend1*Iso_Z[i] - Blob->K;
   Fill_Slice3( Slice_A, Flags_A, Blob );
   Iso_Z += MCZ*MCY;

#ifdef MC_SAFE_BOUNDS
   Fill_X_Y_Edges( );
#endif

   for( k=0; k<MCZ-1; ++k )
   {
      INT Off;

      Slice[2] += MC_SCALE_Z;
      for( i = MCX*MCY-1; i>=0; --i )
         Slice_B[i] = Blob->Blend1*Iso_Z[i] - Blob->K;
      Fill_Slice3( Slice_B, Flags_B, Blob );
#ifdef MC_SAFE_BOUNDS
      Fill_Z_yTop_Edge( );
#endif

      Off = 0;
      for( i=0; i<MCX-1; ++i )
      {
#ifdef MC_SAFE_BOUNDS
         Fill_Vtx_6_10( Off, i );
#endif

         for( j=0; j<MCY-1; ++j )
         {
            PIXEL Index;

            Index = Flags_A[Off];
            Index |= Flags_B[Off]<<4;
            Index |= Flags_A[Off + MCY]<<1 ;
            Index |= Flags_B[Off + MCY]<<5; 
            Index |= Flags_A[Off + 1]<<3;
            Index |= Flags_B[Off + 1]<<7;
            Index |= Flags_A[Off + MCY+1]<<2;
            Index |= Flags_B[Off + MCY+1]<<6;

            if ( (Index!=0x00) && (Index!=0xFF) )
               Emit_MC_Polys( Off, Index, i,j );

            Off++;      /* Off = MCY*i + j */
         }
         Off++;  /* Next row */
      }
      { void *Tmp; Tmp = Slice_A; Slice_A = Slice_B; Slice_B = Tmp; }
      { void *Tmp; Tmp = Flags_A; Flags_A = Flags_B; Flags_B = Tmp; }
      Iso_Z += MCZ*MCY;
   }

   MC_Blob->Flags &= ~OBJ_HAS_NORMALS;
   Mesh_Store_Normals2( MC_Blob );
   Average_Normals( MC_Blob );
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

#if 0    // #ifdef UNIX

EXTERN void Fill_Slice3( FLT *Ptr, PIXEL *Flags, MC_BLOB *Blob )
{
   INT Off, n;
   FLT Xo, Yo, Zo;

   for( n=Blob->Nb_Centers-1; n>=0; --n )
   {
      FLT Tmp, ro, L, Strength;
      INT io, i2, jo, j2;
      INT i, j;

      L = Blob->Radius[n];
      Zo = ro = Slice[2]; // - Blob->G[n][2];
      if ( ro>L || ro<-L ) continue;

      // Strength = Blob->Strength[n] * Blob->Blend1;

      io = (INT)( (-L-MC_OFF_X)/MC_SCALE_X );
      i2 = (INT)( (+L-MC_OFF_X)/MC_SCALE_X );
      jo = (INT)( (-L-MC_OFF_Y)/MC_SCALE_Y );
      j2 = (INT)( (+L-MC_OFF_Y)/MC_SCALE_Y );

      ro = ro*ro;
      Slice[0] = MC_SCALE_X*io + MC_OFF_X;
      for( i=io; i<=i2; ++i )
      {
         FLT Tmp, r2;

         Xo = Tmp = Slice[0];  // - Blob->G[n][0];
         r2 = ro + Tmp*Tmp;
         if ( r2>Blob->Radius2[n] ) goto Skip;

         Off = i*MCY+jo;
         Slice[1] = MC_SCALE_Y*jo + MC_OFF_Y;
         for( j=jo; j<=j2; ++j )
         {
            FLT Tmp, r3;
            Yo = Tmp = Slice[1]; // - Blob->G[n][1];
            r3 = r2 + Tmp*Tmp;
            if ( r3<Blob->Radius2[n] )
            {
               INT M;
               FLT D, X2, X, Y, Z;
               X=Xo/4.0;
               Y=Yo/4.0;
               Z=Zo/4.0;
               D = Y*Y + Z*Z; X2 = X*X;

               for ( M=6; M>=0; --M )
               {
                  X *= 2.0;
                  Y = X*Y + Blob->G[n][1];
                  Z = X*Z + Blob->G[n][2];
                  X = X2-D + Blob->G[n][0];
                  D = Y*Y + Z*Z; X2 = X*X;
                  // fprintf( stderr, "%f    ", X2+D );
               }
               Ptr[Off] = X2+D;
               // fprintf( stderr, "\n" );
            }

            Slice[1] += MC_SCALE_Y;
            Off++;
         }
Skip:
         Slice[0] += MC_SCALE_X;
      }
   }

   for( n = MCX*MCY-1; n>=0; --n )
      if ( Ptr[n] < 0.0 ) Flags[n] = 0x00;
      else Flags[n] = 0x01;
}
#endif

/******************************************************************/

                     // T E S T

#ifndef EMIT_MC_POLYS_ASM

EXTERN void Emit_MC_Polys( INT Off, INT Index, INT io, INT jo )
{
   PIXEL Edge;
   USHORT Vtx_ID, Edge_Table[12];
   INT i;

   i = 0;
   while( (Edge=Edge_Order[Index][i])!=0xFF )
   {
      switch( Edge )
      {
         case 0: Vtx_ID = X_Edge[ Off ]; break;
         case 1: Vtx_ID = Y_Edge[ Off + MCY ]; break;
         case 2: Vtx_ID = X_Edge[ Off + 1 ]; break;
         case 3: Vtx_ID = Y_Edge[ Off ]; break;
         case 4: Vtx_ID = X_Edge[ Off ] = Vtx_6; break;

         case 5: 
            Vtx_ID = MC_Blob->Nb_Vertex;
            MC_Blob->Nb_Vertex++;
            Top_yEdge[ jo ] = Vtx_ID;
#ifdef MC_SAFE_BOUNDS
            if ( io==MCX-2 ) Y_Edge[ Off + MCY ] = Vtx_ID;
#endif
            MC_Blob->Vertex[Vtx_ID][0] = io*MC_SCALE_X + MC_OFF_X + MC_SCALE_X;
            MC_Blob->Vertex[Vtx_ID][2] = Slice[2];
            {
               FLT y;               
               y = Slice_B[Off+MCY]/( Slice_B[Off+MCY] - Slice_B[Off+MCY+1] );
               // y = .5;
               y += (FLT)jo;
               MC_Blob->Vertex[Vtx_ID][1] = MC_OFF_Y + y*MC_SCALE_Y;
#ifdef DO_UV
               MC_Blob->UV[Vtx_ID][0] = y;
               MC_Blob->UV[Vtx_ID][1] = (FLT)io/MCZ;
#endif
            }
            Compute_MC_Normal( Vtx_ID );
         break;

         case 6:
            Vtx_ID = Vtx_6 = MC_Blob->Nb_Vertex;
            MC_Blob->Nb_Vertex++;
#ifdef MC_SAFE_BOUNDS
            if ( jo==MCY-2 ) X_Edge[ Off+1 ] = Vtx_ID;
#endif
            MC_Blob->Vertex[Vtx_ID][1] = jo*MC_SCALE_Y + MC_OFF_Y + MC_SCALE_Y;
            MC_Blob->Vertex[Vtx_ID][2] = Slice[2];
            {
               FLT x;
               x = Slice_B[Off+1]/( Slice_B[Off+1] - Slice_B[Off+MCY+1] );
               // x = .5;
               x += (FLT)io;
               MC_Blob->Vertex[Vtx_ID][0] = MC_OFF_X + x*MC_SCALE_X;
#ifdef DO_UV
               MC_Blob->UV[Vtx_ID][0] = x;
               MC_Blob->UV[Vtx_ID][1] = (FLT)io/MCZ;
#endif
            }
            Compute_MC_Normal( Vtx_ID );
         break;

         case 7: Vtx_ID = Y_Edge[ Off ] = Top_yEdge[ jo ]; break;
         case 8: Vtx_ID = Z_Edge[ jo ]; break;
         case 9: Vtx_ID = Z_Edge[ jo ] = Vtx_10; break;

         case 10:
            Vtx_ID = Vtx_10 = MC_Blob->Nb_Vertex;
            MC_Blob->Nb_Vertex++;

#ifdef MC_SAFE_BOUNDS
            if ( jo==MCY-2 ) Z_Edge[ MCY-1 ] = Vtx_ID;
#endif
            MC_Blob->Vertex[Vtx_ID][0] = io*MC_SCALE_X + MC_OFF_X + MC_SCALE_X;
            MC_Blob->Vertex[Vtx_ID][1] = jo*MC_SCALE_Y + MC_OFF_Y + MC_SCALE_Y;
            {
               FLT z;
               z = Slice_B[Off+MCY+1]/( Slice_A[Off+MCY+1] - Slice_B[Off+MCY+1] );
               // z = -.5;
               // fprintf( stderr, "z=%.4f  ", z );
               MC_Blob->Vertex[Vtx_ID][2] = Slice[2] + z*MC_SCALE_Z;

#ifdef DO_UV
               MC_Blob->UV[Vtx_ID][0] = -z;
               MC_Blob->UV[Vtx_ID][1] = (FLT)io/MCZ;
#endif
            }
            Compute_MC_Normal( Vtx_ID );
         break;

         case 11: Vtx_ID = Z_Edge[ jo + 1 ]; break;
      }

      Edge_Table[Edge] = Vtx_ID;
      i++;
   }

   i = 0;
   while( Poly_Order[Index][i]!=0xFF )
   {
      POLY *P;
      P = MC_Blob->Polys + MC_Blob->Nb_Polys;
      MC_Blob->Nb_Polys++;
      P->Pt[0] = Edge_Table[ Poly_Order[Index][i] ];
      P->Pt[1] = Edge_Table[ Poly_Order[Index][i+1] ];
      P->Pt[2] = Edge_Table[ Poly_Order[Index][i+2] ];
      i+=3;
   }
}
#endif

/******************************************************************/

#endif
