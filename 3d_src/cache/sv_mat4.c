/***********************************************
 *          cache IV saving                    *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"
#include "./mip_cst.h"

  // from ijpeg.h
extern BYTE *JPEG_Out( STRING Name, BYTE *Bits, 
   INT Width, INT Height, INT Type );

/******************************************************************/

static void Extract_8x8_Block( USHORT *Dst, USHORT *Src, INT Width )
{
   INT j;
   for( j=0; j<8; ++j )
   {
      memcpy( Dst, Src, 8*sizeof( USHORT ) );
      Dst += MIP_WIDTH_IV;
      Src += Width;
   }
}

EXTERN INT Save_Txt_Cache_IV( STRING Name, void *C )
{
   FILE *Out;
   TXT_CACHE_III *Cache;
   INT i;
   INT Total_Size, Cur_Store;
   USHORT *Store = NULL;

   Cache = (TXT_CACHE_III*)C;

   Out = Access_File( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( TRUE );

   fwrite( TXT_MAGIK4, 6, 1, Out );
   fprintf( Out, "%c", Cache->Nb_Mips&0xFF );
   fprintf( Out, "%c", ( Cache->Nb_Mips>>8 ) & 0xFF );

//   fprintf( Out, "%c", Cache->Nb_PMips&0xFF );
//   fprintf( Out, "%c", ( Cache->Nb_PMips>>8 ) & 0xFF );

   Total_Size = 0;
   Cur_Store = 0;
   for( i=MIP_INF; i<MIP_MAX; ++i )
   {
      MIP_BLOCK *Cur;      
      for( Cur = Cache->Mips[i]; Cur!=NULL; Cur=Cur->Next )
      {
         Total_Size += Mip_Width[i]*Mip_Width[i]; // N*N, not N*(N+1)
         Cur_Store++;
      }
   }
   if ( Total_Size )
   {
         // pad to MIP_WIDTH_IV*8
      if ( Total_Size&(8*MIP_WIDTH_IV-1) )
         Total_Size = (Total_Size|(8*MIP_WIDTH_IV-1))+1;
      Store = New_Fatal_Object( Total_Size, USHORT );
      if ( Store == NULL ) return( TRUE );
   }
   Out_Message( " * Nb_Mips: %d", Cache->Nb_Mips );
   Out_Message( " * Nb_PMips: %d", Cache->Nb_PMips );
   Out_Message( " * Nb Jpeg to save: %d (%d USHORT)", Cur_Store, Total_Size );

   Cur_Store = 0;
   for( i=0; i<MIP_MAX; ++i )
   {
      MIP_BLOCK *Cur;
      UINT Tmp;
      OBJ_NODE *Last_Obj_Up = NULL, *Last_Obj_Down = NULL;

      for ( Cur = Cache->Mips[i]; Cur!=NULL; Cur = Cur->Next )
      {
         fwrite( &Cur->Mip_Level, sizeof( BYTE ), 1, Out );

            // *Up and *Down must contain the real obj+poly ID here!!
            // TODO: little/big endians pb!!

         Tmp = *(UINT*)&Cur->Up;
         fwrite( &Tmp, sizeof( UINT ), 1, Out );
         Tmp = *(UINT*)&Cur->Down;
         fwrite( &Tmp, sizeof( UINT ), 1, Out );

         if ( Cur->Mip_Level<MIP_INF )
         {
            Compress_Write( (BYTE*)Cur->Bits, Out, Mip_Size[Cur->Mip_Level]*sizeof(USHORT), 8 );
         }
         else     // Accumulate
         {
            INT Width, l, m, n, Nb_Blk;
            USHORT *Src, *Dst;
            Width = Mip_Width[ Cur->Mip_Level ];
            Nb_Blk = Width/8;

            Dst = Store + Cur_Store;
//            fprintf( stderr, "Cur_Store=%d\n", Cur_Store );
            fwrite( &Cur_Store, sizeof( INT ), 1, Out );
            n = 0;
            for( l=0; l<Nb_Blk; ++l )
            {
               Src = Cur->Bits + l*8*Width;
               for( m=0; m<Nb_Blk; ++m )
               {
                  Extract_8x8_Block( Dst, Src, Width );
                  Dst += 8; Cur_Store += 8;
                  Src += 8;
                  if ( !(Cur_Store&(MIP_WIDTH_IV-1)) ) { 
                     Dst+=7*MIP_WIDTH_IV;
                     Cur_Store+=7*MIP_WIDTH_IV;
                  }
               }
            }
                  // don't save the last line
                  // it's the same as the previous...
         }
      }
   }
   fclose( Out );

      // save additionnal jpg.

   if ( Store!=NULL )
   {
      char Tmp_Name[256];
      sprintf( Tmp_Name, "%s", Name );
      for( i=(INT)strlen( Name )-1; i>=0; --i )
         if ( Name[i] == '.' ) break;
      sprintf( Tmp_Name, "%s", Name );
      sprintf( Tmp_Name+i+1, "jpg" );
      Out_Message( "Saving additional jpeg %s for cache '%s'. => Total_Size:%d",
         Tmp_Name, Name, Total_Size );
      Out_Message( " Size: %dx%d", MIP_WIDTH_IV, Total_Size/MIP_WIDTH_IV );
      JPEG_Out( Tmp_Name, (BYTE*)Store, MIP_WIDTH_IV, Total_Size/MIP_WIDTH_IV, 0x03 | 0x08 );

      Out_Message( "\n - Done." );
      M_Free( Store );
   }

   return( FALSE );
}

/******************************************************************/
