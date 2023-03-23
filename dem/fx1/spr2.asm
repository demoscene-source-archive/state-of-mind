;//////////////////////////////////////////////////////////////////////////

[BITS 32]

globl Paste_Sprite_Asm_32
globl Paste_Sprite_Asm_Sat_32
globl Paste_Sprite_Asm_Mask_32
globl Paste_Sprite_Asm_Raw_Mask_32

extrn Offsets0   ; INT Offsets0[ MAX_WIDTH ];
extrn Whats0     ; void *Whats0[ MAX_SCANS ];
extrn Sp_Dst     ; UINT *Sp_Dst;
extrn Sp_BpS     ; INT Sp_BpS;  
extrn dWo        ; INT dWo, dHo;
extrn dHo

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
	lea esi,[Whats0]     ; Whats
	lea ebp,[Offsets0]	; Offsets

	mov [Cont_j_32+2],eax

	mov ecx,[dWo]	; dWo
	mov edx,[dHo]	; dHo

	lea edi, [edi+ecx*4]		; Where += dWo*sizeof(UINT)
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	neg ecx				; i
	neg edx				; j
	xor eax,eax
Loop_j_32:
	inc edx
	jl Go_j_32

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
	add edi, 0x10000000
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

	mov edi,[Sp_Dst]     ; Dst
	mov eax,[Sp_BpS]	   ; BpS
	lea esi,[Whats0]     ; Whats
	lea ebp,[Offsets0]	; Offsets

	mov [Cont_j_s32+2],eax

	mov ecx,[dWo]	; dWo
   lea ebp, [ebp+ecx*4]		; Offsets += dWo
	mov [Go_i_s32+0x03],ebp	
   lea edi, [edi+ecx*4]		; Where += dWo*sizeof(UINT)
	mov ebp,[dHo]	; dHo
	neg ebp				      ; j	
	neg ecx				      ; i
	xor ebx,ebx
Loop_j_s32:
	inc ebp
	jl Go_j_s32

	Leave

Go_j_s32:
	mov eax, [esi]			; eax: What
	push ecx					; ecx = dWo
	mov [Go_i_s32+0x0a],eax
	push esi
	xor esi,esi
Loop_i_s32:
	inc ecx
	jge Cont_j_s32

Go_i_s32:
	mov esi, [0x12345678+ecx*4]   ; Offsets[i]
	mov eax, [0x12345678+4*esi]   ; What[]
	add eax, [edi+4*ecx]          ; 	+dst
	RGB_777_TO_777                ; eax (+ebx) =>edx
	mov [edi+4*ecx],edx	         ; =>*Where
	inc ecx
	jl Go_i_s32

Cont_j_s32:
	add edi, 0x10000000
	pop esi
	pop ecx
	add esi,4
	jmp Loop_j_s32

;//////////////////////////////////////////////////////////////////////////

Paste_Sprite_Asm_Mask_32:
	Enter

	mov edi,[Sp_Dst]     ; Dst
	mov eax,[Sp_BpS]	   ; BpS
	lea esi,[Whats0]     ; Whats
	lea ebp,[Offsets0]	; Offsets

	mov [Cont_j_m32+2],eax

	mov ecx,[dWo]	; dWo
	mov edx,[dHo]	; dHo
	lea edi, [edi+ecx*4]		; Where += dWo*sizeof(UINT)
	neg edx				      ; j
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	neg ecx				      ; i
	mov [Go_i_m32+0x03],ebp
	xor ebx,ebx
Loop_j_m32:
	inc edx
	jl Go_j_m32

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

	mov edi,[Sp_Dst]     ; Dst
	mov eax,[Sp_BpS]	   ; BpS
	lea esi,[Whats0]     ; Whats
	lea ebp,[Offsets0]	; Offsets

	mov [Cont_j_rm32+2],eax

	mov ecx,[dWo]	; dWo
	mov edx,[dHo]	; dHo
	lea edi, [edi+ecx*4]		; Where += dWo*sizeof(UINT)
	neg edx				      ; j
	lea ebp, [ebp+ecx*4]		; Offsets += dWo
	neg ecx				      ; i
	mov [Go_i_rm32+0x03],ebp
	xor ebx,ebx
Loop_j_rm32:
	inc edx
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

