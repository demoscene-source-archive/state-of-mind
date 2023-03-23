;//////////////////////////////////////////////////////////////////////////

;%include "nasm.h"

;//////////////////////////////////////////////////////////////////////////

[BITS 32]
globl Paste_Flare_Asm_8

extrn Sp_Offsets0   ; INT Sp_Offsets0[ MAX_WIDTH ];
extrn Sp_Whats0     ; void *Sp_Whats0[ MAX_SCANS ];
extrn Sp_Dst        ; UINT *Sp_Dst;
extrn Sp_BpS        ; INT Sp_BpS;
extrn Sp_dWo        ; INT Sp_dWo, Sp_dHo;
extrn Sp_dHo
extrn Sp_CMap       ; void *Sp_CMap

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

DATA

Paste_Flare_Asm_8:
	Enter

	mov edi,[Sp_Dst]      ; Where
	lea esi,[Sp_Whats0]   ; Whats
	lea ebp,[Sp_Offsets0] ; Offsets

	mov ecx,[Sp_CMap]     ; Tab
	mov [Blah+2], ecx

	mov ecx,[Sp_BpS]
	mov [Cont_j+2],ecx

	mov ecx,[Sp_dWo]    ; dWo
	mov edx,[Sp_dHo]    ; dHo

	lea edi, [edi+ecx]			; Where += dWo
	lea ebp, [ebp+ecx*4]			; Offsets += dWo
	neg ecx							; i
	neg edx							; j
	xor eax,eax
Loop_j:
	inc edx
	jl Go_j

	Leave

Go_j:
	mov ebx, [esi]			; ebx: What
	push ecx			; ecx = dWo
	push esi
	xor esi,esi
Loop_i:
	inc ecx
	jl Go_i

Cont_j:
	add edi, 0x10000000
	pop esi
	pop ecx
	add esi,4
	jmp Loop_j
Go_i:
	mov esi, [ebp+ecx*4]	 	; Offsets[i]
	mov ah, [ebx+esi]	 		; What[]
	mov al, [edi+ecx]			; | Where[]>>8
Blah:
	mov ah, [0x12345678+eax] ; Tab[...]
	mov [edi+ecx],ah	 		; => Where[]
	inc ecx
	jl Go_i
	jmp Cont_j;

;//////////////////////////////////////////////////////////////////////////
