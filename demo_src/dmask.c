/***********************************************
 *               Packed Bitmaps                *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "drv_io.h"
#include "dmask.h"
#include "truecol.h"
#include "indy.h"

// #define DBG

/*****************************************************************/

EXTERN USHORT *Split_Mask( BYTE *Src, INT Size, INT MBits )
{
   USHORT *New;
   INT i, Bits, Scale, Mask;

   New = New_Fatal_Object( Size, USHORT );
   Bits = 8-MBits;
   Scale = 1<<MBits;
   Mask = (1<<Bits)-1;
   for( i=0; i<Size; ++i )
      New[i] = (USHORT)( ( ((Src[i]>>Bits)*16/Scale ) << 8 ) | (Src[i]&Mask) );
   return( New );
}

/*****************************************************************/

#if 1 // #ifdef UNIX
static void Uncompress_DMask_Lines( USHORT *Dst0, PIXEL *Src, 
   INT BpS, INT H, USHORT *Cvrt )
{
   for( ; H>0; --H )
   {
      USHORT *Dst = Dst0;
#ifdef DBG
   fprintf( stderr, "NEW Line %d\n", H );
#endif
      while(1)
      {
         DMASK_RLE_TYPE Code;
         Code = (DMASK_RLE_TYPE)(*Src++);
         if ( Code == CRLE_EOL ) break;
         else switch( Code )
         {
            case CRLE_FILL:
            {
               USHORT O;
               INT N;
               N = (*Src++)+1;
               O = Cvrt[ *Src++ ];
#ifdef DBG
   fprintf( stderr, "FILL %d / %d\n", N, O );
#endif
               while( N-- ) *Dst++ = O;
            }
            break;
            case CRLE_SKIP: 
            {
               INT N = (*Src++)+1;
               Dst += N;
#ifdef DBG
   fprintf( stderr, "SKIP %d\n", N );
#endif
            }
            break;
            case CRLE_BLOCK:
            {
               INT N;
               N = (*Src++)+1;
#ifdef DBG
   fprintf( stderr, "BLOCK %d\n", N );
#endif
               while( N-- ) *Dst++ = Cvrt[ *Src++ ];
            }
            break;

            case CRLE_SOLO:
#ifdef DBG
               fprintf( stderr, "SOLO %d\n", Src[1] );
#endif
               *Dst++ = Cvrt[*Src++]; 
            break;
            default:
#ifdef DBG
   fprintf( stderr, "CODE %d\n", Code );
#endif
               *Dst++ = Cvrt[ Code ]; 
            break;
         }
      }
      Dst0 += BpS;
   }
}
#else
extern void Uncompress_DMask_Lines( USHORT *Dst, PIXEL *Src, 
   INT BpS, INT H, USHORT *Cvrt );
#endif


EXTERN void *Uncompress_DMask( DMASK *DMask, USHORT *Dst )
{
   INT i, Bits, Scale, Mask;
   USHORT Convert[256];

   if ( DMask==NULL ) return( NULL );
   if ( Dst==NULL )  // new bits
   {
      if ( DMask->Bits!=NULL ) return( DMask->Bits );
      DMask->Bits = New_Fatal_Object( DMask->Size, USHORT );
   }

   if ( DMask->RLE_Type==0 )  // mask with filter
   {
      Bits = 8-DMask->MBits;
      Scale = 1<<DMask->MBits;
      Mask = (1<<Bits)-1;
      for( i=0; i<256; ++i )
         Convert[i] = (USHORT)( ( ((i>>Bits)*16/Scale) << 8 ) | ( i&Mask ) );
   }
   else CMap_To_16bits( Convert, DMask->Pal, DMask->Nb_Col, 0x2565 );

   if ( Dst==NULL )
   {
      Dst = DMask->Bits;
      for( i=0; i<DMask->Size; ++i )
         Dst[i] = Convert[ DMask->Alpha ];
   }

   Uncompress_DMask_Lines( Dst, DMask->Raw, 
      DMask->Width, DMask->Height, Convert );

   return( (void*)DMask->Bits );
}

/*****************************************************************/

static PIXEL *Emit_Raw_Line_II( PIXEL *Dst, PIXEL *Src )
{
   while(1) {
      DMASK_RLE_TYPE Code;

      Code = (DMASK_RLE_TYPE)(*Src++);
      if ( Code==CRLE_EOL ) return( Src );
      else switch( Code )
      {
         case CRLE_FILL:
         {
            PIXEL C;
            INT N;
            N = (*Src++)+1;
            C = *Src++;
            while( N-- ) *Dst++ = C;
         }
         break;
         case CRLE_SKIP: 
         {
            INT N = (*Src++)+1;
            Dst += N;
         }
         break;
         case CRLE_BLOCK:
         {
            INT N;
            N = (*Src++)+1;
            memcpy( Dst, Src, N );
            Dst += N;
            Src += N;
         }
         break;
         case CRLE_SOLO: *Dst++ = *Src++; break;
         default: *Dst++ = Code; break;
      }
   } 
   return( Src );
}

