/***********************************************
 *              CACHE I                        *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "./mip_cst.h"
#include "mem_map.h"
#include "mem_btm.h"

/******************************************************************/
/******************************************************************/

static TEXTURE_MAP *Cache_Fetch_Map( TXT_CACHE *C, INT Map )
{
   if ( Map==-1 ) return( NULL );
   if ( C->Nb_Texture_Maps==0 ) return( NULL );
   if ( Map>=C->Nb_Texture_Maps ) Map = C->Nb_Texture_Maps-1;
   return( &C->Maps[Map] );
}

static void Setup_Material( void *C, OBJECT *Dum )
{
   MATERIAL *M = (MATERIAL*)Dum;
   TXT_CACHE *Cache = (TXT_CACHE*)C;

   if ( M->Mapped == TRUE ) return;

#ifdef DEBUG_CACHE
   fprintf ( stderr, "Map file: %s       ", M->Txt1 );
   fprintf( stderr, "Mapping: %d / %d / %d \n",
      M->Map_Nb1, M->Map_Nb2, M->Map_Nb3 );
#endif
   M_Free( M->Txt1 );

   if ( M->Txt1==NULL ) M->Txt1 = Cache_Fetch_Map( Cache, M->Map_Nb1 );
   if ( M->Txt2==NULL ) M->Txt2 = Cache_Fetch_Map( Cache, M->Map_Nb2 );
   if ( M->Txt3==NULL ) M->Txt3 = Cache_Fetch_Map( Cache, M->Map_Nb3 );

#ifdef DEBUG_CACHE
   if ( M->Txt1!=NULL )
   {
      fprintf( stderr, "    Txt1: 0x%x  ->Ptr: 0x%x   ->Slot:%d\n",
         M->Txt1, M->Txt1->Ptr, M->Txt1->Slot );
   }
#endif
   M->Mapped = TRUE;
}

/******************************************************************/
/******************************************************************/

static USHORT Poly_Flat_Color( )
{
   return( (USHORT)( Cur_Texture_Ptr[Mip_Slot0_Offsets[Cur_Mip_Slot]] ) );
}

static INT Retreive_Texture( INT Mip_Slot, FLT R )
{
   if ( R<EPS_MIP_BLF*EPS_MIP_BLF ) Cur_Mip = 7;  /* Bilinear filtering !! */
   else if ( R<EPS_MIP_0*EPS_MIP_0 ) Cur_Mip = 7;
   else if ( R<EPS_MIP_1*EPS_MIP_1 ) Cur_Mip = 6;
   else if ( R<EPS_MIP_2*EPS_MIP_2 ) Cur_Mip = 5;
   else if ( R<EPS_MIP_3*EPS_MIP_3 ) Cur_Mip = 4;
   else if ( R<EPS_MIP_4*EPS_MIP_4 ) Cur_Mip = 3;
   else if ( R<EPS_MIP_5*EPS_MIP_5 ) Cur_Mip = 2;
   else Cur_Mip = 1;
   if ( Cur_Mip<3 )
   {
      Flat_Color = Poly_Flat_Color( );
      Mip_Mask = Mip_Masks[Cur_Mip];
      return( Cur_Mip );
   }
   Mip_Scale = Mip_Scales[Cur_Mip];
   Uo_Mip = Mip_Uo[Mip_Slot][Cur_Mip];
   Vo_Mip = Mip_Vo[Mip_Slot][Cur_Mip];
   Mip_Mask = Mip_Masks[Cur_Mip];
   UV_Src = Cur_Texture_Ptr;
   return( Cur_Mip );
}

static INT Compute_MipMap( )
{
   FLT R;

      // stored, for later use...
   dU1 = ( P_Edges[1]->UV[0]-P_Edges[0]->UV[0] );
   dU2 = ( P_Edges[2]->UV[0]-P_Edges[1]->UV[0] );

   dV1 = ( P_Edges[1]->UV[1]-P_Edges[0]->UV[1] );
   dV2 = ( P_Edges[2]->UV[1]-P_Edges[1]->UV[1] );

   R = (FLT)fabs( Area*Orig_Poly->U1V2_U2V1 );
   if ( Retreive_Texture( Cur_Mip_Slot, R )<3 )
      return( TRUE );
   return( FALSE );
}

