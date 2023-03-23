/*
 * Letters
 *
 * Skal 98
 ***************************************/

#include "demo.h"
#include "vbuf.h"
#include "fonts.h"

static INT Cur_Text = 0;
static INT Cur_Word = 0;
static INT Nb_Words, S_Len;
#define MAX_WORDS 20
static STRING Words[MAX_WORDS+1];
static INT  Words_L[MAX_WORDS+1];
// static INT  Words_W[MAX_WORDS+1];
static FLT  Words_S[MAX_WORDS+1];

static INT Fx_Nb;
static FFX FFx[] = 
{
   {   100, 500, "O x . O ' . x o x O ,", 3, 300, 5,1.0f },
   {  1100, 400, "advert teased",   3, 230, 175, .6f },

   { 20800, 300, "Machinery",       1, 160,170,1.2f },
   { 25600, 200, "Bored",           3, 140,10,1.0 },
   { 26800, 200, "Pretend",         3, 140,15,1.0 },
   { 28000, 200, "Pathetic",        3, 140,20,1.0 },
   { 29500, 700, "laughing",        8, 0,70,1.7f },
   { 34300, 400, "Diversions",      3, 170,20,0.8f },
   { 36500, 300, "Tedious",         1, 130,80,1.3f },
   { 38600, 800, "crypto fascistic", 3, 80,60,0.6f },
   { 41000, 700, "balistics",       8, 0,190,1.8f },
   { 42100, 100, "Again",           0, 0,0,1.0 },
   { 42250, 100, "Again",           1, 0,0,1.5f },
   { 42400, 100, "Again",           3, 0,0,1.0 },
   { 45000, 200, "had enough",      3, 90,40,1.0 },
   { 45250, 200, "sad enough",      7, 170,40,1.0 },
   { 45500, 200, "mad enough",      3, 90,40,1.0 },
   { 46800, 500, "now it's time",   3, 90,40,1.0 },
   { 47400,1500, "change your ::STATE OF MIND", 6, 35,76,1.2f },

   { 62500, 800, "official sponsor            ", 6, 12,168,1.0 },

   { 65800, 400, "loner",     7, 160,110,1.0 },
   { 66600, 400, "zoner",     8, 0,155,1.4f },
   { 69200, 200, "hangin'",   3, 0,0,1.0 },
   { 71100, 300, "wasted",    6, 0,0,1.2f },
   { 72200, 200, "wasted",    0, 120,60,1.0 },
   { 73400, 400, "wasted",    8, 0,90,1.4f },
   { 75500, 200, "next",      3, 90,80,1.4f },
   { 77800, 200, "static",    3, 0,0,1.0 },
   { 78600, 200, "automatic", 3, 0,0,1.0 },
   { 80900, 400, "planned",   6, 20,140,1.2f },
   { 82200, 400, "anarchy",   2, 130,75,1.0 },
   { 83400, 500, "disregard", 6, 35,110,1.2f },

   { 90000, 600, "shut your mouth",     0, 140,70,1.0 },
   { 91200, 600, "get back in line",    2, 90,110,1.0 },
   { 93400, 300, "just fine",           2, 0,0,1.0 },   
   { 94200, 700, "we hold the levers",  2, 110,70,1.0 },
   { 95200, 700, "we pull the strings", 2, 100,80,1.0 },
   { 98200, 600, "learn :the words",    6, 80,140,1.0 },
   {100200, 400, "right",               1, 0,0,1.3f },
   {100850, 400, "wrong",               1, 0,0,1.3f },
   {101500, 300, "strong",              1, 0,0,1.3f },
   {103200, 500, "No: c: h: a: n: g: e",6, 270,-4,1.0 },

   {122000, 500, "NO.",                 0, 170,80,1.0 },
   {123300, 600, "you won't take me",   3, 0,0,1.0 },
   {124400, 800, "you won't break me",  3, 0,0,1.0 },
   {126300, 400, "step in line",        2, 0,0,1.0 },
   {128200, 700, "my soul is still mine", 3, 0,0,1.0 },
   {132100, 600, "hang with the program", 3, 80,70,1.0 },
   {135000,1100, "i_don't_give_a_shit_about__", 8, 0,70,1.4f },
   {137500, 600, "I make the rules",    2, 90,80,1.0 },
   {138500, 500, "I stand alone",       2, 90,120,1.0 },
   {140500, 500, "try to believe  ",    2, 120,60,1.0 },
   {142000,1000, "commit_yourself____", 8, 0,110,2.4f },   
   {145500,1100, "protection :rejection :       :correction", 6, 10,10,1.0 },
   {149400, 500, "repression",          7, 150,110,1.3f },

   {152700, 900, "STATE_OF_MIND_____",        8, 0,112,2.5 },

   {168200, 400, "consent to escape :what's gone  ", 6, 5,2,1.0 },
   {174200, 300, "won't :change  ", 6, 5,2,1.0 },
   {179200, 600, "a nation :of frantic :addicts   ", 6, 5,2,1.0 },
   {183200, 900, "thousands :of people :but you're :all alone     ", 6, 5,2,1.0 },

   {188400,1900, "Code_and Conception : Skal", 5, 20,10,1.0 },
   {190600,1900, "Visual_Supply : Krafton, Titan, Made", 5, 15,10,1.0 },
   {193500,1800, "Soundtrack : Senser 'Stacked_Up'",  5, 10,10,1.0 },
   
   {201500, 200, "STATE",           4,     160,40,1.0 },
   {201800, 200, "STATE OF",        4,  160,43,1.0 },
   {201100,8700, "STATE OF MIND",   4, 160,45,1.0 },

   {210500, 400, "O x . O x O ,",   2, 300, 5,1.0 },
   {218600, 500, "O x . O",         2, 300, 5,1.0 },
   { 0,0, NULL, -1 }
};

