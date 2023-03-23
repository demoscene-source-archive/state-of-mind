/************************************************
 *             JPEG output                      *
 *                                              *
 * Skal 98                                      *
 ************************************************/

#include "main.h"
#include "drv_io.h"
#include "truecol.h"
#include "./jpeg.h" 
#include "./ijpeg.h"
#include "./markers.h"
#include "./tables.h"
#include "./tables2.h"

extern SHORT JPG_Natural_Order[];

#define FINISH_WRITE                                           \
   { E = ftell( JPG_F_Out ); fseek( JPG_F_Out, S, SEEK_SET );  \
     fputw( (USHORT)(E-S), JPG_F_Out ); fseek( JPG_F_Out, E, SEEK_SET ); }

/*******************************************************/

static void Write_Align( )
{
   if ( JPG_Bit_Pos!=7 ) fputc( JPG_Out_Bits, JPG_F_Out );
   JPG_Out_Bits = 0x00;
   JPG_Bit_Pos = 7;
}

/*******************************************************/

static void fputw( USHORT c, FILE *Out )
{
   fputc( c>>8, Out );
   fputc( c&0xFF, Out );
}
static void fputn( BYTE c1, BYTE c2, FILE *Out )
{
   fputc( ( (c1&0x0F)<<4 ) | (c2&0x0f), Out );
} 

/*******************************************************/

static void Write_Soi( )
{
   Write_Align( );
   fputc( JPG_MARKER_MARKER, JPG_F_Out );
   fputc( JPG_MARKER_SOI, JPG_F_Out );
}

static void Write_Eoi( )
{
   Write_Align( );
   fputc( JPG_MARKER_MARKER, JPG_F_Out );
   fputc( JPG_MARKER_EOI, JPG_F_Out );
}

static void Write_Sof( JPG_INFO *Info )
{
   INT i;
   long S, E;

   Write_Align( );
   fputc( JPG_MARKER_MARKER, JPG_F_Out );
   fputc( JPG_MARKER_SOF | (Info->Type&0x0F), JPG_F_Out );
   S = ftell( JPG_F_Out );
   fputw( 0, JPG_F_Out );   // reserved for Length post-writting

   fputc( Info->Data_Precision, JPG_F_Out );   // Data precision
   fputw( (USHORT)Info->Global_Height, JPG_F_Out );
   fputw( (USHORT)Info->Global_Width, JPG_F_Out );
   fputc( Info->Global_Nb_Comp, JPG_F_Out );      // Nb_Components
   for( i=0; i<Info->Global_Nb_Comp; ++i )
   {
      INT j;

      j = Info->cn[i];
      fputc( j, JPG_F_Out );      // Component index
      fputn( (BYTE)Info->xf[j], (BYTE)Info->yf[j], JPG_F_Out );      // xf + yf
      fputc( (USHORT)Info->tq[j], JPG_F_Out );      // qt
//      fprintf( stderr, "i=%d j=%d xf=%d yf=%d tq=%d\n", i, j, Info->xf[j], Info->yf[j], Info->tq[j] );
   }
   FINISH_WRITE;
}

static void Write_Dqt( JPG_INFO *Info )
{
   INT i;
   long S, E;

   Write_Align( );
   fputc( JPG_MARKER_MARKER, JPG_F_Out );
   fputc( JPG_MARKER_DQT, JPG_F_Out );
   S = ftell( JPG_F_Out );
   fputw( 0, JPG_F_Out );   // reserved for Length post-writting

   for( i=0; i<Info->NbQuantMatrices; ++i )
   {
      INT Big, j;
      for( Big=0,j=63; j>=0; --j )
         if ( Info->QuantMatrices[i][j]>255 ) { Big = 0x10; break; }
      fputc( Big|i, JPG_F_Out );
      if ( Big )
         for( j=0; j<64; ++j ) fputw( (USHORT)Info->QuantMatrices[i][JPG_Natural_Order[j]], JPG_F_Out );
      else for( j=0; j<64; ++j ) fputc( Info->QuantMatrices[i][JPG_Natural_Order[j]], JPG_F_Out );
   }
   FINISH_WRITE;
}

/*******************************************************/

static void Write_Huffman( XHUFF *XHuff )
{
   INT i, Accum;
   for( Accum=0,i=1; i<=16; ++i )
   {
      fputc( XHuff->bits[i], JPG_F_Out );
      Accum += XHuff->bits[i];
   }
   for( i=0; i<Accum; ++i ) fputc( XHuff->huffval[i], JPG_F_Out );
}

