/*
 * - SoM -
 *
 * " Constants aren't; variables won't. "
 *           (Osborn's Law)
 *
 * -=[ Skal 98 (skal.planet-d.net) ]=-
 *************************************************/

/*************************************************/

#include "demo.h"
#include "audio.h"
#include "mp3.h"
#include "fonts.h"

#if !defined( WIN32 )

#include "lib_gfx.h"
EXTERN MEM_ZONE *The_Screen=NULL;
EXTERN MEM_ZONE *Real_Screen=NULL;
#include "mem_map.h"
#define OUTS(x) Out_String(x)

  // in ijpeg.h
extern BYTE *JPEG_Out( STRING Name, BYTE *Bits, 
   INT Width, INT Height, INT Type );
   
#else

#include "../src/gfx_ddrw.h"
EXTERN void *The_Screen;
typedef void *MEM_ZONE;
#define OUTS(x)  { }

#endif

#define DAT_FILE_NAME   "mind.dat" 
#define MP3_FILE_NAME   "mind.mp3"

int DoubleSize;
INT _MP3_Out_;
STRING MP3_File;

// #define BUILD_DAT_FILE

#ifdef BUILD_DAT_FILE
static INT Write_Dat = FALSE;
#endif   // BUILD_DAT_FILE

// #define LOWMEM
// #define ECHO_TIMER

// WINDOWS stuff
#ifdef WIN32
extern int Launch_Dialog();
#endif


/*************************************************/
 
   // Sequences control

extern INT LAST_PHASE, NB_PHASE;
extern PHASE Phases[];

   // Misc

static void Destroy_All( );
static void Banner( );
static void Help( );
EXTERN MP3_STRM *MP3_Strm = NULL;    // <= used in demo_drv.c
// EXTERN MEM_ZONE *The_Screen;
EXTERN INT Nb_Frames, Low_Mem;

// extern void Handle_Spectrum( INT Nb_Chn, FLT Spc[2][32][18] );

/********************************************************************/
/********************************************************************/

static void Destroy_All( )
{
   if ( MP3_Strm!=NULL ) MP3_Destroy( MP3_Strm );
   if ( !No_Sound ) Close_Audio( );
   if ( The_Screen!=NULL ) Driver_Close( The_Screen );
   The_Screen  = NULL;
#if !defined(WIN32)
   if ( Real_Screen!=NULL ) ZONE_DESTROY(Real_Screen);
   Real_Screen = NULL;
#endif
}

#if !defined(WIN32)
static void Banner( )
{
   Out_Message( "\n- SoM v1.3 -" );
}
#endif

static void Help( )
{
#if !defined(WIN32)
   Banner( );
   Out_Message( "\noptions:" );
   Out_Message( "  -double ........... window size x2" );
   Out_Message( "  -nosound .......... turn sound on/off" );
   Out_Message( "  -mp3 <file.mp3> ... guess what :)" );
   Out_Message( "  -vol <float> ...... output volume scale (default 0.80)" );      
   Out_Message( "  -mono ............. force mono output" );
   Out_Message( "  -8b ............... force 8bits sound output" );
   Out_Message( "  -nb <int> ......... Nb MP3 frames ahead (%d).", Nb_Frames );
#ifdef LOWMEM
   Out_Message( "  -lowmem ........... skip some stuff for weak PCs (buggy)" );
#endif
   Out_Message( "" );
#if defined( DUMP_OK )
   Out_Message( " [space key]: dumps a snapshot 'snapxxx.jpg' in current directory." );
#endif

#else
   Out_Message( "options:\n\
   -double ........... window size x2\n\
   -nosound .......... turn sound on/off\n\
   -nofull ........... Don't use DDraw's fullscreen\n\
   -mp3 <file.mp3> ... guess what :)\n\
   -vol <float> ...... output volume scale (default 0.80)\n\
   -mono ............. force mono output\n\
   -8b ............... force 8bits sound output\n\
   -nb <int> ......... Nb MP3 frames ahead (%d)\n\
   ", Nb_Frames );
#endif	// WIN32

   exit( 1 );
}

/*********************************************************************/
/*********************************************************************/

