/***********************************************
 *              Mem transfert                  *
 * Skal 96                                     *
 ***********************************************/

#ifndef _MASKS_H_
#define _MASKS_H_

/********************************************************/

typedef enum {
   RGB_MASK_TYPE,
   INDEXER_TYPE,
   DITHERER_TYPE,
   COLOR_CMAP_TYPE
} SKL_CMAPPER_TYPE;

typedef struct RGB_MASK RGB_MASK;
typedef struct COLOR_INDEXER COLOR_INDEXER;
typedef struct COLOR_DITHERER COLOR_DITHERER;
typedef struct COLOR_CMAP COLOR_CMAP;
typedef struct _CM_DUMMY_ _CM_DUMMY_;

typedef union {
#pragma pack(1)

  RGB_MASK *Mask;
  COLOR_INDEXER *Indexer;
  COLOR_DITHERER *Ditherer;
  COLOR_CMAP *Matcher;
  _CM_DUMMY_ *Dummy;

} SKL_CMAPPER;

struct _CM_DUMMY_ { // Common fields... Beware of casts and union access...
#pragma pack(1)

   SKL_CMAPPER_TYPE Type;
   void (*Col_Convert)( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
   UINT Stamp;

};

struct RGB_MASK {
#pragma pack(1)

   SKL_CMAPPER_TYPE Type;
   void (*Col_Convert)( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
   UINT Stamp;
   RGB_MASK *Next;
   UINT Masks[ 256*4 ];
   FORMAT In, Out;
   UINT Users;

};

struct COLOR_INDEXER {
#pragma pack(1)

   SKL_CMAPPER_TYPE Type;
   void (*Col_Convert)( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
   UINT Stamp;
   COLOR_ENTRY Orig_Cols[ 256 ];
   COLOR_ENTRY Fmt_Cols[ 256 ];
   FORMAT Out;

};

struct COLOR_DITHERER {
#pragma pack(1)

   SKL_CMAPPER_TYPE Type;
   void (*Col_Convert)( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
   UINT Stamp;
   COLOR_ENTRY Match[ 256 ];  /* <= must be same offset than Cols in COLOR_CMAP */
   COLOR_ENTRY Cols[ 256 ];
   UINT *Dst_Stamp;

};

struct COLOR_CMAP {
#pragma pack(1)

   SKL_CMAPPER_TYPE Type;
   void (*Col_Convert)( SKL_CMAPPER *, PIXEL *, PIXEL *, INT );
   UINT Stamp;
   COLOR_ENTRY Cols[ 256 ];
   UINT *Dst_Stamp;

};

/********************************************************/

extern FORMAT Compute_Format_Depth( FORMAT Format );

extern UINT Packed_Field_Pos( FORMAT Format );
extern void Format_Mask_And_Shift( FORMAT Format, UINT Mask[], UINT Shift[] );
extern RGB_MASK *New_Masks( FORMAT In, FORMAT Out );
extern void Destroy_Mask( RGB_MASK *);
extern void Dispose_Mask( RGB_MASK *Mask );

extern MEM_ZONE *Drv_Install_Converter( MEM_ZONE *M );

extern MEM_ZONE *Install_Index_To_Any( MEM_ZONE *M, FORMAT Dst, UINT * );
extern MEM_ZONE *Install_RGB_To_Any( MEM_ZONE *M, FORMAT Fmt_Dst, UINT * );
extern void Clear_CMapper( MEM_ZONE *M );

/********************************************************/

#endif // _MASKS_H_


