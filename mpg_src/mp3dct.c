/************************************************
 *       IMDC & polyphase transform             *
 * heavily based on 'xamp' from Tomislav Uzelac *
 * Skal 98                                      *
 ************************************************/

#define _USE_PRIVATE_

#include "main.h"
#include "bit_strm.h"
#include "./mp3.h"
#include "./mp3dct.h"

static FLT MP3_Win_0[36];
static FLT MP3_Win_1[36];
static FLT MP3_Win_3[36];
static FLT *MP3_Wins[4] = { MP3_Win_0, MP3_Win_1, NULL, MP3_Win_3 };

#ifndef UNIX

extern void ASM_dct12( FLT *, FLT *, FLT * );
extern void ASM_Shift36( FLT *, FLT *, FLT *, FLT * );
extern void ASM_dct36( FLT * );
extern void ASM_dct36_2( FLT *In, FLT *Tmp );
extern void ASM_dewin36_1( FLT *u_ptr, const FLT *Win, FLT *Sample, INT start );
extern void ASM_dewin36_2( FLT *u_ptr, const FLT *Win, FLT *Sample, INT start );
extern void ASM_Poly1( FLT *d, FLT *res_p );
extern void ASM_Poly2( FLT *d );
extern void ASM_Copy_And_Zero( FLT *, FLT *, INT );

#endif   // !UNIX

#define DIRTY_SAMPLES

/*******************************************************/

#ifdef DIRTY_SAMPLES

#define PUT_SAMPLE(s) *Cur_Samples++ = (s)

#else

#define PUT_SAMPLE(s)         \
    { if ( (s)>1.0 ) *Cur_Samples++ = 1.0;   \
      else if ( (s)<-1.0 ) *Cur_Samples++ = -1.0;   \
      else *Cur_Samples++ = (s); }

#endif

/*******************************************************/

   // imdct buffers
   
static FLT s[2][32][18] = { 0 };
static FLT *s_p, *res_p, *In;
static FLT *Cur_Samples;

/*------------------------------------------------------------------*/
/*                                                                  */
/*    Function: Calculation of the inverse MDCT                     */
/*    In the case of short blocks the 3 output vectors are already  */
/*    overlapped and added in this modul.                           */
/*                                                                  */   
/*------------------------------------------------------------------*/

#ifdef UNIX

#define DO_SAVE(a,b) { FLT X = Tmp[(a)]; Tmp[(a)]+=Tmp[(b)]; Tmp[(b)]=X-Tmp[(b)]; }

static void MP3_Do_imdct_12pts( )
{
   INT ss;

   memset( res_p, 0, 36*sizeof( res_p[0] ) );
   for( ss=-18; ss<0; ss+=6 )
   {
      FLT pp1, pp2, Save;
      FLT Tmp[6];
      FLT *In2, *Out2;

      In2 = In + ss+18;

      In2[ 5 ] += In2[ 4 ];
      In2[ 4 ] += In2[ 3 ];
      In2[ 3 ] += In2[ 2 ];
      In2[ 2 ] += In2[ 1 ];
      In2[ 1 ] += In2[ 0 ];

      In2[ 5 ] += In2[ 3 ];
      In2[ 3 ] += In2[ 1 ];

            // even indices. - 3pts -
      pp1 = In2[ 2 ] * 0.866025403f;
      pp2 = In2[ 4 ] * 0.500000000f;
      Save = In2[ 0 ] + pp2;
      Tmp[1] = In2[ 0 ] - In2[ 4 ];
      Tmp[0] = Save + pp1;
      Tmp[2] = Save - pp1;

            // odd indices. - 3pts -
      pp1 = In2[ 3 ] * 0.866025403f;
      pp2 = In2[ 5 ] * 0.500000000f;
      Save = In2[ 1 ] + pp2; 
      Tmp[4] = In2[ 1 ] - In2[ 5 ];
      Tmp[4] *= 0.707106781f;
      Tmp[5] = Save + pp1;
      Tmp[5] *= 0.517638090f;
      Tmp[3] = Save - pp1;
      Tmp[3] *= 1.931851653f;

      DO_SAVE(0,5);
      DO_SAVE(1,4);
      DO_SAVE(2,3);
      
         // Shift to 12 point modified IDCT, multiply by window type 2

      Out2 = res_p + ss+18;

      Out2[ 6] += Tmp[3] * (  0.130526192f * 0.821339815f );
      Out2[ 7] += Tmp[4] * (  0.382683432f * 1.306562965f );
      Out2[ 8] += Tmp[5] * (  0.608761429f * 3.830648788f );
      Out2[ 9] += Tmp[5] * ( -0.793353340f * 3.830648788f );
      Out2[10] += Tmp[4] * ( -0.923879532f * 1.306562965f );
      Out2[11] += Tmp[3] * ( -0.991444861f * 0.821339815f );
      Out2[12] += Tmp[2] * ( -0.991444861f * 0.630236207f );
      Out2[13] += Tmp[1] * ( -0.923879532f * 0.541196100f );
      Out2[14] += Tmp[0] * ( -0.793353340f * 0.504314480f );
      Out2[15] += Tmp[0] * ( -0.608761429f * 0.504314480f );
      Out2[16] += Tmp[1] * ( -0.382683432f * 0.541196100f );
      Out2[17] += Tmp[2] * ( -0.130526192f * 0.630236207f );
   }

   res_p[0] += s_p[0]; s_p[0] = res_p[18];
   res_p[1] += s_p[1]; s_p[1] = res_p[19];
   res_p[2] += s_p[2]; s_p[2] = res_p[20];
   res_p[3] += s_p[3]; s_p[3] = res_p[21];
   res_p[4] += s_p[4]; s_p[4] = res_p[22];
   res_p[5] += s_p[5]; s_p[5] = res_p[23];
   res_p[6] += s_p[6]; s_p[6] = res_p[24];
   res_p[7] += s_p[7]; s_p[7] = res_p[25];
   res_p[8] += s_p[8]; s_p[8] = res_p[26];
   res_p[9] += s_p[9]; s_p[9] = res_p[27];
   res_p[10] += s_p[10]; s_p[10] = res_p[28];
   res_p[11] += s_p[11]; s_p[11] = res_p[29];
   res_p[12] += s_p[12]; s_p[12] = res_p[30];
   res_p[13] += s_p[13]; s_p[13] = res_p[31];
   res_p[14] += s_p[14]; s_p[14] = res_p[32];
   res_p[15] += s_p[15]; s_p[15] = res_p[33];
   res_p[16] += s_p[16]; s_p[16] = res_p[34];
   res_p[17] += s_p[17]; s_p[17] = res_p[35];
}
#else

