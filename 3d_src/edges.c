/***********************************************
 *            Edges                            *
 * Skal 99                                     *
 ***********************************************/

#include "main3d.h"

EXTERN SPAN Poly_Span;

EXTERN SURFACE Top_Surface;
static SURFACE Surface_Stack[MAX_SURFACES];
static EDGE2D  Edge_Stack[MAX_EDGES_2D];
static EDGE2D  Edges_To_Insert[MAX_HEIGHT];
static EDGE2D *Edges_To_Remove[MAX_HEIGHT];

static EDGE2D Edge_Head;
static EDGE2D Edge_Tail;
static EDGE2D Max_Edge = {0x7FFFFFFF};    // sentinel while sorting with x

static EDGE2D   *Free_Edges = &Edge_Stack[0];
static SURFACE  *Free_Surfaces = &Surface_Stack[0];

////////////////////////////////////////////////////////////////////
// Input is a cyclic, null-terminated, list of pointers to 2d-points
////////////////////////////////////////////////////////////////////

EXTERN SURFACE *Poly2D_To_Edges( PT_2D **Cur_Pt )
{
   for( ;Cur_Pt[1]!=NULL; Cur_Pt++ )
   {
      EDGE2D *Cur;
      INT yf, yi;

      yi = (INT)ceil( (*Cur_Pt[0])[1] );
      yf = (INT)ceil( (*Cur_Pt[1])[1] );

      if ( yi==yf ) continue;
      else if ( yf<yi )    // left edge
      {
         FLT Slope;
         INT Tmp;

         Tmp = yf; yf = yi; yi = Tmp;

         Slope  = (*Cur_Pt[0])[0] - (*Cur_Pt[1])[0];
         Slope /= (*Cur_Pt[0])[1] - (*Cur_Pt[1])[1];

         Free_Edges->dx = (INT)( Slope*65536.0);      // => 16:16 fixed-point
         Free_Edges->x = (INT)( 65536.0 * ( (*Cur_Pt[1])[0]+ ((FLT)yi - (*Cur_Pt[1])[1])*Slope ) );
         Free_Edges->Is_Left = 1;
      }
      else         // right edge
      {
         FLT Slope;

         Slope  = (*Cur_Pt[1])[0] - (*Cur_Pt[0])[0];
         Slope /= (*Cur_Pt[1])[1] - (*Cur_Pt[0])[1];

         Free_Edges->dx = (INT)( 65536.0*Slope );
         Free_Edges->x = (INT)( 65536.0 * ( (*Cur_Pt[0])[0]+ ((FLT)yi - (*Cur_Pt[0])[1])*Slope ) );
         Free_Edges->Is_Left = 0;
      }

        // insert the edge on the list to be added on top scan

      Cur = &Edges_To_Insert[yi];
      while ( Cur->Next->x<Free_Edges->x ) Cur = Cur->Next;
      Free_Edges->Next = Cur->Next;
      Cur->Next = Free_Edges;

        // mark the edge as to be removed after bottom point

      Free_Edges->Remove = Edges_To_Remove[ yf-1 ];
      Edges_To_Remove[ yf-1 ] = Free_Edges;

        // associate edge with available surface

      Free_Edges->Surface = (SURFACE*)Free_Surfaces;

      if ( Free_Edges<&Edge_Stack[MAX_EDGES_2D]) Free_Edges++;
   }

      // "Create" a new surface, so we'll know how to sort and draw

   Free_Surfaces->State = 0;

   if ( Free_Surfaces<&Surface_Stack[MAX_SURFACES] ) {
      Free_Surfaces++;
      return( Free_Surfaces-1 );
   }
   else return( Free_Surfaces );
}

////////////////////////////////////////////////////////////////////

#define RND(x)  (((x) + 32767) >> 16)

