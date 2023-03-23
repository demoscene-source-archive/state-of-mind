/***********************************************
 *          Graphics Drivers                   *
 * Skal 96                                     *
 ***********************************************/

#include "main.h"
#include "mem_map.h"
#include "driver.h"
#include "drv_col.h"

/********************************************************/

#ifndef SKL_LIGHT_CODE

EXTERN STRING _Drv_Err_ = NULL, _Drv_Err_2_ = NULL;

EXTERN void Driver_Print_Error( )
{
   if ( _Drv_Err_ == NULL ) return;
   if ( _Drv_Err_2_ == NULL ) Out_Error( _Drv_Err_ );
   else Out_Error( _Drv_Err_, _Drv_Err_2_ );
}

#endif

/********************************************************/

static _G_DRIVER_ *_Reg_Drivers_[ MAX_VIDEO_SUPPORT ] = { NULL };

EXTERN void Register_Video_Support( INT Nb, ... )
{
   int i;
   va_list Arg_List;
   _G_DRIVER_ *Drv;

   SET_DRV_ERR( NULL );

   va_start( Arg_List, Nb );
   for( ; Nb>0; --Nb )
   {
      Drv = va_arg( Arg_List, _G_DRIVER_ * );
      if ( Drv==NULL ) continue;

      for( i=0; i<MAX_VIDEO_SUPPORT; ++i )
         if ( _Reg_Drivers_[i] == NULL )
         {
            _Reg_Drivers_[i] = Drv;
            break;
         }

      if ( i==MAX_VIDEO_SUPPORT )
      {
         SET_DRV_ERR( "No more driver slots." );
         Driver_Print_Error( );
         exit( 1 );
      }
   }
   va_end( Arg_List );

   return;
}

/********************************************************/

EXTERN void Driver_Close( MEM_IMAGE M )
{
   MEM_ZONE_DRIVER *Drv;

   if ( M==NULL ) return;   

   Drv = (MEM_ZONE_DRIVER *)M;

   if ( !(Drv->Type & MEM_IS_DISPLAY ) ) goto End;
   Drv = (MEM_ZONE_DRIVER *)Driver_Open_Mode( M, 0 );  // cleanup

   M_Free( Drv->Req_Modes );

End:
   if ( Drv->Flags & MEM_OWNS_METHODS ) M_Free( Drv->Methods );
   M_Free( Drv );

   Print_Mem_State( );
   return;
}

#if defined(UNIX) || defined(LNX)

static MEM_IMAGE Last_Opened = NULL;
static void User_Abort( int Sig )
{
   Driver_Close( Last_Opened );
   Last_Opened = NULL;
   Out_Message( "Exiting upon signal %s", Sig==SIGINT ? "SIGINT" : "SIGKILL" );
   exit( 0 );
}

#endif

EXTERN MEM_IMAGE Driver_Open_Mode( MEM_IMAGE M, INT Nb )
{
   MEM_ZONE_DRIVER *Drv;

   Drv = (MEM_ZONE_DRIVER *)M;
   if ( !( Drv->Type & MEM_IS_DISPLAY ) ) return( NULL );

   if ( Nb!=0 && Drv->Cur_Req_Mode == Nb ) return( M );  // already opened...

      // Clean all

   if ( Drv->Cleanup_Mode != NULL )
   {
      (*Drv->Cleanup_Mode)( (MEM_ZONE *)Drv );
      Drv->Cleanup_Mode = NULL;
   }

   M = (*Drv->Driver->Open)( Drv, Nb );
   if ( M==NULL ) return( NULL );

   if ( !Drv->Cur_Req_Mode ) // First call. We're coming from text mode
   {
      if ( Drv->CMapper.Dummy != NULL &&
           Drv->CMapper.Dummy->Type != RGB_MASK_TYPE &&
           Drv->CMapper.Dummy->Type != DITHERER_TYPE )
         {
            COLOR_ENTRY Col_Tab[256];
            Drv_Build_RGB_Cube( Col_Tab, (FORMAT)0x332 );  // Default CMap
            Driver_Change_CMap( Drv, 256, Col_Tab );
         }
   }
      
   Drv->Cur_Req_Mode = Nb;

#if defined(UNIX) || defined(LNX)
   if ( !Nb )
   {
      Last_Opened = NULL;
      signal( SIGINT, SIG_DFL );
      signal( SIGKILL, SIG_DFL );
   }
   else if ( Last_Opened==NULL )
   {
      Last_Opened = M;
      signal( SIGINT, User_Abort );
      signal( SIGKILL, User_Abort );
   }
#endif

   return( M );
}

