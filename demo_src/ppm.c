/***********************************************
 *              PPM format                     *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "ld_any.h"
#include "truecol.h"

/*******************************************************/
/*******************************************************/

EXTERN void Save_PPM( char *Name, BITMAP *Bitmap, PIXEL *Pal )
{
   FILE *Out;
   int i;
   PIXEL c;

   if ( Bitmap == NULL ) return;

   if ( Name == NULL ) Name = "dump.ppm";
   if ( Pal == NULL ) Pal = Bitmap->Pal;

   Out = fopen( Name, WRITE_SWITCH );
   if ( Out==NULL ) return;

   fprintf( Out, "P6\n%d %d\n255\n", Bitmap->Width, Bitmap->Height );
   for( i=0; i<Bitmap->Width*Bitmap->Height; ++i )
   {
      c = Bitmap->Bits[i];
      fprintf( Out, "%c%c%c",
         Pal[ 3*c ]<<2, Pal[ 3*c+1 ]<<2, Pal[ 3*c+2 ]<<2 );
   }   
   fprintf( Out, "\n" );
   fclose( Out );
}

EXTERN BITMAP *Load_PPM( char *Name )
{
   PIXEL Head[ 3 ] = { 'P', '6', '\n' };
   int i, j, n, Found, Last, Warning;
   BITMAP *Btm;
   FILE *In;
   PIXEL *Pal;
   PIXEL *Bits;
   INT Width, Height;
   PIXEL Buf[3];
   char Comments[ 256 ];

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

         /* Header */

   fread( Buf, 3, 1, In );
   for( i=0;i<3;++i )
      if ( Buf[i] != Head[i] )
         Exit_Upon_Error( "'%s' has wrong PPM header...", Name );

         /* Comments */

   i = getc( In );
   while( i=='#' )
   {
      n = 0;
      while( ( i=getc( In ) ) != '\n' )
      {
         if ( i==EOF )
            Exit_Upon_Error( " Load_PPM(): unexpected EOF !" );
         if ( n!=255 ) Comments[n++] = i;
      }
      Comments[n] = '\0';
      Out_Message( "Comment: %s", Comments );
      i = getc( In );
   }
   ungetc( (char)i, In );


         /* Size */

   if ( fscanf( In, "%d %d\n255", &Width, &Height ) != 2 )
      Exit_Upon_Error( "Load_PPM(): Size unreadable.\n" );

   fgetc( In );   /* the last '\n' */

   Btm = New_Bitmap( Width, Height, 256 );
   Bits = Btm->Bits;
   Pal = Btm->Pal;
   for( j=0; j<768; ++j ) Pal[ j ] = 0;

   Last = 0;
   Warning = FALSE;
   for( i=0; i<Width*Height; ++i )
   {
      PIXEL r, g, b;

      r = fgetc( In );
      g = fgetc( In );
      b = fgetc( In );

      for( Found=0; Found<Last; Found+=3 )   /* Search for match */
      {
         if (    ( Pal[Found]   == r )
              && ( Pal[Found+1] == g ) 
              && ( Pal[Found+2] == b ) )
         {
            *Bits++ = ( PIXEL )( Found/3 );
            break;
         }
      }

      if ( Found==Last )   /* No matching color => Create a new one */
      {
         Pal[ Last   ] = r;
         Pal[ Last+1 ] = g;
         Pal[ Last+2 ] = b;

         if ( Last>=768 )
         {
            if ( !Warning )
            {
               Out_Message( "Load_PPM(): Warning ! Too many colors.\n" );
               Warning = TRUE;
            }
            Bits[ i ] = ( PIXEL )Match_Best_RGB( Pal, 256, r, g, b );
         }
         else 
         {
            *Bits++ = ( PIXEL )( Last/3 );
            Last += 3;
         }
      }
   }
   fclose( In );
   Last /= 3;
   Btm->Nb_Col = Last;
   if ( Last != 256 )
   {
      PIXEL *Tmp;
      Tmp = New_Fatal_Object( Last*3, PIXEL );
      memcpy( Tmp, Btm->Pal, Last*3 );
      M_Free( Btm->Pal );
      Btm->Pal = Tmp;
   }
   Out_Message( "Load_PPM(): Width:%d Height:%d Nb_Colors:%d ", 
      Btm->Width, Btm->Height, Btm->Nb_Col );
   return( Btm );
}

