/***********************************************
 *              cache III loading              *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "./mip_cst.h"
#include "indy.h"

/******************************************************************/
/******************************************************************/

static void Mesh_Store_UV_Area_II( POLY *P )
{
   FLT dU1, dU2, dV1, dV2;

   dU1 = ( P->UV[1][0]-P->UV[0][0] ) / 65536.0;
   dU2 = ( P->UV[2][0]-P->UV[1][0] ) / 65536.0;
   dV1 = ( P->UV[1][1]-P->UV[0][1] ) / 65536.0;
   dV2 = ( P->UV[2][1]-P->UV[1][1] ) / 65536.0;

   P->U1V2_U2V1 = dU1*dV2-dU2*dV1;
}

   // Hierarchy must be ok before getting here...

static void Check_Poly_Mip( POLY *P, MIP_BLOCK *Mip, TXT_CACHE_III *Cache )
{
   POLY_MIPS *PMips;
   INT U, V, Width;

   if ( P==NULL ) return;  // Mip->Down slot is needed...

   if ( P->Ptr==NULL )
   {
      if ( Cache->Cur_PMips >= Cache->Nb_PMips )
         Exit_Upon_Error( "PMips! %d/%d", Cache->Cur_PMips, Cache->Nb_PMips );
      P->Ptr = &Cache->PMips[ Cache->Cur_PMips++ ];
      PMips = (POLY_MIPS*)P->Ptr;
      PMips->Max_Mip = 0;
   }
   else PMips = (POLY_MIPS*)P->Ptr;

   PMips->Mips[Mip->Mip_Level] = Mip;
   if ( Mip->Mip_Level>PMips->Max_Mip ) PMips->Max_Mip = Mip->Mip_Level;

   Width = Mip_Width[Mip->Mip_Level];
   U = (Mip->UV & 0xff )<<8;
   V = Mip->UV & 0xff00;

   if ( P==Mip->Up )
   {
      P->UV[ 0 ][0] = 0x0000;
      P->UV[ 0 ][1] = 0x0000;
      P->UV[ 1 ][0] = 0xffff;
      P->UV[ 1 ][1] = 0x0000;
      P->UV[ 2 ][0] = 0x0000;
      P->UV[ 2 ][1] = 0xffff;
   }
   else
   {
      P->UV[ 0 ][0] = 0xffff;
      P->UV[ 0 ][1] = 0xffff;
      P->UV[ 1 ][0] = 0x0000;
      P->UV[ 1 ][1] = 0xffff;
      P->UV[ 2 ][0] = 0xffff;
      P->UV[ 2 ][1] = 0x0000;
   }
   Mesh_Store_UV_Area_II( P );
}

static INT Install_New_Mip_Block( MIP_BLOCK *Mip, TXT_CACHE_III *Cache, USHORT *Maps )
{
   MIP_BLOCK *New_Mip;

      // New_Mip will hold NEW pseudo-allocated space for MIP_BLOCK

   New_Mip = Search_Free_Mip_Block( Mip, Cache, Maps );
   if ( New_Mip==NULL ) return( -1 );

   Check_Poly_Mip( New_Mip->Up, New_Mip, Cache );
   Check_Poly_Mip( New_Mip->Down, New_Mip, Cache );

   return( 0 );
}

/******************************************************************/

#define ADD_COL(Col) { USHORT C = (Col); B += (C&0x001F)<<3; G += (C&0x07E0)>>3; R += (C&0xF800)>>8; }

static void Do_One_Mip( USHORT *Src, INT Width )     // in-place mip crunch
{
#if 0
   INT j;
   USHORT *Dst;

   Dst = Src;
   for( j=0; j<=Width; ++j )
   {
      INT i;
      USHORT R,G,B;

      for( i=0; i<Width-j-1; i++ )
      {
         R = G = B = 0;
         ADD_COL( Src[0] ); 
         R *= 5; G *= 5; B *= 5;
         ADD_COL( Src[1] );
         ADD_COL( Src[2*Width] );
         ADD_COL( Src[2*Width+1] );
         R /= 8; G /= 8; B /= 8;
         *Dst++ = RGB_TO_565(R,G,B);
         Src += 2;
      }

         // ( i==Width-j-1 )
      if ( j!=Width )
      {
         R = G = B = 0;
         ADD_COL( Src[0] ); 
         R *= 2; G *= 2; B *= 2;
         ADD_COL( Src[1] );
         ADD_COL( Src[2*Width] );
         R /= 4; G /= 4; B /= 4;
         *Dst++ = RGB_TO_565(R,G,B);
         Src += 2;
      }
         // ( i==Width-j )         
      if ( j!=0 )
      {
         R = G = B = 0;
         ADD_COL( Src[0] ); 
         R *= 2; G *= 2; B *= 2;
         ADD_COL( Src[1] );
         ADD_COL( Src[-2*Width+1] );
         R /= 4; G /= 4; B /= 4;
         *Dst++ = RGB_TO_565(R,G,B);
         Src += 2;
      }
      for( i=0-j+1; i<0; i++ )
      {
         R = G = B = 0;
         ADD_COL( Src[0] ); 
         R *= 5; G *= 5; B *= 5;
         ADD_COL( Src[1] );
         ADD_COL( Src[-2*Width] );
         ADD_COL( Src[-2*Width+1] );
         R /= 8; G /= 8; B /= 8;
         *Dst++ = RGB_TO_565(R,G,B);
         Src += 2;
      }
      Src += 2*Width;
   }
#endif
   INT j;
   USHORT *Dst;

   Dst = Src;
   for( j=0; j<Width; ++j )
   {
      INT i;
      USHORT R,G,B;

      for( i=0; i<Width; i++ )
      {
         R = G = B = 0;
         ADD_COL( Src[0] ); 
         R *= 5; G *= 5; B *= 5;
         ADD_COL( Src[1] );
         ADD_COL( Src[2*Width] );
         ADD_COL( Src[2*Width+1] );
         R /= 8; G /= 8; B /= 8;
         *Dst++ = RGB_TO_565(R,G,B);
         Src += 2;
      }
      Src += 2*Width;
   }
}

