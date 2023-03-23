/*****************************************************
 *          Colors
 *****************************************************/

#include "main.h"
#include "drv_col.h"

/********************************************************************/

EXTERN void Drv_RGB_To_CMap_Entry( COLOR_ENTRY *CMap, PIXEL *Ptr, INT Nb )
{
   INT i;
   for( i=0; i<Nb; ++i )
   {
      CMap[i][RED_F] = *Ptr++;  
      CMap[i][GREEN_F] = *Ptr++;
      CMap[i][BLUE_F] = *Ptr++;
      CMap[i][INDEX_F] = (PIXEL)i;
   }
}

EXTERN void Drv_Transfert_CMap( COLOR_ENTRY *CMap, INT Start, PIXEL *Ptr, INT Nb )
{
   Nb += Start;
   if ( Nb>=256 ) Nb = 256;
   for( ; Start<Nb; ++Start )
   {
      CMap[Start][RED_F] =   *Ptr++;  
      CMap[Start][GREEN_F] = *Ptr++;
      CMap[Start][BLUE_F] =  *Ptr++;
      CMap[Start][INDEX_F] = (PIXEL)Start;
   }
}

EXTERN void Drv_Fill_Pal( COLOR_ENTRY *Pal, INT I, PIXEL r, PIXEL g, PIXEL b )
{
   while( I-- ) 
   {
      Pal[0][RED_F] = r; Pal[0][GREEN_F] = g;  Pal[0][BLUE_F] = g; 
      Pal++;
   }
}

EXTERN INLINE void Drv_Black_Pal( COLOR_ENTRY *Pal, INT I )
{
   INT i;

   if ( Pal == NULL ) return;
   if ( I<=0 ) I=256;
   for( i=0; i<I; ++i )
   {
      Pal[0][RED_F] = Pal[0][GREEN_F] = Pal[0][BLUE_F] = 0; 
      Pal[0][INDEX_F] = (PIXEL)i;
      Pal++;
   }
}

EXTERN INLINE void Drv_Copy_Pal( COLOR_ENTRY *Src, COLOR_ENTRY *Dst, INT Nb )
{
   if ( Nb ) memcpy( Dst, Src, Nb*sizeof( COLOR_ENTRY ) );
}
 
EXTERN COLOR_ENTRY *Drv_New_Colormap( INT Size )
{
   COLOR_ENTRY *Pal;

   if ( !Size ) return( NULL );
   Pal = New_Object( Size, COLOR_ENTRY );
   if ( Pal == NULL ) return( NULL );  // Exit_Upon_Mem_Error( "col. palette", Size*sizeof(COLOR_ENTRY) );
   return( Pal );
}


/********************************************************************/
/********************************************************************/

EXTERN void Drv_Build_Ramp( COLOR_ENTRY *Pal, 
   INT Start, INT n,
	INT r1, INT g1, INT b1, INT r2, INT g2, INT b2 )
{
   INT i;
   static INT Next_Start = 0;

   if ( Pal == NULL ) return;
   if ( Start < 0 ) Start = Next_Start;

   if ( Start+n>256 ) n = 256-Start;
   if ( n<=1 ) n=1;
   Next_Start = Start + n;
   Pal += Start;
   if ( n==1 ) { 
      Pal[0][RED_F]   = (PIXEL)r2;
      Pal[0][GREEN_F] = (PIXEL)g2;
      Pal[0][BLUE_F]  = (PIXEL)b2;
      Pal[0][INDEX_F] = (PIXEL)Start;
      return;
   }
   r1 <<= 8; r2 = ((r2<<8)-r1)/(n-1);
   b1 <<= 8; b2 = ((b2<<8)-b1)/(n-1);
   g1 <<= 8; g2 = ((g2<<8)-g1)/(n-1);
   
   for( i=0; i<n; ++i )
   {
      Pal[i][RED_F]   = (PIXEL)(r1>>8); r1 += r2;
      Pal[i][GREEN_F] = (PIXEL)(g1>>8); g1 += g2;
      Pal[i][BLUE_F]  = (PIXEL)(b1>>8); b1 += b2;      
      Pal[i][INDEX_F] = (PIXEL)(Start+i);
   }
}

