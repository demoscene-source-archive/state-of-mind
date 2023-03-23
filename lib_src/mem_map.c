/***********************************************
 *              mem utilities                  *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "mem_map.h"

/*******************************************************************/

extern MEM_ZONE_METHODS Default_Mem_Zone_Methods;
extern MEM_ZONE_METHODS Virtual_Zone_Methods;
extern void Skl_Match_CMaps( COLOR_ENTRY *Src, COLOR_ENTRY *Dst, INT Nb );

/*******************************************************************/

static INT D_Propagate_Virtual( MEM_ZONE *M, INT Xo, INT Yo, INT Width, INT Height )
{
   if ( M->Dst == NULL ) return( -1 );

   Xo += MEM_Xo(M); Yo += MEM_Yo(M);
   if ( Width<0 ) Width = MEM_Width( M );
   if ( Height<0 ) Height = MEM_Height( M );
   if ( Clip_Zones( MEM_IMG(M->Dst), &Width, &Height, &Xo, &Yo ) == ZONE_CLIP_OUT )
      return( -1 );

   if ( MEM_Flags(M) & MEM_DONT_PROPAGATE ) return( 0 );
   else return( ZONE_PROPAGATE( M->Dst, Xo, Yo, Width, Height ) );
}

static INT D_Flush_Virtual( MEM_ZONE *M )
{
   if ( M->Dst == NULL ) return( -1 );
   if ( MEM_Flags(M) & MEM_DONT_PROPAGATE ) return( 0 );
   else return( ZONE_PROPAGATE( M->Dst, MEM_Xo(M), MEM_Yo(M), MEM_Width(M), MEM_Height(M) ) );
}

EXTERN void MEM_Copy_Rect( MEM_ZONE *M,
   PIXEL *Dst, PIXEL *Src, INT Width, INT Height, INT Pad_Dst )
{
   INT Pad_Src;

   Pad_Src = MEM_BpS( M );

   if ( M->CMapper.Dummy==NULL || M->CMapper.Dummy->Col_Convert == NULL )
   {
      Width *= MEM_Quantum( M );
      while( Height-->0 )
      {
         memcpy( Dst, Src, Width );    /* !!! Beware of alignment */
         Dst += Pad_Dst;
         Src += Pad_Src;
      }
   }
   else
   {
      if ( M->CMapper.Dummy->Type == DITHERER_TYPE )
      {
         if ( M->CMapper.Ditherer->Dst_Stamp != NULL &&
            ( *M->CMapper.Ditherer->Dst_Stamp > M->CMapper.Ditherer->Stamp ) )
         {
               /* Dst_Stamp != NULL means that M->Dst != NULL. We can match colors */
            Skl_Match_CMaps( M->CMapper.Ditherer->Match, M->Dst->CMapper.Matcher->Cols, 256 );
            M->CMapper.Ditherer->Stamp = *M->CMapper.Ditherer->Dst_Stamp;
         }
      }
      while( Height-->0 )
      {
            /* !!! Beware of alignment */
         (*M->CMapper.Dummy->Col_Convert)( &M->CMapper, Dst, Src, Width );
         Dst += Pad_Dst;
         Src += Pad_Src;
      }
   }
}

static INT D_Propagate( MEM_ZONE *M, INT Xo, INT Yo, INT Width, INT Height )
{
   PIXEL *Dst, *Src;
   INT X, Y;

   if ( M->Dst == NULL ) return( -1 );

   if ( Width<0 ) Width = MEM_Width( M );
   if ( Height<0 )  Height = MEM_Height( M );
   {

      X = Xo+MEM_Xo(M); Y = Yo+MEM_Yo(M);
      if ( Clip_Zones( MEM_IMG(M->Dst), &Width, &Height, &X, &Y ) == ZONE_CLIP_OUT )
         return( -1 );

      Dst = ZONE_SCANLINE( M->Dst, Y );
      Dst += X*MEM_Quantum( M->Dst );

      Src = ZONE_SCANLINE( M, Y-MEM_Yo(M) );
      Src += (X-MEM_Xo(M))*MEM_Quantum( M );
   }

   MEM_Copy_Rect( M, Dst, Src, Width, Height, MEM_BpS(M->Dst) );

   if ( MEM_Flags(M) & MEM_DONT_PROPAGATE ) return( 0 );
   else return( ZONE_PROPAGATE( M->Dst, X, Y, Width, Height ) );
}

