/***********************************************
 *              CACHE III                      *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "./mip_cst.h"

/******************************************************************/
/******************************************************************/

EXTERN MIP_BLOCK *Search_Free_Mip_Block( 
   MIP_BLOCK *Blk, TXT_CACHE_III *C, USHORT *Maps )
{
   MIP_ROW *Row;
   INT i, Where, Width;
   USHORT *Dst, *Src;

   Row = C->Mip_Rows[ Blk->Mip_Level ];
   Width = Mip_Width[ Blk->Mip_Level ];

   while( Row!=NULL )
   {
      if ( Row->Nb_Entries<Row->Max_Entries )
         for ( Where=0; Where<Row->Max_Entries; ++Where )
            if ( Row->Slots[Where].Installed==FALSE ) goto Ok;
      Row = Row->Next;
   }
      // allocate new row
   Row = New_Fatal_Object( 1, MIP_ROW );
   Row->Next = C->Mip_Rows[Blk->Mip_Level];  // insert first
   C->Mip_Rows[Blk->Mip_Level] = Row;

         //  allocate space for 1 row of texture
   Row->Ptr = New_Fatal_Object( 256*Width, USHORT );
   if ( Row->Ptr==NULL ) return( NULL );
   Row->Nb_Entries = 0;
   Row->Max_Entries = 256/Width;
   Row->Slots = New_Fatal_Object( Row->Max_Entries, MIP_BLOCK );
      // Row->Slots are then zeroed...
   Where = 0;
   C->Nb_Rows++;
   C->Size += (Width+1) * 256*sizeof(USHORT);
//   fprintf( stderr, "Nb_Rows: %d  Size:%d bytes\n", C->Nb_Rows, C->Size );

Ok:

   Blk->Mip_Row = Row;
   Blk->Row_Entry = Where;
   Blk->Installed = TRUE;
   Blk->Bits = Row->Ptr;
   Blk->UV = (Where*Width)&0xff;  // UV mapping coords

   Dst = Row->Ptr + Blk->UV;
   Src = Maps;
   for( i=0; i<Width; ++i )
   {
      memcpy( Dst, Src, Width*sizeof( USHORT ) );
      Dst += 256;
      Src += Width;
   }   

   Row->Slots[Where] = *Blk;
   Row->Nb_Entries++;

   return( &Row->Slots[Where] );
}

EXTERN void Remove_Mip_Block( MIP_BLOCK *Blk )
{
//   Blk->Mip_Row->Slots[Blk->Row_Entry].Installed = FALSE;
   Blk->Installed = FALSE;
   Blk->Mip_Row->Nb_Entries--;
   // if ( Blk->Mip_Row->Nb_Entries==0 )     => Remove Row!!
}

static void Clear_Mip_Row( MIP_ROW *Row )
{
      // TODO: every MIP_BLOCK should be warned if cancellation occur, here...
   M_Free( Row->Slots );
   M_Free( Row->Ptr );   
}

EXTERN void Destroy_Mip_Pages( TXT_CACHE_III *C )
{
   MIP_ROW *Row;
   INT i;
   for( i=0; i<MIP_MAX; ++i )
   {
      Row = C->Mip_Rows[ i ];
      while( Row!=NULL )
      {
         MIP_ROW *Next = Row->Next;
         C->Nb_Rows--;
         C->Size -= Mip_Width[i]*256*sizeof(USHORT);
         Clear_Mip_Row( Row );
         M_Free( Row );
         Row = Next;
      }
      C->Mip_Rows[i] = NULL;
   }
}

/******************************************************************/
/******************************************************************/

EXTERN void Destroy_Txt_Cache_III( void *C )
{
   TXT_CACHE_III *Cache = (TXT_CACHE_III *)C;
   Destroy_Mip_Pages( Cache );
   M_Free( Cache->PMips );
   Cache->Nb_PMips = Cache->Cur_PMips = 0;
}

/******************************************************************/

EXTERN INT Check_Mips_III( void *C )
{
   TXT_CACHE_III *Cache;
   Cache = (TXT_CACHE_III *)C;

   return( FALSE );  // ok
}

/******************************************************************/
/******************************************************************/

