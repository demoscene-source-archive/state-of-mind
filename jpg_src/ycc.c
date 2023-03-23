/************************************************
 *           RGB <=> YCbCr convertion           *
 *                                              *
 * Skal 98                                      *
 ************************************************/

/*******************************************************
 *
 *  MATRICES:
 *
 * R = Y +            + 1.40200.Cr 
 * G = Y - 0.34414.Cb - 0.71414.Cr
 * B = Y + 1.77200.Cb
 *
 * Y  = 0.29900.R + 0.58700.G + 0.11400.B
 * Cb =-0.16874.R - 0.33126.G + 0.50000.B + 1/2
 * Cr = 0.50000.R - 0.41869.G - 0.08131.B + 1/2
 *
 *
 *******************************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "./jpeg.h"
#include "truecol.h"

/*******************************************************/

#define K1  ( 1.40200 )
#define K3  (-0.71414 )
#define K2  (-0.34414 )
#define K4  ( 1.77200 )

#define KK1  (SHORT)( 0.29900*256.0 )
#define KK2  (SHORT)( 0.58700*256.0 )
#define KK3  (SHORT)( 0.11400*256.0 )

/*******************************************************/

      // Final reconstruction of picture

static void MDU_Block_To_RGB( JPG_INFO *Info, MDU_BLOCK *Block )
{
   INT i, j;

   SHORT *Src = Block->Ycc;
   BYTE *Dst = (BYTE*)Block->Where;
   INT dBpS = Info->Global_Width;
   INT sBpS = (Info->Max_X*BLOCK_X-Block->dx)*3;
//   sBpS = 0;

   if ( Info->Global_Nb_Comp==3 )
   {
      dBpS *= Info->Quantum;
      if ( Info->Format==0x4888 )
         for( j=Block->dy; j>0; j--, Dst+=dBpS, Src+=sBpS  )
            for( i=0; i<Block->dx; i++, Src+=3 )
               ((UINT*)Dst)[i] = (INT)RGB_TO_888(Src[0],Src[1],Src[2]);
      else if ( Info->Format==0x4777 )
         for( j=Block->dy; j>0; j--, Dst+=dBpS, Src+=sBpS )
            for( i=0; i<Block->dx; i++, Src+=3 )
               ((UINT*)Dst)[i] = (INT)RGB_TO_777(Src[0],Src[1],Src[2]);
      else if ( Info->Format==0x3888 ) 
         for( j=Block->dy; j>0; j--, Dst+=dBpS, Src+=sBpS )
            for( i=0; i<Block->dx; i++, Src+=3 )
               { Dst[3*i] = (BYTE)Src[2]; Dst[3*i+1] = (BYTE)Src[1]; Dst[3*i+2] = (BYTE)Src[0]; }
      else if ( Info->Format==0x4565 )
         for( j=Block->dy; j>0; j--, Dst+=dBpS, Src+=sBpS )
            for( i=0; i<Block->dx; i++, Src+=3 )
               ((UINT*)Dst)[i] = (INT)RGB_TO_565(Src[0],Src[1],Src[2]);
      else if ( Info->Format==0x4555 )
         for( j=Block->dy; j>0; j--, Dst+=dBpS, Src+=sBpS )
            for( i=0; i<Block->dx; i++, Src+=3 )
               ((UINT*)Dst)[i] = (INT)RGB_TO_555(Src[0],Src[1],Src[2]);
      else if ( Info->Format==0x2565 )
         for( j=Block->dy; j>0; j--, Dst+=dBpS, Src+=sBpS )
            for( i=0; i<Block->dx; i++, Src+=3 )
               ((USHORT*)Dst)[i] = (USHORT)RGB_TO_565(Src[0],Src[1],Src[2]);
      else if ( Info->Format==0x2555 )
         for( j=Block->dy; j>0; j--, Dst+=dBpS, Src+=sBpS )
            for( i=0; i<Block->dx; i++, Src+=3 )
               ((USHORT*)Dst)[i] = (USHORT)RGB_TO_555(Src[0],Src[1],Src[2]);
      else if ( Info->Format==0x1332 )
         for( j=Block->dy; j>0; j--, Dst+=dBpS, Src+=sBpS )
            for( i=0; i<Block->dx; i++, Src+=3 )
               ((BYTE*)Dst)[i] = (BYTE)RGB_TO_332(Src[0],Src[1],Src[2]);
      else
         for( j=Block->dy; j>0; j--, Dst+=dBpS, Src+=sBpS )
            for( i=0; i<Block->dx; i++, Src+=3 )
               ((BYTE*)Dst)[i] = (Src[0]+Src[1]+Src[2])/3;   // gray?!
   }
   else     // gray
   {
      for( j=Block->dy; j>0; j--, Dst+=dBpS, Src+=sBpS )
         for( i=0; i<Block->dx; i++, Src+=3 )
            ((BYTE*)Dst)[i]  = ( Src[0]*KK3 + Src[1]*KK2 + Src[2]*KK1 ) >> 8;
   }
}

