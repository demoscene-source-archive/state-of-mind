;//////////////////////////////////////////////////////////////////////////

;%include "nasm.h"

;//////////////////////////////////////////////////////////////////////////

[BITS 32]
globl Counting_Sort

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

TEXT
Counting_Sort:

	Enter
	mov esi,[esp+OFF_STCK+0]	; UINT *A         OFFSET?
	mov ebx,[esp+OFF_STCK+4]   ; Nb

	dec ebx
	jle near End_Sort

	mov ebp,esp       ; ebp = save esp
	sub esp, 512*2		; USHORT C[512]
	xor eax,eax
	mov edi,esp       ; edi = C[]
	mov ecx,255

Loop1:
	mov [edi+ecx*4],eax
	dec ecx
	jge Loop1

   lea edx,[ebx*4]         ; edx = (Nb-1)*4
	neg edx
	lea esp,[esp-4+edx*4]   ; esp = B[]

	mov ecx,ebx		; ecx = Nb
	xor eax,eax
Loop2:
	mov edx,[esi+ecx*4]  ; j = A[n]
	mov al,dl            ; j &= 0xFF
	inc word [edi+eax*2] ; C[j]++
	dec ecx
	jge Loop2

	mov ecx,-255
Loop3:
	mov ax,[edi+ecx*2 +256*2-2 ]
	add [edi+ecx*2 +256*2 ],ax
	inc ecx
	jl Loop3

	mov ecx,ebx		; ecx = Nb
	xor edx,edx
Loop4:
	mov eax, [esi+ecx*4] ; Tmp = A[n] 
	mov dl,al            ; & 0xFF;
	dec word [edi+edx*2] ; C[tmp]--
	mov dx, [edi+edx*2]  ; j
	mov [esp+edx*4],eax  ; B[j] = Tmp;
	xor edx,edx
	mov dl,ah            ; (A[n]&0xFF00)>>8
	inc word [edi+edx*2+256*2]

	dec ecx
	jge Loop4

	mov ecx,-255
Loop5:
	mov ax,[edi+ecx*2 +512*2-2 ]
	add [edi+ecx*2 +512*2 ],ax
	inc ecx
	jl Loop5

	mov ecx,ebx
	;xor edx,edx   ;useless
	xor ebx,ebx
	add edi,256*2  ; edi = &C[256]
Loop6:
	mov eax,[esp+ecx*4]	; B[n]
	mov dl,ah            ; Tmp = (B[n]&0xFF00)>>8
	dec word [edi+edx*2]	; C[256+Tmp]--
	mov bx, [edi+edx*2]	; =>j
	mov [esi+ebx*4],eax
	dec ecx
	jge Loop6

	mov esp,ebp

End_Sort:
	Leave

;//////////////////////////////////////////////////////////////////////////
