/***********************************************
 *                                             *
 *    sw_font.h                                *
 *                                             *
 * Skal widgets v0.0 (c)1997                   *
 * Pascal.Massimino@ens.fr                     *
 ***********************************************/
   
#ifndef _SW_FONT_H_
#define _SW_FONT_H_

/***************************************************************/
   
extern SW_FONT *SW_Load_Font( SW_TOP *, STRING Name );
extern SW_FONT *SW_Destroy_Font( SW_TOP *, SW_FONT *Font );
extern void SW_Destroy_Fonts( SW_TOP * );
extern void SW_Destroy_Icons( SW_TOP * );

extern BITMAP *SW_Access_Icon( SW_TOP *Top, INT Icon_Nb );

#define SW_GLOBAL_OFFSET   1
#define SW_DEFAULT_FONT    0

extern INT SW_Print_And_Refresh( MEM_ZONE *M, STRING String, 
      INT x, INT y, INT Type, INT Color, 
      SW_TOP *Top );
extern INT SW_Print( MEM_ZONE *M, STRING String, 
             INT x, INT y, INT Type, INT Color, 
             SW_TOP *Top );
extern void SW_Print_Message( SW_WIDGET *Msg_W, STRING Msg );

extern INT Cursor_Pos;

/***************************************************************/

#endif   /* _SW_FONT_H_ */