EXTERN EVENT_TYPE Driver_Get_Event( MEM_IMAGE M )
{
   MEM_ZONE_DRIVER *Drv;

   Drv = (MEM_ZONE_DRIVER *)M;
   if ( Drv == NULL || !(Drv->Type & MEM_IS_DISPLAY) ) return( DRV_NULL_EVENT );
   Drv->Last_Event = Drv->Event;
   return( (*Drv->Driver->Get_Event)( Drv ) );
}

EXTERN INT Driver_Nb_Req_Modes( MEM_IMAGE M )
{
   MEM_ZONE_DRIVER *Drv;

   Drv = (MEM_ZONE_DRIVER *)M;
   if ( M==NULL || !(Drv->Type & MEM_IS_DISPLAY )) return( -1 );
   return( Drv->Nb_Req_Modes+1 );
}

EXTERN INT Driver_Cur_Req_Mode( MEM_IMAGE M )
{
   MEM_ZONE_DRIVER *Drv;

   Drv = (MEM_ZONE_DRIVER *)M;
   if ( M==NULL || !(Drv->Type & MEM_IS_DISPLAY )) return( -1 );
   return( Drv->Cur_Req_Mode );
}

/*******************************************************************/

   // Public fields

EXTERN INT Zone_Width( MEM_IMAGE M )
{
   return( MEM_Width( (MEM_ZONE *)M ) );
}

EXTERN INT Zone_Height( MEM_IMAGE M )
{
   return( MEM_Height( (MEM_ZONE *)M ) );
}

EXTERN INT Zone_BpS( MEM_IMAGE M )
{
   return( MEM_BpS( (MEM_ZONE *)M ) );
}

EXTERN INT Zone_Pad( MEM_IMAGE M )
{
   return( MEM_Pad( (MEM_ZONE *)M ) );
}

EXTERN PIXEL *Zone_Base_Ptr( MEM_IMAGE M )
{
   return( MEM_Base_Ptr( (MEM_ZONE *)M ) );
}

EXTERN INT Zone_Quantum( MEM_IMAGE M )
{
   return( MEM_Quantum( (MEM_ZONE *)M ) );
}

EXTERN FORMAT Zone_Format( MEM_IMAGE M )
{
   return( MEM_Format( (MEM_ZONE *)M ) );
}

EXTERN INT Zone_Size( MEM_IMAGE M )
{
   return( MEM_Size( (MEM_ZONE *)M ) );
}

EXTERN MEM_IMAGE Zone_Dst( MEM_IMAGE M )
{
   return( (MEM_IMAGE)( (MEM_ZONE *)M )->Dst );
}

EXTERN INT Zone_Propagate( MEM_IMAGE M, INT X, INT Y, INT W, INT H )
{
   return( (*( (MEM_ZONE *)M)->Methods->Propagate)( (MEM_ZONE*)M, X, Y, W, H ) );
}

EXTERN INT Zone_Flush( MEM_IMAGE M )
{
   return( (*( (MEM_ZONE *)M)->Methods->Flush)( (MEM_ZONE *)M ) );
}

EXTERN INT Zone_Flush_Safe( MEM_IMAGE M )
{
   return( (*( (MEM_ZONE *)M)->Methods->Propagate)(
      (MEM_ZONE *)M, 0, 0,
      MEM_Width( (MEM_ZONE *)M ), MEM_Height( (MEM_ZONE *)M ) ) );
}

EXTERN PIXEL *Zone_Scanline( MEM_IMAGE M, INT Y )
{
   return( ZONE_SCANLINE( (MEM_ZONE *)M,Y) );
}

EXTERN void Zone_Destroy( MEM_IMAGE M )
{
   if ( M==NULL ) return;
   ZONE_DESTROY( (MEM_ZONE *)M );
   M_Free( M );
}

EXTERN void Zone_Set_Position( MEM_IMAGE M, INT X, INT Y )
{
   /* TODO: backup parameter in ZONE_SET_POSITION() is
      not used any more... Remove.*/
   if ( M!=NULL ) ZONE_SET_POSITION( (MEM_ZONE *)M, X, Y, 0 );
}

EXTERN INT Zone_Flags( MEM_IMAGE M )
{
   return( MEM_Flags( (MEM_ZONE *)M ) ); 
}

