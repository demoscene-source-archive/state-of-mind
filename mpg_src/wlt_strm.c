/***********************************************
 *        Wavelet streams                      *
 * Skal 98                                     *
 ***********************************************/

#include "drv_io.h"
#include "wlt.h"

/*******************************************************/

EXTERN WLT_STREAM *Init_Wavelet_Stream( WLT_STREAM *Strm,
   INT SizeX, INT SizeY, INT Nb_Frames )
{
   INT i;

   if ( (SizeX<4) || (SizeY<4) || (Nb_Frames<0) ) return( NULL ); // error

   if ( Strm==NULL )
   {
      Strm = New_Fatal_Object( 1, WLT_STREAM );
      if ( Strm==NULL ) return( NULL );
   }
   Mem_Clear( Strm );

   Strm->RLE = New_Fatal_Object( Nb_Frames, USHORT * );
   memset( Strm->RLE, 0, Nb_Frames*sizeof( USHORT *) );
   Strm->Nb_Frames = Nb_Frames;
   Strm->SizeX = SizeX;
   Strm->SizeY = SizeY;

   for( i=1; i<16; ++i )
      if ( ( (FLT)SizeX/(FLT)(1<<i) )<=1.0 ) break;
   Strm->BufX = 1<<i;
   for( i=1; i<16; ++i )
      if ( ( (FLT)SizeY/(FLT)(1<<i) )<=1.0 ) break;
   Strm->BufY = 1<<i;

   i = Strm->BufX>Strm->BufY ? Strm->BufX : Strm->BufY;
   Strm->Buf2 = New_Fatal_Object( i*2, FLT );
   Strm->Buf3 = Strm->Buf2 + i;
   Strm->Main_Buffer = New_Fatal_Object( Strm->BufX*Strm->BufY, FLT );
   Strm->Frame = New_Fatal_Object( Strm->BufX*Strm->BufY, USHORT ); // RLE expension

   Strm->Nb_Col = 0;
   return( Strm );
}
   
EXTERN void Clear_Wavelet_Stream( WLT_STREAM *Strm )
{
   INT i;
   if ( Strm==NULL ) return;
   M_Free( Strm->Main_Buffer );
   M_Free( Strm->Buf2 );
   M_Free( Strm->Frame );
   if ( Strm->RLE!=NULL )
      for( i=0; i<Strm->Nb_Frames; ++i ) M_Free( Strm->RLE[i] );
   M_Free( Strm->RLE );
   Mem_Clear( Strm );
}

EXTERN void Destroy_Wavelet_Stream( WLT_STREAM *Strm )
{
   Clear_Wavelet_Stream( Strm );
   M_Free( Strm );
}

/*******************************************************/

EXTERN WLT_STREAM *Load_Wavelet_Stream( STRING Name )
{
   WLT_STREAM *Strm;
   FILE *In;
   INT SizeX, SizeY, Nb_Frames, Nb_Col, Version;
   INT i;

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );
   if ( fscanf( In, "Wlt %f\n%d %d %d %d",   // <= MAGIC_WLT!!
      &Version,
      &Nb_Frames, &SizeX, &SizeY, &Nb_Col ) != 5 )
      goto Failed;
   Strm = Init_Wavelet_Stream( NULL, SizeX, SizeY, Nb_Frames );
   if ( Strm==NULL ) goto Failed;

   Strm->Nb_Col = Nb_Col;
   if ( Nb_Col ) 
      F_COMPRESS_READ( Strm->CMap, In, Nb_Col*3 );
   for( i=0; i<Nb_Frames; ++i )
   {
      Strm->RLE[i] = (USHORT*)Load_LZW_Block_II( In );
      if ( Strm->RLE[i]==NULL ) goto Failed;
      // fprintf( stderr, "Frame:%d RLE_Len:%d\n", i+1, *(INT *)Strm->RLE[i] );
   }

   F_CLOSE( In );
   return( Strm );

Failed:
   if ( In!=NULL ) F_CLOSE( In );
   return( NULL );
}

/*******************************************************/

EXTERN FLT *Wavelet_Decode_Frame( WLT_STREAM *Strm, INT N )
{
   FLT Min, Max;
   INT Size;

   if ( Strm==NULL || Strm->RLE==NULL || N<0 || N>=Strm->Nb_Frames ) 
      return( NULL );

   if ( Strm->RLE[N]==NULL ) return( NULL );
   Size = Wavelet_Decode_RLE( Strm->Frame, Strm->RLE[N], &Max, &Min );
   // fprintf( stderr, "RLE size:%d Min:%f Max:%f\n", Size, Min, Max );
   Wavelet_16bits_To_Flt( Strm->Main_Buffer, Strm->Frame, 
      Max, Min, Strm->BufX*Strm->BufY );
   Wavelet_Decode_Stream( Strm );
   return( Strm->Main_Buffer );
}

/*******************************************************/