EXTERN void *Uncompress_DMask_II( DMASK *DMask, PIXEL *Dst )
{
   INT j;
   PIXEL *Src;

   if ( DMask==NULL ) return( NULL );
   if ( Dst==NULL )
   {
      if ( DMask->Bits!=NULL ) return( DMask->Bits );
      DMask->Bits = (USHORT *)New_Fatal_Object( DMask->Size, PIXEL );
      Dst = (PIXEL*)DMask->Bits;
      for( j=0; j<DMask->Size; ++j ) Dst[j] = (PIXEL)DMask->Alpha;      
   }

   Src = DMask->Raw;
   for( j=DMask->Height; j>0; --j )
   {
      Src = Emit_Raw_Line_II( Dst, Src );
      Dst += DMask->Width;
   }
   return( (void*)DMask->Bits );
}

EXTERN void DMask_Setup_Pal( UINT *CMap, DMASK *DMask, PIXEL *Pal )
{
   INT Bits = 8-DMask->MBits;
   INT Scale = 1<<DMask->MBits;
   INT Mask = (1<<Bits)-1;
   INT i;
   for( i=0; i<256; ++i )
   {
      UINT F = 255 - (i>>Bits)*255/Scale;
      UINT RGB = *(UINT*)&Pal[4*( i&Mask )];  // Beware of little/big endians!!
      UINT R = ((RGB>>16)&0xff) * F / 256;
      UINT G = ((RGB>> 8)&0xff) * F / 256;
      UINT B = ((RGB>> 0)&0xff) * F / 256;
      F = F/16;
      CMap[i] = ( R<<24 ) | ( G<<16 ) | ( B<<8 ) | F;
   }
}

/*****************************************************************/

EXTERN void Destroy_DMask( DMASK *Mask )
{
   if ( Mask==NULL ) return;
   M_Free( Mask->Pal );
   M_Free( Mask->Bits );
   M_Free( Mask->Raw );
   M_Free( Mask->H_Index );
   M_Free( Mask );
}

EXTERN DMASK *New_DMask( INT W, INT H, INT Nb_Col )
{
   DMASK *New;
   New = New_Fatal_Object( 1, DMASK );
   if ( New==NULL ) return( NULL );
//   Mem_Clear( New );
   New->Pal = New_Fatal_Object( Nb_Col*3, BYTE );
   if ( New->Pal==NULL ) goto Failed;
   New->Nb_Col = Nb_Col;
   New->H_Index = New_Fatal_Object( H, INT );
   if ( New->H_Index==NULL ) goto Failed;
   New->Width = W;
   New->Height = H;
   return( New );

Failed:
   Destroy_DMask( New );
   return( NULL );
}

EXTERN DMASK *Load_DMask( STRING Name )
{
   DMASK *New;
   BYTE Buf[13];    // warning...
   FILE *In;
   INT Nb_Col, i, W, H, Pak_Size;

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

   F_READ( Buf, In, strlen( DMASK_MAGIC ) );
   if ( strncmp( (char *)Buf, DMASK_MAGIC, strlen( DMASK_MAGIC ) ) )
      Exit_Upon_Error( "'%s' is not a DMASK file", Name );
   F_READ( Buf, In, 13 );
   Nb_Col = Buf[0] | ( Buf[1]<<8 );
   W = Buf[2] | ( Buf[3]<<8 );
   H = Buf[4] | ( Buf[5]<<8 );
   Pak_Size = Buf[6] | ( Buf[7]<<8 ) | ( Buf[8]<<16 ) | ( Buf[9]<<24 );
   New = New_DMask( W, H, Nb_Col );
   if ( New==NULL ) goto Failed;
   New->Pak_Size = Pak_Size;

   New->Alpha = Buf[10] | ( Buf[11]<<8 );
   New->RLE_Type = Buf[12];

   for( i=0; i<Nb_Col*3; ++i )
      F_READ( &New->Pal[i], In, 1 );
   
   if ( Nb_Col<=8 ) New->MBits = 5;
   else if ( Nb_Col<=16 ) New->MBits = 4;
   else if ( Nb_Col<=32 ) New->MBits = 3;
   else if ( Nb_Col<=64 ) New->MBits = 2;
   else if ( Nb_Col<=128 ) New->MBits = 1;
   else New->MBits = 0;

   F_READ( New->H_Index, In, New->Height*sizeof( INT ) );
   for( H=0; H<New->Height; ++H ) New->H_Index[H] = Indy_l( New->H_Index[H] );
   New->Raw = (BYTE*)Load_LZW_Block_II( In );
   if ( New->Raw==NULL ) goto Failed;
   New->Size = New->Width * New->Height;
   New->Off_X = 0; 
   New->Off_Y = 0;

   F_CLOSE( In );
   return( New );

Failed:
   Destroy_DMask( New );
   if ( In!=NULL ) F_CLOSE( In );
   return( NULL );
}


/*****************************************************************/

