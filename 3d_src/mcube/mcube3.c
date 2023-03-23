/***********************************************
 * Marching cubes IV (optimized iso-functions) *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

// #define CALL_STATS
// #define PRINT_SLICES
// #define DUMP_TABLE

/******************************************************************/

#ifdef CALL_STATS
EXTERN INT Call_Count;
#endif

EXTERN INT (*_Do_V_)( INT );
EXTERN void (*_Compute_MC_Normal_)( INT );

/******************************************************************/
/******************************************************************/

EXTERN USHORT X_Edge[MCX*MCY];
EXTERN USHORT Y_Edge[MCX*MCY];
EXTERN USHORT Z_Edge[MCY];
EXTERN USHORT Top_yEdge[MCY];
EXTERN USHORT Vtx_6, Vtx_10;
EXTERN VECTOR Slice;

EXTERN MESH *MC_Blob;
EXTERN MC_BLOB *O_Blob;

EXTERN BYTE Poly_Order[256][MC_PTS];
EXTERN BYTE Edge_Order[256][16];

EXTERN FLT Blob_Table[BLOB_TABLE_SIZE];
EXTERN FLT Blob_Table2[BLOB_TABLE_SIZE];

EXTERN FLT MC_SCALE_X  = (40.0/MCX);
EXTERN FLT MC_SCALE_Y  = (40.0/MCY);
EXTERN FLT MC_SCALE_Z  = (40.0/MCZ);
EXTERN FLT I_MC_SCALE_X  = MCX/40.0;
EXTERN FLT I_MC_SCALE_Y  = MCY/40.0;
EXTERN FLT I_MC_SCALE_Z  = MCZ/40.0;
EXTERN FLT MC_OFF_X    = -20.0;
EXTERN FLT MC_OFF_Y    = -20.0;
EXTERN FLT MC_OFF_Z    = -20.0;
EXTERN FLT MC_OFF_END_X    = 20.0;
EXTERN FLT MC_OFF_END_Y    = 20.0;
EXTERN FLT MC_OFF_END_Z    = 20.0;

EXTERN FLT Raw_Slice[2*MCX*MCY];
EXTERN BYTE Raw_Flags[2*MCX*MCY];
EXTERN FLT *Slice_A, *Slice_B;
EXTERN BYTE *Flags_A, *Flags_B;

EXTERN UINT MC_Last_Flags[ 2*MCX*MCY ];  // 1 bit per z-slice
EXTERN UINT *Flags1, *Flags2;
EXTERN UINT MC_Mask;
EXTERN UINT Ngh_Tab[256];

/******************************************************************/
/******************************************************************/

#ifndef DO_V_ASM

EXTERN INT Do_V( INT Off )
{
   INT k; FLT V;

   V = -O_Blob->K;

   for( k=O_Blob->Nb_Centers-1; k>=0; --k )
   {
      FLT x, Tmp;

      Tmp = Slice[0] - O_Blob->G[k][0];
      x  = Tmp*Tmp;
      if ( x>=O_Blob->Radius2[k] ) continue;

      Tmp = Slice[1] - O_Blob->G[k][1];
      x += Tmp*Tmp;
      if ( x>=O_Blob->Radius2[k] ) continue;

      Tmp = Slice[2] - O_Blob->G[k][2];
      x += Tmp*Tmp;
      if ( x>=O_Blob->Radius2[k] ) continue;

      x *= O_Blob->Inv_Radius2[k];
      V += Blob_Table[(INT)floor(x)];
   }
#ifdef CALL_STATS
   Call_Count++;
#endif
   Slice_B[Off] = V;
   if ( V>0.0 ) return( FALSE );
   else return( TRUE );
}
#endif   // DO_V_ASM

#ifndef COMPUTE_MC_NORMAL_ASM

