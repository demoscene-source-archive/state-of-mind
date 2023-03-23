/***********************************************
 *                mat.h                        *
 * Skal 97                                     *
 ***********************************************/

#ifndef _MAT_H_
#define _MAT_H_

#define MAX_MIP      8+1

// #define DEBUG_CACHE

/******************************************************************/

typedef enum {

   TXT_VOID, TXT_CMAP, TXT_RGB16, TXT_GRAY, 
   TXT_DX_DY_8

} MAP_TYPE;

struct TEXTURE_MAP {
#pragma pack(1)

   MAP_TYPE Type;
   SHORT Users;
   SHORT Quantum;
   PIXEL Slot;
   PIXEL Base;
   USHORT Offset;
   PIXEL *Ptr;       /* 64k aligned ? */

};

struct MATERIAL {
#pragma pack(1)

   OBJECT_FIELDS

   SHADER_METHODS *Shader;
   FLT C1, C2, C3;
   TEXTURE_MAP *Txt1, *Txt2, *Txt3;
   INT Map_Nb1, Map_Nb2, Map_Nb3;
   INT Mapped;
   FCOLOR Ambient, Diffuse, Specular;

};

struct TXT_CACHE {
#pragma pack(1)

   PIXEL *Texture_Maps_Real_Ptr;
   PIXEL *Texture_Maps_Ptr;
   INT Nb_Texture_Maps;
   INT Nb_64k_Bunch;
   TEXTURE_MAP *Maps;
   PIXEL *CMap;
   INT Nb_Col, Nb_Offset;

};

#define TEXTURE_MAP_SIZE     (256*256)
#define TEXTURE_MAP_SIZE_16b (256*256*sizeof(USHORT))

/******************************************************************/

extern NODE_METHODS _MATERIAL_;

extern MATERIAL *New_Material( );
extern INT Finish_Material_Data( OBJECT *Mat );
// extern MATERIAL *Search_Material_From_Name( STRING Name, DATA_ARRAY *List );

/******************************************************************/
/******************************************************************/

struct CACHE_METHODS {
#pragma pack(1)

   void   (*Clear_Cache)( void *Cache );
   void   (*Setup_Material)( void *Cache, OBJECT *Mat );
   INT    (*Retreive_Texture)( INT Mip_Slot, FLT R );
   INT    (*Compute_MipMap)( );
   INT    (*Compute_MipMap_Env)( );
   void   (*Init_From_Poly)( );
   void   (*Init_From_Poly_II)( );
   void   (*Init_From_Poly_III)( );
   USHORT (*Poly_Flat_Color)( );

};

extern CACHE_METHODS Cache_Methods_I;
extern CACHE_METHODS Cache_Methods_II;
extern CACHE_METHODS Cache_Methods_III;
extern CACHE_METHODS Cache_Methods;

/******************************************************************/
/******************************************************************/

extern FLT Mip_Scale;
extern INT Cur_Mip;
extern INT Cur_Mip_Slot;
extern FLT Uo_Mip, Vo_Mip;
extern UINT Mip_Masks[9];
extern UINT Mip_Mask;

extern INT Mip_Slot0_Offsets[3];

/******************************************************************
 *                     CACHE public entries                       *
 ******************************************************************/

   // in sv_mat.c and ld_mat.c

#define TXT_MAGIK "txtch"
#define TXT_MAGIK2 "txtch2"

// extern void Do_One_Mip( USHORT *Src, INT Width );

// extern INT Find_Upper_Poly_Vertex( POLY *P, VECTOR *Vtx );
// extern INT Mip_Poly_Order[5];

extern void *Load_Txt_Cache_I( STRING Name );
extern INT Save_Txt_Cache_I( STRING Name, void *Cache );
extern void *Allocate_Texture_Maps_I( void *Cache, INT Nb );

extern void *Load_Txt_Cache_II( STRING Name );
extern void *Load_Txt_Cache_IIbis( STRING Name, STRING Name2 );
extern INT Save_Txt_Cache_II( STRING Name, void *Cache );
extern INT Save_Txt_Cache_IIbis( STRING Name, void *Cache );
extern void *Allocate_Texture_Maps_II( void *Cache, INT Nb );

// extern TEXTURE_MAP *Install_Texture_Maps_I( TXT_CACHE *Cache, PIXEL *Ptr, MAP_TYPE Type1, MAP_TYPE Type2, MAP_TYPE Type3 );

extern void Clear_Cache( void *C );

/******************************************************************/
/******************************************************************/

#define TXT_MAGIK3 "txtch3"
#define TXT_MAGIK4 "txtch4"

#define MIP_MAX   8

typedef struct MIP_BLOCK MIP_BLOCK;
typedef struct MIP_ROW MIP_ROW;

struct MIP_ROW {
#pragma pack(1)

   MIP_ROW *Next;
   USHORT *Ptr;
   MIP_BLOCK *Slots;
   INT Nb_Entries;
   INT Max_Entries;

};

struct MIP_BLOCK { 
#pragma pack(1)

   MIP_BLOCK *Next; 
   BYTE Mip_Level;
   BYTE Installed;   
   USHORT UV;
   POLY *Up, *Down;
   USHORT *Bits;
   MIP_ROW *Mip_Row;
   INT Row_Entry;

};

typedef struct {
#pragma pack(1)

   MIP_BLOCK *Mips[MIP_MAX];
   INT Max_Mip;

} POLY_MIPS;

typedef struct {
#pragma pack(1)

   INT Nb_Mips;
   MIP_BLOCK *Mips[MIP_MAX];

   INT Nb_PMips;
   INT Cur_PMips;
   POLY_MIPS *PMips;

   MIP_ROW *Mip_Rows[MIP_MAX];
   INT Nb_Rows, Size;

} TXT_CACHE_III;


extern void *Load_Txt_Cache_III( STRING Name, INT Mip_Min );
extern INT Check_Mips_III( void *Cache );
extern INT Save_Txt_Cache_III( STRING Name, void *Cache );
extern void Destroy_Txt_Cache_III( void *C );

#define MIP_INF   4        // below this level, compress as usual...
#define MIP_WIDTH_IV  512  // *Must* be a power of 2!!

extern INT Save_Txt_Cache_IV( STRING Name, void *C );
extern void *Load_Txt_Cache_IV( STRING Name, STRING Name2, INT Mip_Min );

extern MIP_BLOCK *Search_Free_Mip_Block( MIP_BLOCK *Blk, 
   TXT_CACHE_III *C, USHORT *Maps );
extern void Remove_Mip_Block( MIP_BLOCK *Blk );

extern void Destroy_Mip_Pages( TXT_CACHE_III *C );

/******************************************************************/

#endif   // _MAT_H_
