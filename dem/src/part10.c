/*
 * line stuff
 *
 * Skal98 (skal.planet-d.net)
 ***************************************/

#include "demo.h"

typedef struct  
{
   FLT xo, yo, x, y;
   FLT Scale, Rot;
} LINES;

#include "lines4.h"

/********************************************************************/
/********************************************************************/

static void Do_Lines( LINES *Lines, INT Nb, FLT eps, FLT Zoom )
{
   INT i;

   Install_Renderer( &Renderer_16b, RENDER_SIZE, The_W, The_H, RENDER_END_ARG );
   Select_Primitives( &Primitives_16 );
   Setup_Rendering( VB(2).Bits, NULL, NULL, NULL, NULL,
      The_W, The_H, The_W*2 );
   Setup_Clipping( 2.0, The_W-2.0, 2.0, The_H-2.0, 1.0, 3000.0 );
   Line_16_CMap = (USHORT*)VB(2).CMap;
   for( i=0; i<Nb; ++i )
   {
      FLT xi, yi, xf, yf;
      FLT C, S, Cx, Cy;
      FLT Scale, Rot;
      FLT xxi, yyi, xxf, yyf;
      Scale = 1.0 + Lines[i].Scale*eps;
      Rot = Lines[i].Rot*eps;
      C = cos( Rot ); S = sin( Rot );
      xi = ( Lines[i].xo - .5 ) * Zoom + .5;
      yi = ( Lines[i].yo - .5 ) * Zoom + .5;
      xf = ( Lines[i].x - .5 ) * Zoom + .5;
      yf = ( Lines[i].y - .5 ) * Zoom + .5;
      Cx = (xi+xf)/2.0; Cy=(yi+yf)/2.0;
      xi = (xi-Cx)*Scale;
      yi = (yi-Cy)*Scale;
      xf = (xf-Cx)*Scale;
      yf = (yf-Cy)*Scale;
      xxi = xi*C + yi*S + Cx;
      yyi =-xi*S + yi*C + Cy;
      xxf = xf*C + yf*S + Cx;
      yyf =-xf*S + yf*C + Cy;
      ((void (*)(FLT,FLT,FLT,FLT))Primitives[RENDER_LINE])( 
         xxi*The_W, yyi*The_H,
         xxf*The_W, yyf*The_H );
   }
}

static void Randomize_Lines( LINES *Lines, INT Nb )
{
   INT i;
   for( i=0; i<Nb; ++i )
   {
      Lines[i].Scale = 1.0*(Random()&0xFF)/256.0;
      Lines[i].Rot = 0.4*M_PI*(Random()&0xFF)/256.0;
   }
}

/********************************************************************/

EXTERN void Init_10( INT Param )
{
   if ( Param==0 ) Randomize_Lines( Lines_1, NB_LINES1 );
   SELECT_565( 1 );
   Copy_Buffer_Short( &VB(1), &VB(VSCREEN) );      // copy screen in VB #1

   SELECT_565( 2 );
   Check_CMap_VBuffer( &VB(2), 256 );
   Init_Anti_Alias_Table( 0, 255, 1.7 );
}

EXTERN void Loop_10( )
{
   FLT x, Zoom, Rot;
   INT i, j;

   VBuffer_Clear( &VB(2) );
   x = sin( Tick_x*M_PI );
   Zoom = 0.9 * ( 1.0-(1.0-x)*(1.0-x) );
   Rot = x*x;
   Drv_Build_Ramp_16( VB(2).CMap, 0, 256, 0, 0, 0, 50, 240, 200, 0x2565 );
   x = Tick_x/0.3;
   Do_Lines( Lines_1, NB_LINES1, 3.0*Rot, 1+Zoom );
   i = (INT)( 4.0-Rot*3.0 );
   j = (INT)( Rot*14.0 );
   Mixer.Mix_16_16_Mix( &VB(VSCREEN), &VB(1), &VB(2),
      Lo16_To_777[j], Hi16_To_777[j],
      Lo16_To_777[i], Hi16_To_777[i] );
}

/********************************************************************/

#if 0

EXTERN void Init_Loop_10_Sprites( )
{
   Check_CMap_VBuffer( &VB(VSCREEN), 256 );
   Init_Anti_Alias_Table( 0, 255, 1.7 );
}

/********************************************************************/
#include "ln.h"
#define FLARE_FILE "flare11.gif"
#define FLARE_NB 1
static FLARE_SPRITE Flare_Sprite[] = {         // flare1?.gif
  { 0,0, 48,48,   1.0, 0.0 }
};
static BITMAP *Flare;
static FLARE Sp_Flare;

EXTERN void Pre_Cmp_Flr( )
{
   Flare = Load_GIF( FLARE_FILE );
   Sp_Flare.Sprites = Flare_Sprite;
   Sp_Flare.Nb_Sprites = FLARE_NB;
   Sp_Flare.Bits = Flare->Bits;
   Sp_Flare.Width = Flare->Width;
   Sp_Flare.BpS = Flare->Width*sizeof(BYTE);
   Sp_Flare.Height = Flare->Height;
   Sp_Flare.Convert = (PIXEL*)New_Fatal_Object( Flare->Nb_Col, UINT );
   CMap_To_16bits( (void*)Sp_Flare.Convert, Flare->Pal, Flare->Nb_Col, 0x777 );
   M_Free( Flare->Pal );
   Flare->Nb_Col = 0;
}

/********************************************************************/

EXTERN void Loop_10_Sprites( FLT x )
{
   INT i, j;
   Install_Renderer( &Renderer_16b, RENDER_SIZE, The_W, The_H, RENDER_END_ARG );
   Select_Primitives( &Primitives_16 );
   Setup_Rendering( VB(VSCREEN).Bits, NULL, NULL, NULL, NULL,
      The_W, The_H, The_W*2 );
   Setup_Clipping( 2.0, The_W-2.0, 2.0, The_H-2.0, 1.0, 3000.0 );
   Line_16_CMap = VB(VSCREEN).CMap;
   Drv_Build_Ramp_16( VB(VSCREEN).CMap, 0, 256, 120, 125, 120, 240, 250, 240, 0x2565 );   
   for( i=0; i<NB_LINES_T; ++i )
   {
      FLT xi, yi;
      FLT xxi, yyi;
      FLT R;
      xi = Lines_T[i].xo;
      yi = Lines_T[i].yo;
      xxi = Lines_T[i].x; xxi = (xxi-xi)*x + xi;
      yyi = Lines_T[i].y; yyi = (yyi-yi)*x + yi;
      ((void (*)(FLT,FLT,FLT,FLT))Primitives[RENDER_LINE])( 
         xi*The_W, yi*The_H, xxi*The_W, yyi*The_H );
      R = .2 + x*.3 + (.4/128.0)*(TRandom(1)&0x7f);
      xxi = 1.0-xxi; xi = 1.0-xi;
      Paste_Flare_Bitmap_16_Sat( 
        xxi*The_W, yyi*The_H, R, &Sp_Flare, &Flare_Sprite[0] );
   }   
}

#endif   // 0

/********************************************************************/

// void Close_10( ) { }

/********************************************************************/
/********************************************************************/