static INT Compute_MipMap_Env( )
{
   FLT R;

   dU1 = ( P_Edges[1]->UV[0]-P_Edges[0]->UV[0] );
   dU2 = ( P_Edges[2]->UV[0]-P_Edges[1]->UV[0] );

   dV1 = ( P_Edges[1]->UV[1]-P_Edges[0]->UV[1] );
   dV2 = ( P_Edges[2]->UV[1]-P_Edges[1]->UV[1] );

         // don't use U1V2_U2V1 field, since it's env-mapping

   R = (FLT)fabs( Area * ( dU1*dV2-dU2*dV1 ) );
   if ( Retreive_Texture( Cur_Mip_Slot, R )<3 )
      return( TRUE );
   return( FALSE );
}

static void Init_From_Poly( )
{
   Cur_Material = (MATERIAL*)Orig_Poly->Ptr;
   if ( Cur_Material != NULL )
   {
      Cur_Texture = Cur_Material->Txt1;
      if ( Cur_Texture!=NULL )
      {
         Cur_Mip_Slot = Cur_Texture->Slot;  
         Cur_Texture_Ptr = Cur_Texture->Ptr;
      }
      else Cur_Texture_Ptr = NULL;
   }    
   else
   {
      Cur_Texture = NULL;
      Cur_Texture_Ptr = NULL;
   }
   UV_Src = NULL; 
}

static void Init_From_Poly_II( )
{
   Cur_Texture_Ptr = Cur_Material->Txt2->Ptr;
   Cur_Mip_Slot = Cur_Material->Txt2->Slot;
   UV_Src2 = Cur_Material->Txt3->Ptr;  // the env. map
   UV_Src = NULL;
}

static void Init_From_Poly_III( )
{
}


/******************************************************************/
/******************************************************************/

EXTERN void Clear_Cache( void *C )
{
   TXT_CACHE *Cache;

   Cache = (TXT_CACHE *)C;
   if ( Cache==NULL ) return;
   M_Free( Cache->Texture_Maps_Real_Ptr );
   Cache->Texture_Maps_Ptr = NULL;
   Cache->Nb_64k_Bunch = 0;
   Cache->Nb_Texture_Maps = 0;
   M_Free( Cache->Maps );
   M_Free( Cache->CMap );
   Cache->Nb_Col = 0;
   Cache->Nb_Offset = 0;
}