EXTERN void Compute_MC_Normal( INT Vtx )
{
   FLT x, y, z;
   INT i;
   FLT *Ns = (FLT*)MC_Blob->Normals[Vtx];
   Ns[0] = 0.0; Ns[1] = 0.0; Ns[2] = 0.0;
   x = MC_Blob->Vertex[Vtx][0];
   y = MC_Blob->Vertex[Vtx][1];
   z = MC_Blob->Vertex[Vtx][2];
   for( i=O_Blob->Nb_Centers-1; i>=0; --i )
   {
      FLT X = x - O_Blob->G[i][0]; 
      FLT Y = y - O_Blob->G[i][1];
      FLT Z = z - O_Blob->G[i][2];
      FLT Tmp  = X*X + Y*Y + Z*Z;
      INT Tmp2 = (INT)floor( Tmp*O_Blob->Inv_Radius2[i] );
      if ( Tmp2<BLOB_TABLE_SIZE )
      {
         Tmp = Blob_Table2[Tmp2];
         Ns[0] += X*Tmp;
         Ns[1] += Y*Tmp;
         Ns[2] += Z*Tmp;
      }
   }
   x  = Ns[0]*Ns[0] + Ns[1]*Ns[1] + Ns[2]*Ns[2];
   if ( x>0.0 )
   {
      x = 1.0/(FLT)sqrt((double)x);
      Ns[0] *= x;
      Ns[1] *= x;
      Ns[2] *= x;
   }
}
#endif   // COMPUTE_MC_NORMAL_ASM

/******************************************************************/
/******************************************************************/

#ifndef DO_V2_ASM

EXTERN INT Do_V2( INT Off )
{
   FLT V;
   FLT x, y, z, r2;
   INT M;
   FLT D;

   V = -O_Blob->K;

   x = Slice[0]; // - O_Blob->G[0][0];
   r2 = x*x; //   if ( r2>=O_Blob->Radius2[0] ) goto End;

   y = Slice[1]; //  - O_Blob->G[0][1];
   r2 += y*y; //   if ( r2>=O_Blob->Radius2[0] ) goto End;

   z = Slice[2]; // - O_Blob->G[0][2];
   r2 += z*z;
   if ( r2>=O_Blob->Radius2[0] ) goto End;
   r2 = 1.0 - r2*O_Blob->Inv_Radius2[0];
   D = y*y + z*z; 
//   fprintf( stderr, "D:%f X:%f 1-r2:%f\n", D, x, r2 );
   for ( M=2; M>=0; --M )
   {
      x *= r2;
      y = x*y + O_Blob->G[0][1];
      z = x*z + O_Blob->G[0][2];
      x = x*x - D + O_Blob->G[0][0];
      D = y*y + z*z;
   }
   V += (D*x*x+.1)*r2;

#ifdef CALL_STATS
   Call_Count++;
#endif
End:

   Slice_B[Off] = V;
   if ( V>0.0 ) return( FALSE );
   else return( TRUE );
}

#endif   // DO_V3_ASM

#ifndef COMPUTE_MC_NORMAL3_ASM

EXTERN void Compute_MC_Normal2( INT Vtx )
{
   return;
}

#endif

/******************************************************************/
/******************************************************************/

#ifndef EMIT_MC_POLYS3_ASM

