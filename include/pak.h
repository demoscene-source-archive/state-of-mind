/***********************************************
 *        Packed Bitmaps as MEM_ZONE           *
 * Skal 97                                     *
 ***********************************************/

#ifndef _PAK_H_
#define _PAK_H_

/*******************************************************/

#include "btm.h"

typedef struct {
#pragma pack(1)

   MEM_ZONE_FIELDS

   INT Pak_Size;
   INT *H_Index;
   INT Nb_Col;
   COLOR_ENTRY *CMap;

} MEM_PAK;

extern MEM_PAK *Btm_To_Pak( BITMAP *Btm, INT Alpha );
extern MEM_PAK *Create_Mem_Pak( INT Height, INT Nb_Col );
extern INT Paste_Pak( MEM_ZONE *M, INT Xo, INT Yo );

   /* functions in ld_pak.c and sv_pak.c */

#define PAK_MAGIC_HEADER   "&skal_pak#"

extern MEM_PAK *Load_PAK( char * );
extern void Save_PAK( char *, MEM_PAK * );

typedef enum {
   CODE_EOL   = 0xFF,
   CODE_SKIP  = 0xFE,
   CODE_FILL  = 0xFD,
   CODE_BLOCK = 0xFC,
   CODE_SOLO  = 0xFB,
   CODE_LAST  = 0xFA
} CODE_TYPE;

/*******************************************************/

#endif /* _PAK_H_ */
