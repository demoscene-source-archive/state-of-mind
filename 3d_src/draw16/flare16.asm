;//////////////////////////////////////////////////////////////////////////

[BITS 32]

globl Paste_Flare_Asm_16

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

Paste_Flare_Asm_16:
	Enter

	mov edi,[Sp_Dst]        ; Where
	lea esi,[Sp_Whats0]     ; Whats
	lea ebp,[Sp_Offsets0]   ; Offsets

	mov ecx,[Sp_dWo]  ; dWo

	lea edi, [edi+ecx*2]    ; Where += dWo
	lea ebp, [ebp+ecx*4]    ; Offsets += dWo
	neg ecx                 ; i
	mov [.Go_i+0x03],ebp
	inc ecx 
	jge .End
	mov [Sp_dWo],ecx

	dec dword [Sp_dHo]
	jle .End

.Go_j:
	mov edx, [esi]    ; ebx: What
	push esi
	mov ebp,[Sp_dWo]

.Go_i:   ; free: ebx, ecx
	mov esi, [0x12345678+ebp*4]    ; Offsets[i]
	mov ax, [edx+esi*2]     ; What[]
	mov [edi+ebp*2],ax      ; => Where[]
	inc ebp
	jl .Go_i

	pop esi
	add edi, [Sp_BpS]
	add esi,4
	dec dword [Sp_dHo]
	jg .Go_j
.End:
	Leave

;//////////////////////////////////////////////////////////////////////////
