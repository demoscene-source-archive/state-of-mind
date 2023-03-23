/***********************************************
 *       VGA+X-modes                           *
 * Skal 97                                     *
 ***********************************************/

#include "main.h"
#include "mem_map.h"
#include "driver.h"
#include "out_regs.h"

#ifdef USE_VBE

/***********************************************************************/

typedef struct {
   INT Width, Height;
} X_MODE;

/***********************************************************************/

static USHORT ModeX_360_480_Table[ ] =
{
   0x6b00, 0x5901, 0x5a02, 0x8e03,
   0x5e04, 0x8a05, 0x0d06, 0x3e07,
   0x4009, 0xea10, 0xac11, 0xdf12,
   0x2d13, 0x0014, 0xe715, 0x0616,
   0xe317, 0x0000
};

static USHORT ModeX_360_240_Table[ ] =
{
   0x6b00, 0x5901, 0x5a02, 0x8e03,
   0x5e04, 0x8a05, 0x0d06, 0x3e07,
   0x4109, 0xea10, 0xac11, 0xdf12,
   0x2d13, 0x0014, 0xe715, 0x0616,
   0xe317, 0x0000
};
static USHORT ModeX_320_480_Table[ ] =
{
   0x0d06, 0x3e07, 0x4009, 0xea10,
   0xac11, 0xdf12, 0x0014, 0xe715,
   0x0616, 0xe317, 0x0000
};

static USHORT ModeX_320_240_Table[ ] =
{
   0x0d06, 0x3e07, 0x4109, 0xea10,
   0xac11, 0xdf12, 0x0014, 0xe715,
   0x0616, 0xe317, 0x0000
};
static USHORT ModeX_256_256_Table[ ] =
{
   0x5f00, 0x3f01, 0x4002, 0x8203,
   0x4e04, 0x9a05, 0x2306, 0xb207,
   0x0008, 0x6109, 0x0a10, 0xac11,
   0xff12, 0x2013, 0x4014, 0x0715,
   0x1716, 0xa317, 0x0000
};
/*
static USHORT Mode_S_320_200_Table[ ] =
{
   0x2306, 0xb207,
   0x0008, 0x6009, 0x0a10, 0xac11,
   0xff12, 0x2013, 0x4014, 0x0715,
   0x1716, 0xa317, 0x0000
};
*/

static USHORT Plane_Outs[4] = { 0x0102, 0x0202, 0x0402, 0x0802 };

/***********************************************************************/

static X_MODE X_Modes[] =     /* Lexically sorted by x and y */
{
   { 360, 480 },     /* 0 */
   { 360, 240 },     /* 1 */
   { 320, 480 },     /* 2 */
   { 320, 400 },     /* 3 */
   { 320, 240 },     /* 4 */
   { 320, 200 },     /* 5 */
   { 256, 256 }      /* 6 */
};

#define MAX_X_MODES   7

/***********************************************************************/

#if 0    // XXX. Useless

#define Clear_Vid_Mem( )   \
   asm( "\tmovw $0x03c4,%%dx  \n"      \
        "\tmovw $0x0f02,%%ax  \n"      \
        "\toutw %%ax,%%dx     \n"      \
        "\tmovl $0x1000,%%ecx \n"      \
        "\tmovl _A000,%%edi   \n"      \
        "\txorl %%eax,%%eax   \n"      \
        "\tcld                \n"      \
        "\trep                \n"      \
        "\tstosl              \n"      \
         : : : "eax", "ecx", "edx", "edi" )

#endif

/***********************************************************************/

EXTERN void VGA_Mode_Set( UINT Mode )
{
   __dpmi_regs r;
   Mem_Clear( &r );
   r.x.ax = (USHORT)Mode;
   __dpmi_int( 0x10, &r );
}

/***********************************************************************/