EXTERN void Emit_MC_Polys3( INT Off, INT Index, INT jo )
{
   BYTE Edge;
   USHORT Vtx_ID=0, Edge_Table[12];
   INT i=0;
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
            MC_Blob->Vertex[Vtx_ID][0] = Slice[0];
            MC_Blob->Vertex[Vtx_ID][2] = Slice[2];
            {
               FLT y;               
               y = Slice_B[Off]/( Slice_B[Off-1] - Slice_B[Off] );
               MC_Blob->Vertex[Vtx_ID][1] = Slice[1] + y*MC_SCALE_Y;
            }
            (*_Compute_MC_Normal_)( Vtx_ID );
         break;

         case 6:
            Vtx_ID = Vtx_6 = MC_Blob->Nb_Vertex;
            MC_Blob->Nb_Vertex++;
            MC_Blob->Vertex[Vtx_ID][1] = Slice[1];
            MC_Blob->Vertex[Vtx_ID][2] = Slice[2];
            {
               FLT x;
               x = Slice_B[Off]/( Slice_B[Off-MCY] - Slice_B[Off] );
               MC_Blob->Vertex[Vtx_ID][0] = Slice[0] + x*MC_SCALE_X;
            }
            (*_Compute_MC_Normal_)( Vtx_ID );
         break;

         case 7: Vtx_ID = Y_Edge[ Off ] = Top_yEdge[ jo ]; break;
         case 8: Vtx_ID = Z_Edge[ jo ]; break;
         case 9: Vtx_ID = Z_Edge[ jo ] = Vtx_10; break;

         case 10:
            Vtx_ID = Vtx_10 = MC_Blob->Nb_Vertex;
            MC_Blob->Nb_Vertex++;

            MC_Blob->Vertex[Vtx_ID][0] = Slice[0];
            MC_Blob->Vertex[Vtx_ID][1] = Slice[1];
            {
               FLT z;
               z = Slice_B[Off]/( Slice_A[Off] - Slice_B[Off] );
               MC_Blob->Vertex[Vtx_ID][2] = Slice[2] + z*MC_SCALE_Z;
            }
            (*_Compute_MC_Normal_)( Vtx_ID );
         break;

         case 11: Vtx_ID = Z_Edge[ jo - 1 ]; break;
      }

      Edge_Table[Edge] = Vtx_ID;
      i++;
   }

   i = 0;
   while( Poly_Order[Index][i]!=0xFF )
   {
      POLY *P = MC_Blob->Polys + MC_Blob->Nb_Polys;
      MC_Blob->Nb_Polys++;
      P->Pt[0] = Edge_Table[ Poly_Order[Index][i++] ];
      P->Pt[1] = Edge_Table[ Poly_Order[Index][i++] ];
      P->Pt[2] = Edge_Table[ Poly_Order[Index][i++] ];
   }
}

#endif   //    EMIT_MC_POLYS3_ASM

/******************************************************************/
/******************************************************************/

#ifndef EMIT_MC_POLYS2_ASM

static void Emit_MC_Polys2( )
{
   INT k;

#if 0
      // Can be avoided, if previous blob didn't reach boundary...
   for( k=MCX*MCY-1; k>=0; --k ) 
   {
      Flags_B[k] = 0x00;
      Slice_B[k] = 0.0;
   }
#endif   // 0 

      // Flags1, Slice_B and Flags_B are ok
      // Begin with slice #1

   MC_Mask = 0x00000001;
   Slice[2] = MC_OFF_Z;

   for( k=MCZ-1; k>0; --k )
   {
      INT Off, i;

      { FLT *Tmp; Tmp = Slice_A; Slice_A = Slice_B; Slice_B = Tmp; }
      { BYTE *Tmp; Tmp = Flags_A; Flags_A = Flags_B; Flags_B = Tmp; }

      MC_Mask <<= 1;
      Slice[2] += MC_SCALE_Z;

#ifdef  PRINT_SLICES
      fprintf( stderr, "Slice k=%d z=%f : \n",k,Slice[2] );
#endif

      Slice[0] = MC_OFF_X;
      Off = MCY + 1;

      for( i=MCX-2; i>0; --i )
      {
         INT j;
         Slice[0] += MC_SCALE_X;
         Slice[1] = MC_OFF_Y;

#ifdef  PRINT_SLICES
         fprintf( stderr, "i=%d : \t", i );
#endif

         for( j=MCY-2; j>0; --j )
         {
            Slice[1] += MC_SCALE_Y;

#ifdef  PRINT_SLICES
            fprintf( stderr, "%c", ( Flags1[ Off ]&MC_Mask ) ? '*' : '.' );
#endif
            if ( Flags1[ Off ]&MC_Mask )
            {
               if ( !(*_Do_V_)( Off ) )      // Slice_B[Off]>0.0
               {
                  Flags_B[Off+1+MCY] |= 0x01;
                  Flags_B[Off+MCY] |= 0x08;            
                  Flags_B[Off+1] |= 0x02;
                  Flags_B[Off] |= 0x04;
                  Flags_A[Off+1+MCY] |= 0x10;
                  Flags_A[Off+MCY] |= 0x80;
                  Flags_A[Off+1] |= 0x20;
                  Flags_A[Off] |= 0x40;
               }
               else Flags1[ Off ] ^= MC_Mask;
            }
#ifdef  PRINT_SLICES
            fprintf( stderr, "%.2x",Flags_A[Off] );
#endif   

            if ( Flags_A[Off]!=0x00 )
            {
               if ( Flags_A[Off]!=0xff )
                  Emit_MC_Polys3( Off, Flags_A[Off], j );
               Flags_A[Off] = 0x00;
            }
            Off++;
         }
         Off += 2;
#ifdef  PRINT_SLICES
         fprintf( stderr, "\n" );
#endif
      }
   }
}

