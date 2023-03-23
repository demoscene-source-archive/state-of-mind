;//////////////////////////////////////////////////////////////////////////

[BITS 32]

;//////////////////////////////////////////////////////////////////////////

globl Paste_Sprite_Asm_32
globl Paste_Sprite_Asm_Sat_32
globl Paste_Sprite_Asm_Mask_32
globl Paste_Sprite_Asm_Raw_Mask_32

extrn Sp_Offsets0   ; INT Sp_Offsets0[ MAX_WIDTH ];
extrn Sp_Whats0     ; void *Sp_Whats0[ MAX_SCANS ];
extrn Sp_Dst        ; UINT *Sp_Dst;
extrn Sp_BpS        ; INT Sp_BpS;  
extrn Sp_dWo        ; INT Sp_dWo, Sp_dHo;
extrn Sp_dHo

extrn R_Remap777
extrn G_Remap777
extrn B_Remap777

   ; eax (+ebx) =>edx
%macro RGB_777_TO_777 0
   mov bl,al
   mov edx, [B_Remap777+ebx*4]
   mov bl,ah
   or  edx, [G_Remap777+ebx*4]
   shr eax,16
   mov bl,al
   or  edx, [R_Remap777+ebx*4]
%endmacro

   ; edx (+ebx) + eax => eax        ; bh = mask
%macro RGB_777_TO_777_BLEND 0
   mov bl,dl
   add eax, [B_Remap777+ebx*4]
   mov bl,dh
   add  eax, [G_Remap777+ebx*4]
   shr edx,16
   mov bl,dl
   add  eax, [R_Remap777+ebx*4]
%endmacro


;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

DATA

Paste_Sprite_Asm_32:
	Enter

	mov edi,[Sp_Dst]     ; Dst
	mov eax,[Sp_BpS]	   ; BpS
	lea esi,[Sp_Whats0]     ; Whats
	lea ebp,[Sp_Offsets0]	; Offsets

	mov ecx,[Sp_dWo]	; dWo

	lea edi, [edi+ecx*4]		; Where += dWo*sizeof(UINT)
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	neg ecx				; i
	xor eax,eax
Loop_j_32:
	dec dword [Sp_dHo]
	jg Go_j_32

	Leave

Go_j_32:
	mov ebx, [esi]			; ebx: What
	push ecx             ; ecx = dWo
	push esi
	xor esi,esi
Loop_i_32:
	inc ecx
	jl Go_i_32

Cont_j_32:
	add edi, dword [Sp_BpS]
	pop esi
	pop ecx
	add esi,4
	jmp Loop_j_32
Go_i_32:
	mov esi, [ebp+ecx*4]	 ; Offsets[i]
	mov eax, [ebx+4*esi]	 ; 	What[]
	mov [edi+4*ecx],eax	 ; =>*Where
	inc ecx
	jl Go_i_32
	jmp Cont_j_32;

;//////////////////////////////////////////////////////////////////////////

Paste_Sprite_Asm_Sat_32:
	Enter

	mov edi,[Sp_Dst]      ; Dst
	mov eax,[Sp_BpS]      ; BpS
	lea esi,[Sp_Whats0]   ; Whats
	lea ebp,[Sp_Offsets0] ; Offsets

	mov ecx,[Sp_dWo]	; dWo
        lea ebp, [ebp+ecx*4]	; Offsets += dWo
        lea edi, [edi+ecx*4]	; Where += dWo*sizeof(UINT)
	neg ecx			; i
	xor ebx,ebx
.Loop_j:
	dec dword [Sp_dHo]
	jg .Go_j

	Leave

.Go_j:
	mov eax, [esi]		; eax: What
	push ecx		; ecx = dWo
	mov [.Go_i+0x07],eax
	push esi
	xor esi,esi
.Loop_i:
	inc ecx
	jge .Cont_j

