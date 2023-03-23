/***********************************************
 *        Packed Bitmaps as MEM_ZONE           *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "mem_map.h"
#include "pak.h"
#include "ld_any.h"

/*****************************************************************/

extern MEM_ZONE_METHODS Default_Pak_Methods;

/*****************************************************************/

EXTERN MEM_PAK *Create_Mem_Pak( INT Height, INT Nb_Col )
{
   MEM_PAK *New;
   INT i;

   New = New_Object( 1, MEM_PAK );
   if ( New == NULL ) return( NULL );
   Mem_Clear( New );
   New->H_Index = New_Object( Height, INT );
   if ( New->H_Index==NULL ) goto Failed;

   MEM_Width( New )   = MEM_BpS( New ) = 0;
   MEM_Height( New )  = Height;
   MEM_Pad( New )     = 0;
   MEM_Quantum( New ) = 1;
   MEM_Format( New )  = FMT_CMAP;
   MEM_Flags( New )  |= MEM_DONT_PROPAGATE;

   New->CMap = New_Object( Nb_Col, COLOR_ENTRY );
   if ( New->CMap == NULL ) goto Failed;
   New->Nb_Col = Nb_Col;
   for( i=0; i<Height; ++i ) New->H_Index[i] = -1;

   New->Methods = &Default_Pak_Methods;
   New->Type = ZONE_TYPE_RAW;

   return( New );

Failed:
   M_Free( New->CMap );
   M_Free( New->H_Index );
   M_Free( New );
   return( NULL );
}

/*****************************************************************/

#ifndef SKL_LIGHT_CODE

/**************************** Pak encoding ************************/

typedef struct 
{
   CODE_TYPE Code;
   PIXEL *Ptr;
   INT Size, Common;
   INT Data;
   INT Alpha;

} PAK_BLOCK;

static INT Emit_Block( PIXEL *Buf, INT Size, PAK_BLOCK *Blk )
{
   if ( Blk->Size<=0 ) return( Size );
   switch( Blk->Code )
   {
      case CODE_FILL:
         if ( Blk->Data == Blk->Alpha )
         {
            Buf[Size++] = CODE_SKIP;
            Buf[Size++] = (PIXEL)Blk->Size-1;
            break;
         }
         else if ( Blk->Size>2 )
         {
            Buf[Size++] = CODE_FILL;
            Buf[Size++] = (PIXEL)Blk->Size-1;
            Buf[Size++] = (PIXEL)Blk->Data;
         }
         else while( Blk->Size-- )
         {
            Buf[Size++] = CODE_SOLO;
            Buf[Size++] = (PIXEL)Blk->Data;
         }
      break;

      case CODE_BLOCK:
         if ( Blk->Size>2 )
         {
            Buf[Size++] = CODE_BLOCK;
            Buf[Size++] = (PIXEL)Blk->Size-1;
            memcpy( Buf+Size, Blk->Ptr, Blk->Size );
            Size += Blk->Size;
         }
         else while( Blk->Size-- )
         {
            if ( Blk->Data>CODE_LAST ) Buf[Size++] = CODE_SOLO;
            Buf[Size++] = (PIXEL)Blk->Data;
         }
      break;

      default: return( Size ); break;
   }
   return( Size );
}

