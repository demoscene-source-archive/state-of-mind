;//////////////////////////////////////////////////////////////////////////

[BITS 32]

globl Paste_Flare_Asm_16_Sat

extrn Sp_Offsets0   ; INT Sp_Offsets0[ MAX_WIDTH ];
extrn Sp_Whats0     ; void *Sp_Whats0[ MAX_SCANS ];
extrn Sp_Dst        ; UINT *Sp_Dst;
extrn Sp_BpS        ; INT Sp_BpS;
extrn Sp_dWo        ; INT Sp_dWo, Sp_dHo;
extrn Sp_dHo
extrn Sp_CMap       ; void *Sp_CMap

extrn RGB71_To_16
extrn Lo16_To_777
extrn Hi16_To_777

%macro RGB16_TO_777_Add 3      ; %1=b  %2=a %3=c
   mov %{1}l,%{2}l
   add e%{3}x,[Lo16_To_777+e%{1}x*4]      ; Lo16_To_777[In&0xFF]
   mov %{1}l,%{2}h
   add e%{3}x,[Hi16_To_777+e%{1}x*4]      ; Hi16_To_777[In>>8]
%endmacro

%macro RGB_777_TO_16 3      ; %1=b %2=c %3=a   _BX:a,c,b
   mov %{1}l,%{2}l
   mov %{3}x, [RGB71_To_16+256*0+e%{1}x*2]
   mov %{1}l,%{2}h
   or  %{3}x, [RGB71_To_16+256*2+e%{1}x*2]
   shr e%{2}x,16
   or  %{3}x, [RGB71_To_16+256*4+e%{2}x*2]
%endmacro

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

DATA

Paste_Flare_Asm_16_Sat:
	Enter

	mov edi,[Sp_Dst]        ; Where
	lea esi,[Sp_Whats0]     ; Whats
	lea ebp,[Sp_Offsets0]   ; Offsets

	mov ecx,[Sp_dWo]  ; dWo

	lea edi, [edi+ecx*2]    ; Where += dWo
	lea ebp, [ebp+ecx*4]    ; Offsets += dWo
	neg ecx                 ; i
	xor eax,eax
	inc ecx 
	jge near .End
	mov [.Go_i+0x03],ebp
	mov [Sp_dWo],ecx
   mov ecx,[Sp_CMap]
   mov [.Map+0x03],ecx
	dec dword [Sp_dHo]
	jl .End

.Go_j:
	push esi
	mov edx, [esi]    ; ebx: What
	mov ebp,[Sp_dWo]

.Go_i:   ; free: ebx, ecx
	mov esi, [0x12345678+ebp*4]    ; Offsets[i]
	mov al, [edx+esi]     ; What[]

	test al, al	
	jz .Blah
.Map:
	mov ecx, [0x12345678+eax*4]    ; Color 0x777
   mov bx, [edi+ebp*2]
	RGB16_TO_777_Add a,b,c

	RGB_777_TO_16 a,c,b
	mov [edi+ebp*2],bx      ; => Where[]
.Blah:
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