static void MP3_Do_imdct_12pts( )      // 12 pts IMDCT
{
   BZero( res_p, 36*sizeof( res_p[0] ) );
   ASM_dct12( In, res_p, s_p );
}

#endif

#ifdef UNIX

static void MP3_Do_imdct_36pts( FLT *Win_bt, INT Sub_Band )  
{
   FLT Tmp[18];
   In[17] += In[16]; In[16] += In[15];
   In[15] += In[14]; In[14] += In[13];
   In[13] += In[12]; In[12] += In[11];
   In[11] += In[10]; In[10] += In[9];
   In[9]  += In[8];  In[8]  += In[7];
   In[7]  += In[6];  In[6]  += In[5];
   In[5]  += In[4];  In[4]  += In[3];
   In[3]  += In[2];  In[2]  += In[1];
   In[1]  += In[0];

      // 18 point IDCT for odd indices
      
         // input aliasing for 18 point IDCT
   In[17] += In[15]; In[15] += In[13];
   In[13] += In[11]; In[11] += In[9];
   In[9]  += In[7];  In[7]  += In[5];
   In[5]  += In[3];  In[3]  += In[1];

   {
      FLT tmp[4+1], tmp_[4], tmpo[4], tmpo_[4];

   // Fast 9 Point Inverse Discrete Cosine Transform
   //
   // By  Francois-Raymond Boyer
   //         mailto:boyerf@iro.umontreal.ca
   //         http://www.iro.umontreal.ca/~boyerf
   //
   // The code has been optimized for Intel processors
   //  (takes a lot of time to convert float to and from iternal FPU representation)
   //
   // It is a simple "factorization" of the IDCT matrix.

               // 9 point IDCT on even indices
      {
         FLT i0p12 = In[0]+In[0] + In[12];
         FLT i0 = In[0] - In[12];
         FLT i4 = In[4], i8 = In[8], i16 = In[16];
         tmp[4] = i4 - i8  - i16;
         tmp[1] = i0+i0 + tmp[4];
         tmp[4] = i0 - tmp[4];
         tmp[0] = i0p12 + i4*1.8793852415718f  + i8*1.532088886238f   + i16*0.34729635533386f;
         tmp[2] = i0p12 - i4*0.34729635533386f - i8*1.8793852415718f  + i16*1.532088886238f;
         tmp[3] = i0p12 - i4*1.532088886238f   + i8*0.34729635533386f - i16*1.8793852415718f;
      }
      {
         FLT i6_ = In[6]*1.732050808f;
         FLT i2 = In[2], i10 = In[10], i14 = In[14];

         tmp_[0] = +i6_ + i2*1.9696155060244f + i10*1.2855752193731f  + i14*0.68404028665134f;
         tmp_[1] = (i2 - i10 - i14)*1.732050808f;
         tmp_[2] = -i6_ + i2*1.2855752193731f - i10*0.68404028665134f + i14*1.9696155060244f;
         tmp_[3] = -i6_ + i2*0.68404028665134f+ i10*1.9696155060244f  - i14*1.2855752193731f;
      }
            // 9 point IDCT on odd indices
      {
               // 5 points on odd indices (not realy an IDCT)

         FLT i0p12 = In[1]+In[1] + In[13];
         FLT i0 = In[1]-In[13];  // In[1]+In[1];
         FLT i5 = In[5], i9 = In[9], i17 = In[17];
         FLT tmp4o;

         tmp4o = i5 - i9 - i17;
         // tmpo[1] = i0 + tmp4o - In[13] - In[13];
         tmpo[1] = i0+i0 + tmp4o;
         tmp4o = (i0 - tmp4o)*0.707106781f; /* Twiddled */
         Tmp[4] = (tmp[4] + tmp4o)*(-0.541196100f);
         Tmp[13] = (tmp[4] - tmp4o)*(-1.306562965f);

         tmpo[0] = i0p12   + i5*1.8793852415718f  + i9*1.532088886238f       + i17*0.34729635533386f;
         tmpo[2] = i0p12   - i5*0.34729635533386f - i9*1.8793852415718f      + i17*1.532088886238f;
         tmpo[3] = i0p12   - i5*1.532088886238f   + i9*0.34729635533386f     - i17*1.8793852415718f;

      }
      {
               // 4 points on even indices */

         FLT i6_ = In[7]*1.732050808f;      // Sqrt[3]

         tmpo_[0] = In[3]*1.9696155060244f  + i6_ + In[11]*1.2855752193731f  + In[15]*0.68404028665134f;
         tmpo_[1] = (In[3] - In[11] - In[15])*1.732050808f;
         tmpo_[2] = In[3]*1.2855752193731f  - i6_ - In[11]*0.68404028665134f + In[15]*1.9696155060244f;
         tmpo_[3] = In[3]*0.68404028665134f - i6_ + In[11]*1.9696155060244f  - In[15]*1.2855752193731f;
      }

            // Twiddle factors on odd indices and Butterflies on 9 point IDCT's
            // and twiddle factors for 36 point IDCT
      {
         FLT e, o;

         e = tmp[0] + tmp_[0]; o = (tmpo[0] + tmpo_[0])*0.501909918f;
         Tmp[0] = (e + o)*(-0.500476342f*.5f);  
         Tmp[17] = (e - o)*(-11.46279281f*.5f);

         e = tmp[1] + tmp_[1]; o = (tmpo[1] + tmpo_[1])*0.517638090f;
         Tmp[1] = (e + o)*(-0.504314480f*.5f);
         Tmp[16] = (e - o)*(-3.830648788f*.5f);

         e = tmp[2] + tmp_[2]; o = (tmpo[2] + tmpo_[2])*0.551688959f;
         Tmp[2] = (e + o)*(-0.512139757f*.5f);
         Tmp[15] = (e - o)*(-2.310113158f*.5f);

         e = tmp[3] + tmp_[3]; o = (tmpo[3] + tmpo_[3])*0.610387294f;
         Tmp[3] = (e + o)*(-0.524264562f*.5f);
         Tmp[14] = (e - o)*(-1.662754762f*.5f);

         e = tmp[3] - tmp_[3]; o = (tmpo[3] - tmpo_[3])*0.871723397f;
         Tmp[5] = (e + o)*(-0.563690973f*.5f);
         Tmp[12] = (e - o)*(-1.082840285f*.5f);

         e = tmp[2] - tmp_[2]; o = (tmpo[2] - tmpo_[2])*1.183100792f;
         Tmp[6] = (e + o)*(-0.592844523f*.5f);
         Tmp[11] = (e - o)*(-0.930579498f*.5f);

         e = tmp[1] - tmp_[1]; o = (tmpo[1] - tmpo_[1])*1.931851653f;
         Tmp[7] = (e + o)*(-0.630236207f*.5f);
         Tmp[10] = (e - o)*(-0.821339815f*.5f);

         e = tmp[0] - tmp_[0]; o = (tmpo[0] - tmpo_[0])*5.736856623f;
         Tmp[8] = (e + o)*(-0.678170852f*.5f);
         Tmp[9] =  (e - o)*(-0.740093616f*.5f);
      }
            // shift to modified IDCT
      res_p[0 ] =-Tmp[9]  * Win_bt[ 0] + s_p[0];
      res_p[17] = Tmp[9]  * Win_bt[17] + s_p[17];
      res_p[1 ] =-Tmp[10] * Win_bt[ 1] + s_p[1];
      res_p[16] = Tmp[10] * Win_bt[16] + s_p[16];
      res_p[2 ] =-Tmp[11] * Win_bt[ 2] + s_p[2];
      res_p[15] = Tmp[11] * Win_bt[15] + s_p[15];
      res_p[3 ] =-Tmp[12] * Win_bt[ 3] + s_p[3];
      res_p[14] = Tmp[12] * Win_bt[14] + s_p[14];
      res_p[4 ] =-Tmp[13] * Win_bt[ 4] + s_p[4];
      res_p[13] = Tmp[13] * Win_bt[13] + s_p[13];
      res_p[5 ] =-Tmp[14] * Win_bt[ 5] + s_p[5];
      res_p[12] = Tmp[14] * Win_bt[12] + s_p[12];
      res_p[6 ] =-Tmp[15] * Win_bt[ 6] + s_p[6];
      res_p[11] = Tmp[15] * Win_bt[11] + s_p[11];
      res_p[7 ] =-Tmp[16] * Win_bt[ 7] + s_p[7];
      res_p[10] = Tmp[16] * Win_bt[10] + s_p[10];
      res_p[8 ] =-Tmp[17] * Win_bt[ 8] + s_p[8];
      res_p[9 ] = Tmp[17] * Win_bt[ 9] + s_p[9];

      s_p[0 ] = Tmp[8]  * Win_bt[18];
      s_p[1 ] = Tmp[7]  * Win_bt[19];
      s_p[2 ] = Tmp[6]  * Win_bt[20];
      s_p[3 ] = Tmp[5]  * Win_bt[21];
      s_p[4 ] = Tmp[4]  * Win_bt[22];
      s_p[5 ] = Tmp[3]  * Win_bt[23];
      s_p[6 ] = Tmp[2]  * Win_bt[24];
      s_p[7 ] = Tmp[1]  * Win_bt[25];
      s_p[8 ] = Tmp[0]  * Win_bt[26];
      s_p[9 ] = Tmp[0]  * Win_bt[27];
      s_p[10] = Tmp[1]  * Win_bt[28];
      s_p[11] = Tmp[2]  * Win_bt[29];
      s_p[12] = Tmp[3]  * Win_bt[30];
      s_p[13] = Tmp[4]  * Win_bt[31];
      s_p[14] = Tmp[5]  * Win_bt[32];
      s_p[15] = Tmp[6]  * Win_bt[33];
      s_p[16] = Tmp[7]  * Win_bt[34];
      s_p[17] = Tmp[8]  * Win_bt[35];

   }
}

