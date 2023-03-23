;//////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////

globl Bumpify_I

;//////////////////////////////////////////////////////////////////////
%define CLAMP_OFF 320
extern I_Table

TEXT

align4
Bumpify_I:	; *Dst, *Src, W, H, Dst_BpS, Src_BpS, xo, yo, *Env_Map

	Enter

	mov edi,[esp+OFF_STCK]		; Dst
	mov esi,[esp+OFF_STCK+4]	; Src
	mov eax,[esp+OFF_STCK+16]	; Dst_BpS
	mov [PT2+2], eax
	mov eax,[esp+OFF_STCK+20]	; Src_BpS
	mov [PT2+2], eax
	mov [PT3+2], eax

	mov edx, [esp+OFF_STCK+8]	; W
	add edx, [esp+OFF_STCK+8]	; +xo
	mov [PT1_I+2,eax

	mov edx, [esp+OFF_STCK+12]	; H
	neg edx

Loop_j_I:
	push edx
	mov edx, [esp+OFF_STCK+8]	; W
	xor ecx,ecx
	neg edx
Loop_i_I:
	xor eax,eax
	xor ebx,ebx
	mov al, [1+esi+edx]
	mov bl, [esi+edx]
	sub eax,ebx
	add eax,edx
PT1_I:
	add eax,0x12345678		; xo + W
	mov cl,[I_Table+CLAMP_OFF+eax]	; =>x
	xor eax,eax
PT3_I:
	mov al, [esi+0x12345678]	; Src[i+Src_BpS]
	sub eax,ebx
	add eax,ebp			; ebp = yo-j (=jo)
	mov ch,[I_Table+CLAMP_OFF+eax]	; =>y
PT4_I:
	mov al,[0x12345678+ecx]		; Env_Map[]
	mov [edi+edx],al

	inc edx
	jl Loop_i_I

	pop edx
PT2_I:
	add edi,0x12345678
	add esi,0x12345678
	inc edx
	jl Loop_j_I

	Leave

;//////////////////////////////////////////////////////////////////////
