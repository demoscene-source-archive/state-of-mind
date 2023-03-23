/***********************************************
 *      Music utilities                        *
 *                                             *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "demo_drv.h"
#include "music.h"

EXTERN void *The_Song = NULL;

#ifndef DISABLE_MIKMOD

#ifdef USE_MIKMOD_3

/********************************************************************/
/*                Mikmod v3.x interface                             */
/********************************************************************/

extern MDRIVER *md_driver;

EXTERN void *Init_Player( char *Name, int Menu, int *Freqs )
{
   int Freq, Which_Driver, Max_Drv;

      // Register the loaders we want to use..
            
//   MikMod_RegisterLoader(load_mod);
//   MikMod_RegisterLoader(load_xm);


#ifdef USE_LOAD_S3M
   MikMod_RegisterLoader(load_s3m);
#endif
#ifdef USE_LOAD_IT
   MikMod_RegisterLoader(load_it);
#endif

      // Register the drivers we want to use:

#if defined(__DJGPP__) || defined(__WATCOMC__)

   MikMod_RegisterDriver(drv_ss);
   MikMod_RegisterDriver(drv_awe);
   MikMod_RegisterDriver(drv_sbpro);
   MikMod_RegisterDriver(drv_sb16);
   MikMod_RegisterDriver(drv_sb);
   MikMod_RegisterDriver(drv_gus);
   Max_Drv = 7;

#elif defined( LNX )

   MikMod_RegisterDriver(drv_oss);
   Max_Drv = 2;

#endif

   MikMod_RegisterDriver(drv_nos);

   if ( Menu & ASK_DRIVER )
   {
      int Answer;

      Out_Message( "Drivers:" );
      MD_InfoDriver( );
      Out_Message( "Use which one ?" );
Menu_Try:
      Answer = Get_Me_A_Number( stdin );
      if ( ( Answer<0 ) || ( Answer>Max_Drv ) ) goto Menu_Try;
      Which_Driver = Answer;
   }
   else Which_Driver = 0;  /* autodetect */

   if ( Menu & ASK_MIX_FREQ )       /* Menu freq */
   {
      int Answer, i;

      printf( "Mixing frequency ?\n" );
      for( i=0; Freqs[i] != 0; ++i )
         Out_Message( " %d: %dKhz", i+1, Freqs[i]/1000 );
      Out_Message( " %d: Custom", i+1 );
      Out_Message( " %d: No sound", i+2 );

Try_Again:
      Answer = Get_Me_A_Number( stdin );
      if ( ( Answer<1 ) || ( Answer>i+2 ) ) goto Try_Again;
      if ( Answer == i+2 ) return( NULL );
      else if ( Answer == i+1 )
      {
         do { printf( "\nFreq ? =>" ); fflush( stdout ); }
         while( ( fscanf( stdin, "%d", &Freq ) != 1 ) || ( Freq<5000 ) );
      }
      else { 
         Freq = Freqs[ Answer-1 ]; 
      }
      Out_Message( "Mixing at %d Hz", Freq );
   }
   else if ( Freqs!=NULL ) Freq = Freqs[0];  // first as default
   else  Freq = DEFAULT_FREQ;

   /*
      Initialize soundcard parameters.. you _have_ to do this
      before calling MD_Init(), and it's illegal to change them
      after you've called MD_Init()
   */
                           
   md_mixfreq      = Freq;
   md_dmabufsize   = 32000;                     // standard dma buf size
   md_mode         = DMODE_16BITS|DMODE_STEREO|DMODE_SOFT_MUSIC; // standard mixing mode
   md_device       = Which_Driver;
   md_volume       = 128;
   md_musicvolume  = 128;
   md_reverb       = 0;

      //  initialize soundcard

   MikMod_Init( );

   if ( !( Menu&ASK_DRIVER ) )
      Out_Message( "Driver detected: %s\n", md_driver->Version );

#if 0
   Out_Message( "Using %s for %d bit %s %s sound at %u Hz\n\n",
      md_driver->Name,
      ( md_mode&DMODE_16BITS ) ? 16:8,
      ( md_mode&DMODE_INTERP ) ? "interpolated":"normal",
      ( md_mode&DMODE_STEREO ) ? "stereo":"mono",
      md_mixfreq );
#endif                                             

      /* load the module */
      
   The_Song = (void *)MikMod_LoadSong( Name, 16 );
            
      /* didn't work -> exit with errormsg. */
                  
   if( The_Song == NULL )
   {
      Out_Error( "MikMod Error:" );
      MD_Exit( );
      return( NULL );
   }

      /*
         set the number of voices to use.. you
         could add extra channels here (e.g. md_numchn=The_Song->numchn+4; )
         to use for your own soundeffects:
      */

   md_numchn = ((UNIMOD*)The_Song)->numchn;

   return( The_Song );
}         

