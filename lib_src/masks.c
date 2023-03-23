/***********************************************
 *    colors masks utilities                   *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "mem_map.h"
#include "driver.h"
#include "cvrt_col.h"
#include "drv_col.h"

/*******************************************************************/

EXTERN FORMAT Compute_Format_Depth( FORMAT Format )
{
   INT D, i;
   D = 0;

   if ( (Format&0x0000F000)!=0 ) return( Format ); // already computed
   if ( Format == 0xFFFFFFFF ) return( Format );
   if ( (Format&0xFFF) == 0x000 ) return( (FORMAT)((Format&0xFFFF0FFF) | 0x1000) );
   for( i=0; i<12; i+=4 ) D += (Format>>i)&0x0F;
   D = (D+7)/8;
   return( (FORMAT)(Format | D<<12) );
}

/*******************************************************************/

static COLOR_CMAP *New_Color_Matcher( )
{
   COLOR_CMAP *Mask;

   Mask = New_Object( 1, COLOR_CMAP );
   if ( Mask==NULL ) return( NULL );
   Mem_Clear( Mask );
   Mask->Type = COLOR_CMAP_TYPE;
   Mask->Col_Convert = NULL;
   Mask->Stamp = 0;
   Mask->Dst_Stamp = NULL;
   return( Mask );
}

/*******************************************************************/

#ifndef SKL_NO_CVRT

/*******************************************************************/

static void Build_RGB_Convertion_Mask( FORMAT In, FORMAT Out, UINT Tab[256] )
{
   UINT C, i, In_Depth, Out_Depth;
   UINT In_Mask[4], In_Shift[4];
   UINT Out_Mask[4], Out_Shift[4];
   UINT Shift_R, Shift_G1, Shift_G2, Shift_B;

   memset( Tab, 0, 256*sizeof(UINT) );

   In_Depth = Format_Depth( In );
   Out_Depth = Format_Depth( Out );

   Format_Mask_And_Shift( In, In_Mask, In_Shift );
   Format_Mask_And_Shift( Out, Out_Mask, Out_Shift );

      /* RGB colors are mapped with four planes of 32bits final color:
       * Tab[i] for Red bits.
       * Tab[256+i] for upper green bits.
       * Tab[512+i] for lower green bits (if needed)
       * Tab[768+i] for blue bits.
       * This could be split or re-ordered to minimize cache misses
       * while converting...
       */

   if ( In_Depth == 1 )
   {
      Shift_R = Shift_G1 = Shift_B = 8;
      Shift_G2 = 0;
   }
   else if ( In_Depth == 2 )
   {
      Shift_R = Shift_G1 = 16;
      Shift_G2 = 8;
      Shift_B = 8;
   }
   else /* if ( In_Depth == 3 || In_Depth == 4 ) */
   {
      Shift_R = 24;
      Shift_G1 = 16;
      Shift_G2 = 0;
      Shift_B = 8;
   }

   for( i=0; i<256; ++i )
   {
      C = (i<<Shift_R)>>In_Shift[RED_F];
      Tab[i] = ( ( C & Out_Mask[RED_F] )<<Out_Shift[RED_F] )>>COL_BITS_FIX;

      C = (i<<Shift_G1)>>In_Shift[GREEN_F];
      Tab[256+i] = ( ( C & Out_Mask[GREEN_F] )<<Out_Shift[GREEN_F] )>>COL_BITS_FIX;

      if ( Shift_G2 )
      {
         C = (i<<Shift_G2)>>In_Shift[GREEN_F];
         Tab[512+i] = ( ( C & Out_Mask[GREEN_F] )<<Out_Shift[GREEN_F] )>>COL_BITS_FIX;
      }
      else Tab[512+i] = 0;

      C = (i<<Shift_B)>>In_Shift[BLUE_F];
      Tab[768+i] = ( ( C & Out_Mask[BLUE_F] )<<Out_Shift[BLUE_F] )>>COL_BITS_FIX;
   }
}

/*******************************************************************/

static COLOR_INDEXER *New_Color_Indexer( FORMAT Out )
{
   COLOR_INDEXER *Mask;

   Mask = New_Object( 1, COLOR_INDEXER );
   if ( Mask==NULL ) return( NULL );
   Mem_Clear( Mask );
   Mask->Type = INDEXER_TYPE;
   Mask->Col_Convert = Skl_Indexer_Tab[Format_Depth(Out)-1];
   Mask->Out = Out;
   Mask->Stamp = 0;
   return( Mask );
}

