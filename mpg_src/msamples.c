/************************************************
 *        MP3-decoded Samples cache             *
 * Skal 98                                      *
 ************************************************/

#define _USE_PRIVATE_

#include "main.h"

#ifdef MP3_USE_SAMPLES

#include "bit_strm.h"
#include "./mp3.h"

/*******************************************************/

static INT Busy = FALSE;
static MP3_STRM Strm = { 0 };
static INT Cur_Sample = -1, Cur_Pos = 0;
static INT List_Len = 0;
static MP3_SAMPLE *MP3_Samples;
static SHORT *List = NULL;

EXTERN SHORT *Sample_Data[ MP3_MAX_SAMPLES ];
EXTERN INT Sample_Len[ MP3_MAX_SAMPLES ];
static INT Sample_Done[ MP3_MAX_SAMPLES ];
static SHORT Sample_Nb[ MP3_MAX_SAMPLES ];
static SHORT Sample_Users[ MP3_MAX_SAMPLES ];
static INT Max_Sample, Nb_Samples, Cur_Slot;

#define NB_SAMPLES_AHEAD   4
#define NB_FRAME_EACH_TIME 4

// #define BLAH_BLAH

/*******************************************************/

EXTERN INT Init_MP3_Cache( SHORT *MP3_List, MP3_SAMPLE *Samples, BYTE *The_MP3 )
{
   INT i;
   Busy = FALSE;

   if ( Samples==NULL || MP3_List==NULL || The_MP3==NULL ) return( 0 );

   Mem_Clear( &Strm );
   Strm.Bit_Strm.Mthds = BSTRM_Dflt_Methods;
   Strm.Cur_Frame = 0;
   Strm.Bit_Strm.Raw =  Strm.Bit_Strm.Raw_Ptr = The_MP3;
   Strm.Bit_Strm.File_Len = 0;
   Strm.Bit_Strm.Bit_Pos = 0;
   Strm.Bit_Strm.Available_Bytes = 0;

   MP3_Samples = Samples;
   Nb_Samples=0;
   while( Samples[Nb_Samples].Len!= 0 )
   {
      Strm.Bit_Strm.File_Len += Samples[Nb_Samples].Len;
      Nb_Samples++;
   }

   List_Len=0;
   List = MP3_List;
   while( List[List_Len]!=-1 )
   {
      if ( List[List_Len]>=Nb_Samples ) return( -1 );    // this is an error
      List_Len++;
   }
   Cur_Sample = -1;
   Cur_Pos = 0;

   for( i=0; i<MP3_MAX_SAMPLES; ++i ) 
   {
      Sample_Data[i] = NULL; 
      Sample_Nb[i] = 0;
      Sample_Len[i] = 0;
      Sample_Done[i] = 0;
      Sample_Users[i] = 0;
   }
//   if ( Nb_Samples<MP3_MAX_SAMPLES ) Max_Sample = Nb_Samples;
//   else 
   Max_Sample = MP3_MAX_SAMPLES;

   return( List_Len );
}

/*******************************************************/

static void Select_Job( INT n, INT Slot )
{
   BSTRM_Cur = &Strm.Bit_Strm;
   Strm.Bit_Strm.Raw_Ptr = BSTRM_Ptr = 
      Strm.Bit_Strm.Raw + MP3_Samples[n].Frame_Offset;
   Strm.Bit_Strm.Bit_Pos = BSTRM_Bit_Pos = 0;
   Strm.Bit_Strm.Available_Bytes = MP3_Samples[n].Len;
   Strm.Bit_Strm.Available_Bits = MP3_Samples[n].Len * 8;

   Cur_Slot = Slot;
   MP3_Samples[n].Slot = (SHORT)Slot;
   if ( Sample_Data[Slot]!=NULL )
      if ( Sample_Len[Slot]>MP3_Samples[n].Final_Len )
         goto Ok;
#ifdef BLAH_BLAH
   Out_Message( "  -[discarding %d shorts in slot #%d. Reallocating %d shorts]-", 
      Sample_Len[Slot], Slot, MP3_Samples[n].Final_Len+1152 );
#endif
   M_Free( Sample_Data[Slot] );
   Sample_Data[Slot] = New_Fatal_Object( MP3_Samples[n].Final_Len+1152, SHORT );
   Sample_Len[Slot] = MP3_Samples[n].Final_Len;

Ok:
   Sample_Nb[Slot] = (SHORT)n;
   Cur_Sample = n;
   Sample_Done[Slot] = 0;
   Sample_Users[Slot] = 0;

   Busy = TRUE;
#ifdef BLAH_BLAH
   Out_Message( " -o- starting decoding of sample #%d (slot:%d). Len=%d shorts",
      n, Slot, Sample_Len[Slot] );
#endif
}

static INT Advance_Job( INT Nb )
{
   INT Len;
   while( Nb-- )
   {
      MP3_Read_Header( &Strm.Header );

      Len = 0;
      if ( Strm.Header.Layer==1 ) Len = MP3_Read_Frame_Layer3( &Strm );
      if ( Len==-1 ) goto Finished;

         // transfert new data
      memcpy( Sample_Data[Cur_Slot] + Sample_Done[Cur_Slot],
               MP3_Sample, Len*sizeof( SHORT ) );
      Sample_Done[Cur_Slot] += Len;
      if ( Sample_Done[Cur_Slot]>=MP3_Samples[Cur_Sample].Final_Len )
      {
Finished:
         Busy = FALSE;
         break;
      }
   }
   return( Sample_Done[Cur_Slot] );
}

/*******************************************************/

EXTERN INT Advance_MP3_Cache( INT List_Pos )
{
   INT i;

      // we're here to ensure that at least NB_SAMPLES_AHEAD
      // samples are decompressed and available
      // return 0 if so.
      // else return last decompressed len

   if ( Busy ) 
   {
      i = Advance_Job( NB_FRAME_EACH_TIME );
      if ( i != 0 ) return( i );
   }

      // search slot and find what job ought to be done
   
   i = 0;
   if ( List_Pos>=List_Len ) return( 0 );
   if ( List_Pos<0 ) List_Pos = 0;
   while( 1 )
   {
      INT What, Slot, Older, Min_Users;

      What = List[ List_Pos ];
      if ( MP3_Samples[What].Slot!=-1 )
      {
         Sample_Users[ MP3_Samples[What].Slot ]++;
         List_Pos++;
         if ( List_Pos==List_Len ) return( 0 ); // all ok
         i++;
         if ( i==NB_SAMPLES_AHEAD ) return( 0 ); // queue is ok
         continue;   // continue scrutinizing
      }

         // sample not available. Launch job.
      for( Slot=0; Slot<MP3_MAX_SAMPLES; ++Slot )
         if ( Sample_Data[Slot]==NULL ) goto Ok;

            // need to discard an old slot
      Older = 0; Min_Users = Sample_Users[Older];
      for( Slot=1; Slot<Max_Sample; ++Slot )
         if ( Sample_Users[Slot]<Min_Users )
         {
            Older = Slot; Min_Users = Sample_Users[Older];
         }
      Slot = Older;
Ok:
      Select_Job( What, Slot );
      return( 1 );
   }
   return( 0 );
}


/*******************************************************/

#endif   // MP3_USE_SAMPLES
