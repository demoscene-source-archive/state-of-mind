/***********************************************     *
 *                                                   *              
 *          Color fader. Old                         *
 * Skal 96                                           *
 *****************************************************/

#include "main.h"
#include "colfader.h"
#include "mem_map.h"
#include "driver.h"

/********************************************************************/

EXTERN COLOR_FADER *Drv_Setup_Fader(
   COLOR_FADER *CF, void *Drv, INT Start, INT Size, INT Speed )
{
   if ( CF==NULL )
   {
      CF = New_Object( 1, COLOR_FADER );
      if ( CF == NULL ) Exit_Upon_Mem_Error( "Color fader", sizeof( COLOR_FADER ) );
   }
   CF->Drv = Drv;
   CF->Start = (PIXEL)Start;
   CF->Size = (SHORT)Size;
   CF->Speed = (SHORT)Speed;
   if ( Speed<=0 ) Speed = 1;
   CF->Timer = (SHORT)Speed;
   return( CF );
}

EXTERN SHORT Drv_Prepare_Fade_To_Color( COLOR_FADER *CF, PIXEL R, PIXEL G, PIXEL B )
{
   INT i;

   for( i=0; i<CF->Size; ++i )
   {
      CF->Dst_Pal[CF->Start+i][RED_F] = R;
      CF->Dst_Pal[CF->Start+i][GREEN_F] = G;
      CF->Dst_Pal[CF->Start+i][BLUE_F] = B;
      CF->Dst_Pal[CF->Start+i][INDEX_F] = CF->Pal[CF->Start+i][INDEX_F];
   }
   return( CF->Timer );
}

EXTERN INLINE SHORT Drv_Do_Fade_To( COLOR_FADER *CF )
{
   INT i;

   if ( CF->Timer==0 ) return( 0 );
   else if ( CF->Timer==CF->Speed )
      for( i=0; i<CF->Size; ++i )
      {
         INT j;
         SHORT Tmp;

         j = i*6;

         Tmp = CF->Pal[CF->Start+i][RED_F]<<7;
         CF->Col_Fix8[ j ] = Tmp;        /* Cur col */
         Tmp = ( CF->Dst_Pal[CF->Start+i][RED_F]<<7 ) - Tmp;
         CF->Col_Fix8[ j+1 ] = Tmp/CF->Speed; /* Increment */
         j += 2;
         Tmp = CF->Pal[CF->Start+i][GREEN_F]<<7;
         CF->Col_Fix8[ j ] = Tmp;
         Tmp = ( CF->Dst_Pal[CF->Start+i][GREEN_F]<<7 ) - Tmp;
         CF->Col_Fix8[ j+1 ] = Tmp/CF->Speed;
         j += 2;
         Tmp = CF->Pal[CF->Start+i][BLUE_F]<<7;
         CF->Col_Fix8[ j ] = Tmp;
         Tmp = ( CF->Dst_Pal[CF->Start+i][BLUE_F]<<7 ) - Tmp;
         CF->Col_Fix8[ j+1 ] = Tmp/CF->Speed;
      }

   if ( CF->Timer>1 )
   {
      COLOR_ENTRY Final[256];
      for( i=0; i<CF->Size; i++ )
      {
         INT j;
         j = i*6;
         CF->Col_Fix8[ j ] += CF->Col_Fix8[ j+1 ];
         Final[i][RED_F] = (PIXEL)( CF->Col_Fix8[ j ]>>7 );
         j += 2;
         CF->Col_Fix8[ j ] += CF->Col_Fix8[ j+1 ];
         Final[i][GREEN_F] = (PIXEL)( CF->Col_Fix8[ j ]>>7 );
         j += 2;
         CF->Col_Fix8[ j ] += CF->Col_Fix8[ j+1 ];
         Final[i][BLUE_F] = (PIXEL)( CF->Col_Fix8[ j ]>>7 );

         Final[i][INDEX_F] = CF->Pal[ CF->Start+i ][INDEX_F];
      }
      Driver_Change_CMap( (MEM_ZONE *)CF->Drv, CF->Size, Final );
   }
   else
   {
      Driver_Change_CMap( (MEM_ZONE *)CF->Drv, CF->Size, CF->Dst_Pal+CF->Start );
      memcpy( CF->Pal + CF->Start, CF->Dst_Pal + CF->Start,
         CF->Size*sizeof( COLOR_ENTRY ) );
   }

   CF->Timer--;
   return( CF->Timer );
}

/********************************************************************/