extern FONT_FX Font_FX[9];

extern INT Bypass_II( INT Timer );
extern void Post_FX( INT Timer );

/********************************************************************/

static void Split_Text( STRING S )
{
   INT i, j;
   Nb_Words = 0;
   S_Len = strlen( S ) + 1;
   for( i=0; i<S_Len; ++i )
   {
      while( S[i]==' ' ) if ( ++i==S_Len ) break;
      if ( i==S_Len ) break;
      Words[Nb_Words] = &S[i];
      j = 0;
      while( S[i]!=' ' ) { 
         if ( ++i==S_Len ) break; 
         else j++; 
      }
      Words_L[Nb_Words] = j; 
      Nb_Words++;
      if ( Nb_Words==MAX_WORDS ) break;
   }
   Words[Nb_Words] = NULL;
}

static void Store_Words_Time( )
{
   INT i;
   for( i=0; i<=Nb_Words; ++i )
      Words_S[i] = 1.0f*i/Nb_Words;
}

static void Advance_Word( FLT Tick_X )
{
   while ( Tick_X<Words_S[Cur_Word]||Tick_X>=Words_S[Cur_Word+1] )
      if ( Cur_Word<Nb_Words ) Cur_Word++;
      else break;
}
 
/********************************************************************/

EXTERN INT Bypass_I( INT Timer )
{
   if ( Cur_Text==-1 ) return( 0 ); // finished

   while ( Timer>FFx[Cur_Text].TStart + FFx[Cur_Text].TLength )
   {
      Cur_Text++;
      if (FFx[Cur_Text].Text==NULL ) { Cur_Text=-1; return( 0 ); }
   }

   if ( Timer<FFx[Cur_Text].TStart ) return( 0 );
   Split_Text( FFx[Cur_Text].Text );
   Cur_Word = 0;

      // choose FX and init.

   Fx_Nb = FFx[Cur_Text].Fx_Nb;
   SELECT_CMAP(VLETTERS);
   Check_CMap_VBuffer( &VB(VLETTERS), FNT_COL );

   if ( Font_FX[Fx_Nb].Init_Fx!=NULL )
      ( *Font_FX[Fx_Nb].Init_Fx )( );

      // go.
   Post_Loop = NULL;
   Bypass_Loop = NULL;

   if ( Font_FX[Fx_Nb].Post_Fx!=NULL )
   {
      Post_Loop = Post_FX;
      return( 0 );
   }
   else
   {
      Bypass_Loop  = Bypass_II;
      return( Bypass_Loop( FFx[Cur_Text].TStart ) );
   }
}