static INT D_Flush( MEM_ZONE *M )
{
   PIXEL *Dst, *Src;

   if ( M->Dst == NULL ) return( -1 );

   Dst = ZONE_SCANLINE( M->Dst, MEM_Yo(M) );
   Dst += MEM_Xo(M)*MEM_Quantum( M->Dst );

   Src = ZONE_SCANLINE( M, 0 );

   MEM_Copy_Rect( M, Dst, Src, MEM_Width(M), MEM_Height(M), MEM_BpS(M->Dst) );

   if ( MEM_Flags(M) & MEM_DONT_PROPAGATE ) return( 0 );
   else return( ZONE_PROPAGATE( M->Dst, MEM_Xo(M), MEM_Yo(M), MEM_Width(M), MEM_Height(M) ) );
}

static void D_Destroy( MEM_ZONE *M )
{
   if ( MEM_Flags( M ) & IMG_OWNS_PTR )
   {  M_Free( MEM_Base_Ptr( M ) ) }
   else MEM_Base_Ptr( M ) = NULL;

   MEM_Flags( M ) &= ~IMG_OWNS_PTR;

   if ( M->Flags & MEM_OWNS_METHODS )
   {
      M_Free( M->Methods );
      M->Methods = DEFAULT_METHODS;
      M->Flags &= ~MEM_OWNS_METHODS;
   }
   MEM_Turn_Backup_Off( M );

   Clear_CMapper( M );

   memset( MEM_IMG( M ), 0, sizeof( MEM_IMG ) );
   MEM_Format( M ) = FMT_NONE;
}

static PIXEL *D_Get_Scanline( MEM_ZONE *M, INT Y )
{
   PIXEL *Ptr;

   if ( Y>=MEM_Height(M) || Y<0 ) return( NULL );

   if ( MEM_Flags(M)&IMG_VIRTUAL )   /* Virtual zone */
   {
      /* if ( M->Dst == NULL ) return( NULL ); shouldn't happen */
      Ptr = ZONE_SCANLINE( M->Dst, MEM_Yo(M)+Y );
      Ptr += MEM_Xo(M)*MEM_Quantum(M);
      return( Ptr );
   }
   else 
   {
      Ptr = MEM_Base_Ptr(M);
      Ptr += Y*MEM_BpS(M);
      return( Ptr );
   }
}

static ZONE_CLIP_TYPE D_Set_Position( MEM_ZONE *M, INT X, INT Y, INT Backup )
{
   INT Width, Height;

   if ( MEM_Backup(M) != NULL ) MEM_Restore_Backup( M );

   MEM_Xo(M) = X; MEM_Yo(M) = Y;
   if ( M->Dst == NULL ) return( ZONE_NO_CLIP );
   if ( !Backup ) goto No_Backup;

   if ( MEM_Backup(M) == NULL ) MEM_Turn_Backup_On( M );
   if ( MEM_Backup(M) != NULL ) MEM_Store_Backup( M );

No_Backup:
   Width = MEM_Width( M );
   Height = MEM_Height( M );
   return( Clip_Zones( MEM_IMG(M->Dst), &Width, &Height, &X, &Y ) );
}

/*******************************************************************/

MEM_ZONE_METHODS Default_Mem_Zone_Methods =
{
   D_Propagate, D_Flush, D_Destroy, D_Get_Scanline,
   D_Set_Position,
};

static MEM_ZONE_METHODS Virtual_Zone_Methods =
{
   D_Propagate_Virtual, D_Flush_Virtual, D_Destroy, D_Get_Scanline,
   D_Set_Position
};

/*******************************************************************/

EXTERN BACKUP_HEADER *MEM_Turn_Backup_On( MEM_ZONE *M )
{
   INT Size;

   if ( M==NULL || M->Dst == NULL) return( NULL );
   M_Free( MEM_Backup(M) );
   Size = MEM_Width( M )* MEM_Height( M );
   Size *= MEM_Quantum( M->Dst );
   MEM_Backup(M) = (BACKUP_HEADER *)New_Object( Size+sizeof( BACKUP_HEADER ), PIXEL );
   if ( MEM_Backup(M) == NULL ) return( NULL );
   Mem_Clear( MEM_Backup(M) );
   /* MEM_Store_Backup(M); */
   return( MEM_Backup(M) );
}

EXTERN void MEM_Turn_Backup_Off( MEM_ZONE *M )
{
   if ( M==NULL || MEM_Backup(M) == NULL) return;
   MEM_Restore_Backup( M );
   M_Free( MEM_Backup(M) );
}

