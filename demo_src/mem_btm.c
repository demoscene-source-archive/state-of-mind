/***********************************************
 *        Bitmaps as MEM_ZONE                  *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "mem_map.h"
#include "mem_btm.h"

/*******************************************************/

extern MEM_ZONE_METHODS Zone_Methods;

/*******************************************************/ 

static void Destroy( MEM_ZONE *Zone )
{
   MEM_BITMAP *Btm;
   Btm = (MEM_BITMAP *)Zone;

   if ( MEM_Flags( Btm ) & IMG_OWNS_PTR ) {  M_Free( Btm->Bits ); }      
   if ( Btm->Flags & MEM_OWNS_METHODS ) { M_Free( Btm->Methods ); }      
   M_Free( Btm->Pal );
   M_Free( Btm->Blur_Pal );
   M_Free( Btm->Sat_Pal );
   M_Free( Btm->Bits );
   M_Free( Btm );
}

static PIXEL *Get_Scanline( MEM_ZONE *M, INT Y )
{
   return( (*DEFAULT_METHODS->Get_Scanline)( M, Y ) );
}

static MEM_ZONE_METHODS Zone_Methods =
{
   NULL, NULL, Destroy, Get_Scanline, NULL,
};

/*******************************************************/

EXTERN MEM_BITMAP *Full_MEM_BITMAP( 
   FORMAT Format, 
   int Width, int Height,  PIXEL *Bits,
   int Nb_Col, int Nb_Blur_Col, int Nb_Sat_Col )
{
   MEM_BITMAP *Btm;

   Btm = New_MEM_Zone( MEM_BITMAP, NULL );
   if ( Btm == NULL ) 
#ifdef FINAL
      Exit_Upon_Mem_Error( "MEM_BITMAP", 0 );
#else
      return( NULL );
#endif
   Mem_Clear( Btm );
   Btm->Methods = &Zone_Methods;

   Format = MEM_Format( Btm ) = Compute_Format_Depth( Format );
   if ( Mem_Img_Set( MEM_IMG(Btm), IMG_OWNS_PTR,
      Width, Height, 0, Format_Depth( Format ), Format, Bits ) == NULL )
         goto Failed;

   Btm->Nb_Col      = Nb_Col;
   Btm->Nb_Blur_Col = Nb_Blur_Col;
   Btm->Nb_Sat_Col  = Nb_Sat_Col;

   Btm->Pal = New_Object( Btm->Nb_Col, COLOR_ENTRY );
   if ( Nb_Col && Btm->Pal == NULL ) goto Failed;
   Btm->Blur_Pal = New_Object( Btm->Nb_Blur_Col, PIXEL );
   if ( Nb_Blur_Col && Btm->Blur_Pal == NULL ) goto Failed;
   Btm->Sat_Pal = New_Object( Btm->Nb_Sat_Col, PIXEL );
   if ( Nb_Sat_Col && Btm->Sat_Pal == NULL ) goto Failed;

   Btm->Bits = Bits;

   return( Btm );

Failed:
   Destroy( (MEM_ZONE *)Btm );
   return( NULL );
}

EXTERN MEM_BITMAP *New_MEM_BITMAP( FORMAT Format, int Width, int Height, int Nb_Col )
{
   MEM_BITMAP *Btm;

   Btm = Full_MEM_BITMAP( Format, Width, Height, NULL, Nb_Col, 0, 0 );
   if ( Btm==NULL ) return( NULL );
   return( Btm );
}

