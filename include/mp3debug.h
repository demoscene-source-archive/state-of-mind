/************************************************
 *         MP3 debugging func                   *
 * Skal 98                                      *
 ************************************************/

#ifndef _MP3_DEBUG_H_
#define _MP3_DEBUG_H_

/******************************************************************/

#if (MP3_VERSION<=002)
extern void MP3_Print_Header( MP3_HEADER *H );
extern INT MP3_Header_Sane( MP3_HEADER *H );
#define MP3_Report_Hdr_Err(E)   { if ( (E)!=MP3_HDR_OK ) switch (E) { \
   case MP3_HDR_NSTD: Exit_Upon_Error( "non standard MPEG 2.5 format." ); break; \
   case MP3_HDR_LAYER1: Exit_Upon_Error( "Unsupported MPEG Layer1 format." ); break;   \
   case MP3_HDR_F_FRMT: Exit_Upon_Error( "Unsupported Free Format." ); break;    \
   default: Exit_Upon_Error( "Header error. Blah." ); break;                  \
   } }
#else
#define MP3_Header_Sane(H)    (TRUE)
#define MP3_Print_Header(H) 
#define MP3_Report_Hdr_Err(E)
#endif

#if (MP3_VERSION<=001)
extern void MP3_Queue_State( UINT Ret, INT How_Many );
#else
#define MP3_Queue_State(b,c)
#endif

/*******************************************************/
#if (BSTRM_VERSION<=001)       // debug
extern void Queue_State( STRING S, UINT Ret, INT How_Many );
#else
#define Queue_State(a,b,c)   
#endif
/*******************************************************/

#endif      // _MP3_DEBUG_H_
