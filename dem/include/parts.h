/*
 * parts
 *
 * Skal 98 (skal.planet-d.net)
***************************************/

/*******************************************************************/

extern void Loop_0( );

/*******************************************************************/

extern void Pre_Cmp_1( INT );
extern void Init_1( INT Param );
void Init_1_2( INT Param );
extern void Loop_2_2( );
extern void Loop_2_3( );
extern void Loop_2( );

extern void Close_1( );

/*******************************************************************/

extern void Pre_Cmp_2( INT );
extern void Init_2( INT Param );
extern void Loop_2_1( );
extern void Close_2( );

/*******************************************************************/

extern void Pre_Cmp_3( INT );
extern void Init_3( INT Param );
extern void Loop_3_Raw( );
extern void Close_3( );

/*******************************************************************/

extern void Pre_Cmp_4( INT );
extern void Init_4( INT Param );
extern void Loop_4( );
extern void Loop_4_2( );
extern void Close_4( );

/*******************************************************************/

extern void Pre_Cmp_5( INT );
extern void Init_5( INT Param );
extern void Loop_5( );
extern void Close_5( );

/*******************************************************************/

extern void Pre_Cmp_8( INT );
extern void Init_8( INT Param );
extern void Loop_8_1( );
extern void Loop_8_2( );
extern void Loop_8_3( );
extern void Close_8( );

/*******************************************************************/

#if 0
extern void Copy_Buffer_Mask( BYTE *Dst, INT Dst_BpS,
   BYTE *Src, INT Src_BpS, INT W, INT H, BYTE Off );
extern void Copy_Buffer_Mask_III( BYTE *Dst, INT Dst_BpS,
   BYTE *Src, INT Src_BpS, INT W, INT H, BYTE Off );
extern void Copy_Buffer_Mask_II( BYTE *Dst, INT Dst_BpS,
   BYTE *Src, INT Src_BpS, INT W, INT H );
#endif

// extern BYTE Mask_Map[256], Mask_Map_II[256];
// extern void Init_Mask_Map_I( ), Init_Mask_Map_II( );
// extern void Do_Masked_Copy( INT Cnt, BYTE *Where, INT BpS, 
//   BYTE *Src, INT W, INT H, INT Src_BpS );

// extern BYTE *Plasma_Src2;

extern void Init_Dissolve( INT Param );
extern void Do_Dissolve( );
extern void Init_Dissolve_II( INT Param );
extern void Do_Dissolve_II( );
extern void Do_Dissolve_X( );
extern void Do_Dissolve_X2( );
extern void Do_Dissolve_Y( );
extern void Pre_Cmp_9( INT );
extern void Init_9( INT Param );
extern void Init_9_2( INT Param );
extern void Init_9_3( INT Param );
extern void Loop_9_1( );
extern void Loop_9_2( );
extern void Loop_9_4( );
extern void Close_9( );

/*******************************************************************/

extern void Loop_Lines( );
extern void Loop_Lines_Inv_Rot( );
extern void Loop_Lines_No_Rot( );

/*******************************************************************/

extern void Pre_Cmp_10( INT );
extern void Init_10( INT Param );
// extern void Init_Loop_10_Sprites( );
// extern void Loop_10_Sprites( FLT x );
extern void Loop_10( );
extern void Close_10( );

/*******************************************************************/

extern void Pre_Cmp_11( INT );
extern void Init_11( INT Param );
extern void Loop_11_Raw( );
extern void Loop_11_1( );
extern void Loop_11_2( );
extern void Close_11( );

/*******************************************************************/

extern void Pre_Cmp_12( INT );
extern void Init_12( INT Param );
extern void Loop_12_1( );
extern void Loop_12_2( );
extern void Close_12( );

/*******************************************************************/

extern void Bi_Tunnel( PIXEL *Dst, PIXEL *Env_Map_Bits,
   USHORT *Map1, USHORT *Map2 );

extern void Pre_Cmp_13( INT );
extern void Init_13( INT Param );
extern void Loop_13_1( );
extern void Loop_13_2( );
extern void Loop_13_3( );
extern void Loop_13_4( );
extern void Close_13( );

/*******************************************************************/

