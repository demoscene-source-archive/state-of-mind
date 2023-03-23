/***********************************************
 *              z-Lights                       *
 *                                             *
 * Skal 97.                                    *
 ***********************************************/

#include "main3d.h"
#include "color.h"

#ifdef USE_OLD_ZLIGHT_CODE

// EXTERN PIXEL Shdw_Attenuation_Table[16*16*256];
EXTERN PIXEL Shdw_Attenuation_Table[16*16];

/******************************************************************/
/******************************************************************/

EXTERN void Render_zBuf_Light( )
{
   INT i;

      // Clear counters
   // BZero( ZBuffer_Front, ZBuffer_Size*sizeof(PIXEL) );
 
   // Debug
   BZero( ZBuffer_Back, ZBuffer_Size*sizeof(USHORT) );

   Render_zBuf_Shade( );            // Render every objects

   for( i=0; i<_RCst_.Pix_Width*_RCst_.Pix_Height; ++i )  // Post-process attenuations
   {
      USHORT Src;
      Src = _RCst_.Base_Ptr[i] << 8;
      Src |= ZBuffer_Shade[i];
      _RCst_.Base_Ptr[i] = ((PIXEL*)Shade_Table)[Src];
      ((PIXEL *)ZBuffer_Front)[i]=0x00;      // reset counter
   }
}

EXTERN void Render_zBuf_Light2( )
{
   INT i;

      // Clear counters
   BZero( ZBuffer_Shade, ZBuffer_Size*sizeof(PIXEL) );
   
   Render_zBuf_Shade( );      // Render every objects

   for( i=0; i<_RCst_.Pix_Width*_RCst_.Pix_Height; ++i )  // Post-process attenuations
   {
      USHORT Src;
      Src = Shdw_Attenuation_Table[ ((PIXEL*)ZBuffer_Shade)[i] ];
      if ( Src )
      {
         Src |= ( _RCst_.Base_Ptr[i]&0x00FF )<<8;
         _RCst_.Base_Ptr[i] = ((PIXEL*)Shade_Table)[Src];
      }

      // Src = Shdw_Attenuation_Table[ ((PIXEL*)ZBuffer_Shade)[i] ];
      // if ( Src ) _RCst_.Base_Ptr[i] = Src;

      // ZBuffer_Shade[i]=0x00;      // reset counter on the fly
   }
}

/******************************************************************/

EXTERN void Init_Attenuation_Table( )
{
   INT P, M;
   for( M=0; M<16; ++M )
      for( P=0; P<16; ++P )
      {
         INT K;
         K = (M*16+P);
         if ( P>M ) 
         {
               // Shdw_Attenuation_Table[i+K] = 0x80/(P-M+1);
               // Shdw_Attenuation_Table[i+K] = i-(P-M)*i/16;
               Shdw_Attenuation_Table[K] = 0x80; //  /(P-M+1);
         }
         else Shdw_Attenuation_Table[K] = 0;
      }
}

/******************************************************************/

EXTERN void Init_Volumic_Attenuation_Table( )
{
   INT P, M;
   for( M=0; M<16; ++M )
      for( P=0; P<16; ++P )
      {
         INT K;
         K = (M*16+P);
         if ( P>M ) 
         {
            Shdw_Attenuation_Table[K] = 0x80/(P-M+1);
         }
         else if ( P==M )
            Shdw_Attenuation_Table[K] = 0x80/(P+1);
         else if ( P<M ) 
           Shdw_Attenuation_Table[K] = 0x80/(M-P+1);
      }
}

/******************************************************************/
/******************************************************************/

#endif   // USE_OLD_ZLIGHT_CODE

