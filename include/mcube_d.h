/*
 * Table builder. Hack.
 * 
 ****************************************************/

static PIXEL Poly_Order[256][16];
static PIXEL Edge_Order[256][16];
static INT Polys[8][12];

static INT Polys_Nb_Points[8];
static INT Nb_Polys;

static PIXEL Masks[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
static INT Neigh_Edges[8][3] = 
{
   { 3,8,0 },     /* vertex #0 */
   { 0,9,1 },     /* vertex #1 */
   { 1,10,2 },    /* vertex #2 */
   { 2,11,3 },    /* vertex #3 */
   { 8,7,4 },     /* vertex #4 */
   { 9,4,5 },     /* vertex #5 */
   { 10,5,6 },    /* vertex #6 */
   { 11,6,7 }     /* vertex #7 */
};

/******************************************************************/

static INT Cleanup_Poly( INT Nb_Pts, INT *Pts )
{
   INT j;

Restart:
   for( j=0; j<Nb_Pts-1; ++j )
   {
      INT n;
      for( n=j+1; n<Nb_Pts; ++n )
      {
         INT k, k2, Remove;
         Remove = Pts[n];
         if ( Remove != Pts[j] ) continue;
         for( k=0, k2=0; k2<Nb_Pts; ++k2 )
            if ( Pts[k2]!=Remove )
               Pts[k++] = Pts[k2];
         Nb_Pts = k;
         goto Restart;
      }
   }
   return( Nb_Pts );
}

extern void MC_Merge_Poly( INT Nb_New_Points, INT *Edges )
{
   INT i;

Restart:   

      /* Search a poly with common vertice(s) to insert in.
         Insert. and restart the search. */

   for( i=0; i<Nb_Polys; ++i )
   {
      INT j;
      for( j=0; j<Polys_Nb_Points[i]; ++j )
      {
         INT k;
         for( k=0; k<Nb_New_Points; ++k )
         {
            INT n, m;
            INT Scratch[24];

            if ( Polys[i][j]!=Edges[k] ) continue;
            
            /* Insert edges in poly #i, offset j */

               /* Copy beginning of original poly */ 
            for( n=0; n<j; n++ ) Scratch[n] = Polys[i][n];

               /* Insert new vertex */
            m=k;
            do
            {
               m++;
               if ( m>=Nb_New_Points ) m=0;
               Scratch[n++] = Edges[m];
            } while( m!=k );
            n--;

               /* Finish with tail */
            for( m=j+1; m<Polys_Nb_Points[i]; ++m )
               Scratch[n++] = Polys[i][m];

            if ( i!=Nb_Polys-1 )
            {
               for( k=0; k<Polys_Nb_Points[Nb_Polys-1];++k )
                  Polys[i][k] = Polys[Nb_Polys-1][k];
               Polys_Nb_Points[i] = k;
            }            
            n = Cleanup_Poly( n, Scratch );
#if 0
            if ( n>6 )  /* Split in two */
            {
               INT Scratch2[16];
               m = j;
               for( k=0; k<Nb_New_Points; ++k ) /* Small part */
               {
                  m++;
                  if (m>=n) m=0;
                  Polys[Nb_Polys-1][k] = Scratch[m];
               }
               Polys_Nb_Points[Nb_Polys-1] = Cleanup_Poly( k, Polys[Nb_Polys-1] );
               Nb_Polys++;

               for( k=0; k<n-Nb_New_Points; ++k )
               {
                  m++; if ( m>n ) m=0;
                  Scratch2[k] = Scratch[m];
               }
               n -= Nb_New_Points;
               for( k=0; k<n; ++k ) Scratch[k] = Scratch2[k];
               n = Cleanup_Poly( n, Scratch );
            }
#endif

               /* Re-install new poly in tmp slot #Nb_Polys */
            Edges = Polys[Nb_Polys-1];
            for( k=0; k<n;++k ) Edges[k] = Scratch[k];
            Nb_New_Points = Polys_Nb_Points[Nb_Polys-1] = 
               Cleanup_Poly( n, Edges );
            Nb_Polys--;
            goto Restart;
         }
      }
   }
      /* => Add as new poly */
   if ( Edges != Polys[Nb_Polys] )
   {
      for( i=0; i<Nb_New_Points; ++i )
         Polys[Nb_Polys][i] = Edges[i];
      Polys_Nb_Points[Nb_Polys] = Cleanup_Poly( Nb_New_Points, Polys[Nb_Polys] );
   }
   else Polys_Nb_Points[Nb_Polys] = Nb_New_Points;
   Nb_Polys++;
}

extern void Init_MCube_Tab( )
{
   INT i;

   for( i=0; i<256; ++i )
   {
      INT n, k, Order;

      for( k=0; k<16; ++k ) Poly_Order[i][k] = 0xFF;
      for( k=0; k<sizeof(Polys); ++k ) ( (PIXEL *)Polys )[k] = 0xFF;

      Nb_Polys = 0;

      printf( "i=%d (", i );
      for( k=0; k<8; ++k )
      {
         if ( i&Masks[k] )
         {
            printf( "*" );
            MC_Merge_Poly( 3, Neigh_Edges[k] );
         }
         else printf( "." );
      }
      printf( ") => " );

      Order = 0;
      for( k=0; k<Nb_Polys;++k )
      {
         INT N;
         printf( "[" );
         for( n=0; n<Polys_Nb_Points[k]; ++n ) printf( "%d ", Polys[k][n] );
         printf( "]" );

         N = Polys_Nb_Points[k];
         switch( N )
         {
            case 3:
               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][1];
               Poly_Order[i][Order++] = Polys[k][2];
            break;
            case 4:
               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][1];
               Poly_Order[i][Order++] = Polys[k][2];

               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][2];
               Poly_Order[i][Order++] = Polys[k][3];
            break;
            case 5:
               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][1];
               Poly_Order[i][Order++] = Polys[k][2];

               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][2];
               Poly_Order[i][Order++] = Polys[k][3];

               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][3];
               Poly_Order[i][Order++] = Polys[k][4];
            break;
            case 6:
               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][1];
               Poly_Order[i][Order++] = Polys[k][2];

               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][2];
               Poly_Order[i][Order++] = Polys[k][3];

               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][3];
               Poly_Order[i][Order++] = Polys[k][5];

               Poly_Order[i][Order++] = Polys[k][5];
               Poly_Order[i][Order++] = Polys[k][3];
               Poly_Order[i][Order++] = Polys[k][4];
            break;

            case 7:
               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][1];
               Poly_Order[i][Order++] = Polys[k][2];

               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][2];
               Poly_Order[i][Order++] = Polys[k][3];

               Poly_Order[i][Order++] = Polys[k][0];
               Poly_Order[i][Order++] = Polys[k][3];
               Poly_Order[i][Order++] = Polys[k][6];

               Poly_Order[i][Order++] = Polys[k][6];
               Poly_Order[i][Order++] = Polys[k][3];
               Poly_Order[i][Order++] = Polys[k][4];

               Poly_Order[i][Order++] = Polys[k][6];
               Poly_Order[i][Order++] = Polys[k][4];
               Poly_Order[i][Order++] = Polys[k][5];

               printf( " NbPOints: %d!!  ", N );
            break;

            case 0: case 1: case 2: /* Shouldn't happen */
            default:
               printf( " NbPOints: %d!!  ", N );
            break;
         }
      }
      printf( "\n" );
   }
}

/******************************************************************/