EXTERN INT Bypass_II( INT Timer )
{
   FLT Tick_X;
   Tick_X = (1.0f*Timer-FFx[Cur_Text].TStart )/FFx[Cur_Text].TLength;
   if ( Timer > FFx[Cur_Text].TStart+FFx[Cur_Text].TLength )
   {
      Bypass_Loop = Bypass_I;
      Tick_X = 1.0;
   }
   return( (*Font_FX[Fx_Nb].Do_Fx)( Tick_X ) );
}

EXTERN void Post_FX( INT Timer )
{
   FLT Tick_X;
   Tick_X = (1.0f*Timer-FFx[Cur_Text].TStart )/FFx[Cur_Text].TLength;
   if ( Timer > FFx[Cur_Text].TStart+FFx[Cur_Text].TLength )
   {
      Bypass_Loop = Bypass_I;
      Post_Loop = NULL;
      Tick_X = 1.0;
   }
   (*Font_FX[Fx_Nb].Post_Fx)( Tick_X );
}

/********************************************************************/

static void Print_Word_1( VBUFFER *V, INT W, FONT *F )
{
   INT i;
   FLT X, Y, R;

   if ( FFx[Cur_Text].Xo!=0 )
   {
      X = FFx[Cur_Text].Xo;
      Y = FFx[Cur_Text].Yo;
   }
   else { X = 0.5*The_W; Y = 0.5*The_H-10; }
   R = FFx[Cur_Text].Ro;

   X -= R*F->Avrg_Size*Words_L[W];
   VBuffer_Clear( V );
   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 0.2f, 0.0 );

   CMap_To_16bits( (void*)VB(VLETTERS).CMap, F->Btm->Pal, F->Btm->Nb_Col, 0x4565 );
   for( i=0; i<Words_L[W]; ++i )
   {
      INT C = F->Char_Map[ Words[W][i] ];
      X += Print_Letter( V, C, X, Y, R, F );
   }
//   Words_L[W] = 0;   // <= means: "ok, printed".
}

static void Print_Word_2( VBUFFER *V, FLT Tick_X, FONT *F, FLT Rand_Amp )
{
   INT i;
   FLT X, Y, R;

   R = FFx[Cur_Text].Ro;
   R *= ( 1.0f + Rand_Amp*(TRandom(0)&0x07)*(1.0f-Tick_X)/16.0f );

   if ( FFx[Cur_Text].Xo!=0 )
   {
      X = FFx[Cur_Text].Xo;
      Y = FFx[Cur_Text].Yo;
   }
   else { X = 0.5f*The_W; Y = 0.5f*The_H-R*10.0f; }

   Y -= 1.0f*(TRandom(1)&0x07);
   X -= R*F->Avrg_Size*Words_L[Cur_Word];

   VBuffer_Clear( V );
   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 0.2f, 0.0 );
   CMap_To_16bits( (void*)VB(VLETTERS).CMap, F->Btm->Pal, F->Btm->Nb_Col, 0x777 );
   for( i=0; i<Words_L[Cur_Word]; ++i )
   {
      INT C = F->Char_Map[ Words[Cur_Word][i] ];
      X += Print_Letter( V, C, X, Y, R, F );
   }
}

static void Print_Word_3( VBUFFER *V, FLT Tick_X, FONT *F, FLT Rand_Amp )
{
   INT i;
   FLT X, Y, R;

   R = FFx[Cur_Text].Ro;
   R *= ( 1.0f + Rand_Amp*(TRandom(0)&0x07)*(1.0f-Tick_X)/16.0f );

   if ( FFx[Cur_Text].Xo!=0 )
   {
      X = FFx[Cur_Text].Xo;
      Y = FFx[Cur_Text].Yo;
   }
   else { X = 0.5f*The_W; Y = 0.5f*The_H-R*10.0f; }

   Y -= 1.0f*(TRandom(1)&0x03);
   X -= R*F->Avrg_Size*Words_L[Cur_Word];

   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 0.2f, 0.0 );
   CMap_To_16bits( (void*)VB(VLETTERS).CMap, F->Btm->Pal, F->Btm->Nb_Col, 0x2565 );
   for( i=0; i<Words_L[Cur_Word]; ++i )
   {
      INT C = F->Char_Map[ Words[Cur_Word][i] ];
      X += Print_Letter_II( V, C, X, Y, R, F );
   }
}

/********************************************************************/

static void Init_Fx1( )
{
   Store_Words_Time( );
}