/********************************************************************/

static void Tick_Handler( )
{
   MP_HandleTick( );
   // MD_SetBPM( The_Song->bpm );
}

EXTERN void Set_Sound_Pos( INT Pos, INT Row )
{

#ifdef PREDICT_BAR
   The_Pat = (Pos<<6) | Row;
#endif

   if ( No_Sound ) Global_Timer = ( Pos<<8 ) | ( Row<<2 );

   if ( The_Song == NULL ) return;

   ((UNIMOD *)The_Song)->sngpos = Pos;
   ((UNIMOD *)The_Song)->patpos = Row;

}

EXTERN void Start_Song( void *What )
{
   UNIMOD *The_Song;
   The_Song = (UNIMOD *)What;
   The_Song->extspd = TRUE;
   MD_RegisterPlayer( Tick_Handler );
   Player_Start( The_Song );
   MD_SetBPM( The_Song->bpm );
}

EXTERN void Update_Sound( )
{
   MikMod_Update( );
}

/********************************************************************/

EXTERN void Close_Player( void *mf )
{

#ifndef NO_SOUND

   if ( mf==NULL ) mf = The_Song;

   Player_Stop( );         // stop playing
   MikMod_FreeSong( (UNIMOD *)mf );  // and free the module
   MikMod_Exit( );

   if ( mf==The_Song ) The_Song = NULL;

   My_closeitimer( );
   
#endif   // !NO_SOUND

}

/********************************************************************/

#endif   // USE_MIKMOD_3

#ifdef USE_MIKMOD_2

/********************************************************************/
/*                Mikmod v2.10 interface                            */
/********************************************************************/

extern DRIVER *md_driver;

