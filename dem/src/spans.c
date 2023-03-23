/*
 * Spans for line (test.unused.)
 *
 * Skal 98
 ***************************************/

#include "spans.h"

/********************************************************************/

typedef struct SEG SEG;
struct SEG {
   SEG *Next;
   FLT xi, xf;
   INT    Data1;
   USHORT Data2;
   USHORT Flag;
};
static BYTE SFlags[The_H];
static SEG *SFirst[The_H];
static SEG *Base_Seg, *Free;
static INT Max_Seg;

#define THROW_SEG(S) { (S)->Next = Free; Free = (S); }
#define NEW_SEG(a)   { (a)=Free; Free = Free->Next; } // Warning!!

/********************************************************************/

static Free_Span_Chain( INT y )
{
   SEG *Cur = SFirst[y];
   while( Cur->Next!=NULL ) Cur=Cur->Next;
   Cur->Next = Free;
   Free = SFirst[y];
}

EXTERN void Init_Spans_Pass( )
{
   INT i;
   for( i=0; i<The_H; ++i )
   {
      NEW_SEG( SFirst[i] );
      SFirst[i]->xi = _RCst_.Clips[4];
      SFirst[i]->xf = _RCst_.Clips[5];
      SFirst[i]->Flag = 0x00;
      SFlags[i] = 0;
   }
}

EXTERN void Init_Spans( )
{
   INT i;

   Max_Seg = 3000;
   Base_Seg = New_Fatal_Object( Max_Seg, SEG );
   Free = Base_Seg;
   for( i=0; i<Max_Seg-1; ++i )
   {
      Base_Seg[i].Next = &Base_Seg[i+1];
      Base_Seg[i].xi = _RCst_.Clips[4];
      Base_Seg[i].xf = _RCst_.Clips[5];
      Base_Seg[i].Flag = 0x00;
   }
   Base_Seg[i].Next = NULL;
   Base_Seg[i].xi = _RCst_.Clips[4];
   Base_Seg[i].xf = _RCst_.Clips[5];
   Base_Seg[i].Flag = 0x00;

   Init_Spans_Pass( );
}

/********************************************************************/

static void Paint( FLT xi, FLT xf, INT y )
{
   INT i, j;
   USHORT *Dst;
   i = (INT)floor(xi);
   j = (INT)floor(xf);
   Dst = ((USHORT*)VB(VSCREEN).Bits) + y*VB(VSCREEN).BpS;
   Dst += j;
   i = i-j;
   for( ; i<0; ++i ) Dst[i] = 0xffff;
}

   // Flag: type (gauche/droite) de la moitié gauche
   // SFlags: bit0->set=gauche/clr=droite   // bit1:set=filled spans

static void Insert_Span( FLT xi, FLT xf, INT y, BYTE Flag )
{
   SEG *Cur, **Prev;
   Cur = SFirst[y];
   Prev = &SFirst[y];
   for( ;Cur!=NULL; Prev = &Cur->Next, Cur=Cur->Next )
   {
      if ( xi<=Cur->xi )
      {
         if ( xf<Cur->xi ) {           /* case 1 */ 
            break;
         }
         else if ( xf<Cur->xf ) {      /* case 2 */
            Paint( Cur->xi, xf, y );            
            Cur->xi = xf;              // Change[xf,Cur->xf]
            break;
         }
         else 
         {
            if ( xf==Cur->xf )  {      /* case 4 */
               Paint( Cur->xi, Cur->xf, y );
               *Prev = Cur->Next;      // <= delete[]
               break;
            }
            else {                     /* case 6 */ 
               Paint( Cur->xi,Cur->xf, y );
               *Prev = Cur->Next;      // <= delete[]
            }
         }
      }
      else
      {
         if ( xi>Cur->xf ) {           /* case 8 */ 
            continue;
         }
         else if ( xf<Cur->xf ) {      /* case 3 */
            Paint( xi, xf, y );
            // Split(xi,xf)
            break;
         }
         else if ( xf==Cur->xf )  {    /* case 5 */
            Paint( xi, xf, y );
            Cur->xf = xi;              // Change[Cur->xi,xi]
            break;
         }
         else {                        /* case 7 */
            Paint( xi, Cur->xf, y );            
            Cur->xf = xi;              // Change[Cur->xi,xi]
         }
      }
   }
} 

/********************************************************************/

EXTERN void Draw_Spans_I( VBUFFER *V )
{
   INT y, BpS;
   INT Flag;
   USHORT *Dst;

   Dst = (USHORT*)V->Bits;
   BpS = V->BpS;
   for( y=0; y<V->H; ++y )
   {
      SEG *Cur;
      Cur = SFirst[y];
      Flag = SFlags[y];
      fprintf( stderr, "y=%d : ", y );
      while( Cur->Next!=NULL )
      {
         fprintf( stderr, "[%d,%d] ", Cur->xi, Cur->xf );
         Cur = Cur->Next;
      }
      fprintf( stderr, "\n" );
      Dst = (USHORT*)((BYTE*)Dst + BpS);
      Free_Span_Chain( y );
   }
}