.Go_i:
	mov esi, [ebp+ecx*4]   ; Offsets[i]
	mov eax, [0x12345678+4*esi]   ; What[]
	add eax, [edi+4*ecx]          ; 	+dst
	RGB_777_TO_777                ; eax (+ebx) =>edx
	mov [edi+4*ecx],edx	         ; =>*Where
	inc ecx
	jl .Go_i

.Cont_j:
	add edi, dword [Sp_BpS]
	pop esi
	pop ecx
	add esi,4
	jmp .Loop_j

;//////////////////////////////////////////////////////////////////////////

Paste_Sprite_Asm_Mask_32:
	Enter

	mov edi,[Sp_Dst]      ; Dst
	mov eax,[Sp_BpS]	    ; BpS
	lea esi,[Sp_Whats0]   ; Whats
	lea ebp,[Sp_Offsets0] ; Offsets

	mov [Cont_j_m32+2],eax
	mov ecx,[Sp_dWo]	; dWo
	lea edi, [edi+ecx*4]		; Where += dWo*sizeof(UINT)
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	neg ecx				      ; i
	mov [Go_i_m32+0x03],ebp
	xor ebx,ebx
Loop_j_m32:
	dec dword [Sp_dHo]	; dHo
	jg Go_j_m32

	Leave

Go_j_m32:
   push edx
	mov eax, [esi]			; eax: What
	push ecx					; ecx = dWo
	mov [Go_i_m32+0x0a],eax
	push esi
	xor esi,esi
Loop_i_m32:
	inc ecx
	jge Cont_j_m32

Go_i_m32:
	mov esi, [0x12345678+ecx*4]   ; Offsets[i]
	mov ebx, [0x12345678+4*esi]   ; What[]
	mov eax,ebx
	shr ebx,16
	test bh,bh
	jz Skip_m32

	mov edx, [edi+4*ecx]	         ; 	+dst
   RGB_777_TO_777_BLEND          ; edx + (ebx) + eax => eax
	RGB_777_TO_777                ; eax (+ebx) =>edx
	mov [edi+4*ecx],edx	         ; =>*Where
	inc ecx
	jl Go_i_m32
   jmp Cont_j_m32

Skip_m32
	mov [edi+4*ecx],eax	         ; =>*Where
	inc ecx
	jl Go_i_m32

Cont_j_m32:
	add edi, 0x10000000
	pop esi
	pop ecx
	add esi,4
	pop edx
	jmp Loop_j_m32

;//////////////////////////////////////////////////////////////////////////

Paste_Sprite_Asm_Raw_Mask_32:
	Enter

	mov edi,[Sp_Dst]      ; Dst
	mov eax,[Sp_BpS]	    ; BpS
	lea esi,[Sp_Whats0]   ; Whats
	lea ebp,[Sp_Offsets0] ; Offsets

	mov [Cont_j_rm32+2],eax

	mov ecx,[Sp_dWo]	; dWo
	lea edi, [edi+ecx*4]		; Where += dWo*sizeof(UINT)
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	neg ecx				      ; i
	mov [Go_i_rm32+0x03],ebp
	xor ebx,ebx
Loop_j_rm32:
	dec dword [Sp_dHo]
	jl Go_j_rm32

	Leave

Go_j_rm32:
   push edx
	mov eax, [esi]			; eax: What
	push ecx					; ecx = dWo
	mov [Go_i_rm32+0x0a],eax
	push esi
	xor esi,esi
Loop_i_rm32:
	inc ecx
	jge Cont_j_rm32

Go_i_rm32:
	mov esi, [0x12345678+ecx*4]   ; Offsets[i]
	mov ebx, [0x12345678+4*esi]   ; What[]
	mov eax,ebx
	shr ebx,16
	test bh,bh
	jz Skip_rm32
	mov eax, [edi+4*ecx]	         ; 	+dst
Skip_rm32
	mov [edi+4*ecx],eax	         ; =>*Where
	inc ecx
	jl Go_i_rm32

Cont_j_rm32:
	add edi, 0x10000000
	pop esi
	pop ecx
	add esi,4
	pop edx
	jmp Loop_j_rm32

;//////////////////////////////////////////////////////////////////////////