extern void Pre_Cmp_14( INT );
extern void Loop_14_Anim_Smooth( );
extern void Loop_14_Anim( );
// extern void Loop_14_Anim_Destroy( );
// extern void Loop_14_Anim_Flash( );
extern void Loop_14_Anim_Mask( );
extern void Loop_14_Anim_Mask_Raw( );
extern void Loop_14_Anim_Mask_Sat( );
extern void Loop_14_Anim_Mask_Move( );
extern void Loop_14_Anim_Mask_Move_II( );
extern void Loop_14_Anim_Mask_Move_Sprites( );
extern void Loop_14_Anim_Mask_Sat_2( );
extern void Loop_14_Anim_Mask_Dissolve( );
extern void Loop_14_Blur_I( );
extern void Loop_14_Blur_II( );
extern void Loop_14_Welcome( );
extern void Loop_14_Powered( );
extern void Loop_14_Powered_II( );
extern void Loop_14_No_Way( );
extern void Loop_14_No_Way_II( );
extern void Loop_14_Ready( );
extern void Loop_14_End( );

#if 0
   // various hack

void Loop_14_Anim_Mask_Sat_Lines( );
void Loop_14_Anim_Mask_Sat_Lines_Out( );

   // in spectrum.c
extern void Init_Spectrum( void *Strm );
extern void Close_Spectrum( );
extern void Do_Spectrum( VBUFFER *Out );

#endif

/*******************************************************************/

   // in part2.c

extern SPRITE *Sprs_Glxy[4];
extern BITMAP *Sky;
extern void Set_Bck( PIXEL *Dst, PIXEL *Map );
extern MATRIX2 Ptcl_M;

extern void Pre_Cmp_Ptcl( );
extern void Init_Ptcl( INT Param );
extern void Loop_Ptcl_I( );
extern void Loop_Ptcl_II( );
extern void Loop_Ptcl_III( );
extern void Loop_Ptcl_IV( );
extern void Close_Ptcl( );

/*******************************************************************/

extern void Pre_Cmp_6( );
extern void Init_6( );
extern void Loop_6_1( );
extern void Loop_6_2( );
extern void Loop_6_3( );
extern void Loop_6_4( );
extern void Loop_6_5( );
extern void Loop_6_6( );
extern void Close_6( );

extern void Loop_6_02( );  // tunnel
extern void Loop_6_05( );  // mixed clipping + tunnel + Mask
extern void Loop_6_06( );  // Tunnel + scratch + beats
extern void Loop_6_08( );  // Tunnel + scratch + VMask

extern void Init_6_zBuf( );
extern void Loop_6_zBuf( );

#define USE_FLARE
extern BITMAP *Flare;
extern FLARE Light_Flare;
extern FLARE_SPRITE Flare_Sprite[];

extern SHADER_METHODS Shader1;
extern void Set_UV_Shader_Func( OBJ_NODE *Obj );

/*******************************************************************/

   // in world2.c

extern void Pre_Cmp_World2( );
extern void Init_World2( );
extern void Close_World2( );
extern void Loop_W2_01( );  // world2 + Deform_Blur
extern void Loop_W2_02( );  // world2
extern void Loop_W2_03( );  // world2 + Lines
extern void Loop_Lines( );
extern void Loop_Lines_Inv_Rot( );

/*******************************************************************/

   // in world3.c


extern void Pre_Cmp_World3( );
extern void Init_World3( INT );
extern void Close_World3( );
// extern void Loop_W3_01( );
extern void Loop_W3_02( );
extern void Loop_W3_03( );

/*******************************************************************/

   // in world4.c

extern void Pre_Cmp_World4( );
extern void Init_World4( INT );
extern void Close_World4( );
extern void Loop_W4_Shad( );
extern void Loop_W4_02( );

/*******************************************************************/

extern void Pre_Cmp_7( INT );
extern void Init_7( INT Param );
extern void Loop_7_1( );
extern void Loop_7_2( );
extern void Loop_7_2_Blur( );

extern void Loop_7_3( );
extern void Loop_7_4( );
extern void Loop_7_5( );
extern void Close_7( );

/*******************************************************************/

   // in part1.c

extern void Init_D2( INT Param );
extern void Loop_D2( );
extern void Loop_D2_Destroy( );
extern void Close_D2( );

extern BYTE UScroll;
extern void Scroll_1( );
extern void Scroll_2( );
extern void Scroll_3( INT n );
extern void Close_Scroll( );
extern void Init_Scroll( INT Param );
;

/*******************************************************************/

   // in blb.c

extern void Init_Blb( INT Param );
extern void Init_Blb_I( INT Param );
extern void Loop_Blb( );
extern void Close_Blb( );
extern void Loop_Anim_Blb_I( );
extern void Loop_Anim_Blb_II( );
extern void Loop_Anim_Obj_1( );
extern void Loop_Anim_Obj_2( );
extern void Loop_Anim_Obj_3( );
extern void Loop_Anim_Obj_4( );

