;//////////////////////////////////////////////////////////////////////

;include "nasm.h"

;//////////////////////////////////////////////////////////////////////

globl Do_Fire
globl Do_Fire2
extrn Hash_Seed
extrn Hash1
extrn Rand_Tab

DATA

align4
Lines: dd 0

;//////////////////////////////////////////////////////////////////////

TEXT

align4
Do_Fire:

; Do fire effect...
; 20(%esp): Src   24(%esp) Width   28(%esp): Height
; 32(%esp): Skip
	push ebp
	push edi
	push esi
	push ebx

	mov edi,[20+esp]		; Src
	mov esi,edi
	xor ebx,ebx
	add esi,[32+esp]		; Dst+Skip
	mov bl, [Hash_Seed]

	mov eax,[28+esp]
	dec eax
	mov [Lines],eax

	mov ebp,[24+esp]		; Width 
align4
FIRE_Y:
	mov ecx,ebp			; ecx = n 
	dec ecx
align2
FIRE_X:
	movzx edx, byte [esi+ecx]	; Src[n+W*_SKIP] 
	test dl,dl
	je STORE2
	movzx ebx, byte [Hash1+ebx]
	movsx eax, byte [Rand_Tab+ebx]
	dec eax
	je STORE2
	dec edx
	add ecx,eax			; n+=k !!! 
	test ecx,ecx
	jge CONT1
	xor ecx,ecx
	jmp STORE2
CONT1:
	cmp ecx,ebp			; >Width ? 
	jl STORE2
	mov ecx,ebp
	dec ecx
STORE2:
	mov [edi+ecx],dl		; => Dst 
NO_STORE2:
	dec ecx
	jge FIRE_X

	add esi,ebp
	add edi,ebp
	dec dword [Lines]
	jg FIRE_Y

	pop ebx
	pop esi
	pop edi
	pop ebp
	ret

;////////////////////////////////////////////////////////////////////

Do_Fire2:

; Do fire effect...
; 20(esp): Src   24(esp) Width   28(esp): Height
; 32(esp): Skip
	push ebp
	push edi
	push esi
	push ebx

	mov edi,[20+esp]		; Src 
	mov esi,edi
	xor ebx,ebx
	add esi,[32+esp]		; Dst+Skip 
	mov bl, byte [Hash_Seed]

	mov eax,[28+esp]
	dec eax
	mov [Lines],eax

	mov ebp,[24+esp]		; Width 
align4
FIRE_Y2:
	mov ecx,ebp			; ecx = n 
	dec ecx
align2
FIRE_X2:
	movzx edx,byte [esi+ecx]	; Src[n+W*_SKIP] 
	test dl,dl
	je STORE3
	movzx ebx, byte [Hash1+ebx]
	movsx eax, byte [Rand_Tab+ebx]
	dec eax
	je STORE3
	dec edx
	add ecx,eax			; n+=k !!! 
	test ecx,ecx
	jge CONT2
	xor ecx,ecx
	jmp STORE3
CONT2:
	cmp ecx,ebp			; >Width ? 
	jl STORE3
	mov ecx,ebp
	dec ecx
STORE3:
	mov [esi+ecx],dl		; => Dst 
NO_STORE3:
	dec ecx
	jge FIRE_X2

	add esi,ebp
	add esi,ebp
	dec dword [Lines]
	jg FIRE_Y2

	pop ebx
	pop esi
	pop edi
	pop ebp
	ret

;/////////////////////////////////////////////////////////////////////