EXTERN INT Zone_Set_Flags( MEM_IMAGE M, INT Flag )
{
   MEM_Flags( (MEM_ZONE *)M ) |= Flag;
   return( MEM_Flags( (MEM_ZONE *)M ) );
}

EXTERN INT Zone_Clear_Flags( MEM_IMAGE M, INT Flag )
{
   MEM_Flags( (MEM_ZONE *)M ) &= ~Flag;
   return( MEM_Flags( (MEM_ZONE *)M ) );
}

EXTERN INT Zone_Xo( MEM_IMAGE M )
{
   return( MEM_Xo( (MEM_ZONE *)M ) ); 
}

EXTERN INT Zone_Yo( MEM_IMAGE M )
{
   return( MEM_Xo( (MEM_ZONE *)M ) ); 
}

EXTERN DRV_EVENT *Zone_Event( MEM_IMAGE M )
{
   MEM_ZONE_DRIVER *Drv;

   Drv = (MEM_ZONE_DRIVER *)M;
   if ( Drv == NULL || !(Drv->Type & MEM_IS_DISPLAY) ) return( NULL );
   return( &Drv->Event );
}

/*******************************************************************/

EXTERN void Print_Zone_Specs( STRING What, MEM_IMG *Zone )
{
#ifndef SKL_LIGHT_CODE

   UINT Shift[4], Mask[4];

   Out_String( "%s: - Size: %d x %d x 0x%.4x (Q=%d)",
      What, IMG_Width(Zone), IMG_Height(Zone),
      IMG_Format(Zone), IMG_Quantum(Zone) );

   if ( IMG_Format(Zone) == FMT_CMAP )
   {
      Out_Message( " (- Colormap -)" );
      return;
   }
   else Out_Message( " (- RGB -)" );
   Format_Mask_And_Shift( IMG_Format(Zone), Mask, Shift );
   Out_Message( "    - Shifts:(%d,%d,%d) Masks:(0x%.2x,0x%.2x,0x%.2x)",
      Shift[RED_F], Shift[GREEN_F], Shift[BLUE_F],
      Mask[RED_F], Mask[GREEN_F], Mask[BLUE_F] );

#endif   // SKL_LIGHT_CODE
}

EXTERN void Drv_Print_Req_Infos( MEM_ZONE_DRIVER *M,
   void (*Print)( STRING, MEM_IMG * ) )
{
#ifndef SKL_LIGHT_CODE

   INT i;
   if ( M==NULL || !(M->Type&MEM_IS_DISPLAY) ) return;
   if ( M->Nb_Req_Modes==0 ) return;
   Out_Message( "\n * Requested mode(s):" );
   for( i=0; i<M->Nb_Req_Modes; ++i )
   {
      char S[32];
      sprintf( S, "Mode %ld", i+1 );
      (*Print)( S, &M->Req_Modes[i] );
   }

#endif   // SKL_LIGHT_CODE
}

/********************************************************/

EXTERN INT Driver_Change_CMap( MEM_IMAGE Dummy, INT Nb, COLOR_ENTRY *CMap )
{
   MEM_ZONE *M;

   M = (MEM_ZONE *)Dummy;

   if ( MEM_Quantum(M)>1 ) return( -1 );
   if ( M->CMapper.Dummy==NULL ) return(-1);

   if ( M->CMapper.Dummy->Type == INDEXER_TYPE )
   {
      Skl_Store_CMap( M->CMapper.Indexer->Orig_Cols, CMap, Nb );
      Skl_Store_Formatted_CMap( (UINT *)M->CMapper.Indexer->Fmt_Cols, 
         CMap, Nb, M->CMapper.Indexer->Out );
      M->CMapper.Indexer->Stamp++;
   }
   else if ( M->CMapper.Dummy->Type == DITHERER_TYPE )
   {     // FMT_332
      Skl_Store_CMap( M->CMapper.Ditherer->Cols, CMap, Nb );
#if 0
      if ( M->CMapper.Ditherer->Dst_Stamp != NULL &&
         ( *M->CMapper.Ditherer->Dst_Stamp > M->CMapper.Ditherer->Stamp ) )
      {
            /* Dst_Stamp != NULL means that M->Dst != NULL. We can match colors */
         Drv_Match_CMaps( M->CMapper.Ditherer->Match, M->Dst->CMapper.Matcher->Cols, Nb );
         M->CMapper.Ditherer->Stamp = *M->CMapper.Ditherer->Dst_Stamp;
      }
      else M->CMapper.Ditherer->Stamp++;
#endif
   }
   else if ( M->CMapper.Dummy->Type == COLOR_CMAP_TYPE )
   {
      Skl_Store_CMap( M->CMapper.Matcher->Cols, CMap, Nb );
      if ( M->Type & MEM_IS_DISPLAY )
      {
         M->CMapper.Matcher->Stamp++;
         return( (*((MEM_ZONE_DRIVER *)M)->Driver->Change_Colors)( (MEM_ZONE_DRIVER *)M, Nb, CMap ) );
      }
      if ( M->CMapper.Matcher->Dst_Stamp != NULL &&
         ( *M->CMapper.Matcher->Dst_Stamp > M->CMapper.Matcher->Stamp ) )
      {
            /* Dst_Stamp != NULL means that M->Dst != NULL. We can match colors */
         Skl_Match_CMaps( M->CMapper.Matcher->Cols, M->Dst->CMapper.Matcher->Cols, Nb );
         M->CMapper.Matcher->Stamp = *M->CMapper.Matcher->Dst_Stamp;
      }
      else M->CMapper.Matcher->Stamp++;
   }

   return( 0 );
}


