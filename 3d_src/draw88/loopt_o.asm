;//////////////////////////////////////////////////////////////////////////

[BITS 32]

TEXT

%macro DO_PIXEL 1
	mov al, [ebx]
	add ebp, edx
	adc bh,dh
	add esi,eax
	adc bl,dl
	mov [edi+%{1}*2],al
%endmacro

%macro DO_ONE_PIXEL 0
	mov al,[ebx]
	add ebp,edx
	adc bh,dh
	add esi,eax
	adc bl,dl
	mov [edi+ecx*2],al
%endmacro

;//////////////////////////////////////////////////////////////////////////

extrn _RCst_
extrn UV_Src
extrn DyU
extrn DyV
extrn DyiZ
extrn Scan_U
extrn Scan_V
extrn Scan_UiZ
extrn Scan_ViZ
extrn Scan_iZ
extrn Scan_Start
extrn Scan_H
extrn Scan_Pt1
extrn Scan_Pt2
extrn _Div_Tab_1_

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

      ; This is *the* big macro  
      ; %1=COUNT (32/16/8..)  %2=SHIFT (5/4/3..) %3=MASK (=COUNT-1)

%macro MKLOOP 6

;//////////////////////////////////////////////////////////////////////////

extrn %{4}
extrn %{5}
globl %{6}

%{6}:

	Enter

	sub esp, 20		; [ Scan_H | U | V | U2 | V2 ]

	mov edi, [Scan_Start]
	mov ecx, edi		;%ecx = y
	inc edi			; y++
	shl ecx,2		; %ecx = y*4
	imul edi, [_RCst_+PIX_BPS]
	add edi, [_RCst_+BASE_PTR]     	; %edi = _RCst_+BASE_PTR + y*BpS - 1
	mov esi, [Scan_H]
	inc edi			; Texel is in upper byte...
	mov [esp], esi
	mov ebx, [UV_Src]	; Base_Txt

	fld dword [%{5}]	; 32
	fld dword [DyV]
	fxch st1
	fmul st1,st0		; 32 | dv
	fld dword [DyU]
	fxch st1
	fmul st1,st0		; 32 | du | dv
	fld dword [DyiZ]	; diz|du|dv
	fxch st1
	fmulp st1,st0		; diz|du|dv

align2
.Loop_Y:

	mov ebp, [Scan_Pt2+ecx]
	mov bx, word [Scan_Pt1+ecx]
	sub word bx, bp		; bx = -Len
	jl .Ok

	add edi,[_RCst_+PIX_BPS]
	add ecx,4			; y++
	dec dword [esp]
	jg .Loop_Y
   fcompp      ; diz|du|dv
   add esp, 20
   fstp st0

   Leave

