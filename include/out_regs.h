/***********************************************
 *         outs utilities                      *
 * Skal 96                                     *
 ***********************************************/

#ifndef _OUT_REGS_H_
#define _OUT_REGS_H_

#define MISC_OUT  0x03c2
#define SEQ_P     0x03c4
#define SEQ_DATA  0x03c5
#define CRTC_P    0x03d4
#define CRTC_DATA 0x03d5

/////////////////////////////////////////////////////////////////////////

#if defined(__DJGPP__)

// #define _OUTW_3D4(S)         \
//  asm( " movw $0x03d4,%%dx\n" " movw %0,%%ax\n" " outw %%ax,%%dx\n" \
//    : : "g" ( (S) ) : "eax", "edx" )
// #define _OUTB_3D4(S)         \
//  asm( " movw $0x03d4,%%dx\n" " movw %0,%%ax\n" " outb %%al,%%dx\n" \
//    : : "g" ( (S) ) : "eax", "edx" )

#define outw(P,V) outportw( (unsigned short)(P), (unsigned short)(V) )
#define outb(P,V) outportb( (unsigned short)(P), (unsigned char)(V) )
#define inw(P) inportw( (unsigned short)(P) )
#define inb(P) inportb( (unsigned short)(P) )

#endif   // __DJGPP__

/////////////////////////////////////////////////////////////////////////

#if defined(__WATCOMC__) || defined( WIN32 )

#define outb(P,V)  outp((P),(V))
#define outw(P,V)  outpw((P),(V))
#define inb(P)  inp((P))
#define inw(P)  inpw((P))

#endif   // __WATCOMC__

/////////////////////////////////////////////////////////////////////////

#define _OUTW_3D4(S) outw(0x3d4,(S))
#define _OUTB_3D4(S) outb(0x3d4,(S))

#define Remove_Write_Protect( )     \
{ outb( CRTC_P, 0x11 ); outb( CRTC_DATA, inb( CRTC_DATA ) & 0x7f ); }

#define Restore_Write_Protect( )  \
{ outb( CRTC_P, 0x11 ); outb( CRTC_DATA, inb( CRTC_DATA ) | 0x80 ); }

#define Disable_Chain4( )           \
   outw( SEQ_P, 0x0604 )

#define outCR(I,V)   { outb( 0x03d4, (I) ); outb( 0x03d5, (V) ); }
// #define inCR(I)   ( outb( 0x03d4, (I) ), inb( 0x03d5 ) )

extern UINT inCR( UINT Index );
extern void Out_Crts( USHORT *Tab );

/////////////////////////////////////////////////////////////////////////

#endif   // _OUT_REGS_H_
