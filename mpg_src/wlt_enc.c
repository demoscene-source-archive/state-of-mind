/***********************************************
 *        Wavelet encoder (DAUB4)              *
 * Skal 98                                     *
 ***********************************************/

#include "drv_io.h"
#include "wlt.h"
#include "indy.h"

extern double fabs( double );
extern double log( double );

/*******************************************************/

EXTERN void Do_Daub4( FLT *Dst, FLT *Src, INT Len )
{
   INT nh, j;
   double Tmp;

   if ( Len<4 ) return;
   nh = Len>>1;

   for( j=0; j<=Len-4; j+=2 )
   {
      Tmp  = WLT_Daub4[0]*Src[j]   + WLT_Daub4[1]*Src[j+1];
      Tmp += WLT_Daub4[2]*Src[j+2] + WLT_Daub4[3]*Src[j+3];
      Dst[0] = (FLT)Tmp;

      Tmp  = WLT_Daub4[3]*Src[j]   - WLT_Daub4[2]*Src[j+1];
      Tmp += WLT_Daub4[1]*Src[j+2] - WLT_Daub4[0]*Src[j+3];
      Dst[nh] = (FLT)Tmp;

      Dst++;
   }
   Tmp  = WLT_Daub4[0]*Src[Len-2] + WLT_Daub4[1]*Src[Len-1];
   Tmp += WLT_Daub4[2]*Src[0]     + WLT_Daub4[3]*Src[1];
   Dst[0] = (FLT)Tmp;

   Tmp  = WLT_Daub4[3]*Src[Len-2] - WLT_Daub4[2]*Src[Len-1];
   Tmp += WLT_Daub4[1]*Src[0]     - WLT_Daub4[0]*Src[1];
   Dst[nh] = (FLT)Tmp;
}

EXTERN FLT *Wavelet_Encoder_2D( FLT *Buf1, FLT *Buf2, FLT *Src, 
   INT N, INT M )
{
   INT i, j, k;

      // Size MUST be a power of two
      // Src: array [N*M]
      // Buf1: array [max(N,M)]
      // Buf2: array [max(N,M)]

   for( j=0; j<M; ++j )
   {
      FLT *Row;
      Row = &Src[j*N];
      for( i=N; i>=4; i>>=1 )
      {
         Do_Daub4( Buf1, Row, i );
         memcpy( Row, Buf1, i*sizeof( Src[0] ) );
      }
   }
   for( i=0; i<N; ++i )
   {
      for( j=M; j>=4; j>>=1 )
      {
         for( k=0; k<j; ++k ) Buf1[k] = Src[ i + k*N ];
         Do_Daub4( Buf2, Buf1, j );
         for( k=0; k<j; ++k ) Src[ i + k*N ] = Buf2[k];
      }
   }
   return( Src );
}

EXTERN FLT *Wavelet_Encode_Stream( WLT_STREAM *Strm )
{
   return( Wavelet_Encoder_2D( Strm->Buf2, Strm->Buf3, Strm->Main_Buffer,
      Strm->BufX, Strm->BufY ) );
}

/*******************************************************/

#ifndef WLT_2D

EXTERN FLT *Wavelet_Encoder( FLT *Dst, FLT *Src, INT Size )
{
   INT n;

      // Size MUST be a power of two

   if ( Dst==NULL )
   {
      Dst = New_Object( Size, FLT );
      if ( Dst==NULL ) return( NULL );
   }
   for( n=Size; n>=4; n>>=1 )
   {
      Do_Daub4( Dst, Src, n );
      memcpy( Src, Dst, n*sizeof( Src[0] ) );
   }
   return( Src );
}

#endif   // !WLT_2D

/*******************************************************/

EXTERN FLT Decimate_Wavelet( FLT *C, INT Size, FLT Thresh )
{
   FLT Max, Percent;
   INT i, Len, Ok;

   Max = 0.0;
   for( i=0; i<Size; ++i )
      if ( (FLT)fabs( (double)C[i] )>Max )
         Max = (FLT)fabs( C[i] );

   if ( Max==0.0 )
      return( 0 );

   Thresh *= (FLT)log(Max);
//   Thresh *= Max;

   Len = 0; Ok = 0;
   for( i=0; i<Size; ++i )
   {
      if ( C[i]==0.0 ) continue;
      Ok++;
      if ( log( fabs( C[i] ) )<Thresh )
      // if ( fabs( C[i] )<Thresh )
         C[i] = 0.0;
      else Len++;
   }

   Percent = 100.0f*Len/Ok;
   return( Percent );
}

/*******************************************************/