#endif // EMIT_MC_POLYS2_ASM

/******************************************************************/
/******************************************************************/

static void Init_Do_MCube( MC_BLOB *Blob )
{
   O_Blob = Blob;
   MC_Blob = Blob->Blob;
   MC_Blob->Nb_Vertex = 0;
   MC_Blob->Nb_Polys = 0;

   Flags_A = Raw_Flags;
   Flags_B = Flags_A + MCX*MCY;

   Slice_A = Raw_Slice;
   Slice_B = Slice_A + MCX*MCY;
}

#ifdef USE_MCUBE3

EXTERN void Do_MCube3( MC_BLOB *Blob )
{
#ifdef CALL_STATS
   Call_Count = 0;
#endif

   Init_Do_MCube( Blob );

   _Do_V_ = Do_V;
   _Compute_MC_Normal_ = Compute_MC_Normal;

   MC_Grow_Neighbours( );
   Emit_MC_Polys2( );

   MC_Blob->Flags = (OBJ_FLAG)( MC_Blob->Flags & ~OBJ_HAS_NORMALS);
   Mesh_Store_Normals2( MC_Blob );
    
#ifdef CALL_STATS
   fprintf( stderr, "Calls: %d/%d (%f %%)\n", Call_Count, 32768, 100.0/32768.0*Call_Count );
#endif
}

#endif   // DO_MCUBE3

#ifdef USE_MCUBE4

EXTERN void Do_MCube4( MC_BLOB *Blob )
{
   INT k;

   Init_Do_MCube( Blob );

   _Do_V_ = Do_V2;
   _Compute_MC_Normal_ = Compute_MC_Normal2;

   for( k=0; k<MCX*MCY; ++k ) Flags2[k] = 0x3FFFFFFC;
   { void *Tmp; Tmp = Flags1; Flags1 = Flags2; Flags2 = Tmp; }

   // MC_Grow_Neighbours( );
   Emit_MC_Polys2( );

   MC_Blob->Flags &= ~OBJ_HAS_NORMALS;
   Mesh_Store_Normals2( MC_Blob );

   Average_Normals( MC_Blob );      // HACK
}

#endif   // DO_MCUBE4

/******************************************************************/
/******************************************************************/

#ifndef GROW_NEIGHBOURS_ASM

EXTERN void MC_Grow_Neighbours( )
{
   INT k;
   UINT *F1, *F2;

      // Will grow flags1 into flags2
      // WARNING! this WILL bug on boundary...

   F1 = Flags1 + MCY+1;
   F2 = Flags2 + MCY+1;
      // Flip flags table. => Flags1[] is ok.
   { UINT *Tmp; Tmp = Flags1; Flags1 = Flags2; Flags2 = Tmp; }

   for( k=MCX*MCY-MCY-2-MCY-1; k>=0; --k )
   {
      UINT Column, Prev;
      Prev = F1[k];
      if ( Prev==0x00 ) continue;
      Column  = Ngh_Tab[ (Prev&0xFF) ] >> 8;
      Prev >>= 8;
      Column |= Ngh_Tab[ (Prev&0xFF) ];
      Prev >>= 8;
      Column |= Ngh_Tab[ (Prev&0xFF) ]<<8;
      Prev >>= 8;
      Column |= Ngh_Tab[ (Prev&0xFF) ]<<16;

      F2[k-1-MCY] |= Column;
      F2[k  -MCY] |= Column;
      F2[k+1-MCY] |= Column;
      F2[k-1]     |= Column;
      F2[k  ]     |= Column;
      F2[k+1]     |= Column;
      F2[k-1+MCY] |= Column;
      F2[k+MCY]   |= Column;
      F2[k+1+MCY] |= Column;
   }
}

#endif      //  GROW_NEIGHBOURS_ASM

/******************************************************************/
/******************************************************************/

