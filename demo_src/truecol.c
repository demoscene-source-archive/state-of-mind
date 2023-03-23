/***********************************************
 *           True color utilities              *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "ld_any.h"
#include "truecol.h"

#define Bound(x)  { if ((x)<0) (x)=0; else if ((x)>255) (x)=255; }

#if 0
static INT _Bound(INT x) {
   if ( x<0 ) return(0);
   else if ( x>255 ) return(255);
   else return(x);
}
#endif

/*******************************************************/

static void Finish_Format( void *CMap, INT *R, INT *G, INT *B,
   INT Nb_Col, INT Format )
{
   INT i;
   if ( (Format&0xFFFF)==0x2565 )
      for( i=0; i<Nb_Col; ++i )
         ((USHORT*)CMap)[i] = RGB_TO_565(R[i],G[i],B[i]);
   else if ( (Format&0xFFFF)==0x4565 )
      for( i=0; i<Nb_Col; ++i )
         ((UINT*)CMap)[i] = RGB_TO_565(R[i],G[i],B[i]);
   else if ( (Format&0xFFFF)==0x2555 )
      for( i=0; i<Nb_Col; ++i )
         ((USHORT*)CMap)[i] = RGB_TO_555(R[i],G[i],B[i]);
   else if ( (Format&0xFFFF)==0x4555 )
      for( i=0; i<Nb_Col; ++i )
         ((UINT*)CMap)[i] = RGB_TO_555(R[i],G[i],B[i]);
   else if ( (Format&0xFFF)==0x777 )
      for( i=0; i<Nb_Col; ++i )
         ((UINT*)CMap)[i] = RGB_TO_777(R[i],G[i],B[i]);
   else if ( (Format&0xFFFF)==0x3888 )
      for( i=0; i<Nb_Col; ++i )
      {
         ((BYTE*)CMap)[3*i+0] = (BYTE)R[i];
         ((BYTE*)CMap)[3*i+1] = (BYTE)G[i];
         ((BYTE*)CMap)[3*i+2] = (BYTE)B[i];
      }    
   else if ( (Format&0xFFFF)==0x4888 )
      for( i=0; i<Nb_Col; ++i )  // leave upper byte untouched...
      {
         UINT C = ((UINT*)CMap)[i];
         C = (C&0xFF000000) | RGB_TO_888(R[i],G[i],B[i]);
         ((UINT*)CMap)[i] = C;
      }
}

EXTERN void CMap_To_16bits( void *CMap, PIXEL *Pal, INT Nb_Col, INT Format )
{
   INT i, j;
   INT R[256], G[256], B[256];

   if ( Nb_Col>256 ) Nb_Col = 256;
   
   for( i=0, j=0; i<Nb_Col; ++i )
   {
      R[i] = Pal[j++]; G[i] = Pal[j++]; B[i] = Pal[j++];
   }
   Finish_Format( CMap, R, G, B, Nb_Col, Format );
}

EXTERN void CMap_To_16bits_With_Mix( void *CMap, 
   PIXEL *Pal, INT Nb_Col, INT Format, 
   INT Mix, INT Ro, INT Go, INT Bo )
{
   INT i;
   INT R[256],G[256],B[256];

   Mix = Mix^0xFF;
   Ro = ((Ro*Mix)&0xFF00); //^0xFF00; 
   Go = ((Go*Mix)&0xFF00); //^0xFF00;
   Bo = ((Bo*Mix)&0xFF00); //^0xFF00;
   Mix = Mix^0xFF;

   if ( Nb_Col>256 ) Nb_Col=256;
   for( i=0; i<Nb_Col; ++i )
   {
      R[i] = ( Mix*(*Pal++) + Ro )>>8; // Bound(R[i]);
      G[i] = ( Mix*(*Pal++) + Go )>>8; // Bound(G[i]);
      B[i] = ( Mix*(*Pal++) + Bo )>>8; // Bound(B[i]);
   }
   Finish_Format( CMap, R, G, B, Nb_Col, Format );
}

