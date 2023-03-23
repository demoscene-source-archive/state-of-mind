/***********************************************
 *          Any format dispatcher              *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "btm.h"
#include "ld_any.h"

/*******************************************************/

EXTERN BITMAP *Load_Any( char *Name )
{
   BITMAP *Btm;
   INT i, Type;

   Type = 2;  /* default: GIF */
   i = strlen( Name )-1;
   while( Name[i] != '.' ) if ( i--==0 ) goto Load;

   if ( !strncmp( Name+i+1, "btm", 3 ) ) Type = 0;
   else if ( !strncmp( Name+i+1, "BTM", 3 ) ) Type = 0;
   else if ( !strncmp( Name+i+1, "ppm", 3 ) ) Type = 1;
   else if ( !strncmp( Name+i+1, "PPM", 3 ) ) Type = 1;
   else if ( !strncmp( Name+i+1, "gif", 3 ) ) Type = 2;
   else if ( !strncmp( Name+i+1, "GIF", 3 ) ) Type = 2;
   else if ( !strncmp( Name+i+1, "tga", 3 ) ) Type = 3;
   else if ( !strncmp( Name+i+1, "TGA", 3 ) ) Type = 3;
   else if ( !strncmp( Name+i+1, "bmp", 3 ) ) Type = 4;
   else if ( !strncmp( Name+i+1, "BMP", 3 ) ) Type = 4;
   else if ( !strncmp( Name+i+1, "raw", 3 ) ) Type = 5;
   else if ( !strncmp( Name+i+1, "RAW", 3 ) ) Type = 5;

Load:
   switch( Type )
   {
      case 5: Btm = Load_RAW( Name ); break;
      case 4: Btm = Load_BMP( Name ); break;
      case 3: Btm = Load_TGA( Name ); break;
      case 2: Btm = Load_GIF( Name ); break;
      case 1: Btm = Load_PPM( Name ); break;
      case 0: default: Btm = Load_BTM( Name ); break;
   }
   return( Btm );
}

/*******************************************************/