#else

static void MP3_Do_imdct_36pts( FLT *Win_bt, INT Sub_Band )  // 36 pts IMDCT
{
   FLT Tmp[18];
   ASM_dct36( In );
   ASM_dct36_2( In, Tmp ); // tmp, tmp_, tmpo, tmpo_, Tmp );
   ASM_Shift36( res_p, s_p, Tmp, Win_bt );    
}

#endif   //UNIX

/*******************************************************/

/* 
 * fast DCT according to Lee[84]
 * reordering according to Konstantinides[94]
 */ 

static void MP3_Polyphase( INT ch )
{
   static FLT u[2][2][17*16];       /* no v[][], it's redundant */
   static INT u_start[2] = { 0, 0 }; /* first element of u[][] */
   static INT u_div[2]   = { 0, 0 }; /* which part of u[][] is currently used */
   INT start = u_start[ch];
   INT div = u_div[ch];

   {
      FLT d[32];

         // step 1: initial reordering and 1st (16 wide) butterflies

#ifdef UNIX

      {
         FLT c[16];
         d[0] = res_p[ 0*18]; d[16]=(d[0]  - res_p[31*18]) * MP3_b[1];  d[0] += res_p[31*18];
         d[1] = res_p[ 1*18]; d[17]=(d[1]  - res_p[30*18]) * MP3_b[3];  d[1] += res_p[30*18];
         d[3] = res_p[ 2*18]; d[19]=(d[3]  - res_p[29*18]) * MP3_b[5];  d[3] += res_p[29*18];
         d[2] = res_p[ 3*18]; d[18]=(d[2]  - res_p[28*18]) * MP3_b[7];  d[2] += res_p[28*18];
         d[6] = res_p[ 4*18]; d[22]=(d[6]  - res_p[27*18]) * MP3_b[9];  d[6] += res_p[27*18];
         d[7] = res_p[ 5*18]; d[23]=(d[7]  - res_p[26*18]) * MP3_b[11]; d[7] += res_p[26*18];
         d[5] = res_p[ 6*18]; d[21]=(d[5]  - res_p[25*18]) * MP3_b[13]; d[5] += res_p[25*18];
         d[4] = res_p[ 7*18]; d[20]=(d[4]  - res_p[24*18]) * MP3_b[15]; d[4] += res_p[24*18];
         d[12]= res_p[ 8*18]; d[28]=(d[12] - res_p[23*18]) * MP3_b[17]; d[12]+= res_p[23*18];
         d[13]= res_p[ 9*18]; d[29]=(d[13] - res_p[22*18]) * MP3_b[19]; d[13]+= res_p[22*18];
         d[15]= res_p[10*18]; d[31]=(d[15] - res_p[21*18]) * MP3_b[21]; d[15]+= res_p[21*18];
         d[14]= res_p[11*18]; d[30]=(d[14] - res_p[20*18]) * MP3_b[23]; d[14]+= res_p[20*18];
         d[10]= res_p[12*18]; d[26]=(d[10] - res_p[19*18]) * MP3_b[25]; d[10]+= res_p[19*18];
         d[11]= res_p[13*18]; d[27]=(d[11] - res_p[18*18]) * MP3_b[27]; d[11]+= res_p[18*18];
         d[9] = res_p[14*18]; d[25]=(d[9]  - res_p[17*18]) * MP3_b[29]; d[9] += res_p[17*18];
         d[8] = res_p[15*18]; d[24]=(d[8]  - res_p[16*18]) * MP3_b[31]; d[8] += res_p[16*18];

             // a test to see what can be done with memory separation
             // first we process indexes 0-15

         c[0] = d[0] + d[8 ]; c[8]  = ( d[0] - d[8 ]) * MP3_b[ 2];
         c[1] = d[1] + d[9 ]; c[9]  = ( d[1] - d[9 ]) * MP3_b[ 6];
         c[2] = d[2] + d[10]; c[10] = ( d[2] - d[10]) * MP3_b[14];
         c[3] = d[3] + d[11]; c[11] = ( d[3] - d[11]) * MP3_b[10];
         c[4] = d[4] + d[12]; c[12] = ( d[4] - d[12]) * MP3_b[30];
         c[5] = d[5] + d[13]; c[13] = ( d[5] - d[13]) * MP3_b[26];
         c[6] = d[6] + d[14]; c[14] = ( d[6] - d[14]) * MP3_b[18];
         c[7] = d[7] + d[15]; c[15] = ( d[7] - d[15]) * MP3_b[22];

            // step 3: 4-wide butterflies

         d[0] = c[0] + c[4] ; d[4 ] = ( c[0] - c[4] ) * MP3_b[ 4];
         d[1] = c[1] + c[5] ; d[5 ] = ( c[1] - c[5] ) * MP3_b[12];
         d[2] = c[2] + c[6] ; d[6 ] = ( c[2] - c[6] ) * MP3_b[28];
         d[3] = c[3] + c[7] ; d[7 ] = ( c[3] - c[7] ) * MP3_b[20];

         d[8] = c[8] + c[12]; d[12] = ( c[8] - c[12]) * MP3_b[ 4];
         d[9] = c[9] + c[13]; d[13] = ( c[9] - c[13]) * MP3_b[12];
         d[10]= c[10]+ c[14]; d[14] = (c[10] - c[14]) * MP3_b[28];
         d[11]= c[11]+ c[15]; d[15] = (c[11] - c[15]) * MP3_b[20];


            // step 4: 2-wide butterflies
         {
            FLT b8  = MP3_b[8];
            FLT b24 = MP3_b[24];

            c[0] = d[0 ]+ d[2 ]; c[2]  = ( d[0] - d[2 ]) *  b8;
            c[1] = d[1 ]+ d[3 ]; c[3]  = ( d[1] - d[3 ]) * b24;
            c[4] = d[4 ]+ d[6 ]; c[6]  = ( d[4] - d[6 ]) *  b8;
            c[5] = d[5 ]+ d[7 ]; c[7]  = ( d[5] - d[7 ]) * b24;
            c[8] = d[8 ]+ d[10]; c[10] = ( d[8] - d[10]) *  b8;
            c[9] = d[9 ]+ d[11]; c[11] = ( d[9] - d[11]) * b24;
            c[12]= d[12]+ d[14]; c[14] = (d[12] - d[14]) *  b8;
            c[13]= d[13]+ d[15]; c[15] = (d[13] - d[15]) * b24;
         }

            // step 5: 1-wide butterflies.       this is a little 'hacked up'
         {
            FLT b16 = MP3_b[16];

            d[0] = -2*( c[0] + c[1] );
            d[1] = ( c[0] - c[1] ) * b16; 

            d[2] = c[2] + c[3]; 
            d[3] = ( c[2] - c[3] ) * b16 - d[2];

            d[4] = c[4] +c[5]; 
            d[5] = ( c[4] - c[5] ) * b16 + d[4];

            d[6] = c[6] + c[7];
            d[7] = ( c[6] - c[7] ) * b16 - d[5];

            d[8 ]= c[8] + c[9];
            d[9] = ( c[8] - c[9] ) * b16;
            d[11]= d[8] + d[9];
            d[11] +=(c[10] - c[11]) * b16; 
            d[10]= c[10] + c[11]; 

            d[12]  = c[12]+ c[13]; 
            d[13] = (c[12] - c[13]) * b16;
            d[13] += -d[8]-d[9]+d[12];

            d[14]  = c[14]+ c[15]; 
            d[15] = (c[14] - c[15]) * b16 - d[11];
            d[14] += -d[8] -d[10];
         }
      }

#else
      ASM_Poly1( d, res_p );
#endif

            // step 6: final resolving & reordering

      {
         FLT *u_p;

         u_p = (FLT *)&u[ch][div][start];

         u_p[ 0*16] =  d[1];
         u_p[ 2*16] =  d[9] -d[14];
         u_p[ 4*16] =  d[5] -d[6];
         u_p[ 6*16] = -d[10] +d[13];
         u_p[ 8*16] =  d[3];
         u_p[10*16] = -d[8] -d[9] +d[11] -d[13];
         u_p[12*16] =  d[7];
         u_p[14*16] =  d[15];

           // the other 32 are stored for use with the next granule

         u_p = (FLT*)&u[ch][!div][start];

         u_p[16*16] =  d[0];
         u_p[14*16] = -d[8];
         u_p[12*16] = -d[4];
         u_p[10*16] =  d[8] -d[12];
         u_p[ 8*16] = -d[2];
         u_p[ 6*16] = -d[8] -d[10] +d[12];
         u_p[ 4*16] =  d[4] -d[6];
         u_p[ 2*16] = -d[14];
         u_p[ 0*16] = -d[1];
      }

         // memory separation, second part

#ifdef UNIX
      {
         FLT c[16];
         c[0] = d[16] + d[24]; c[8]  = (d[16] - d[24]) * MP3_b[ 2];
         c[1] = d[17] + d[25]; c[9]  = (d[17] - d[25]) * MP3_b[ 6];
         c[2] = d[18] + d[26]; c[10] = (d[18] - d[26]) * MP3_b[14];
         c[3] = d[19] + d[27]; c[11] = (d[19] - d[27]) * MP3_b[10];
         c[4] = d[20] + d[28]; c[12] = (d[20] - d[28]) * MP3_b[30];
         c[5] = d[21] + d[29]; c[13] = (d[21] - d[29]) * MP3_b[26];
         c[6] = d[22] + d[30]; c[14] = (d[22] - d[30]) * MP3_b[18];
         c[7] = d[23] + d[31]; c[15] = (d[23] - d[31]) * MP3_b[22];

         d[16] = c[0] + c[4]; d[20] = (c[0] - c[4]) * MP3_b[ 4];
         d[17] = c[1] + c[5]; d[21] = (c[1] - c[5]) * MP3_b[12];
         d[18] = c[2] + c[6]; d[22] = (c[2] - c[6]) * MP3_b[28];
         d[19] = c[3] + c[7]; d[23] = (c[3] - c[7]) * MP3_b[20];

         d[24] = c[8] + c[12]; d[28] = (c[8] - c[12]) * MP3_b[ 4];
         d[25] = c[9] + c[13]; d[29] = (c[9] - c[13]) * MP3_b[12];
         d[26] = c[10]+ c[14]; d[30] = (c[10] - c[14]) * MP3_b[28];
         d[27] = c[11]+ c[15]; d[31] = (c[11] - c[15]) * MP3_b[20];

         {
            FLT b8  = MP3_b[8];
            FLT b24 = MP3_b[24];

            c[0] = d[16] + d[18]; c[2] = (d[16] - d[18]) *  b8;
            c[1] = d[17] + d[19]; c[3] = (d[17] - d[19]) * b24;
            c[4] = d[20] + d[22]; c[6] = (d[20] - d[22]) *  b8;
            c[5] = d[21] + d[23]; c[7] = (d[21] - d[23]) * b24;
            c[8] = d[24] + d[26]; c[10]= (d[24] - d[26]) *  b8;
            c[9] = d[25] + d[27]; c[11]= (d[25] - d[27]) * b24;
            c[12]= d[28] + d[30]; c[14]= (d[28] - d[30]) *  b8;
            c[13]= d[29] + d[31]; c[15]= (d[29] - d[31]) * b24;
         }

         {
            FLT b16 = MP3_b[16];

            d[16] = c[0] + c[1];
            d[17]= (c[0] - c[1])*b16;

            d[18] = c[2] + c[3];
            d[19]= (c[2] - c[3])*b16;

            d[20] = ( c[4] + c[5] ) + d[16]; 
            d[21] = ( c[4] - c[5] )*b16 + d[17];
            d[22]  = c[6] + c[7] + d[16] + d[18];
            d[23] = ( c[6] - c[7] )*b16 + d[16] + d[17] + d[19];

            d[24]  = c[8] + c[9];
            d[25] = (c[8] - c[9])*b16;
            d[26]  = c[10] + c[11] + d[24];
            d[27] = ( c[10] - c[11] )*b16 + d[24] + d[25];

            d[28]  = c[12] + c[13] - d[20];
            d[29] = (c[12] - c[13])*b16 + d[28]-d[21];

            d[30]  = c[14] + c[15] - d[22];
            d[31] = (c[14] - c[15]) * b16 - d[23];
         }
      }
#else
      ASM_Poly2( d );
#endif

            // step 6: final resolving & reordering 

      {
         FLT *u_p;

         u_p = (FLT*)&u[ch][!div][start];

         u_p[ 1*16] = -d[30];   
         u_p[ 3*16] = -d[22] + d[26];
         u_p[ 5*16] =  d[18] + d[20] - d[26];
         u_p[ 7*16] = -d[18] + d[28];
         u_p[ 9*16] = -d[28];
         u_p[11*16] = -d[20] + d[24];
         u_p[13*16] =  d[16] - d[24];
         u_p[15*16] = -d[16];

            // the other 32 are stored for use with the next granule

         u_p = (FLT*)&u[ch][div][start];

         u_p[15*16] =  d[31];
         u_p[13*16] =  d[23] - d[27];
         u_p[11*16] = -d[19] - d[20] - d[21] + d[27];
         u_p[ 9*16] =  d[19] - d[29];
         u_p[ 7*16] = -d[18] + d[29];
         u_p[ 5*16] =  d[18] + d[20] + d[21] - d[25] - d[26];
         u_p[ 3*16] = -d[17] - d[22] + d[25] + d[26];
         u_p[ 1*16] =  d[17] - d[30];
      }
   }

         //  we're doing dewindowing and calculating final samples now

   {
      const FLT *DeWin = MP3_DeWin_T[0] + 16 - start;
      FLT *u_ptr = (FLT*)&u[ch][div][0];
#ifdef UNIX
      INT j;

      for( j=16; j>0; --j )
      {
         FLT out;

         out  = u_ptr[ 0] * DeWin[0 ];
         out += u_ptr[ 1] * DeWin[1 ];
         out += u_ptr[ 2] * DeWin[2 ];
         out += u_ptr[ 3] * DeWin[3 ];
         out += u_ptr[ 4] * DeWin[4 ];
         out += u_ptr[ 5] * DeWin[5 ];
         out += u_ptr[ 6] * DeWin[6 ];
         out += u_ptr[ 7] * DeWin[7 ];
         out += u_ptr[ 8] * DeWin[8 ];
         out += u_ptr[ 9] * DeWin[9 ];
         out += u_ptr[10] * DeWin[10];
         out += u_ptr[11] * DeWin[11];
         out += u_ptr[12] * DeWin[12];
         out += u_ptr[13] * DeWin[13];
         out += u_ptr[14] * DeWin[14];
         out += u_ptr[15] * DeWin[15];
         PUT_SAMPLE(out);

         DeWin += 32;
         u_ptr+=16;
      }

      if (div & 0x1)
      {
         {        // slot #17 of u[2][2][]
            FLT out;

            out  = u_ptr[ 0] * DeWin[0 ];
            out += u_ptr[ 2] * DeWin[2 ];
            out += u_ptr[ 4] * DeWin[4 ];
            out += u_ptr[ 6] * DeWin[6 ];
            out += u_ptr[ 8] * DeWin[8 ];
            out += u_ptr[10] * DeWin[10];
            out += u_ptr[12] * DeWin[12];
            out += u_ptr[14] * DeWin[14];

            PUT_SAMPLE(out);
         }

         DeWin -= 48;
         DeWin += start;
         DeWin += start;

         for( j=15; j>0; --j )
         {
            FLT out;

            u_ptr -= 16;

            out = -u_ptr[ 0] * DeWin[15];
            out += u_ptr[ 1] * DeWin[14];
            out -= u_ptr[ 2] * DeWin[13];
            out += u_ptr[ 3] * DeWin[12];
            out -= u_ptr[ 4] * DeWin[11];
            out += u_ptr[ 5] * DeWin[10];
            out -= u_ptr[ 6] * DeWin[9 ];
            out += u_ptr[ 7] * DeWin[8 ];
            out -= u_ptr[ 8] * DeWin[7 ];
            out += u_ptr[ 9] * DeWin[6 ];
            out -= u_ptr[10] * DeWin[5 ];
            out += u_ptr[11] * DeWin[4 ];
            out -= u_ptr[12] * DeWin[3 ];
            out += u_ptr[13] * DeWin[2 ];
            out -= u_ptr[14] * DeWin[1 ];
            out += u_ptr[15] * DeWin[0 ];

            PUT_SAMPLE(out);
            DeWin -= 32;
         }
      }
      else
      {
         {          // slot #17 of u[2][2][]
            FLT out;

            out  = u_ptr[ 1] * DeWin[1 ];
            out += u_ptr[ 3] * DeWin[3 ];
            out += u_ptr[ 5] * DeWin[5 ];
            out += u_ptr[ 7] * DeWin[7 ];
            out += u_ptr[ 9] * DeWin[9 ];
            out += u_ptr[11] * DeWin[11];
            out += u_ptr[13] * DeWin[13];
            out += u_ptr[15] * DeWin[15];

            PUT_SAMPLE(out);
         }

         DeWin -= 48;
         DeWin += start;
         DeWin += start;

         for( j=15; j>0; --j )
         {
            FLT out;

            u_ptr -= 16;

            out  = u_ptr[ 0] * DeWin[15];
            out -= u_ptr[ 1] * DeWin[14];
            out += u_ptr[ 2] * DeWin[13];
            out -= u_ptr[ 3] * DeWin[12];
            out += u_ptr[ 4] * DeWin[11];
            out -= u_ptr[ 5] * DeWin[10];
            out += u_ptr[ 6] * DeWin[9 ];
            out -= u_ptr[ 7] * DeWin[8 ];
            out += u_ptr[ 8] * DeWin[7 ];
            out -= u_ptr[ 9] * DeWin[6 ];
            out += u_ptr[10] * DeWin[5 ];
            out -= u_ptr[11] * DeWin[4 ];
            out += u_ptr[12] * DeWin[3 ];
            out -= u_ptr[13] * DeWin[2 ];
            out += u_ptr[14] * DeWin[1 ];
            out -= u_ptr[15] * DeWin[0 ];

            PUT_SAMPLE(out);
            DeWin -= 32;
         }
      }
#else
      if (div & 0x1)
         ASM_dewin36_1( u_ptr, DeWin, Cur_Samples, (48-2*start)*4 );
      else
         ASM_dewin36_2( u_ptr, DeWin, Cur_Samples, (48-2*start)*4 );
      Cur_Samples += 32;
#endif
   }
   u_div[ch] ^= 0x01;
   u_start[ch] = (u_start[ch]-1) & 0x0f;
}

