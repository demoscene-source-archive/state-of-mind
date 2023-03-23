/***********************************************
 *        basic CPU detecting                  *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "cpu.h"

/***********************************************/

EXTERN UINT _CPU_ = _CPU_UNKNOWN_;
EXTERN UINT _CPU_Speed_ = 0;

#ifdef UNIX
#define ASM_Detect_CPU( )  _CPU_UNKNOWN_
#endif

EXTERN UINT Detect_CPU( )
{
   _CPU_ = ASM_Detect_CPU( );
   return( _CPU_ );
}

EXTERN UINT Print_CPU_Specs( )
{
   UINT CPU_ID = 0, CPU_EDX = 0;

   if ( _CPU_ == _CPU_UNKNOWN_ )
      _CPU_ = ASM_Detect_CPU( );

   switch ( _CPU_ )
   {
      case _CPU_x86_: Out_Message( "CPU: <=386" ); break;
      case _CPU_OLD486_: Out_Message( "CPU: Old 486 (no CPUID)" ); break;
   }
   if ( _CPU_ == _CPU_486_ || _CPU_ == _CPU_586_ )
   {
#if defined(DOS) ||  defined(LNX)
      asm( "\tmovl $0x01,%%eax\n"
           "\t.byte 0x0F\n"
           "\t.byte 0xA2\n"
           "\tmovl %%eax,%0\n"
           "\tmovl %%edx,%1\n"
           : "=g" ( CPU_ID ), "=g" ( CPU_EDX )
           :
           : "eax", "ebx", "ecx", "edx" );
      Out_Message( "CPU: %s      ( CPUID: Familly: %d  Model: %d  Stepping: %d )",
         _CPU_ == _CPU_486_ ? "486" : "586",
         ( CPU_ID >> 8 )&0x07, ( CPU_ID >> 4 )&0x07, CPU_ID&0x07 );
      if ( CPU_EDX & 0x01 )
         Out_Message( "   - Built-in FPU" );
      if ( CPU_EDX & 0x02 )
         Out_Message( "   - V86-mode OK" );
      if ( CPU_EDX & 0x04 )
         Out_Message( "   - I/O Breakpoints" );
      if ( CPU_EDX & 0x08 )
         Out_Message( "   - Page size ext." );
      if ( CPU_EDX & 0x10 )
         Out_Message( "   - Time Stamp counter" );
      if ( CPU_EDX & 0x20 )
         Out_Message( "   - Pentium-specific registers" );
      if ( CPU_EDX & 0x80 )
         Out_Message( "   - Exception Check" );
      if ( CPU_EDX & 0x100 )
         Out_Message( "   - CMPXCHG8B instr." );
#endif
   }
   else Out_Message( "CPU: Unknown" );
   return( _CPU_ );
}

/***********************************************/

