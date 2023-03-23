/***********************************************
 *              I/O utilities                  *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "lzw.h"

#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
#include <direct.h>
#endif

// #define DEBUG_IO

// #define BUILD_DAT_FILE

/******************************************************************/

#ifdef BUILD_DAT_FILE

EXTERN INT  Write_Dat_File = FALSE;

#endif   // BUILD_DAT_FILE

EXTERN STRING Dat_File_Name = NULL;
EXTERN FILE *Dat_File = NULL;

static long Length = 0;
static long Cur_Start = 0;

/******************************************************************/

static FILE *Open_Dat_File( STRING Switch )
{
   Dat_File = fopen( Dat_File_Name, Switch );
   if ( Dat_File == NULL )
#ifdef WIN32
   {
      char S[1024], *Cwd;
      Cwd = getcwd( NULL, 256 );
      sprintf( S, "Can't open data file '%s'\n Cwd=%s\n", Dat_File_Name, Cwd );
      free( Cwd );
      MessageBox( NULL, S, "IO error", MB_OK|MB_ICONWARNING );
      PostQuitMessage( 0 );
      exit( 1 );
   }
#else
      Exit_Upon_Error( "Can't open data file '%s'.\n", Dat_File_Name );
#endif
   Cur_Start = Length = 0;

#ifdef DEBUG_IO
   Out_Message( "Using Dat file '%s'.\n", Dat_File_Name );
#endif

   return( Dat_File );
}

/******************************************************************/

   /* Hooked routine */

/* if BUILD_DAT_FILE is defined the following (bivalent) routines
   will be used to transfert various files into the .dat file.
   Otherwise, (for the final version), only input from dat file
   will be used, if opened... */ 

extern FILE *F_OPEN( STRING Name, STRING Switch )
{
   int Rewinded;
   FILE *In;

   if ( Name==NULL ) return( NULL );

   if ( Dat_File_Name == NULL )     // dat file not use.
   {
      Cur_Start = 0;
      In = fopen( Name, Switch );
      if ( In==NULL ) return( NULL );
      fseek( In, 0, SEEK_END );
      Length = ftell( In );
      fseek( In, 0, SEEK_SET );
      return( In );
   }


#if !defined( __WATCOM__ ) && defined( BUILD_DAT_FILE )

      // WATCOM bug. Dat file will be built with DJGPP. Period.

   if ( Write_Dat_File == TRUE )
   {
      int i;

      In = fopen( Name, Switch );
      if ( In==NULL ) return( NULL );

      if ( Dat_File == NULL ) Open_Dat_File( WRITE_SWITCH );
      fseek( In, 0, SEEK_END );
      Length = ftell( In );
      fseek( In, 0, SEEK_SET );
      Out_Message( "Copying file '%s' (%d bytes) in dat file '%s'",
         Name, Length, Dat_File_Name );

      fprintf( Dat_File, "%s\n%d\n", Name, Length );  // Header
      for ( i=512; i<Length; i+=512 )               // Data
      {
         PIXEL Buffer[ 512 ];    // <= Brain-damaged with WATCOM!!
         fread( Buffer, 512, 1, In );
         fwrite( Buffer, 512, 1, Dat_File );
      }
      i = 512-(i-Length);
      if ( i )
      {
         PIXEL Buffer[ 512 ];    // <= Brain-damaged with WATCOM!!
         fread( Buffer, i, 1, In );
         fwrite( Buffer, i, 1, Dat_File );
      }

      fseek( In, 0, SEEK_SET );     // rewind()
      return( In );
   }
#endif      // __WATCOMC__

      // Use Dat file

   if ( Dat_File == NULL ) Open_Dat_File( READ_SWITCH );

   Rewinded = FALSE;
   while( 1 )              // Search/read stamp header
   {
      int c, i;
      char Line[100];

      c = fgetc( Dat_File );
Rewind:
      if ( c==EOF ) 
      {
         rewind( Dat_File );
         c = fgetc( Dat_File );
      }
      i = 0;
      do {
         Line[i++] = ( char )c;
         c = fgetc( Dat_File );
         if ( c==EOF ) goto Rewind;
      }
      while ( c!='\n' && i<99 );
      Line[i]='\0';
#if 0
      if ( c==EOF )
      {
         if ( Rewinded )
            Exit_Upon_Error( "Unexpected EOF in Dat_File.\nFile '%s' not found.\n", Name );
         else { rewind( Dat_File ); Rewinded = TRUE; }
      }
#endif
      fscanf( Dat_File, "%ld\n", &Length );
      if ( !strcmp( Name, Line ) ) break;
      fseek( Dat_File, Length, SEEK_CUR );   // Skip this block
   }
   Cur_Start = ftell( Dat_File );
   return( Dat_File );
}