EXTERN void MC_Blobify2( MC_BLOB *Blob, FLT Radius, FLT Strength )
{
   INT i;

   if ( Blob==NULL ) return;
   if ( Blob->Nb_Centers==MAX_CENTERS ) return;

   i = Blob->Nb_Centers++;
   Blob->Radius[i]      = Radius;
   Blob->Radius2[i]     = (Radius*Radius);
   Blob->Inv_Radius2[i] = 1.0f*BLOB_TABLE_SIZE/(Radius*Radius);
   Blob->Strength[i]    = Strength;
   Set_Vector( Blob->G[i], 0.0, 0.0, 0.0 );
}

#if 0
EXTERN void MC_Set_Constants2( MC_BLOB *Blob )
{
   INT i;
   FLT Radius;

   if ( Blob==NULL ) return;

   for ( i=0; i<Blob->Nb_Centers; ++i )
   {
      Radius = Blob->Radius[i];
      Blob->Radius2[i]     = (Radius*Radius);
      Blob->Inv_Radius2[i] = 1.0/(Radius*Radius);
   }
}
#endif

EXTERN MESH *MC_Finish_Blob2( MC_BLOB *Blob, INT Max_Vtx, INT Max_Polys )
{
   INT j;

   Build_MCube_Tab( );
   Build_Default_Blob_Table( );
   Build_Ngh_Table( );

   O_Blob = Blob;
   Blob->Blob = Set_Mesh_Data( Blob->Blob, Max_Vtx, Max_Polys );
   if ( Blob->Blob == NULL ) 
      goto Failed;

   for( j=0; j<Blob->Blob->Nb_Polys; ++j )
   {
      Blob->Blob->Polys[j].Nb_Pts = 3;
   }
   Blob->Blob->Flags = (OBJ_FLAG)( Blob->Blob->Flags | OBJ_DONT_OPTIMIZ);

      // For bounding box to computed right...
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

EXTERN void Build_Ngh_Table( )
{
   INT i;

   for( i=255; i>=0; --i )
   {
      INT j;

      Ngh_Tab[i] = 0x00000000;
      for( j=7; j>=0; --j )
      {
         UINT Bit;
         Bit = 1<<(j+8);
         Bit = Bit | (Bit<<1) | (Bit>>1);    // 3 neighbours along z
         if ( i&(1<<j) ) Ngh_Tab[i] |= Bit;
      }
   }
      // Fill all flags/index the first time
      // All grid points in [1,MCY-2]x[1,MCX-2]x[1,MCZ-2]
      // will be tested the first time Do_MCube()
      // is called...
      // Always start with Flags1 filled

   Flags1 = MC_Last_Flags; 
   Flags2 = Flags1 + MCX*MCY;

   for( i=2*MCX*MCY-1; i>=0; --i ) Flags1[i] = 0x00000000;
   for( i=2; i<MCX-2; ++i )
   {
      INT j;
      for( j=2; j<MCY-2; ++j ) Flags1[i*MCY+j] = 0x3FFFFFFC;
   }

   for( i=2*MCX*MCY-1; i>=0; --i ) 
   {
      Raw_Flags[i] = 0x00;
      Raw_Slice[i] = 0.0;
   }
}

EXTERN void Build_Default_Blob_Table( )
{
   INT i;
   for( i=BLOB_TABLE_SIZE-1; i>=0; --i )
   {
      FLT x;
      x = (FLT)i/BLOB_TABLE_SIZE;
/*
      x = (FLT)sqrt( x );
      if ( x<.5 ) Blob_Table[i] = 1-2*x*x;
      else Blob_Table[i] = 2*(1-x)*(1-x);
*/
      x = (FLT)sqrt( x );
      Blob_Table[i] = cos( x*M_PI )/2.0 + .5;
      if ( i!=0 ) Blob_Table2[i] = sin( x*M_PI )/x;

      // Blob_Table3[i] = (INT)( 256.0*( cos( x*M_PI )/2.0 + .5 ) );
      // if ( i!=0 ) Blob_Table4[i] = sin( x*M_PI )/x;
   }
}

EXTERN void Set_MC_Dimensions( FLT Xo, FLT Yo, FLT Zo, FLT Dx, FLT Dy, FLT Dz )
{
   MC_OFF_X = Xo-Dx;
   MC_OFF_Y = Yo-Dy;
   MC_OFF_Z = Zo-Dz;
   MC_SCALE_X = 2.0*Dx/MCX; I_MC_SCALE_X = 1.0/MC_SCALE_X;
   MC_SCALE_Y = 2.0*Dy/MCY; I_MC_SCALE_Y = 1.0/MC_SCALE_Y;
   MC_SCALE_Z = 2.0*Dz/MCZ; I_MC_SCALE_Z = 1.0/MC_SCALE_Z;
   MC_OFF_END_X = MC_OFF_X + (MCX-1)*MC_SCALE_X;
   MC_OFF_END_Y = MC_OFF_Y + (MCY-1)*MC_SCALE_Y;
   MC_OFF_END_Z = MC_OFF_Z + (MCZ-1)*MC_SCALE_Z;
}

/******************************************************************/
/******************************************************************/

static BYTE MC_Masks[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
static BYTE MC_Order[12] = { 11,1,2, 0,4,6, 3,7,5, 8,9,10 };

static void Compute_Edge_Order( )
{
   INT i;
   for( i=0; i<256; ++i )
   {
      INT Total, k, n;

      Total = 0;
      for( k=0; k<12; ++k )
      {
         for( n=0; n<12; ++n )
         {
            if ( Poly_Order[i][n]==MC_Order[k] )
            {               
               Edge_Order[i][Total++] = MC_Order[k];   
               break;
            }
         }
      }
      while( Total!=16 ) Edge_Order[i][Total++] = 0xFF;
   }
}

#ifdef DUMP_TABLE

static void Dump_MCube_Tab( )
{
   INT i;
   FILE *Out;

   Out = fopen( "toto.h", "w" );
   if ( Out==NULL ) exit(1);

   fprintf( Out, "static BYTE Poly_Order[256][MC_PTS] = {\n" );
   for( i=0; i<256; ++i )
   {
      int k;
      
      fprintf( Out, "  {" );

      for( k=0; k<MC_PTS; ++k )
      {
         fprintf( Out, "%d", Poly_Order[i][k] );
         if (k!=MC_PTS-1) fprintf( Out, "," );
         if ((k%3)==2 && k<12 ) fprintf( Out, " " );
      }

      fprintf( Out, "}" );
      if ( i!=255 ) fprintf( Out, ",\t\t/* %d  */\n", i );
      else fprintf( Out," \t\t/* %d */\n", i );
   }
   fprintf( Out, "};\n" );

   fprintf( Out, "static BYTE Edge_Order[256][16] = {\n" );
   for( i=0; i<256; ++i )
   {
      INT Total, k, n;

      Total = 0;
      fprintf( Out, "  {" );

      for( k=0; k<16; ++k )
      {
         fprintf( Out, "%d", Edge_Order[i][k] );
         if ( k!=15 ) fprintf( Out, "," );
      }

      fprintf( Out, "}" );
      if ( i!=255 ) fprintf( Out, ",\t\t/* %d  */\n", i );
      else fprintf( Out," \t\t/* %d */\n", i );
   }
   fprintf( Out, "};\n" );
   fclose( Out );
}
#endif   // DUMP_TABLE

/******************************************************************/

static INT R1_Tab[8] = { 4,5,1,0, 7,6,2,3 };
static INT R2_Tab[8] = { 1,5,6,2, 0,4,7,3 };
static INT R3_Tab[8] = { 1,2,3,0, 5,6,7,4 };
static INT R1_E_Tab[12] = { 4,9,0,8, 6,10,2,11, 7,5,1,3 };
static INT R2_E_Tab[12] = { 9,5,10,1, 8,7,11,3, 0,4,6,2 };
static INT R3_E_Tab[12] = { 1,2,3,0, 5,6,7,4, 9,10,11,8 };

#define NB_CASES 27
#define CFG_PTS   16
static INT MC_15_Polys[NB_CASES][CFG_PTS+1] = { /* Bits, Polys-edges... 255 */

   { 0x01, 0,3,8, 255 },
   { 0x03, 1,3,8, 1,8,9, 255 },
   { 0x83, 1,3,8, 1,8,9, 6,7,11, 255 },

   { 0x07, 8,9,10, 8,10,3, 2,3,10, 255 },
   { 0x87, 8,9,10, 8,10,3, 2,3,10, 6,7,11, 255 },

   { 0x0f, 8,9,10, 8,10,11, 255 },

   { 0x21, 0,3,8, 4,5,9, 255 },
   { 0xa1, 0,3,8, 4,5,9, 6,7,11, 255 },
   { 0x41, 0,3,8, 5,6,10, 255 },

   { 0x1b, 4,9,1, 4,1,2, 2,7,4, 2,11,7, 255 }, /* hexa */

   { 0x1d, 0,1,4, 1,11,4, 1,10,11, 11,7,4, 255 },
   { 0x17, 2,7,9, 2,3,7, 9,7,4, 2,9,10, 255 },

   { 0x55, 0,1,5, 0,5,4, 2,3,7, 2,7,6, 255 },

   { 0xa5, 0,3,8, 4,5,9, 6,7,11, 2,1,10, 255 },


   { 0xFE, 0,8,3, 255 },
   { 0xFC, 1,8,3, 1,9,8, 255 },
   { 0x7C, 1,8,3, 1,9,8, 6,11,7, 3,8,7, 3,7,11, 255 }, /* XXXX */

   { 0xf8, 8,10,9, 8,3,10, 2,10,3, 255 },
   { 0x78, 8,10,9, 8,3,10, 2,10,3, 6,11,7, 255 },

   { 0xf0, 8,10,9, 8,11,10, 255 },

   { 0xde, 0,8,3, 4,9,5, 4,0,9, 4,8,0, 255 },      /* XXX */
   { 0x5e, 0,8,3, 4,5,5, 6,11,7, 255 },
   { 0xbe, 0,8,3, 5,10,6, 255 },

   /* { 0xe4, 4,1,9, 4,2,1, 2,4,7, 2,7,11, 255 }, */ /* hexa */

   { 0xe2, 0,4,1, 1,4,11, 1,11,10, 11,4,7, 255 },
   { 0xe8, 2,9,7, 2,7,3, 9,4,7, 2,10,9, 255 },

   { 0xaa, 0,5,1, 0,4,5, 2,7,3, 2,6,7, 255 },   /* XXX */

   { 0x5a, 0,8,3, 4,9,5, 6,11,7, 2,10,1, 255 }
};

EXTERN void Build_MCube_Tab( )
{
   INT i, j;

   for( i=0; i<256; ++i )
      for( j=0; j<MC_PTS; ++j )
         Poly_Order[i][j] = 255;

   for( i=0; i<NB_CASES; ++i )
   {
      INT r1, r2, r3, I;

      I = MC_15_Polys[i][0];
      if ( I==0x00 ) continue;
      if ( I==0xFF ) continue;
      for( r1=0; r1<4; ++r1 )
         for( r2=0; r2<4; ++r2 )
            for( r3=0; r3<4; ++r3 )
            {
               INT New_Config, k, r;

               New_Config = 0x00;
               for( j=7; j>=0; --j )
               {
                  if ( !( I&MC_Masks[j]) ) continue;
                  k = j;
                  for( r=0; r<r1; ++r ) k=R1_Tab[k];
                  for( r=0; r<r2; ++r ) k=R2_Tab[k];
                  for( r=0; r<r3; ++r ) k=R3_Tab[k];
                  New_Config |= MC_Masks[k];
               }
               if ( Poly_Order[New_Config][0] != 255 ) continue;

               /* printf( "Config %d: case %d <= [", i, Found ); */
               for( j=0; j<CFG_PTS; ++j )
               {
                  INT k, r;
                  k = MC_15_Polys[i][j+1];
                  if ( k==255 ) break;
                  /* printf( "%d->", k ); */
                  for( r=0; r<r1; ++r ) k=R1_E_Tab[k];
                  for( r=0; r<r2; ++r ) k=R2_E_Tab[k];
                  for( r=0; r<r3; ++r ) k=R3_E_Tab[k];
                  Poly_Order[New_Config][j] = k;
                  /* printf( "%d ", k ); */
               }
               /* printf( "]\n" ); */
            }
   }
   Compute_Edge_Order( );
}


/******************************************************************/
/******************************************************************/
