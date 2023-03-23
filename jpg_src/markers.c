/************************************************
 *              JPEG marker loader              *
 *                                              *
 * Skal 98                                      *
 ************************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "bit_strm.h"
#include "./jpeg.h"

/*******************************************************/

#include "./markers.h"
#define END_QUANTIZATION_TABLE 0xFF
#define END_CODE_TABLE 0xFF

/*******************************************************/

SHORT JPG_Natural_Order[] = {
  0,  1,  8, 16,  9,  2,  3, 10,
 17, 24, 32, 25, 18, 11,  4,  5,
 12, 19, 26, 33, 40, 48, 41, 34,
 27, 20, 13,  6,  7, 14, 21, 28,
 35, 42, 49, 56, 57, 50, 43, 36,
 29, 22, 15, 23, 30, 37, 44, 51,
 58, 59, 52, 45, 38, 31, 39, 46,
 53, 60, 61, 54, 47, 55, 62, 63,
 63, 63, 63, 63, 63, 63, 63, 63, // extra entries for safety in decoder
 63, 63, 63, 63, 63, 63, 63, 63
};

INT JPG_Zigzag_Index[] =
{
   0,  1,  5,  6, 14, 15, 27, 28,
   2,  4,  7, 13, 16, 26, 29, 42,
   3,  8, 12, 17, 25, 30, 41, 43,
   9, 11, 18, 24, 31, 40, 44, 53,
  10, 19, 23, 32, 39, 45, 52, 54,
  20, 22, 33, 38, 46, 51, 55, 60,
  21, 34, 37, 47, 50, 56, 59, 61,
  35, 36, 48, 49, 57, 58, 62, 63
};

/*******************************************************/

#if 0
static BYTE JPG_First_Marker( JPG_INFO *Info )
{
   if ( BSTRM_ONE_BYTE( )!=0xFF ) return( 0xFF ) return( 0 );
   Info->Marker = BSTRM_ONE_BYTE( );
   if ( Info->Marker!=M_SOI ) return( 0 );
   return( Info->Marker );
}
#endif

static BYTE JPG_Next_Marker( JPG_INFO *Info )
{
Restart:
   while ( BSTRM_ONE_BYTE( )!=0xFF );
   do { Info->Marker = BSTRM_ONE_BYTE( ); } 
   while( Info->Marker == 0xFF );
   if ( Info->Marker==0x00 ) 
      goto Restart;
   return( (BYTE)Info->Marker );
}

#if 0
static BYTE JPG_Get_SOI( JPG_INFO *Info )
{
   int i;

       // Reset all parameters that are defined to be reset by SOI

   for( i=0; i<NUM_ARITH_TBLS; ++i )
   {
      Info->arith_dc_L[i] = 0;
      Info->arith_dc_U[i] = 1;
      Info->arith_ac_K[i] = 5;
   }
   Info->Restart_Interval = 0;

       // Set initial assumptions for colorspace etc

   Info->Color_Space = JCS_UNKNOWN;
   Info->CCIR601_Sampling = FALSE;      // Assume non-CCIR sampling???

   Info->Saw_JFIF_Marker = FALSE;

#if defined( USE_JFIF )
   Info->density_unit = 0;   // set default JFIF APP0 values
   Info->X_density = 1;
   Info->Y_density = 1;
   Info->saw_Adobe_marker = FALSE;
   Info->Adobe_transform = 0;
#endif

   Info->Saw_SOI = TRUE;

   return( 0 );
}
#endif

/*******************************************************/

static void Check_Scan( JPG_INFO *Info, JPG_SCAN *Scan )
{
   INT i;

   if ( Scan->Nb_Comp==1 )
   {
      i = Info->Global_Width*Info->xf[Scan->ci[0]]-1;
      i = ( i/Info->Max_X ) + 1;
      if ( Info->Type!=3 ) i = ( (i-1)/8 ) + 1;
      Scan->MDUWide = i;

      i = Info->Global_Height*Info->yf[Scan->ci[0]]-1;
      i = ( i/Info->Max_Y ) + 1;
      if ( Info->Type!=3 ) i = ( (i-1)/8 ) + 1;
      Scan->MDUHigh = i;
   }
   else
   {
      Scan->MDUWide = Info->MDUWide;
      Scan->MDUHigh = Info->MDUHigh;
   }
}

