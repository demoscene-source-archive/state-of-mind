;//////////////////////////////////////////////////////////////////////

globl ASM_dewin36_1
globl ASM_dewin36_2

%macro ld 1
	fld dword [ecx+%1*4]
	fmul dword [ebx+%1*4]
%endmacro
%macro lD 2
	fld dword [ecx+%1*4]
	fmul dword [ebx+%2*4]
%endmacro
%macro ldd 1
	fld dword [ecx+%1*4]    ; 0 | x | -
	fmul dword [ebx+%1*4]   ; 0* | x | -
	fxch st2                ; - | x | 0*
	faddp st1               ; x* | 0*
%endmacro
%macro lDDD 2
	lD %1,%2          ; x|a
	lD (%1+1),(%2-1)  ; y|x|a
	fxch st1          ; x|y|a
	faddp st2         ; y|x'
	fsubp st1         ; x'-y
%endmacro

;//////////////////////////////////////////////////////////////////////

TEXT

align4
ASM_dewin36_1:	; *u_ptr, *Win, FLT *Sample, (48-2*start)*4 = esp+28

	push ebx
	push ecx
	push edx

	mov ecx,[esp+16]	; ecx = u_ptr
	mov ebx,[esp+20]	; ebx = Win
	mov eax,[esp+24]	; eax = Sample
	mov edx,16
align4
Loop11:
	ld 0
	ld 1		; 1 | 0
	ldd 2		; 0+1|2
	ldd 3	   ; A|3
	ldd 4		; A|4
	ldd 5		; A|5
	ldd 6		; A|6
	ldd 7    ; A|7
	ldd 8    ; A|8
	ldd 9
	ldd 10
	ldd 11
	ldd 12
	ldd 13
	ldd 14
	ldd 15	; A|15
	faddp st1   ; A
	add ecx,16*4	; u_ptr+=16
	add ebx,32*4	; Win+=32
	add eax,4
	dec edx
	fstp dword [eax-4]
	jg near Loop11

	ld 0
	ld 2		; 2 | 0
	ldd 4		; 0+2|4
	ldd 6		; A|6
	ldd 8
	ldd 10
	ldd 12
	ldd 14	; A|14
	faddp st1
	sub ebx,[esp+28]	; Win-(48-2*Start)
	add eax,4
	mov edx,15
	fstp dword [eax-4]

Loop12:
	sub ecx,16*4
	lD 0,15
	lD 1,14		; 1 | 0
	fsubp st1	; 0-1
	lDDD 2,13
	lDDD 4,11
	lDDD 6,9
	lDDD 8,7
	lDDD 10,5
	lDDD 12,3
	lDDD 14,1
	sub ebx,32*4
   add eax,4
	fchs	
	dec edx
	fstp dword [eax-4]
	jg near Loop12

	pop edx
	pop ecx
	pop ebx
	ret

align4
ASM_dewin36_2:	; *u_ptr, *Win, FLT *Sample, 4*(48-2*start) = esp+28

	push ebx
	push ecx
	push edx
	mov ecx,[esp+16]	; ecx = u_ptr
	mov ebx,[esp+20]	; ebx = Win
	mov eax,[esp+24]	; eax = Sample
	mov edx,16
align4
Loop21:
	ld 0
	ld 1		; 1 | 0
	ldd 2		; 0+1|2
	ldd 3	   ; A|3
	ldd 4		; A|4
	ldd 5		; A|5
	ldd 6		; A|6
	ldd 7    ; A|7
	ldd 8    ; A|8
	ldd 9
	ldd 10
	ldd 11
	ldd 12
	ldd 13
	ldd 14
	ldd 15	; A|15
	add ecx,16*4	; u_ptr+=16
	faddp st1   ; A
	add ebx,32*4	; Win+=32
	fstp dword [eax]
	add eax,4
	dec edx
	jg near Loop21

	ld 1
	ld 3
	ldd 5
	ldd 7
	ldd 9
	ldd 11
	ldd 13
	ldd 15
	faddp st1
	sub ebx,[esp+28]	; -(48-2*Start)
	fstp dword [eax]
	add eax,4

	mov edx,15
.Loop22:
   sub ecx,16*4
	lD 0,15
	lD 1,14		; 1 | 0
	fsubp st1	; 0-1
	lDDD 2,13
	lDDD 4,11
	lDDD 6,9
	lDDD 8,7
	lDDD 10,5
	lDDD 12,3
	lDDD 14,1
	sub ebx,32*4
	add eax,4	
   fstp dword [eax-4]
	dec edx
	jg near .Loop22

	pop edx
	pop ecx
	pop ebx
	ret

;//////////////////////////////////////////////////////////////////////