static INT Do_All_Mips( MIP_BLOCK *Mip, USHORT *Maps, 
   INT Mip_Min, TXT_CACHE_III *New,
   OBJ_NODE **Last_Obj_Up, OBJ_NODE **Last_Obj_Down,
   UINT Up, UINT Down )
{
   INT Level;
   POLY *P;
   USHORT ID, Poly;
   MESH *Msh;

   ID = (USHORT)( (Up>>16)&0xFFFF );
   Poly = (USHORT)( Up&0xFFFF );
//      fprintf( stderr, "Up: ID:0x%x  Poly:#0x%x\n", ID, Poly );
   if ( ((*Last_Obj_Up)==NULL) || ( (*Last_Obj_Up)->ID != ID ) )
      *Last_Obj_Up = Search_Node_From_ID( _RCst_.The_World->Root, ID );
   Msh = (MESH*)(*Last_Obj_Up)->Data;
   P = &Msh->Polys[Poly];
   Mip->Up = P;

   ID = (USHORT)( (Down>>16)&0xFFFF );
   Poly = (USHORT)( Up&0xFFFF );
   if ( ID!=0xFFFF )
   {
      Poly = (USHORT)( Down&0xFFFF );
      if ( ((*Last_Obj_Down)==NULL) || ( (*Last_Obj_Down)->ID != ID ) )
         *Last_Obj_Down = Search_Node_From_ID( _RCst_.The_World->Root, ID );
      Msh = (MESH*)(*Last_Obj_Down)->Data;
      P = &Msh->Polys[Poly];
      Mip->Down = P;         
   }
   else Mip->Down = NULL;

   if ( Install_New_Mip_Block( Mip, New, Maps ) ) return( -1 );

   for( Level=Mip->Mip_Level-1; Level>=Mip_Min; --Level )
   {
      MIP_BLOCK New_Mip;
      INT Width;

      Width = Mip_Width[Level];
      Do_One_Mip( Maps, Width );
      New_Mip.Mip_Level = (BYTE)Level;
      New_Mip.Installed = FALSE;
      New_Mip.Up = Mip->Up;
      New_Mip.Down = Mip->Down;
      if ( Install_New_Mip_Block( &New_Mip, New, Maps ) ) return( -1 );
   }
   return( 0 );
}

/******************************************************************/

static FILE *Open_Txt_Cache_File( STRING Name, STRING Magik, TXT_CACHE_III **New )
{
   FILE *In;
   char Head[10];
   PIXEL C1[2];

   In = Access_File( Name, READ_SWITCH );
   if ( In==NULL ) return( NULL );

   F_READ( Head, In, 6 );        // *Do* use F_READ
   if ( strncmp( Magik, (char *)Head, 6 ) )
      Exit_Upon_Error( "'%s' has wrong Txt_Cache header.", Name );

   *New = New_Fatal_Object( 1, TXT_CACHE_III );
   if ( *New==NULL ) return( In );
      // Warning !!! *New needs being zeroed here!!!

   F_READ( &C1, In, 2 );
   (*New)->Nb_Mips = C1[0] + C1[1]*256;

   (*New)->Nb_PMips = 2*(*New)->Nb_Mips;
   (*New)->PMips = New_Fatal_Object( (*New)->Nb_PMips, POLY_MIPS );
   (*New)->Cur_PMips = 0;

   return( In );
}

/******************************************************************/