EXTERN void Edges_Scan( void )
{
   Edge_Head.Next = &Edge_Tail;
   Edge_Head.Prev = NULL;

   Edge_Tail.Next = NULL;
   Edge_Tail.Prev = &Edge_Head;

   Top_Surface.Next = Top_Surface.Prev = &Top_Surface;

   Poly_Span.Dst = 0;
   for( Poly_Span.y=0; Poly_Span.y<_RCst_.Pix_Height; Poly_Span.y++ )
   {
      INT  x;
      FLT  fx, fy, w, w2;
      EDGE2D  *Edge, *Edge2;
      SURFACE *Surf, *Surf2;

         // Insert (and sort) new edges for this scanline
         // Result is a sorted list of active edges
         // between Edge_Head and Edge_Tail.

      Edge = Edges_To_Insert[Poly_Span.y].Next;
      Edge2 = &Edge_Head;
      while ( Edge!=&Max_Edge )
      {
         EDGE2D *Next;      
         while ( Edge->x > Edge2->Next->x )
            Edge2 = Edge2->Next;

         Next = Edge->Next;
         Edge->Next = Edge2->Next;
         Edge->Prev = Edge2;
         Edge2->Next->Prev = Edge;
         Edge2->Next = Edge;

         Edge2 = Edge;
         Edge = Next;
      }

           // Emit spans between edges

      Top_Surface.State = 1;
      Top_Surface.xo = 0;

      fy = -(FLT)Poly_Span.y;    // Beware of minus sign!

      for( Edge=Edge_Head.Next; Edge!=NULL; Edge=Edge->Next )
      {
         Surf = Edge->Surface;

         if ( Edge->Is_Left )
         {
            if ( ++Surf->State==1 )
            {
               fx = ( 1.0f/65536.0 )*(FLT)Edge->x;
               w = Surf->w0 + Surf->Tdw[0]*fx + Surf->Tdw[1]*fy;

               Surf2 = Top_Surface.Next;
               w2 = Surf2->w0 + Surf2->Tdw[0]*fx + Surf2->Tdw[1]*fy;
               if ( w>=w2 ) // on top
               {
                  INT Count;
                  x = RND( Edge->x );
                  Count = x - Surf2->xo;
                  if ( Count>0 )
                  {
                     Poly_Span.Count = Count;
                     Poly_Span.Surf = Surf2;
                     (*Surf2->Draw_Span)( );
                  }
                  Surf->xo = x;
                  Surf->w = w;
               }
               else  // Not visible. insert where applicable
               {
                  do {
                     Surf2 = Surf2->Next;
                     w2 = Surf2->w0 + Surf2->Tdw[0]*fx + Surf2->Tdw[1]*fy;
                  } while( w<w2 );
               }
               Surf->Next = Surf2;
               Surf->Prev = Surf2->Prev;
               Surf2->Prev->Next = Surf;
               Surf2->Prev = Surf;
            }
         }
         else
         {
            if ( --Surf->State==0 )
            {
               if ( Top_Surface.Next==Surf )
               {
                  INT Count;
                        
                  x = RND( Edge->x );
                  Count = x - Surf->xo;
                  if ( Count>0 )
                  {
                     Poly_Span.Count = Count;
                     Poly_Span.Surf = Surf;
                     (*Surf->Draw_Span)( );
                  }
                  Surf->Next->xo = x;
                  Surf->Next->w = Surf->Next->w0;
                  Surf->Next->w += Surf->Next->Tdw[0]*x;
                  Surf->Next->w += Surf->Next->Tdw[1]*fy;
               }
                  // Remove from list
               Surf->Next->Prev = Surf->Prev;
               Surf->Prev->Next = Surf->Next;
            }
         }
      }

        // discard processed edges

      Edge = Edges_To_Remove[Poly_Span.y];
      Edges_To_Remove[Poly_Span.y] = NULL;
      while( Edge!=NULL )
      {
         Edge->Prev->Next = Edge->Next;
         Edge->Next->Prev = Edge->Prev;
         Edge = Edge->Remove;
      }

        // update the remaining edges one scanline, and re-sort

      Edge = Edge_Head.Next;
      while( Edge!=&Edge_Tail )
      {
         EDGE2D *Next = Edge->Next;

         Edge->x += Edge->dx;
         while( Edge->x<Edge->Prev->x )
         {
            Edge2 = Edge->Prev;
            Edge2->Next = Edge->Next;
            Edge->Next->Prev = Edge2;
            Edge2->Prev->Next = Edge;
            Edge->Prev = Edge2->Prev;
            Edge->Next = Edge2;
            Edge2->Prev = Edge;
         }
         Edge = Next;
      }

         // next scanline. Clear previous edges behind us

      Edges_To_Insert[Poly_Span.y].Next = &Max_Edge;
      Poly_Span.Dst += _RCst_.Pix_BpS;
   }
      // restore pointers

   Free_Edges = Edge_Stack;
   Free_Surfaces = Surface_Stack;
}