static void Write_Dht( JPG_INFO *Info )
{
   INT i;
   long S, E;

   Write_Align( );
   fputc( JPG_MARKER_MARKER, JPG_F_Out ); 
   fputc( JPG_MARKER_DHT, JPG_F_Out );
   S = ftell( JPG_F_Out );
   fputw( 0, JPG_F_Out );   // reserved for Length post-writting

   for( i=0; i<Info->Nb_DC_Tab; ++i )
   {
      fputc( i, JPG_F_Out );
      Write_Huffman( Info->DCXHuff[i] );
   }
   for( i=0; i<Info->Nb_AC_Tab; ++i )
   {
      fputc( i | 0x10, JPG_F_Out );
      Write_Huffman( Info->ACXHuff[i] );
   }
   FINISH_WRITE;
}

static void Write_Sos( JPG_SCAN *Scan )
{
   INT i;
   long S, E;

   Write_Align( );
   fputc( JPG_MARKER_MARKER, JPG_F_Out ); 
   fputc( JPG_MARKER_SOS, JPG_F_Out );
   S = ftell( JPG_F_Out );
   fputw( 0, JPG_F_Out );   // reserved for Length post-writting

   fputc( Scan->Nb_Comp, JPG_F_Out );
   for( i=0; i<Scan->Nb_Comp; ++i )
   {
      fputc( Scan->ci[i], JPG_F_Out );
      fputn( (BYTE)Scan->td[i], (BYTE)Scan->ta[i], JPG_F_Out );
   }
   fputc( Scan->SSS, JPG_F_Out );
   fputc( Scan->SSE, JPG_F_Out );
   fputn( (BYTE)Scan->SAH, (BYTE)Scan->SAL, JPG_F_Out );

   FINISH_WRITE;
}

/*******************************************************/
/*******************************************************/

extern INT JPG_Zigzag_Index[];

static void Quantize_Zigzag( INT *Out, FLT *In, INT *Quant, INT Bound )
{
   INT i;
   for( i=0; i<BLOCKSIZE; ++i )
   {
      INT C = (INT)floor( In[i] );
      if ( C<-Bound ) C = -Bound;
      else if ( C>=Bound ) C = Bound-1;
      if ( C>0 ) C = ( C+Quant[i]/2 ) / Quant[i];
      else C = ( C-Quant[i]/2 ) / Quant[i];
      Out[ JPG_Zigzag_Index[i] ] = C;
   }
}

/*******************************************************/

   // type: bit 0-1: Nb components
   //       bit 2  : Save as raw block
   //       bit 3  : input is RGB 565

