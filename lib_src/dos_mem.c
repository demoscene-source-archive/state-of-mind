/***********************************************
 *          Some DOS thingies                  *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "mem_map.h"
#include "driver.h"

#if defined(__DJGPP__) || defined(__WATCOMC__)

/************************************************************/

EXTERN USHORT Dos_Segment = 0;

#if defined(__DJGPP__)

EXTERN int _The_DS_ = -1;
EXTERN unsigned _The_Segment_ = 0;
EXTERN UINT Enabled = FALSE;

static _go32_dpmi_seginfo Dos_Info = { 0 };

#endif   // __DJGPP__


#if defined(__WATCOMC__)

EXTERN void *Dos_Data = NULL;
static USHORT W_Selector = -1;
static INT W_Size = 0;

EXTERN INT __dpmi_int( char no, __dpmi_regs *in )
{
   union REGS r;
   r.w.ax       = 0x300;
   r.h.bl       = no;
   r.h.bh       = 0;
   r.w.cx       = 0;
   r.x.edi      = FP_OFF( in );
   int386( 0x31, &r, &r );

   return( r.w.cflag );

}
#endif   // __WATCOMC__


/************************************************************/

#ifndef SKL_LIGHT_CODE
static int CPUs[] = { 0x0, 0x0, 286, 386, 486, 586 };
#endif   // SKL_LIGHT_CODE

/************************************************************/

EXTERN void Dos_Print_Info( )
{
#ifndef SKL_LIGHT_CODE

#if defined(__DJGPP__)

   int Flags;
   char String[128];
   __dpmi_version_ret Version;

   if ( __dpmi_get_version( &Version ) == -1 ) return;
   Out_Message( "DPMI:\n * version %d.%d",
      Version.major, Version.minor );
   Out_Message( " * %d-bits impl.", ( Version.flags & 0x01 )? 32 : 16 );
   Out_Message( " * Virtual mem. %savailable", ( Version.flags & 0x04 )? "" : "not " );
   if ( Version.cpu<0x06 )
      Out_Message( " * CPU: %d", CPUs[ Version.cpu ] );

   if ( __dpmi_get_capabilities( &Flags, String ) == -1 )
      Out_Message( "Can't get capabilities" );
   else
   {
      Out_Message( "Capabilities:" );
      Out_Message( " * Device mapping: %s", ( Flags&0x04 ) ? "OK" : "!OK" );
      Out_Message( " * Conv. mem mapping: %s", ( Flags&0x08 ) ? "OK" : "!OK" );
      Out_Message( " * Vendor (%c.%c): %s", String[0] + '0', String[1] + '0', String + 2 );
   }


#endif   // __DJGPP__

#endif   // SKL_LIGHT_CODE
}

/************************************************************/

EXTERN void Free_Dos_Buffer( )
{

#if defined(__DJGPP__)
   _go32_dpmi_free_dos_memory( &Dos_Info );
   Dos_Segment = 0; Dos_Info.size = 0;
#endif

#if defined(__WATCOMC__)
   union REGS dr;   
   dr.w.ax = 0x0101;
   dr.w.dx = W_Selector;  
   int386(0x31, &dr, &dr);
   Dos_Segment = 0; W_Size = 0;
   W_Selector = -1;
#endif                    

}

EXTERN USHORT Allocate_Dos_Buffer( UINT Size )
{

#if defined(__DJGPP__)

   if ( Dos_Info.size != Size )
   {
      if ( Dos_Info.size != 0 )
      {
         _go32_dpmi_free_dos_memory( &Dos_Info );
         Dos_Segment = 0;
      }
      Dos_Info.size = Size;
      if ( _go32_dpmi_allocate_dos_memory( &Dos_Info ) )
      {
         SET_DRV_ERR( "Could not allocate DOS buffer" );
         Dos_Segment = 0; Dos_Info.size = 0;
         return( 0 );
      }
      Dos_Segment = Dos_Info.rm_segment;
   }

#endif   // __DJGPP__

#if defined(__WATCOMC__)

   if ( W_Size != Size )
   {
      union REGS r;
      if ( W_Size != 0 )
      {
         Free_Dos_Buffer( );
      }
      r.w.ax = 0x0100;
      r.w.bx = (Size/16) + 1;
      int386( 0x31, &r, &r );  // allocate DOS memory
      Dos_Segment  = r.w.ax;
      W_Selector = r.w.dx;
      W_Size = Size;

      r.w.ax = 0x0006;
      r.w.bx = W_Selector;
      int386(0x31, &r, &r);
      Dos_Data = (void *)( ( ((UINT)r.w.cx)<<16 ) + r.w.dx );
   }

#endif   // __WATCOMC__

   return( Dos_Segment );
}

