/***********************************************
 *   misc tables & useful FPU constants        *
 * Skal 98                                     *
 ***********************************************/

#ifndef _TABLES_H_
#define _TABLES_H_

/******************************************************************/

extern FLT _Div_Tab_64_[64];
extern FLT _Div_Tab_32_[32];
extern FLT _Div_Tab_16_[16];
extern FLT _Div_Tab_8_[8];
extern FLT _Div_Tab_4_[4];
extern FLT _Div_Tab_2_[2];
extern FLT _Div_Tab_1_[64];
extern FLT _CST_1_, _CST_64_,_CST_32_, _CST_16_, _CST_8_, _CST_4_, _CST_2_;
extern FLT _CST_65536_;

extern void Init_Anti_Alias_Table( INT C0, INT C1, FLT Gamma );
extern PIXEL AA_Table[256];   // Anti-alias table

/******************************************************************/

#endif   // _TABLES_H_