////////////////////////////////////////////////////////////////////

EXTERN void Edges_Clear_Screen( INT Start, INT End )
{
   INT i;

   for ( i=Start; i<End; i++ )
   {
      Edges_To_Insert[i].Next = &Max_Edge;
      Edges_To_Remove[i] = NULL;
   }
      // init stack
   Free_Edges = Edge_Stack;
   Free_Surfaces = Surface_Stack;
}


EXTERN void Edges_Init( INT Left, INT Right )
{
   Edge_Head.Next = &Edge_Tail;
   Edge_Head.Prev = NULL;
   Edge_Head.x = (Left<<16)-0xFFFF;
   Edge_Head.Is_Left = 1;
   Edge_Head.Surface = &Top_Surface;

   Edge_Tail.Next = NULL;
   Edge_Tail.Prev = &Edge_Head;
   Edge_Tail.x = Right<<16;
   Edge_Tail.Is_Left = 0;
   Edge_Tail.Surface = &Top_Surface;

      // This actually needs being done once at start

   Edges_Clear_Screen( 0, MAX_HEIGHT );
}

////////////////////////////////////////////////////////////////////

EXTERN void Poly2D_To_Edges_Direct( 
   PT_2D **Cur_Pt, SURFACE *Surf, INT BpS )
{
       // use Edges_To_Remove as stack

   Poly_Span.Surf = Surf;

   for( ;Cur_Pt[1]!=NULL; Cur_Pt++ )
   {
      INT yf, yi;
      INT y, x, ix, dx;
      FLT Slope;

      yi = (INT)ceil( (*Cur_Pt[0])[1] );
      yf = (INT)ceil( (*Cur_Pt[1])[1] );

      if ( yi==yf ) continue;
      else if ( yf<yi )    // left edge
      {
         y = yf; yf = yi; yi = y;
         Slope  = (*Cur_Pt[0])[0] - (*Cur_Pt[1])[0];
         Slope /= (*Cur_Pt[0])[1] - (*Cur_Pt[1])[1];
         x = (INT)( 65536.0 * ( (*Cur_Pt[1])[0]+ ((FLT)yi - (*Cur_Pt[1])[1])*Slope ) );
      }
      else         // right edge
      {
         Slope  = (*Cur_Pt[1])[0] - (*Cur_Pt[0])[0];
         Slope /= (*Cur_Pt[1])[1] - (*Cur_Pt[0])[1];
         x = (INT)( 65536.0 * ( (*Cur_Pt[0])[0]+ ((FLT)yi - (*Cur_Pt[0])[1])*Slope ) );
      }
            // scan

      dx = (INT)( 65536.0*Slope );
      Poly_Span.Dst = yi*BpS;
      Poly_Span.y = yi;
      for( y=yi; y<yf; ++y )
      {
         if ( Edges_To_Remove[y]==NULL )
         {
            Edges_To_Remove[y] = Free_Edges;
            if ( Free_Edges<&Edge_Stack[MAX_EDGES_2D]) Free_Edges++;
            Edges_To_Remove[y]->x = RND(x);
         }
         else
         {
            ix = RND(x);
            if ( ix<Edges_To_Remove[y]->x )
            {
               Poly_Span.x = ix;
               Poly_Span.Count = Edges_To_Remove[y]->x - Poly_Span.x;
            }
            else
            {
               Poly_Span.x = Edges_To_Remove[y]->x;
               Poly_Span.Count = ix - Poly_Span.x;
            }
            if ( Poly_Span.Count>0 )
            {
               Surf->xo = (SHORT)Poly_Span.x;
               (*Surf->Draw_Span)( );
            }
            Edges_To_Remove[y] = NULL;
         }
         x += dx;
         Poly_Span.y++;
         Poly_Span.Dst += BpS;
      }
   }
         // restore stack
   Free_Edges = &Edge_Stack[0];
}

////////////////////////////////////////////////////////////////////
