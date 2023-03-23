/*
 * fonts+lettering
 *
 * Skal 98 (skal.planet-d.net)
***************************************/

#include "btm.h"

/*******************************************************************/

#define NAME0  "font32.gif"
#define NAME1  "font42.gif"
#define NAME2  "font632.gif"
#define NAME3  "font62.gif"
#define NAME4  "font82.gif"
#define NAME5  "font94.gif"

#define FNT_COL   16
#define GLEV      196      // gray level

typedef struct 
{
   long TStart, TLength;
   STRING Text;
   INT Fx_Nb;
   FLT Xo, Yo, Ro;

} FFX;

typedef struct 
{
   void (*Init_Fx)( );
   INT (*Do_Fx)( FLT Tick_X );
   INT (*Post_Fx)( FLT Tick_X );
   
} FONT_FX;

typedef struct 
{
   BITMAP *Btm;
   INT FWidth[26+26+3];     // Warning. Size must be <65k
   INT FPos[26+26+3];
   INT Nb_Chars, Avrg_Size;
   unsigned char Char_Map[128];         // Warning.

} FONT;

extern FONT *Font_Maps;

extern void Init_Letters( );
extern FLT Print_Letter( VBUFFER *Dst, INT i, 
   FLT X, FLT Y, FLT R, FONT *F );
extern FLT Print_Letter_II( VBUFFER *Dst, INT i, 
   FLT X, FLT Y, FLT R, FONT *F );
extern FLT Print_Letter_III( VBUFFER *Dst, INT i, 
   FLT X, FLT Y, FLT R, FONT *F );
extern INT Bypass_I( INT Timer );

/*******************************************************************/


