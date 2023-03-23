/***********************************************
 *              BITMAP utilities               *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "btm.h"
#include "drv_col.h"

extern INT Skl_Best_Match_RGB( PIXEL R, PIXEL G, PIXEL B, COLOR_ENTRY *C, INT n );

/*******************************************************/

EXTERN void Remap_BTM( BITMAP *Btm, PIXEL *Pal, INT Nb )
{
   INT i;
   COLOR_ENTRY CMap[256];

   Drv_RGB_To_CMap_Entry( CMap, Pal, Nb );
   for( i=0; i<Btm->Width*Btm->Height; ++i )
   {
      PIXEL R, G, B;
      INT C;
      C = 3*Btm->Bits[ i ];
      R = Btm->Pal[ C ];
      G = Btm->Pal[ C+1 ];
      B = Btm->Pal[ C+2 ];
      C = Skl_Best_Match_RGB( R, G, B, CMap, Nb );
      Btm->Bits[i] = (PIXEL)C;
   }
   M_Free( Btm->Pal );
   Btm->Pal = New_Fatal_Object( Nb*3, PIXEL );
   memcpy( Btm->Pal, Pal, Nb*3 );
   Btm->Nb_Col = Nb;
}

EXTERN void Remap_BTM_II( BITMAP *Btm, PIXEL *Pal, INT Nb, INT Transp )
{
   INT i;
   COLOR_ENTRY CMap[256];

   Drv_RGB_To_CMap_Entry( CMap, Pal, Nb );
   for( i=0; i<Btm->Width*Btm->Height; ++i )
   {
      PIXEL R, G, B;
      INT C;
      C = Btm->Bits[ i ];
      if ( C==Transp ) { Btm->Bits[i] = (PIXEL)C; continue; }
      C = 3*C;
      R = Btm->Pal[ C ];
      G = Btm->Pal[ C+1 ];
      B = Btm->Pal[ C+2 ];
      C = Skl_Best_Match_RGB( R, G, B, CMap, Nb );
      Btm->Bits[i] = (PIXEL)C;
   }
   M_Free( Btm->Pal );
   Btm->Pal = New_Fatal_Object( Nb*3, PIXEL );
   memcpy( Btm->Pal, Pal, Nb*3 );
   Btm->Nb_Col = Nb;
}

EXTERN void Offset_BTM( BITMAP *Btm, PIXEL Offset )
{
   INT i;

   for( i=0; i<Btm->Width*Btm->Height; ++i )
      Btm->Bits[i] += Offset;
}

EXTERN void Offset_BTM_2( BITMAP *Btm, PIXEL Offset, PIXEL Zero )
{
   INT i;

   for( i=0; i<Btm->Width*Btm->Height; ++i )
      if ( Btm->Bits[i]!=Zero )
         Btm->Bits[i] += Offset;
}

EXTERN void Rescale_BTM_2( BITMAP *Btm, PIXEL Min, PIXEL Max, PIXEL Zero )
{
   INT i;
   FLT A;

   A = 1.0;   
   for( i=0; i<Btm->Width*Btm->Height; ++i )
      if ( (FLT)Btm->Bits[i]>A )
         A = (FLT)Btm->Bits[i];
   A = (1.0f*Max-1.0f*Min)/A;
   for( i=0; i<Btm->Width*Btm->Height; ++i )
   {
      INT S;
      S = Btm->Bits[i];
      if ( S!=Zero )
         Btm->Bits[i] = Min + (PIXEL)( A*S );
   }
}

EXTERN void Rescale_BTM( BITMAP *Btm, PIXEL Min, PIXEL Max )
{
   INT i;
   FLT A;
   A = (1.0f*Max-1.0f*Min)/256.0f;
   for( i=0; i<Btm->Width*Btm->Height; ++i )
   {
      INT S;
      S = Btm->Bits[i];
      Btm->Bits[i]  = Min + (PIXEL)( A*S );
   }
}

/*******************************************************/

static PIXEL *Pal;
static int Lum_Comp( const void *I1, const void *I2 )
{
   FLT L1, L2;
   PIXEL i1, i2;
   i1 = *(PIXEL*)I1;
   i2 = *(PIXEL*)I2;
   L1 = 0.40f*Pal[i1*3] + 0.35f*Pal[i1*3+1] + 0.25f*Pal[i1*3+2];
   L2 = 0.40f*Pal[i2*3] + 0.35f*Pal[i2*3+1] + 0.25f*Pal[i2*3+2];
   if ( L1>L2 ) return( 1 );
   else return( 0 );
}

EXTERN INT Reduce_CMap( PIXEL *Order, PIXEL *CMap, INT Nb_Col, INT Pad_To )
{
   INT i, j, New_Col;
   PIXEL New_Pal[768], Index[256];

   for( i=0; i<Nb_Col; ++i )  /* Search common colors...*/
   {
      INT R, G, B;
      R = CMap[ 3*i ];
      G = CMap[ 3*i+1 ];
      B = CMap[ 3*i+2 ];
      Order[i] = (PIXEL)i;
      for( j=0; j<i; ++j )
      {
         INT I;
         I = 3*Order[j];
         if ( R!=CMap[I] || G!=CMap[I+1] || B!=CMap[I+2] )
            continue;
         Order[i] = Order[j];
         break;
      }
   }
   New_Col = 0;
   for( i=0; i<Nb_Col; ++i )
   {
      if ( Order[i]==i )
      {
         New_Pal[3*New_Col] = CMap[3*i];
         New_Pal[3*New_Col+1] = CMap[3*i+1];
         New_Pal[3*New_Col+2] = CMap[3*i+2];
         New_Col++;
         Index[i] = (PIXEL)i;
      }
      else
      {
         Order[i] = Index[ Order[i] ];
      }
   }
   i = New_Col-1;
   for ( ; New_Col<Pad_To; ++New_Col )
   {
      New_Pal[3*New_Col]   = New_Pal[3*i];
      New_Pal[3*New_Col+1] = New_Pal[3*i+1];
      New_Pal[3*New_Col+2] = New_Pal[3*i+2];
      Index[New_Col] = (PIXEL)i;
   }
   memcpy( CMap, New_Pal, New_Col*3 );
   memcpy( Order, Index, New_Col );

   for( i=0; i<New_Col; ++i ) Index[i] = (PIXEL)i;
   Pal = CMap;
   qsort( Index, New_Col, sizeof( PIXEL ), Lum_Comp );
   for( i=0; i<New_Col; ++i )
   {
      INT J;
      J = Index[i]*3;
      New_Pal[3*i]   = CMap[J];
      New_Pal[3*i+1] = CMap[J+1];
      New_Pal[3*i+2] = CMap[J+2];
      Order[i] = Order[ Index[i] ];
   }
   memcpy( CMap, New_Pal, New_Col*3 );
   return( New_Col );
}

EXTERN void Remap_8bits( PIXEL *Ptr, PIXEL *Map, INT Size )
{
   INT i;
   for( i=0; i<Size; ++i ) Ptr[i] = Map[ Ptr[i] ];
}

/*******************************************************/