EXTERN MEM_PAK *Btm_To_Pak( BITMAP *Btm, INT Alpha )
{
   MEM_PAK *Pak;
   INT i, Size, x, y;
   PIXEL *Buf, *Ptr;

   Pak = Create_Mem_Pak( Btm->Height, Btm->Nb_Col );
   if ( Pak==NULL ) return( NULL );
   for( i=0; i<Btm->Nb_Col; ++i )
   {
      Pak->CMap[i][RED_F] = Btm->Pal[3*i];
      Pak->CMap[i][GREEN_F] = Btm->Pal[3*i+1];
      Pak->CMap[i][BLUE_F] = Btm->Pal[3*i+2];
      Pak->CMap[i][INDEX_F] =(BYTE)i;
   }

   MEM_Width( Pak ) = MEM_BpS( Pak ) = Btm->Width;

   Buf = New_Fatal_Object( 2*Btm->Width*Btm->Height, PIXEL );
   Ptr = Btm->Bits;
   Size = 0;
   for( y=0; y<Btm->Height; ++y )
   {
      PAK_BLOCK Last;

      Pak->H_Index[y] = Size;

      Last.Code = CODE_BLOCK;
      Last.Size = 1;
      Last.Common = 1;
      Last.Data = Ptr[0];
      Last.Alpha = Alpha;
      Last.Ptr = Ptr;

      for( x=1; x<Btm->Width; ++x )
      {
         INT Data;

         Data = Ptr[x];

         if ( Data==Last.Data )
         {
            Last.Common++;
            goto Finish;
         }

         if ( Last.Common>2 || Last.Data==Last.Alpha )
         {
            Last.Code = CODE_FILL;
            Size = Emit_Block( Buf, Size, &Last );
            Last.Ptr = Ptr+x;
            Last.Size = 0;
         }
         else
         {
            Last.Code = CODE_BLOCK;
            Size = Emit_Block( Buf, Size, &Last );
            Last.Ptr = Ptr+x;
            Last.Size = 0;
         }
         Last.Common = 0;
         Last.Data = Data;

Finish:
         Last.Size++; 
         if ( Last.Size<256 ) { continue; }
         if ( Last.Data!=Last.Alpha ) 
         {
            if ( Last.Common>2 ) Last.Code = CODE_FILL;
            Size = Emit_Block( Buf, Size, &Last );
         }
         Last.Code = CODE_BLOCK;
         Last.Data = Data;
         Last.Size = 0;
         Last.Common = 0;
         Last.Ptr = Ptr+x;
      }

      if ( Last.Data!=Last.Alpha )
      {
         if ( Last.Common>2 ) Last.Code = CODE_FILL;
         Size = Emit_Block( Buf, Size, &Last );
      }
      
      Buf[Size++] = CODE_EOL;
      Ptr += Btm->Width;
   }

   MEM_Base_Ptr(Pak) = My_Realloc( Buf, Size );
   if ( MEM_Base_Ptr(Pak) == NULL ) Exit_Upon_Mem_Error( "PAK buffer", 0 );

   Pak->Pak_Size = Size;
   return( Pak );

}

#endif   // SKL_LIGHT_CODE

/**************************************************************/

static PIXEL *Emit_Raw_Scanline( PIXEL *Dst, PIXEL *Src )
{
   CODE_TYPE Code;

   while(1) {   
      Code = (CODE_TYPE)(*Src++);
      switch( Code )
      {
         case CODE_EOL: return( Src ); break;
         case CODE_FILL:
         {
            PIXEL N, C;
            N = *Src++;
            N++;
            C = *Src++;
            memset( Dst, C, N );
            Dst += N;
         }
         break;
         case CODE_SKIP: Dst += (*Src++)+1; break;
         case CODE_BLOCK:
         {
            INT N;
            N = *Src++;
            N++;
            memcpy( Dst, Src, N );
            Dst += N;
            Src += N;
         }
         break;

         case CODE_SOLO: *Dst++ = *Src++; break;

         default: *Dst++ = Code; break;
      }
   } 

   return( Src );
}

/*******************************************************************/

static INT Propagate_Pak( MEM_ZONE *M, INT Xo, INT Yo, INT Width, INT Height )
{
   PIXEL *Dst, *Src;
   INT Src_X;
   INT X, Y;
   MEM_PAK *Pak;
   
   Pak = (MEM_PAK *)M;

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
      Src_X = X-MEM_Xo(M);
   }


   if ( MEM_Flags(M) & MEM_DONT_PROPAGATE ) return( 0 );
   else return( ZONE_PROPAGATE( M->Dst, X, Y, Width, Height ) );
}