static USHORT Poly_Flat_Color( )
{
   INT i;
   MIP_BLOCK *Cur_Mip_Block;
   POLY_MIPS *Cur_Poly_Mips;

   Cur_Poly_Mips = (POLY_MIPS*)Orig_Poly->Ptr;
   if ( Cur_Poly_Mips==NULL ) return( 0x0000 );
   for ( i=0; i<Cur_Poly_Mips->Max_Mip; ++i )
   {
      Cur_Mip_Block = Cur_Poly_Mips->Mips[i];
      if ( Cur_Mip_Block!=NULL ) break;
   }
   if ( Cur_Mip_Block==NULL ) 
      return( 0x0000 ); // black
   UV_Src = (PIXEL*)Cur_Mip_Block->Bits;
   return( ((USHORT*)UV_Src)[Cur_Mip_Block->UV+257] );
}

static INT Retreive_Texture( INT Mip_Slot, FLT R )
{
   FLT Wf;
   MIP_BLOCK *Cur_Mip_Block;
   POLY_MIPS *Cur_Poly_Mips;

   Cur_Poly_Mips = (POLY_MIPS*)Orig_Poly->Ptr;
   if ( Cur_Poly_Mips==NULL ) { Cur_Mip = 2; goto Abort; }

   if ( R<EPS_MIP_BLF*EPS_MIP_BLF ) Cur_Mip = MIP_MAX-1;  /* Bilinear filtering !! */
   else if ( R<EPS_MIP_0*EPS_MIP_0 ) Cur_Mip = MIP_MAX-1;
   else if ( R<EPS_MIP_1*EPS_MIP_1 ) Cur_Mip = 7;
   else if ( R<EPS_MIP_2*EPS_MIP_2 ) Cur_Mip = 6;
   else if ( R<EPS_MIP_3*EPS_MIP_3 ) Cur_Mip = 5;
   else if ( R<EPS_MIP_4*EPS_MIP_4 ) Cur_Mip = 4;
   else if ( R<EPS_MIP_5*EPS_MIP_5 ) Cur_Mip = 3;
   else Cur_Mip = 2;   

   if ( Cur_Mip>Cur_Poly_Mips->Max_Mip )
      Cur_Mip = Cur_Poly_Mips->Max_Mip;

   if ( Cur_Mip<2 )
   {
Abort:
      Flat_Color = Poly_Flat_Color( );
      Mip_Mask = Mip_Masks[Cur_Mip];
      return( Cur_Mip );
   }

Retry:
   Cur_Mip_Block = Cur_Poly_Mips->Mips[Cur_Mip];
   if ( Cur_Mip_Block==NULL ) { Cur_Mip++; goto Retry; }

   UV_Src = (PIXEL*)Cur_Mip_Block->Bits;
   UV_Src += Cur_Mip_Block->UV * sizeof( USHORT );
   Mip_Mask = Mip_Masks[Cur_Mip];

   Wf = 1.0f*Mip_Width[Cur_Mip];
   Mip_Scale = 256.0*256.0*(Wf-2.0f);
   if ( Orig_Poly==Cur_Mip_Block->Down )
      { Uo_Mip = 1.5f*65536.0f; Vo_Mip = 1.5f*65536.0f; }
   else { Uo_Mip = 0.5f*65536.0f; Vo_Mip = 0.5f*65536.0f; }
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
      /* ... */
   return( TRUE );
}

static void Init_From_Poly( )
{
   UV_Src = NULL;
}

static void Init_From_Poly_II( )
{
   UV_Src2 = NULL;
   UV_Src = NULL;
}

static void Init_From_Poly_III( )
{
   UV_Src2 = NULL;
   UV_Src = NULL;
}

static void Setup_Material( void *C, OBJECT *Dum )
{
   MATERIAL *M = (MATERIAL*)Dum;
   M_Free( M->Txt1 );
   M->Txt2 = NULL;
   M->Txt3 = NULL;
}

/******************************************************************/
/******************************************************************/

EXTERN CACHE_METHODS Cache_Methods_III = 
{
   Destroy_Txt_Cache_III,
   Setup_Material,
   Retreive_Texture,
   Compute_MipMap,
   Compute_MipMap_Env,
   Init_From_Poly,
   NULL,
   NULL,
   Poly_Flat_Color
};

/******************************************************************/
/******************************************************************/

