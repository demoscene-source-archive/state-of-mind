/***********************************************
 *    Marching cubes (for blobs)               *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

/* #define DO_UV */

/******************************************************************/

// #ifndef EMIT_MC_POLYS_ASM
#if 0

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
         case 4: Vtx_ID = X_Edge[ Off ] = Vtx_6;  break;

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
               /* y = .5; */
               y += (FLT)jo;
               MC_Blob->Vertex[Vtx_ID][1] = MC_OFF_Y + y*MC_SCALE_Y;
#ifdef DO_UV
               MC_Blob->UV[Vtx_ID][0] = y;
               MC_Blob->UV[Vtx_ID][1] = (FLT)io/MCZ;
#endif
            }
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
               /* x = .5; */
               x += (FLT)io;
               MC_Blob->Vertex[Vtx_ID][0] = MC_OFF_X + x*MC_SCALE_X;
#ifdef DO_UV
               MC_Blob->UV[Vtx_ID][0] = x;
               MC_Blob->UV[Vtx_ID][1] = (FLT)io/MCZ;
#endif
            }
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
               /* z = -.5; */
               MC_Blob->Vertex[Vtx_ID][2] = Slice[2] + z*MC_SCALE_Z;

#ifdef DO_UV
               MC_Blob->UV[Vtx_ID][0] = -z;
               MC_Blob->UV[Vtx_ID][1] = (FLT)io/MCZ;
#endif
            }
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

#endif   // 0

/******************************************************************/

#ifndef FILL_SLICE3_ASM

EXTERN void Fill_Slice3( FLT *Ptr, PIXEL *Flags, MC_BLOB *Blob )
{
   INT n;
   for( n=Blob->Nb_Centers-1; n>=0; --n )
   {
      INT io, i2, jo, j2;
      INT i, j;
      // FLT Strength = Blob->Strength[n] * Blob->Blend1;
      FLT L = Blob->Radius[n];
      FLT ro = Slice[2] - Blob->G[n][2];
      if ( ro>L || ro<-L ) continue;

      io = (INT)( (Blob->G[n][0]-L-MC_OFF_X)/MC_SCALE_X );
      i2 = (INT)( (Blob->G[n][0]+L-MC_OFF_X)/MC_SCALE_X );
      jo = (INT)( (Blob->G[n][1]-L-MC_OFF_Y)/MC_SCALE_Y );
      j2 = (INT)( (Blob->G[n][1]+L-MC_OFF_Y)/MC_SCALE_Y );

      ro = ro*ro;
      Slice[0] = MC_SCALE_X*io + MC_OFF_X;
      for( i=io; i<=i2; ++i )
      {
         INT Off;
         FLT Tmp = Slice[0] - Blob->G[n][0];
         FLT r2 = ro + Tmp*Tmp;
         if ( r2>Blob->Radius2[n] ) goto Skip;

         Off = i*MCY+jo;
         Slice[1] = MC_SCALE_Y*jo + MC_OFF_Y;
         for( j=jo; j<=j2; ++j )
         {
            FLT Tmp, r3;
            Tmp = Slice[1] - Blob->G[n][1];
            r3 = r2 + Tmp*Tmp;
            if ( r3<Blob->Radius2[n] )
            {
               INT Tmp;
               Tmp = (INT)( r3*Blob->Inv_Radius2[n] );
               Ptr[Off] += Blob_Table[Tmp];  // *Strength;
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
#endif   // FILL_SLICE3_ASM

/******************************************************************/

#ifndef EMIT_ALL_MC_ASM

EXTERN void Emit_All_MC( MC_BLOB *Blob )
{
   INT k; 
   for( k=0; k<MCZ-1; ++k )
   {
      INT Off, i;

      Slice[2] += MC_SCALE_Z;
      for( i = MCX*MCY-1; i>=0; --i ) Slice_B[i] = -Blob->K;
      Fill_Slice3( Slice_B, Flags_B, Blob );
#ifdef MC_SAFE_BOUNDS
      Fill_Z_yTop_Edge( );
#endif

      Off = 0;
      for( i=0; i<MCX-1; ++i )
      {
         INT j;
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
      { FLT *Tmp; Tmp = Slice_A; Slice_A = Slice_B; Slice_B = Tmp; }
      { PIXEL *Tmp; Tmp = Flags_A; Flags_A = Flags_B; Flags_B = Tmp; }
   }
}

#endif   // EMIT_ALL_MC_ASM

/******************************************************************/

#ifdef UNIX
EXTERN void Clear_Slice_A( FLT K )
{
   INT i;
   for( i = MCX*MCY-1; i>=0; --i ) Slice_A[i] = K;
}
#endif   // UNIX

/******************************************************************/

