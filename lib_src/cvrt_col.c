/*
 * C-Portable color converters
 * Slow and still-to-optimize code
 *
 ******************************************/

#include "main.h"
#include "mem_map.h"
#include "cvrt_col.h"

/*******************************************************************/

#ifndef USE_ASM_CONVERTERS

static void Skl_CMap8_To_RGB32( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   UINT *Fmt_Cols = (UINT *)CM->Indexer->Fmt_Cols;
   while( --Size>=0 ) ( (UINT *)Dst)[Size] = Fmt_Cols[ Src[Size] ];
}

static void Skl_CMap8_To_RGB24( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   UINT *Fmt_Cols = (UINT *)CM->Indexer->Fmt_Cols;
   while( --Size>=0 )
   {
      PIXEL *C;
      C = (PIXEL*)( Fmt_Cols + (*Src++) );
      *Dst++ = *C++;
      *Dst++ = *C++;
      *Dst++ = *C++;
   }
}

static void Skl_CMap8_To_RGB16( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   UINT *Fmt_Cols = (UINT *)CM->Indexer->Fmt_Cols;
   while( --Size>=0 )
      ( (USHORT *)Dst)[Size] = (USHORT)( Fmt_Cols[ Src[Size] ] );
}

#endif   // USE_ASM_CONVERTERS

EXTERN void (*Skl_Indexer_Tab[4])( SKL_CMAPPER *, PIXEL *, PIXEL *, INT ) =
{
   NULL, Skl_CMap8_To_RGB16, Skl_CMap8_To_RGB24, Skl_CMap8_To_RGB32
};

/**************************************************************/
/**************************************************************/

#ifndef USE_ASM_CONVERTERS

static void Skl_RGB8_To_RGB8( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   UINT *Mask = CM->Mask->Masks;
   Src += Size; Dst += Size; Size = -Size;
   while( Size<0 )
   {
      UINT D; PIXEL C;
      C = Src[Size];
      D  = (Mask)[ C ];
      D |= (Mask + 256)[ C ];
      D |= (Mask + 768)[ C ];
      Dst[Size++] = (PIXEL)(D&0xFF);
   }
}

static void Skl_RGB8_To_RGB16( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   USHORT *Dst2 = (USHORT *)Dst;
   UINT *Mask = CM->Mask->Masks;

  Src += Size; Dst2 += Size; Size = -Size;
   while( Size<0 )
   {
      UINT D; PIXEL C;
      C = Src[Size];
      D  = (Mask)[ C ];
      D |= (Mask + 256)[ C ];
      D |= (Mask + 512)[ C ];
      D |= (Mask + 768)[ C ];
      Dst2[Size++] = (USHORT)D;
   }
}

static void Skl_RGB8_To_RGB24( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   UINT *Mask = CM->Mask->Masks;
   while( Size>0 )
   {
      UINT D;
      PIXEL C;
      C = *Src++;
      D  = (Mask)[ C ];
      D |= (Mask + 256)[ C ];
      D |= (Mask + 768)[ C ];
      *Dst++ = (PIXEL)( (D>>16)&0xFF );
      *Dst++ = (PIXEL)( (D>>8)&0xFF );
      *Dst++ = (PIXEL)( D&0xFF );
      Size--;
   }

}

static void Skl_RGB8_To_RGB32( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   UINT *Dst2 = (UINT *)Dst;
   UINT *Mask = CM->Mask->Masks;

   Src += Size; Dst += Size; Size = -Size;
   while( Size<0 )
   {
      UINT D;
      PIXEL C;
      C = Src[Size];
      D  = (Mask)[ C ];
      D |= (Mask + 256)[ C ];
      D |= (Mask + 768)[ C ];
      Dst2[Size++] = D;
   }
}

/**************************************************************/

static void Skl_RGB16_To_RGB8( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   USHORT *Src2 = (USHORT *)Src;
   UINT *Mask = CM->Mask->Masks;
  Src2 += Size; Dst += Size; Size = -Size;
   while( Size<0 )
   {
      PIXEL D; USHORT C;
      C = Src2[Size];
      D  = (PIXEL)(Mask)[ C>>8 ];
      D |= (PIXEL)(Mask + 256)[ C>>8 ];
      D |= (PIXEL)(Mask + 512)[ C&0xFF ];
      D |= (PIXEL)(Mask + 768)[ C&0xFF ];
      Dst[Size++] = D;
   }
}