/*******************************************************/

static void JPG_MakeConsistentFrameSize( JPG_INFO *Info )
{
   INT i, Max_X, Max_Y;

   Max_Y = Max_X = 1;

   for( i=0; i<MAX_COMPONENTS; ++i )
   {
      if ( Info->yf[i]>Max_Y ) Max_Y = Info->yf[i];
      if ( Info->xf[i]>Max_X ) Max_X = Info->xf[i];
   }
   for( i=0; i<MAX_COMPONENTS; ++i )
   {
      if ( Info->xf[i] )
      {
         if ( !Info->Width[i] )
            Info->Width[i] = ( ((Info->Global_Width*Info->xf[i])-1)/Max_X ) + 1;
         if ( !Info->Height[i] )
            Info->Height[i] = ( ((Info->Global_Height*Info->yf[i])-1)/Max_Y ) + 1;
      }
   }
   Info->Max_Y = Max_Y; Info->Max_X = Max_X;
   Info->MDUWide = ( Info->Global_Width-1 )/Max_X + 1;
   if ( Info->Global_Height ) Info->MDUHigh = ( Info->Global_Height-1 )/Max_Y + 1;
   else Info->MDUHigh = 0;

   if ( Info->Type!=3 )
   {
      Info->MDUWide = ( Info->MDUWide-1 )/8 + 1;
      if ( Info->MDUHigh )
         Info->MDUHigh = ( Info->MDUHigh-1 )/8 + 1;
   }
#ifndef LIGHT_CODE
   for( i=0; i<MAX_COMPONENTS; ++i )
      if ( Info->xf[i] )
      {
         INT Test_W, Test_H;

         Test_W = ( ( Info->Width[i]-1 )/Info->xf[i] ) + 1;
         if ( Info->Type!=3 ) Test_W = (Test_W-1)/8 + 1;
         if ( Info->MDUWide!=Test_W )
            Exit_Upon_Error( "inconsistent JPEG sizes" );
         if ( Info->MDUHigh )
         {
             Test_H = ( ( Info->Height[i]-1 )/Info->yf[i] ) + 1;
             if ( Info->Type!=3 ) Test_H = (Test_H-1)/8 + 1;
             if ( Info->MDUHigh!=Test_H )
                Exit_Upon_Error( "inconsistent JPEG sizes" );
         }
      }
#endif
}

static BYTE JPG_Get_SOF( JPG_INFO *Info )
{
   INT Length;
   INT ci;

   Length = BSTRM_ONE_WORD( );
   if ( Length<8 ) return( -1 );
   Length -= 8;

   Info->Data_Precision = BSTRM_ONE_BYTE( );
   Info->Scale = 1.0f*( 1<<(14-Info->Data_Precision) );
   Info->Global_Height = BSTRM_ONE_WORD( );
   Info->Global_Width  = BSTRM_ONE_WORD( );
   
   Info->Global_Nb_Comp = BSTRM_ONE_BYTE( );

   for( ci=0; ci<MAX_COMPONENTS; ++ci )
      Info->xf[ci] = Info->yf[ci] = Info->tq[ci] = 0;

   for( ci=0; ci<Info->Global_Nb_Comp; ++ci )
   {
      BYTE c, j;
      j = BSTRM_ONE_BYTE( );
      c = BSTRM_ONE_BYTE( );
      Info->cn[ci] = j;
      Info->xf[j] = HI15(c);
      Info->yf[j] = LO15(c);
      Info->tq[j] = BSTRM_ONE_BYTE( );
   }
   JPG_MakeConsistentFrameSize( Info );

   return( 0 );
}

/*******************************************************/