/********************************************************************/

EXTERN void Drv_Build_RGB_Cube( COLOR_ENTRY *Pal, FORMAT Format )
{
   int i, r, g, b, r_max, g_max, b_max;

   if ( Pal == NULL ) return;

   i = Format_Red_Bits(Format); r_max = (1<<i)-1;
   i = Format_Green_Bits(Format); g_max = (1<<i)-1;
   i = Format_Blue_Bits(Format); b_max = (1<<i)-1;

   i = 0;
   for( r=0; r<=r_max; ++r )
      for( g=0; g<=g_max; ++g )
         for( b=0; b<=b_max; ++b )
         {
            Pal[i][RED_F]   = r*255/r_max;
            Pal[i][GREEN_F] = g*255/g_max;
            Pal[i][BLUE_F]  = b*255/b_max;
            Pal[i][INDEX_F] = i;
            i++;
         }
}

/********************************************************************/
/********************************************************************/

EXTERN INT Skl_Best_Match_RGB( PIXEL R, PIXEL G, PIXEL B, COLOR_ENTRY *C, INT Nb )
{
   INT Dist, Best, n;

   Dist = 3*256*256;
   Best = -1;
   for( n=0; n<Nb; ++n )
   {
      int x, D;
      x = (int)(R) - (int)C[n][RED_F]; D = x*x;
      if ( D>128*128 ) continue; 
      x = (int)(G) - (int)C[n][GREEN_F]; D += x*x;
      if ( D>2*128*128 ) continue;
      x = (int)(B) - (int)C[n][BLUE_F]; D += x*x;
      if ( D<Dist ) { Dist = D; Best = n; if (D==0) break; }
   }
   /*
      fprintf( stderr, "(%d,%d,%d)=>#%d (%d,%d,%d)\n",
         R,G,B,Best,C[Best][RED_F],C[Best][GREEN_F],C[Best][BLUE_F] );
   */
   if ( Best == -1 ) { /* fprintf( stderr, "*" ); */ Best = 0; }
   return( Best );
}

/********************************************************************/

EXTERN void Skl_Store_Formatted_CMap(
   UINT *Dst, COLOR_ENTRY *CMap, INT Nb, FORMAT Format )
{
   UINT Out_Mask[4], Out_Shift[4];
   INT i;

   Format_Mask_And_Shift( Format, Out_Mask, Out_Shift );

   for( i=0; i<Nb; ++i )
   {
      UINT Col;
      Col  = ( CMap[i][RED_F] & Out_Mask[RED_F] )<<Out_Shift[RED_F];
      Col |= ( CMap[i][GREEN_F] & Out_Mask[GREEN_F] )<<Out_Shift[GREEN_F];
      Col |= ( CMap[i][BLUE_F] & Out_Mask[BLUE_F] )<<Out_Shift[BLUE_F];
      Dst[ CMap[i][INDEX_F] ] = Col>>COL_BITS_FIX;
   }
}

EXTERN void Skl_Store_CMap( COLOR_ENTRY *Dst, COLOR_ENTRY *Src, INT Nb )
{
   for( Nb--; Nb>=0; Nb-- )
   {
      INT j;
      j = Src[Nb][INDEX_F];
      Dst[j][RED_F] = Src[Nb][RED_F];
      Dst[j][GREEN_F] = Src[Nb][GREEN_F];
      Dst[j][BLUE_F] = Src[Nb][BLUE_F];
      Dst[j][INDEX_F] = (PIXEL)j;
   }
}

EXTERN void Skl_Match_CMaps( COLOR_ENTRY *Src, COLOR_ENTRY *Dst, INT Nb )
{
   for( Nb--; Nb>=0; Nb-- )
   {
      PIXEL R, G, B;
      INT j;
      R = Src[Nb][RED_F];
      G = Src[Nb][GREEN_F];
      B = Src[Nb][BLUE_F];
      j = (PIXEL)Skl_Best_Match_RGB( R, G, B, Dst, 256 );
      Src[Nb][INDEX_F] = (PIXEL)j;
   }
}

/*******************************************************************/
/*******************************************************************/
