/***********************************************
 *         world resources bookeeping          *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/
/******************************************************************/

EXTERN void  **Obj_To_Sort = NULL;
EXTERN UINT   *Sorted = NULL;

EXTERN P_POLY  *Poly_To_Sort = NULL;
EXTERN UINT    *Poly_Keys = NULL;

EXTERN P_VERTICE *P_Vertex = NULL;
EXTERN PIXEL     *Vertex_State = NULL;

static WORLD_REAL_RSC Rsc = { 0 };
static BYTE *Base_Rsc_Ptr = NULL;
static INT Total_Rsc_Size = 0;

/******************************************************************/

EXTERN void World_Clean_Resources( )
{
   M_Free( Base_Rsc_Ptr );
   Total_Rsc_Size = 0;
   Mem_Clear( &Rsc );

   Sorted = NULL;
   Poly_Keys = NULL;
   Obj_To_Sort = NULL;
   Poly_To_Sort = NULL;
   P_Vertex = NULL;
   Vertex_State = NULL;
}

EXTERN void *World_Check_Resources( WORLD_REAL_RSC *New )
{
   INT Total;
   BYTE *Ptr;

   Total  = New->Sorted_Sz*sizeof( UINT );
   Total += New->Poly_Keys_Sz*sizeof( UINT );
   Total += New->Obj_To_Sort_Sz*sizeof( void * );
   Total += New->Poly_To_Sort_Sz*sizeof( P_POLY );
   Total += New->P_Vertex_Sz*sizeof( P_VERTICE );
   Total += New->Vertex_State_Sz*sizeof( BYTE );
   Total = (Total+7) & ~7;
   if ( Total>Total_Rsc_Size ) 
   {
      M_Free( Base_Rsc_Ptr ); Total_Rsc_Size = 0;
      Base_Rsc_Ptr = New_Fatal_Object( Total, BYTE );
      if ( Base_Rsc_Ptr == NULL ) return( NULL );
      Total_Rsc_Size = Total;
   }
   Ptr = (BYTE*)Base_Rsc_Ptr;
   Sorted = (UINT*)Ptr;          Ptr += New->Sorted_Sz*sizeof(UINT);
   Poly_Keys = (UINT*)Ptr;       Ptr += New->Poly_Keys_Sz*sizeof(UINT);
   Obj_To_Sort = (void**)Ptr;    Ptr += New->Obj_To_Sort_Sz*sizeof(void*);
   Poly_To_Sort = (P_POLY*)Ptr;  Ptr += New->Poly_To_Sort_Sz*sizeof(P_POLY);
   P_Vertex = (P_VERTICE*)Ptr;   Ptr += New->P_Vertex_Sz*sizeof(P_VERTICE);
   Vertex_State = (BYTE*)Ptr;    Ptr += New->Vertex_State_Sz*sizeof(BYTE);
   Rsc = *New;

   return( (void*)&Rsc );
}

/******************************************************************/
