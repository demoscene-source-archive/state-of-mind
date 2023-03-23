/***********************************************
 *              mem utilities                  *
 * Skal 96                                     *
 ***********************************************/

#include "mem.h"

   // Flag for printing mem. infos

// #define DEBUG_MEM  2			// 2 = reduced verbose
// #define DEBUG_MEM  1             // full verbose

/*******************************************************************/
/*******************************************************************/

#ifdef DEBUG_MEM

static int Allocated = 0;
static int Max_Reached = 0;
static int Last_Slot = 0;

#define MAX_SLOT     30000
static struct {
   void *Ptr;
   int   Size;
} Mem_Slots[ MAX_SLOT ];

EXTERN void *My_Realloc( void *Ptr, int Size )
{
   void *New;
   New = My_Malloc( Size );
   if ( New == NULL ) return( NULL );
   if ( Ptr != NULL )
   {
      memcpy( New, Ptr, Size );
      My_Free( Ptr );
   }
   else bzero( New, Size );
   return( New );
};

EXTERN void *My_Malloc( int Size )
{
   void *Ptr;

   if ( Size == 0 ) return( NULL );
   Size = ( Size + 0x07 ) & ~0x07;    // align to 8
   Ptr = (void *)malloc( Size );
   if ( Ptr != NULL ) 
   {
      bzero( Ptr, Size );
      Allocated += Size;
      if ( Allocated > Max_Reached ) Max_Reached = Allocated;
      Mem_Slots[ Last_Slot ].Ptr = Ptr;
      Mem_Slots[ Last_Slot ].Size = Size;
      Last_Slot++;
      if ( Last_Slot>= MAX_SLOT )
         Exit_Upon_Error( "Not enough slots in My_Malloc() !\n" );
#if (DEBUG_MEM==2)
      Out_String( "+ %d bytes   \t\tTotal:%d  \r", Size, Allocated );
#else
      Out_Message( "Malloc'ed: %d bytes (0x%.8x). \t\tTotal: %d", Size, Ptr, Allocated );
#endif
   }
   return( Ptr );
}

EXTERN void My_Free( void *Ptr )
{
   int i;
   for( i=Last_Slot-1; i>=0; --i )
      if ( Mem_Slots[i].Ptr == Ptr ) break;
   if ( i<0 )
   {
      Out_Message( "Ptr 0x%.8x not found in Mem_Slots[] !", Ptr );
      free( Ptr );
      return;
   }
   Allocated -= Mem_Slots[i].Size;
#if (DEBUG_MEM==2)
      Out_String( "- %d bytes   \t\tTotal:%d   \r", Mem_Slots[i].Size, Allocated );
#else
   Out_Message( "Free'ed: %d bytes (0x%.8x). \t\tTotal: %d",
      Mem_Slots[i].Size, Ptr, Allocated );
#endif
   free( Ptr );

   Last_Slot--;
   Mem_Slots[i] = Mem_Slots[ Last_Slot ];
   Mem_Slots[ Last_Slot ].Ptr = NULL;
   Mem_Slots[ Last_Slot ].Size = 0;
}

EXTERN void Print_Mem_State( )
{
   INT i;
   Out_Message( "Memory state:" );
   Out_Message( " * Malloc'ed: %d bytes.", Allocated );
   Out_Message( " * Max. allocated: %d bytes.", Max_Reached );
   Out_Message( " * Current slots: %d.", Last_Slot );
   if ( Last_Slot>20 ) Last_Slot = 20;
   for( i=0; i<Last_Slot; ++i )
   {
      Out_Message( "Slot #%d: Ptr=0x%.8x  Size=%d",
         i, Mem_Slots[i].Ptr, Mem_Slots[i].Size );
   }
}

/*******************************************************************/

#else

EXTERN void *My_Malloc( int Size )
{
   void *Ptr;
   Size = ( Size + 0x07 ) & ~0x07;
   Ptr = (void *)calloc( 1, Size ); // important!
   if ( Ptr==NULL ) return( NULL );
   return( Ptr );
}

EXTERN void *My_Realloc( void *Ptr, int Size )
{
   if ( Size == 0 ) { M_Free( Ptr ); return( NULL ); }
   if ( Ptr==NULL ) return( (void *)My_Malloc( Size ) );
   else return( (void *)realloc( Ptr, Size ) );
}

EXTERN void My_Free( void *Ptr )
{
   free( Ptr );
}

EXTERN void Print_Mem_State( )
{
}

#endif // DEBUG_MEM

/*******************************************************************/

EXTERN void *My_Fatal_Malloc( int Size )
{
   void *Ptr;
   if ( !Size ) return( NULL );
   Ptr = (void *)My_Malloc( Size );
   if ( Ptr == NULL )
      Exit_Upon_Mem_Error( NULL, Size );
   return( Ptr );
}

/*******************************************************************/

   // useful func

EXTERN STRING Str_Dup( STRING In )
{
   char *Out;
   INT n;
   n = strlen( In ) + 1;
   Out = New_Fatal_Object( n, char );
   if ( Out==NULL ) return( NULL );
   memcpy( Out, In, n );
   return( (STRING)Out );
}

/*******************************************************************/

      // Useful

EXTERN PIXEL *Malloc_64( PIXEL **_Real_Bits, PIXEL **_Bits, INT Size )
{
   PIXEL *Bits, *New_Bits;
   ADDR Tmp;

   Bits = New_Bits = New_Object( Size, PIXEL );
   if ( New_Bits == NULL ) return( NULL );

   Tmp = (ADDR)( New_Bits ) & 0xFFFF;
   if ( Tmp==0 ) goto Ok;
   M_Free( New_Bits );
   if ( Tmp>=0xFF00 ) goto Safe;
   Tmp = 0x10000-Tmp-8;
   Bits = New_Object( Tmp, PIXEL );
   New_Bits = New_Object( Size, PIXEL );
   M_Free( Bits );
   if ( New_Bits == NULL ) return( NULL );
   Bits = New_Bits;
   if ( ( (ADDR)( New_Bits ) & 0xFFFF ) == 0 ) goto Ok;
   
   M_Free( New_Bits );

Safe:
   New_Bits = ( PIXEL *)My_Malloc( Size + 0xFFFF );
   if ( New_Bits == NULL ) return( NULL );
   Bits = ( PIXEL *)( ( (ADDR)New_Bits + 0xFFFF ) & ~0xFFFF );

Ok:
   *_Bits = Bits;
   *_Real_Bits = New_Bits;
   return( New_Bits );
}

/*******************************************************************/
