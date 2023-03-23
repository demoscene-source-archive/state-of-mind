/***********************************************
 *        RLE-encoding for DMASK               *
 * Skal 98                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "dmask.h"

typedef struct 
{
   DMASK_RLE_TYPE Code;
   PIXEL *Ptr;
   INT Size;
   INT Alpha;

} RLE_BLOCK;

static RLE_BLOCK Blk;
static PIXEL *Out_Buf = NULL;
static INT (*Emit_Block)( INT );

// #define DBG

/*****************************************************************/

static INT Emit_Block_I( INT Size )
{
   switch( Blk.Code )
   {
      case CRLE_FILL:
         if ( Blk.Ptr[0] == Blk.Alpha )
         {
            Out_Buf[Size++] = CRLE_SKIP;
            Out_Buf[Size++] = (PIXEL)Blk.Size-1;
#ifdef DBG
            fprintf( stderr, "SKIP %d\n", Blk.Size );
#endif
         }
         else if ( Blk.Size>2 )
         {
            Out_Buf[Size++] = CRLE_FILL;
            Out_Buf[Size++] = (PIXEL)Blk.Size-1;
            Out_Buf[Size++] = (PIXEL)Blk.Ptr[0];
#ifdef DBG
            fprintf( stderr, "FILL %d with %d\n", Blk.Size, Blk.Ptr[0] );
#endif
         }
         else while( Blk.Size-- )
         {
            if ( Blk.Ptr[0]>CRLE_LAST )
               Out_Buf[Size++] = CRLE_SOLO;
            Out_Buf[Size++] = (PIXEL)Blk.Ptr[0];
#ifdef DBG
          fprintf( stderr, "SOLO %d\n", Blk.Ptr[0] );
#endif
            Blk.Ptr++;
         }
      break;

      case CRLE_BLOCK:
         if ( Blk.Size>2 )
         {
            INT i;
            for( i=Blk.Size-1; i>=0; --i ) if ( Blk.Ptr[i]>CRLE_LAST ) break;
            if ( i>=0 )
            {
               Out_Buf[Size++] = CRLE_BLOCK;
               Out_Buf[Size++] = (PIXEL)Blk.Size-1;
#ifdef DBG
               fprintf( stderr, "BLOCK %d\n", Blk.Size );
#endif
            }
            memcpy( Out_Buf+Size, Blk.Ptr, Blk.Size );
            Size += Blk.Size;
         }
         else while( Blk.Size-- )
         {
            if ( Blk.Ptr[0]>CRLE_LAST ) Out_Buf[Size++] = CRLE_SOLO;
            Out_Buf[Size++] = (PIXEL)Blk.Ptr[0];
#ifdef DBG
            fprintf( stderr, "SOLO %d\n", Blk.Ptr[0] );
#endif
            Blk.Ptr++;
         }
      break;
   }
   return( Size );
}

static INT Emit_Block_II( INT Size )   // BLOCK/FILL only
{
   switch( Blk.Code )
   {
      case CRLE_FILL:
         Out_Buf[Size++] = CRLE_FILL;
         Out_Buf[Size++] = (PIXEL)Blk.Size-1;
         Out_Buf[Size++] = (PIXEL)Blk.Ptr[0];
      break;

      case CRLE_BLOCK:
      {
         INT i;
         for( i=Blk.Size-1; i>=0; --i ) if ( Blk.Ptr[i]>CRLE_LAST ) break;
         if ( i>=0 )
         {
            Out_Buf[Size++] = CRLE_BLOCK;
            Out_Buf[Size++] = (PIXEL)Blk.Size-1;
         }
         memcpy( Out_Buf+Size, Blk.Ptr, Blk.Size );
         Size += Blk.Size;
      }
      break;
   }
   return( Size );
}

static INT Emit_Block_III( INT Size )  // BLOCK/FILL/SKIP only
{
   switch( Blk.Code )
   {
      case CRLE_FILL:
         if ( Blk.Ptr[0] == Blk.Alpha )
         {
            Out_Buf[Size++] = CRLE_SKIP;
            Out_Buf[Size++] = (PIXEL)Blk.Size-1;
         }
         else
         {
            Out_Buf[Size++] = CRLE_FILL;
            Out_Buf[Size++] = (PIXEL)Blk.Size-1;
            Out_Buf[Size++] = (PIXEL)Blk.Ptr[0];
         }
      break;

      case CRLE_BLOCK:
      {
         INT i;
         for( i=Blk.Size-1; i>=0; --i ) if ( Blk.Ptr[i]>CRLE_LAST ) break;
         if ( i>=0 )
         {
            Out_Buf[Size++] = CRLE_BLOCK;
            Out_Buf[Size++] = (PIXEL)Blk.Size-1;
         }
         memcpy( Out_Buf+Size, Blk.Ptr, Blk.Size );
         Size += Blk.Size;
      }
      break;
   }
   return( Size );
}


static INT (*Emit_Block_F[3])( INT ) = 
{
   Emit_Block_I, Emit_Block_II,  Emit_Block_III 
};