static void Skl_RGB16_To_RGB16( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   USHORT *Dst2 = (USHORT *)Dst, *Src2 = (USHORT *)Src;
   UINT *Mask = CM->Mask->Masks;
   Src2 += Size; Dst2 += Size; Size = -Size;
   while( Size<0 )
   {
      USHORT D, C;
      C = Src2[Size];
      D  = (USHORT)(Mask)[ C>>8 ];
      D |= (USHORT)(Mask + 256)[ C>>8 ];
      D |= (USHORT)(Mask + 512)[ C&0xFF ];
      D |= (USHORT)(Mask + 768)[ C&0xFF ];
      Dst2[Size++] = D;
   }
}

static void Skl_RGB16_To_RGB24( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   USHORT *Src2 = (USHORT *)Src;
   UINT *Mask = CM->Mask->Masks;

   while( --Size>=0 )
   {
      UINT D;
      USHORT C;
      C = *Src2++;
      D  = (Mask)[ C>>8 ];
      D |= (Mask + 256)[ C>>8 ];
      D |= (Mask + 512)[ C&0xFF ];
      D |= (Mask + 768)[ C&0xFF ];
      *Dst++ = (PIXEL)( (D>>16)&0xFF );
      *Dst++ = (PIXEL)( (D>>8)&0xFF );
      *Dst++ = (PIXEL)( D&0xFF );
   }
}

static void Skl_RGB16_To_RGB32( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   UINT *Dst2 = (UINT *)Dst;
   USHORT *Src2 = (USHORT *)Src;
   UINT *Mask = CM->Mask->Masks;
   Src2 += Size; Dst2 += Size; Size = -Size;
   while( Size<0 )
   {
      UINT D; USHORT C;
      C = Src2[Size];
      D  = (Mask)[ C>>8 ];
      D |= (Mask + 256)[ C>>8 ];
      D |= (Mask + 512)[ C&0xFF ];
      D |= (Mask + 768)[ C&0xFF ];
      Dst2[Size++] = D;
   }
}

/**************************************************************/

static void Skl_RGB24_To_RGB8( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   UINT *Mask = CM->Mask->Masks;
   while( --Size>=0 )
   {
      UINT D;      
      D  = (Mask + 768)[ *Src++ ];
      D |= (Mask + 256)[ *Src++ ];
      D |= Mask[ *Src++ ];
      *Dst++ = (PIXEL)D;
   }
}

static void Skl_RGB24_To_RGB16( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   USHORT *Dst2 = (USHORT *)Dst;
   UINT *Mask = CM->Mask->Masks;

   while( --Size>=0 )
   {
      UINT D; PIXEL C;
      D  = (Mask + 768)[ *Src++ ];
      C = *Src++;
      D |= (Mask + 512)[ C ];
      D |= (Mask + 256)[ C ];
      *Dst2++ = (USHORT)( D | Mask[ *Src++ ] );
   }
}

static void Skl_RGB24_To_RGB32( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   while( --Size>=0 )     // TODO: this can be optimized, for sure...
   {
      *Dst++ = *Src++;
      *Dst++ = *Src++;
      *Dst++ = *Src++;
      Dst++;
   }
}

/**************************************************************/

static void Skl_RGB32_To_RGB8( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   UINT *Mask = CM->Mask->Masks;
   while( --Size>=0 )
   {
      UINT D;
      D  = (Mask + 768)[ *Src++ ];
      D |= (Mask + 256)[ *Src++ ];
      D |= Mask[ *Src++ ];
      *Dst++ = (PIXEL)D;
      Src++;
   }
}

