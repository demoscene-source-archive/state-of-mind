/************************************************
 *           MP3 quantization                   *
 * heavily based on 'xamp' from Tomislav Uzelac *
 * Skal 98                                      *
 ************************************************/

#define _USE_PRIVATE_

// #include "config.h"
#include "main.h"
#include "bit_strm.h"
#include "./mp3.h"
#include "mp3quant.h"      // tables used only here

EXTERN INT  MP3_Id, MP3_SFreq, MP3_Mode, MP3_Ext;
EXTERN SHORT *MP3_Tab_l, *MP3_Tab_s;

EXTERN void (*MP3_Handle_Spectrum)( INT Nb_Chn, FLT Sp[2][32][18] ) = NULL;
EXTERN void (*MP3_Handle_Signal)( INT Len, const FLT *Left, const FLT *Right ) = NULL;
EXTERN FLT MP3_Out_Scale;

static USHORT *Re_Order;
static INT HUFF_Non_Zero[2];
#ifndef _SKIP_MPEG2_
static const FLT *Tab_is2_Ptr;
#endif

/*******************************************************/

      // we assume that a<MP3_MAX_GAIN (=46)

static FLT Quant_Long( 
   INT sfb, INT a,   // <= global gain
   INT Scalefac_Scale, INT Scalefac )
{
   if ( Scalefac_Scale ) a -= ( Scalefac<<2 );
   else a -= ( Scalefac<<1 );
   a += 127-210;
   if ( a<0 ) return( 0.0 );
   else return( MP3_Gain[a] );
}

static FLT Quant_Short( 
   INT a,      // <= global gain
   INT Sub_Block_Gain, INT Scalefac_Scale, INT Scalefac )
{
   a -= ( Sub_Block_Gain<<3 );
   if ( Scalefac_Scale ) a -= ( Scalefac<<2 );
   else a -= ( Scalefac<<1 );
   a += 127-210;
   if ( a<0 ) return( 0.0 );
   else return( MP3_Gain[a] );
}

#define Quant_Pow(i,a)  ( MP3_Pow[(i)+8192]*(a) )

/*******************************************************/
/*******************************************************/
/*                     MONO stuff                      */
/*******************************************************/
/*******************************************************/

static void MP3_Quant_Mono( MP3_INFO *Info, INT gr, INT ch )
{
   FLT a, *Out;
   INT Global_Gain, Scalefac_Scale;
   USHORT *Scale_l;
   INT *In;

   Global_Gain = Info->Global_Gain[gr][ch];
   Scalefac_Scale = Info->Scale_Factor_Scale[gr][ch];
   Scale_l = MP3_Scale_l[ch];
   Out = MP3_Out[ch][0];
   In = HUFF_IS[ch];

   if ( Info->Block[gr][ch]>=2 ) // short or mixed
   {
      INT l, Win_Len, sfb, Scalefac;

      if ( Info->Block[gr][ch]==3 )
      {
               // long part of mixed blocks

         sfb = 0; l = 0;
         Scalefac = Scale_l[sfb];
         a = Quant_Long( sfb, Global_Gain, Scalefac_Scale, Scalefac );

         while( l<36 )
         {
            Out[l] = Quant_Pow( In[l], a );
            if ( l==MP3_Tab_l[sfb] )
            {
               sfb++;
               Scalefac = Scale_l[sfb];
               a = Quant_Long( sfb, Global_Gain, Scalefac_Scale, Scalefac );
            }
            l++;
         }
         sfb = 3; // will now proceed short part of mixed blocks
         Win_Len = MP3_Tab_s[sfb] - MP3_Tab_s[sfb-1];
      }
      else     // short blocks
      {
         sfb = 0; l = 0;
         Win_Len = MP3_Tab_s[0] + 1;
      }

      while( l<HUFF_Non_Zero[ch] )  // short part
      {
         INT Win;
         for( Win=0; Win<3; ++Win )
         {
            INT i;
            INT Sub_Gain = Info->Sub_Block_Gain[gr][ch][Win];            
            Scalefac = MP3_Scale_s[ch][sfb][Win];
            a = Quant_Short( Global_Gain, Sub_Gain, Scalefac_Scale, Scalefac );
            for( i=Win_Len; i>0; --i )
            {
               Out[ Re_Order[l] ] = Quant_Pow( In[l], a );
               l++;
            }
         }
         sfb++;
         Win_Len = MP3_Tab_s[sfb] - MP3_Tab_s[sfb-1];
      }
      while( l<576 ) Out[ Re_Order[l++] ] = 0;
   }
   else  // Long blocks
   {
      INT Scalefac, sfb, l;   // , Preflag;

      sfb = 0; l = 0;
      // Preflag = Info->Preflag[gr][ch];
      Scalefac = Scale_l[sfb];
      if ( Info->Preflag[gr][ch] ) Scalefac += Pretab[sfb];    // Preflag
      a = Quant_Long( sfb, Global_Gain, Scalefac_Scale, Scalefac );

      while( l<HUFF_Non_Zero[ch] )
      {
         Out[l] = Quant_Pow( In[l], a );
         if ( l==MP3_Tab_l[sfb] )
         {
            sfb++;
            Scalefac = Scale_l[sfb];
            if ( Info->Preflag[gr][ch] ) Scalefac += Pretab[sfb];    // Preflag
            a = Quant_Long( sfb, Global_Gain, Scalefac_Scale, Scalefac );
         }
         l++;
      }
      while( l<576 ) Out[l++] = 0;
   }
}