EXTERN void MEM_Store_Backup( MEM_ZONE *M )
{
   PIXEL *Dst, *Src;
   INT Pad;
   INT X, Y;
   INT Width, Height;
   ZONE_CLIP_TYPE Clip;

   if ( M->Dst == NULL || MEM_Base_Ptr(M->Dst)==NULL ) return;

   X = MEM_Xo(M); Y = MEM_Yo(M);

   Width = MEM_Width( M );
   Height = MEM_Height( M );
   Clip = Clip_Zones( MEM_IMG(M->Dst), &Width, &Height, &X, &Y );
   MEM_Backup( M )->X = X; MEM_Backup( M )->Y = Y;
   if ( Clip == ZONE_CLIP_OUT )
   {
      MEM_Backup( M )->Width = 0;
      return;
   };
   Width *= MEM_Quantum(M->Dst);      
   MEM_Backup( M )->Width = Width;
   MEM_Backup( M )->Height = Height;

   Src = ZONE_SCANLINE( M->Dst, Y );
   Src += X*MEM_Quantum( M->Dst );

   Pad = MEM_BpS( M->Dst );

   Dst = ( (PIXEL *)MEM_Backup( M ) ) + sizeof( BACKUP_HEADER );

   while( Height-->0 )
   {
      memcpy( Dst, Src, Width );      /* !!! Beware of alignment */
      Src += Pad;
      Dst += Width;
   }
}

EXTERN void MEM_Restore_Backup( MEM_ZONE *M )
{
   PIXEL *Dst, *Src;
   INT Pad;
   INT X, Y;
   INT Width, Height, y;

   if ( M->Dst == NULL || MEM_Base_Ptr(M->Dst)==NULL ) return;
   Width = MEM_Backup(M)->Width;
   if ( Width==0 ) return;
   Height = MEM_Backup(M)->Height;

   Src = ( (PIXEL *)MEM_Backup( M ) ) + sizeof( BACKUP_HEADER );

   X = MEM_Backup(M)->X;
   Y = MEM_Backup(M)->Y;
   Dst = ZONE_SCANLINE( M->Dst, Y );
   Dst += X*MEM_Quantum( M->Dst );
   Pad = MEM_BpS( M->Dst );

   for( y=Height; y; --y )
   {
      memcpy( Dst, Src, Width );      /* !!! Beware of alignment */
      Src += Width;
      Dst += Pad;
   }

   ZONE_PROPAGATE( M->Dst, X, Y, Width, Height );
}

EXTERN void Clean_Up_Zone( MEM_ZONE *M )
{
      /* !! Don't destroy ->Methods !! */

   Clear_CMapper( M );

   memset( MEM_IMG( M ), 0, sizeof( MEM_IMG ) );
   MEM_Format( M ) = FMT_NONE;

}

/*******************************************************************/

EXTERN MEM_ZONE *_New_MEM_Zone( INT Size, MEM_ZONE_METHODS *Methods )
{
   MEM_ZONE *New;

   New = (MEM_ZONE *)My_Malloc( Size );
   if ( New == NULL ) return( NULL );
   memset( New, 0, Size );

   New->Flags = MEM_NO_FLAG;
   if ( Methods == NULL ) New->Methods = DEFAULT_METHODS;
   else
   {
      if ( Assign_Methods( New )==NULL )
      {
         M_Free( New );
         return( NULL );
      }
      else *New->Methods = *Methods;
   }

   New->Type = (ZONE_TYPE)( ZONE_TYPE_RAW | ZONE_TYPE_STAMP );   /* Default + Stamp */
   New->Backup = NULL;

   return( New );
}

EXTERN MEM_ZONE_METHODS *Assign_Methods( MEM_ZONE *M )
{
   MEM_ZONE_METHODS *Methods;

   if ( M->Flags & MEM_OWNS_METHODS ) return( M->Methods );
   Methods = New_Object( 1, MEM_ZONE_METHODS );
   if ( Methods == NULL ) return( NULL );
   *Methods = *DEFAULT_METHODS;
   M->Methods = Methods;
   M->Flags |= MEM_OWNS_METHODS;

   return( Methods );
}
 
EXTERN MEM_IMG *Mem_Img_Set( MEM_IMG *Img, INT F, 
   INT W, INT H, INT P, USHORT Q, FORMAT Fmt, PIXEL *Ptr )
{
   IMG_Width(Img) = W;
   IMG_Height(Img) = H;
   IMG_Pad(Img) = P;
   if ( Q==0 ) Q = Format_Depth( Fmt );
   IMG_Quantum(Img) = Q;
   IMG_Format(Img) = Fmt;
   IMG_BpS(Img) = (P+W)*Q;
   IMG_Size(Img) = IMG_BpS(Img)*H;

   if ( F&IMG_VIRTUAL ) Ptr = NULL;
   else if ( Ptr == NULL )
   {
      Ptr = New_Object( W*H*Q, PIXEL );
      if ( Ptr == NULL ) return( NULL );
      F |= IMG_OWNS_PTR;
   }
   IMG_Flags(Img) = F;
   IMG_Base_Ptr(Img) = Ptr;
   return( Img );
}