#if 0
static BYTE JPG_Get_DNL( JPG_INFO *Info, JPG_SCAN *Scan )
{
   INT Length, i, n;

   Length =  BSTRM_ONE_WORD( );
   Info->Global_Height =  BSTRM_ONE_WORD( );
   if ( Scan->Nb_Comp )
   {
      JPG_MakeConsistentFrameSize( Info );
      Check_Scan( Info, Scan );
      Resize_IOB( Info );
      if ( Info->Global_Height )
      {
         Install_IOB( Scan->Iob[0] );
         Info->Nb_MDU = Info->MDUWide*Info->MDUHigh;
      }
   }
   else Info->Nb_MDU = -1;
   return( 0 );
}
#endif

/*******************************************************/

static BYTE JPG_Get_SOS( JPG_SCAN *Scan )
{
   INT Length, i, n;

   Length = BSTRM_ONE_WORD( );

   Scan->Nb_Comp = n = BSTRM_ONE_BYTE( );

      // Collect the component-spec parameters

   for( i=0; i<n; ++i )
   {
      BYTE cb;
      Scan->ci[i] = BSTRM_ONE_BYTE( );
      cb = BSTRM_ONE_BYTE( );
      Scan->td[i] = HI15(cb);
      Scan->ta[i] = LO15(cb);
   }

      // Collect the additional scan parameters Ss, Se, Ah/Al.
   Scan->SSS = BSTRM_ONE_BYTE( );
   Scan->SSE = BSTRM_ONE_BYTE( );
   n = BSTRM_ONE_BYTE( );
   Scan->SAH = HI15(n);
   Scan->SAL = LO15(n);

   return( 0 );
}

/*******************************************************/

#if 0
static BYTE JPG_Get_DAC( JPG_INFO *Info )
{
   INT Length;

   Length = BSTRM_ONE_WORD( );
   if ( Length<2 ) return( -1 );
   Length -= 2;

   while ( Length>0 )
   {
      INT Index, Val;
      Index = BSTRM_ONE_BYTE( );
      Val = BSTRM_ONE_BYTE( );
      Length -= 2;

      if ( Index<0 || Index>=(2*NUM_ARITH_TBLS) )
         return( -1 );
      if ( Index>=NUM_ARITH_TBLS )
      {        // define AC table
         Info->arith_ac_K[Index-NUM_ARITH_TBLS] = (BYTE)Val;
      }
      else
      {			// define DC table
         Info->arith_dc_L[Index] = (BYTE) ( Val&0x0F );
         Info->arith_dc_U[Index] = (BYTE) ( Val>>4 );
         if ( Info->arith_dc_L[Index] > Info->arith_dc_U[Index])
            return( -1 );  // bad DAC value
      }
   }

   return( 0 );
}
#endif

static BYTE JPG_Get_DQT( JPG_INFO *Info )
{
   INT Length;

   Length = BSTRM_ONE_WORD( );
   if ( Length<2 ) return( -1 );
   Length -= 2;

   while( Length>0 )
   {
      INT n, Prec, Index, i;
      INT *Quant_Ptr;

      n = BSTRM_ONE_BYTE( );
      if ( n==END_QUANTIZATION_TABLE ) break;   // !!!

      Prec = HI15(n);
      Index = LO15(n);

      if ( Index>=MAX_DEVS ) return( 1 );
      if ( Info->QuantMatrices[Index]==NULL )
         Info->QuantMatrices[Index] = New_Fatal_Object( 64, INT );
      Quant_Ptr = Info->QuantMatrices[Index];
      for( i=0; i<64; ++i )
      {
         USHORT Tmp;
         if ( Prec ) Tmp = BSTRM_ONE_WORD( );  // Word quantization
         else Tmp = BSTRM_ONE_BYTE( );
         if ( !Tmp )
         {
            if ( i ) Tmp = (USHORT)Quant_Ptr[ JPG_Natural_Order[i-1] ];
            else Tmp = 16;
         }
         Quant_Ptr[ JPG_Natural_Order[i] ] = Tmp;
      }
      Length -= 64 + 1;
      if ( Prec ) Length -= 64;
   }
#if 0
      // rewind one byte (0xff)
   BSTRM_Ptr--;
   BSTRM_Bit_Pos -= 8;
   BSTRM_Buf32 >>= 8;
   BSTRM_Buf32 |= BSTRM_Ptr[0]<<24;
#endif

   return( 0 );
}