static COLOR_DITHERER *New_Color_Ditherer( MEM_ZONE *Src )
{
   INT i;
   COLOR_DITHERER *Mask;
   COLOR_ENTRY *Mask_Src;
   UINT *Fmt;
   RGB_MASK Dummy;

   Mask = New_Object( 1, COLOR_DITHERER );
   if ( Mask==NULL ) return( NULL );
   Mem_Clear( Mask );
   Mask->Type = DITHERER_TYPE;
   Mask->Col_Convert = Skl_Ditherer_Tab[MEM_Quantum(Src)-1];
   Mask->Stamp = 0;
   Mask->Dst_Stamp = NULL;

   Fmt = (UINT *)&Dummy.Masks;
   Build_RGB_Convertion_Mask( MEM_Format(Src), FMT_332, Fmt );

   Mask_Src = (COLOR_ENTRY *)Mask->Cols;
   for( i=0; i<256; ++i )
   {
      Mask->Cols[i][0] = (PIXEL)( Fmt[i] & 0xFF );
      Mask->Cols[i][1] = (PIXEL)( Fmt[i+256] & 0xFF );
      Mask->Cols[i][2] = (PIXEL)( Fmt[i+512] & 0xFF );
      Mask->Cols[i][3] = (PIXEL)( Fmt[i+768] & 0xFF );
      Mask->Match[i][RED_F] = (i&0xe0);
      Mask->Match[i][GREEN_F] = (i&0x1C)<<3;
      Mask->Match[i][BLUE_F] = (i&0x03)<<6;
      Mask->Match[i][INDEX_F] = (PIXEL)i;  /* To be best-matched later */
   }
   return( Mask );
}

/*******************************************************************/

static RGB_MASK *Masks = NULL;
static USHORT User_Defined_Mask = 0x0000;

/*******************************************************************/

EXTERN RGB_MASK *New_Masks( FORMAT In, FORMAT Out )
{
   RGB_MASK *New, **Cur;
//   INT i;
   INT In_Depth, Out_Depth;

   if ( In == 0xFFFF )
   {
      In = (FORMAT)(0xFFFF0000 | User_Defined_Mask);
      User_Defined_Mask++;
   }

   for( Cur=&Masks; (*Cur)!=NULL; Cur = &(*Cur)->Next )
   {
      if ( (*Cur)->In!=In || (*Cur)->Out!=Out ) continue;
      (*Cur)->Users++;
      New = *Cur;
      return( New );
   }

   New = New_Object( 1, RGB_MASK );
   if ( New==NULL ) return( NULL );
   New->Next = Masks;
   Masks = New;
   New->Type = RGB_MASK_TYPE;
   New->In = In;
   New->Out = Out;
   New->Users = 1;
   In_Depth = Format_Depth( In );
   Out_Depth = Format_Depth( Out );

   if ( (In_Depth>0) && (In!=Out) )
   {
      New->Col_Convert = Skl_Convert_Tab[ Out_Depth+4*In_Depth-5 ];
      Build_RGB_Convertion_Mask( In, Out, New->Masks );
   }
   else New->Col_Convert = NULL;    

   return( New );
}

EXTERN void Dispose_Mask( RGB_MASK *Mask )
{
   RGB_MASK *Cur;

   if ( Mask == NULL ) return;
   Mask->Users--;
   if ( (Mask->Type==RGB_MASK_TYPE)&&(Mask->Users==0) ) return;

   if ( Masks==Mask ) Masks = Mask->Next;
   else
   {
      for ( Cur=Masks; Cur->Next!=Mask; Cur = Cur->Next )
         if ( Cur==NULL ) break;
      if ( Cur!=NULL ) Cur->Next = Mask->Next;
   }
   M_Free( Mask );
}

#endif      // SKL_NO_CVRT

/*******************************************************************/

static BYTE Bit_Masks[] = { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };

EXTERN UINT Packed_Field_Pos( FORMAT Format )
{
   UINT F_Pos;

   F_Pos = Format_F_Pos(Format);
   if ( F_Pos != POS_PACKED ) return( F_Pos );
   if ( (Format&0x0FFF) == 0x000 ) return( F_Pos );  /* RGB_CMAP */

   F_Pos = 0x000;  /* no gap between color bits */

   return( F_Pos<<16 );    /* no FMT_REVERSED_BIT... */
}

EXTERN void Format_Mask_And_Shift( FORMAT Format, UINT Mask[], UINT Shift[] )
{
      /* decode Format into usable masks and shifts...*/

   INT i;
   UINT Field_Pos;
   INT Reversed, Total_Bits; 

   Field_Pos = Format_F_Pos( Format );
   if ( Field_Pos == POS_PACKED ) Field_Pos = Packed_Field_Pos( Format )>>16;

   Reversed = Format & FMT_REVERSED_BIT;
   Total_Bits = 0;

   i = ( Format & 0x000F ); Mask[ BLUE_F ] = (UINT)Bit_Masks[ i ];
   i += ( Field_Pos & 0x000F ); Shift[ BLUE_F ] = i;
   Total_Bits += i;

   i = ( Format & 0x00F0 )>>4; Mask[ GREEN_F ] = (UINT)Bit_Masks[ i ];
   i += ( Field_Pos & 0x00F0 )>>8; Shift[ GREEN_F ] = i + Total_Bits;
   Total_Bits += i;

   i = ( Format & 0x0F00 )>>8; Mask[ RED_F ] = (UINT)Bit_Masks[ i ];
   i += ( Field_Pos & 0x0F00 )>>16; Shift[ RED_F ] = i + Total_Bits;
   Total_Bits += i;   

/*
   i = ( Format & 0xF000 )>>12; Mask[ ALPHA_F ] = (UINT)Bit_Masks[ i ];
   i += ( Field_Pos & 0xF000 )>>24; Shift[ ALPHA_F ] = i;
*/
      /* TODO: Fix that! */
   Mask[ ALPHA_F ] = 0x00;
   Shift[ ALPHA_F ] = 0;;

   if ( Reversed )
   {
      Shift[BLUE_F] = Total_Bits-Shift[BLUE_F];
      Shift[GREEN_F] = Total_Bits-Shift[GREEN_F];
      Shift[RED_F] = Total_Bits-Shift[RED_F];
   }
}