static INT Do_Fx1( FLT Tick_X )
{
   if ( Cur_Word>=Nb_Words ) Cur_Word=Nb_Words-1;
   if ( (TRandom(2)&0x03)==0x00 )
      Print_Word_1( &VB(VLETTERS), Cur_Word, &Font_Maps[2] );
   else Print_Word_1( &VB(VLETTERS), Cur_Word, &Font_Maps[3] );
   Advance_Word( Tick_X );
//   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VLETTERS_II), &VB(VLETTERS) );
   VBuf_Map_8_Bits( &VB(VSCREEN), &VB(VLETTERS) );
   return( 1 );
}

/********************************************************************/

static void Letter_Scratch( )
{
   INT c1, c2;
   c1 = (TRandom(0)&0x17) + 70;
   c2 = (TRandom(1)&0x07);
   Drv_Build_Ramp_16( VB(SCRATCH).CMap, 0, 8, c1,c1,c1, c2,c2,c2,0x777 );
   Get_Next_Scratch( 0x17 );
}

static void Init_Fx2( )
{
   Store_Words_Time( );
}

static INT Do_Fx2_Base( FLT Tick_X, FONT *F )
{
   if ( Cur_Word==Nb_Words ) return( 0 );
   Print_Word_2( &VB(VLETTERS), Tick_X, F, 1.0 );
   Advance_Word( Tick_X );
   Letter_Scratch( );
   Mixer.Mix_Sat8_8( &VB(VSCREEN), &VB(VLETTERS), &VB(SCRATCH) );
   return( 1 );
}
static INT Do_Fx2( FLT Tick_X )
{
   return( Do_Fx2_Base( Tick_X, &Font_Maps[TRandom(2)&0x01] ) );
}
static INT Do_Fx22( FLT Tick_X )
{
   return( Do_Fx2_Base( Tick_X, &Font_Maps[2+(TRandom(2)&0x01)] ) );
}

static INT Do_Fx23( FLT Tick_X )
{
   if ( Cur_Word==Nb_Words ) return( 0 );
   VBuffer_Clear( &VB(VSCREEN) );
   Print_Word_3( &VB(VSCREEN), Tick_X, &Font_Maps[TRandom(1)&0x01], 0.1f );
   Advance_Word( Tick_X );
   return( 1 );
}

/********************************************************************/

static void Init_Fx3( )
{
   Store_Words_Time( );
}

static INT Post_Fx3( FLT Tick_X )
{
   if ( Cur_Word==Nb_Words ) return( 0 );
   if ( (TRandom(2)&0x03)==0x00 )
      Print_Word_3( &VB(VSCREEN), Tick_X, &Font_Maps[2], 1.0 );
   else Print_Word_3( &VB(VSCREEN), Tick_X, &Font_Maps[3], 1.0 );
   Advance_Word( Tick_X );   
   return( 1 );
}

/********************************************************************/

static void Init_Fx4( )
{
   static INT Ok = FALSE;
   if ( !Ok )
   {
      SELECT_565( VLETTERS_II );
      Copy_Buffer_Short( &VB(VLETTERS_II), &VB(VSCREEN) );
      Ok = TRUE;
   }
   VBuffer_Clear( &VB(VLETTERS) );
}

static FLT Print_Words_Base( FONT *F, FLT Xo, FLT Y, FLT Ro, INT *Spc )
{
   CMap_To_16bits( (void*)VB(VLETTERS).CMap, F->Btm->Pal, F->Btm->Nb_Col, 0x777 );
   for( ; Cur_Word<Nb_Words; ++Cur_Word )
   {
      FLT X = Xo;
      INT i;
      if ( Words[Cur_Word][0]==':' )
      {
          Cur_Word++;
          break;
      }
      for( i=0; i<Words_L[Cur_Word]; ++i )
      {
         INT C = Words[Cur_Word][i];
         if  ( C=='_' ) C = 255;
         else C = F->Char_Map[ C ];
         if ( C==255 ) { X+=Font_Maps[0].Avrg_Size; continue; }
         Print_Letter( &VB(VLETTERS), C, X, Y, Ro, F );
         X += Spc[C];
      }
      Y += 32;
   }
   return( Y );
}