/*******************************************************/

static void Negate_Freqs( FLT *F, INT n )      // modifies *res_p
{
   for( ; n>0; n -= 2 )
   {
      INT i;
      for( i=-18; i<0; i+=2 ) F[i+18 +19] = -F[i+18 +19];
      F += 2*18;
   }
}

EXTERN void MP3_imdct( INT Block, INT Block_Type, INT n )
{
   INT Sub_Band, i;
   FLT *Win_bt;
   FLT res[32+1][18];  // 18 additional FLT for expansion

   s_p = s[n][0];
   res_p = res[0];
   In = MP3_Out[n][0];

   Win_bt = MP3_Wins[ Block_Type ];
   if ( Block==2 )     // short
   {
      for( Sub_Band=0; Sub_Band<MP3_imdcts[n]; ++Sub_Band )
      {
         MP3_Do_imdct_12pts( );  // n
         s_p += 18; res_p += 18; In += 18;
      }
   }
   else if ( Block==3 )   // mixed
   {
      MP3_Do_imdct_36pts( Win_bt, 0 );
      s_p += 18; res_p += 18; In += 18;
      MP3_Do_imdct_36pts( Win_bt, 1 );
      s_p += 18; res_p += 18; In += 18;
      for( Sub_Band=2; Sub_Band<MP3_imdcts[n]; ++Sub_Band )
      {
         MP3_Do_imdct_12pts( );
         s_p += 18; res_p += 18; In += 18;
      }
   }
   else for( Sub_Band=0; Sub_Band<MP3_imdcts[n]; ++Sub_Band )
   {
      MP3_Do_imdct_36pts( Win_bt, Sub_Band );
      s_p += 18; res_p += 18; In += 18;
   }

   i = (32-Sub_Band) * 18*sizeof( s_p[0] );
   if ( i ) 
#ifndef UNIX
   {
      ASM_Copy_And_Zero( res_p, s_p, i );
   }
#else
   {      
      memcpy( res_p, s_p, i );
      memset( s_p, 0, i );
   }
#endif

   Negate_Freqs( res[0], MP3_imdcts[n] );
   Cur_Samples = MP3_Out[n][0];
   for( res_p=res[0], i=18; i>0; --i, res_p++ ) MP3_Polyphase( n );
}

/*******************************************************/

#define PI12      0.261799387f
#define PI36      0.087266462f

EXTERN void MP3_Init_imdct( )
{
   INT i, j;

   for( i=0; i<36; i++ )  MP3_Win_0[i] = (FLT)sin( PI36 * (i+0.5) );
   for( i=0; i<18; i++ )  MP3_Win_1[i] = (FLT)sin( PI36 * (i+0.5) );
   for( i=18; i<24; i++ ) MP3_Win_1[i] = 1.0f;
   for( i=24; i<30; i++ ) MP3_Win_1[i] = (FLT)sin( PI12 * (i+0.5-18) );
   for( i=30; i<36; i++ ) MP3_Win_1[i] = 0.0f;
   for( i=0; i<6; i++ )   MP3_Win_3[i] = 0.0f;
   for( i=6; i<12; i++ )  MP3_Win_3[i] = (FLT)sin( PI12 * (i+0.5-6.0) );
   for( i=12; i<18; i++ ) MP3_Win_3[i] = 1.0f;
   for( i=18; i<36; i++ ) MP3_Win_3[i] = (FLT)sin( PI36 * (i+0.5) );
}

/*******************************************************/
/*******************************************************/