EXTERN USHORT Wavelet_Flt_To_16bits( USHORT *Dst, FLT *Src, 
   FLT *Max, FLT *Min, INT Size )
{
   INT i;
   FLT dT, T;
   USHORT Key;

   *Max = -1.0e30f;
   *Min = 1.0e30f;
   for( i=0; i<Size; ++i )
   {
      if ( (*Max)<Src[i] ) *Max = Src[i];
      if ( (*Min)>Src[i] ) *Min = Src[i];
   }

   dT = ( (*Max)-(*Min) );
   if ( dT<=0.0 ) { memset( Dst, 0, Size ); return( 0 ); }

#ifdef WLT_LOG
   dT = 1.0/dT;
   for( i=0; i<Size; ++i )
   {
      T = ( Src[i]-(*Min) )*dT;
      Dst[i] = (USHORT)( 65535.9/log(2.0)*log( 1.0+T ) );
   }
   T = -(*Min)*dT;
   Key = (USHORT)( 65535.9/log(2.0)*log( 1.0+T ) );
#else
   dT = 65535.9f/dT;
   for( i=0; i<Size; ++i )
   {
      T = ( Src[i]-(*Min) )*dT;
      Dst[i] = (USHORT)(T);    // Don't use floor()!! It bugs!!
   }
   T = -(*Min)*dT;
   Key = (USHORT)(T);    // Don't use floor()!! It bugs!!
#endif
   return( Key );
}

/*******************************************************/

static INT Output_Data( USHORT **Dst, USHORT *Src, USHORT Len )
{
   USHORT i;

   while ( Len>0x7FFF )
   {
      Output_Data( Dst, Src, 0x7FFF );
      Src += 0x7FFF;
      Len -= 0x7FFF;
   }
   (*Dst)[0] = (USHORT)Len | 0x8000;
   (*Dst) += 1;
   for( i=0; i<Len; ++i )
      (*Dst)[i] = Src[i];
   (*Dst) += Len;
   return( (Len+1)*sizeof(USHORT) );
}

static INT Output_Key( USHORT **Dst, USHORT Len )
{
   while ( Len>0x7FFF )
   {
      Output_Key( Dst, 0x7FFF );
      Len -= 0x7FFF;
   }

   (*Dst)[0] = Len;
   (*Dst) += 1;
   return( sizeof(USHORT) );
}

EXTERN INT Wavelet_To_RLE( USHORT *Dst, USHORT *Src, 
   USHORT Key, FLT Max, FLT Min, INT Size )
{
   INT i, Len, Block;
   USHORT Count;
   USHORT *Ptr, *Base_Dst;

   Base_Dst = Dst;
   Dst += 2;      // space for final length

   *Dst++ = Indy_s( Key );
   *(FLT *)Dst = Indy_f( Max );
   Dst += 2;
   *(FLT *)Dst = Indy_f( Min );
   Dst += 2;

   Len = 10; 
   Count = 0;
   Block = -1;

   Ptr = NULL;
   for( i=0; i<Size; ++i )
   {
      if ( Src[i]==Key )
      {
         if ( Block==-1 ) {
            Block = 0; 
            Count = 1;  
            Ptr = Src;
            continue;
         }   // Key block starts
         else if ( Block==1 )
         {
            Len += Output_Data( &Dst, Ptr, Count );
            Block = 0;
            Count = 1;
            Ptr = Src+i;
            continue;
         }
         else Count++;
      }
      else
      {
         if ( Block==-1 ) {
            Block = 1; 
            Count = 1;  
            Ptr = Src;
            continue;
         }   // Data block starts
         else if ( Block==0 )
         {
            Len += Output_Key( &Dst, Count );
            Block = 1;
            Count = 1;
            Ptr = Src+i;
            continue;
         }
         else Count++;
      }
   }
   if ( (Count>0) && (Block!=-1) )
   {
      if ( Block==1 ) Len += Output_Data( &Dst, Ptr, Count );
      else Len += Output_Key( &Dst, Count );
   }
   *Dst = 0;      // EOF
   Len += sizeof(USHORT);
   Len += 2*sizeof( USHORT );

   *(INT *)Base_Dst = Indy_l( Len );   // write total length

   return( Len );
}

/*******************************************************/

