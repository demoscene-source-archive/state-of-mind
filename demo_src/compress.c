/***********************************************
 *       Compressed I/O utilities              *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "lzw.h"

/*******************************************************/

EXTERN int Compress_Read( BYTE *Bits, FILE *In, INT Size )
{
   INT Ret;

   Ret = fgetc( In );
   if ( Ret==0x02 )
   {
      INT i;
      i = (INT)fgetc( In );
      Ret = LZW_Decoder( Bits, In, Size, i );
      if ( !Ret ) Ret = i;
   }
   else
   {
      Size  = fgetc( In );
      Size += fgetc( In )*256;
      Size += fgetc( In )*256*256;
      Size += fgetc( In )*256*256*256;
      Ret = fread( Bits, Size, 1, In );
      if ( Ret ) Ret = 8;
   }
   return( Ret );
}

EXTERN void *Load_LZW_Block_II( FILE *In )
{
   void *Where;
   INT Ret, Size;

   if ( In==NULL ) return( NULL );
   Size  = fgetc( In );
   Size += fgetc( In )*256;
   Size += fgetc( In )*256*256;
   Size += fgetc( In )*256*256*256;
   Where = (void *)New_Object( Size, BYTE );
   if ( Where==NULL ) goto End;
   Ret = Compress_Read( (BYTE*)Where, In, Size );
      // TODO: test the returned value Ret
End:
   return ( Where );
}

EXTERN void *Load_LZW_Block( STRING Name )
{
   FILE *In;
   void *Where;

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );
   Where = Load_LZW_Block_II( In );
   F_CLOSE( In );
   return ( Where );
}

EXTERN void *Load_Block( STRING Name )
{
   FILE *In;
   void *Where;
   INT Size;

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );
   Size  = fgetc( In );
   Size += fgetc( In )*256;
   Size += fgetc( In )*256*256;
   Size += fgetc( In )*256*256*256;

   Where = (void *)New_Object( Size, BYTE );
   if ( Where==NULL ) goto End;
   if ( F_READ( Where, In, Size ) != 1 )
      { M_Free( Where ); }  // Error

End:
   F_CLOSE( In );
   return ( Where );
}

EXTERN void *Load_Raw_Block( STRING Name, INT *Size )
{
   FILE *In;
   void *Where;
   INT Len;
   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );
   Len = F_SIZE( In );
   if ( Size!=NULL ) *Size = Len;   
   Where = (void *)New_Object( Len, BYTE );
   if ( Where==NULL ) goto End;
   if ( F_READ( Where, In, Len ) != 1 )
      { M_Free( Where ); }  // Error

End:
   F_CLOSE( In );
   return ( Where );
}

EXTERN void *Load_LZW_Data( STRING Name )
{
   FILE *In;
   void *Where;
   INT Size, Length;

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

   fseek( In, 0, SEEK_END );
   Length = ftell( In );      // real length
   fseek( In, 0, SEEK_SET );
   Length -= 4;

   Size  = fgetc( In );     // length of uncompress block
   Size += fgetc( In )*256;
   Size += fgetc( In )*256*256;
   Size += fgetc( In )*256*256*256;
   
   Where = (void *)New_Object( Length, BYTE );
   if ( Where==NULL ) goto End;
   if ( F_READ( Where, In, Length ) != 1 )
      { M_Free( Where ); }  // Error

End:
   F_CLOSE( In );
   return ( Where );
}

/*******************************************************/

EXTERN INT Add_Raw_Block( FILE *Out, BYTE *Bits, INT Size )
{
   INT Ret;
   fputc( 0x00, Out );
   fputc( Size&0xFF, Out );
   fputc( (Size>>8)&0xFF, Out );
   fputc( (Size>>16)&0xFF, Out );
   fputc( (Size>>24)&0xFF, Out );
   Ret = fwrite( Bits, Size, 1, Out );
   return( Ret );
}

EXTERN INT Add_LZW_Block( FILE *Out, BYTE *Bits, INT Size, INT Bit_Size )
{
   INT Ret;
   fputc( 0x02, Out );
   fputc( Bit_Size, Out );
   Ret = LZW_Encoder( Bits, Out, Size, Bit_Size );
   return( Ret );
}

EXTERN INT Compress_Write( BYTE *Bits, FILE *Out, INT Size, INT Bit_Size )
{
   INT Len, Ret;

   Len = LZW_Encoder( Bits, NULL, Size, Bit_Size );
//   Out_Message( "LZW:  Len=%d   Size=%d", Len, Size );
   if ( Len<Size )
      Ret = Add_LZW_Block( Out, Bits, Size, Bit_Size );
   else Ret = Add_Raw_Block( Out, Bits, Size );
   return( Ret );   
}

EXTERN INT Save_LZW_Block_II( BYTE *Where, FILE *Out, INT Size, INT Bit_Size )
{
   INT Ret;

   if ( Out==NULL ) return( 0 );

   fputc( Size&0xFF, Out );
   fputc( (Size>>8)&0xFF, Out );
   fputc( (Size>>16)&0xFF, Out );
   fputc( (Size>>24)&0xFF, Out );

   Ret = Compress_Write( Where, Out, Size, Bit_Size );
   return( Ret );
}