EXTERN void *Allocate_Texture_Maps_I( void *C, INT Nb )
{
   TXT_CACHE *Cache;
   INT i;

   Cache = (TXT_CACHE *)C;
   if ( Cache==NULL ) Cache = New_Fatal_Object( 1, TXT_CACHE );

   if ( Nb==Cache->Nb_Texture_Maps ) goto No_Clear;
   Clear_Cache( Cache );

   if ( Malloc_64( 
      &Cache->Texture_Maps_Real_Ptr, &Cache->Texture_Maps_Ptr, 
      Nb*TEXTURE_MAP_SIZE ) == NULL )
      return( NULL );

   Cache->Maps = (TEXTURE_MAP *)New_Object( 3*Nb, TEXTURE_MAP );
   if ( Cache->Maps == NULL )
   {
      Clear_Cache( Cache );
      return( NULL );
   }   

No_Clear:
   memset( Cache->Texture_Maps_Ptr, 0, Nb*TEXTURE_MAP_SIZE );

   for( i=0; i<Nb; ++i )
   {
      INT Map;
      Map = i;
      Cache->Maps[3*i].Type = TXT_VOID;
      Cache->Maps[3*i].Users = 0;
      Cache->Maps[3*i].Quantum = 0;
      Cache->Maps[3*i].Ptr = Cache->Texture_Maps_Ptr + Map*TEXTURE_MAP_SIZE;
      Cache->Maps[3*i].Slot = 0;
      Cache->Maps[3*i].Base = (BYTE)Map;
      Cache->Maps[3*i].Offset = 0;
      Cache->Maps[3*i+1].Type = TXT_VOID;
      Cache->Maps[3*i+1].Users = 0;
      Cache->Maps[3*i+1].Quantum = 0;
      Cache->Maps[3*i+1].Ptr = Cache->Texture_Maps_Ptr + Map*TEXTURE_MAP_SIZE;
      Cache->Maps[3*i+1].Slot = 1;
      Cache->Maps[3*i+1].Base = (BYTE)Map;
      Cache->Maps[3*i+1].Offset = 0;
      Cache->Maps[3*i+2].Type = TXT_VOID;
      Cache->Maps[3*i+2].Users = 0;
      Cache->Maps[3*i+2].Quantum = 0;
      Cache->Maps[3*i+2].Ptr = Cache->Texture_Maps_Ptr + Map*TEXTURE_MAP_SIZE;
      Cache->Maps[3*i+2].Slot = 2;
      Cache->Maps[3*i+2].Base = (BYTE)Map;
      Cache->Maps[3*i+2].Offset = 0;
   }
   Cache->Nb_64k_Bunch = Nb;
   Cache->Nb_Texture_Maps = 3*Nb;
   Cache->CMap = NULL;
   Cache->Nb_Col = 0;
   Cache->Nb_Offset = 0;
   return( Cache );
}

#if 0    // useless

EXTERN TEXTURE_MAP *Fill_Texture_Slot( TXT_CACHE *Cache,
   PIXEL *Ptr, MAP_TYPE Type )
{
   INT i, j, Quantum;

   if ( Cache->Maps==NULL ) return( NULL );

   Quantum = Txt_Quantum[ Type ];

   switch( Quantum )
   {
      default: case 1:
         for( i=0; i<Cache->Nb_Texture_Maps; ++i )
         {
            if ( Cache->Maps[i].Users!=0 ) continue;
            Cache->Maps[i].Users = 1;
            Cache->Maps[i].Quantum = Quantum;
            Cache->Maps[i].Type = Type;
            if ( Ptr != NULL )
            {
               MemCopy( Cache->Maps[i].Ptr, Ptr, TEXTURE_MAP_SIZE );
            }
            return( Cache->Maps + i );
         }
      break;
      case 2:
         for( i=1; i<Cache->Nb_Texture_Maps; i+=3 )
         {
            if ( (Cache->Maps[i].Users!=0)||(Cache->Maps[i+1].Users!=0) ) continue;
            Cache->Maps[i].Users = 1;
            Cache->Maps[i].Quantum = Quantum;
            Cache->Maps[i].Type = Type;
            Cache->Maps[i+1].Users = 1;
            Cache->Maps[i+1].Quantum = Quantum;
            Cache->Maps[i+1].Type = Type;
            if ( Ptr != NULL )
            {
               MemCopy( Cache->Maps[i].Ptr, Ptr, Quantum*TEXTURE_MAP_SIZE );
            }
            return( Cache->Maps + i );
         }
      break;
   }
   return( NULL );
}

#endif   // 0

/******************************************************************/

EXTERN CACHE_METHODS Cache_Methods_I = 
{
   Clear_Cache,
   Setup_Material,
   Retreive_Texture,
   Compute_MipMap,
   Compute_MipMap_Env,
   Init_From_Poly,
   Init_From_Poly_II,
   Init_From_Poly_III,
   Poly_Flat_Color
};

EXTERN CACHE_METHODS Cache_Methods_II = 
{
   Clear_Cache,
   Setup_Material,
   Retreive_Texture,
   Compute_MipMap,
   Compute_MipMap_Env,
   Init_From_Poly,
   Init_From_Poly_II,
   Init_From_Poly_III,
   Poly_Flat_Color
};

/******************************************************************/
/******************************************************************/

