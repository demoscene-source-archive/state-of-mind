/************************************************
 *         MP3 debugging func                   *
 * Skal 98                                      *
 ************************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "bit_strm.h"
#include "./mp3.h"

/*******************************************************/

#if (MP3_VERSION<=002)

EXTERN INT MP3_Header_Sane( MP3_HEADER *H )
{
   if ( H->Layer==0 || H->Bitrate_Index==0x0F || H->Sampling_Frequency==3 )
      return( FALSE );
   if ( H->ID==1 && H->Layer==3 && H->Protection_Bit==1 )
      return( FALSE );
   return( TRUE );
}

EXTERN void MP3_Print_Header( MP3_HEADER *H )
{
   fprintf( stderr, "Header:\n" );
   fprintf( stderr, " ID:  ........ %d\n", H->ID );
   fprintf( stderr, " Layer: ...... %d\n", H->Layer );
   fprintf( stderr, " P_Bit: ...... %d\n", H->Protection_Bit );
   fprintf( stderr, " Bitrate_I: .. %d\n", H->Bitrate_Index );
   fprintf( stderr, " Sampling_F: . %d\n", H->Sampling_Frequency );
   fprintf( stderr, " Padding: .... %d\n", H->Padding_Bit );
   fprintf( stderr, " Private: .... %d\n", H->Private_Bit );
   fprintf( stderr, " Mode: ....... %d\n", H->Mode );
   fprintf( stderr, " Mode_Ext: ... %d\n", H->Mode_Extension );
   fprintf( stderr, " Copyright: .. %d\n", H->Copyright );
   fprintf( stderr, " Original: ... %d\n", H->Original );
   fprintf( stderr, " Emphasis: ... %d\n", H->Emphasis );   
}

#endif

#if (MP3_VERSION<=001)       // debug
EXTERN void MP3_Queue_State( UINT Ret, INT How_Many )
{
   fprintf( stderr, 
      "MP3_Buf 0x%.8x: 0x%.2x %.2x (Bit_Pos:%d) How=%d => Ret=0x%.8x/0x%x\n",
      MP3_Buf, MP3_Buf[0], MP3_Buf[1], 
      MP3_Bit_Pos, How_Many, Ret, Ret & BSTRM_Mask[How_Many] );
}
#endif

/*******************************************************/

#if (BSTRM_VERSION<=001)       // debug
EXTERN void Queue_State( STRING S, UINT Ret, INT How_Many )
{
   fprintf( stderr, 
      "%s 0x%.8x: 0x%.2x %.2x %.2x %.2x  (left:%d) How=%d => Ret=0x%.8x/0x%x\n",
      S,
      BSTRM_Ptr, BSTRM_Ptr[0], BSTRM_Ptr[1], BSTRM_Ptr[2], BSTRM_Ptr[3],
      BSTRM_Bits_Left, How_Many, Ret, Ret & BSTRM_Mask[How_Many] );
}
#endif

/*******************************************************/