/********************************************************/

EXTERN MEM_IMG *Parse_Mode_String( STRING S, INT *Nb_Modes, MEM_IMG **M )
{
   char *S2;
   INT Nb;
   INT Width[256], Height[256];    // 256 should be enough...
   FORMAT Format[256];

   if ( S==NULL ) return( NULL );
   S2 = S;
   Nb = 0;

   while( *S2 != '\0' )
   {
      UINT F_Pos;

      if ( *S2 == '?' ) Width[Nb] = - 1;
      else if ( ( sscanf( S2, "%d:", Width+Nb ) != 1 ) || Width[Nb]<=0 )
         goto Error;
      while ( *S2!=':' ) { if ( *S2=='\0' ) goto Error; else S2++; }
      S2++;

      if ( *S2 == '?' ) Height[Nb] = - 1;
      else if ( ( sscanf( S2, "%d:", Height+Nb ) != 1 ) || Height[Nb]<=0 )
         goto Error;   
      while ( *S2!=':' ) { if ( *S2=='\0' ) goto Error; else S2++; }
      S2++;

      if ( *S2 == '?' ) Format[Nb] = (FORMAT)0xFFFFFFFF;    /* !!! */
      else if ( sscanf( S2, "%x", Format+Nb ) != 1 ) goto Error;
      if ( Format[Nb]&FMT_SPECIAL_BIT ) Format[Nb] = (FORMAT)0xFFFFFFFF; /* !!! */
      Format[Nb] = Compute_Format_Depth( Format[Nb] );
      F_Pos = Format_F_Pos( *Format );
      if ( F_Pos == 0 )
      {
         F_Pos = Packed_Field_Pos( Format[Nb] );
         Format[Nb] = (FORMAT)( ( Format[Nb]&0xF000FFFF ) | F_Pos );
      }

#if 0
         // TODO: Fix that?
         // reject RGB fields > 8
      if ( (Format[Nb] & 0x00F)>0x008 ) Format[Nb] = (Format[Nb] & ~0x00F ) | 0x008;
      if ( (Format[Nb] & 0x0F0)>0x080 ) Format[Nb] = (Format[Nb] & ~0x0F0 ) | 0x080;
      if ( (Format[Nb] & 0xF00)>0x800 ) Format[Nb] = (Format[Nb] & ~0xF00 ) | 0x800;
#endif
      if ( Nb<256 ) Nb++;
      else break;

      while ( *S2!=';' && *S2!='\0' ) S2++;
      if ( *S2=='\0' ) break;
      S2++;
   }
   if ( !Nb ) return( *M );
   *M = (MEM_IMG*)My_Realloc( *M, ( (*Nb_Modes)+Nb )*sizeof( MEM_IMG ) );
   if ( (*M)==NULL ) return( NULL );
   memset( (*M)+(*Nb_Modes), 0, Nb*sizeof( MEM_IMG ) );
   while( --Nb>=0 )
   {
      (*M)[(*Nb_Modes)].Width = Width[Nb]; 
      (*M)[(*Nb_Modes)].Height = Height[Nb];
      (*M)[(*Nb_Modes)].Format = Format[Nb];
      (*Nb_Modes)++;
   }
   return( *M );

Error:
   SET_DRV_ERR2( "can't parse mode string '%s'", S );
   return( NULL );
}