/*******************************************************/   
/*******************************************************/
/*                   STEREO stuff                      */
/*******************************************************/
/*******************************************************/

#define MAX(a,b)  ( a>b?a:b )
#define IS_ILLEGAL 0xfeed

#define i_sq2  0.707106781188

   // ms_flag = 1 if MP3_Ext = 0,2,3
   // ms_flag = 0 if MP3_Ext = 1

static void Stereo_Long_Ok( INT l, FLT a[2], INT IS_Pos )
{
   FLT FTmp;
   FTmp = Quant_Pow( HUFF_IS[0][l], a[0] );

#ifdef _SKIP_MPEG2_

   MP3_Out[1][0][l] = Tab_is[IS_Pos]* FTmp;
   MP3_Out[0][0][l] = FTmp - Tab_is[IS_Pos]*FTmp;

#else

   if ( MP3_Id==1 )
   {
      MP3_Out[1][0][l] = Tab_is[IS_Pos]* FTmp;
      MP3_Out[0][0][l] = FTmp - Tab_is[IS_Pos]*FTmp;
   }
   else if ( IS_Pos&0x01 )
   {
      MP3_Out[0][0][l] = Tab_is2_Ptr[ (IS_Pos>>1)+1 ]* FTmp;
      MP3_Out[1][0][l] = FTmp;
   }
   else
   {
      MP3_Out[0][0][l] = FTmp;
      MP3_Out[1][0][l] = Tab_is2_Ptr[ IS_Pos>>1 ] * FTmp;
   }

#endif   // !_SKIP_MPEG2_
}

static void Stereo_Long_Illegal( INT l, FLT a[2] )
{
   FLT Mi, Si;
   Mi = Quant_Pow( HUFF_IS[0][l], a[0] );
   Si = Quant_Pow( HUFF_IS[1][l], a[1] );
   if ( MP3_Ext!=1 ) 
   {
      DBL Tmp; Tmp = (Mi+Si)*i_sq2; Si = (Mi-Si)*i_sq2; Mi = Tmp;
   }
   MP3_Out[0][0][l] = Mi;
   MP3_Out[1][0][l] = Si;
}

/*******************************************************/