align4
.Ok:

	fld1
	fld dword [Scan_ViZ+ecx]; viz | 1 |diz|du|dv
	fld dword [Scan_UiZ+ecx]; uiz | viz | 1 |diz|du|dv
	fld dword [Scan_iZ+ecx] ; iz |  uiz | viz | 1 |diz|du|dv
	fdiv st3, st0		; iz |  uiz | viz | 1/iz |diz|du|dv

	add ecx,4		; y++
	neg ebp
	push ecx		; ecx = y
	neg bx			; bx = Len > 0
	mov ecx,edi
	mov dx, bx
	add ecx,[_RCst_+PIX_BPS]
	shr bx, %{2}		; Len
	push ecx				; edi = Base ptr
	and edx, %{3}       ; edx = Left_Over
	xor ecx,ecx
	push esi		; esi = Scan_H
	mov cx, bx		; Len
	push dx
	lea edi, [edi+ebp*2]		; %edi: Dst
	push bx			; % Len|Left_Over | esi | edi | ecx | Scan_H | U | V | U2 | V2 %

		; Compute first U & V

	fld st1			; uiz | iz |  uiz | viz | 1/iz |diz|du|dv
	fmul st0,st4		; U | iz |  uiz | viz | 1/iz |diz|du|dv
	fistp dword [esp+4+16]  ; iz | uiz | viz | 1/iz |diz|du|dv
	fxch st2		; viz | uiz | iz | 1/iz |diz|du|dv
	fmul st3,st0		; viz | uiz | iz | V |diz|du|dv
	fxch st3		; V | uiz | iz | viz |diz|du|dv
	fistp dword [esp+8+16]	; uiz | iz | viz | diz|du|dv
	fxch st1		; iz | uiz | viz | diz|du|dv

	test ecx, ecx
	jnz .Len_Ok	; Len != 0

		; //// Len = 0  (edx=Left_Over) ////

	fld dword [%{4}+edx*4] ; S | iz | uiz | viz | diz|du|dv
	fld st5			; du | S | iz | uiz | viz | diz|du|dv
	mov ecx, edx		; ->ecx: Left_Over
	fmul st0,st1		; du' | S | iz | uiz | viz | diz|du|dv
	fsubp st3		; S | iz | uiz' | viz | diz|du|dv
	fxch st2		; uiz' | iz | S | viz | diz|du|dv
	fxch st3		; viz | iz | S | uiz' | diz|du|dv
	fld st6			; dv | viz | iz | S | uiz' | diz|du|dv
	fmul st0,st3		; dv' | viz | iz | S | uiz' | diz|du|dv
	fsubp st1		; viz' | iz | S | uiz' | diz|du|dv
	fxch st1		; iz | viz' | S | uiz' | diz|du|dv
	fld st4			; diz | iz | viz' | S | uiz' | diz|du|dv
	fmulp st3		; iz | viz' | diz' | uiz' | diz|du|dv
	fsubrp st2		; viz' | iz' | uiz' | diz|du|dv
	fxch st1		; iz' | viz' | uiz' | diz|du|dv
	fxch st2		; uiz' | viz' | iz' | diz|du|dv
	fld1			; 1 | uiz' | viz' | iz' | diz|du|dv
	fxch st3		; iz' | uiz' | viz' | 1 | diz|du|dv
	fdiv st3,st0

		; lost fdiv here.

	pop edx			; junk

	jmp .Left		; ecx = Left_Over
				; Draw the rest

.Len_Ok:	; ecx: Len!=0
		; iz |  uiz | viz | diz|du|dv

		; //// advance 1 span + launch div ////

	fsub st3	; iz' | uiz | viz | diz|du|dv
	fxch st1	; uiz | iz' | viz | diz|du|dv
	fsub st4	; uiz' | iz' | viz | diz|du|dv
	fxch st2	; viz | iz' | uiz' | diz|du|dv
	fsub st5	; viz' | iz' | uiz' | diz|du|dv
	fxch st1	; iz' | viz' | uiz' | diz|du|dv
	fld1		; 1 | iz' | viz' | uiz' | diz|du|dv
	fxch st3	; uiz' | iz' | viz' | 1 | diz|du|dv
	fxch st1	; iz' | uiz' | viz' | 1 | diz|du|dv
	fdiv st3,st0	; iz' | uiz' | viz' | =>1/iz' | diz|du|dv

		; lost fdiv here.