/************************************************************/

#if defined(__DJGPP__)

/* We only need these 2 funcs for mapping linear frame buffer.
 * So infos will stay in Mem_Info between the calls...
 * No safe, but that's it. :)
 */
static __dpmi_meminfo Mem_Info = { 0 };
static void *Malloced = NULL;

#endif


EXTERN PIXEL *Map_Physical_Address( UINT Size, PIXEL *Address )
{
   Size |= 0xFFFF;

#if defined(__DJGPP__)

// fprintf( stderr, "physical address: 0x%x Size: %d  __djgpp:0x%.8x\n", Address, Size, __djgpp_conventional_base );

   Malloced = NULL;
   Mem_Info.size = Size;
   Mem_Info.address = ( UINT )Address;

   if ( __dpmi_physical_address_mapping( &Mem_Info ) != -1 )
      Address = ( PIXEL *)( Mem_Info.address );
   else
   {
      UINT Real_Add;

      Size = (Size|0x1FFF) + 1;

      Malloced = (void *)My_Malloc( Size );
      if ( Malloced == NULL ) goto Failed;
      Mem_Info.address = (UINT)Malloced;
      Mem_Info.size = Size;
      Real_Add = ( (UINT)Malloced | 0xFFF ) + 1;   // align to page

// fprintf( stderr, "Malloc mapping: 0x%x=>0x%x (0x%x bytes)\n", Address, Malloced, Size );

      if ( __djgpp_map_physical_memory( (void *)Real_Add, Size, (unsigned long)Address )==-1 )
      {
// fprintf( stderr, "Map_* failed\n" );
         Malloced = NULL;
         goto Failed;
      }
      Address = (PIXEL *)Real_Add;
   }

#endif      // __DJGPP__

#if defined(__WATCOMC__)

   {
      union REGS  r;
      r.x.eax = 0x0800;
      r.x.ebx = (UINT)Address >> 16;              // hi
      r.x.ecx = (UINT)Address & 0xffff;           // lo
      r.w.si  = Size >> 16;
      r.w.di  = Size & 0xffff;
      int386( 0x31, &r, &r );  

      Address = (PIXEL *)( r.w.cx + ( r.w.bx<<16 ) );
   }

#endif   // __WATCOMC__

//   fprintf( stderr, "Returned address:0x%.8x, Size:%d", Address, Size );

   return( Address );

Failed:
   SET_DRV_ERR2( "physical mapping of address 0x%x failed !\n", Address );
   return( (PIXEL*)0x0001 ); // <= don't use NULL ! WATCOM's returned Address *can* be 0x0000...
}

EXTERN void Unmap_Physical_Address( PIXEL *Address )
{
#if defined(__DJGPP__)

   if ( Malloced != NULL )
   {
      M_Free( Malloced );
      return;
   }

   Mem_Info.size = 0;
   Mem_Info.address = ( UINT )Address;

/* func 0x801 not found in CWSDPMI.EXE !!! This call can then fail. */
/* For PMODE/DJ, since physical and linear address are the same,
   we could skip that... We must call it anyway, just in case we're
   under DOSemu */
   if ( __dpmi_free_physical_address_mapping( &Mem_Info ) == -1 )
   {
      // Exit_Upon_Error( "__dpmi_free_physical_address_mapping() failed! Why!?" );
   }

#endif   // __DJGPP__

#if defined(__WATCOMC__)

   {
      union REGS r;
      r.x.eax = 0x801;
      r.x.ebx = ((UINT)Address) >> 16;
      r.x.ecx = ((UINT)Address) & 0xffff;
      int386(0x31, &r, &r);
   }

#endif      // __WATCOMC__
}

/************************************************************/

EXTERN STRING _Retreive_String( PIXEL *Ptr, INT n, STRING S )
{
   UINT Seg, Off;

   Seg = ( UINT )( *( USHORT *)( Ptr+2 ) );
   Off = ( UINT )( *( USHORT *)( Ptr ) );
   return( _Retreive_String2( Seg, Off, n, S ) );
}

EXTERN STRING _Retreive_String2( UINT Seg, UINT Off, INT n, STRING S )
{
   STRING Src;
   INT i;

   Src = (STRING)SEG_OFF_FP( Seg, Off );
   for( i=0; i<n-1; ++i )
   {
      char c;
      c = _farpeekb( _dos_ds, (unsigned)Src+i );
      if ( c=='\0' ) break;
      S[i] = c;
   }
   S[i] = '\0';
   return( S );
}

