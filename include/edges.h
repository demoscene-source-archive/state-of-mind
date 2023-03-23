/***********************************************
 *            Edges and spans                  *
 * Skal 99                                     *
 ***********************************************/

#ifndef _EDGES_H_
#define _EDGES_H_

/******************************************************************/

typedef struct EDGE2D EDGE2D;
typedef struct SPAN SPAN;

struct EDGE2D {
#pragma pack(1)

   INT      x, dx;
   INT      Is_Left;
   SURFACE *Surface;
   EDGE2D  *Next, *Prev;
   EDGE2D  *Remove;

};


struct SPAN {
#pragma pack(1)

    INT      x, y, Count;
    INT      Dst;
    SURFACE *Surf;

};

#define MAX_EDGES_2D   5000
#define MAX_SURFACES   1000
#define MAX_HEIGHT     800

extern SURFACE  Top_Surface;
extern SPAN Poly_Span;

extern void Edges_Clear_Screen( INT Start, INT End );
extern void Edges_Init( INT Left, INT Right );
extern SURFACE *Poly2D_To_Edges( PT_2D **Cur_Pt );
extern void Poly2D_To_Edges_Direct( PT_2D **Cur_Pt, SURFACE *Surf, INT BpS );
extern void Edges_Scan( void );

/******************************************************************/

#endif   // _EDGES_H_