static BYTE JPG_Get_COM( JPG_INFO *Info )
{
   INT Length;

   Length = BSTRM_ONE_WORD( );
   if ( Length<2 ) return( -1 );
   Length -=2;
#if 0
   fprintf( stderr, "Comments:\n" );
   while ( --Length>=0 )
      fprintf( stderr, "%c", BSTRM_ONE_BYTE( ) );
   fprintf( stderr, "\n" );
#endif
   while ( --Length>=0 ) BSTRM_ONE_BYTE( );
   return( 0 );
}

static BYTE JPG_Get_DHT( JPG_INFO *Info )
{
   INT Length;

   Length = BSTRM_ONE_WORD( );
   if ( Length<2 ) return( -1 );
   Length -= 2;
   while( 1 )
   {
      INT Index, Where;
      XHUFF *XHuff;
      DHUFF *DHuff;

      Index = BSTRM_ONE_BYTE( );
      if ( Index == END_CODE_TABLE ) break;

      Where = HI15( Index );
      Index = LO15( Index );

      XHuff = New_Fatal_Object( 1, XHUFF );
      DHuff = New_Fatal_Object( 1, DHUFF );

      JPG_ReadHuffman( XHuff, DHuff );

      if ( Where )
      {
         Info->ACXHuff[Index] = XHuff;
         Info->ACDHuff[Index] = DHuff;
         Info->Nb_AC_Tab = MAX( Info->Nb_AC_Tab, (Index+1) );
      }
      else
      {
         Info->DCXHuff[Index] = XHuff;
         Info->DCDHuff[Index] = DHuff;
         Info->Nb_DC_Tab = MAX( Info->Nb_DC_Tab, (Index+1) );
      }
   }
   BSTRM_Ptr--;
   BSTRM_Bit_Pos -= 8;
   BSTRM_Buf32 >>= 8;
   BSTRM_Buf32 |= BSTRM_Ptr[0]<<24;

   return( 0 );
}

static BYTE JPG_Get_DRI( JPG_INFO *Info )
{
   if ( BSTRM_ONE_WORD( ) != 4 ) return( 1 );
   /* Info->Resync_Interval = */ BSTRM_ONE_WORD( );
   return( 0 );
}

static void JPG_Sync_Stream( )
{
   if ( (BSTRM_Bit_Pos&0x07)!=0x00 )
   {
      BSTRM_Ptr++;
      BSTRM_Bit_Pos = (BSTRM_Bit_Pos+8) & ~0x07;
   }
   BSTRM_Buf32  = BSTRM_Ptr[0]<<24;
   BSTRM_Buf32 |= BSTRM_Ptr[1]<<16;
   BSTRM_Buf32 |= BSTRM_Ptr[2]<<8;
   BSTRM_Buf32 |= BSTRM_Ptr[3];
}

static BYTE JPG_Skip_Marker( JPG_INFO *Info )
{
   INT Length;

   Length = BSTRM_ONE_WORD( );
   if ( Length<2 ) return(-1);
   Length -= 2;

//   fprintf( stderr, "Skipping %d bytes for marker 0x%x\n", Length, Info->Marker&0xFF );
#if 0
   JPG_Sync_Stream( );   
   BSTRM_Ptr += Length;
   BSTRM_Bit_Pos = (BSTRM_Bit_Pos+Length*8) & ~0x07;
   if ( BSTRM_Bit_Pos<=BSTRM_Cur->Available_Bits-32 )
      JPG_Sync_Stream( );
   else BSTRM_Buf32 = 0x00;
#endif
   while( --Length>=0 ) BSTRM_ONE_BYTE( );
   return( 0 );
}

/*******************************************************/

