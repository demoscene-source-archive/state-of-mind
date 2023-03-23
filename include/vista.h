/***********************************************
 *              Main 3D header                 *
 *                                             *
 * Skal 97                                     *
 ***********************************************/

#ifndef _VISTA_H_
#define _VISTA_H_

/*********************************************************************/

#define VISTA_UNKNOWN 0x00
#define VISTA_INIT    0x01
#define VISTA_DELTA   0x02
typedef struct VISTA_CHUNK VISTA_CHUNK;

struct VISTA_CHUNK
{
#pragma pack(1)
   FLT Time;
   INT Type;   // DELTA or INIT
   FLT Z_Min, Z_Max;
   INT Nb_Polys;
   VISTA_CHUNK *Next;
   USHORT *Obj_ID;
   USHORT *Poly_ID;
};

typedef struct 
{
#pragma pack(1)
   INT Max_Polys, Cur_Nb;      // max visible polys
   INT Nb_Chunks;
   VISTA_CHUNK *Chunks;
   FLT Cur_Time;
   VISTA_CHUNK *Cur_Chunk;
   USHORT *Poly_ID;
   USHORT *Obj_ID;
   OBJ_NODE *Camera;
   FLT Lens, Clip;
} VISTA;


   // in vista.c
extern VISTA_CHUNK *New_Vista_Chunk( INT Nb_Polys );
extern void Destroy_Vista_Chunk( VISTA_CHUNK *V );
extern void Destroy_Vista( VISTA *V );
extern VISTA *Load_Vista( STRING In_Name, WORLD *W );
#ifdef UNIX
extern void Swap_Vista_Chunk( VISTA_CHUNK *Cur );
#else
#define Swap_Vista_Chunk(c) { }
#endif

extern void Sort_Vista_Polys( OBJ_NODE *Root );  // ==Sort_Objects()
extern void Render_Vista( OBJ_NODE *Root );  // replaces Render_Camera()

extern void Select_Vista( VISTA *Vista );
extern VISTA *Select_Vista_World( WORLD *W, OBJ_NODE *Cam_Node );

   // in vista2.c
extern void Init_Vista_Builder( WORLD *W, OBJ_NODE *Cam_Node );
extern void Vista_Analysis( WORLD *W );
extern void Vista_Emit_Data( FLT Time, INT Nb_Thresh );
extern void Vista_Start_New_Bunch( );
extern void Fill_Vista_Chunk( VISTA_CHUNK *Cur, UINT *Accum, INT Nb );
extern void Vista_New_Bunch( WORLD *W );
extern void Dump_Vista( STRING Out_Name );


/*********************************************************************/

   // small type for I/O

typedef struct
{
#pragma pack(1)
   FLT Time;
   INT Type;   // DELTA or INIT
   FLT Z_Min, Z_Max;
   INT Nb_Polys;
}  VISTA_CHUNK_IO;

typedef struct 
{
#pragma pack(1)
   INT Max_Polys;      // max visible polys
   USHORT Camera_ID;
   USHORT Dummy;
   FLT Lens, Clip;
   INT Nb_Chunks;
} VISTA_IO;

/*********************************************************************/

#endif   // _VISTA_H_