EXTERN INT Paste_Pak( MEM_ZONE *M, INT Xo, INT Yo )
{
   PIXEL *Dst, *Src, *X_Min, *X_Max, *Ptr;
   INT X, Y, Width, Height;
   MEM_PAK *Pak;
   
   Pak = (MEM_PAK *)M;

   if ( M->Dst == NULL ) return( -1 );

   Width = MEM_Width( Pak );
   Height = MEM_Height( Pak );
   X = Xo; Y = Yo;
   if ( Clip_Zones( MEM_IMG(M->Dst), &Width, &Height, &X, &Y ) == ZONE_CLIP_OUT )
      return( -1 );
   Dst = ZONE_SCANLINE( M->Dst, Y );
   Dst += Xo*MEM_Quantum( M->Dst );
   
   Y = Y-Yo;

   while( Height>0 )
   {
      Src = MEM_Base_Ptr(Pak) + Pak->H_Index[Y];
      Ptr = Dst;
      X_Min = Dst + X-Xo;
      X_Max = X_Min + Width;

      while( Ptr<X_Max )
      {
         CODE_TYPE Code;

         Code = (CODE_TYPE)(*Src++);
         switch( Code )
         {
            case CODE_EOL: Ptr=X_Max; break;
            case CODE_FILL:
            {
               INT N;
               PIXEL C;
               N = *Src++; N++;
               C = *Src++;
               if ( Ptr+N<=X_Min ) { Ptr+=N; break; }
               else if ( Ptr<X_Min )
               {
                  INT Skip;
                  Skip = (INT)(X_Min-Ptr);
                  N -= Skip;
                  Ptr=X_Min;
               }
               if ( Ptr+N>=X_Max ) { N = (INT)( X_Max-Ptr ); }
               memset( Ptr, C, N );
               Ptr += N;
            }
            break;

            case CODE_BLOCK:
            {
               INT N;
               N = *Src++; N++;
               if ( Ptr+N<X_Min ) { Src+=N; Ptr+=N; break; }
               else if ( Ptr<X_Min ) 
               {
                  INT Skip;
                  Skip = (INT)(X_Min-Ptr);
                  Src += Skip;
                  N -= Skip;
                  Ptr=X_Min;
               }
               if ( Ptr+N>=X_Max ) { N = (INT)( X_Max-Ptr ); }
               memcpy( Ptr, Src, N );
               Ptr += N; Src += N;
            }
            break;

            case CODE_SOLO: if ( Ptr>=X_Min) *Ptr = *Src; Src++; Ptr++; break;
            case CODE_SKIP: Ptr += (*Src++)+1; break;
            default: if ( Ptr>=X_Min) *Ptr = Code; Ptr++; break;
         }
      }
      Height--;
      Y++;
      Dst += MEM_BpS(M->Dst);
   }
   return( 1 );
}

static INT Flush_Pak( MEM_ZONE *M )
{
   INT y;
   PIXEL *Dst, *Src;
   MEM_PAK *Pak;

   Pak = (MEM_PAK *)M;
   if ( M->Dst == NULL ) return( -1 );

   Dst = ZONE_SCANLINE( M->Dst, MEM_Yo(M) );
   Dst += MEM_Xo(M)*MEM_Quantum( M->Dst );

   Src = MEM_Base_Ptr( Pak );
   for( y=0; y<MEM_Height(M); ++y )
   {
      Src = Emit_Raw_Scanline( Dst, Src );
      Dst += MEM_BpS( M->Dst );
   }
   if ( MEM_Flags(M) & MEM_DONT_PROPAGATE ) return( 0 );
   else return( ZONE_PROPAGATE( M->Dst, MEM_Xo(M), MEM_Yo(M), MEM_Width(M), MEM_Height(M) ) );
}

static void Destroy_Pak( MEM_ZONE *M )
{
   MEM_PAK *Pak;

   Pak = (MEM_PAK *)M;
   M_Free( Pak->CMap );
   M_Free( Pak->H_Index );   
   (*DEFAULT_METHODS->Destroy)( M );
}

static PIXEL *Get_Scanline_Pak( MEM_ZONE *M, INT Y )
{
   MEM_PAK *Pak;

   Pak = (MEM_PAK *)M;
   if ( Y>=MEM_Height(Pak) || Y<0 || Pak->H_Index==NULL  ) return( NULL );
   return( MEM_Base_Ptr( Pak ) + Pak->H_Index[Y] );
}

static ZONE_CLIP_TYPE Set_Position_Pak( MEM_ZONE *M, INT X, INT Y, INT Backup )
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

static MEM_ZONE_METHODS Default_Pak_Methods =
{
   Propagate_Pak, Flush_Pak, Destroy_Pak, Get_Scanline_Pak,
   Set_Position_Pak
};

/*******************************************************************/

