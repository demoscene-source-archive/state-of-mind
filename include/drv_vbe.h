/***********************************************
 *      VBE2.0 graphics Driver                 *
 * Skal 96                                     *
 ***********************************************/

#ifndef _DRV_VBE_H_
#define _DRV_VBE_H_

#include "main.h"
#include <stdarg.h>

#ifdef USE_VBE

/********************************************************/

#if defined(__DJGPP__)

#include <dpmi.h>
#include <sys/nearptr.h>
#include <sys/farptr.h>
#include <go32.h>          // <= for _dos_ds
#include <sys/exceptn.h>   // <= for _djgpp_ds_alias

extern int kbhit( );
extern int getkey( );  /* ALT's have 0x100 set */

#define DOS_CTRL_KEY    0x04
#define DOS_SHIFT_KEY   0x83
#define DOS_ALT_KEY     0x08
#define DOS_TAB_KEY     23
#define DOS_LEFT_KEY    100
#define DOS_RIGHT_KEY   102
#define DOS_UP_KEY      98
#define DOS_DOWN_KEY    104

#endif   // __DJGPP__

#if defined(__WATCOMC__)

#include <i86.h>

      // DPMI registers       <=>  <dpmi.h>
typedef union {
#pragma pack(1)
  struct {
    unsigned long edi;
    unsigned long esi;
    unsigned long ebp;
    unsigned long res;
    unsigned long ebx;
    unsigned long edx;
    unsigned long ecx;
    unsigned long eax;
  } d;
  struct {
    unsigned short di, di_hi;
    unsigned short si, si_hi;
    unsigned short bp, bp_hi;
    unsigned short res, res_hi;
    unsigned short bx, bx_hi;
    unsigned short dx, dx_hi;
    unsigned short cx, cx_hi;
    unsigned short ax, ax_hi;
    unsigned short flags;
    unsigned short es;
    unsigned short ds;
    unsigned short fs;
    unsigned short gs;
    unsigned short ip;
    unsigned short cs;
    unsigned short sp;
    unsigned short ss;
  } x;
  struct {
    unsigned char edi[4];
    unsigned char esi[4];
    unsigned char ebp[4];
    unsigned char res[4];
    unsigned char bl, bh, ebx_b2, ebx_b3;
    unsigned char dl, dh, edx_b2, edx_b3;
    unsigned char cl, ch, ecx_b2, ecx_b3;
    unsigned char al, ah, eax_b2, eax_b3;
  } h;
} __dpmi_regs;

#define DOS_CTRL_KEY    0x04
#define DOS_SHIFT_KEY   0x83
#define DOS_ALT_KEY     0x08
#define DOS_TAB_KEY     23
#define DOS_LEFT_KEY    100
#define DOS_RIGHT_KEY   102
#define DOS_UP_KEY      98
#define DOS_DOWN_KEY    104

#endif      // __WATCOMC__

#include "out_regs.h"

/********************************************************/
/********************************************************/

typedef struct {
#pragma pack(1)

  USHORT  ModeAttributes;
  BYTE   WinAAttributes;
  BYTE   WinBAttributes;
  USHORT  Granularity;
  USHORT  WinSize;
  USHORT  WinASegment;
  USHORT  WinBSegment;
  UINT    WinFuncPtr;
  USHORT  bytePerLine;
  USHORT  XRes;
  USHORT  YRes;
  BYTE   XCharSize;
  BYTE   YCharSize;
  BYTE   NPlanes;
  BYTE   BitsPerPixel;
  BYTE   NBanks;
  BYTE   MemoryModel;
  BYTE   BankSize;
  BYTE   NPages;
  BYTE   Reserved;
  BYTE   RedMaskSize, RedFieldPos;
  BYTE   GreenMaskSize, GreenFieldPos;
  BYTE   BlueMaskSize, BlueFieldPos;
  BYTE   RsvdMaskSize, RsvdMaskPos;
  BYTE   DirectColorInfo;
  void   *PhysBasePtr;
  UINT    OffScreenMemOffset;
  USHORT  OffScreenMemSize;
  BYTE   reserved[ 206 ];

} VBE_MODE;

typedef struct {
#pragma pack(1)
  char     VbeSignature[4];
  USHORT   VbeVersion;  		/* 4 */
  void    *OemStringPtr;   	/* 6 */
  BYTE    Capabilities[4];	/* 10 */
  USHORT  *VideoModePtr;	   /* 14 */
  USHORT   TotalMemory;	   	/* 18 */
  USHORT   OemSoftwareRev;	   /* 20 */
  void    *OemVendorNamePtr;	/* 22 */
  void    *OemProductNamePtr;	/* 26 */
  void    *OemProductRevPtr;	/* 30 */
  BYTE    Reserved[ 222 ];
  BYTE    OemData[256];
} VBE_INFO;