/******************************************************************/

EXTERN void F_CLOSE( FILE *F )
{
   if ( Dat_File==NULL ) fclose( F );
}

EXTERN void Close_Dat_File( )
{
   if ( Dat_File != NULL ) fclose( Dat_File );
   Dat_File = NULL;
   Dat_File_Name = NULL;
   Cur_Start = Length = 0;
}

EXTERN void USE_DAT_FILE( STRING Name, INT Write )
{
   Dat_File_Name = Name;
#ifdef BUILD_DAT_FILE
   Write_Dat_File = Write;
#endif
}

/******************************************************************/

EXTERN FILE *Access_File( STRING Name, STRING Switch )
{
   FILE *File;
   Length = 0;
   File = F_OPEN( Name, Switch );
   if ( File != NULL ) return( File );

#ifdef FINAL
   Exit_Upon_Error( "Can't access file '%s'.\n", Name );
#endif
   return( NULL );
}

EXTERN long F_TELL( FILE *File )
{
   return( ftell( File ) - Cur_Start );
}

EXTERN long F_SIZE( FILE *File )
{
   return( (long)Length );
}

EXTERN long F_SEEK( FILE *File, long Off, int How )
{
   long Ok;
   switch( How )
   {
      case SEEK_END:
         Ok = fseek( File, Cur_Start+Length, SEEK_SET );
      break;
      case SEEK_SET:
         Ok = fseek( File, Off+Cur_Start, SEEK_SET );
      break;
      default: case SEEK_CUR:
         Ok = fseek( File, Off, How );
      break;
   }
   return( Ok );
}

/******************************************************************/
/******************************************************************/

#ifndef SKL_LIGHT_CODE 
EXTERN INT Get_Me_A_Number( FILE *In )
{
   int c, i;
   char Buf[ 20 ];

Try_Again:
   Out_String( "\n=>" );
   Buf[ i=0 ] = '\0';
   while( 1 )
   { 
      c = getc( In );
      if ( c!=EOF && c!='\n' && i<19 ) Buf[i++] = c;
      else break;
   }
   Buf[1] = '\0';
   if ( ( Buf[0]<'0' ) || ( Buf[0]>'9' ) ) goto Try_Again;

   return( Buf[0]-'0' );
}
#endif

/******************************************************************/

#if 0

EXTERN int Compress_Read( PIXEL *Bits, INT Size, FILE *In )
{
   int i, Ret;
   char Last;

   i = fgetc( In );
   if ( i==0x02 ) Ret = LZW_Decoder( Bits, In, Size, (INT)fgetc( In ) );
   else Ret = fread( Bits, Size, 1, In );

/*
   if ( i&0x01 )
   {
      i = 0; Last = 0;
      while( i<Size )
      {
         int Count;
         char Delta;

         Count = (int)fgetc( In );
         if ( Count==0xFE ) { Delta = 0; goto Read_Data; }
         else if ( Count==0xFF ) 
         { Delta = (char)fgetc( In ); goto Read_Data; }
         Delta = (char)Count;
         Last += Delta; Bits[i++] = (PIXEL)Last;
         continue;
   Read_Data:
         Count = ( (int)fgetc( In )<<8 ) | (int)fgetc( In );
         while( (Count-->=0 ) && i<Size )
         {  Last += Delta; Bits[i++] = Last; }
      }
      Ret = Size;
   }
*/
      /* Perform deltas... */
/*
   {
      int i;
      for( i=1; i<Size; ++i )
      {
         INT Tmp;
         Tmp = (INT)Bits[i];
         Tmp += (INT)Bits[i-1];
         Bits[i] = (PIXEL)Tmp;
      }
   }
*/
   return( Ret );
}

#endif

/******************************************************************/