#if 0
extern void Setup_Obj( );
extern void Render_Obj( OBJ_NODE *Node );
extern void Set_Node_Rot( OBJ_NODE *Node, FLT Rot );
extern void Set_Node_Rot_II( OBJ_NODE *Node, FLT Rot );
extern OBJ_NODE *Blob_Node, *Obj1, *Obj2, *Obj3, *Obj4;
#endif

/*******************************************************************/

   // in dsp.c

extern BYTE *Dsp_Radial_1;
extern BYTE *Dsp_Radial_2; // [2*The_W*The_H];
extern BYTE *Dsp_Radial;
extern INT Dsp_Parity;

extern void Pre_Cmp_Dsp( INT Use_MMX );
extern void Init_Dsp( INT Param );
extern void Loop_Dsp_I( );
extern void Loop_Dsp_II( );
extern void Loop_Dsp_III( );
extern void Deform_Blur( FLT Amp, INT i, INT j );
extern void Deform_Blur_II( FLT Amp, INT i, INT j );
extern void Init_Deform_Blur( );
// extern void Close_Dsp( );

extern void Init_Blur_II( INT Param );
extern void Loop_Blur_II( FLT x );
extern void Close_Blur_II( );

extern void Init_Blur_I( INT Param );
extern void Loop_Deform_Blur_I( );
extern void Loop_Deform_Blur_II( );
extern void Loop_Deform_Blur_III( );
extern void Loop_Deform_Blur_IV( );

extern void Init_Dissolve( INT Param );
extern void Do_Dissolve( );

/*******************************************************************/

#if 0       // OLD

   // in sim.c

extern void Pre_Cmp_Vtx( );
extern void Init_Vtx( INT Param );
extern void Loop_Vtx( );
extern void Close_Vtx( );

#endif

/*******************************************************************/

   // in appear.c

extern void Pre_Cmp_Appear( );
extern void Init_Appear( INT Param );
extern void Loop_Appear( );
extern void Loop_Appear_II( );
extern void Close_Appear_II( );
extern void Loop_Disappear( );
extern void Loop_Disappear_2( );
extern void Close_Disappear( );
extern void Destroy_Appear( );

/*******************************************************************/

   // in panim.c

extern DMASK *Mask012;
extern DMASK *Mask022;
extern DMASK *MaskDie;
extern DMASK *MaskWork;
extern DMASK *MaskConsume;
extern DMASK *MaskBuy;
extern DMASK *Mask052;
extern DMASK *Mask072;
extern DMASK *Mask082;
extern DMASK *Mask112;
extern DMASK *Mask122;
extern DMASK *Mask132;
extern DMASK *Mask162;
extern DMASK *MaskGreets1;
extern DMASK *MaskGreets2;
extern DMASK *MaskGreets3;
extern DMASK *Mask202;
extern DMASK *Mask212;
extern DMASK *Mask222;
extern DMASK *MRest;
extern DMASK *Head_For, *Modern, *Upgrade, *Resist;

extern BITMAP_16 *End_Pic;

extern BITMAP_16 *Cnt4, *Cnt5;
extern BITMAP_16 *Count1, *Count2, *Count3;

extern DMASK *Cur_Mask;
extern INT Anim_Cnt, Anim_Max_Cnt, Cur_Anim_Size, Cur_Anim_Max_Size;
extern BITMAP_16 *Cur_Anim;
extern BYTE *Cur_Anim_Bits;
extern INT Cur_Anim_Width, Cur_Anim_Height;

extern BITMAP_16 *Blur012;
extern BITMAP_16 *Restb1, *Restb2;

extern BITMAP_16 *Anim12, *Anim32, *Anim93;
extern BITMAP_16 *Anim72, *Anim82;

extern FLT Time_Scale, Time_Off;

/*******************************************************************/

extern void Pre_Cmp_Anim( );
extern void Pre_Cmp_Mask( );
extern void Close_Anim( );
extern void Close_Mask( );
extern void Pre_Cmp_Blb( );

/*******************************************************************/

   // in caustics

extern void Pre_Cmp_Caustics( );
extern void Init_Caustics( INT Param );
extern void Loop_Caustics_I( );
extern void Loop_Caustics_II( );
extern void Loop_Caustics_III( );
extern void Loop_Caustics_IV( );
extern void Close_Caustics( );

/*******************************************************************/

   // in parts.c

extern void Init_00( INT Param );   // parental I
extern void Init_01( INT Param );   // Tunnel
extern void Init_02( INT Param );   // tmp...
extern void Init_03( INT Param );   // 
extern void Init_Greets( INT Param );   // greets
extern void Init_04( INT Param );   // 
extern void Init_05( INT Param );   //
extern void Init_06( INT Param );
extern void Init_07( INT Param );

/*******************************************************************/
/*******************************************************************/