static void Select_Screen_VBuffer( VBUFFER *VBuf, MEM_ZONE *Screen )
{
   if (!(VBuf->Flags&VBUF_VIRTUAL) )
      M_Free( VBuf->Bits );

   VBuf->Type = VSCR;
   VBuf->W = MEM_Width( Screen );
   VBuf->H = MEM_Height( Screen );
   VBuf->BpS = MEM_BpS( Screen );
   VBuf->Size = VBuf->H*VBuf->BpS;
   VBuf->Bits = (void *)ZONE_SCANLINE( Screen, 0 );
   VBuf->Nb_Col = 0; // a priori.
   VBuf->CMap = NULL;
   VBuf->Flags = VBUF_VIRTUAL;
   VBuf->Quantum = sizeof( USHORT );   // 0x565
}

EXTERN void Setup_Video( )
{
#if defined( WIN32 )

   Register_Video_Support( 1, NULL );
   The_Screen = Driver_Call( NULL );
   if ( The_Screen == NULL ) {
      Driver_Close( The_Screen );
      The_Screen=NULL;
      Out_Message( "DDirect draw init (16bpp) failed" );
      goto Failed;
   }
   Select_Screen_VBuffer( &VB(VSCREEN), The_Screen );

#else

#ifndef USE_SVGA_LIB
#define  _G_SVGL_DRIVER_ 0
#endif

   Register_Video_Support( 4, _G_DGA_DRIVER_,
      _G_X11_DRIVER_, _G_SVGL_DRIVER_, _G_VBE_DRIVER_ );
   if ( !DoubleSize ) 
      The_Screen = (MEM_ZONE*)Driver_Call( NULL,
         DRV_NAME, "S.o.M.",
         DRV_MODE, "320:200:0x2565",
         DRV_CONVERT,
         DRV_END_ARG );
   else {
      Real_Screen = (MEM_ZONE*)Driver_Call( NULL,
         DRV_NAME, "S.o.M.",
         DRV_MODE, "640:400:0x2565",
         DRV_CONVERT,
         DRV_END_ARG );
      if ( Real_Screen==NULL ) goto Oops;
      The_Screen = New_MEM_Zone(MEM_ZONE,DEFAULT_METHODS);
      if ( The_Screen==NULL ) goto Oops;
      if (Mem_Img_Set(MEM_IMG(The_Screen),IMG_NO_FLAG,
         320,200,0,2,(FORMAT)0x2565,NULL)==NULL ) goto Oops;
   }
   if ( The_Screen == NULL )
   {
Oops:
      Out_Message( "Can't open 16bpp mode!" );
      Driver_Print_Error( );
      goto Failed;
   }
   Select_Screen_VBuffer( &VB(VSCREEN), The_Screen );
#endif
   return;

Failed:
   Destroy_All( );
   exit( 1 );
}

#if !defined(WIN32)
void Do_Flush() {
   if (DoubleSize) 
   {
      USHORT *Src = (USHORT*)ZONE_SCANLINE(The_Screen,0);
      INT BpS = MEM_BpS(Real_Screen);
      USHORT *Dst = (USHORT*)ZONE_SCANLINE(Real_Screen,0);
      USHORT *Dst2;
      INT i,j;
      BpS /= sizeof(USHORT);
      Dst2 = Dst+BpS;
      BpS*=2;
      Dst+=The_W*2;
      Dst2+=The_W*2;
      Src+=The_W;
      for( j = The_H; j>0; --j ) {
         for(i=-The_W;i<0;++i)
            Dst[2*i] = Dst[2*i+1] =
              Dst2[2*i] = Dst2[2*i+1] =
                Src[i];
         Dst += BpS;
         Dst2 += BpS;
         Src+=The_W;
      }
      ZONE_FLUSH( Real_Screen );
   }
   else ZONE_FLUSH( The_Screen );   
}
#endif

/********************************************************************/

