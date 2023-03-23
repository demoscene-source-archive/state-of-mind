/***********************************************
 *                mat.c                        *
 * Skal 97                                     *
 ***********************************************/

#include "main3d.h"

static PIXEL Txt_Quantum[ 5 ] = { 0, 1, 2, 1, 1 };

/******************************************************************/

static void Init_Material( OBJECT *Obj )
{
   MATERIAL *Mat;

   Mat = (MATERIAL*)Obj;

   Mat->Users = 0;
   Mat->Shader = NULL;
   Mat->Txt1 = Mat->Txt2 = Mat->Txt3 = NULL;
   Mat->Map_Nb1 = Mat->Map_Nb2 = Mat->Map_Nb3 = -1;
   Mat->Mapped = FALSE;
   Mat->C1 = Mat->C2 = Mat->C3 = 0.0;

   Mat->Type = MATERIAL_TYPE;
}

EXTERN MATERIAL *New_Material( )
{
   MATERIAL *New;

   New = New_Fatal_Object( 1, MATERIAL );   
   Mem_Clear( New );
   Default_Obj_Fields( (OBJECT *)New );
   Init_Material( (OBJECT*)New );

   return( New );
}

EXTERN INT Finish_Material_Data( OBJECT *Mat )
{
   if ( Mat==NULL ) return( 0 );

   if ( Mat->Flags&OBJ_OWNS_NAME )
   {
      M_Free( Mat->Name );
      Mat->Flags = (OBJ_FLAG)( Mat->Flags & ~OBJ_OWNS_NAME);
   }
      /* ... */
   return( 0 );
}

static INT Finish_Material( OBJ_NODE *Node )
{
   MATERIAL *Mat;

   Mat = (MATERIAL *)Node->Data;
   if ( Mat==NULL ) return( 0 );
   return( Finish_Material_Data( (OBJECT*)Mat ) );   
}

/******************************************************************/

static void Remove_Texture_Map_User( TEXTURE_MAP *M )
{
   if ( M==NULL ) return;
   M->Users--;
   if ( M->Quantum==2 )
   {
      M++;
      M->Users--;
   }
}

static void Destroy_Material( OBJECT *Obj )
{
   MATERIAL *M;

   M = (MATERIAL *)Obj;
   if ( M->Mapped==TRUE )  // Cache1, already setup...
   {
      Remove_Texture_Map_User( M->Txt1 );
      Remove_Texture_Map_User( M->Txt2 );
      Remove_Texture_Map_User( M->Txt3 );
      M->Txt1 = M->Txt2 = M->Txt3 = NULL;
      M->Map_Nb1 = M->Map_Nb2 = M->Map_Nb3 = -1;
      M->Mapped = FALSE;
   }
   else
   {
      // else: Original format.
      M_Free( M->Txt1 );   // Map name to free
      M->Txt2 = NULL;
      M->Txt3 = NULL;
   }
   M->Mapped = FALSE;
}

/******************************************************************/

EXTERN NODE_METHODS _MATERIAL_ =   
{
   sizeof( MATERIAL ),
   Init_Material, Finish_Material, Destroy_Material,
   NULL, NULL,     // transform, anim
   NULL,    // Render
   NULL,    // Post
   NULL,    // Copy
   NULL, NULL, NULL, NULL
};

/******************************************************************/

#if 0
EXTERN TEXTURE_MAP *Install_Texture_Maps_I( TXT_CACHE *Cache,
   PIXEL *Ptr, MAP_TYPE Type1, MAP_TYPE Type2, MAP_TYPE Type3 )
{
   INT i;

   if ( Cache->Maps==NULL ) return( NULL );
   for( i=0; i<Cache->Nb_Texture_Maps; i+=3 )
   {
      if ( (Cache->Maps[i].Users!=0)||(Cache->Maps[i+1].Users!=0)||(Cache->Maps[i+2].Users!=0) )
         continue;
      Cache->Maps[i].Users = 1;
      Cache->Maps[i].Type = Type1;
      Cache->Maps[i].Quantum = Txt_Quantum[ Type1 ];
      Cache->Maps[i+1].Users = 1;
      Cache->Maps[i+1].Type = Type2;
      Cache->Maps[i+1].Quantum = Txt_Quantum[ Type2 ];
      Cache->Maps[i+2].Users = 1;
      Cache->Maps[i+2].Type = Type3;
      Cache->Maps[i+2].Quantum = Txt_Quantum[ Type3 ];
      if ( Ptr != NULL )
      {
         MemCopy( Cache->Maps[i].Ptr, Ptr, TEXTURE_MAP_SIZE );
      }
      return( Cache->Maps + i );
   }
   return( NULL );
}
#endif

/******************************************************************/