/********************************************************************/

EXTERN void Draw_Span_Line( FLT xo, FLT yo, FLT x1, FLT y1 )
{
   FLT dx, dy, S;
   INT yi, yf;

   if ( y1<yo ) { 
      FLT Tmp;
      Tmp = yo; yo=y1; y1=Tmp;
      Tmp = xo; xo=x1; x1=Tmp;
   }

   dy = y1-yo; dx = x1-xo;

   if ( dy<.001 ) return;     // Horizontal
   {
      INT y, x, xf;
      if ( dx<0.0 ) { FLT Tmp = xo; xo=x1; x1=Tmp; }
      if ( (xo>=_RCst_.Clips[1]) || (x1<_RCst_.Clips[0]) ) return;
      if ( xo<_RCst_.Clips[0] ) xo = _RCst_.Clips[0];
      if ( x1>_RCst_.Clips[1] ) x1 = _RCst_.Clips[1];
      y = (INT)ceil(yo);
      x = (INT)ceil( xo ); xf = (INT)ceil( x1 );
      for( ; x<xf; ++x )
      { 
      }
      return;
   }

   S = fabs( dx );
   if ( S<.001 )      // Vertical
   {
      INT x, y, yf;

      if ( (xo>=_RCst_.Clips[1]) || (xo<(_RCst_.Clips[0]+1.0)) ) return;
      x = (INT)ceil(xo);
      if ( y1>=_RCst_.Clips[3] ) y1 = _RCst_.Clips[3];
      if ( yo<_RCst_.Clips[2] ) yo = _RCst_.Clips[2];
      y = (INT)ceil( yo ); yf = (INT)ceil( y1 );

      for( ; y<yf; ++y ) 
      { 
      }
      return;
   }

   xo += (_RCst_.Clips[2]-yo)*dx/dy; yo = _RCst_.Clips[2];
   x1 += (_RCst_.Clips[3]-y1)*dx/dy; y1 = _RCst_.Clips[3];

   if ( S<dy )    // >45'
   {
      INT xi;

      S = dx/dy;     // <1.0

         // Clip x
      if ( S<0.0 )
      {
         if ( (xo<_RCst_.Clips[0]) || (x1>=_RCst_.Clips[1]) ) return;
         yo += (_RCst_.Clips[1]-xo)/S; xo = _RCst_.Clips[1];
         y1 += (_RCst_.Clips[0]-x1)/S; x1 = _RCst_.Clips[0];
      }
      else
      {
         if ( (xo>=_RCst_.Clips[1]) || (x1<_RCst_.Clips[0]) ) return;
         yo += (_RCst_.Clips[0]-xo)/S; xo = _RCst_.Clips[0];
         y1 += (_RCst_.Clips[1]-x1)/S; x1 = _RCst_.Clips[1];
      }

      yi = (INT)ceil( yo );
      xo += S*( (FLT)yi-yo );
      xi = (INT)ceil( xo );      
      yf = (INT)ceil( y1 );

      if ( dx>0.0 )
      {
         yf -= yi;
         while( yf-- )
         {
         }
      }
      else
      {
         yf -= yi;
         while( yf-- )
         {
         }
      }
   }
   else // <45'
   {
      INT xi, xf;

      S = dy/dx;     // <1.0

         // Clip x
      if ( S<0.0 )
      {
         if ( (xo<_RCst_.Clips[0]) || (x1>=_RCst_.Clips[1]) ) return;
         yo += (_RCst_.Clips[1]-xo)*S; xo = _RCst_.Clips[1];
         y1 += (_RCst_.Clips[0]-x1)*S; x1 = _RCst_.Clips[0];
      }
      else
      {
         if ( (xo>=_RCst_.Clips[1]) || (x1<_RCst_.Clips[0]) ) return;
         yo += (_RCst_.Clips[0]-xo)*S; xo = _RCst_.Clips[0];
         y1 += (_RCst_.Clips[1]-x1)*S; x1 = _RCst_.Clips[1];
      }

      xi = (INT)ceil( xo );
      yo += ( (FLT)xi-xo )*S;
      yi = (INT)ceil( yo );
      xf = (INT)ceil( x1 );

      if ( dx>0.0 )
      {
         xf -= xi;
         while( xf-- )
         {
         }
      }
      else
      {
         xi -= xf;
         while( xi-- )
         {
         }
      }
   }
}

/********************************************************************/


