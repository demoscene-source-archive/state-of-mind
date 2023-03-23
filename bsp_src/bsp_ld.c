/***********************************************
 *              BSP                            *
 *                                             *
 * Skal 98.                                    *
 ***********************************************/

#include "main3d.h"
#include "indy.h"
#include "bsp.h"

/******************************************************************/

EXTERN BSP_TREE *BSP_Load( STRING Name )
{
   FILE *In;
   BSP_TREE *New;
   INT i, Nl, Nn, Nf;
   char Head[10];

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

   F_READ( Head, In, 6 );
   if ( strncmp( BSP_MAGIK, (char *)Head, 6 ) )
      Exit_Upon_Error( "'%s' has wrong BSP header.", Name );

   F_READ( &Nn, In, 4 ); Nn = Indy_l( Nn );
   F_READ( &Nl, In, 4 ); Nl = Indy_l( Nl );
   F_READ( &Nf, In, 4 ); Nf = Indy_l( Nf );

   New = BSP_New_Tree( Nn, Nl );
   if ( New==NULL ) goto Failed;
   New->Nb_Nodes = Nn;
   New->Free_Leaves = Nf;

   F_COMPRESS_READ( (BYTE*)New->Nodes, In, New->Nb_Nodes*sizeof( BSP_NODE ) );
   F_COMPRESS_READ( (BYTE*)New->Leaves, In, New->Max_Leaves*sizeof( BSP_LEAF ) );

   for( i=0; i<New->Nb_Nodes; ++i )
   {
      New->Nodes[i].Flags = Indy_s( New->Nodes[i].Flags );
      New->Nodes[i].Up = Indy_s( New->Nodes[i].Up );
      New->Nodes[i].Right = Indy_s( New->Nodes[i].Right );
      New->Nodes[i].ID = Indy_s( New->Nodes[i].ID );
      New->Nodes[i].Mins[0] = Indy_f( New->Nodes[i].Mins[0] );
      New->Nodes[i].Mins[1] = Indy_f( New->Nodes[i].Mins[1] );
      New->Nodes[i].Mins[2] = Indy_f( New->Nodes[i].Mins[2] );
      New->Nodes[i].Maxs[0] = Indy_f( New->Nodes[i].Maxs[0] );
      New->Nodes[i].Maxs[1] = Indy_f( New->Nodes[i].Maxs[1] );
      New->Nodes[i].Maxs[2] = Indy_f( New->Nodes[i].Maxs[2] );

      New->Nodes[i].Plane[0] = Indy_f( New->Nodes[i].Plane[0] );
      New->Nodes[i].Plane[1] = Indy_f( New->Nodes[i].Plane[1] );
      New->Nodes[i].Plane[2] = Indy_f( New->Nodes[i].Plane[2] );
      New->Nodes[i].Plane[3] = Indy_f( New->Nodes[i].Plane[3] );
   }

   for( i=0; i<New->Max_Leaves; ++i )
   {
      New->Leaves[i].Next = Indy_l( New->Leaves[i].Next );
      New->Leaves[i].Obj_ID = Indy_s( New->Leaves[i].Obj_ID );
      New->Leaves[i].Poly1 = Indy_s( New->Leaves[i].Poly1 );
      New->Leaves[i].Poly2 = Indy_s( New->Leaves[i].Poly2 );
   }

Failed:
   F_CLOSE( In );
   return( New );
}

/******************************************************************/