static JPG_MARKER JPG_Read_Markers( JPG_INFO *Info, JPG_SCAN *Scan )
{
   int S;
   S = BSTRM_Bit_Pos;

   while( 1 )
   {
      if ( JPG_Next_Marker( Info )==0 ) return( M_ERROR );

      switch( Info->Marker )
      {
         case M_SOI:
            Info->End_Of_Image = FALSE;
            Info->Image_Sequence++;
            // if ( JPG_Get_SOI( Info )!=0 ) return( M_ERROR );
         break;
         case M_EOI:
            Info->Marker = 0; // processed the marker
            Info->End_Of_Image = TRUE;  // finished.
            return( M_EOI );   // ok for markers
         break;

         case M_SOS:
            // Info->Resync_Count = 0;
            if ( JPG_Get_SOS( Scan )!=0 ) return( M_ERROR );

               // finish setup

            Check_Scan( Info, Scan );

            if ( Info->Global_Height )
               Info->Nb_MDU = Info->MDUWide*Info->MDUHigh;
            else Info->Nb_MDU = -1;

            memset( Scan->Last_DC, 0, MAX_SOURCES*sizeof( INT ) ); // ResetCodec( );
            Info->Marker = 0; // processed the marker

            return( M_SOS );   // ok for markers
         break;

         case M_DQT:
            if ( JPG_Get_DQT( Info )!=0 ) return( M_ERROR );
         break;

         case M_DRI:
            if ( JPG_Get_DRI( Info )!=0 ) return( M_ERROR );
         break;

         case M_DNL:
            // if ( JPG_Get_DNL( Info, Scan )!=0 ) return( M_ERROR );
            Exit_Upon_Error( "-M_DNL- dynamic resizing unsupported" );
         break;


            // Frame style Markers

         case M_DHT:
            if ( JPG_Get_DHT( Info )!=0 ) return( M_ERROR );
         break;

         case M_DAC:
            Exit_Upon_Error( "-M_DAC- Arithmetic coding unsupported" );
           // if ( JPG_Get_DAC( Info )!=0 ) return( M_ERROR );
         break;

         case M_SOF0:   // baseline
         case M_SOF1:   // Extended sequential, Huffman
         case M_SOF3:      // Lossless, Huffman
            Info->Type = LO15( Info->Marker );
            if ( JPG_Get_SOF( Info ) ) return( M_ERROR );
         break;

         case M_JPG:       // Reserved for JPEG extensions
         case M_SOF9:      // Extended sequential, arithmetic
         case M_SOF10:     // Progressive, arithmetic
         case M_SOF11:     // Lossless, arithmetic
         case M_SOF13:     // Differential sequential, arithmetic
         case M_SOF14:     // Differential progressive, arithmetic
         case M_SOF15:     // Differential lossless, arithmetic
            Exit_Upon_Error( "-JPEG- Arithmetic coding unsupported" );
         break;

         case M_SOF2:      // Progressive, Huffman
         case M_SOF5:      // Differential sequential, Huffman
         case M_SOF6:      // Differential progressive, Huffman
         case M_SOF7:      // Differential lossless, Huffman
            Exit_Upon_Error( "-JPEG- Frame type 0x%x unsupported",
               LO15( Info->Marker ) );
         break;

            // Application specific Markers
         case M_APP0:
         case M_APP1:
         case M_APP2:
         case M_APP3:
         case M_APP4:
         case M_APP5:
         case M_APP6:
         case M_APP7:
         case M_APP8:
         case M_APP9:
         case M_APP10:
         case M_APP11:
         case M_APP12:
         case M_APP13:
         case M_APP14:
         case M_APP15:
            JPG_Skip_Marker( Info );
         break;

            // JPEG specific Markers

         case M_COM:
            if ( JPG_Get_COM( Info )!=0 ) return( M_ERROR );
         break;

         case M_TEM:    // parameterless markers
         case M_RST0:
         case M_RST1:
         case M_RST2:
         case M_RST3:
         case M_RST4:
         case M_RST5:
         case M_RST6:
         case M_RST7:
            JPG_Skip_Marker( Info );
         break;

         default:
//            fprintf( stderr, "Unknown marker 0x%x\n", Info->Marker&0xFF );
            JPG_Skip_Marker( Info );
            return( M_EOI );
         break;
      }
   }
   return( M_VOID );  // we shouldn't get here
}