/*******************************************************/

#define MAX_ZOOM  4
#define MAX_W     (MAX_ZOOM*BLOCK_X)
#define MAX_H     (MAX_ZOOM*BLOCK_Y)

static void Expand_X( FLT *Src )
{
   INT i;
   for( i=MAX_W*MAX_H/2-1; i>=0; --i )
      Src[2*i] = Src[2*i+1] = Src[i];
}
static void Expand_Y( FLT *Src, INT w )
{
   INT j;
   w *= BLOCK_X;
   for( j=(MAX_H/2-1)*w; j>=0; j-=w )
   {
      memcpy( &Src[2*j], &Src[j], w*sizeof( FLT ) );
      memcpy( &Src[2*j+w], &Src[j], w*sizeof( FLT ) );
   }
}

/*******************************************************/

EXTERN void JPG_Write_Matrix( JPG_INFO *Info, FLT Matrix[BLOCKSIZE], 
   MDU_BLOCK *Block, 
   INT x, INT y, INT nx, INT ny,
   FLT Shift )
{
   INT i, j, Sc_x, Sc_y;
   FLT Tmp[MAX_W*MAX_H];
   FLT *Src;
   SHORT *Dst;

   Sc_x = Info->Max_X/nx;
   Sc_y = Info->Max_Y/ny;

   for( i=0; i<BLOCKSIZE; ++i ) Tmp[i] = Matrix[i] + Shift;

   for( i=1; i<Sc_x; ++i ) Expand_X( Tmp );
   for( i=1; i<Sc_y; ++i ) Expand_Y( Tmp, i-1+Sc_x );

   if ( Block->Ycc==NULL ) { Block->Ycc = Info->Base; Info->Base=NULL; }
   if ( Block->Ycc==NULL )    
      Block->Ycc = New_Fatal_Object( Info->Unit, SHORT );

   Dst = Block->Ycc;
   Dst += x*Sc_x*BLOCK_X*3;
   Dst += (y*Sc_y*BLOCK_Y)*Info->Max_X*BLOCK_X*3;
   Src = Tmp;

   if ( Block->Comp==1 ) // Y
      for( j=0; j<Sc_y*BLOCK_Y; ++j, Dst += 3*Info->Max_X*BLOCK_X )
         for( i=0; i<Sc_x*BLOCK_X*3; i+=3 )
         {
            SHORT Y = (SHORT)( (*Src++)*Info->Scale );
            Dst[i+0] = Y;   // B
            Dst[i+1] = Y;   // G
            Dst[i+2] = Y;   // R
         }
   else if ( Block->Comp==2 )  // Cb
      for( j=0; j<Sc_y*BLOCK_Y; ++j, Dst += 3*Info->Max_X*BLOCK_X )
         for( i=0; i<Sc_x*BLOCK_X*3; i+=3 )
         {
            FLT Cb = ( (*Src++) - 128.0f )*Info->Scale;
            Dst[i+2] += (SHORT)( Cb*K4 );   // B
            Dst[i+1] += (SHORT)( Cb*K2 );   // G
         }
   else // if ( Block->Comp==3 )  // Cr
      for( j=0; j<Sc_y*BLOCK_Y; ++j, Dst += 3*Info->Max_X*BLOCK_X )
         for( i=0; i<Sc_x*BLOCK_X*3; i+=3 )
         {
            FLT Cr = ( (*Src++) - 128.0f )*Info->Scale;
            Dst[i+0] += (SHORT)( Cr*K1 );   // R
            Dst[i+1] += (SHORT)( Cr*K3 );   // G
         }

   if ( (x==nx-1) && (y==ny-1) && (Block->Comp==Info->Global_Nb_Comp) )
   {
           // time to ouput Max_X*Max_Y RGB matrix

      INT r, m;
      r = 14 - Info->Data_Precision;
      m = (0x100<<r)-1;
      for ( i=0; i<Info->Unit; ++i )
      {
         SHORT C = Block->Ycc[i];
         if ( C<0 ) C = 0;
         else if ( C>m ) C = m;
         Block->Ycc[i] = C>>r;
      }
      MDU_Block_To_RGB( Info, Block );
         // recycle malloced block
      if ( Info->Base==NULL ) Info->Base = Block->Ycc;
      else M_Free( Block->Ycc );
      Block->Ycc = NULL;
      // Block->Comp = 0;     // really useful??
   }
}

