;//////////////////////////////////////////////////////////////////////////

[BITS 32]
globl Paste_Sprite_Asm_16
globl Paste_Sprite_Asm_8
globl Paste_Sprite_Asm_88
globl Paste_Sprite_Asm_8_Map16_Or
globl Paste_Sprite_Asm_8_Map16

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

Paste_Sprite_Asm_8:
	Enter

	mov edi,[Sp_Dst]		; Dst
	mov ecx,[Sp_BpS]	; BpS
	lea esi,[Sp_Whats0]	; Whats
	lea ebp,[Sp_Offsets0]	; Offsets

	mov [.Cont+2],ecx

	mov ecx,[Sp_dWo]	; dWo

	lea edi, [edi+ecx]		; Where += dWo
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	neg ecx				; i
	xor eax,eax

.Loop_j:
	dec dword [Sp_dHo]
	jg .Go_j

	Leave

.Go_j:
	mov ebx, [esi]			; ebx: What
	push ecx					; ecx = dWo
	inc ecx
	jl .Go_i

.Cont:
	add edi, 0x10000000
	pop ecx
	add esi,4
	jmp .Loop_j
.Go_i:
	mov edx, [ebp+ecx*4]	; Offsets[i]
	mov al, [ebx+edx]    ; What[]
	test al,al
	jz .Blah
	mov [edi+ecx],al     ; =>*Where
.Blah:
	inc ecx
	jl .Go_i
	jmp .Cont;

;//////////////////////////////////////////////////////////////////////////

Paste_Sprite_Asm_8_Map16_Or:
	Enter

	mov edi,[Sp_Dst]		 ; Dst
	mov ecx,[Sp_BpS]      ; BpS
	lea esi,[Sp_Whats0]	 ; Whats
	lea ebp,[Sp_Offsets0] ; Offsets

	mov [.Cont+2],ecx

	mov ecx,[Sp_dWo]	; dWo
	mov eax,[Sp_CMap]	; Cmap

	lea edi, [edi+ecx*2]		; Where += dWo
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	mov [.Map+4], eax
	neg ecx				; i
	mov [.Go_i+3],ebp
	xor eax,eax

.Loop_j:
	dec dword [Sp_dHo]   ; dHo--
	jg .Go_j

	Leave

.Go_j:
	mov ebx, [esi]			; ebx: What
	push ecx					; ecx = dWo
	inc ecx
	jl .Go_i

.Cont:
	add edi, 0x10000000
	pop ecx
	add esi,4
	jmp .Loop_j
.Go_i:
	mov edx, [0x12345678+ecx*4]	; Offsets[i]
	mov bp,[edi+ecx*2]            ; =>*Where
	mov al, [ebx+edx]	 	; What[]
.Map:
	or bp,[0x12345678+eax*2]
	mov [edi+ecx*2],bp	 	; =>*Where
	inc ecx
	jl .Go_i
	jmp .Cont;

;//////////////////////////////////////////////////////////////////////////

Paste_Sprite_Asm_8_Map16:
	Enter

	mov edi,[Sp_Dst]		 ; Dst
	mov ecx,[Sp_BpS]      ; BpS
	lea esi,[Sp_Whats0]	 ; Whats
	lea ebp,[Sp_Offsets0] ; Offsets

	mov [.Cont+2],ecx

	mov ecx,[Sp_dWo]	; dWo
	mov eax,[Sp_CMap]	; Cmap

	lea edi, [edi+ecx*2]		; Where += dWo
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	mov [.Map+4], eax
	neg ecx				; i
	mov [.Go_i+3],ebp
	xor eax,eax

.Loop_j:
	dec dword [Sp_dHo]   ; dHo--
	jg .Go_j

	Leave

.Go_j:
	mov ebx, [esi]			; ebx: What
	push ecx					; ecx = dWo
	inc ecx
	jl .Go_i

.Cont:
	add edi, 0x10000000
	pop ecx
	add esi,4
	jmp .Loop_j
.Go_i:
	mov edx, [0x12345678+ecx*4]	; Offsets[i]
	mov al, [ebx+edx]             ; What[]
	test al,al
	jz .Gah
.Map:
	mov bp,[0x12345678+eax*2]
	mov [edi+ecx*2],bp	 	; =>*Where
;	mov word [edi+ecx*2],0xffff	 	; =>*Where
.Gah:
	inc ecx
	jl .Go_i
	jmp .Cont;

;//////////////////////////////////////////////////////////////////////////

Paste_Sprite_Asm_16:
	Enter

	mov edi,[Sp_Dst]       ; Dst
	mov eax,[Sp_BpS]       ; BpS
	lea esi,[Sp_Whats0]    ; Whats
	lea ebp,[Sp_Offsets0]  ; Offsets

	mov [.Cont+2],eax

	mov ecx,[Sp_dWo]	; dWo

	lea edi, [edi+ecx*2]		; Where += dWo*sizeof(USHORT)
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	neg ecx                 ; i
	xor eax,eax
.Loop_j:
   dec dword [Sp_dHo]
	jg .Go_j

	Leave

.Go_j:
	mov ebx, [esi]			; ebx: What
	push ecx					; ecx = dWo
	inc ecx
	jl .Go_i

.Cont:
	add edi, 0x10000000
	pop ecx
	add esi,4
	jmp .Loop_j
.Go_i:
	mov edx, [ebp+ecx*4]	 ; Offsets[i]
	mov ax, [ebx+2*edx]	 ; 	What[]
	test ax,ax
	jz .Blah
	mov [edi+2*ecx],ax	 ; =>*Where
.Blah:
	inc ecx
	jl .Go_i
	jmp .Cont;

;//////////////////////////////////////////////////////////////////////////

Paste_Sprite_Asm_88:
	Enter

	mov edi,[Sp_Dst]       ; Dst
	mov eax,[Sp_BpS]       ; BpS
	lea esi,[Sp_Whats0]    ; Whats
	lea ebp,[Sp_Offsets0]  ; Offsets

	mov [.Cont+2],eax

	mov ecx,[Sp_dWo]       ; dWo

	lea edi, [edi+ecx*2]		; Where += dWo*sizeof(USHORT)
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	neg ecx                 ; i
	xor eax,eax
.Loop_j:
   dec dword [Sp_dHo]
	jg .Go_j

	Leave

.Go_j:
	mov ebx, [esi]			; ebx: What
	push ecx			; ecx = dWo
	inc ecx
	jl .Go_i

.Cont:
	add edi, 0x10000000
	pop ecx
	add esi,4
	jmp .Loop_j
.Go_i:
	mov edx, [ebp+ecx*4]	 ; Offsets[i]
	mov al, [ebx+edx]	 ; What[]
	test al,al
	jz .Blah
	mov [edi+2*ecx+1],al	 ; =>*Where
.Blah:
	inc ecx
	jl .Go_i
	jmp .Cont;

;//////////////////////////////////////////////////////////////////////////