EXTERN BYTE *JPEG_Out_Raw( FILE *File, BYTE *Bits, INT Width, INT Height,
   INT Type )
{
   INT j, Last_DC[3], DCTBound;
   FLT DCTShift;
   JPG_INFO Info;
   JPG_SCAN Scan;
   EHUFF *EHuff_AC;
   EHUFF *EHuff_DC;

   JPG_Init_Info( &Info );
   JPG_Make_Scan( &Scan );   

   JPG_Out_Bits = 0x00;
   JPG_Bit_Pos = 7;

   Info.Data_Precision = 8;
   DCTShift = 128.0;   // (data precision=8)
   DCTBound = 1024;
   Info.Type = 0;    // baseline type

   Info.cn[0] = 0; 
   Info.cn[1] = 1; 
   Info.cn[2] = 2;
   Info.xf[ 0 ] = 1; Info.yf[ 0 ] = 1; 
   Info.xf[ 1 ] = 1; Info.yf[ 1 ] = 1;
   Info.xf[ 2 ] = 1; Info.yf[ 2 ] = 1;
   Info.tq[ Info.cn[0] ] = 0;  // luminance for primary
   Info.tq[ Info.cn[1] ] = 1;  // chrominance for the rest
   Info.tq[ Info.cn[2] ] = 1;  // chrominance for the rest

      // default tables

   Info.Nb_AC_Tab = 1;
   Info.ACXHuff[0] = New_Fatal_Object( 1, XHUFF );
   EHuff_AC = New_Fatal_Object( 1, EHUFF );
   JPG_Specified_Huffman( LuminanceACBits, LuminanceACValues,
      Info.ACXHuff[0], EHuff_AC );

   Info.Nb_DC_Tab = 1;
   Info.DCXHuff[0] = New_Fatal_Object( 1, XHUFF );
   EHuff_DC = New_Fatal_Object( 1, EHUFF );
   JPG_Specified_Huffman( LuminanceDCBits, LuminanceDCValues,
      Info.DCXHuff[0], EHuff_DC );

   JPG_F_Out = File;

   Info.Global_Width = Width;
   Info.Global_Height = Height;

   Info.Global_Nb_Comp = Type&0x03;  // 3 or 1
   Scan.Nb_Comp = Type&0x03;     // 3 or 1

   Scan.ci[0] = 0; Scan.ci[1] = 1; Scan.ci[2] = 2;
   Scan.ta[0] = 0; Scan.ta[1] = 0; Scan.ta[2] = 0;
   Scan.td[0] = 0; Scan.td[1] = 0; Scan.td[2] = 0;

   JPG_Out_Bits = 0x00;
   JPG_Bit_Pos = 7;

   Write_Soi( );
   Write_Sof( &Info );      // Type = 1

   if ( !(Type&0x04) )  // write full-infos
   {
      Write_Dqt( &Info );      // Write out quantization
      Write_Dht( &Info );      // and Huffman tables
   }
   Write_Sos( &Scan );

   Last_DC[0] = Last_DC[1] = Last_DC[2] = 0;

   for( j=0; j<Height; j+=8 )
   {
      INT Max_k, i;
      Max_k = Height-j; if ( Max_k>8 ) Max_k = 8;

      for( i=0; i<Width; i+=8 )
      {
         INT k, n, m, Max_n;
         FLT *R, *G, *B, In[3][64];
         BYTE *Src;

         if ( Type & 0x08 ) Src = Bits + 2*(Width*j+i);
         else Src = Bits + 3*(Width*j+i);

         Max_n = Width-i; if ( Max_n>8 ) Max_n = 8;

         R = In[0]; G = In[1]; B = In[2];
         for( k=0; k<Max_k; ++k )
         {
            if ( Type & 0x08 )
            {
               USHORT *Ptr = (USHORT*)Src;
               for( n=0; n<Max_n; ++n )
               {
                  USHORT C = *Ptr++;
                  R[n] = 1.0f*( (C&0xF800)>>8 );
                  G[n] = 1.0f*( (C&0x07E0)>>3 );
                  B[n] = 1.0f*( (C&0x001F)<<3 );
               }
               Src += 2*Width;
            }
            else
            {
               BYTE *Ptr = Src;
               for( n=0; n<Max_n; ++n )
               {
                  B[n] = 1.0f*(*Ptr++);
                  G[n] = 1.0f*(*Ptr++);
                  R[n] = 1.0f*(*Ptr++);
               }
               Src += 3*Width;
            }
            for( m=n; m<8; ++m )
            {
               R[m] = R[n-(m-n)-1];
               G[m] = G[n-(m-n)-1];
               B[m] = B[n-(m-n)-1];
            }
            R += 8; G += 8; B += 8;            
         }
         for( m=7-k; m>=0; --m )
         {
            memcpy( R+m*8, R -m*8-8, 8*sizeof( FLT ) );
            memcpy( G+m*8, G -m*8-8, 8*sizeof( FLT ) );
            memcpy( B+m*8, B -m*8-8, 8*sizeof( FLT ) );
         }

            // Ok. Do the stuff

         RGB_To_Ycc( In, In, DCTShift );  // in place replacement...

         for( m=0; m<Scan.Nb_Comp; ++m )
         {
            INT Out2[64];
            Do_Chen_DCT( In[m] );
            Quantize_Zigzag( Out2, In[m], Info.QuantMatrices[Info.tq[m]], DCTBound );
            JPG_Encode_DC( Out2[0], &Last_DC[m], EHuff_DC );
            JPG_Encode_AC( Out2, EHuff_AC );
         }
//         fprintf( stderr, "i,j=(\t%d,\t%d)   Max_k/Max_n = %d/%d       \r", i, j, Max_k, Max_n );
      }
   }

   Write_Eoi( );

   M_Free( EHuff_AC ); M_Free( EHuff_DC );
   M_Free( Info.ACXHuff[0] );
   M_Free( Info.DCXHuff[0] );

   JPG_Clean_Up_Info( &Info );
   return( Bits );
}

EXTERN BYTE *JPEG_Out( STRING Name, BYTE *Bits, 
   INT Width, INT Height, INT Type )
{
   FILE *Out;
   
   Out = fopen( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( NULL );
   Bits = JPEG_Out_Raw( Out, Bits, Width, Height, Type );
   fclose( Out );
   return( Bits );
}

/*******************************************************/
/*******************************************************/