align4
.Loop1:		; iz | uiz | viz | 1/iz | diz|du|dv

		; //// Compute next U2, V2 //// 

	fld st1			; uiz | iz |  uiz | viz | 1/iz |diz|du|dv
	fmul st0,st4		; U | iz |  uiz | viz | 1/iz |diz|du|dv
	fistp dword [esp+12+16]  ; iz | uiz | viz | 1/iz |diz|du|dv
	fxch st2		; viz | uiz | iz | 1/iz |diz|du|dv
	fmul st3,st0		; viz | uiz | iz | V |diz|du|dv
	fxch st3		; V | uiz | iz | viz |diz|du|dv
	fistp dword [esp+16+16]	; uiz | iz | viz | diz|du|dv
	fxch st1		; iz | uiz | viz | diz|du|dv

	dec ecx			; Len--
	jg .Not_Last

		; //// Last n-pixels span ////

	mov ecx, dword [esp]
	shr ecx,16		; cx: Left_Over
	test ecx,ecx
	jz .No_Left_Over

	fld dword [%{4}+ecx*4] ; S | iz | uiz | viz | diz|du|dv
	fld st5			; du | S | iz | uiz | viz | diz|du|dv
	fmul st0,st1		; du' | S | iz | uiz | viz | diz|du|dv
	fsubp st3		; S | iz | uiz' | viz | diz|du|dv
	fxch st2		; uiz' | iz | S | viz | diz|du|dv
	fxch st3		; viz | iz | S | uiz' | diz|du|dv
	fld st6			; dv | viz | iz | S | uiz' | diz|du|dv
	fmul st0,st3		; dv' | viz | iz | S | uiz' | diz|du|dv
	fsubp st1		; viz' | iz | S | uiz' | diz|du|dv
	fxch st1		; iz | viz' | S | uiz' | diz|du|dv
	fld st4			; diz | iz | viz' | S | uiz' | diz|du|dv
	fmulp st3		; iz | viz' | diz' | uiz' | diz|du|dv
	fsubrp st2		; viz' | iz' | uiz' | diz|du|dv
	fxch st1		; iz' | viz' | uiz' | diz|du|dv
	fxch st2		; uiz' | viz' | iz' | diz|du|dv
	fld1			; 1 | uiz' | viz' | iz' | diz|du|dv
	fxch st3		; iz' | uiz' | viz' | 1 | diz|du|dv
	fdiv st3,st0
	xor ecx,ecx		; <= means: last span
	jmp .Setup 	; Draw this span

.No_Left_Over:
		; Load with junk, just to keep st() count right
	fld1			; 1 | iz | uiz | viz | diz|du|dv
	jmp .Setup 	; Draw this span

.Not_Last:
		; //// advance 1 span + launch div ////

	fsub st3	; iz' | uiz | viz | diz|du|dv
	fxch st1	; uiz | iz' | viz | diz|du|dv
	fsub st4	; uiz' | iz' | viz | diz|du|dv
	fxch st2	; viz | iz' | uiz' | diz|du|dv
	fsub st5	; viz' | iz' | uiz' | diz|du|dv
	fxch st1	; iz' | viz' | uiz' | diz|du|dv
	fld1		; 1 | iz' | viz' | uiz' | diz|du|dv
	fxch st3	; uiz' | iz' | viz' | 1 | diz|du|dv
	fxch st1	; iz' | uiz' | viz' | 1 | diz|du|dv
	fdiv st3,st0	; iz' | uiz' | viz' | =>1/iz' | diz|du|dv

align4
.Setup:
	mov edx, [esp+16+16]	; V2
	mov eax, [esp+12+16]	; U2
	mov ebp, [esp+ 8+16]	; V
	mov esi, [esp+4+16]	; U
	mov bh, [esp+8+16 + 2]  ; Vi
	mov bl, [esp+4+16 + 2]  ; Ui
	mov [esp+8+16], edx	; V2 => V
	mov [esp+4+16], eax	; U2 => U
	sub edx, ebp		; edx = V2-V
	sub eax, esi		; eax = U2-U
	shr edx, %{2}
	shr eax, %{2}
	mov [esp+16+16], edx	; => dV
	mov [esp+12+16], eax	; => dU
	shl ebp, 16		; ebp = Vf
	shl esi, 16		; esi = Uf
	shl edx, 16		; edx = dVf
	shl eax, 16		; eax = dUf

	mov dh, [ esp+16+16 + 2 ]	; dVi
	mov dl, [ esp+12+16 + 2 ]	; dUi

;//// unrolled loop ////

align4
.Span_Loop:
   DO_PIXEL 0
   DO_PIXEL 1
%if (%{1}>2)
   DO_PIXEL 2
   DO_PIXEL 3
