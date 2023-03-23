/*
 * - State of Mind -
 *
 * Skal98 (skal.planet-d.net)
 ********************************************/

#include "demo.h"

/****** Timing params ******/

//
//               BEAT ~= 3800
//

PHASE Phases[ ] =
{
  {     0, 1, 0,  Init_00,       Loop_Deform_Blur_II, NULL,  NULL,        0,0, 0 },
  {  5800, 1, 0,  NULL,          Loop_14_Anim_Mask,   Clear_DMask,  NULL, 162,0, 0 },
  {  7300, 1, 0,  Init_Dissolve, Do_Dissolve,       NULL,  NULL,        207,0, 0 },
  {  7900, 1, 8,  Init_00,       Loop_14_Anim_Mask_Move,     Clear_DMask,  NULL, 221,0, 0 },

      // a world of technology
  {  9100, 16, 1, Init_01,       Loop_6_02,      NULL,  NULL,  257,0, 0 },
  {  9150, 0, 0,  Init_Dissolve, Do_Dissolve_X,  NULL,  NULL,  257,0, 0 },
  {  9300, 0, 0,  NULL,    Loop_6_02,            NULL,  NULL,  257,0, 0 },
  { 10900, 0, 1,  Init_Dissolve,    Do_Dissolve_Y, NULL, NULL, 306,0, 0 },
  { 11200, 0, 0,  NULL,    Loop_6_02,            NULL,  NULL,  321,0, 0 },
  { 12400, 0, 2,  Init_Dissolve,    Do_Dissolve_X2, NULL, NULL, 356,0, 0 },
  { 12600, 0, 0,  NULL,    Loop_6_02,     Clear_DMask,  NULL,  368,0, 0 },

      // + beat   (+titles)
  { 13900, 0, 2,    Init_01,     Loop_6_06,     NULL, NULL,         446,0, 0 },
  { 17500, 0, 3,    Init_01,     Loop_6_08,     Clear_DMask,  NULL, 489,0, 0 },
  { 19450, 0, 0,    NULL,        Loop_6_06,     NULL,  NULL,        541,0, 0 },
  { 22200, 0, 4,    Init_01,     Loop_6_08,     Clear_DMask,  NULL, 619,0, 0 },
  { 24000, 0, 0,    NULL,        Loop_6_06,     NULL,  NULL,        669,0, 0 },
  { 26500, 0, 6,    Init_01,     Loop_6_08,     Clear_DMask,  NULL, 738,0, 0 },
  { 28000, 0, 0,    NULL,        Loop_6_06,     NULL,  NULL,        781,0, 0 },
  { 31500, 0, 7,    Init_01,     Loop_6_08,     Clear_DMask,  NULL, 875,0, 0 },
  { 32300, 0, 0,    Init_Blur_I, Loop_Deform_Blur_I,   NULL,  NULL, 897,0, 0 },

      // dangerous stuff inside
  { 34500, 5, 8,  Init_01,  Loop_6_05,   NULL,  NULL,       961,0, 0 },
  { 34600, 0, 0,  Init_Dissolve, Do_Dissolve_X, NULL,NULL,  961,0, 0 },
  { 35200, 0, 0,  NULL,     Loop_6_05,   NULL, NULL,        982,0, 0 },
  { 35600, 0, 0,  Init_Dissolve, Do_Dissolve_X, NULL,NULL,  1000,0, 0 },
  { 36200, 0, 0,  NULL,     Loop_6_05,   Close_6, NULL,     1020,0, 0 },
  { 41000, 0, 1,  Init_Blur_I, Loop_Deform_Blur_I, NULL,  NULL, 1138,0, 0 },

  { 42100, 1, 1,  Init_World3,  Loop_W3_03,  Close_World3,  NULL,   1172,0, 0 },

      // work + buy + consume + die
  { 49300, 1, 1,  Init_00,   Loop_Appear_II,         Close_Appear_II, NULL, 1371,0, 0 },
  { 50100, 1, 0,  NULL,   Loop_14_Anim_Mask_Move_II,  NULL,  NULL,          1394,0, 0 },
  { 53000, 1, 2,  Init_00,   Loop_14_Anim_Mask_Move, Clear_DMask,  NULL,    1473,0, 0 },
  { 53500, 1, 9,  Init_00,   Loop_Disappear,         Close_Disappear, NULL, 1487,0, 0 },

  { 53900, 1, 3,  Init_00,   Loop_Appear_II,             Close_Appear_II, NULL, 1501,0, 0 },
  { 54500, 1, 0,  NULL,      Loop_14_Anim_Mask_Move_II,  Clear_DMask,  NULL,    1515,0, 0 },

  { 56600, 1,10,  Init_00,   Loop_Appear_II,               Close_Appear_II, NULL,  1572,0, 0 },
  { 57100, 1, 0,  NULL,      Loop_14_Anim_Mask_Move_Sprites,  Clear_DMask,  NULL,  1586,0, 0 },

  { 59000, 1,11,  Init_00,   Loop_Appear_II,               Close_Appear_II, NULL,  1643,0, 0 },
  { 59300, 1, 0,  NULL,      Loop_14_Anim_Mask_Move_Sprites,  Clear_DMask_And_Anim,  NULL,  1648,0, 0 },
  { 60900, 1, 9,  Init_00,   Loop_Disappear,         Close_Disappear, NULL,        1693,0, 0 },

      // powered + scratch + imagine
  { 61100, 1, 7,  Init_00,   Loop_14_Powered,   Close_Blur_II,  NULL,     1700,0, 0 },

      // world II  < a)Blur (b)Normal >
  { 63900, 2, 0,    Init_04,  Loop_W2_01,  NULL,  NULL,   1778,0, 0 },
  { 65000, 0, 1,    Init_04,  Loop_W2_03,  Close_World2,  NULL,   1805,0, 0 },
  { 88700, 1, 9,    Init_00,  Loop_Disappear, Destroy_Appear, NULL, 2466,0, 0 },

      // blobz     (+deform_blur)
  { 89500, 4, 0, Init_02,     Loop_Anim_Blb_I,    NULL,  NULL,      2489,0, 0 },
  { 96200, 0, 0, Init_10,     Loop_10,            NULL,  NULL,      2674,0, 0 },
  { 98200, 0, 0, Init_Blb,    Loop_Anim_Blb_I,    Close_Blb,  NULL, 2731,0, 0 },
  {105000, 0, 0, Init_Blur_I, Loop_Deform_Blur_I, Clear_DMask_And_Anim,  NULL,      2916,0, 0 },

      // galaxy
  {106300, 9, 0,  Init_Ptcl, Loop_Ptcl_I,     NULL, NULL,  2957,0, 0 },
  {111500, 0, 1,  Init_Ptcl, Loop_Ptcl_II,    NULL, NULL,  3099,0, 0 },
  {113000, 0, 2,  Init_Ptcl, Loop_Ptcl_I,     NULL, NULL,  3137,0, 0 },
  {114000, 0, 3,  Init_Ptcl, Loop_Ptcl_II,    NULL, NULL,  3165,0, 0 },
  {115500, 0, 4,  Init_Ptcl, Loop_Ptcl_I,     NULL, NULL,  3208,0, 0 },
  {117000, 0, 5,  Init_Ptcl, Loop_Ptcl_II,    NULL, NULL,  3250,0, 0 },
  {118500, 0, 6,  Init_Ptcl, Loop_Ptcl_I,     NULL, NULL,  3293,0, 0 },
  {119000, 0, 7,  Init_Ptcl, Loop_Ptcl_II,    NULL, NULL,  3305,0, 0 },
  {121300, 0, 8,  Init_Ptcl, Loop_Ptcl_I,     Close_Ptcl, NULL, 3371,0, 0 },

      // ->no way
  {122800, 2, 2,    Init_03, Loop_14_No_Way,     Clear_DMask, NULL,  3414,0, 0 },
  {126300, 0, 3,    Init_03, Loop_14_No_Way_II,  NULL, NULL,         3511,0, 0 },
  {134000, 0,0,     NULL,    Loop_Deform_Blur_IV, NULL, NULL,         3600,0, 0 },

      // logo
  {134500, 2, 0, Init_03,  Loop_D2_Destroy, NULL, NULL,     3755,0, 0 },
  {135800, 0, 0, NULL,     Loop_D2,         Close_D2, NULL,     3803,0, 0 },
  {152000, 1, 1, Init_03,  Loop_14_Anim_Mask_Sat, NULL, NULL, 4225,0, 0 },
  {153600, 0, 1, Init_Dissolve, Do_Dissolve_X2, Close_Caustics, NULL, 4267,0, 0 },

      // ready to take off?    
      // Warning!!! "Clear_DMask()" here!
  {154400,0,11,  Init_03,  Loop_14_Ready, Clear_DMask_And_Anim,  NULL,   4289,0, 0 },
  {161000,0,0,   NULL,     Loop_Deform_Blur_IV,   NULL, NULL,   4300,0, 0 },

      // greets
  {161500, 1, 0,  Init_Greets, Loop_W4_02, Clear_DMask,  NULL,   4488,0, 0 },
  {169100, 1, 1,  Init_Greets, Loop_W4_02, Clear_DMask,  NULL,   4699,0, 0 },
  {178300, 1, 2,  Init_Greets, Loop_W4_02, Close_World4,  NULL,  4955,0, 0 },
  {185500, 0, 0,  Init_Blur_I, Loop_Deform_Blur_I, NULL,  NULL,  5154,0, 0 },

      // Behind / blur
  {186700, 1, 0, Init_05,   Loop_Deform_Blur_III,   NULL, NULL,  5185,0, 0 },
  {201000, 0, 1, Init_05,   Loop_0,                 NULL, NULL,  5583,0, 0 },

  {210000, 0, 0, NULL, Loop_Fade, NULL,  NULL,  5613,0, 0 },

      // Bomb
  {211000, 1,13, Init_03,  Loop_14_End, Clear_DMask_And_Anim,  NULL, 5613,0, 0 },

      // last void phase. fake. Don't change.
  {218200, 0, 0, NULL, Loop_Fade, NULL,  NULL,  5613,0, 0 },
  {219200, 0, 0, NULL, Loop_0, NULL, NULL,  5613,0, 0 },  
  {    -1, 0, 0, NULL, Loop_0, NULL, NULL,  5613,0, 0 },  

  { 0 }

};

EXTERN INT LAST_PHASE = sizeof( Phases )/sizeof(Phases[0]);


/*******************************************************************/
/*******************************************************************/