EXTERN void Format_To_Zone_Type( MEM_ZONE *M )
{
   M->Type = (ZONE_TYPE)( M->Type & (~0x0F) );
   if ( (MEM_Format(M)&0x1FFF)==FMT_CMAP ) 
      M->Type = (ZONE_TYPE)( M->Type | ZONE_TYPE_INDEXED );
   else M->Type = (ZONE_TYPE)( M->Type | ZONE_TYPE_RGB );
}

/*******************************************************************/
/*******************************************************************/

EXTERN ZONE_CLIP_TYPE  Clip_Zones( MEM_IMG *Dst,
   INT *Width, INT *Height, INT *xo, INT *yo )
{
   ZONE_CLIP_TYPE Clip;

   if ( Dst==NULL ) return ZONE_NO_CLIP;
   if ( (*xo)>=Dst->Width || (*yo)>=Dst->Height ) return ZONE_CLIP_OUT;
   if ( (*xo)+(*Width)<0 || (*yo)+(*Height)<0 ) return ZONE_CLIP_OUT;

   Clip = ZONE_NO_CLIP;

   if ( (*yo)<0 )
   {
      *Height += (*yo);
      *yo = 0;
      Clip = (ZONE_CLIP_TYPE)( Clip | ZONE_CLIP_Y );
   }
   if ( (*xo)<0 )
   {
      *Width += (*xo);
      *xo = 0;
      Clip = (ZONE_CLIP_TYPE)( Clip | ZONE_CLIP_X );
   }
   if ( (*yo)+(*Height)>Dst->Height )
   {
      *Height = Dst->Height-(*yo);
      Clip = (ZONE_CLIP_TYPE)( Clip | ZONE_CLIP_Y );
   }
   if ( (*xo)+(*Width)>Dst->Width )
   {
      *Width = Dst->Width-(*xo);
      Clip = (ZONE_CLIP_TYPE)( Clip | ZONE_CLIP_X );
   }

   return( Clip );
}

/*******************************************************************/

EXTERN MEM_ZONE *Extract_Converted_Zone(
   MEM_ZONE *New,
   MEM_ZONE *Target, INT Width, INT Height, INT X, INT Y, FORMAT Format,
   MEM_ZONE_METHODS *Methods )
{

   if ( New==NULL ) New = New_MEM_Zone( MEM_ZONE, Methods );
   if ( New==NULL ) return( NULL );

   if ( Format == 0xFFFFFFFF ) Format = MEM_Format(Target);
   if ( Format_F_Pos(Format)==0 ) Format = (FORMAT)( Format | Packed_Field_Pos( Format ) );
   if ( Format_Depth(Format)==0 ) Format = Compute_Format_Depth( Format );

   if ( Mem_Img_Set( MEM_IMG(New), IMG_NO_FLAG,
      Width, Height, 0, 0, Format, NULL ) == NULL )
      goto Failed;
   Format_To_Zone_Type( New );

   MEM_Xo(New) = X;
   MEM_Yo(New) = Y;
   New->Dst = Target;

   if ( Format != MEM_Format(Target) || (MEM_Format(Target)&0x1FFF)!=FMT_CMAP )
   {
      if ( Drv_Install_Converter( New ) == NULL ) goto Failed;
   }
   else if ( Format != MEM_Format(Target) )  /* TODO: ?!?! */
   {
      New->CMapper.Dummy = Target->CMapper.Dummy;
      New->Type = (ZONE_TYPE)( New->Type & (~MEM_OWNS_CMAPPER) );
   }
   return( New );

Failed:
   ZONE_DESTROY( New );
   return( NULL );
}

EXTERN MEM_ZONE *Extract_Virtual_Zone( 
   MEM_ZONE *M,
   MEM_ZONE *Target,
   INT Width, INT Height, INT X, INT Y )
{
      // Create virtual entry point in Dst

   if ( Clip_Zones( MEM_IMG(Target), &Width, &Height, &X, &Y ) != ZONE_NO_CLIP )
      return( NULL );   // Can't virtualize...

   if ( M==NULL ) M = New_MEM_Zone( MEM_ZONE, NULL );
   if ( M==NULL ) return( NULL );

   M->Methods = &Virtual_Zone_Methods;

   ZONE_SET_POSITION( M, X, Y, FALSE );    // No backup here

   M->Dst = Target;

   Mem_Img_Set( MEM_IMG(M), IMG_VIRTUAL,
      Width, Height, 
      MEM_BpS(Target) - Format_Depth( MEM_Format(Target) )*Width,
      0, MEM_Format(Target), NULL );

   return( M );
}

/*******************************************************************/
/*******************************************************************/

