//   Common poly drawing func
// what needs being #defined
// - _FUNC_    func name ( _Draw_UVc_64_16)
// - LOG2_PIX_PER_SPANS  (e.g: 6 for 64-pix spans)
// - DIV_TAB  (e.g:_Div_Tab_64_)

#define PIX_PER_SPANS   (1<<LOG2_PIX_PER_SPANS)

EXTERN void _FUNC_( )
{
   INT y = Scan_Start;
   USHORT *Dst = ((USHORT *)_RCst_.Base_Ptr) + (y+1)*(_RCst_.Pix_BpS/2);
   INT dy = Scan_H;
   DyU *= 1.0f*PIX_PER_SPANS; 
   DyV *= 1.0f*PIX_PER_SPANS; 
   DyiZ *= 1.0f*PIX_PER_SPANS;

   while( dy>0 )
   {
      INT Len, Left_Over;
      FLT uiz, viz, iz, z;
      INT U, V;
      USHORT *Ptr;

      iz  = Scan_iZ[y]; z = 1.0/iz;
      Len = Scan_Pt2[y];
      Ptr = Dst - Len;
      Len = Len - Scan_Pt1[y];
      if ( Len<=0 ) goto Skip;

      Left_Over = (Len)&(PIX_PER_SPANS-1);
      Len = Len>>LOG2_PIX_PER_SPANS;

      uiz = Scan_UiZ[y]; U = (INT)( uiz*z );
      viz = Scan_ViZ[y]; V = (INT)( viz*z );

      if ( Len==0 )
      {
         uiz -= DyU*DIV_TAB[Left_Over];
         viz -= DyV*DIV_TAB[Left_Over];
         iz  -= DyiZ*DIV_TAB[Left_Over];
         z = 1.0/iz;
         goto Finish;
      }
      iz -= DyiZ; uiz -= DyU; viz -= DyV;
      z = 1.0/iz;

      while( --Len>=0 )
      {
         INT Span;

         INT U2 = (INT)( uiz*z ); 
         INT V2 = (INT)( viz*z );
         INT dU = (U2-U)>>LOG2_PIX_PER_SPANS; 
         INT dV = (V2-V)>>LOG2_PIX_PER_SPANS;
         if ( Len ) {
            uiz -= DyU; viz -= DyV; iz -= DyiZ;
         }
         else if ( Left_Over ) {
            uiz -= DyU*DIV_TAB[Left_Over];
            viz -= DyV*DIV_TAB[Left_Over];
            iz  -= DyiZ*DIV_TAB[Left_Over];
         }
         z = 1.0/iz;  // launch the fdiv
         Span = -PIX_PER_SPANS; Ptr -= Span;
         while( Span<0 )
         {
            USHORT Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
            Ptr[Span++] = ((USHORT*)UV_Src)[ Off&Mip_Mask ];
            U += dU; V += dV;
         }
         U = U2; V = V2;    // for safety with roundoffs
      }

Finish:
      if ( Left_Over>0 )
      {
         FLT U2 = uiz*z; 
         FLT V2 = viz*z;
         INT dU = (INT)( (U2-(FLT)U)*_Div_Tab_1_[Left_Over] );
         INT dV = (INT)( (V2-(FLT)V)*_Div_Tab_1_[Left_Over] );
         while( --Left_Over>=0 )
         {
            USHORT Off = ( (U>>16)&0x00FF ) | ( (V>>8)&0xFF00 );
            *Ptr++ = ((USHORT*)UV_Src)[ Off&Mip_Mask ];
            U += dU; V += dV;
         }
      }
Skip:
      Dst = (USHORT*)((BYTE*)Dst+_RCst_.Pix_BpS);
      ++y;
      dy--;
   }
}

#undef PIX_PER_SPANS