EXTERN MEM_IMAGE Driver_Call( _G_DRIVER_ *Driver, ... )
{
   DRIVER_ARG Args;
   va_list Arg_List;
   VAR_ARG_TYPE Arg;
   MEM_ZONE_DRIVER *M;
   _G_DRIVER_ *Drv;
   INT i;

   Mem_Clear( &Args );
   Args.Refresh_Method = (REFRESH_MTHD)_ALL_REFRESH_METHODS_;  // All methods, a priori
   Args.Display_Name = NULL;
   Args.Detect = FALSE;
   Args.Convert = FALSE;
   Args.Force_Convert = 0xFFFFFFFF;
   Args.Direct = FALSE;
   Args.Window_Name = "-"; //"-(Empty)-";
   Args.Nb_Modes = 0;
   Args.Modes = 0;

   M = NULL;
   SET_DRV_ERR( NULL );

   va_start( Arg_List, Driver );

   while( 1 )
   {
      Arg = va_arg( Arg_List, VAR_ARG_TYPE );
      switch( Arg )
      {
         case DRV_END_ARG: goto End_Parsing; 

         case DRV_DIRECT: Args.Direct = TRUE; break;

         case DRV_DETECT: Args.Detect = TRUE; break;

#ifndef SKL_NO_CVRT

         case DRV_CONVERT: Args.Convert = TRUE; break;

         case DRV_FORCE:
            Args.Force_Convert = va_arg( Arg_List, UINT );
            Args.Force_Convert &= 0xFFFFFFFF;
            Args.Convert = TRUE;
         break;

#endif   // SKL_NO_CVRT

         case DRV_DONT_USE:
            {
              int Mask = ~va_arg( Arg_List, REFRESH_MTHD );
              Args.Refresh_Method = (REFRESH_MTHD)(Args.Refresh_Method&Mask);
              if ( Args.Refresh_Method == _VOID_ )
              {
                 SET_DRV_ERR( "No refresh method" );
                 return( NULL );
              }
            }
         break;

         case DRV_DISPLAY: 
            Args.Display_Name = va_arg( Arg_List, STRING );
         break;

         case DRV_PRINT_INFO: Args.Print = TRUE; break;

         case DRV_NAME:
            Args.Window_Name = va_arg( Arg_List, STRING );
         break;

         case DRV_MODE:
         {
            STRING S;
            S = va_arg( Arg_List, STRING );
            if ( Parse_Mode_String( S, &Args.Nb_Modes, &Args.Modes ) == NULL )
               return( NULL );
         }
         break;

         default:    // just skip..
         {
            INT i;
            void *Tmp;
            i = Arg & 0xFF;
            while( i-- ) Tmp = va_arg( Arg_List, void * );
         }
         break;
      }
   }
End_Parsing:

   va_end( Arg_List );

      // Detect and setup driver

   for( i=0; i<MAX_VIDEO_SUPPORT; ++i )
   {
      if ( Driver == NULL )
      {
         if ( _Reg_Drivers_[i] == NULL ) continue;
         else Drv = _Reg_Drivers_[i];
      }
      else Drv = Driver;

      M = (*Drv->Setup)( Drv, (void *)&Args );
      if ( M==NULL ) goto Failed;

      M->Driver = Drv;

         // Adapt required modes (and convert if wanted)

      if ( Args.Nb_Modes )
         if ( !(*Drv->Adapt)( M, (void *)&Args ) )
            goto Failed;

         // Now, driver is owning this malloc'ed mem

      M->Req_Modes = Args.Modes; Args.Modes = NULL;
      M->Nb_Req_Modes = Args.Nb_Modes; Args.Nb_Modes = 0;

      M->Cur_Req_Mode = 0;
      M->Cleanup_Mode = NULL;

      if ( Args.Print ) (*Drv->Print_Info)( M );

         // And open first mode if desired
      if ( !Args.Detect )
      {
         M = (MEM_ZONE_DRIVER *)Driver_Open_Mode( (MEM_ZONE *)M, 1 );
         if ( M==NULL ) goto End;
      }
      return( (MEM_IMAGE )M );

Failed:
      if ( M!=NULL ) ZONE_DESTROY( M );
      M_Free( M );

      if ( Driver != NULL ) return( NULL );  // early exit
   }
   if ( i==MAX_VIDEO_SUPPORT && _Drv_Err_==NULL ) SET_DRV_ERR( "No suitable driver" );
End:
   if ( M != NULL ) ZONE_DESTROY( M );
   M_Free( M );
   M_Free( Args.Modes );
   return( NULL );
}

/********************************************************/
/********************************************************/