EXTERN void Drv_Build_Ramp_16( void *CMap, 
   INT Start, INT n,
	INT r1, INT g1, INT b1, INT r2, INT g2, INT b2,
	INT Format )
{
   INT i, N;
   static INT Next_Start = 0;
   static INT lR = 0, lG = 0, lB = 0;
   INT R[256],G[256],B[256];

   if ( CMap == NULL ) return;
   if ( Start < 0 ) Start = Next_Start;
   if ( Start+n>256 ) n = 256-Start;
   Next_Start = Start + n;

   if ( r1==-1 ) { r1 = lR; g1 = lG; b1 = lB; }
   else { r1<<=8; g1<<=8; b1<<=8; }
   lR = (r2<<=8); lG = (g2<<=8); lB = (b2<<=8);


   if ( n==1 ) N = 1; else N = n-1;
   r2 = (r2-r1)/N;
   g2 = (g2-g1)/N;
   b2 = (b2-b1)/N;
   for( i=0; i<n; ++i )
   {
      R[i] = r1>>8; Bound(R[i]); r1 += r2;
      G[i] = g1>>8; Bound(G[i]); g1 += g2;
      B[i] = b1>>8; Bound(B[i]); b1 += b2;
   }
   if ( Start )
   {
      if ( ((Format&0xFFFF)==0x2565) || ( (Format&0xFFFF)==0x2555 ) )
         CMap = (void*)((USHORT*)CMap + Start);
      else if ( ((Format&0xFFFF)==0x3888) || ( (Format&0xFFFF)==0x3777 ) )
         CMap = (void*)((BYTE*)CMap + 3*Start);
      else if ( ((Format&0xFFF)==0x332) )
         CMap = (void*)((BYTE*)CMap + Start);
      else CMap = (void*)((UINT*)CMap + Start);
   }
   Finish_Format( CMap, R, G, B, n, Format );
}

/*******************************************************/

   // Type -> 0: fade between Mixing_Col and Col
   //      -> 1: fade between Mixing_Col and T[Col]
   //      -> 2: fade between T[Col] and Col
   //      -> 3: fade between Col and T[Col]

EXTERN void CMap_To_16bits_With_Fx(
   void *CMap, PIXEL *Pal, INT Nb_Col, INT Format,
   INT Mixing_Col, INT T[3][3], INT Level, INT Type  )
{
   INT i;
   INT Ro, Bo, Go;
   INT R[256],G[256],B[256];

   if ( Type==0 || Type==1 )
   {
      Ro = ((Mixing_Col>>16)&0xFF ) * ( 255-Level );
      Go = ((Mixing_Col>> 8)&0xFF ) * ( 255-Level );
      Bo = ((Mixing_Col    )&0xFF ) * ( 255-Level );
   }

   if ( Nb_Col>256 ) Nb_Col = 256;
   if ( Type==0 )
      for( i=0; i<Nb_Col; ++i ) // Fade to Mixing_Col. level is in [0,255]
      {
         R[i] = ( (*Pal++)*Level + Ro ) >> 8; Bound(R[i]);
         G[i] = ( (*Pal++)*Level + Go ) >> 8; Bound(G[i]);
         B[i] = ( (*Pal++)*Level + Bo ) >> 8; Bound(B[i]);
      }
   else if ( Type==1 )
      for( i=0; i<Nb_Col; ++i ) // fade between Mixing_col and T[Col]
      {
         INT r,g,b;
         r = *Pal++; g = *Pal++; b = *Pal++;
         R[i] = ( T[0][0]*r + T[0][1]*g + T[0][2]*b ) >> 8;
         R[i] = ( R[i]*Level + Ro )>>8;
         Bound(R[i]);
         G[i] = ( T[1][0]*r + T[1][1]*g + T[1][2]*b ) >> 8;
         G[i] = ( G[i]*Level + Go )>>8;
         Bound(G[i]);
         B[i] = ( T[2][0]*r + T[2][1]*g + T[2][2]*b ) >> 8;
         B[i] = ( B[i]*Level + Bo )>>8;
         Bound(B[i]);
      }
   else if ( Type==2 )     // fade between T[Col] and Col
      for( i=0; i<Nb_Col; ++i )
      {
         INT r,g,b;
         r = *Pal++; g = *Pal++; b = *Pal++;
         R[i] = ( T[0][0]*r + T[0][1]*g + T[0][2]*b )>>8;
         R[i] = ( (R[i]-r)*Level + (r<<8) ) >> 8;
         Bound(R[i]);
         G[i] = ( T[1][0]*r + T[1][1]*g + T[1][2]*b )>>8;
         G[i] = ( (G[i]-g)*Level + (g<<8) ) >> 8;
         Bound(G[i]);
         B[i] = ( T[2][0]*r + T[2][1]*g + T[2][2]*b )>>8;
         B[i] = ( (B[i]-b)*Level + (b<<8) ) >> 8;
         Bound(B[i]);
      }
   else if ( Type==3 )     // fade between T[Col] and Col
      for( i=0; i<Nb_Col; ++i )
      {
         INT r,g,b;
         r = *Pal++; g = *Pal++; b = *Pal++;
         R[i] = ( T[0][0]*r + T[0][1]*g + T[0][2]*b )>>8;
         R[i] = ( (r-R[i])*Level + 256*R[i] ) >> 8;
         Bound(R[i]);
         G[i] = ( T[1][0]*r + T[1][1]*g + T[1][2]*b )>>8;
         G[i] = ( (g-G[i])*Level + 256*G[i] ) >> 8;
         Bound(G[i]);
         B[i] = ( T[2][0]*r + T[2][1]*g + T[2][2]*b )>>8;
         B[i] = ( (b-B[i])*Level + 256*B[i] ) >> 8;
         Bound(B[i]);
      }
   Finish_Format( CMap, R, G, B, Nb_Col, Format );
}

