/***********************************************
 *              JPEG marker codes              *
 * Skal 98                                     *
 ***********************************************/

#ifndef _J_MARKERS_H_
#define _J_MARKERS_H_

/*******************************************************/

typedef enum {

  M_SOF0  = 0xc0,
  M_SOF1  = 0xc1,
  M_SOF2  = 0xc2,
  M_SOF3  = 0xc3,
  
  M_SOF5  = 0xc5,
  M_SOF6  = 0xc6,
  M_SOF7  = 0xc7,
  
  M_JPG   = 0xc8,
  M_SOF9  = 0xc9,
  M_SOF10 = 0xca,
  M_SOF11 = 0xcb,
  
  M_SOF13 = 0xcd,
  M_SOF14 = 0xce,
  M_SOF15 = 0xcf,
  
  M_DHT   = 0xc4,
  
  M_DAC   = 0xcc,
  
  M_RST0  = 0xd0,
  M_RST1  = 0xd1,
  M_RST2  = 0xd2,
  M_RST3  = 0xd3,
  M_RST4  = 0xd4,
  M_RST5  = 0xd5,
  M_RST6  = 0xd6,
  M_RST7  = 0xd7,
  
  M_SOI   = 0xd8,
  M_EOI   = 0xd9,
  M_SOS   = 0xda,
  M_DQT   = 0xdb,
  M_DNL   = 0xdc,
  M_DRI   = 0xdd,
  M_DHP   = 0xde,
  M_EXP   = 0xdf,
  
  M_APP0  = 0xe0,
  M_APP1  = 0xe1,
  M_APP2  = 0xe2,
  M_APP3  = 0xe3,
  M_APP4  = 0xe4,
  M_APP5  = 0xe5,
  M_APP6  = 0xe6,
  M_APP7  = 0xe7,
  M_APP8  = 0xe8,
  M_APP9  = 0xe9,
  M_APP10 = 0xea,
  M_APP11 = 0xeb,
  M_APP12 = 0xec,
  M_APP13 = 0xed,
  M_APP14 = 0xee,
  M_APP15 = 0xef,
  
  M_JPG0  = 0xf0,
  M_JPG13 = 0xfd,
  M_COM   = 0xfe,
  
  M_TEM   = 0x01,
  
  M_ERROR = 0x0100,
  M_VOID  = 0x0000

} JPG_MARKER;

#define JPG_MARKER_MARKER 0xff
#define JPG_MARKER_FIL 0xff

#define JPG_MARKER_SOI 0xd8
#define JPG_MARKER_EOI 0xd9
#define JPG_MARKER_SOS 0xda
#define JPG_MARKER_DQT 0xdb
#define JPG_MARKER_DNL 0xdc
#define JPG_MARKER_DRI 0xdd
#define JPG_MARKER_DHP 0xde
#define JPG_MARKER_EXP 0xdf

#define JPG_MARKER_DHT 0xc4

#define JPG_MARKER_SOF 0xc0
#define JPG_MARKER_RSC 0xd0
#define JPG_MARKER_APP 0xe0
#define JPG_MARKER_JPG 0xf0

#define JPG_MARKER_RSC_MASK 0xf8

/*******************************************************/

#endif   // _J_MARKERS_H_
