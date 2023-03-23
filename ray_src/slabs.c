/***********************************************
 *           bounding slabs.c                  *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "ray.h"

/******************************************************************/

EXTERN INT Intersect_Slabs( RAY *Ray, SLABS *Slabs, FLT *Min, FLT *Max )
{
   INT i;
   SLAB *Cur;

   *Min = -_SLAB_HUGE_; *Max = _SLAB_HUGE_;

   Cur = Slabs->Slabs;

   for( i=Slabs->Nb; i>0; --i, Cur++ )
   {
      FLT D_N, Z;

      D_N = Dot_Product( Ray->Dir, Cur->N );
      if ( fabs(D_N)<_SLAB_EPSILON_ ) continue;
      Z = Dot_Product( Ray->Orig, Cur->N );
      Z = ( Cur->d - Z )/D_N;
      if ( D_N>_SLAB_EPSILON_ )
      {
         if ( *Max>Z ) *Max = Z; 
      }
      else 
      {
         if ( *Min<Z ) *Min = Z;
      }
      if ( *Min>=*Max ) return( FALSE );
   }
   if ( *Min<_SLAB_EPSILON_ ) *Min = _SLAB_EPSILON_;
   return( TRUE );
}

/******************************************************************/

static SLAB Default_Box_Slabs[6] = {
   { { 1.0, 0.0, 0.0 }, -123.0 },
   { {-1.0, 0.0, 0.0 }, -123.0 },
   { { 0.0, 1.0, 0.0 }, -123.0 },
   { { 0.0,-1.0, 0.0 }, -123.0 },
   { { 0.0, 0.0, 1.0 }, -123.0 },
   { { 0.0, 0.0,-1.0 }, -123.0 },
};

static SLABS BBox_Slabs =
{
   6, &Default_Box_Slabs[0]
};

EXTERN INT Intersect_Box( RAY *Ray, BBOX *Box, FLT *Min, FLT *Max )
{
   Default_Box_Slabs[0].d =  Box->Maxs[0];
   Default_Box_Slabs[1].d = -Box->Mins[0];
   Default_Box_Slabs[2].d =  Box->Maxs[1];
   Default_Box_Slabs[3].d = -Box->Mins[1];
   Default_Box_Slabs[4].d =  Box->Maxs[2];
   Default_Box_Slabs[5].d = -Box->Mins[2];
   return( Intersect_Slabs( Ray, &BBox_Slabs, Min, Max ) );
}

/******************************************************************/