static void Stereo_Short_Ok( INT l, FLT a[2], INT IS_Pos )
{
#ifdef _SKIP_MPEG2_

   FLT FTmp;
   FTmp = Quant_Pow( HUFF_IS[0][l], a[0] );
   MP3_Out[1][0][Re_Order[l]] = Tab_is[IS_Pos]* FTmp;
   MP3_Out[0][0][Re_Order[l]] = FTmp -Tab_is[IS_Pos]*FTmp;

#else
   if ( MP3_Id==1 )
   {
      FLT FTmp;
      FTmp = Quant_Pow( HUFF_IS[0][l], a[0] );
      MP3_Out[1][0][Re_Order[l]] = Tab_is[IS_Pos]* FTmp;
      MP3_Out[0][0][Re_Order[l]] = FTmp -Tab_is[IS_Pos]*FTmp;
   }
   else // MPEG2.0
   {
      FLT FTmp;

      FTmp = Quant_Pow( HUFF_IS[0][l], a[0] );
      if ( IS_Pos&0x01 )
      {
//
//       WARNING !!! Compiler's bug (gcc) !!! Can't use (IS_Pos>>1)+1
//       instead of (IS_Pos+1)>>1 !!! inlining fault?
//
//
         MP3_Out[0][0][Re_Order[l]] = Tab_is2_Ptr[ (IS_Pos+1)>>1 ]* FTmp;
         MP3_Out[1][0][Re_Order[l]] = FTmp;
      }
      else
      {
         MP3_Out[0][0][Re_Order[l]] = FTmp;
         MP3_Out[1][0][Re_Order[l]] = Tab_is2_Ptr[ IS_Pos>>1 ] * FTmp;
      }
   }
#endif   // !_SKIP_MPEG2_

}

static void Stereo_Short_Illegal( INT l, FLT a[2] )
{
   FLT Mi, Si;
   Mi = Quant_Pow( HUFF_IS[0][l], a[0] );
   Si = Quant_Pow( HUFF_IS[1][l], a[1] );
   if ( MP3_Ext!=1 ) 
   {
      FLT Tmp; Tmp = (Mi+Si)*i_sq2; Si = (Mi-Si)*i_sq2; Mi = Tmp;
   }
   MP3_Out[0][0][Re_Order[l]] = Mi;
   MP3_Out[1][0][Re_Order[l]] = Si;
}

/*******************************************************/
/*******************************************************/
#ifndef _SKIP_MPEG2_
static void Find_IS_Bound( INT IS_Bound[3], INT gr, MP3_INFO *Info )
{
   IS_Bound[0] = IS_Bound[1] = IS_Bound[2] = -1;

   if ( Info->Block[gr][0]>=2 )
   {
      INT sfb, Tmp;

      Tmp = HUFF_Non_Zero[1];
      sfb = 0;
      while( ( (3*MP3_Tab_s[sfb]+2)<Tmp ) && (sfb<12) ) sfb++;
      while( ( IS_Bound[0]<0 || IS_Bound[1]<0 || IS_Bound[2]<0 )
         && !(Info->Mixed_Block_Flag[gr][0] && sfb<3 ) && sfb )
         {
            INT Win, Win_Len;
            for( Win=2; Win>=0; --Win )
            {
               if ( IS_Bound[Win]>=0 ) continue;

               if ( sfb==0 ) 
               {
                  Win_Len = MP3_Tab_s[0]+1;
                  Tmp = (Win+1)*Win_Len - 1;
               }
               else 
               {
                  Win_Len = MP3_Tab_s[sfb] - MP3_Tab_s[sfb-1];
                  Tmp = ( 3*MP3_Tab_s[sfb-1]+2 ) + (Win+1)*Win_Len;
               }
               for( ; Win_Len>0; --Win_Len )
                  if ( HUFF_IS[1][Tmp--]!=0 )
                  {
                     IS_Bound[Win] = MP3_Tab_s[sfb] + 1;
                     break;
                  }
            }
            sfb--;
         }

            // mixed block

      if ( sfb==2 && Info->Mixed_Block_Flag[gr][0] )
      {
         INT Win;
         if ( IS_Bound[0]<0 && IS_Bound[1]<0 && IS_Bound[2]<0 )
         {
            Tmp = 35;
            while( HUFF_IS[1][Tmp]==0 ) Tmp--;
            sfb = 0;
            while( (MP3_Tab_l[sfb]<Tmp) && (sfb<21) ) sfb++;
            IS_Bound[0] = IS_Bound[1] = IS_Bound[2] = MP3_Tab_l[sfb]+1;
         }
         else for( Win=0; Win<3; ++Win )
            if ( IS_Bound[Win]<0 ) IS_Bound[Win] = 36;
      }
#ifndef _SKIP_MPEG2_
      if ( MP3_Id==1 )         
#endif
         IS_Bound[0] = IS_Bound[1] = IS_Bound[2] = MAX( IS_Bound[0], MAX(IS_Bound[1],IS_Bound[2] ) );
      Tmp = HUFF_Non_Zero[0];
      sfb = 0;
      while( ((3*MP3_Tab_s[sfb]+2)<Tmp) && (sfb<12) ) sfb++;
      MP3_imdcts[0] = MP3_imdcts[1] = ( MP3_Tab_s[sfb]-1 )/6 + 1;
   }
   else     // long block
   {
      INT sfb, Tmp;

      Tmp = HUFF_Non_Zero[1];
      while( HUFF_IS[1][Tmp]==0 ) Tmp--;
      sfb=0;
      while( (MP3_Tab_l[sfb]<Tmp) && (sfb<21) ) sfb++;
      IS_Bound[0] = IS_Bound[1] = IS_Bound[2] = MP3_Tab_l[sfb]+1;
      MP3_imdcts[0] = MP3_imdcts[1] = ( HUFF_Non_Zero[0]-1 )/18 + 1;
   }
}
#endif  // _SKIP_MPEG2_

