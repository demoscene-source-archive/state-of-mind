/***********************************************
 *         outs utilities                      *
 * rem: what a mess...                         *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "out_regs.h"

/////////////////////////////////////////////////////////////////////////

#if defined(__DJGPP__)

#if 0

EXTERN UINT inb( USHORT Port )
{
   UINT Ret;

   asm( "\tmovw %1,%%dx\n"
        "\txorl %%eax,%%eax\n"
        "\tinb  %%dx,%%al\n"
        "\tmovl %%eax,%0\n"	/* <= this line is brain damaged with -O6 flag */
        : "=g" (Ret)
        : "g" (Port)
        : "eax", "edx", "ebp", "esp" );
   return( Ret );
}

EXTERN UINT inw( USHORT Port )
{
   UINT Ret;

   asm( "\tmovw %1,%%dx\n"
        "\txorl %%eax,%%eax\n"
        "\tinw  %%dx,%%ax\n"
        "\tmovl %%eax,%0\n"	/* <= this line is brain damaged with -O6 flag */
        : "=g" (Ret)
        : "g" (Port)
        : "eax", "edx", "ebp", "esp" );
   return( Ret );
}

EXTERN void outb( USHORT Port, UINT Val )
{
   asm( "\tmovw %0,%%dx\n"
        "\tmovl %1,%%eax\n"
        "\toutb %%al,%%dx\n"
        :
        : "g" (Port), "g" (Val)
        : "eax", "edx", "ebp", "esp" );
}

EXTERN void outw( USHORT Port, UINT Val )
{
   out
   asm( "\tmovw %0,%%dx\n"
        "\tmovl %1,%%eax\n"
        "\toutw %%ax,%%dx\n"
        :
        : "g" (Port), "g" (Val)
        : "eax", "edx", "ebp", "esp" );
}
#endif      // 0

#endif      // __DJGPP__

/////////////////////////////////////////////////////////////////////////

EXTERN UINT inCR( UINT Index )
{
   outb( 0x03d4, Index );
   return( (UINT)inb( 0x03d5 ) );
}

EXTERN void Out_Crts( USHORT *Tab )
{
   /* Stop_Restart_Seq( ); */
   outw( SEQ_P, 0x0100 ); 
   outb( MISC_OUT, 0xe3 );
   outw( SEQ_P, 0x0300 );

   outCR( 0x11, inCR( 0x11 ) & 0x7F ); /* Remove protect...*/
   while( *Tab != 0x0000 ) outw( CRTC_P, *Tab++ );
   outCR( 0x11, inCR( 0x11 ) | 0x80 ); /* Restore protection */
}

/////////////////////////////////////////////////////////////////////////