EXTERN INT Save_LZW_Block( BYTE *Where, STRING Name, INT Size )
{
   FILE *Out;
   INT Ret;

//   Out = Access_File( Name, WRITE_SWITCH );
   Out = fopen( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( 0 );
   Ret = Save_LZW_Block_II( Where, Out, Size, 8 );
   fclose( Out );
   return( Ret );
}

#ifndef FINAL

EXTERN INT Save_Block( BYTE *Where, STRING Name, INT Size )
{
   FILE *Out;
   INT Ret;

//   Out = Access_File( Name, WRITE_SWITCH );
   Out = fopen( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( 0 );

   fputc( Size&0xFF, Out );
   fputc( (Size>>8)&0xFF, Out );
   fputc( (Size>>16)&0xFF, Out );
   fputc( (Size>>24)&0xFF, Out );

   Ret = fwrite( Where, Size, 1, Out );
   fclose( Out );
   return( Ret );
}

EXTERN INT Save_Raw_Block( BYTE *Where, STRING Name, INT Size )
{
   FILE *Out;
   INT Ret;

//   Out = Access_File( Name, WRITE_SWITCH );
   Out = fopen( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( 0 );

   Ret = fwrite( Where, Size, 1, Out );
   fclose( Out );
   return( Ret );
}

#endif   /* FINAL */


/*******************************************************/
/*******************************************************/

#if 0       // OLD junk

/* Format:
   [ 0xFF | Key | Count_High | Count_Low ]
   [ 0xFE | Count_High | Count_Low ]  for bytes 0x00
 */
static int Count_Out_Data_RLE( int C, INT Count )
{
   if ( Count>0 )
   {
      if ( C==0x00 ) return( 3 );
      else return( 4 );
   }
   else return( 1 );   
}

static BYTE *Out_Data_RLE( char C, INT Count, BYTE *Out )
{
   Count &= 0xFFFF;
   if ( Count>0 )
   {
      if ( C == 0x00 ) *Out++ = 0xFE;
      else { *Out++ = 0xFF; *Out++ = (BYTE)C; }
      *Out++ = Count>>8; *Out++ = Count&0xFF;
   }
   else if ( C == (char)0xFF || C == (char)0xFE )
   {
      *Out++ = 0xFF; *Out++ = (BYTE)C; 
      *Out++ = 0x00; *Out++ = 0x00;
   }
   else *Out++ = (BYTE)C;
   return( Out );
}

static int RLE_Compress( BYTE *Bits, INT Size, BYTE *Out )
{
   int Count, i, How_Many;
   char Last_Delta, Last;
   
   Count = 0; How_Many = 0;
   Last = 0; Last_Delta = 0;
   i = 0;
   while( i<Size )
   {
      char Delta, Next;

      Next = (char)Bits[i++]; Delta = Next-Last;
      if ( Delta == Last_Delta )
      {
         if ( Count == 0xFFFF ) 
         {
            if ( Out != NULL ) Out = Out_Data_RLE( Last_Delta, Count, Out );
            else How_Many += Count_Out_Data_RLE( Last_Delta, Count );
            Count = 0;
         }
         else Count++;
      }
      else 
      {
         if ( Out != NULL ) Out = Out_Data_RLE( Last_Delta, Count, Out );
         else How_Many += Count_Out_Data_RLE( Last_Delta, Count );
         Count = 0;
      }
      Last_Delta = Delta;
      Last = Next;
   }
   if ( Out != NULL ) Out = Out_Data_RLE( Last_Delta, Count, Out );
   else How_Many += Count_Out_Data_RLE( Last_Delta, Count );
   if ( Out==NULL ) return( How_Many );
   else return( Size );
}

EXTERN int Compress_Write( BYTE *Bits, FILE *Out, INT Size, INT Bit_Size )
{
   INT Len, Len2, Format, Ret, Last;
   BYTE *RLE_Buffer;
   FILE *Tmp_File;

/*
   Len = RLE_Compress( Bits, Size, NULL );
   Out_Message( "Len=%d   Size=%d\n", Len, Size );
   if ( Len>=Size )
   {
      Format = 0x00;
      Len = Size;
   }
   else Format = 0x01;
*/
   Len = Size; Format = 0x00; /* Disable RLE, for now */

   RLE_Buffer = (BYTE *)malloc( Len );
   if ( RLE_Buffer == NULL )
      Exit_Upon_Mem_Error( "RLE Buffer", Len );
/*
   if ( Format == 0x00 ) memcpy( RLE_Buffer, Bits, Len );
   else RLE_Compress( Bits, Len, RLE_Buffer );
*/
      /* Perform deltas... */
/*
   for( Len2=1; Len2<Len; ++Len2 )
   {
      INT Tmp;
      Tmp = (INT)RLE_Buffer[Len2];
      Tmp -= (INT)RLE_Buffer[Len2-1];
      RLE_Buffer[Len2] = (BYTE)Tmp;
   }
*/
   Len2 = LZW_Encoder( Bits, NULL, Len, 8 );
   Out_Message( "LZW:  Len2=%d   Len=%d\n", Len2, Len );   
   if ( Len2<Len )
   {
      fputc( Format|0x02, Out );
      fputc( 8, Out );
      Ret = LZW_Encoder( RLE_Buffer, Out, Len, 8 );
   }
   else
   {
      fputc( Format, Out );
      Ret = fwrite( RLE_Buffer, Len, 1, Out );
   }
   free( RLE_Buffer );

   return( Ret );   
}

#endif      // 0

/******************************************************************/