/*******************************************************/

#define FETCH_COEFFS(c)                                               \
{                                                                     \
   Scalefac[(c)] = MP3_Scale_l[(c)][sfb];                             \
   if ( Preflag[(c)] ) Scalefac[(c)] += Pretab[sfb];                  \
   a[(c)] = Quant_Long( sfb, Global_Gain[(c)], Scalefac_Scale[(c)],   \
      Scalefac[(c)] );                                                \
}
#define FETCH_COEFFS_NO_PREFLAG(c)                                    \
{                                                                     \
   Scalefac[(c)] = MP3_Scale_l[(c)][sfb];                             \
   a[(c)] = Quant_Long( sfb, Global_Gain[(c)], Scalefac_Scale[(c)],   \
      Scalefac[(c)] );                                                \
}

static void MP3_Quant_Stereo( MP3_INFO *Info, INT gr )
{
   USHORT *Global_Gain, *Scalefac_Scale;
   INT IS_Bound[3], Sub_Block_Gain[2];   
   FLT a[2];

   Global_Gain = Info->Global_Gain[gr];
   Scalefac_Scale = Info->Scale_Factor_Scale[gr];

#ifndef _SKIP_MPEG2_
   if ( MP3_Ext==1 || MP3_Ext==3 )
   {
      Tab_is2_Ptr = Tab_is2[ Info->Scale_Factor_Compress[0][1] & 0x01 ];
      Find_IS_Bound( IS_Bound, gr, Info );
   }
   else
#endif
   if ( Info->Block[gr][0]<=1 )   // long
   {
      IS_Bound[0] = IS_Bound[1] = IS_Bound[2] = MAX( HUFF_Non_Zero[0], HUFF_Non_Zero[1] );
      MP3_imdcts[0] = MP3_imdcts[1] = ( IS_Bound[0]-1 )/18 + 1;
   }
   else  // short/mixed
   {
      INT sfb, Tmp;
      IS_Bound[0] = IS_Bound[1] = IS_Bound[2] = 576;
      Tmp = (MAX( HUFF_Non_Zero[0], HUFF_Non_Zero[1] )) / 3;
      sfb = 0; while( (MP3_Tab_s[sfb]<Tmp) && (sfb<12) ) sfb++;
      MP3_imdcts[0] = MP3_imdcts[1] = ( MP3_Tab_s[sfb]-1 )/6 + 1;
   }

   if ( Info->Block[gr][0]>=2 )
   {
      INT Win, Win_Len, sfb, l, lmax;

      if ( Info->Block[gr][0]==3 )
      {
            // long blocks of mixed stereo block
         INT Scalefac[2];
         l = 0; sfb = 0;
         FETCH_COEFFS_NO_PREFLAG(0);
         FETCH_COEFFS_NO_PREFLAG(1);

         while( l<36 )
         {
            INT IS_Pos;

            if ( l<IS_Bound[0] )
            {
Illegal_l:
               Stereo_Long_Illegal( l, a );
            }
            else {
               IS_Pos = Scalefac[1];
#ifdef _SKIP_MPEG2_
               if ( IS_Pos==7 ) goto Illegal_l;
#else
               if ( MP3_Id==1 ) { if ( IS_Pos==7 ) goto Illegal_l; }
               else { if ( IS_Pos==HUFF_IS_Max[sfb] ) goto Illegal_l; }
#endif                 
               Stereo_Long_Ok( l, a, IS_Pos );
            }

            if ( l==MP3_Tab_l[sfb] )
            {
               sfb++;
               FETCH_COEFFS_NO_PREFLAG(0);
               FETCH_COEFFS_NO_PREFLAG(1);
            }
            l++;
         }            
         sfb = 3; // will proceed short blocks of mixed stereo block
         Win_Len = MP3_Tab_s[sfb] - MP3_Tab_s[sfb-1];
      }
      else
      {
            // short blocks
         sfb = 0; l = 0;
         Win_Len = MP3_Tab_s[0] + 1;
      }

      lmax = MAX(HUFF_Non_Zero[0], HUFF_Non_Zero[1]);
      while( l<lmax )
      {
         for( Win=0; Win<3; ++Win )
         {
            INT i, IS_Pos;
            INT Scalefac[2];
            Sub_Block_Gain[0] = Info->Sub_Block_Gain[gr][0][Win];
            Sub_Block_Gain[1] = Info->Sub_Block_Gain[gr][1][Win];
            Scalefac[0] = MP3_Scale_s[0][sfb][Win];
            Scalefac[1] = MP3_Scale_s[1][sfb][Win];
            a[0] = Quant_Short( Global_Gain[0], Sub_Block_Gain[0],
               Scalefac_Scale[0], Scalefac[0] );

            if ( MP3_Tab_s[sfb] < IS_Bound[Win] )
            {
               a[1] = Quant_Short( Global_Gain[1], Sub_Block_Gain[1],
                  Scalefac_Scale[1], Scalefac[1] );
Illegal:
               for( i=Win_Len; i>0; --i )
               {
                  Stereo_Short_Illegal( l, a );
                  l++;
               }
            }
            else {
               IS_Pos = Scalefac[1];
#ifdef _SKIP_MPEG2_
               if ( IS_Pos==7 ) goto Illegal;
#else
               if ( MP3_Id==1 ) { if ( IS_Pos==7 ) goto Illegal; }
               else { if ( IS_Pos==HUFF_IS_Max[sfb+6] ) goto Illegal; }
#endif                  

               for( i=Win_Len; i>0; --i )
               {
                  Stereo_Short_Ok( l, a, IS_Pos );                  
                  l++;
               }
            }
         }
         Win_Len = MP3_Tab_s[sfb+1] - MP3_Tab_s[sfb];
         sfb++;
      }
      while( l<576 )
      {
         INT L = (INT)Re_Order[ l++ ];
         MP3_Out[0][0][L] = MP3_Out[1][0][L] = 0;
      }
   }
   else              // long blocks
   {
      INT sfb, l, lmax;
      BYTE *Preflag;
      INT Scalefac[2];

      Preflag = Info->Preflag[gr];

      sfb = 0; l = 0;
      FETCH_COEFFS(0);
      FETCH_COEFFS(1);
      while( l<IS_Bound[0] )
      {
         Stereo_Long_Illegal( l, a );
         if ( l==MP3_Tab_l[sfb] )
         {
            sfb++;
            FETCH_COEFFS(0);
            FETCH_COEFFS(1);
         }
         l++;
      }

      lmax = MAX(HUFF_Non_Zero[0], HUFF_Non_Zero[1]);
      while( l<lmax )
      {
         INT IS_Pos;

         IS_Pos = Scalefac[1];
#ifdef _SKIP_MPEG2_
         if ( IS_Pos==7 ) Stereo_Long_Illegal( l, a );
         else Stereo_Long_Ok( l, a, IS_Pos );
#else
         if ( MP3_Id==1 )
         {
            if ( IS_Pos==7 ) Stereo_Long_Illegal( l, a );
            else Stereo_Long_Ok( l, a, IS_Pos );
         }
         else
         {
            if ( IS_Pos==HUFF_IS_Max[sfb] ) Stereo_Long_Illegal( l, a );
            else Stereo_Long_Ok( l, a, IS_Pos );
         }
#endif

         if ( l==MP3_Tab_l[sfb] )
         {
            sfb++;
            Scalefac[1] = MP3_Scale_l[1][sfb];
            FETCH_COEFFS(0);
         }
         l++;
      }
      while( l<576 )
      {
         MP3_Out[0][0][l] = MP3_Out[1][0][l] = 0;
         l++;
      }
   }
}

