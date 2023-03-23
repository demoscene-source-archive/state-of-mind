/***********************************************
 *              BITMAP utilities               *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "btm.h"
// #include "ld_any.h"

/*******************************************************/

EXTERN void Destroy_Bitmap( BITMAP *Btm )
{
   if ( Btm==NULL ) return;
   M_Free( Btm->Pal );
   M_Free( Btm->Bits );
   M_Free( Btm );
}

EXTERN BITMAP *New_Bitmap( int Width, int Height, int Nb_Col )
{
   BITMAP *Btm;

   Btm = New_Object( 1, BITMAP );
   if ( Btm == NULL ) goto Failed;
   Btm->Bits = New_Object( Width*Height, PIXEL );
   if ( Btm==NULL ) goto Failed;
   if ( Nb_Col ) 
   {
      Btm->Pal = New_Object( 3*Nb_Col, PIXEL );
      if ( Btm->Pal==NULL ) goto Failed;
   }
   Btm->Nb_Col      = Nb_Col;
   Btm->Width       = Width;
   Btm->Height      = Height;
   Btm->Size        = Width*Height;

   return( Btm );
Failed:
   Destroy_Bitmap( Btm );
   return( NULL );
}

/*******************************************************/
