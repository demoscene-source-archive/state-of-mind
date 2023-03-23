/***********************************************
 *           TGA headers                       *
 * from netpbm's tga.h file.                   *
 ***********************************************/

#ifndef _TGA_H_
#define _TGA_H_

/*******************************************************************/

typedef struct
{
#pragma pack(1)
    PIXEL IDLength;		/* length of Identifier String */
    PIXEL CoMapType;		/* 0 = no map */
    PIXEL ImgType;		/* image type (see below for values) */
    PIXEL Index_lo, Index_hi;	/* index of first color map entry */
    PIXEL Length_lo, Length_hi;	/* number of entries in color map */
    PIXEL CoSize;		/* size of color map entry (15,16,24,32) */
    PIXEL X_org_lo, X_org_hi;	/* x origin of image */
    PIXEL Y_org_lo, Y_org_hi;	/* y origin of image */
    PIXEL Width_lo, Width_hi;	/* width of image */
    PIXEL Height_lo, Height_hi;	/* height of image */
    PIXEL PixelSize;		/* pixel size (8,16,24,32) */
    PIXEL AttBits;		/* 4 bits, number of attribute bits per pixel */
    PIXEL Rsrvd;		/* 1 bit, reserved */
    PIXEL OrgBit;		/* 1 bit, origin: 0=lower left, 1=upper left */
    PIXEL IntrLve;		/* 2 bits, interleaving flag */
} TGA_HEAD;

// Definitions for image types.

#define TGA_Null 0
#define TGA_Map 1
#define TGA_RGB 2
#define TGA_Mono 3
#define TGA_RLEMap 9
#define TGA_RLERGB 10
#define TGA_RLEMono 11
#define TGA_CompMap 32
#define TGA_CompMap4 33

// Definitions for interleave flag.

#define TGA_IL_None 0
#define TGA_IL_Two 1
#define TGA_IL_Four 2

/*******************************************************************/

#endif   // _TGA_H_
