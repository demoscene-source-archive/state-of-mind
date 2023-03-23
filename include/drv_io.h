/***********************************************
 *              I/O utilities                  *
 * Skal 96                                     *
 ***********************************************/

#ifndef _DRV_IO_H_
#define _DRV_IO_H_

#include "main.h"
#include <stdio.h>

/******************************************************************/

extern STRING Dat_File_Name;
extern FILE *Dat_File;
extern void Close_Dat_File( );
extern void USE_DAT_FILE( STRING Name, INT Write );

extern FILE *F_OPEN( STRING Name, STRING Switch );
extern void F_CLOSE( FILE *f );
extern long F_TELL( FILE * );
extern long F_SIZE( FILE * );
extern long F_SEEK( FILE *, long, int );
#define F_WRITE(a,b,c,d)   fwrite( (a),(b),(c),(d) )
#define F_READ(ptr,stream,size)  ( fread( (ptr), (size), 1, (stream) ) )

extern FILE *Access_File( STRING, STRING );

extern INT Get_Me_A_Number( FILE *In );

/******************************************************************/

#if defined( LNX ) || defined( UNIX )
#ifndef READ_SWITCH
#define READ_SWITCH        "r"
#endif   // READ_SWITCH
#ifndef WRITE_SWITCH
#define WRITE_SWITCH       "w"
#endif   // WRITE_SWITCH
#endif

#if defined(__DJGPP__) || defined(__WATCOMC__) || defined( WIN32 )
#ifndef READ_SWITCH
#define READ_SWITCH        "rb"
#endif
#ifndef WRITE_SWITCH
#define WRITE_SWITCH       "wb"
#endif
#endif

   // raw block reading

extern INT Save_Block( BYTE *Where, STRING Name, INT Size );
extern INT Save_Raw_Block( BYTE *Where, STRING Name, INT Size );
extern void *Load_Block( STRING Name );

   // Functions in compress.c

extern int Compress_Read( BYTE *Bits, FILE *Out, INT Size );
#define F_COMPRESS_READ(ptr,stream,size)  ( Compress_Read( (ptr), (stream), (size) ) )

extern INT Save_LZW_Block( BYTE *Where, STRING Name, INT Size );
extern INT Save_LZW_Block_II( BYTE *Where, FILE *Out, INT Size, INT Bit_Size );

extern void *Load_Raw_Block( STRING Name, INT * );
extern void *Load_LZW_Data( STRING Name );
extern void *Load_LZW_Block( STRING Name );
extern void *Load_LZW_Block_II( FILE *In );
extern INT Compress_Write( BYTE *Bits, FILE *Out, INT Size, INT Bit_Size );
extern INT Add_Raw_Block( FILE *Out, BYTE *Bits, INT Size );
extern INT Add_LZW_Block( FILE *Out, BYTE *Bits, INT Size, INT Bit_Size );

/******************************************************************/

#endif   // _DRV_IO_H_