EXTERN UINT VBE_Search_Best_X_Mode( MEM_IMG *M, MEM_IMG *X_Mode )
{
   INT i, Best;

   Best = -1;
   for( i=0; i<MAX_X_MODES; ++i )
   {
      if ( IMG_Width(M)>X_Modes[i].Width ) continue;
      if ( IMG_Height(M)>X_Modes[i].Height ) continue;
      Best = i;
   }
   if ( Best==-1 ) { SET_DRV_ERR( "X-mode match failed" ); }
   else
   {
      IMG_Width( X_Mode ) = X_Modes[i].Width;
      IMG_Height( X_Mode ) = X_Modes[i].Height;
      IMG_Format( X_Mode ) = IMG_Format( M );
   }
   return( Best );
}

EXTERN void VBE_Init_VGA_Mode( UINT Mode, MEM_ZONE_VBE *VBE )
{
   if ( Mode>=MAX_X_MODES ) 
      Exit_Upon_Error( "X-mode %d not defined.", Mode );

   VGA_Mode_Set( 0x13 );

   if ( Mode == 4 ) /* Tweak 320x240 mode */
   {
      Disable_Chain4( );
      Out_Crts( ModeX_320_240_Table );
   }
   else if ( Mode == 2 ) /* Tweak 320x480 mode */
   {
      Disable_Chain4( );
      Out_Crts( ModeX_320_480_Table );
   }
   else if ( Mode == 3 ) /* Tweak 320x400 mode */
   {
      outw( SEQ_P, 0x0604 );   /* Disable chain4 */
      outw( CRTC_P, 0x4009 );
      outw( CRTC_P, 0xe317 );
      outw( CRTC_P, 0x0014 );
      outb( SEQ_P, 0xe3 );
   }
   else if ( Mode == 1 ) /* Tweak 360x240 mode */
   {
      Disable_Chain4( );
      Out_Crts( ModeX_360_240_Table );
   }
   else if ( Mode == 0 ) /* Tweak 360x480 mode */
   {
      Disable_Chain4( );
      Out_Crts( ModeX_360_480_Table );
   }
   else if ( Mode == 6 ) /* Tweak 256x256 mode */
   {
      Out_Crts( ModeX_256_256_Table );
   }
#if 0
   else if ( Mode == _320x200_H_ ) /* Tweak 320x200 highcol mode */
   {
      PIXEL i;

      VGA_Mode_Set( Mode );
      outb( SEQ_P, 0x04 );
      outb( SEQ_DATA, ( inb( SEQ_DATA ) & 0x7f ) | 0x04 );
      outb( CRTC_P, 0x14 );
      outb( CRTC_DATA, inb( CRTC_DATA ) & 0xbf );
      outb( CRTC_P, 0x17 );
      outb( CRTC_DATA, inb( CRTC_DATA ) | 0x40 );
      outb( CRTC_P, 0x09 );
      outb( CRTC_DATA, inb( CRTC_DATA ) & 0xe0 );
      outw( SEQ_P, 0x0f02 );
      for( i=0; i<64; ++i )
      {
         outb( 0x03c8, i );      /* Red */
         outb( 0x03c9, i ); outb( 0x03c9, 0 ); outb( 0x03c9, 0 );
         outb( 0x03c8, i+64 );   /* Green */
         outb( 0x03c9, 0 ); outb( 0x03c9, i ); outb( 0x03c9, 0 );
         outb( 0x03c8, i+128 );  /* Blue */
         outb( 0x03c9, 0 ); outb( 0x03c9, 0 ); outb( 0x03c9, i );
      }
   }
#endif
   else if ( Mode == 5 ) { /* Do nothing */ }

   VBE->The_Display.Width = X_Modes[Mode].Width;
   VBE->The_Display.Height = X_Modes[Mode].Height;
   VBE->The_Display.BpS = X_Modes[Mode].Width;
   VBE->The_Display.Size = VBE->The_Display.BpS*VBE->The_Display.Height;
   VBE->The_Display.Quantum = 1;
   VBE->The_Display.Pad = 0;
   VBE->Base_Ptr = (PIXEL *)BASE_FP( 0xA000 );
   VBE->Bank_Size = VBE->The_Display.Size;
   VBE->Cur_Bank = 0;
   VBE->The_Display.Format = FMT_CMAP;

   VBE->Refresh = _VGA_MODE_;
}

/***********************************************************************/

#endif   // USE_VBE