/*******************************************************/

EXTERN BYTE *Load_Raw_PPM( STRING Name, USHORT Format,
   INT *Width, INT *Height )
{
   PIXEL Head[ 3 ] = { 'P', '6', '\n' };
   int i, n;
   FILE *In;
   BYTE *Bits=NULL;
   PIXEL Buf[3];
   char Comments[ 256 ];

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

         /* Header */

   fread( Buf, 3, 1, In );
   for( i=0;i<3;++i )
      if ( Buf[i] != Head[i] )
         Exit_Upon_Error( "'%s' has wrong PPM header...", Name );

   i = getc( In );
   while( i=='#' )   // Comments
   {
      n = 0;
      while( ( i=getc( In ) ) != '\n' )
      {
         if ( i==EOF )
            Exit_Upon_Error( " Load_PPM(): unexpected EOF !" );
         if ( n!=255 ) Comments[n++] = i;
      }
      Comments[n] = '\0';
      Out_Message( "Comment: %s", Comments );
      i = getc( In );
   }
   ungetc( (char)i, In );

   if ( fscanf( In, "%d %d\n255", Width, Height ) != 2 )          // Size
      Exit_Upon_Error( "Load_Raw_PPM(): Size unreadable.\n" );

   fgetc( In );   /* the last '\n' */

   if ( (Format&0xFFFF) == 0x2565 || (Format&0xFFF) == 0x2555 )
   {
      USHORT *Dst;
      Bits = New_Fatal_Object( (*Width)*(*Height)*2, BYTE );
      Dst = (USHORT*)Bits;
      if ( (Format&0xFFF) == 0x565 )
         for( i=0; i<(*Width)*(*Height); ++i )
         {
            USHORT r, g, b;
            r = fgetc( In );  // red
            g = fgetc( In );  // green
            b = fgetc( In );  // blue
            *Dst++ = RGB_TO_565(r,g,b);
         }
      else
         for( i=0; i<(*Width)*(*Height); ++i )
         {
            USHORT r, g, b;
            r = fgetc( In );  // blue
            g = fgetc( In );  // green
            b = fgetc( In );  // red
            *Dst++ = RGB_TO_555(r,g,b);
         }
   }
   else if ( (Format&0xFFFF) == 0x3888 )
   {
      Bits = New_Fatal_Object( (*Width)*(*Height)*3, BYTE );
      for( i=0; i<(*Width)*(*Height); ++i )
      {
         Bits[3*i+2] = fgetc( In );  // blue
         Bits[3*i+1] = fgetc( In );  // green
         Bits[3*i+0] = fgetc( In );  // red
      }
   }

   fclose( In );
   return( Bits );
}

EXTERN BITMAP_16 *Load_PPM_16b( STRING Name, USHORT Format )
{
   BYTE *Bits;
   INT W, H;
   BITMAP_16 *New;

      // format must be 0x3888, 0x2555 or 0x2565
   Bits = Load_Raw_PPM( Name, Format, &W, &H );
   if ( Bits==NULL ) return( NULL );
   New = New_Fatal_Object( 1, BITMAP_16 );
   New->Bits = Bits;
   New->Width = W;
   New->Height = H;
   New->Size = W*H;
   if ( Format==0x3888 ) New->Size *= 3;
   else New->Size *= 2; 
   New->Format = Format;
   New->Nb_Col = 0;
   New->Pal = NULL;
   return( New );
}
/*******************************************************/