/*******************************************************/
/*******************************************************/

#ifdef UNIX

static FLT Cs[8] = {
   0.857492925712, 0.881741997318, 0.949628649103, 0.983314592492, 
   0.995517816065, 0.999160558175, 0.999899195243, 0.999993155067
};
static FLT Ca[8] = {
   -0.5144957554270, -0.4717319685650, -0.3133774542040, -0.1819131996110,
   -0.0945741925262, -0.0409655828852, -0.0141985685725,	-0.00369997467375
};

static void MP3_Alias_Reduction( FLT *x )
{
   INT i;
   for ( i=30; i>=0; i--, x+=18 )   // requant of subbands [1-32]
   {   	
	   FLT Tmp;

      Tmp = x[0]*Cs[0] + x[-1]*Ca[0];
      x[-1] = x[-1]*Cs[0] - x[0]*Ca[0];
      x[0] = Tmp;

      Tmp = x[1]*Cs[1] + x[-2]*Ca[1];
      x[-2] = x[-2]*Cs[1] - x[1]*Ca[1];
      x[1] = Tmp;

      Tmp = x[2]*Cs[2] + x[-3]*Ca[2];
      x[-3] = x[-3]*Cs[2] - x[2]*Ca[2];
      x[2] = Tmp;

      Tmp = x[3]*Cs[3] + x[-4]*Ca[3];
      x[-4] = x[-4]*Cs[3] - x[3]*Ca[3];
      x[3] = Tmp;

      Tmp = x[4]*Cs[4] + x[-5]*Ca[4];
      x[-5] = x[-5]*Cs[4] - x[4]*Ca[4];
      x[4] = Tmp;

      Tmp = x[5]*Cs[5] + x[-6]*Ca[5];
      x[-6] = x[-6]*Cs[5] - x[5]*Ca[5];
      x[5] = Tmp;

      Tmp = x[6]*Cs[6] + x[-7]*Ca[6];
      x[-7] = x[-7]*Cs[6] - x[6]*Ca[6];
      x[6] = Tmp;

      Tmp = x[7]*Cs[7] + x[-8]*Ca[7];
      x[-8] = x[-8]*Cs[7] - x[7]*Ca[7];
      x[7] = Tmp;

	}
}
#else

