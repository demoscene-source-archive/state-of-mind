/***********************************************
 *        Wavelet decoder (DAUB4)              *
 * Skal 98                                     *
 ***********************************************/

#include "wlt.h"
#include "indy.h"

extern double log( double );
extern double exp( double );

EXTERN double WLT_Daub4[4] = {
   0.4829629131445341,
   0.8365163037378079,
   0.2241438680420134,
  -0.1294095225512604
};

/*******************************************************/

#ifdef UNIX

EXTERN void Do_Inv_Daub4( FLT *Dst, FLT *Src, INT Len )
{
   INT nh, i;
   double Tmp;

   if ( Len<4 ) return;
   nh = (Len>>1);

   Tmp  = WLT_Daub4[2]*Src[nh-1] + WLT_Daub4[1]*Src[Len-1];
   Tmp += WLT_Daub4[0]*Src[0]    + WLT_Daub4[3]*Src[nh];
   *Dst++ = (FLT)Tmp;

   Tmp  = WLT_Daub4[3]*Src[nh-1] - WLT_Daub4[0]*Src[Len-1];
   Tmp += WLT_Daub4[1]*Src[0]    - WLT_Daub4[2]*Src[nh];
   *Dst++ = (FLT)Tmp;

   for( i=nh-1; i>0; i-- )
   {
      Tmp  = WLT_Daub4[2]*Src[0]   + WLT_Daub4[1]*Src[nh];
      Tmp += WLT_Daub4[0]*Src[1] + WLT_Daub4[3]*Src[1+nh];
      *Dst++ = (FLT)Tmp;

      Tmp  = WLT_Daub4[3]*Src[0]   - WLT_Daub4[0]*Src[nh];
      Tmp += WLT_Daub4[1]*Src[1] - WLT_Daub4[2]*Src[1+nh];
      *Dst++ = (FLT)Tmp;
      Src++;
   }
}
#endif

/*******************************************************/

EXTERN FLT *Wavelet_Decoder_2D( FLT *Buf1, FLT *Buf2, FLT *Src, 
   INT N, INT M )
{
   INT i, j, k;

      // Size MUST be a power of two
      // Src: array [N*M]
      // Buf1: array [max(N,M)]
      // Buf2: array [max(N,M)]

   for( i=0; i<N; ++i )
   {
      for( j=4; j<=M; j<<=1 )
      {
         for( k=0; k<j; ++k ) Buf1[k] = Src[ i + k*N ];
         Do_Inv_Daub4( Buf2, Buf1, j );
         for( k=0; k<j; ++k ) Src[ i+k*N ] = Buf2[k];
      }
   }
   for( j=0; j<M; ++j )
   {
      FLT *Row;
      Row = &Src[j*N];
      for( i=4; i<=N; i<<=1 )
      {
         Do_Inv_Daub4( Buf1, Row, i );
         memcpy( Row, Buf1, i*sizeof( Src[0] ) );
      }
   }
   return( Src );
}

EXTERN FLT *Wavelet_Decode_Stream( WLT_STREAM *Strm )
{
   return( Wavelet_Decoder_2D( Strm->Buf2, Strm->Buf3, Strm->Main_Buffer,
      Strm->BufX, Strm->BufY ) );
}

/*******************************************************/

EXTERN void Wavelet_16bits_To_Flt( FLT *Dst, USHORT *Src, 
   FLT Max, FLT Min, INT Size )
{
   INT i;
   FLT dT;

#ifdef WLT_LOG
   dT = (Max-Min);
   for( i=0; i<Size; ++i )
      Dst[i] = Min + dT*(exp( log(2.0)/256.0*(FLT)(Src[i]) )-1.0 );
#else
   dT = (Max-Min)/(65535.9f);
   for( i=0; i<Size; ++i )
      Dst[i] = Min + dT*(FLT)( Src[i] );
#endif
}

/*******************************************************/

EXTERN INT Wavelet_Decode_RLE( USHORT *Dst, USHORT *Src, 
   FLT *Max, FLT *Min )
{
   USHORT Key, Len;
   INT i, Size;

   Size = Indy_l( *(INT *)Src );  // expected size 
   Src += 2;

   Key = Indy_s( *Src++ );
   *Max = Indy_f( *(FLT *)Src );
   Src += 2;
   *Min = Indy_f( *(FLT *)Src );
   Src += 2;
   while( 1 )
   {
      Len = Indy_s( *Src++ );
      if ( Len==0 ) break;
      if ( (Len&0x8000)==0x8000 )
      {
         for( i=Len&0x7FFF; i>0; --i ) *Dst++ = Indy_s( *Src++ );
      }
      else
      {
         for( i=Len;i>0; --i ) *Dst++ = Key;
      }
   }
   return( Size - 4 - 2*4 - 2 );
}

/*******************************************************/

#ifndef WLT_2D

EXTERN FLT *Wavelet_Decoder( FLT *Dst, FLT *Src, INT Size )
{
   INT n;

      // Size MUST be a power of two

   if ( Dst==NULL )
   {
      Dst = New_Object( Size, FLT );
      if ( Dst==NULL ) return( NULL );
   }
   for( n=4; n<=Size; n<<=1 ) 
   {
      Do_Inv_Daub4( Dst, Src, n );
      memcpy( Src, Dst, n*sizeof( Src[0] ) );
   }

   return( Src );
}

#endif   // WLT_2D

/*******************************************************/

#ifdef WLT_8BITS

EXTERN void Wavelet_8bits_To_Flt( FLT *Dst, PIXEL *Src, 
   FLT Max, FLT Min, INT Size )
{
   INT i;
   FLT dT;

#ifdef WLT_LOG
   dT = (Max-Min);
   for( i=0; i<Size; ++i )
      Dst[i] = Min + dT*(exp( log(2.0)/65536.0*(FLT)(Src[i]) )-1.0 );
#else
   dT = (Max-Min)/(65535.9);
   for( i=0; i<Size; ++i )
      Dst[i] = Min + dT*(FLT)( Src[i] );
#endif
}

#endif   // WLT_8BITS

/*******************************************************/