extern void Load_All()
{
   OUTS( "Loading & massaging...\n" );

   memset( &Mixer.VBuffers[0], 0, sizeof( VBUFFER )*MAX_VBUFFER );

   Pre_Cmp_Anim( ); OUTS( "." );
   Pre_Cmp_6( ); OUTS( "." );
   Pre_Cmp_World2( ); OUTS( "." );
   Pre_Cmp_World3( ); OUTS( "." );
   Pre_Cmp_World4( ); OUTS( "." );
   Pre_Cmp_Mask( ); OUTS( "." );
   Pre_Cmp_Dsp( FALSE ); OUTS( "." );
   Init_Scratch( ); OUTS( "." );
   Pre_Cmp_Appear( ); OUTS( "." );
   Pre_Cmp_Caustics( ); OUTS( "." );
   Pre_Cmp_Ptcl( ); OUTS( "." );
   Init_Letters( );
   Pre_Cmp_Destroy_Screen( );
   Pre_Cmp_Blb( ); 
   /*  
      Pre_Cmp_Flr( );
      Init_Tunnels( FALSE ); OUTS( "." );
   */
   OUTS( ".\n" );
}

int main( int argc, char **argv )
{
   INT Try, n;
   FLT MP3_Vol;
#if defined(WIN32)
   FullScreen = TRUE;
#endif
   DoubleSize = FALSE;
   No_Sound = FALSE;
   Low_Mem = FALSE;
   MP3_File = NULL;
   _MP3_Out_ = MP3_OUT_STEREO|MP3_OUT_16BITS;
   Nb_Frames = 6;
   MP3_Vol = 0.80f;
   Try = 1;
   n = 1;

   while ( n<argc )
   {
      if ( argv[n][0] == '+' || argv[n][0] == '-' )
      {
         if ( !strcmp( argv[n], "-h" ) || !strcmp( argv[n], "-H" ) )
            Help( );
         else if ( !strcmp( argv[n], "-double" ) ) DoubleSize = TRUE;
         else if ( !strcmp( argv[n], "-nosound" ) ) No_Sound = TRUE;
//         else if ( !strcmp( argv[n], "-db" ) ) _MP3_Out_ |= MP3_DEBUG;
         else if ( !strcmp( argv[n], "-8b" ) ) _MP3_Out_ &= ~MP3_OUT_16BITS;
         else if ( !strcmp( argv[n], "-mono" ) ) _MP3_Out_ &= ~MP3_OUT_STEREO;
//         else if ( !strcmp( argv[n], "-ask" ) ) _MP3_Out_ |= MP3_ASK_DRIVER;
         else if ( !strcmp( argv[n], "-couettes" ) ) { Out_Message( "ReZ, stop gamines." ); exit( 0 ); }
         else if ( !strcmp( argv[n], "-nb" ) )
         {
            n++;
            if ( n==argc ) Exit_Upon_Error( "Missing value after -nb option" );
            Nb_Frames = atoi( argv[n] );
            if ( Nb_Frames<2 ) Nb_Frames = 2;
            else if ( Nb_Frames>20 ) Nb_Frames = 20;
         }
#ifdef LOWMEM
         else if ( !strcmp( argv[n], "-lowmem" ) ) Low_Mem = TRUE;
#endif
#if defined(WIN32)
         else if ( !strcmp( argv[n], "-nofull" ) ) FullScreen = FALSE;
#endif
#ifdef DEBUG_OK
         else if ( !strcmp( argv[n], "-d" ) ) Debug( );
#endif
         else if ( !strcmp( argv[n], "-mp3" ) )
         {
            if ( n==argc ) Exit_Upon_Error( "missing file name for -mp3 option" );
            MP3_File = argv[++n];
         }
         else if ( !strcmp( argv[n], "-vol" ) )
         {
            if ( n==argc ) Exit_Upon_Error( "missing value for -vol option" );
            sscanf( argv[++n], "%f", &MP3_Vol );
         }
#ifdef BUILD_DAT_FILE
         else if ( !strcmp( argv[n], "-D" ) ) Write_Dat = TRUE;
#endif
      }
      else
      {
         Try = atoi( argv[n] );
         if ( Try<1 ) Try = 1;
         if ( Try>LAST_PHASE-1 ) Try = LAST_PHASE-1;
      }
      n++;
   }

#ifdef BUILD_DAT_FILE
   USE_DAT_FILE( DAT_FILE_NAME, Write_Dat );
#else
   USE_DAT_FILE( DAT_FILE_NAME, FALSE );
#endif

#ifndef UNIX
    MP3_Init_FPU();     // global FPU setting
#endif

    Init_Timing( LAST_PHASE, Phases );

#if defined(WIN32)
   if ( Launch_Dialog()==0 ) return 0;
#else
   Load_All();
#endif
   
   if ( !No_Sound ) Register_All_Audio_Drivers( );

      // MP3 streaming setup

   if ( No_Sound )
      goto Ok;


//   MP3_Handle_Spectrum = Handle_Spectrum;

   if ( MP3_File==NULL ) MP3_File = MP3_FILE_NAME;
   else Close_Dat_File( );   // not needed anymore
   MP3_Strm = MP3_Init( MP3_File, _MP3_Out_, MP3_Vol );
   if ( MP3_Strm==NULL )   // No_Sound = TRUE;
   {
#if defined(WIN32)
      Driver_Close( The_Screen );
      The_Screen=NULL;
      Out_Message( "Sound initialization failed.\nPlease use '-nosound' option to proceed anyway." );
#else
      Out_Message( "Please use '-nosound' option to proceed anyway." );
#endif
      exit( 1 );
   }

Ok:
   Close_Dat_File( );   // not needed anymore

   Setup_Video( );
   Init_Mixing( FALSE );

      // C'est parti...

//   Init_Spectrum( (void*)MP3_Strm );

   Start_Demo( Try-1 );

#if defined( WIN32 )
   
   while(1)
   {
      if ( !No_Sound ) MP3_More( MP3_Strm, Timer, Nb_Frames );
      if ( Advance_Timer( ) == -1 ) break;
      ZONE_FLUSH( The_Screen );
      if ( Driver_Get_Event( The_Screen )==DRV_KEY_PRESS ) break;
   }

#else

   while(1)
   {
      DRV_EVENT *Ev;

      if ( !No_Sound ) MP3_More( MP3_Strm, Timer, Nb_Frames );
      if ( Advance_Timer( ) == -1 ) break;
      Do_Flush();ZONE_FLUSH( The_Screen );

#ifdef ECHO_TIMER
      {
         static long Last_Value = -1;
         if ( Last_Value != -1 ) // petit hack rapide pour afficher Timer
            fprintf( stderr, "%ld   %ld   %d %d\r", Last_Value, Timer,
               MP3_Strm==NULL ? 0 : MP3_Strm->Cur_Frame, Phase );
      }
#endif   // ECHO_TIMER

      if (DoubleSize) { 
         Driver_Get_Event( Real_Screen );
         Ev = &((MEM_ZONE_DRIVER *)Real_Screen)->Event;
      }
      else {
         Driver_Get_Event( The_Screen );
         Ev = &((MEM_ZONE_DRIVER *)The_Screen)->Event;
      }
      if ( Ev->Event & DRV_KEY_PRESS ) 
      {
         switch( Ev->Key )
         {
#ifdef ECHO_TIMER
            case 't':   // hack
            {
               if ( Last_Value==-1 ) Last_Value = Timer;
               else Last_Value = -1;
            }
            break;
#endif   // ECHO_TIMER

#if defined(DUMP_OK)
            case ' ':
            {
               static INT Count = 1;
               char Name[256];
               sprintf( Name, "snap%.3d.jpg", Count++ );
               if ( DoubleSize ) 
                  JPEG_Out( Name, ZONE_SCANLINE( Real_Screen, 0 ),
                     MEM_BpS(Real_Screen)/2, MEM_Height(Real_Screen), 0x03|0x08 );
               else JPEG_Out( Name, ZONE_SCANLINE( The_Screen, 0 ),
                  MEM_BpS(The_Screen)/2, MEM_Height(The_Screen), 0x03|0x08 );
            }
            break;
#endif
            default: goto Finished; break;
         }
      } 
   }

Finished:

#endif

   Destroy_All( );

#if !defined(WIN32)
   Banner( );
   Out_Message( "\n'%s -h' for options list.\n", argv[0] );
#endif

   return 0;
}

/********************************************************************/
