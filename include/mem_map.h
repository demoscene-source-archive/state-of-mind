/***********************************************
 *              Mem transfert                  *
 * Skal 97                                     *
 ***********************************************/

#ifndef _MEM_MAP_H_
#define _MEM_MAP_H_

typedef enum  {
   ZONE_NO_CLIP  = 0x00,
   ZONE_CLIP_X   = 0x01,
   ZONE_CLIP_Y   = 0x02,
   ZONE_CLIP_XY  = 0x03,
   ZONE_CLIP_OUT = 0x04
} ZONE_CLIP_TYPE;

typedef enum { 
   ZONE_TYPE_RAW     = 0x00,
   ZONE_TYPE_INDEXED = 0x01,
   ZONE_TYPE_RGB     = 0x02,

   MEM_IS_DISPLAY      = 0x10,

   MEM_OWNS_CMAPPER    = 0x20,
   ZONE_TYPE_STAMP     = 0xdead

} ZONE_TYPE;

typedef void *MEM_IMAGE;    /* Public access */

typedef struct MEM_IMG MEM_IMG;
typedef struct MEM_ZONE MEM_ZONE;
typedef struct MEM_ZONE_METHODS MEM_ZONE_METHODS;

/********************************************************/

typedef enum { 
   FMT_NONE = 0x0000,
   FMT_CMAP = 0x1000,
   FMT_233  = 0x1233,
   FMT_332  = 0x1332,
   FMT_444  = 0x2444,
   FMT_555  = 0x2555,
   FMT_565  = 0x2565,
   FMT_24b  = 0x3888,
   FMT_32b  = 0x4888
} FORMAT;

#define F_POS_MASK        0xCFFF0000
#define FMT_REVERSED_BIT  0x40000000
#define FMT_SPECIAL_BIT   0x80000000
#define Format_Depth(F)    ( ((F)>>12)&0x0F )
#define Format_F_Pos(F)    ( ( (F)&F_POS_MASK )>>16 )
#define Format_Red_Bits(F)     ( ((F)>>8)&0x0F )
#define Format_Green_Bits(F)   ( ((F)>>4)&0x0F )
#define Format_Blue_Bits(F)    ( (F)&0x0F )

/********************************************************/

#include "masks.h"

typedef struct {
#pragma pack(1)

   INT X, Y, Width, Height;

} BACKUP_HEADER;

struct MEM_ZONE_METHODS
{
#pragma pack(1)

   INT (*Propagate)( MEM_ZONE *, INT, INT, INT, INT );
   INT (*Flush)( MEM_ZONE * );
   void (*Destroy)( MEM_ZONE * );
   PIXEL *(*Get_Scanline)( MEM_ZONE *, INT );
   ZONE_CLIP_TYPE (*Set_Position)( MEM_ZONE *, INT, INT, INT );

};

struct MEM_IMG
{
#pragma pack(1)

   INT Flags;
   INT Width, Height;
   INT Pad;
   INT BpS, Size;
   INT Quantum;
   FORMAT Format;
   PIXEL *Base_Ptr;

};

#define MEM_ZONE_FIELDS          \
   ZONE_TYPE Type;               \
   INT Flags;                    \
   MEM_ZONE_METHODS *Methods;    \
   BACKUP_HEADER *Backup;        \
   MEM_ZONE *Dst;                \
   INT Xo, Yo;                   \
   SKL_CMAPPER CMapper;          \
   MEM_IMG Zone;

struct MEM_ZONE
{
#pragma pack(1)

   MEM_ZONE_FIELDS

};

/********************************************************/

#define New_MEM_Zone(M,m) (M *)_New_MEM_Zone( sizeof( M ), (m) )
extern MEM_ZONE *_New_MEM_Zone( INT Size, MEM_ZONE_METHODS *Methods );

#define Extract_Zone(m,t,w,h,x,y,M) Extract_Converted_Zone(m,t,w,h,x,y,0xFFFFFFFF,M)
extern MEM_ZONE *Extract_Converted_Zone(
   MEM_ZONE *New,
   MEM_ZONE *Target, INT Width, INT Height, INT X, INT Y, FORMAT Format,
   MEM_ZONE_METHODS *Original_Methods );

extern MEM_ZONE *Extract_Virtual_Zone( 
   MEM_ZONE *New,
   MEM_ZONE *Target,
   INT Width, INT Height, INT X, INT Y );

extern MEM_ZONE_METHODS *Assign_Methods( MEM_ZONE *M );
extern MEM_IMG *Mem_Img_Set( MEM_IMG *Img, INT F, 
   INT W, INT H, INT P, 
   USHORT Q, FORMAT Fmt, PIXEL *Ptr );
