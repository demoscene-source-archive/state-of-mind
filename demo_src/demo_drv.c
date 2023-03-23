/***********************************************
 *              Demo driver.                   *
 *           sequences/timing/synchro          *
 * Skal97                                      *
 ***********************************************/

#include "main.h"
#include "demo_drv.h"
#include "music.h"

#define USING_MP3

#ifdef LNX

// #define PRINT_ITIMER

#endif

#ifdef USING_MP3

#include "mp3.h"
extern MP3_STRM *MP3_Strm;

#endif

/********************************************************************/

static int Last_Phase = 0;
static PHASE *Phases = NULL;
static FLT _T_dx_ = 0.0, _G_dx_ = 0.0;
static long Tick_In, Tick_Out; // , Nb_Ticks, Nb_Ticks2;
static long Starting_Tick = -1;       // sequence bunch timer

#ifndef DISABLE_MIKMOD

/********************************************************************/

EXTERN void Check_Sound( )
{
   if ( !No_Sound ) Update_Sound( );
   else if ( The_Song != NULL ) 
   {
#if 0
      The_Song->sngpos = Global_Timer>>8; 
      The_Song->patpos = (Global_Timer>>2)&0x3F;
#endif
   }
}

/********************************************************************/

EXTERN void Select_Player( char *Song_Name, int Music_Menu, int *Freqs )
{
#ifndef NO_SOUND

#ifndef UNIX
   if ( !No_Sound )
   {

      if ( Init_Player( Song_Name, Music_Menu, Freqs ) == NULL ) 
      {
         Out_Message( "switching sound off" );
         sleep( 1 );
         No_Sound = TRUE;
      }
   }
#else
   No_Sound = TRUE;
#endif

#else
   No_Sound = TRUE;
   The_Song = NULL;
#endif

}

/********************************************************************/

#endif   // !DISABLE_MIKMOD

EXTERN void Precompute_All( INT Last )
{
   INT i;

   if ( !Precompute_Level ) return;

   for( i=0; i<Last; ++i )
   {
      if ( !Phases[i].Precompute_Flag )
      {
         INT j;
         long Final_Tick;
         long Nb_Ticks;

         Tick_In = Phases[i].Timing;
         if ( i+1!=Last ) 
         {
           Tick_Out = Phases[i+1].Timing;
           j = Phases[i].Length;
           if ( j>0 ) Final_Tick = Phases[i+j].Timing;
           else Final_Tick = Tick_Out; 
         }
         else 
         {
            Final_Tick = Tick_Out = Tick_In;
         }   
         Starting_Tick = Tick_In;
         Nb_Ticks = Tick_Out - Tick_In;
         if ( Nb_Ticks ) _T_dx_ = 1.0f / (FLT)( Nb_Ticks );
         else _T_dx_ = 0.0;
         Tick_x = 0.0;
         Global_x = 0.0;
         Tick_dx = 0.0;
         Global_dx = 0.0;
         _G_dx_ = 0.0;

         if ( Phases[i].Precompute != NULL )
            ( *Phases[i].Precompute )( Precompute_Level );
         Phases[i].Precompute_Flag = Precompute_Level;
      }
   }
//   Print_Mem_State( );
}

/********************************************************************/

EXTERN void Init_Timing( INT Last, PHASE *Demo )
{
      // Demo sequences settings

   Last_Phase = Last;
   Phases = Demo;
   Timer = 0;
   Global_Timer = 0;
   Phase = -1;
   if ( Precompute_Level ) Precompute_All( Last_Phase );
}

/********************************************************************/

EXTERN void Start_Demo( INT Start )
{
   INT j;
   long Final_Tick;
   long Nb_Ticks, Nb_Ticks2;

   for( Phase=0; Phase<=Start; ++Phase )
   {
      if ( !Phases[Phase].Precompute_Flag )
         if ( Phases[Phase].Precompute != NULL )
         {
            ( *Phases[Phase].Precompute )( Precompute_Level );
            Phases[Phase].Precompute_Flag = 2;
         }

      if ( Phases != NULL && Phases[ Phase ].Init!=NULL ) 
         ( *(Phases[ Phase ].Init) )( Phases[ Phase ].Param );

         // No_Sound must be set before getting here
   }
   Phase -= 1;

//   if ( Phase>0 ) Out_Message( "Beginning with Phase #%d", Phase+1 );

   Timer = Tick_In  = Phases[Phase].Timing;
   Tick_Out = Phases[Phase+1].Timing;
   j = Phases[Phase].Length;
   if ( j>0 ) Final_Tick = Phases[Phase+j].Timing;
   else Final_Tick = Tick_Out;
   Starting_Tick = Tick_In;

   Nb_Ticks = Tick_Out - Tick_In;   
   if ( Nb_Ticks ) _T_dx_ = 1.0f / (FLT)( Nb_Ticks );
   else _T_dx_ = 0.0;
   Tick_x = 0.0;
   Tick_dx = 0.0;
   Global_dx = 0.0;

   Global_x = 0.0;
   Nb_Ticks2 = Final_Tick - Tick_In;
   if ( Nb_Ticks2 ) _G_dx_ = 1.0f / (FLT)( Nb_Ticks2 );
   else _G_dx_ = 0.0;

#ifndef DISABLE_MIKMOD

#ifndef UNIX
   if ( !No_Sound )
   {
      if ( The_Song != NULL )
         Start_Song( The_Song );
   }
#endif   // !UNIX

   if ( Phase>=1 && Phases[Phase].Pos != 255 )
      Set_Sound_Pos( Phases[Phase].Pos, Phases[Phase].Row );

#endif   // DISABLE_MIKMOD

#ifdef USING_MP3

   if ( MP3_Strm!=NULL && Phase>=1 && Phases[Phase].Pos != -1 )
   {
      INT Skip;
      Skip = Phases[Phase].Pos - MP3_Strm->Cur_Frame;
      MP3_Skip_Frame( MP3_Strm, Skip );   // skip to frame
   }

#endif   // USING_MP3

   My_setitimer( Phases[Phase].Timing );     // Launch timer
}

