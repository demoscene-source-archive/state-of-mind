/***********************************************
 *              cache II loading               *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "indy.h"

/******************************************************************/

EXTERN void *Load_Txt_Cache_II( STRING Name )
{
   FILE *In;
   TXT_CACHE *New;
   char Head[20];
   PIXEL C1[2];
   INT Nb_Maps, i; 

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

   F_READ( Head, In, 6 );        /* *Do* use F_READ */
   if ( strncmp( TXT_MAGIK2, (char *)Head, 6 ) )
   {
      Exit_Upon_Error( "'%s' has wrong Txt_Cache header.", Name );
   }

   F_READ( &C1, In, 2 );
   Nb_Maps = C1[0] + C1[1]*256;

      // Nb_Maps must be upper multiple of 3

   New = (TXT_CACHE*)Allocate_Texture_Maps_II( NULL, Nb_Maps/3 );
   if ( New==NULL ) goto End;
   // New->Nb_Texture_Maps = Nb_Maps;

   if ( Nb_Maps==0 ) goto Skip1;
   for ( i=0; i<Nb_Maps; ++i )
   {
      F_READ( &New->Maps[i], In, sizeof( TEXTURE_MAP ) );
      New->Maps[i].Users = Indy_s( New->Maps[i].Users );
      New->Maps[i].Quantum = Indy_s( New->Maps[i].Quantum );
      New->Maps[i].Offset = Indy_s( New->Maps[i].Offset );
      New->Maps[i].Ptr = New->Texture_Maps_Ptr 
         + New->Maps[i].Base*TEXTURE_MAP_SIZE_16b;
   }
   F_COMPRESS_READ( New->Texture_Maps_Ptr, In, TEXTURE_MAP_SIZE_16b*( Nb_Maps/3 ) );

   if (Indy_s(0x1234)!=0x1234) 
   {
      USHORT *Ptr = (USHORT*)New->Texture_Maps_Ptr;
      for( i=0; i<(TEXTURE_MAP_SIZE_16b/2)*( Nb_Maps/3 ); ++i )
         Ptr[i] = Indy_s(Ptr[i]);
   }

Skip1:
   New->CMap = NULL;
   New->Nb_Col = 0;
   New->Nb_Offset = 0;

//   fprintf( stderr, " Nb_Textures: %d     Nb cols: %d   Nb_Offsets:%d\n",
//      New->Nb_Texture_Maps, New->Nb_Col, New->Nb_Offset );

End:
   F_CLOSE( In );
   return( (void*)New );
}

/******************************************************************/