static void Skl_RGB32_To_RGB16( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   USHORT *Dst2 = (USHORT *)Dst;
   UINT *Mask = CM->Mask->Masks;

   while( --Size>=0 )
   {
      UINT D; PIXEL C;
      D  = (Mask + 768)[ *Src++ ];
      C = *Src++;
      D |= (Mask + 512)[ C ];
      D |= (Mask + 256)[ C ];
      *Dst2++ = (USHORT)( D | Mask[ *Src++ ] );
      Src++;
   }
}

static void Skl_RGB32_To_RGB24( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   while( --Size>=0 )
   {
      *Dst++ = *Src++;
      *Dst++ = *Src++;
      *Dst++ = *Src++;
      Src++;
   }
}

/**************************************************************/

#endif   // USE_ASM_CONVERTERS

EXTERN void (*Skl_Convert_Tab[])( SKL_CMAPPER *, PIXEL *, PIXEL *, INT ) =
{
   Skl_RGB8_To_RGB8,  Skl_RGB8_To_RGB16,  Skl_RGB8_To_RGB24,  Skl_RGB8_To_RGB32,
   Skl_RGB16_To_RGB8, Skl_RGB16_To_RGB16, Skl_RGB16_To_RGB24, Skl_RGB16_To_RGB32,
   Skl_RGB24_To_RGB8, Skl_RGB24_To_RGB16, NULL,               Skl_RGB24_To_RGB32,
   Skl_RGB32_To_RGB8, Skl_RGB32_To_RGB16, Skl_RGB32_To_RGB24, NULL
};

/**************************************************************/
/**************************************************************/

#ifndef USE_ASM_CONVERTERS

static void Skl_RGB8_To_CMap8( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   COLOR_ENTRY *Mask = CM->Ditherer->Cols;
   COLOR_ENTRY *Match = CM->Ditherer->Match;
   Src += Size; Dst += Size; Size = -Size;
   while( Size<0 )
   {
      PIXEL D; PIXEL C;
      C = Src[Size];
      D  = Mask[ C ][0];
      D |= Mask[ C ][1];      // no green
      D |= Mask[ C ][3];
      Dst[Size++] = Match[D][INDEX_F];
   }
}
static void Skl_RGB16_To_CMap8( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   USHORT *Src2 = (USHORT *)Src;
   COLOR_ENTRY *Mask = CM->Ditherer->Cols;
   COLOR_ENTRY *Match = CM->Ditherer->Match;
   Src2 += Size; Dst += Size; Size = -Size;
   while( Size<0 )
   {
      PIXEL D; USHORT C;
      C = Src2[Size];
      D  = Mask[ C>>8 ][0];
      D |= Mask[ C>>8 ][1];
      D |= Mask[ C&0xFF ][2];
      D |= Mask[ C&0xFF ][3];
      Dst[Size++] = Match[D][INDEX_F];
   }
}
static void Skl_RGB24_To_CMap8( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   COLOR_ENTRY *Mask = CM->Ditherer->Cols;
   COLOR_ENTRY *Match = CM->Ditherer->Match;

   while( --Size>=0 )
   {
      PIXEL D;
      D  = Mask[ *Src++ ][3];
      D |= Mask[ *Src++ ][1];
      D |= Mask[ *Src++ ][0];
      *Dst++ = Match[D][INDEX_F];
   }
}

static void Skl_RGB32_To_CMap8( SKL_CMAPPER *CM, PIXEL *Dst, PIXEL *Src, INT Size )
{
   COLOR_ENTRY *Mask = CM->Ditherer->Cols;
   COLOR_ENTRY *Match = CM->Ditherer->Match;

   while( --Size>=0 )
   {
      PIXEL D;
      D  = Mask[ *Src++ ][3];
      D |= Mask[ *Src++ ][1];
      D |= Mask[ *Src++ ][0];
      *Dst++ = Match[D][INDEX_F];
      Src++;
   }
}

/**************************************************************/

#endif   // USE_ASM_CONVERTERS

EXTERN void (*Skl_Ditherer_Tab[4])( SKL_CMAPPER *, PIXEL *, PIXEL *, INT ) =
{
   Skl_RGB8_To_CMap8, Skl_RGB16_To_CMap8, Skl_RGB24_To_CMap8, Skl_RGB32_To_CMap8
};

/**************************************************************/
/**************************************************************/