/********************************************************************/

EXTERN INT Advance_Timer( )
{
   INT j;
   long Final_Tick;
   long Nb_Ticks, Nb_Ticks2;

   Global_Timer++;

   if ( Phases == NULL ) return( -1 );

   Final_Tick = Timer;
   Timer = My_getitimer( );
   Tick_dx = _T_dx_*( Timer - Final_Tick );
   Global_dx = _G_dx_*( Timer - Final_Tick );

   if ( Timer<Tick_Out ) goto Not_Done_With;

      // Next sequence

   if ( Phases[Phase].Close != NULL) 
   {
      ( *( Phases[Phase].Close ) )( );
//      Print_Mem_State( );
   }

   Phase++;
   if ( Phase >= Last_Phase ) { Phase = -1; return( -1 ); }

#ifdef PRINT_ITIMER

#ifdef USING_MP3

   if ( MP3_Strm!=NULL )
      fprintf( stderr, "\nPhase %d  Frame=%d\n", 
         Phase, MP3_Strm->Cur_Frame );
   else fprintf( stderr, "\nPhase %d\n", Phase );

#else

   if ( The_Song!=NULL )
      fprintf( stderr, "\nPhase %d  Sgn,Pat_Pos= %d,%d\n", 
         Phase,
         ((UNIMOD *)The_Song)->sngpos,
         ((UNIMOD *)The_Song)->patpos );
   else fprintf( stderr, "\nPhase %d\n", Phase );

#endif

#endif

   Timer = Tick_In =  Phases[Phase].Timing;
   if ( Timer==-1 ) { Phase = -1; return( -1 ); }  // End
   Tick_Out = Phases[Phase+1].Timing;
   Nb_Ticks = Tick_Out - Tick_In;
   if ( Nb_Ticks ) _T_dx_ = 1.0f / (FLT)( Nb_Ticks );
   else _T_dx_ = 0.0;
   Tick_x = 0.0;

   j = Phases[Phase].Length;
   if ( j>0 ) 
   {
      Final_Tick = Phases[Phase+j].Timing;
      Starting_Tick = Tick_In;
      Global_x = 0.0;
      Nb_Ticks2 = Final_Tick - Starting_Tick;   
      if ( Nb_Ticks2 ) _G_dx_ = 1.0f / (FLT)( Nb_Ticks2 );
      else _G_dx_ = 0.0;
   }

   if ( !Phases[Phase].Precompute_Flag )
      if ( Phases[Phase].Precompute != NULL )
      {
         ( *Phases[Phase].Precompute )( Precompute_Level );
         Phases[Phase].Precompute_Flag = 2;
      }

   if ( Phases[Phase].Init != NULL )
   {
      ( *(Phases[Phase].Init ) )( Phases[ Phase ].Param );
   }

Not_Done_With:

#ifndef DISABLE_MIKMOD
   Check_Sound( );
#endif

   Tick_x = _T_dx_*( Timer-Tick_In );
   if ( Tick_x>1.0 ) Tick_x = 1.0;  // Clamp
   Global_x = _G_dx_*( Timer-Starting_Tick );
   if ( Global_x>1.0 ) Global_x = 1.0; // clamp

#ifdef PRINT_ITIMER
   fprintf( stderr, "Tick: %ld   Tick_x:%lf Global_x:%lf     \r",
      Timer, Tick_x, Global_x );
#endif
   if ( Bypass_Loop!=NULL )
      if ( (*Bypass_Loop)( Timer ) ) goto Skip;
   if ( Phases[Phase].Loop != NULL ) ( *(Phases[Phase].Loop) )( );
Skip:
   if ( Post_Loop!=NULL )
      ( (*Post_Loop)( Timer ) );
   return( Phase );
}

/********************************************************************/

