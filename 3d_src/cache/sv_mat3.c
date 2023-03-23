/***********************************************
 *             cache III saving                *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"
#include "./mip_cst.h"

/******************************************************************/

EXTERN INT Save_Txt_Cache_III( STRING Name, void *C )
{
   FILE *Out;
   TXT_CACHE_III *Cache;
   MIP_BLOCK *Cur;
   INT i;
   
   Cache = (TXT_CACHE_III*)C;

   Out_Message( " * Nb_Mips: %d", Cache->Nb_Mips );
   Out_Message( " * Nb_PMips: %d", Cache->Nb_PMips );

   Out = Access_File( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( TRUE );


   fwrite( TXT_MAGIK3, 6, 1, Out );
   fprintf( Out, "%c", Cache->Nb_Mips&0xFF );
   fprintf( Out, "%c", ( Cache->Nb_Mips>>8 ) & 0xFF );

//   fprintf( Out, "%c", Cache->Nb_PMips&0xFF );
//   fprintf( Out, "%c", ( Cache->Nb_PMips>>8 ) & 0xFF );

   for( i=0; i<MIP_MAX; ++i )
   {
      UINT Tmp;
      OBJ_NODE *Last_Obj_Up = NULL, *Last_Obj_Down = NULL;

      Cur = Cache->Mips[i];
      while( Cur!=NULL )
      {
         fwrite( &Cur->Mip_Level, sizeof( BYTE ), 1, Out );
            // *Up and *Down must contain the real obj+poly ID here!!
            // TODO: little/big endians pb!!
         Tmp = *(UINT*)&Cur->Up;
         fwrite( &Tmp, sizeof( UINT ), 1, Out );
         Tmp = *(UINT*)&Cur->Down;
         fwrite( &Tmp, sizeof( UINT ), 1, Out );

         Compress_Write( (BYTE*)Cur->Bits, Out, Mip_Size[Cur->Mip_Level]*sizeof(USHORT), 8 );
         Cur = Cur->Next;
      }
   }

   fclose( Out );
   return( FALSE );
}

/******************************************************************/