extern void Format_To_Zone_Type( MEM_ZONE *M );

extern BACKUP_HEADER *MEM_Turn_Backup_On( MEM_ZONE *M );
extern void MEM_Turn_Backup_Off( MEM_ZONE *M );

extern void MEM_Restore_Backup( MEM_ZONE *M );
extern void MEM_Store_Backup( MEM_ZONE *M );

extern void Clean_Up_Zone( MEM_ZONE *M );

extern MEM_ZONE_METHODS Default_Mem_Zone_Methods;
#define DEFAULT_METHODS (&Default_Mem_Zone_Methods)

extern void MEM_Copy_Rect( MEM_ZONE *, PIXEL *, PIXEL *, INT, INT, INT );

/********************************************************/

   /* Some flags... */

#define MEM_NO_FLAG           0x00
#define MEM_DONT_PROPAGATE    0x01
#define MEM_OWNS_METHODS      0x02

#define IMG_NO_FLAG         0x00
#define IMG_WRITABLE_PAD    0x01
#define IMG_OWNS_PTR        0x02
#define IMG_VIRTUAL         0x04
#define IMG_DISABLED        0x08
#define IMG_DIRECT          0x10

#define Img_Is_Disabled(M)    ( MEM_Flags(M) & IMG_DISABLED )
#define Img_Disable_Zone(M)     MEM_Flags(M) |=  IMG_DISABLED
#define Img_Enable_Zone(M)      MEM_Flags(M) &= ~IMG_DISABLED
#define Img_Owns_Bitmap(M)    ( MEM_Flags(M) & IMG_OWNS_PTR )
#define Img_Has_Bitmap(M)     ( MEM_Base_Ptr(M) != NULL )


extern ZONE_CLIP_TYPE Clip_Zones( MEM_IMG *Dst, INT *Width, INT *Height, INT *xo, INT *yo );

#define MEM_IMG(M)       (&(M)->Zone)
#define MEM_Xo(M)       ((M)->Xo)
#define MEM_Yo(M)       ((M)->Yo)
#define MEM_Backup(M)       ((M)->Backup)

#define IMG_Flags(R)     ((R)->Flags)
#define IMG_Width(R)     ((R)->Width)
#define IMG_Height(R)    ((R)->Height)
#define IMG_Pad(R)       ((R)->Pad)
#define IMG_Quantum(R)   ((R)->Quantum)
#define IMG_Format(R)    ((R)->Format)
#define IMG_BpS(R)       ((R)->BpS)
#define IMG_Size(R)      ((R)->Size)
#define IMG_Base_Ptr(R)  ((R)->Base_Ptr)
#define IMG_Mask(R)      ((R)->Mask)

#define MEM_Flags(M)     (MEM_IMG((M))->Flags)
#define MEM_Width(M)     (MEM_IMG((M))->Width)
#define MEM_Height(M)    (MEM_IMG((M))->Height)
#define MEM_Pad(M)       (MEM_IMG((M))->Pad)
#define MEM_Quantum(M)   (MEM_IMG((M))->Quantum)
#define MEM_Format(M)    (MEM_IMG((M))->Format)
#define MEM_BpS(M)       (MEM_IMG((M))->BpS)
#define MEM_Size(M)      (MEM_IMG((M))->Size)
#define MEM_Base_Ptr(M)  (MEM_IMG((M))->Base_Ptr)

#define ZONE_DESTROY(M)     ( (*(M)->Methods->Destroy)( (MEM_ZONE*)(M) ) )
#define ZONE_SCANLINE(M,Y)  ( (*(M)->Methods->Get_Scanline)( (MEM_ZONE*)(M),(Y) ) )
#define ZONE_FLUSH(M)       ( (*(M)->Methods->Flush)( (MEM_ZONE*)(M) ) )
#define ZONE_FLUSH_SAFE(M)  ( (*(M)->Methods->Propagate)( (MEM_ZONE*)(M) , 0, 0, MEM_Width(M), MEM_Height(M) ) )
#define ZONE_SET_POSITION(M,X,Y,B) ( (*(M)->Methods->Set_Position)( (MEM_ZONE*)(M), (X), (Y), (B) ) )
#define ZONE_PROPAGATE(M,X,Y,W,H) ( (*(M)->Methods->Propagate)( (MEM_ZONE*)(M), (X),(Y),(W),(H) ) )

/********************************************************/

#endif   // _MEM_MAP_H_


