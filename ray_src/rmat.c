/***********************************************
 *          materials for rays                 *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"
#include "ray.h"

EXTERN INT Nb_Texture_Layers = 0;
EXTERN RAY_MATERIAL Ray_Materials[MAX_RAY_MATERIALS];
EXTERN RAY_MATERIAL *Cur_Ray_Mat = NULL;
EXTERN POLY *Cur_Ray_Poly = NULL;

/******************************************************************/

EXTERN void Get_Mesh_Ray_Materials( RAY *Ray, OBJ_NODE *Node )
{
   POLY *P;

   Nb_Texture_Layers = 0;
   Cur_Ray_Mat = &Ray_Materials[Nb_Texture_Layers]; 
   Nb_Texture_Layers++;

   Cur_Ray_Poly = P = &((MESH*)Node->Data)->Polys[Ray->Id];
   
   Cur_Material = Cur_Ray_Mat->Source = (MATERIAL*)P->Ptr;
   if ( _RCst_.The_World->Cache!=NULL )
   {
      Cur_Ray_Mat->CMap = ((TXT_CACHE*)_RCst_.The_World->Cache)->CMap;
      Cur_Ray_Mat->Nb_Col = ((TXT_CACHE*)_RCst_.The_World->Cache)->Nb_Col;
   }
   if ( Cur_Material != NULL )
   {
      Cur_Texture = Cur_Ray_Mat->Cur_Texture = Cur_Material->Txt1;
      if ( Cur_Texture!=NULL )
      {
         Cache_Methods.Retreive_Texture( Cur_Texture->Slot, 0.0 );
         Cur_Ray_Mat->Uo_Mip = Uo_Mip;
         Cur_Ray_Mat->Vo_Mip = Vo_Mip;
         Cur_Ray_Mat->Mip_Scale = Mip_Scale / 65536.0f;
         Cur_Texture_Ptr = Cur_Ray_Mat->Cur_Texture_Ptr = Cur_Texture->Ptr;
      }
      Assign_FColor( Cur_Ray_Mat->Ambient, Cur_Material->Ambient );
      Assign_FColor( Cur_Ray_Mat->Diffuse, Cur_Material->Diffuse );
      Assign_FColor( Cur_Ray_Mat->Specular, Cur_Material->Specular );
   }
//   Cur_Ray_Mat = &Ray_Materials[0]; 
}

/******************************************************************/


