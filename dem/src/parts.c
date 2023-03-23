/*
 * Parts
 *
 * Skal 98
 ***************************************/

#include "demo.h"

/********************************************************************/

EXTERN void Init_00( INT Param )
{
   switch( Param )
   {
      case 0:        // Parental I. Raw
         Timer0 = Timer;
         Next_Beat = Timer0 + BEAT_TIMER_III;
         Cur_Mask = Mask022;
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim32; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 180,240,160, 0x2565 );
         Setup_Anim( );
         Init_Deform_Blur( );
         Dsp_Radial = Dsp_Radial_1;
         SELECT_565( 1 );
         Loop_14_Anim_Mask( );         
         Copy_Buffer_Short( &VB(1), &VB(VSCREEN) );
         VBuffer_Clear( &VB(VSCREEN) );
      break;

      case 1:        // Parental II
         Cur_Mask = MaskWork;
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim32; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 180,220,160, 0x2565 );
         Setup_Anim( );

         Init_Appear( 1 );
         Copy_Buffer_Short( &VB(VBLUR_II), &VB(VSCREEN) );
         Init_Dsp( 0 ); // for safety
      break;

      case 2:       // Parental II + Dissolve
         Cur_Mask = MaskWork; 
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim32; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 180,220,160, 0x2565 );
         Setup_Anim( );
      break;

      case 3:       // Parental + Skal
         Cur_Mask = MaskBuy;
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim12; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 220,210,170, 0x2565 );
         Setup_Anim( );

         Init_Appear( 1 );
         Copy_Buffer_Short( &VB(VBLUR_II), &VB(VSCREEN) );      // copy screen in VB #1
      break;

      case 10:       // Parental II + Dissolve
         Cur_Mask = MaskConsume; 
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim32; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 180,220,160, 0x2565 );
         Setup_Anim( );
         JA_Parity = 0; // sprites #1
      break;

      case 11:       // Parental II + Dissolve
         Cur_Mask = MaskDie; 
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim12; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 240,220,150, 0x2565 );
         Setup_Anim( );
         JA_Parity = 0; // sprites #1
      break;

      case 7:        // official sponsor
         Cur_Mask = Mask012;
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim32; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
//         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 200,220,190, 0x2565 );
         Setup_Anim( );
         Init_Blur_II( 0 );
         Init_Scroll( 3 );
      break;

      case 8:        // Welcome(c)...
         Cur_Mask = Mask052; 
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim12; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 220,220,220, 0x2565 );
         Setup_Anim( );
      break;

      case 9:
         Init_Appear( 2 ); // (disappear)         
         Copy_Buffer_Short( &VB(VBLUR_II), &VB(VSCREEN) ); // copy screen in VB #3
      break;
   }
}

EXTERN void Init_01( INT Param )    // torus
{
   switch( Param )
   {
      case 1:     // A world of technology
         Init_6( );
         Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 0.2f, 0.0 );
         Setup_Rendering( VB(VSCREEN).Bits, NULL, NULL, NULL, NULL, 
            VB(VSCREEN).W, VB(VSCREEN).H, VB(VSCREEN).BpS );
         Setup_Cam_Screen( 0.5*The_W, 0.5*The_H, 0.5*The_W, 0.5*The_H );
         Cur_Mask = Mask072; 
         Setup_Mask_II( VMASK, Cur_Mask );
         Time_Scale = 0.85; Time_Off = 0.0;
      break;

      case 2:  // tunnel + bump
         Cur_Mask = NULL;
         Init_Dsp( 0 );
      break;

      case 3:     // back from maaaarz
         Cur_Mask = Mask082; 
         Setup_Mask_II( VMASK, Cur_Mask );
      break;
      case 4:     // Dive into modernity
         Cur_Mask = Modern; 
         Setup_Mask_II( VMASK, Cur_Mask );
      break;
      case 6:     // Improve?
         Cur_Mask = Mask112; 
         Setup_Mask_II( VMASK, Cur_Mask );
      break;
      case 7:     // kiss
         Cur_Mask = Mask132; 
         Setup_Mask_II( VMASK, Cur_Mask );
      break;

      case 8:  // hot stuff inside
         Timer0 = Timer;
         Next_Beat = Timer0 + BEAT_TIMER_III;
         Setup_Rendering( VB(VSCREEN).Bits, NULL, NULL, NULL, NULL, 
            VB(VSCREEN).W, VB(VSCREEN).H, VB(VSCREEN).BpS );
         Time_Scale = 0.32f; Time_Off = 0.2f;
         Cur_Mask = Upgrade;  // Mask152; 
         Setup_Mask_II( VMASK, Cur_Mask );
         Init_Dsp( 0 );
      break;
   }
}

/********************************************************************/

EXTERN void Init_02( INT Param )
{
   switch( Param )
   {
      case 0:  // blobz
         Cur_Mask = Mask122;
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim32; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 200,250,170, 0x2565 );
         Setup_Anim( );         
         Init_Blb( 0 );
         Init_Scroll( 1 );
      break;
   }
}