extern  void MP3_Alias_Reduction( FLT *x );

#endif

/*******************************************************/
/*         public dequantization entries               */
/*******************************************************/

EXTERN void MP3_MPEG1( MP3_INFO *Info, INT Nb_Chn )
{
   INT n, gr;     // MP3_Id = 1

//   UINT _T1,_T2, _T3, _T4, _T;
//   _T1=_T2=_T3=_T4 = 0;

   Re_Order = MP3_Re_Order[ 1 ][ MP3_SFreq ];

   for( gr=0; gr<2; gr++ )       // MPEG1.0
   {
      MP3_imdcts[0] = MP3_imdcts[1] = 32;
      for( n=0; n<Nb_Chn; ++n )
      {
         INT End;
         End = MP3_Bit_Pos + Info->Part2_3_Len[gr][n];
//         _T = _Clock( );
         MP3_Decode_Scale_Factors_MPEG1( Info, gr, n );
//         _T1 += _Clock()-_T;
//         _T = _Clock( );
         HUFF_Non_Zero[n] = MP3_Huffman_Decode( Info, gr, n, End );
//         _T2 += _Clock()-_T;
      }
//      _T = _Clock( );
      if ( ( MP3_Mode!=1 ) || ( MP3_Mode==1 && MP3_Ext==0 ) )
         for( n=0; n<Nb_Chn; ++n ) MP3_Quant_Mono( Info, gr, n );
      else MP3_Quant_Stereo( Info, gr );  // real stereo
//      _T3 += _Clock()-_T;
//      _T = _Clock( );
      if ( Info->Block[gr][0]<=1 ) MP3_Alias_Reduction( MP3_Out[0][1] ); // long
      if( Nb_Chn==2 )
      {
         if ( Info->Block[gr][1]<=1 ) MP3_Alias_Reduction( MP3_Out[1][1] ); // long

         if ( MP3_Handle_Spectrum!=NULL ) (*MP3_Handle_Spectrum)( Nb_Chn, MP3_Out );

         MP3_imdct( Info->Block[gr][0], Info->Block_Type[gr][0], 0 );
         MP3_imdct( Info->Block[gr][1], Info->Block_Type[gr][1], 1 );
         if ( MP3_Handle_Signal!=NULL ) (*MP3_Handle_Signal)( 18*32, (const FLT *)MP3_Out[0][0], (const FLT *)MP3_Out[1][0] );
         
      }
      else
      {
         if ( MP3_Handle_Spectrum!=NULL ) (*MP3_Handle_Spectrum)( Nb_Chn, MP3_Out );
         MP3_imdct( Info->Block[gr][0], Info->Block_Type[gr][0], 0 );
         if ( MP3_Handle_Signal!=NULL ) (*MP3_Handle_Signal)( 18*32, (const FLT *)MP3_Out[0][0], NULL );
      }
//      _T4 += _Clock()-_T;
   }
//   fprintf( stderr, "Decode_Scale:%d t \tHuff:%d t \t Quant:%d \t imdct:%d     \t\t\r",
//      _T1, _T2, _T3, _T4 );
}