EXTERN void *Load_Txt_Cache_III( STRING Name, INT Mip_Min )
{
   FILE *In;
   TXT_CACHE_III *New;
   INT i; 
   OBJ_NODE *Last_Obj_Up, *Last_Obj_Down;


   In = Open_Txt_Cache_File( Name, TXT_MAGIK3, &New );
   if ( In==NULL ) return( NULL );
   if ( New==NULL ) goto End;
   if ( New->Nb_Mips==0 ) goto End;

   Last_Obj_Up = NULL;
   Last_Obj_Down = NULL;

   for ( i=0; i<New->Nb_Mips; ++i )
   {
      MIP_BLOCK Mip;
      UINT Tmp;
      UINT Up, Down;
      USHORT Maps[256*256];

      Mem_Clear( &Mip );
      F_READ( &Mip.Mip_Level, In, sizeof( BYTE ) );
      F_READ( &Tmp, In, sizeof( UINT ) ); 
      Up = (UINT)Indy_l( Tmp );   // WARNING with DEC-alpha!!!
      F_READ( &Tmp, In, sizeof( UINT ) ); 
      Down = (UINT)Indy_l( Tmp );   // WARNING with DEC-alpha!!!

      F_COMPRESS_READ( (BYTE*)Maps, In, Mip_Size[Mip.Mip_Level]*sizeof( USHORT ) );
      if (Indy_s(0x1234)!=0x1234) 
      {
         USHORT *Ptr = (USHORT*)Maps;
         INT j;
         for( j=0; j<Mip_Size[Mip.Mip_Level]; ++j ) Ptr[j] = Indy_s(Ptr[j]);
      }
      if ( Do_All_Mips( 
         &Mip, Maps, Mip_Min, 
         New, &Last_Obj_Up, &Last_Obj_Up, Up, Down ) )
            goto Failed;
   }

//   fprintf( stderr, " Nb_Mips: %d\n", New->Nb_Mips );

End:
   F_CLOSE( In );
   return( (void*)New );

Failed:
   Destroy_Txt_Cache_III( (void*)New ); 
   M_Free( New );
   goto End;
}

/******************************************************************/

static void Extract_8x8_Block( USHORT *Dst, USHORT *Src, INT Width )
{
   INT j;
   for( j=0; j<8; ++j )
   {
      memcpy( Dst, Src, 8*sizeof( USHORT ) );
      Src += MIP_WIDTH_IV;
      Dst += Width;
   }
}

EXTERN void *Load_Txt_Cache_IV( STRING Name, STRING Name2, INT Mip_Min )
{
   FILE *In;
   TXT_CACHE_III *New;
   INT i, j; 
   BITMAP_16 *Add = NULL;
   OBJ_NODE *Last_Obj_Up, *Last_Obj_Down;

   Add = Load_JPEG_565( Name2, &i );
   if ( Add==NULL ) return( NULL );
   if ( i!=3 ) goto Failed;

   In = Open_Txt_Cache_File( Name, TXT_MAGIK4, &New );
   if ( In==NULL ) return( NULL );
   if ( New==NULL ) goto End;
   if ( New->Nb_Mips==0 ) goto End;

   Last_Obj_Up = NULL;
   Last_Obj_Down = NULL;

   for ( i=0; i<New->Nb_Mips; ++i )
   {
      MIP_BLOCK Mip;
      UINT Tmp;
      USHORT Maps[256*256];
      UINT Up, Down;

      Mem_Clear( &Mip );
      F_READ( &Mip.Mip_Level, In, sizeof( BYTE ) ); 
      F_READ( &Tmp, In, sizeof( UINT ) ); 
      Up = (UINT)Indy_l( Tmp );   // WARNING with DEC-alpha!!!
      F_READ( &Tmp, In, sizeof( UINT ) );
      Down = (UINT)Indy_l( Tmp ); // WARNING with DEC-alpha!!!

      if ( Mip.Mip_Level<MIP_INF )
      {
         F_COMPRESS_READ( (BYTE*)Maps, In, Mip_Size[Mip.Mip_Level]*sizeof( USHORT ) );
         if (Indy_s(0x1234)!=0x1234) 
         {
            USHORT *Ptr = (USHORT*)Maps;
            for( j=0; j<Mip_Size[Mip.Mip_Level]; ++j ) Ptr[j] = Indy_s(Ptr[j]);
         }
      }
      else
      {
         INT k, l, Off, Width, Nb_Blk;
         USHORT *Src;
         USHORT *Dst;

         F_READ( &Off, In, sizeof( INT ) );  // !! Indian
         Off = Indy_l( Off );
//         fprintf( stderr, "Cur_Store=%d\n", Off );
         Src = (USHORT*)Add->Bits;
         Src += Off;
         Off &= MIP_WIDTH_IV - 1;
         Width = Mip_Width[ Mip.Mip_Level ];
         Nb_Blk = Width/8;

         for( k=0; k<Nb_Blk; ++k )
         {
            Dst = Maps + k*8*Width;
            for( l=0; l<Nb_Blk; ++l )
            {
               Extract_8x8_Block( Dst, Src, Width );
               Src += 8;
               Dst += 8;
               Off += 8;
               if ( Off==MIP_WIDTH_IV ) { 
                  Src += 7*MIP_WIDTH_IV; 
                  Off=0;
               }
            }
         }
      }

      if ( Do_All_Mips( 
         &Mip, Maps, Mip_Min, 
         New, &Last_Obj_Up, &Last_Obj_Up, Up, Down ) )
            goto Failed;
   }

//   fprintf( stderr, " Nb_Mips: %d\n", New->Nb_Mips );

End:
   F_CLOSE( In );

   Destroy_16b( Add );
   return( (void*)New );

Failed:
   Destroy_Txt_Cache_III( (void*)New ); 
   M_Free( New );
   goto End;
}

/******************************************************************/
