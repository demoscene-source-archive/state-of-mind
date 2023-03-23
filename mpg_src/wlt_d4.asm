;//////////////////////////////////////////////////////////////////////

globl Do_Inv_Daub4
extrn WLT_Daub4

;//////////////////////////////////////////////////////////////////////

TEXT

align4
Do_Inv_Daub4:	; FLT *Dst, FLT *Src, INT Len     (extern double WLT_Daub4[4])

	push esi
	push eax
	push ebx
	push ecx

	fld qword [WLT_Daub4]		; D0
	mov eax,[esp+20]		; Dst
	fld qword [WLT_Daub4+8]		; D1
	mov esi,[esp+20+4]	; Src
	fld qword [WLT_Daub4+16]	; D2
	mov ebx,[esp+20+8]	; Len
	fld qword [WLT_Daub4+24]	; D3 | D2 | D1 | D0
	cmp ebx,4
	jge Ok
End:
	fcompp
	pop ecx
	pop ebx
	fcompp
	pop eax
	pop esi
	ret

Ok:
	mov ecx,ebx
	shl ecx,1			; nh=(Len>>1)*4

	fld dword [esi+ecx-4]	; X=Src[nh-1] | D3 | D2 | D1 | D0
	fld st0			; X | X | D3 | D2 | D1 | D0
	fmul st3		; D2.X | X | D3 | D2 | D1 | D0
	fxch st1		; X | D2.X | D3 | D2 | D1 | D0
	fmul st2		; D3.X | D2.X | D3 | D2 | D1 | D0

	fld dword [esi+ecx*2-4]	; Y=Src[Len-1] | D3.X | D2.X | D3 | D2 | D1 | D0
	fld st0			; Y | Y | D3.X | D2.X | D3 | D2 | D1 | D0
	fmul st6		; D1.Y | Y | D3.X | D2.X | D3 | D2 | D1 | D0
	fxch st1		; Y | D1.Y | D3.X | D2.X | D3 | D2 | D1 | D0
	fmul st7		; D0.Y | D1.Y | D3.X | D2.X | D3 | D2 | D1 | D0
	fsubp st2		; D1.Y | D3.X-D0.Y | D2.X | D3 | D2 | D1 | D0
	faddp st2		; D3.X-D0.Y | D2.X+D1.Y | D3 | D2 | D1 | D0
	fxch st1
	fstp dword [eax]
	fstp dword [eax+4]

	fld dword [esi]		; X=Src[0] | D3 | D2 | D1 | D0
	fld st0			; X | X | D3 | D2 | D1 | D0
	fmul st5		; D0.X | X | D3 | D2 | D1 | D0
	fxch st1		; X | D0.X | D3 | D2 | D1 | D0
	fmul st4		; D1.X | D0.X | D3 | D2 | D1 | D0

	fld dword [esi+ecx]	; Y=Src[nh] | D1.X | D0.X | D3 | D2 | D1 | D0
	fld st0			; Y | Y | D1.X | D0.X | D3 | D2 | D1 | D0
	fmul st5		; D2.Y | Y | D1.X | D0.X | D3 | D2 | D1 | D0
	fxch st1		; Y | D2.Y | D1.X | D0.X | D3 | D2 | D1 | D0
	fmul st4		; D3.Y | D2.Y | D1.X | D0.X | D3 | D2 | D1 | D0
	faddp st3		; D2.Y | D1.X | D0.X+D3.Y | D3 | D2 | D1 | D0
	fsubp st1		; D1.X-D2.Y | D0.X+D3.Y | D3 | D2 | D1 | D0
	fxch st1

	shr ebx,1			; ebx = nh

	fadd dword [eax]
	fstp dword [eax]
	fadd dword [eax+4]
	fstp dword [eax+4]

LoopI:
	dec ebx				; <= i =nh-1
	jle End

	fld dword [esi]		; X=Src[i] | D3 | D2 | D1 | D0
	fld st0			; X | X | D3 | D2 | D1 | D0
	fmul st3		; D2.X | X | D3 | D2 | D1 | D0
	fxch st1		; X | D2.X | D3 | D2 | D1 | D0
	fmul st2		; D3.X | D2.X | D3 | D2 | D1 | D0

	add eax,8

	fld dword [esi+ecx]	; Y=Src[i+nh] | D3.X | D2.X | D3 | D2 | D1 | D0
	fld st0			; Y | Y | D3.X | D2.X | D3 | D2 | D1 | D0
	fmul st6		; D1.Y | Y | D3.X | D2.X | D3 | D2 | D1 | D0
	fxch st1		; Y | D1.Y | D3.X | D2.X | D3 | D2 | D1 | D0
	fmul st7		; D0.Y | D1.Y | D3.X | D2.X | D3 | D2 | D1 | D0
	fsubp st2		; D1.Y | D3.X-D0.Y | D2.X | D3 | D2 | D1 | D0
	faddp st2		; D3.X-D0.Y | D2.X+D1.Y | D3 | D2 | D1 | D0
	fxch st1

	add esi,4

	fstp dword [eax]
	fstp dword [eax+4]

	fld dword [esi]		; X=Src[i+1] | D3 | D2 | D1 | D0
	fld st0			; X | X | D3 | D2 | D1 | D0
	fmul st5		; D0.X | X | D3 | D2 | D1 | D0
	fxch st1		; X | D0.X | D3 | D2 | D1 | D0
	fmul st4		; D1.X | D0.X | D3 | D2 | D1 | D0

	fld dword [esi+ecx]	; Y=Src[i+1+nh] | D1.X | D0.X | D3 | D2 | D1 | D0
	fld st0			; Y | Y | D1.X | D0.X | D3 | D2 | D1 | D0
	fmul st5		; D2.Y | Y | D1.X | D0.X | D3 | D2 | D1 | D0
	fxch st1		; Y | D2.Y | D1.X | D0.X | D3 | D2 | D1 | D0
	fmul st4		; D3.Y | D2.Y | D1.X | D0.X | D3 | D2 | D1 | D0
	faddp st3		; D2.Y | D1.X | D0.X+D3.Y | D3 | D2 | D1 | D0
	fsubp st1		; D1.X-D2.Y | D0.X+D3.Y | D3 | D2 | D1 | D0
	fxch st1
	fadd dword [eax]
	fstp dword [eax]
	fadd dword [eax+4]
	fstp dword [eax+4]

	jmp LoopI

;//////////////////////////////////////////////////////////////////////