EXTERN MEM_ZONE *Drv_Install_Converter( MEM_ZONE *M )
{
   UINT *Dst_Stamp;
   if ( M->Dst == NULL ) return( NULL );

   if ( (M->Dst->CMapper.Dummy!=NULL) && (M->Dst->CMapper.Dummy->Type!=RGB_MASK_TYPE) )
      Dst_Stamp = &M->Dst->CMapper.Dummy->Stamp;   /* Warning !!! Beware of the union fields !! */
   else Dst_Stamp = NULL;

   if ( Format_Depth( MEM_Format( M ) ) == 1 )
   {
      Install_Index_To_Any( (MEM_ZONE *)M, MEM_Format( M->Dst ), Dst_Stamp );
   }
   else Install_RGB_To_Any( (MEM_ZONE *)M, MEM_Format( M->Dst ), Dst_Stamp );

   return( M );
}

EXTERN MEM_ZONE *Install_Index_To_Any( MEM_ZONE *M, FORMAT Dst, UINT *Dst_Stamp )
{
   if ( Dst==0xFFFFFFFF )  // only CMap storing ( display )
   {         
      M->CMapper.Matcher = New_Color_Matcher( );
      if ( M->CMapper.Matcher == NULL ) return( NULL );
      M->CMapper.Matcher->Dst_Stamp = Dst_Stamp;
   }
#ifndef SKL_NO_CVRT
   else if ( (Dst&0x1FFF) == FMT_CMAP )
   {
            // Index to Index
      M->CMapper.Matcher = New_Color_Matcher( );
      if ( M->CMapper.Matcher == NULL ) return( NULL );
      M->CMapper.Matcher->Dst_Stamp = Dst_Stamp;
   }
   else
   {        // Index => RGB
      M->CMapper.Indexer = New_Color_Indexer( Dst );
      if ( M->CMapper.Indexer==NULL ) return( NULL );
   }
#endif   // SKL_NO_CVRT

   M->Type = (ZONE_TYPE)( M->Type | MEM_OWNS_CMAPPER );

   return M;
}

EXTERN MEM_ZONE *Install_RGB_To_Any( MEM_ZONE *M, FORMAT Fmt_Dst, UINT *Dst_Stamp )
{
#ifndef SKL_NO_CVRT

   COLOR_ENTRY Col_Tab[256];

   if ( (Fmt_Dst&0x1FFF) == FMT_CMAP )
   {
#ifndef FINAL
      Out_Message( "Warning: target format is FMT_CMAP. Maybe you meant FMT_332 !!" );
#endif
      M->CMapper.Ditherer = New_Color_Ditherer( M );      
      if ( M->CMapper.Ditherer == NULL ) return( NULL );
      M->CMapper.Ditherer->Dst_Stamp = Dst_Stamp;
   }
   else if ( Fmt_Dst!=0xFFFFFFFF )
   {
      M->CMapper.Mask = New_Masks( MEM_Format(M), Fmt_Dst );
      if ( M->CMapper.Mask == NULL ) return( NULL );
      if ( Format_Depth( Fmt_Dst )==1 && (M->Type & MEM_IS_DISPLAY ) )
      {
            /* ~FMT_332 */
         Drv_Build_RGB_Cube( Col_Tab, Fmt_Dst );
         (*((MEM_ZONE_DRIVER *)M)->Driver->Change_Colors)( (MEM_ZONE_DRIVER *)M, 256, Col_Tab );
      }
   }
   M->Type = (ZONE_TYPE)( M->Type | MEM_OWNS_CMAPPER );

#endif   // SKL_NO_CVRT

   return( M );
}

EXTERN void Clear_CMapper( MEM_ZONE *M )
{
   if ( M->CMapper.Dummy == NULL ) goto Ok;
   if ( M->Type & MEM_OWNS_CMAPPER )
   {
#ifndef SKL_NO_CVRT
      if ( M->CMapper.Dummy->Type == RGB_MASK_TYPE )
         Dispose_Mask( M->CMapper.Mask );
      else { M_Free( M->CMapper.Dummy ); };
#else
      M_Free( M->CMapper.Dummy );
#endif
   }
   M->CMapper.Dummy = NULL;
Ok:
   M->Type =(ZONE_TYPE)( M->Type & (~MEM_OWNS_CMAPPER));
}


/*******************************************************************/

