/*
 * MP3 streaming basic funcs
 *        Skal98
 *****************************/

#include "main.h"
#include "music.h"

#ifndef DISABLE_MIKMOD

/******************************************************************/
/******************************************************************/

#ifdef USE_MIKMOD_3

EXTERN void Do_Install_Smp( INT Where, INT Off, SHORT *Src, INT Smp_Len )
{
   SHORT *Dst;
   INT i;

   Dst = (SHORT*)Samples[Where];
   Dst += Off;
//   fprintf( stderr, "Transfert: Where=%d  Smp_len=%d\n", Where, Smp_Len );

//   if ( Smp_Len+Off>MP3_Buf_Length ) Smp_Len = MP3_Buf_Length-Off;
   for( i=0; i<Smp_Len; ++i ) Dst[i] = Src[i];
}

#else

EXTERN void Do_Install_Smp( INT Where, INT Off, SHORT *Src, INT Smp_Len )
{
   BYTE *Dst;
   INT i;

   Dst = (BYTE*)Samples[Where];
   Dst += Off;
//   fprintf( stderr, "Transfert: Where=%d  Smp_len=%d\n", Where, Smp_Len );

//   if ( Smp_Len+Off>MP3_Buf_Length ) Smp_Len = MP3_Buf_Length-Off;
   for( i=0; i<Smp_Len; ++i ) Dst[i] = Src[i]>>8;
}

#endif

   //       Summary:
   // 64kbits/s => 8000 bytes/s
   // 100bpm => 100 ticks = 60*8000 = 480 000 bytes
   // 1 tick = 480000/100 = 4800 bytes
   // 4 ticks/sample => 19200 bytes for 1 base sample.
   //

#if 0
EXTERN LINT Init_MP3_Basic( void *The_Song, INT Out, INT Speed )
{
   LINT Change_Rate;
   INT Length;
   FLT Ratio;

   if ( Samples==NULL ) return( -1 );  // mikmod init failed...

           // setting are for 32Khz / 64kbits/s, so...
            // Rate = 64 or 128 (kbits/s)

   Ratio = (1.0*Speed/32000.0);
   Length = (INT)( 1.0 + 19200.0*Ratio );

   fprintf( stderr, "%dHz [%s]\n",
      Speed, Out==MP3_OUT_STEREO ? "Stereo" : "Mono" );
   fprintf( stderr, "Ratio=%f  Length=%d\n", Ratio, Length );

   if ( Samples[MP3_SLOT0] != NULL ) free( Samples[MP3_SLOT0] );
   if ( Samples[MP3_SLOT1] != NULL ) free( Samples[MP3_SLOT1] );
   if ( Out==MP3_OUT_STEREO )
   {
      if ( Samples[MP3_SLOT0_S] != NULL ) free( Samples[MP3_SLOT0_S] );
      if ( Samples[MP3_SLOT1_S] != NULL ) free( Samples[MP3_SLOT1_S] );      
   }

   Change_Rate = (LINT)( 1.0 * 60000.0 / 100.0 );

#ifdef USE_MIKMOD_3

   ((UNIMOD*)The_Song)->loop = TRUE;
//   md_mixfreq = Speed;   

   Samples[MP3_SLOT0] = New_Fatal_Object( Length+16, SHORT );
   Samples[MP3_SLOT1] = New_Fatal_Object( Length+16, SHORT );

   ((UNIMOD*)The_Song)->samples[0].length = Length;
   ((UNIMOD*)The_Song)->samples[0].speed = Speed;
   ((UNIMOD*)The_Song)->samples[1].length = Length;
   ((UNIMOD*)The_Song)->samples[1].speed = Speed;

   if ( Out==MP3_OUT_STEREO )
   {
      Samples[MP3_SLOT0_S] = New_Fatal_Object( Length+16, SHORT );
      Samples[MP3_SLOT1_S] = New_Fatal_Object( Length+16, SHORT );

      ((UNIMOD*)The_Song)->samples[2].length = Length;
      ((UNIMOD*)The_Song)->samples[2].speed = Speed;
      ((UNIMOD*)The_Song)->samples[3].length = Length;
      ((UNIMOD*)The_Song)->samples[3].speed = Speed;
   }

#else

   mp_loop = TRUE;
//   md_mixfreq = Speed;

   Samples[MP3_SLOT0] = New_Fatal_Object( Length+16, SHORT );
   Samples[MP3_SLOT1] = New_Fatal_Object( Length+16, SHORT );
   ((UNIMOD*)The_Song)->instruments[0].samples[0].length = Length;
   ((UNIMOD*)The_Song)->instruments[0].samples[0].c2spd = Speed;
   ((UNIMOD*)The_Song)->instruments[1].samples[0].length = Length;
   ((UNIMOD*)The_Song)->instruments[1].samples[0].c2spd = Speed;
   if ( Out==MP3_OUT_STEREO )
   {
      Samples[MP3_SLOT0_S] = New_Fatal_Object( Length+16, SHORT );
      Samples[MP3_SLOT1_S] = New_Fatal_Object( Length+16, SHORT );
      ((UNIMOD*)The_Song)->instruments[2].samples[0].length = Length;
      ((UNIMOD*)The_Song)->instruments[2].samples[0].c2spd = Speed;
      ((UNIMOD*)The_Song)->instruments[3].samples[0].length = Length;
      ((UNIMOD*)The_Song)->instruments[3].samples[0].c2spd = Speed;
   }

#endif

   MP3_Buf_Length = Length;
   return( Change_Rate );
}
#endif

/******************************************************************/

#endif   // DISABLE_MIKMOD