EXTERN INT Save_Wavelet_Stream( WLT_STREAM *Strm, STRING Name )
{
   FILE *Out;
   INT i, Len;

   if ( Strm==NULL ) return( 0 );
   Out = Access_File( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( -1 );
   fprintf( Out, "Wlt %f\n%d %d %d %d",   // <= !!MAGIC_WLT
      WLT_VERSION,
      Strm->Nb_Frames, Strm->SizeX, Strm->SizeY, Strm->Nb_Col );
   if ( Strm->Nb_Col ) 
      Compress_Write( Strm->CMap, Out, Strm->Nb_Col*3, 8 );
   for( i=0; i<Strm->Nb_Frames; ++i )
   {
      if ( Strm->RLE[i] == NULL )
         Exit_Upon_Error( "Bad RLE array" );
      Len = Indy_l( *(INT *)Strm->RLE[i] );
      Save_LZW_Block_II( (PIXEL *)Strm->RLE[i], Out, Len, 8 );
      // fprintf( stderr, "Frame:%d RLE_Len:%d\n", i+1, Len );
   }
   fclose( Out );

   Out = Access_File( Name, READ_SWITCH );
   fseek( Out, 0, SEEK_END );
   Len = ftell( Out );
   fclose( Out );
      
   return( Len );
}

/*******************************************************/

#ifdef WLT_8BITS

EXTERN void Wavelet_Flt_To_8bits( PIXEL *Dst, FLT *Src, 
   FLT *Max, FLT *Min, INT Size )
{
   INT i;
   FLT dT, T;

   *Max = -1.0e30;
   *Min = 1.0e30;
   for( i=0; i<Size; ++i )
   {
      if ( (*Max)<Src[i] ) *Max = Src[i];
      if ( (*Min)>Src[i] ) *Min = Src[i];
   }

   dT = ( (*Max)-(*Min) );
   if ( dT<=0.0 ) { memset( Dst, 0, Size ); return; }

#ifdef WLT_LOG
   dT = 1.0/dT;
   for( i=0; i<Size; ++i )
   {
      T = ( Src[i]-(*Min) )*dT;
      Dst[i] = (PIXEL)( 255.9/log(2.0)*log( 1.0+T ) );
   }
   T = -(*Min)*dT;
   Key = (PIXEL)( 255.9/log(2.0)*log( 1.0+T ) );
#else
   dT = 256.0/dT;
   for( i=0; i<Size; ++i )
   {
      T = ( Src[i]-(*Min) )*dT;
      Dst[i] = (PIXEL)(T-.5);    // Don't use floor()!! It bugs!!
   }
   T = -(*Min)*dT;
   Key = (PIXEL)(T-.5);    // Don't use floor()!! It bugs!!
#endif
}

#endif   // WLT_8BITS

/*******************************************************/

#ifdef WLT_OLD_PAK

EXTERN INT Pack_Wavelet( UINT *Dst, FLT *C, INT Size, FLT Thresh )
{
   FLT Max;
   INT i, Len;

   Max = 0.0;
   for( i=0; i<Size; ++i )
      if ( (FLT)fabs( (double)C[i] )>Max ) Max = fabs( C[i] );

   *(FLT *)(&Dst[1]) = Max;

   if ( Max==0.0 )
   {
      Dst[0] = 0;
      return( 2 );
   }
   Thresh = log(Max)*Thresh;

   Len = 0;
   for( i=0; i<Size; ++i )
   {
      if ( C[i]==0.0 ) continue;
      if ( log( fabs( C[i] ) )>=Thresh )
      {
         char P;
         P = (char)floor( 255.9*C[i]/Max );
         Dst[2+Len] = (i<<8) | P;
         Len++;
      }
   }
   Dst[0] = Len;
   fprintf( stderr, "Thresh:%e. Nb good coeffs:%d\n", Thresh, Len );
   return( Len+2 );
}

EXTERN INT Pack_Wavelet_II( UINT *Dst, FLT *C, INT Size, FLT Thresh )
{
   FLT Max;
   INT i, Len;

   Max = 0.0;
   for( i=0; i<Size; ++i )
      if ( (FLT)fabs( (double)C[i] )>Max ) Max = fabs( C[i] );

   *(FLT *)(&Dst[1]) = Max;

   if ( Max==0.0 )
   {
      Dst[0] = 0;
      return( 2 );
   }
   Thresh = log(Max)*Thresh;

   Len = 0;
   for( i=0; i<Size; ++i )
   {
      if ( C[i]==0.0 ) continue;
      if ( log( fabs( C[i] ) )>=Thresh )
      {
         char P;
         P = (char)floor( 255.9*C[i]/Max );
         Dst[2+Len] = (i<<8) | P;
         Len++;
      }
   }
   Dst[0] = Len;
   fprintf( stderr, "Thresh:%e. Nb good coeffs:%d\n", Thresh, Len );
   return( Len+2 );
}

#endif   // WLT_OLD_PAK

/*******************************************************/
