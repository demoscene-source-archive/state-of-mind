;//////////////////////////////////////////////////////////////////////////

;%include "nasm.h"

;//////////////////////////////////////////////////////////////////////////
;////////////////////////////////////////////////////////////////////////////

[BITS 32]

globl ASM_Detect_CPU

;//////////////////////////////////////////////////////////////////////////

TEXT

align4
ASM_Detect_CPU:
	push edx

; Detect 386/other...
	mov edx,esp
	and esp,0xFFFFFFFC	; 4-align esp
	pushf			; pushfl
	pop eax
	mov ecx,eax		; save original flags in ecx
	xor eax,0x00040000
	push eax
	popf			; popfl
	pushf
	pop eax
	xor eax,ecx
	shr eax,18
	and eax,0x01
	push ecx		; restore original flags
	popf
	mov esp,edx
	test eax,eax
	jne Not_386
	mov eax,0x01		; _CPU_x86_
	jmp End
	
Not_386:	; Detect ID flag presence
	pushf
	pop eax
	mov ecx,eax
	or eax,0x00200000	; Set ID flag
	push eax
	popf			; Warning !!! With PMODE/DJ, ID flag is
	pushf			; mangled during popfl/pushfl !!!
	pop eax			; whereas CWSDPMI does it ok...
	xor eax,ecx		; => CPUID is not detect correctly..
	jnz Do_CPUID
	mov eax,0x02		; Old 486. return _CPU_OLD486_
	jmp End
Do_CPUID:
	mov eax,0x01
	db 0x0F			; CPUID instr.
	db 0xA2
	mov ebx,eax
	mov eax,0x03		; _CPU_486_
	and ebx,0x0F00
	cmp ebx,0x0500
	jne End
	mov eax,0x04		; _CPU_586_
End:
        pop ebx
        ret

;///////////////////////////////////////////////////////////////////////////
