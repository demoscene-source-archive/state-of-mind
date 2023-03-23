/*
 * Letters
 *
 * Skal 98
 ***************************************/

#include "demo.h"
#include "sprite.h"
#include "vbuf.h"
#include "fonts.h"

/********************************************************************/

EXTERN FONT *Font_Maps;

/********************************************************************/

static unsigned char Char_To_Font_I( unsigned char C )
{
   if ( C>='a' && C<='z' ) return( C-'a' );
   else if ( C>='A' && C<='Z' ) return(  C-'A' + 26 );
   else if ( C=='\'' ) return( 26+26 );
   else if ( C==',' ) return(  26+27 );
   else if ( C=='.' ) return(  26+28 );
   else return( 255 );
}

static unsigned char Char_To_Font_II( unsigned char C )
{
   if ( C>='a' && C<='z' ) return( C-'a' );
   else if ( C>='A' && C<='Z' ) return(  C-'A' );
   else if ( C=='\'' ) return( 26 );
   else if ( C==',' ) return(  27 );
   else if ( C=='.' ) return(  28 );
   else return( 255 );
}

/********************************************************************/

static INT Find_Text( INT k, FONT *F )
{
   while( k<F->Btm->Width )
   {
      INT j;
      BYTE *Ptr = (BYTE*)F->Btm->Bits + k;
      for ( j=F->Btm->Height; j>0; --j )
      {
         // if ( Ptr[0]>=0x8410 ) break;
         if ( Ptr[0] ) break;
         Ptr += F->Btm->Width;
      }
      if ( j ) break;
      k++;
   }
   return( k );
}

static void Font_Init( FONT *F, STRING Name, INT Avrg )
{
   INT i, k, l;

   F->Btm = Load_GIF( Name );
   F->Avrg_Size = Avrg;

      // Store width

//   F->Nb_Chars = 0;
   k=0, i=0;
   while( k<F->Btm->Width )
   {
      F->FPos[i] = k = Find_Text( k, F );
      if ( k==F->Btm->Width ) break;
      l = k;
      do { l++; } while( Find_Text( l, F )==l );
      F->FWidth[i] = l-k;
      k = l;
      i++;
   }
   F->Nb_Chars = i;
   if ( F->Nb_Chars<=40 )
      for( i=0; i<128; ++i )
         F->Char_Map[i] = Char_To_Font_II( (unsigned char)i );
   else
      for( i=0; i<128; ++i )
         F->Char_Map[i] = Char_To_Font_I( (unsigned char)i );
}

EXTERN void Init_Letters( )
{
   Font_Maps = New_Fatal_Object( 6, FONT );
   Font_Init( &Font_Maps[0], NAME0, 11 );  // normal
   Font_Init( &Font_Maps[1], NAME1, 11 );

   Font_Init( &Font_Maps[2], NAME2, 10 );  // penciled
   Font_Init( &Font_Maps[3], NAME3, 10 );

   Font_Init( &Font_Maps[4], NAME4, 11 );  // typewriter

   Font_Init( &Font_Maps[5], NAME5, 90 );  // big
   Bypass_Loop = Bypass_I;
}

/********************************************************************/

EXTERN FLT Print_Letter( VBUFFER *Dst, INT i, 
   FLT X, FLT Y, FLT R, FONT *F )
{
   SPRITE Sp;

   if (i==255) return( R*F->Avrg_Size ); // space
   Sp.Uo = 0;
   Sp.Vo = 0;
   Sp.dU = F->FWidth[i];
   Sp.dV = F->Btm->Height;
   Sp.BpS = F->Btm->Width;
   Sp.Bits = (PIXEL*)( ((BYTE*)F->Btm->Bits)+F->FPos[i] );
   Paste_Sprite_Bitmap_8( (PIXEL*)Dst->Bits, Dst->BpS,
      X + Sp.dU/2.0f, Y + Sp.dV/2.0f, R, &Sp );
   return( R*F->FWidth[i] );
}

/********************************************************************/

EXTERN FLT Print_Letter_II( VBUFFER *Dst, INT i, 
   FLT X, FLT Y, FLT R, FONT *F )
{
   SPRITE Sp;

   if (i==255) return( R*F->Avrg_Size ); // space
   Sp.Uo = 0;
   Sp.Vo = 0;
   Sp.dU = F->FWidth[i];
   Sp.dV = F->Btm->Height;
   Sp.BpS = F->Btm->Width;
   Sp.Bits = (PIXEL*)( ((BYTE*)F->Btm->Bits)+F->FPos[i] );
   Paste_Sprite_Bitmap_8_Map16_Or( (USHORT*)Dst->Bits, Dst->BpS,
      X + Sp.dU/2.0f, Y + Sp.dV/2.0f, R, &Sp, (USHORT*)VB(VLETTERS).CMap );
   return( R*F->FWidth[i] );
}

EXTERN FLT Print_Letter_III( VBUFFER *Dst, INT i, 
   FLT X, FLT Y, FLT R, FONT *F )
{
   SPRITE Sp;

   if (i==255) return( R*F->Avrg_Size ); // space
   Sp.Uo = 0;
   Sp.Vo = 0;
   Sp.dU = F->FWidth[i];
   Sp.dV = F->Btm->Height;
   Sp.BpS = F->Btm->Width;
   Sp.Bits = (PIXEL*)( ((BYTE*)F->Btm->Bits)+F->FPos[i] );
   Paste_Sprite_Bitmap_8_Map16( (USHORT*)Dst->Bits, Dst->BpS,
      X + Sp.dU/2.0f, Y + Sp.dV/2.0f, R, &Sp, (USHORT*)VB(VLETTERS).CMap );
   return( R*F->FWidth[i] );
}

/********************************************************************/
/********************************************************************/

