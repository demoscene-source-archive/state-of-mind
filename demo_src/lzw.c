/***********************************************
 *              LZW static datas               *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "lzw.h"

EXTERN INT LZW_Curr_Size, LZW_Nb_Avail_Bytes, LZW_Nb_Bits_Left;
EXTERN BYTE *LZW_Byte_Buffer = NULL;
EXTERN USHORT LZW_Code_Masks[ ] = {
      0x0000,
      0x0001, 0x0003, 0x0007, 0x000F,
      0x001F, 0x003F, 0x007F, 0x00FF,
      0x01FF, 0x03FF, 0x07FF, 0x0FFF,
      0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF
};
EXTERN UINT LZW_Cur_Byte = 0;
EXTERN BYTE *LZW_Bytes_Ptr = NULL;