EXTERN void Init_03( INT Param )
{
   switch( Param )
   {
      case 0:      // blured logo
         Init_D2( 0 );
         Init_Scroll( Param );         
      break;

      case 1:      // blured logo final
         Cur_Mask = Mask212;
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim93; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Setup_Anim( );
         Init_D2( 1 );
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 190,220,170, 0x2565 );
      break;

      case 2:        // no way + scroll
         Cur_Mask = Mask162;
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim72; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 190,220,170, 0x2565 );
         Setup_Anim( );
         Init_Scroll( 2 );
      break;

      case 3:        // no way_II...
         Cur_Mask = NULL;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 180,220,150, 0x4777 );
      break;

      case 11:        // ready?
         Cur_Mask = MRest;
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim93; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 180,220,150, 0x2565 );
         Setup_Anim( );

              // Ready II
         Init_Caustics( 0 );     // <- will init VB(1)
         SELECT_CMAP(2);
         Check_CMap_VBuffer( &VB(2), 256 );
         Drv_Build_Ramp_16( VB(2).CMap, 0, 256, 0, 0, 0, 190, 255, 230, 0x4777 );
         Anim_Cnt = 0;         
      break;

      case 13:        // Bomb (End)
         Cur_Mask = Mask222;
         Setup_Mask_II( VMASK, Cur_Mask );
         Cur_Anim = Anim82; Cur_Anim_Width = 160; Cur_Anim_Height = 100;
         Set_Anim_CMap( &VB(JANIM), 256, 0,0,0, 180,220,150, 0x2565 );
         Setup_Anim( );
      break;
   }
}

EXTERN void Init_Greets( INT Param )
{
   switch( Param )
   {
      case 0:        // greet1. setup
         Time_Scale = 1.0; Time_Off = 0.0;
         Cur_Mask = MaskGreets1;
         Setup_Mask_II( VMASK, Cur_Mask );
         Init_World4( 0 );
      break;

      case 1:        // greet2
         Cur_Mask = MaskGreets2;
         Setup_Mask_II( VMASK, Cur_Mask );
         Init_World4( 1 );   // sets up camera...
      break;

      case 2:        // greet3
         Time_Scale = 0.98;
         Cur_Mask = MaskGreets3;
         Setup_Mask_II( VMASK, Cur_Mask );
         Init_World4( 2 );
      break;
   }
}


EXTERN void Init_04( INT Param )
{
   switch( Param )
   {
      case 0:        // WorldII. 1rst time. Out vbuf is VB2
         Init_Deform_Blur( );
         Dsp_Radial = Dsp_Radial_1;
         Init_World2( );
         Setup_Clipping( 0.0, The_W-0.0, 0.0, The_H-0.0, 1.0, 2000.0 );
         Setup_Rendering( VB(VBLUR_II).Bits, NULL, NULL, NULL, NULL, 
            VB(VBLUR_II).W, VB(VBLUR_II).H, VB(VBLUR_II).BpS );
         Setup_Cam_Screen( 0.5*The_W, 0.5*The_H, 0.5*The_W, 0.5*The_H );
         Time_Scale = 1.0; Time_Off = 0.0;
      break;

      case 1:
         Setup_Rendering( VB(VSCREEN).Bits, NULL, NULL, NULL, NULL, 
            VB(VSCREEN).W, VB(VSCREEN).H, VB(VSCREEN).BpS );
//         Setup_Cam_Screen( 0.5*The_W, 0.5*The_H, 0.5*The_W, 0.5*The_H );
      break;
   }
}

EXTERN void Init_05( INT Param )
{
   UINT CMap[256];
   switch( Param )
   {
      case 0:
      {
         Init_Deform_Blur( );
         Dsp_Radial = Dsp_Radial_2;
         SELECT_565( 1 );
         memcpy( VB(1).Bits, End_Pic->Bits, End_Pic->Size );
         SELECT_565_V(4);
         Extract_Virtual_VBuffer( &VB(4), &VB(1), 210, 0, 68, 200 );
         Drv_Build_Ramp_16( (void*)CMap, 0, 256, 0,0,0, 200,220,255, 0x777 );
         Paste_256_Y( &VB(4), (BYTE*)Count1->Bits, 0x00, CMap, Count1->Width );
         DISPOSE( 4 );
         Destroy_16b( Count1 );
         Destroy_16b( Count2 );
         Destroy_16b( Count3 );
         Destroy_16b( Cnt4 );
         Destroy_16b( Cnt5 );
      }
      break;
      case 1:
         Cur_Mask = Resist;
         Setup_Mask_II( VMASK, Cur_Mask );
         DISPOSE( 1 );
         SELECT_565_V( 1 );
         VB(1).Bits = End_Pic->Bits;
         Mixer.Mix_16_8_Blend_Mask_II( &VB(VSCREEN), &VB(VMASK), &VB(1) );
         DISPOSE( 1 );
         Clear_DMask( );
         Destroy_16b( End_Pic );
      break;
   }
}

/********************************************************************/
