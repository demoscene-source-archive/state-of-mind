/*
 * Debug mode / tests
 *
 ******************************/

#include "demo.h"
#include "spans.h"
#include "fonts.h"

extern  void Setup_Video( );
extern void Do_Blur_Y( VBUFFER *Out );

extern INT LAST_PHASE, NB_PHASE;
extern PHASE Phases[];

#if !defined( WIN32 )

#include "mem_map.h"
#include "driver.h"
#define OUTS(x) Out_String(x)

#else

#include "../src/gfx_ddrw.h"
typedef void *MEM_ZONE;
#define OUTS(x)  { }

#endif

extern MEM_ZONE *The_Screen;

/********************************************************************/

EXTERN void Init_D2t( INT Param )
{
   Cur_Mask = Mask122;
   Setup_Mask_II( VMASK, Cur_Mask );

   if ( Param==0 )
   {
      SELECT_CMAP( 1 );
      Check_CMap_VBuffer( &VB(1), 256 );

      SELECT_CMAP( 2 );
      Extract_DMask_Raw_Col( Cur_Mask, (BYTE*)VB(2).Bits );
//      SCnt = 0;

      Init_Caustics( 1 );      
      Time_Scale = 2600.0;
   }
   else if ( Param==1 )
   {
      Drv_Build_Ramp_16( VB(2).CMap, 0, 8, 0,0,0, 255,240,200, 0x2565 );
   }
   Drv_Build_Ramp_16( VB(1).CMap, 0, 256, 0,0,0, 140, 140, 250, 0x777 );
}

EXTERN void Loop_D2t( )
{
   FLT x;
   INT R, G, B;

   x = Get_Beat( );
   Scratch_And_Flash( x );
   Loop_Caustics_II( );
   Do_Blur_Y( &VB(1) );

   x = (FLT)fmod( 2.0*x, 1.0 ); // 4.0??
   R = (INT)(150.0 + 65.0+sin(x*M_PI) );
   G = (INT)(250.0 - 85.0+sin(x*M_PI) );
   B = (INT)(180.0 - 65.0+sin(x*M_PI) );
   Drv_Build_Ramp_16( (UINT*)VB(3).CMap, 0, 256, 0, 0, 0, R, G, B, 0x777 );

   Mixer.Mix_Sat8_8( &VB(VSCREEN), &VB(1), &VB(3) );
}


/********************************************************************/

EXTERN void Debug( )
{
   static PHASE DPhases[ ] =
   {
     {    0, 1, 0,  Init_Ptcl, Loop_Ptcl_I,     NULL, NULL,  2957,0, 0 },
     {20000, 0, 0,  NULL, Loop_0, NULL, NULL,  0,0, 0 },
     {   -1, 0, 0,  NULL, Loop_0, NULL, NULL,  120,0, 0 },
     { 0 }
   };
   INT N = sizeof( DPhases )/sizeof(DPhases[0]);
   INT i;

   OUTS( "debug mode..." );
   memcpy( Phases, DPhases, N*sizeof(PHASE) );

   Init_Timing( N, Phases );

   Pre_Cmp_Caustics( );
   Pre_Cmp_Ptcl( );
   Pre_Cmp_Destroy_Screen( );
   Init_Letters( );

   Setup_Video( );
   Init_Mixing( FALSE );

   Start_Demo( 0 );

   while(1)
   {
#ifdef LNX
   {
      LINT T1;
      T1 = _Clock( );
      if ( Advance_Timer( ) == -1 ) break;
      T1 = _Clock() - T1;
//      fprintf( stderr, "%d ticks  Timer:%d   \r", T1, Timer );
   }
#else
      if ( Advance_Timer( ) == -1 ) break;
#endif
      ZONE_FLUSH( The_Screen );
      if ( Driver_Get_Event( The_Screen )==DRV_KEY_PRESS ) break;
   }
   if ( The_Screen!=NULL ) Driver_Close( The_Screen );
   exit( 0 );
}

/********************************************************************/