/*******************************************************/

static void I_Quantize_Zigzag( FLT *Out, INT *In, INT *Quant )
{
   INT i;
   for( i=0; i<BLOCKSIZE; ++i )
      Out[i] = 1.0f*In[ JPG_Zigzag_Index[i] ]*Quant[i];
}

/*******************************************************/
/*******************************************************/

static void JPG_Decode_Scan( JPG_INFO *Info, JPG_SCAN *Scan )
{
   INT In[BLOCKSIZE];
   INT i, j;
   FLT IDCTShift;
   MDU_BLOCK *Cur_Blocks;

   if ( Info->Data_Precision>8 ) IDCTShift = 2048.0;
   else IDCTShift = 128.0;

   Cur_Blocks = Info->Blocks;
   i = 0;
   while( 1 )
   {
      if ( Info->Nb_MDU>=0 )
         if ( Info->Cur_MDU>=Info->Nb_MDU ) 
            break;

      for( j=0; j<Scan->Nb_Comp; ++j )
      {
         XHUFF *XHuff_DC = Info->DCXHuff[Scan->td[j]];
         DHUFF *DHuff_DC = Info->DCDHuff[Scan->td[j]];
         XHUFF *XHuff_AC = Info->ACXHuff[Scan->ta[j]];
         DHUFF *DHuff_AC = Info->ACDHuff[Scan->ta[j]];
         INT   *Matrix = Info->QuantMatrices[ Info->tq[ Scan->ci[j]] ];
         INT    y, x, ny, nx;

         nx = Info->xf[ Scan->ci[j] ];
         ny = Info->yf[ Scan->ci[j] ];

         Cur_Blocks->Comp++;
         for( y=0; y<ny; ++y )
            for( x=0; x<nx; ++x )
            {
               FLT Out[BLOCKSIZE];
               In[0] = Scan->Last_DC[j];
               JPG_Decode_DC( In, XHuff_DC, DHuff_DC );
               Scan->Last_DC[j] = In[0];
               JPG_Decode_AC( In, XHuff_AC, DHuff_AC );
               I_Quantize_Zigzag( Out, In, Matrix );
               Do_Chen_IDCT( Out );
               JPG_Write_Matrix( Info, Out, Cur_Blocks, 
                  x, y, nx, ny, IDCTShift );
            }
      }
      Cur_Blocks++;
//      fprintf( stderr, "Cur=%d\n", Info->Cur_MDU );
      Info->Cur_MDU++;
   }
   JPG_Sync_Stream( );  // get rid of residual bits
}

// static void JPG_Lossless_Decode_Scan() { }

/*******************************************************/

EXTERN void *JPG_Decode( JPG_INFO *Info, INT *Width, INT *Height )
{
   JPG_SCAN Scan;

   JPG_Make_Scan( &Scan );

   while( 1 )
   {
      Info->Cur_MDU = 0;
      switch( JPG_Read_Markers( Info, &Scan ) ) // Type
      {
         case M_EOI:
            goto Finished;
         break;

         case M_SOS:
            if ( Info->Nb_MDU>=0 )
               if ( Info->Cur_MDU>=Info->Nb_MDU )  // all decoded
                  goto Finished;
            if ( Info->Blocks==NULL ) JPG_Allocate_MDU_Blocks( Info );
            memset( Scan.Last_DC, 0, MAX_SOURCES*sizeof( INT ) );
//            if ( Info->Type==3 ) JPG_Lossless_Decode_Scan( );
//            else
            JPG_Decode_Scan( Info, &Scan );
         break;

         case M_ERROR: default: goto Failed; break;
      }
   }
Finished:
   *Width  = Info->Global_Width;
   *Height = Info->Global_Height;
   JPG_Clean_Up_Info( Info );
   return( Info->Dst );

Failed:
   *Width = *Height = 0;
   JPG_Clean_Up_Info( Info );
   return( NULL );
}

/*******************************************************/

