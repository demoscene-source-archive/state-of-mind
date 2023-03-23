/************************************************
 *               JPEG loader                    *
 *                                              *
 * Skal 98                                      *
 ************************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "btm.h"
#include "truecol.h"
#include "bit_strm.h"
#include "./jpeg.h"
#include "./tables.h"

// #define USE_Q  30

/*******************************************************/

EXTERN void JPG_Make_Scan( JPG_SCAN *Scan )
{
   INT i;

   Mem_Clear( Scan );
   Scan->Nb_Comp = 0;
   for( i=0; i<MAX_SOURCES; ++i )
   {
      Scan->ta[i] = 0;
      Scan->td[i] = 0;
   }
   Scan->SSS = 0;           // predictor type
   Scan->SSE = BLOCKSIZE-1; // point transform
   Scan->SAH = 0;
   Scan->SAL = 0;
}

EXTERN void JPG_Clean_Up_Info( JPG_INFO *Info )
{
   INT i;
   for( i=0; i<MAX_DEVS; ++i )
   {
      M_Free( Info->ACDHuff[i] );
      M_Free( Info->ACXHuff[i] );
      M_Free( Info->DCDHuff[i] );
      M_Free( Info->DCXHuff[i] );
   }
#ifdef USE_Q
   M_Free( Info->QuantMatrices[0] );
   M_Free( Info->QuantMatrices[1] );
#endif
   M_Free( Info->Base );
   M_Free( Info->Blocks );
}

/*******************************************************/

EXTERN void JPG_Init_Info( JPG_INFO *Info )
{
      // Image infos

   Mem_Clear( Info );

//   Info->JpegMode = 0;
//   Info->Jfif = 0;
   Info->Image_Sequence = -1;
   Info->Nb_MDU = 0;
   Info->Cur_MDU = 0;
   Info->NbQuantMatrices = 2;
#ifndef USE_Q
   Info->QuantMatrices[0] = LuminanceQuantization;
   Info->QuantMatrices[1] = ChrominanceQuantization;
#else
   {
      INT i;
      Info->QuantMatrices[0] = New_Fatal_Object( 64, INT );
      Info->QuantMatrices[1] = New_Fatal_Object( 64, INT );
      for( i=0; i<64; ++i )
      {
         INT C;
         C = LuminanceQuantization[i]*USE_Q/50;
         if ( C>255 ) C = 255;
         Info->QuantMatrices[0][i] = C;
         C = ChrominanceQuantization[i]*USE_Q/50;
         if ( C>255 ) C = 255;
         Info->QuantMatrices[1][i] = C;
      }
   }
#endif
   Info->Nb_AC_Tab = 0;
   Info->Nb_DC_Tab = 0;

      // Frame infos

   Info->Type = 0;
   Info->Q = 0;
   Info->Global_Height = 0;
   Info->Global_Width = 0;
   Info->Data_Precision = 8;
   Info->Scale = 1.0*( 1<<(14-8) );
   Info->Global_Nb_Comp = 0;
//   Info->Resync_Interval = 0;
   memset( Info->cn, 0, MAX_COMPONENTS*sizeof( INT ) );
   memset( Info->xf, 0, MAX_COMPONENTS*sizeof( INT ) );
   memset( Info->yf, 0, MAX_COMPONENTS*sizeof( INT ) );
   memset( Info->tq, 0, MAX_COMPONENTS*sizeof( INT ) );
   memset( Info->Width, 0, MAX_COMPONENTS*sizeof( INT ) );
   memset( Info->Height, 0, MAX_COMPONENTS*sizeof( INT ) );

   Info->Blocks = NULL;
   Info->Base = NULL;
   Info->Dst = NULL;
   Info->Size = Info->Unit = 0;
   Info->Format = 0x3888;
   Info->Quantum = 3;
}

/*******************************************************/

EXTERN BIT_STRM *JPEG_Load_Bitstream( char *Name )
{
   BIT_STRM *Bit_Strm;

   Bit_Strm = BSTRM_Init( NULL, Name );
   if ( Bit_Strm==NULL ) return( NULL );
   BSTRM_DO_INIT( Bit_Strm );    // F_CLOSE( In ) is done here
   return( Bit_Strm );
}

EXTERN BITMAP_16 *JPEG_Decode_Bitstream( BIT_STRM *Bit_Strm, INT *Nb_Comp, USHORT Format )
{
   JPG_INFO Info;
   BITMAP_16 *New;

   New = NULL;
   BSTRM_Select( Bit_Strm );

   JPG_Init_Info( &Info );
   Info.Format = Format;

   New = New_Fatal_Object( 1, BITMAP_16 );
   New->Bits = (void*)JPG_Decode( &Info, &New->Width, &New->Height );
   BSTRM_UN_SELECT( Bit_Strm );
   BSTRM_Destroy( Bit_Strm );
   M_Free( Bit_Strm );
   if ( New->Bits==NULL ) goto Failed;
   New->Size = Info.Size;	// New->Width*New->Height * 3;
   New->Format = Info.Format;  // 0x3888

   if ( Nb_Comp!=NULL ) *Nb_Comp = Info.Global_Nb_Comp;

   return( New ); 

Failed:
   if ( Bit_Strm!=NULL ) { 
      BSTRM_Destroy( Bit_Strm ); 
      M_Free( Bit_Strm );
   }
   if ( New!=NULL ) { M_Free( New->Bits ); M_Free( New ); }
   return( NULL );
}

/*******************************************************/

EXTERN BITMAP_16 *Load_Raw_JPEG( char *Name, INT *Nb_Comp, USHORT Format )
{
   BIT_STRM *Bit_Strm;
   BITMAP_16 *Btm;

   Bit_Strm = JPEG_Load_Bitstream( Name );
   if ( Bit_Strm==NULL ) return( NULL );
   Btm = JPEG_Decode_Bitstream( Bit_Strm, Nb_Comp, Format );
   return( Btm );   
}   

/*******************************************************/
