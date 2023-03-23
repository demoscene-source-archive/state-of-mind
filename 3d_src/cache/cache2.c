/***********************************************
 *              CACHE II                       *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "mem_map.h"
#include "mem_btm.h"

 /******************************************************************/
/******************************************************************/

EXTERN void *Allocate_Texture_Maps_II( void *C, INT Nb )
{
   TXT_CACHE *Cache;
   INT i;

   Cache = (TXT_CACHE *)C;
   if ( Cache==NULL )
   {
      Cache = New_Fatal_Object( 1, TXT_CACHE );
      Mem_Clear( Cache );
   }

   if ( Nb==Cache->Nb_Texture_Maps ) goto No_Clear;
   Clear_Cache( Cache );

   if ( Malloc_64( 
      &Cache->Texture_Maps_Real_Ptr, &Cache->Texture_Maps_Ptr, 
      Nb*TEXTURE_MAP_SIZE_16b ) == NULL )
      return( NULL );

   Cache->Maps = (TEXTURE_MAP *)New_Object( 3*Nb, TEXTURE_MAP );
   if ( Cache->Maps == NULL )
   {
      Clear_Cache( Cache );
      return( NULL );
   }   
   Cache->Nb_64k_Bunch = Nb;

No_Clear:
   memset( Cache->Texture_Maps_Ptr, 0, Nb*TEXTURE_MAP_SIZE_16b );

   for( i=0; i<Nb; ++i )
   {
      INT Map;
      Map = i;
      Cache->Maps[3*i].Type = TXT_VOID;
      Cache->Maps[3*i].Users = 0;
      Cache->Maps[3*i].Quantum = 0;
      Cache->Maps[3*i].Ptr = Cache->Texture_Maps_Ptr + Map*TEXTURE_MAP_SIZE_16b;
      Cache->Maps[3*i].Slot = 0;
      Cache->Maps[3*i].Base = (BYTE)Map;
      Cache->Maps[3*i].Offset = 0;
      Cache->Maps[3*i+1].Type = TXT_VOID;
      Cache->Maps[3*i+1].Users = 0;
      Cache->Maps[3*i+1].Quantum = 0;
      Cache->Maps[3*i+1].Ptr = Cache->Texture_Maps_Ptr + Map*TEXTURE_MAP_SIZE_16b;
      Cache->Maps[3*i+1].Slot = 1;
      Cache->Maps[3*i+1].Base = (BYTE)Map;
      Cache->Maps[3*i+1].Offset = 0;
      Cache->Maps[3*i+2].Type = TXT_VOID;
      Cache->Maps[3*i+2].Users = 0;
      Cache->Maps[3*i+2].Quantum = 0;
      Cache->Maps[3*i+2].Ptr = Cache->Texture_Maps_Ptr + Map*TEXTURE_MAP_SIZE_16b;
      Cache->Maps[3*i+2].Slot = 2;
      Cache->Maps[3*i+2].Base = (BYTE)Map;
      Cache->Maps[3*i+2].Offset = 0;
   }
   Cache->Nb_Texture_Maps = 3*Nb;
   Cache->CMap = NULL;
   Cache->Nb_Col = 0;
   Cache->Nb_Offset = 0;
   return( Cache );
}

/******************************************************************/
/******************************************************************/

