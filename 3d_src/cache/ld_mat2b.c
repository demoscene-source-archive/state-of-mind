/***********************************************
 *              cache II loading               *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "indy.h"

/******************************************************************/

EXTERN void *Load_Txt_Cache_IIbis( STRING Name, STRING Name2 )
{
   FILE *In;
   TXT_CACHE *New;
   char Head[20];
   PIXEL C1[2];
   INT Nb_Maps, i, j; 
   BITMAP_16 *Add = NULL;

   Add = Load_JPEG_565( Name2, &i );
   if ( Add==NULL ) return( NULL );
   if ( i!=3 ) { Destroy_16b( Add ); return( NULL ); }

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
      TEXTURE_MAP *TMap = &New->Maps[i];
      // TODO: Indian problems here!!!
      F_READ( TMap, In, sizeof( TEXTURE_MAP ) );
      if ( Indy_s(0x1234)!=0x1234 ) {
        if ( sizeof(MAP_TYPE)==2 ) TMap->Type = (MAP_TYPE)Indy_s(TMap->Type);
        else TMap->Type = (MAP_TYPE)Indy_l(TMap->Type);
        TMap->Users = Indy_s(TMap->Users);
        TMap->Quantum = Indy_s(TMap->Quantum);
        TMap->Offset = Indy_s(TMap->Offset);
      }
      New->Maps[i].Ptr = New->Texture_Maps_Ptr 
         + New->Maps[i].Base*TEXTURE_MAP_SIZE_16b;
   }
#if 0
   if ( Indy_s(0x1234)!=0x1234 ) {
      INT i;
      for( i=0; i<(TEXTURE_MAP_SIZE_16b/2)*( Nb_Maps/3 ); ++i )
        ((USHORT*)New->Texture_Maps_Ptr)[i]=((USHORT*)Add->Bits)[i];      
   } else 
#endif
   memcpy( New->Texture_Maps_Ptr, Add->Bits, TEXTURE_MAP_SIZE_16b*( Nb_Maps/3 ) );

Skip1:
   New->CMap = NULL;
   New->Nb_Col = 0;
   New->Nb_Offset = 0;

//   fprintf( stderr, " Nb_Textures: %d     Nb cols: %d   Nb_Offsets:%d\n",
//      New->Nb_Texture_Maps, New->Nb_Col, New->Nb_Offset );

End:
   Destroy_16b( Add );
   F_CLOSE( In );
   return( (void*)New );
}

/******************************************************************/