EXTERN void *Init_Player( char *Name, int Menu, int *Freqs )
{
   int Freq, Which_Driver, Max_Drv;

      // Register the loaders we want to use..
            
//   MikMod_RegisterLoader(load_mod);
//   MikMod_RegisterLoader(load_xm);

#ifdef USE_LOAD_S3M
   ML_RegisterLoader(&load_s3m);
#endif
#ifdef USE_LOAD_UNI
   ML_RegisterLoader(&load_uni);
#endif

      // Register the drivers we want to use:

   MD_RegisterDriver(&drv_nos);  // first will be last...

#if defined(__DJGPP__) || defined(__WATCOMC__)

   MD_RegisterDriver(&drv_ss);
   MD_RegisterDriver(&drv_sb);
   MD_RegisterDriver(&drv_gus);
   Max_Drv = 7;

#endif

   if ( Menu & ASK_DRIVER )
   {
      int Answer;

      Out_Message( "Drivers:" );
      MD_InfoDriver( );
      Out_Message( "Use which one ?" );
Menu_Try:
      Answer = Get_Me_A_Number( stdin );
      if ( ( Answer<0 ) || ( Answer>Max_Drv ) ) goto Menu_Try;
      Which_Driver = Answer;
   }
   else Which_Driver = 0;  /* autodetect */


   if ( Menu & ASK_MIX_FREQ )       /* Menu freq */
   {
      int Answer, i;

      printf( "Mixing frequency ?\n" );
      for( i=0; Freqs[i] != 0; ++i )
         Out_Message( " %d: %dKhz", i+1, Freqs[i]/1000 );
      Out_Message( " %d: Custom", i+1 );
      Out_Message( " %d: No sound", i+2 );

Try_Again:
      Answer = Get_Me_A_Number( stdin );
      if ( ( Answer<1 ) || ( Answer>i+2 ) ) goto Try_Again;
      if ( Answer == i+2 ) return( NULL );
      else if ( Answer == i+1 )
      {
         do { printf( "\nFreq ? =>" ); fflush( stdout ); }
         while( ( fscanf( stdin, "%d", &Freq ) != 1 ) || ( Freq<5000 ) );
      }
      else { 
         Freq = Freqs[ Answer-1 ]; 
      }
      Out_Message( "Mixing at %d Hz", Freq );
   }
   else if ( Freqs!=NULL ) Freq = Freqs[0];  // first as default
   else Freq = DEFAULT_FREQ;

   /*
      Initialize soundcard parameters.. you _have_ to do this
      before calling MD_Init(), and it's illegal to change them
      after you've called MD_Init()
   */
                           
   md_mixfreq      = Freq;
   md_dmabufsize   = 32000;                     // standard dma buf size
   md_mode         = DMODE_16BITS|DMODE_STEREO; // standard mixing mode */
   md_device       = Which_Driver;
   mp_volume       = 128;

      //  initialize soundcard

   if (!MD_Init( ) )
   {
      Out_Error( "Mikmod error:%s", myerr );
      return( NULL );
   }

   if ( !( Menu&ASK_DRIVER ) )
      Out_Message( "Driver detected: %s\n", md_driver->Name );

#if 0
   Out_Message( "Using %s for %d bit %s %s sound at %u Hz\n\n",
      md_driver->Name,
      ( md_mode&DMODE_16BITS ) ? 16:8,
      ( md_mode&DMODE_INTERP ) ? "interpolated":"normal",
      ( md_mode&DMODE_STEREO ) ? "stereo":"mono",
      md_mixfreq );
#endif                                             

      /* load the module */
      
   The_Song = (void *)ML_LoadFN( Name );
            
      /* didn't work -> exit with errormsg. */
                  
   if( The_Song == NULL )
   {
      Out_Error( "MikMod Error: %s", myerr );
      MD_Exit( );
      return( NULL );
   }

      /*
         set the number of voices to use.. you
         could add extra channels here (e.g. md_numchn=The_Song->numchn+4; )
         to use for your own soundeffects:
      */

   MP_Init( (UNIMOD *)The_Song );
   md_numchn = ((UNIMOD*)The_Song)->numchn;

   return( The_Song );
}         
/********************************************************************/

static void Tick_Handler( )
{
   MP_HandleTick( );
   // MD_SetBPM( mp_bpm );
}

EXTERN void Set_Sound_Pos( INT Pos, INT Row )
{

#ifdef PREDICT_BAR
   The_Pat = (Pos<<6) | Row;
#endif

   if ( No_Sound ) Global_Timer = ( Pos<<8 ) | ( Row<<2 );

   if ( The_Song == NULL ) return;

   mp_sngpos = Pos;
   mp_patpos = Row;

}

EXTERN void Start_Song( void *What )
{
   UNIMOD *The_Song;
   The_Song = (UNIMOD *)What;
   // mp_extspd = TRUE;
   MD_RegisterPlayer( Tick_Handler );
   MD_PlayStart( );
   MD_SetBPM( mp_bpm );
}

EXTERN void Update_Sound( )
{
   MD_Update( );
}

/********************************************************************/

EXTERN void Close_Player( void *mf )
{

#ifndef NO_SOUND

   if ( mf==NULL ) mf = The_Song;

   MD_PlayStop( );         // stop playing
   ML_Free( (UNIMOD *)mf );  // and free the module
   MD_Exit( );

   if ( mf==The_Song ) The_Song = NULL;

   My_closeitimer( );
   
#endif   // !NO_SOUND

}

/********************************************************************/

#endif   // USE_MIKMOD_2

#endif   // DISABLE_MIKMOD