/*******************************************************/

#ifndef SKL_LIGHT_CODE

EXTERN void Prepare_Remap_RGB565( USHORT *Map1, USHORT *Map2, BYTE Mtx[3][3] )
{
   INT i;
   for( i=0; i<256; ++i )
   {
      INT R, G, B, Ro, Go, Bo;
      Bo = (i&0x1F)<<3; Go = (i&0xE0)>>5; Ro = 0;
      R = ( (Mtx[0][0]*Ro)+(Mtx[0][1]*Go)+(Mtx[0][2]*Bo) ) >> 8; Bound(R);
      G = ( (Mtx[1][0]*Ro)+(Mtx[1][1]*Go)+(Mtx[1][2]*Bo) ) >> 8; Bound(G);
      B = ( (Mtx[2][0]*Ro)+(Mtx[2][1]*Go)+(Mtx[2][2]*Bo) ) >> 8; Bound(B);
      Map1[i] = RGB_TO_565( R, G, B );

      Bo = 0; Go = (i&0x07)<<5; Ro = (i&0xF8);
      R = ( (Mtx[0][0]*Ro)+(Mtx[0][1]*Go)+(Mtx[0][2]*Bo) ) >> 8; Bound(R);
      G = ( (Mtx[1][0]*Ro)+(Mtx[1][1]*Go)+(Mtx[1][2]*Bo) ) >> 8; Bound(G);
      B = ( (Mtx[2][0]*Ro)+(Mtx[2][1]*Go)+(Mtx[2][2]*Bo) ) >> 8; Bound(B);
      Map2[i] = RGB_TO_565( R, G, B );
   }
}
#endif // SKL_LIGHT_CODE

/*******************************************************/

#ifndef SKL_LIGHT_CODE

EXTERN BITMAP_16 *Load_16b( char *Name )
{
   BYTE *Src;
   USHORT *Dst;
   BITMAP *Btm;
   BITMAP_16 *New;
   INT i, j;
   USHORT CMap[256];

   Btm = Load_GIF( Name );
   if ( Btm == NULL ) return( NULL );

   New = New_Fatal_Object( 1, BITMAP_16 );
   if ( New==NULL ) goto Failed;
//   Mem_Clear( New );

   New->Bits = (void*)New_Fatal_Object( Btm->Width*Btm->Height, USHORT );
   if ( New->Bits==NULL )
   {
      M_Free( New );
      goto Failed;
   }
   New->Width = Btm->Width;
   New->Height = Btm->Height;
   New->Size = New->Width * New->Height * sizeof( USHORT );
   New->Format = 0x4565;

   CMap_To_16bits( CMap, Btm->Pal, Btm->Nb_Col, New->Format );

   Src = Btm->Bits;
   Dst = (USHORT*)New->Bits;
   for( j=0; j<Btm->Height; ++j )
   {
      Src += Btm->Width;
      Dst += Btm->Width;
      for( i=-Btm->Width; i<0; ++i ) Dst[i] = CMap[ Src[i] ];
   }
Failed:
   Destroy_Bitmap( Btm );
   return( New );
}

#endif   // SKL_LIGHT_CODE

/*******************************************************/

EXTERN void Destroy_16b( BITMAP_16 *Btm )
{
   if ( Btm==NULL ) return;
   M_Free( Btm->Bits );
   M_Free( Btm->Pal );
   Btm->Nb_Col = 0;
   M_Free( Btm );
}

/*******************************************************/