/********************************************************/

#include "v_cards.h"

/********************************************************/

typedef struct MEM_ZONE_VBE MEM_ZONE_VBE;

struct MEM_ZONE_VBE {
#pragma pack(1)

   MEM_ZONE_DRIVER_FIELDS

      /* VBE specific */

   MEM_IMG      The_Display;
   VBE_INFO    *VInfo;
   INT          RAMDAC_Cur_Width, RAMDAC_Width; 
   PIXEL       *Base_Ptr;
   USHORT       Mode_Number;
   INT          Bank_Size, Total_Mem;
   INT          VESA_Win_No, Cur_Bank;    /* 176/180 */
   void        (*Set_Bank)( MEM_ZONE_VBE *, INT );
   void        *LFB_To_Be_Mapped;
   void        *PM_WinFuncPtr;         /* 188 */
   void        *RM_WinFuncPtr;         /* 192 */
   __dpmi_regs  SVGA_Regs;             /* 196 */

   CARD_METHODS *Video_Card;

};


/********************************************************/

   /* needed by v_vards.c.  => external */

extern PIXEL *Get_VBE_Info( PIXEL **VInfo, INT Vbe20 );
extern VBE_MODE *Get_Mode_Info( USHORT Mode, VBE_MODE *_VBE_Info_ );
extern void Vesa_Info_To_Mem_Img( MEM_IMG *M, VBE_MODE *Info );
extern MEM_IMG *VBE_Detect_Available_Modes( MEM_ZONE_VBE *VBE, USHORT *Ptr );
extern void VBE_Print_Mode_Info( STRING S, MEM_IMG *M );
extern USHORT VBE_Set_VESA_Mode( USHORT Mode, MEM_ZONE_VBE *VBE );
extern INT Do_Init_VESA_Mode( MEM_ZONE_VBE *VBE );

#endif      // USE_VBE


   /* Some DOS funcs in dos_mem.c */

#if defined(__DJGPP__)

extern void _Enable( PIXEL *, INT );
extern void _Disable( );
extern void Safe_Bzero( PIXEL *Ptr, INT Size );

   // in dos_mem.c

extern int _The_DS_;
extern unsigned _The_Segment_;
extern UINT Enabled;
#define Dos_Data  (Dos_Segment<<4)

#endif   // __DJGPP__


#if defined(__WATCOMC__)

#define _Enable(A,B)
#define _Disable( )
#define Safe_Bzero(P,S) bzero( (P),(S) )
#define movedata(s1,P1,s2,P2,L) memcpy( (P2),(P1),(L) )
#define _farpokeb(s1,P,v)  (char *)(P) = (v)
#define _farpeekw(s1,P)  (*(unsigned short *)(P))
#define _farpeekb(s1,P)  (*(unsigned char *)(P))

   // in dos_mem.c

extern void *Dos_Data;
extern INT __dpmi_int( char no, __dpmi_regs *in ); // simulate djgpp's one. 
#define dosmemput(Src,Len,Dst)   _fmemcpy( (Dst),(Src),(Len) )
#define dosmemget(Src,Len,Dst)   _fmemcpy( (Dst),(Src),(Len) )

#endif   // __WATCOMC__

   // in dos_mem.c

extern USHORT Dos_Segment;
extern void DOSemu_MEM_Copy_Rect( MEM_ZONE *M,
   PIXEL *Dst, PIXEL *Src, INT Width, INT Height, INT Pad_Dst );

extern void Free_Dos_Buffer( );
extern USHORT Allocate_Dos_Buffer( UINT Size );
extern PIXEL *Map_Physical_Address( UINT Size, PIXEL *Address );
extern void Unmap_Physical_Address( PIXEL *Address );
extern STRING _Retreive_String( PIXEL *Ptr, INT n, STRING S );
extern STRING _Retreive_String2( UINT Seg, UINT Off, INT n, STRING S );

   // Some DOS funcs in drv_vga.c

extern UINT VBE_Search_Best_X_Mode( MEM_IMG *M, MEM_IMG *X_Mode );
extern void VBE_Init_VGA_Mode( UINT Mode, MEM_ZONE_VBE * );
extern void VGA_Mode_Set( UINT Mode );

/********************************************************/
/********************************************************/

#endif   //_DRV_VBE_H_