/*******************************************************/

EXTERN void JPG_Allocate_MDU_Blocks( JPG_INFO *Info )
{
   INT j;
   char *Where;
   MDU_BLOCK *Cur;

   if ( Info==NULL ) return;
   M_Free( Info->Blocks );
   M_Free( Info->Base );
   M_Free( Info->Dst );
   Info->Size = 0;
   if ( Info->Global_Nb_Comp==1 )  // force grayscale output
   {
      Info->Quantum = 1;
      Info->Format = 0x0000;
   }
   else
   {
      Info->Quantum = (Info->Format & 0xF000 )>>12;
      if ( Info->Quantum==0 ) Info->Quantum = 1;	// CMap/gray
   }
   Info->Unit = Info->Max_Y*Info->Max_X*BLOCKSIZE*3;	// MDU unit
   Info->Base = New_Fatal_Object( Info->Unit, SHORT );
   Info->Blocks = New_Fatal_Object( Info->MDUWide*Info->MDUHigh, MDU_BLOCK );
   Info->Size = Info->Global_Width*Info->Global_Height*Info->Quantum;
   Info->Dst = (void*)New_Fatal_Object( Info->Size, BYTE );

   Cur = Info->Blocks;
   Where = (char*)Info->Dst;
   for( j=0; j<Info->Global_Height; )
   {
      INT dy, i;
      char *Ptr;
      Ptr = Where;
      Where += Info->Max_Y*BLOCK_Y*Info->Global_Width*Info->Quantum;
      dy = j; j += Info->Max_Y*BLOCK_Y;
      if ( j>Info->Global_Height ) j=Info->Global_Height;
      dy = j-dy;

      for( i=0; i<Info->Global_Width; )
      {
         INT dx;
         dx = i; i += Info->Max_X*BLOCK_X;
         if ( i>Info->Global_Width ) i=Info->Global_Width;
         dx = i-dx;

         Cur->Ycc = NULL;
         Cur->Where = (void*)Ptr;
         Cur->Comp = 0;
         Cur->dx = (BYTE)dx; Cur->dy = (BYTE)dy; 
         Cur++;
         Ptr += Info->Max_X*BLOCK_X*Info->Quantum;
      }
   }
}

/*******************************************************/