static INT Post_Fx4( FLT Tick_X )
{
   FLT Xo, Y, R;
   FONT *F;

   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 0.2f, 0.0 );
   Letter_Scratch( );

   { 
      INT j;
      BYTE *Dst = (BYTE*)VB(VLETTERS).Bits;
      BYTE *Src = (BYTE*)VB(SCRATCH).Bits;
      for( j=The_H; j>0; --j )
      {
         INT i;
         for( i=0; i<The_W; ++i ) Dst[i] = (Dst[i]>>1)+3-(Src[i]>>1);
         Dst += The_W;
         Src += The_W;
      }
   }

   R = FFx[Cur_Text].Ro;
   if ( FFx[Cur_Text].Xo!=0 )
   {
      Xo = FFx[Cur_Text].Xo;
      Y = FFx[Cur_Text].Yo;
   }
   else { Xo = 0.5f*The_W; Y = 0.5f*The_H-R*10.0f; }

   if ( (TRandom(2)&0x03)==0x00 )
   {
      F = &Font_Maps[2];
      R *= ( 1.0f + 0.08f*(TRandom(0)&0x07)*(1.0f-Tick_X)/16.0f );      
   }
   else F = &Font_Maps[3];

   Y -= F->Btm->Height*R*0.5f;
   Y -= 0.1f*(TRandom(1)&0x07);

   Cur_Word = 0;
   Print_Words_Base( F, Xo, Y, R, Font_Maps[2].FWidth );

   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VLETTERS_II), &VB(VLETTERS) );
   return( 1 );
}

/********************************************************************/

static FLT Print_Words_Base_II( FONT *F, FLT Xo, FLT Y, FLT Ro, INT *Spc )
{
   for( ; Cur_Word<Nb_Words; ++Cur_Word )
   {
      FLT X = Xo;
      INT i;
      if ( Words[Cur_Word][0]==':' )
      {
          Cur_Word++;
          break;
      }
      for( i=0; i<Words_L[Cur_Word]; ++i )
      {
         INT C = Words[Cur_Word][i];
         if  ( C=='_' ) C = 255;
         else C = F->Char_Map[ C ];
         if ( C==255 ) { X+=Font_Maps[0].Avrg_Size; continue; }
         Print_Letter_II( &VB(VSCREEN), C, X, Y, Ro, F );
         X += Spc[C];
      }
      Y += 32;
   }
   return( Y );
}

static void Print_Words_II( )
{
   FLT Xo, Yo, Ro, dY;
   FONT *F;

   Ro = FFx[Cur_Text].Ro;
   if ( FFx[Cur_Text].Xo!=0 )
   {
      Xo = FFx[Cur_Text].Xo;
      Yo = FFx[Cur_Text].Yo;
   }
   else { Xo = 0.5f*The_W; Yo = 0.5f*The_H-Ro*10.0f; }

   if ( (TRandom(2)&0x07)==0x00 ) { 
      F = &Font_Maps[1]; dY = 1.0f*(TRandom(1)&0x03); }
   else { F = &Font_Maps[0]; dY = 0; }
   Yo += dY;

   CMap_To_16bits( (void*)VB(VLETTERS).CMap, F->Btm->Pal, F->Btm->Nb_Col, 0x2565 );
   Cur_Word = 0;
   Yo = Print_Words_Base_II( F, Xo, Yo, Ro, Font_Maps[0].FWidth );
   Yo += 32-dY;

   if ( (TRandom(1)&0x03)==0x00 ) F = &Font_Maps[2];
   else F = &Font_Maps[3];
   Yo -= F->Btm->Height*Ro*0.5f;

   CMap_To_16bits( (void*)VB(VLETTERS).CMap, F->Btm->Pal, F->Btm->Nb_Col, 0x2565 );

   for( ; Cur_Word<Nb_Words; ++Cur_Word )
   {
      FLT X = Xo;
      INT i;
      for( i=0; i<Words_L[Cur_Word]; ++i )
      {
         INT C = Words[Cur_Word][i];
         if  ( C=='_' ) C = 255;
         else C = F->Char_Map[ C ];
         if ( C==255 ) { X+=Font_Maps[3].Avrg_Size; continue; }
         Print_Letter_II( &VB(VSCREEN), C, X, Yo, Ro, F );
         X += Font_Maps[3].FWidth[C]-3;
      }
      Yo += 32;
   }
   Letter_Scratch( );
   Mixer.Mix_Sat16_8( &VB(VSCREEN), &VB(VSCREEN), &VB(SCRATCH) );   
}