%endif
%if (%{1}>4)
   DO_PIXEL 4
   DO_PIXEL 5
   DO_PIXEL 6
   DO_PIXEL 7
%endif
%if (%{1}>8)
   DO_PIXEL 8
   DO_PIXEL 9
   DO_PIXEL 10
   DO_PIXEL 11
   DO_PIXEL 12
   DO_PIXEL 13
   DO_PIXEL 14
   DO_PIXEL 15
%endif
%if (%{1}>16)
   DO_PIXEL 16
   DO_PIXEL 17
   DO_PIXEL 18
   DO_PIXEL 19
   DO_PIXEL 20
   DO_PIXEL 21
   DO_PIXEL 22
   DO_PIXEL 23
   DO_PIXEL 24
   DO_PIXEL 25
   DO_PIXEL 26
   DO_PIXEL 27
   DO_PIXEL 28
   DO_PIXEL 29
   DO_PIXEL 30
   DO_PIXEL 31
%endif

	add edi, %{1}*2
	test ecx,ecx
	jnz near .Loop1

	pop ecx
	shr ecx,16		; ecx: Left Over

;/////////////  Finish with left over ///////////////

align4
.Left:		;  iz | uiz | viz | 1/iz | diz|du|dv
	test ecx, ecx
	fstp st0	;  uiz | viz | 1/iz | 	diz|du|dv
	jz near .End

	fmul st0, st2			; U2 | viz | 1/iz
	fxch st1			; viz | U2 | 1/iz
	fmulp st2,st0			; U2 | V2
	fisub dword [esp+4+12]		; U2-U | V2
	fld dword [_Div_Tab_1_+ecx*4]	; S | U2-U | V2
	fxch st2			; V2 | U2-U | S
	fisub dword [esp+8+12] 		; V2-V | U2-U | S
	fmul st0,st2			; dV | U2-U | S
	fxch st2			; S | U2-U | dV
	fmulp st1			; dU | dV
	fistp dword [esp+12+12]		; => dU
	fistp dword [esp+16+12] 	; => dV

	lea edi, [edi+2*ecx]
	neg ecx
	mov ebp, [esp+8+12]	; V
	mov esi, [esp+4+12]	; U
	mov bh, [esp+8+12 + 2 ] ; Vi
	mov bl, [esp+4+12 + 2 ] ; Ui
	mov edx, [esp+16+12]	; edx = dV
	mov eax, [esp+12+12]	; eax = dU
	shl ebp, 16		; ebp = Vf
	shl esi, 16		; esi = Uf
	shl edx, 16		; edx = dVf
	shl eax, 16		; eax = dUf

	mov dh, [ esp+16+12 + 2 ]	; dVi
	mov dl, [ esp+12+12 + 2 ]	; dUi

.Loop_Left:
   DO_ONE_PIXEL
	inc ecx
	jl .Loop_Left

	pop esi
	pop edi
	pop ecx
	dec dword [esp]
	jg near .Loop_Y
	jmp .Exit

align4
.End:
	fcompp
	pop esi
	pop edi
	fstp st0		; diz|du|dv
	pop ecx
	dec dword [esp]
	jg near .Loop_Y

.Exit:
	fcompp		; diz|du|dv
	add esp, 20
	fstp st0

	Leave

%endmacro         ; MKLOOP

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

      ;  Generate all routines.

MKLOOP 32,5,31, _Div_Tab_32_ , _CST_32_, _Draw_UVc_32_88

MKLOOP 16,4,15, _Div_Tab_16_ , _CST_16_, _Draw_UVc_16_88

MKLOOP 8,3,7, _Div_Tab_8_ , _CST_8_, _Draw_UVc_8_88

MKLOOP 4,2,3, _Div_Tab_4_ , _CST_4_, _Draw_UVc_4_88

MKLOOP 2,1,1, _Div_Tab_2_ , _CST_2_, _Draw_UVc_2_88

;//////////////////////////////////////////////////////////////////////////