EXTERN MEM_BITMAP *Align_MEM_BTM_64( MEM_BITMAP *Btm )
{
   PIXEL *New_Bits, *Bits;
   UINT Size;

      /* one should always align textures *FIRST* (less junk) */

   if ( !( MEM_Flags(Btm) & IMG_OWNS_PTR ) ) return( Btm );
   if ( ( (int)(Btm->Bits) & 0xFFFF ) == 0 ) return( Btm );

   Size = MEM_Size(Btm);
   New_Bits = Bits = NULL;

   if ( Malloc_64( &New_Bits, &Bits, Size ) == NULL ) return( NULL );

   if ( (Size&0x0F) == 0 )
      F_MemCopy( New_Bits, Btm->Bits, Size, "btm_goblayz" );
   else MemCopy( New_Bits, Btm->Bits, Size );
   M_Free( Btm->Bits );
   Btm->Bits = New_Bits;
   MEM_Base_Ptr(Btm) = Bits;

   return( Btm );
}

/*******************************************************/

#if 0

EXTERN MEM_BITMAP *Duplicate_MEM_BITMAP( MEM_BITMAP *Src, INT Width, INT Height )
{
   BITMAP *New;
   int W, H;
   int i, j, ii, jj;
   PIXEL *New_Bits;

   W = Src->Width; H = Src->Height;

   if ( Width == 0 || Height == 0 ) { Width = W; Height = H; }

   New = New_Bitmap( Src->Type, Width, Height, Src->Nb_Col );

   if ( Src->Nb_Col ) 
      memcpy( New->Pal, Src->Pal, Src->CMap_Depth*Src->Nb_Col ); 
   if ( Src->Nb_Blur_Col )
   {
      New->Blur_Pal = New_Fatal_Object( Src->Nb_Blur_Col, PIXEL );
      memcpy( New->Blur_Pal, Src->Blur_Pal, Src->Nb_Blur_Col );
   }
   if ( Src->Nb_Sat_Col )
   {
      New->Sat_Pal = New_Fatal_Object( Src->Nb_Sat_Col, PIXEL );
      memcpy( New->Sat_Pal, Src->Sat_Pal, Src->Nb_Sat_Col );
   }

   if ( New->Type & BTM_WITHOUT_BITS ) return( New );

   if ( W == Width || H == Height )
   {
      memcpy( MEM_Base_Ptr(Btm), MEM_Base_Ptr(Src), MEM_Size(Src) );
      return( New );
   }

      /* Beurk... XXXX */

   New_Bits = MEM_Base_Ptr(NewSrc);
   if ( Src->Pixel_Depth == COL_8BITS )
   {
      for( j=0; j<Height; j++ )
         for( i=0; i<Width; ++i )
         {
            ii = (i*W)/Width;
            jj = (j*H)/Height;
            *New_Bits++ = Src->Bits[ ii+W*jj ];
         }
   }
   else if ( Src->Pixel_Depth == COL_16BITS )
   {
      USHORT *Bits;
      Bits = (USHORT *)Src->Bits;
      for( j=0; j<Height; j++ )
         for( i=0; i<Width; i++ )
         {
            ii = (i*W)/Width;
            jj = (j*H)/Height;
            *(USHORT*)New_Bits = Bits[ ii+W*jj ];
            New_Bits += 2;
         }
   }
   else if ( Src->Pixel_Depth == COL_24BITS )
   {
      for( j=0; j<Height; j++ )
         for( i=0; i<Width; i++ )
         {
            ii = (i*W)/Width;
            ii += W * (j*H)/Height;
            ii *= 3;
            *New_Bits++ = Src->Bits[ ii ];
            *New_Bits++ = Src->Bits[ ii+1 ];
            *New_Bits++ = Src->Bits[ ii+2 ];
         }
   }
   else if ( Src->Pixel_Depth == COL_24BITS )
   {
      UINT *Bits;
      Bits = (UINT *)Src->Bits;
      for( j=0; j<Height; j++ )
         for( i=0; i<Width; i++ )
         {
            ii = (i*W)/Width;
            ii += W * (j*H)/Height;
            *( UINT *)New_Bits = Bits[ ii ];
            New_Bits += 4;
         }
   }
   return( New );
}