/************************************************************/

#if defined(__DJGPP__)

EXTERN void _Enable( PIXEL *Ptr, INT Size )
{
   unsigned Limit;

   if ( Enabled ) _Disable( );

   if ( !__djgpp_nearptr_enable( ) )  goto Dos_Emu_Workaround;

   Enabled = 1;
   return;

Dos_Emu_Workaround:
      /*
       * Hopefully, linux's kernel won't resent setting _my_ds()'s
       * limit to the desired Ptr+Size. At least, we can try :)
       */

   Limit = (unsigned)Ptr + Size + __djgpp_conventional_base;
   Limit |= 0xFFF;

   if ( __dpmi_set_segment_limit( _my_ds(), Limit ) )
      goto Really_Failed;
   if ( __dpmi_get_segment_limit( _my_ds() ) != Limit )
      goto Really_Failed;     // We set it but DPMI ignored/truncated it

   __dpmi_set_segment_limit( __djgpp_ds_alias, Limit );
   __dpmi_set_segment_limit( _my_cs(), Limit );
   Enabled = 2;
   return;

Really_Failed:
      /*
       * All right, all right... it all failed, so we're back
       * to this @#!&%*(#! selectors...:(
       */

   _The_Segment_ = (unsigned)Ptr & ~0xF;
   if ( _The_DS_==-1 )
   {
      _The_DS_ = __dpmi_segment_to_descriptor( _The_Segment_>>4 );
      if ( !_The_DS_ ) goto Get_The_Funk_Out;
   }
   Limit = ((unsigned)Ptr + Size ) | 0x1FFFF;
   if ( __dpmi_set_segment_base_address( _The_DS_, _The_Segment_ ) )
      goto Get_The_Funk_Out;
   if ( __dpmi_set_segment_limit( _The_DS_, Limit ) )
      goto Get_The_Funk_Out;

   Enabled = 3;
   return;

Get_The_Funk_Out:
   Exit_Upon_Error( " Fatal: Couldn't unprotect memory" );
}

EXTERN void _Disable( )
{
   if ( Enabled==1 )
      __djgpp_nearptr_disable( );
   // else: don't touch _The_DS_... it's useless...

   Enabled = 0;
   _The_Segment_ = 0;
}

#endif      // __DJGPP__

/************************************************************/

#if defined(__DJGPP__)

EXTERN void Safe_Bzero( PIXEL *Ptr, INT Size )
{
   if ( Enabled != 3 )
   {
      bzero( Ptr+__djgpp_conventional_base, Size );
      return;
   }
   for( --Size; Size>=0; --Size )
      _farpokeb( _The_DS_, (unsigned)Ptr + Size, 0 );
}

#endif

/************************************************************/

EXTERN void DOSemu_MEM_Copy_Rect( MEM_ZONE *M,
   PIXEL *Dst, PIXEL *Src, INT Width, INT Height, INT Pad_Dst )
{
   INT Pad_Src;

   Pad_Src = MEM_BpS( M );

   if ( M->CMapper.Dummy==NULL || M->CMapper.Dummy->Col_Convert == NULL )
   {
      Width *= MEM_Quantum( M );
      while( Height-->0 )
      {
         movedata(
            _my_ds(), (unsigned)Src,
            _The_DS_, (unsigned)Dst,
            Width );
         Dst += Pad_Dst;
         Src += Pad_Src;
      }
   }
   else
   {
      if ( M->CMapper.Dummy->Type == DITHERER_TYPE )
      {
         if ( M->CMapper.Ditherer->Dst_Stamp != NULL &&
            ( *M->CMapper.Ditherer->Dst_Stamp > M->CMapper.Ditherer->Stamp ) )
         {
               /* Dst_Stamp != NULL means that M->Dst != NULL. We can match colors */
            Skl_Match_CMaps( M->CMapper.Ditherer->Match, M->Dst->CMapper.Matcher->Cols, 256 );
            M->CMapper.Ditherer->Stamp = *M->CMapper.Ditherer->Dst_Stamp;
         }
      }
      while( Height-->0 )
      {
         PIXEL Tmp[ 4*8192 ]; // <= should be enough...

            // !!! Beware of alignment
         (*M->CMapper.Dummy->Col_Convert)( &M->CMapper, Tmp, Src, Width );
         movedata(
            _my_ds(), (unsigned)Tmp,
            _The_DS_, (unsigned)Dst,
            Pad_Dst );     // <= !!!!
         Dst += Pad_Dst;
         Src += Pad_Src;
      }
   }
}

/************************************************************/

#endif