/*******************************************************/

#ifndef _SKIP_MPEG2_ 

EXTERN void MP3_MPEG2( MP3_INFO *Info, INT Nb_Chn )
{
   INT n;      // MP3_Id = 0

   Re_Order = MP3_Re_Order[ 0 ][ MP3_SFreq ];
   MP3_imdcts[0] = MP3_imdcts[1] = 32;

           // only one granule for MPEG2.0    gr=0

   for( n=0; n<Nb_Chn; ++n )
   {
      INT End;
      End = MP3_Bit_Pos + Info->Part2_3_Len[0][n];
      MP3_Decode_Scale_Factors_MPEG2( Info, n );
      HUFF_Non_Zero[n] = MP3_Huffman_Decode( Info, 0, n, End );
   }
   if ( ( MP3_Mode!=1 ) || ( MP3_Mode==1 && MP3_Ext==0 ) )
      for( n=0; n<Nb_Chn; ++n )
         MP3_Quant_Mono( Info, 0, n );
   else MP3_Quant_Stereo( Info, 0 );      // real stereo

   if ( Info->Block[0][0]<=1 ) MP3_Alias_Reduction( MP3_Out[0][1] );

   if( Nb_Chn==2 )
   {
      if ( Info->Block[0][1]<=1 ) MP3_Alias_Reduction( MP3_Out[1][1] );
      if ( MP3_Handle_Spectrum!=NULL ) (*MP3_Handle_Spectrum)( Nb_Chn, MP3_Out );

      MP3_imdct( Info->Block[0][0], Info->Block_Type[0][0], 0 );
      MP3_imdct( Info->Block[0][1], Info->Block_Type[0][1], 1 );
      if ( MP3_Handle_Signal!=NULL ) (*MP3_Handle_Signal)( 18*32, (const FLT *)MP3_Out[0][0], (const FLT *)MP3_Out[1][0] );
   }
   else
   {
      if ( MP3_Handle_Spectrum!=NULL ) (*MP3_Handle_Spectrum)( Nb_Chn, MP3_Out );
      MP3_imdct( Info->Block[0][0], Info->Block_Type[0][0], 0 );
      if ( MP3_Handle_Signal!=NULL ) (*MP3_Handle_Signal)( 18*32, (const FLT *)MP3_Out[0][0], NULL );
   }
}
#endif   // !_SKIP_MPEG2_

/*******************************************************/

extern double pow( double, double );
EXTERN void MP3_Quant_Init( )
{
   INT i;

   MP3_Pow[8192] = 0.0;
   for( i=1; i<8192; ++i )
   {
      FLT x = (FLT)pow( 1.0*i, 1.33333333333 );
      MP3_Pow[8192+i] = x;
      MP3_Pow[8192-i] = -x;
   }

   for( i=-127; i<MP3_MAX_GAIN; ++i )
   {
      if ( i ) MP3_Gain[i+127] = (FLT)pow( 2.0, 0.25*i );
      else MP3_Gain[127+i] = 1.0f;
   }
}

/*******************************************************/