EXTERN BITMAP *Clear_Bitmap_Resources( BITMAP *Bitmap )
{
   if ( Bitmap == NULL ) return( NULL );

   if ( !( Bitmap->Type & BTM_WITHOUT_BITS ) )
      M_Free( Bitmap->Real_Bits );
   Bitmap->Real_Bits = Bitmap->Bits = NULL;

   if ( !( Bitmap->Type & BTM_WITHOUT_CMAP ) )
      M_Free( Bitmap->Pal );
   Bitmap->Pal = NULL;

   if ( !( Bitmap->Type & BTM_WITHOUT_BLUR_PAL ) )
      M_Free( Bitmap->Blur_Pal );
   Bitmap->Blur_Pal = NULL;

   if ( !( Bitmap->Type & BTM_WITHOUT_SAT_PAL ) )
      M_Free( Bitmap->Sat_Pal );
   Bitmap->Sat_Pal = NULL;
   return( Bitmap );
}

EXTERN BITMAP *Destroy_Mem_Bitmap( MEM_BITMAP *Bitmap )
{
   Clear_Bitmap_Resources( Bitmap );
   M_Free( Bitmap );
   return( NULL );
}

/*******************************************************/

EXTERN void Resize_Bitmap( BITMAP *Bitmap, int Width, int Height )
{
   int W, H;
   int i, j, Size;
   PIXEL *New_Bits, *Bits;

   W = Bitmap->Width;
   H = Bitmap->Height;
   if ( ( W == Width ) && ( H == Height ) )
      return;

   Size = Width*Height*Bitmap->Pixel_Depth;

   if ( Bitmap->Type & BTM_WITHOUT_BITS ) goto Just_Template;

   New_Bits = ( PIXEL *)My_Malloc( ( Size+31 ) & 0xFFFFFFF0 );
   if ( New_Bits == NULL ) Exit_Upon_Mem_Error( "resized bitmap", Size );

      /* XXX A finir avec les differentes Depths... */

   Bits = ( PIXEL *)( ( (UINT)(New_Bits) + 15 ) & 0xFFFFFFF0 );
   if ( Bitmap->Pixel_Depth == 1 )
   {
      PIXEL *Ptr = Bits;
      for( j=0; j<Height; ++j )
      {
         PIXEL *Ptr2 = Bitmap->Bits + (j*H/Height)*W;
         for( i=0; i<Width; ++i )
            *Ptr++ = Ptr2[ i*W/Width ];
      }
   }
   else if ( Bitmap->Pixel_Depth == 2 )
   {
      USHORT *Ptr = (USHORT *)Bits;
      for( j=0; j<Height; ++j )
      {
         int jj = j*H/Height;
         for( i=0; i<Width; ++i )
         {
            int ii = 2*( i*W/Width + jj );
            *Ptr++ = *( USHORT *)( Bitmap->Bits + ii );
         }
      }
   }
   else if ( Bitmap->Pixel_Depth == 3 )
   {
      PIXEL *Ptr = Bits;
      for( j=0; j<Height; ++j )
      {
         int jj = j*H/Height;
         for( i=0; i<Width; ++i )
         {
            int ii = 3*( i*W/Width + jj );
            *Ptr++ = Bitmap->Bits[ ii++ ];
            *Ptr++ = Bitmap->Bits[ ii++ ];
            *Ptr++ = Bitmap->Bits[ ii ];
         }
      }
   }
   else if ( Bitmap->Pixel_Depth == 4 )
   {
      UINT *Ptr = ( UINT *)Bits;
      for( j=0; j<Height; ++j )
      {
         int jj = (j*H)/Height;
         for( i=0; i<Width; ++i )
         {
            int ii = 4*( jj + (i*W)/Width );
            *Ptr++ = *( UINT *)( Bitmap->Bits + ii );
         }
      }
   }

   M_Free( Bitmap->Real_Bits );
   Bitmap->Real_Bits = New_Bits;
   Bitmap->Bits = Bits;
Just_Template:
   Bitmap->Width = Width;
   Bitmap->Height = Height;
}

/*******************************************************/

#endif /* 0 */