static void Init_Fx5( )
{
   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 0.2f, 0.0 );
}

static INT Do_Fx5( FLT Tick_X )
{
   VBuffer_Clear( &VB(VSCREEN) );
   Print_Words_II( );
   return( 1 );
}

/********************************************************************/

static void Init_Fx6( )
{
   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 0.2f, 0.0 );
   S_Len = strlen(  FFx[Cur_Text].Text ) + 1;
}

static INT Post_Fx6( FLT Tick_X )
{
   INT i, E, Co;
   FLT X,Xo,Yo, Ro;
   FONT *F;

   Xo = Tick_X * S_Len;
   E = (INT)floor( Xo );
   Ro = Xo-1.0f*E;
   Co = ((INT)(126*Ro) ^ 0x7f ) + 0x50;

   F = &Font_Maps[4];

   CMap_To_16bits( (void*)VB(VLETTERS).CMap, F->Btm->Pal, F->Btm->Nb_Col, 0x2565 );
   Ro = FFx[Cur_Text].Ro;
   if ( FFx[Cur_Text].Xo!=0 )
   {
      Xo = FFx[Cur_Text].Xo;
      Yo = FFx[Cur_Text].Yo;
   }
   else { Xo = .08f*The_W; Yo = 0.5f*The_H-Ro*10.0f; }

   X = Xo;
   for( i=0; i<E; ++i )
   {
      INT C = FFx[Cur_Text].Text[i];
      
      if  ( C=='_' ) C = 255;
      else if ( C==':' ) { Yo+=30; X = Xo; continue; }
      else C = F->Char_Map[ C ];
      if ( C==255 ) { X+=F->Avrg_Size; continue; }
      if ( i==E-1 )
        Drv_Build_Ramp_16( (USHORT*)VB(VLETTERS).CMap, 0, F->Btm->Nb_Col,
           Co, Co, Co, 255,255,255, 0x2565 );
//      CMap_To_16bits( (void*)VB(VLETTERS).CMap, F->Btm->Pal, F->Btm->Nb_Col, 0x2565 );
      Print_Letter_II( &VB(VSCREEN), C, X, Yo, Ro, F );
      X += F->FWidth[C];
   }
   return( 1 );
}
/********************************************************************/

static void Print_Word_4( VBUFFER *V, FONT *F, FLT X )
{
   INT i;
   FLT Y, R;

   R = FFx[Cur_Text].Ro;
   if ( FFx[Cur_Text].Yo!=0 ) Y = FFx[Cur_Text].Yo;
   else { Y = 5; }
   Y -= F->Btm->Height*R/2.0f;
   i=0;
   while( 1 )
   {
      INT C = FFx[Cur_Text].Text[i];
      if ( C=='\0' ) break;
      if ( C=='_' ) C = 255;
      else C = F->Char_Map[ C ];
      X += Print_Letter_III( V, C, X, Y, R, F );
      i++;
   }
}

static void Init_Fx7( )
{
   S_Len = strlen(  FFx[Cur_Text].Text ) + 1;
   S_Len = S_Len*Font_Maps[5].Avrg_Size + The_W;
}

static INT Post_Fx7( FLT Tick_X )
{
   Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 0.2f, 0.0 );
   CMap_To_16bits( (void*)VB(VLETTERS).CMap, Font_Maps[5].Btm->Pal, Font_Maps[5].Btm->Nb_Col, 0x2565 );
   Print_Word_4( &VB(VSCREEN), &Font_Maps[5], The_W - 1.3f*Tick_X*S_Len );
   return( 1 );
}

/********************************************************************/
/********************************************************************/

static FONT_FX Font_FX[] = { 
   { Init_Fx1, Do_Fx1,   NULL },          // 0
   { Init_Fx2, Do_Fx2,   NULL },          // 1
   { Init_Fx2, Do_Fx22,  NULL },          // 2
   { Init_Fx3, NULL,     Post_Fx3 },      // 3

   { Init_Fx4, NULL,     Post_Fx4 },      // 4
   { Init_Fx5, Do_Fx5, NULL },            // 5

   { Init_Fx6, NULL,     Post_Fx6 },      // 6

   { Init_Fx2, Do_Fx23,  NULL },          // 7
   { Init_Fx7, NULL,     Post_Fx7 },      // 8
};

/********************************************************************/
/********************************************************************/
