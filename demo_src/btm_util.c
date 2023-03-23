/***********************************************
 *         BITMAP misc utilities               *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "btm.h"

EXTERN BITMAP *Resize_Bitmap( BITMAP *Btm, int Width, int Height )
{
   int W, H;
   int i, j, Size;
   PIXEL *New_Bits, *Ptr;

   W = Btm->Width;
   H = Btm->Height;
   if ( ( W == Width ) && ( H == Height ) ) return( Btm );
   Size = Width*Height;

   New_Bits = ( PIXEL *)My_Malloc( ( Size+31 ) & 0xFFFFFFF0 );
   if ( New_Bits == NULL ) return( NULL );

   Ptr = New_Bits;
   for( j=0; j<Height; ++j )
   {
      PIXEL *Ptr2 = Btm->Bits + (j*H/Height)*W;
      for( i=0; i<Width; ++i )
         *Ptr++ = Ptr2[ i*W/Width ];
   }
   M_Free( Btm->Bits );
   Btm->Bits = New_Bits;
   Btm->Width = Width;
   Btm->Height = Height;
   Btm->Size = Width*Height;
   return( Btm );
}

/*******************************************************/
/****************** misc. stuff ************************/
/*******************************************************/

#ifndef SKL_LIGHT_CODE


EXTERN void Print_BTM_Infos( BITMAP *Btm, FILE *Where )
{
   fprintf( Where, "BTM: Size: %d x %d pixels. Nb_Col: %d.\n",
      Btm->Width, Btm->Height, Btm->Nb_Col );
}

EXTERN BITMAP *Duplicate_Bitmap( BITMAP *Src, INT Width, INT Height )
{
   BITMAP *New;
   int W, H;
   int i, j, ii, jj;
   PIXEL *New_Bits;

   W = Src->Width; H = Src->Height;

   if ( Width == 0 || Height == 0 ) { Width = W; Height = H; }

   New = New_Bitmap( Width, Height, Src->Nb_Col );
   if ( New==NULL ) return( NULL );

   if ( Src->Nb_Col ) memcpy( New->Pal, Src->Pal, 3*Src->Nb_Col ); 

   if ( W == Width || H == Height )
   {
      memcpy( New->Bits, Src->Bits, W*H );
      return( New );
   }

   New_Bits = New->Bits;
   for( j=0; j<Height; j++ )
      for( i=0; i<Width; ++i )
      {
         ii = (i*W)/Width;
         jj = (j*H)/Height;
         *New_Bits++ = Src->Bits[ ii+W*jj ];
      }
   return( New );
}

EXTERN PIXEL Search_Max_Color( BITMAP *BTM )
{
   INT i;
   PIXEL Bit, Max = 0;

   for( i=0; i<BTM->Size; ++i )
   {
      Bit = BTM->Bits[ i ];
      if ( Bit>Max ) Max = Bit;
   }
   return( Max+1 );
}

EXTERN void Filter_Bitmap( BITMAP *Btm, PIXEL And, PIXEL Or )
{
   int i;
   PIXEL *Ptr;

   Ptr = Btm->Bits;
   for( i=Btm->Size; i>0; --i )
   {
      *Ptr = ( (*Ptr) & And ) | Or;
      Ptr++;
   }
}

EXTERN void Scroll_Bitmap_Up( PIXEL *Src, USHORT W, USHORT H, USHORT Offset )
{
   int i;
   PIXEL Save[ 8*320 ];	/* ~8 lines safe */

   Offset *= W;
   for( i=Offset-1; i>=0; --i ) Save[i] = Src[i];
   for( i=H*W-Offset; i>0; --i )
   {
      *Src = Src[ Offset ];
      Src++;
   }
   for( i=Offset-1; i>=0; --i ) *Src++ = Save[i];
}

EXTERN void Scroll_Bitmap_Down( PIXEL *Src, USHORT W, USHORT H, USHORT Offset )
{
   int i;
   PIXEL *Ptr, *Ptr2, Save[ 8*320 ];

   Offset *= W;
   Ptr = &Src[ H*W-Offset ];
   Ptr2 = Save;
   for( i=Offset; i>0; --i ) *Ptr2++ = *Ptr++;

   i = H*W-Offset-1;
   Src = &Src[i]; 
   for( ; i>=0; --i )
   {
      Src[Offset] = *Src;
      Src--;
   }
   Ptr = Save;
   Src++;
   for( i=Offset; i>0; --i ) *Src++ = *Ptr++;
}

#endif   // SKL_LIGHT_CODE

/*******************************************************/