static INT Do_Emit_Block( INT Size )
{
   if ( Blk.Size<=0 ) return( Size );
   if ( Blk.Size>256 )
   {
      INT Tmp;
      Tmp = Blk.Size;
      Blk.Size = 256;
      Size = (*Emit_Block)( Size );
      Blk.Size = Tmp-256;
   }
   return( (*Emit_Block)( Size ) );
}

EXTERN DMASK *Compress_DMask( DMASK *DMask, PIXEL *In_Buf, 
   INT Alpha, INT Type )
{
   INT Offset, x, y;
   PIXEL *Ptr0;

   M_Free( DMask->Raw );
   DMask->Pak_Size = 0;
   DMask->Alpha = (USHORT)Alpha;
   Blk.Alpha = Alpha;
   DMask->RLE_Type = (USHORT)Type;
   Emit_Block = Emit_Block_F[Type];

   Out_Buf = New_Fatal_Object( 2*DMask->Width*DMask->Height, PIXEL );
   Ptr0 = In_Buf;
   Offset = 0;

   for( y=DMask->Height; y>0; --y, Ptr0 += DMask->Width )
   {
      PIXEL *Ptr = Ptr0, *BPtr;
      INT Code, Size;
#ifdef DBG
      fprintf( stderr, "*** LINE %d ***\n", y );
#endif
      DMask->H_Index[y] = Offset;      
      x = DMask->Width;
      Blk.Code = CRLE_LAST;
      Size = 0;
      while( x>0 )
      {         
         Size = 0;
         BPtr = Ptr;
         while ( Ptr[++Size]==Ptr[0] ) if (!(x--)) break;
         if ( Size>3 ) Code = CRLE_FILL;
         else 
         {
            while( Ptr[Size]!=Ptr[Size-1] ) { if (!(--x)) break; Size++; }
            Code = CRLE_BLOCK;            
         }
         Size--;
         if ( Blk.Code==CRLE_BLOCK && Code==CRLE_BLOCK )
         {
            Blk.Size += Size;
         }
         else 
         {
            if ( x==0 && Code==CRLE_FILL && BPtr[0]==Blk.Alpha )
               Size = 0;
            if ( Blk.Code==CRLE_BLOCK && Code!=CRLE_BLOCK )
               Offset = Do_Emit_Block( Offset ); // purge previous block
            Blk.Ptr = BPtr;
            Blk.Size = Size;
            Blk.Code = (DMASK_RLE_TYPE)Code;
            if ( Code!=CRLE_BLOCK )
            {
               Offset = Do_Emit_Block( Offset );
               Blk.Code = CRLE_LAST;
            }
         }
         Ptr += Size;
      }
      if ( Code==CRLE_BLOCK )   // purge last
         Offset = Do_Emit_Block( Offset );
      Out_Buf[Offset++] = CRLE_EOL;
   }

   Out_Buf = (PIXEL*)My_Realloc( Out_Buf, Offset );
   if ( Offset!=0 && Out_Buf==NULL )
      Exit_Upon_Mem_Error( "DMASK buf", 0 );

   DMask->Pak_Size = Offset;
   
   DMask->Raw = Out_Buf;
   Out_Buf = NULL;
   return( DMask );
}

EXTERN DMASK *Save_DMask( DMASK *DMask, STRING Name )
{
   FILE *Out;
   BYTE Buf[13];

   if ( DMask==NULL || DMask->Height==0 || DMask->Width==0 ||
      DMask->Raw==NULL || DMask->Pak_Size==0 ) return( NULL );

   Out = fopen( Name, WRITE_SWITCH );
   if ( Out==NULL ) return( NULL );
   fwrite( DMASK_MAGIC, 1, strlen(DMASK_MAGIC), Out );
   Buf[0] = DMask->Nb_Col &0xFF;
   Buf[1] = ( DMask->Nb_Col>>8 ) &0xFF;
   Buf[2] = DMask->Width &0xFF;
   Buf[3] = ( DMask->Width>>8 ) &0xFF;
   Buf[4] = DMask->Height &0xFF;
   Buf[5] = ( DMask->Height>>8 ) &0xFF;
   Buf[6] = DMask->Pak_Size &0xFF;
   Buf[7] = ( DMask->Pak_Size>>8 ) &0xFF;
   Buf[8] = ( DMask->Pak_Size>>16 ) &0xFF;
   Buf[9] = ( DMask->Pak_Size>>24 ) &0xFF;
   Buf[10] = DMask->Alpha &0xFF;
   Buf[11] = ( DMask->Alpha>>8 ) &0xFF;
   Buf[12] = DMask->RLE_Type &0xFF;
   fwrite( Buf, 13, 1, Out );
   if ( DMask->Nb_Col ) fwrite( DMask->Pal, 3, DMask->Nb_Col, Out );
   fwrite( DMask->H_Index, sizeof(INT), DMask->Height, Out );
   Save_LZW_Block_II( DMask->Raw, Out, DMask->Pak_Size, 8 );
   fclose( Out );

   return( DMask );
}

/*****************************************************************/
