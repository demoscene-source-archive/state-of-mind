;//////////////////////////////////////////////////////////////////////////

            ; UNUSED IN THE DEMO !!


[BITS 32]
globl Do_Da_Bump
globl Do_Da_Bump_2
globl Paste_III
globl Paste_III_Map

%define CLAMP_OFF	320
extrn I_Table

%define The_W  320
%define The_W2  640

;//////////////////////////////////////////////////////////////////////////

DATA

Do_Da_Bump:    ; +0:  PIXEL *Dst       +4:  INT Dst_BpS
               ; +8:  PIXEL *Src       +12: PIXEL *Env_Map
               ; +16: INT xo           +20: INT yo

	Enter

	mov edi,[esp+OFF_STCK+0]   ; Dst	
	add edi,304

	mov esi,[esp+OFF_STCK+4]   ; Dst_BpS
	mov [.I4_2+2],esi

	mov esi,[esp+OFF_STCK+8]   ; Src
	add esi,304

	mov eax,[esp+OFF_STCK+12]  ; Env_Map
	mov [.I3+0x02],eax			; =>Env_Map
	mov [.I3_2+0x02],eax		; =>Env_Map

	mov eax,[esp+OFF_STCK+16]  ; 	xo
	sub eax,304
	mov [.I1+0x01],eax
	mov [.I1_2+0x01],eax

	mov eax,[esp+OFF_STCK+20]  ; 	yo
	sub eax,184
	mov [.j3+0x02],eax		; =>(yo-j)   = jo

	xor ebx,ebx
	mov ecx,-184

.j3:
	push ecx
	mov edx, 0x1234578
	sub edx, ecx					; edx = jo
	mov ecx, -304
	mov [.I2+0x01],edx
	mov [.I2_2+0x01],edx

.i3:
	mov dx,[esi+ecx]
	mov al,dh
	sub al,dl
	movsx eax,al
.I1:
	add eax, 0x12345678
	sub eax, ecx
	mov bl,[CLAMP_OFF+I_Table+eax]

	mov al,[esi+ecx+320]
	sub al,dl
	movsx eax,al
.I2:
	add eax, 0x12345678
	mov bh,[CLAMP_OFF+I_Table+eax]
.I3:
	mov dl,[0x12345678+ebx]				; Env_Map

	inc ecx

	mov al,[esi+ecx+1]
	sub al,dh
	movsx eax,al
.I1_2:
	add eax, 0x12345678
	sub eax, ecx
	mov bl,[CLAMP_OFF+I_Table+eax]

	mov al,[esi+ecx+320]
	sub al,dh
	movsx eax,al
.I2_2:
	add eax, 0x12345678
	mov bh,[CLAMP_OFF+I_Table+eax]
.I3_2:
	mov dh,[0x12345678+ebx]				; Env_Map
	mov [edi+ecx-1],dx

	inc ecx

	jl near .i3

.I4_2:
	add edi,0x12345678
	add esi,320
	pop ecx
	add ebp, 320
	inc ecx
	jl near .j3

	Leave

;//////////////////////////////////////////////////////////////////////////

Do_Da_Bump_2:  ; +0:  PIXEL *Src/*Dst  +4:  INT Dst_BpS
               ; +8:  PIXEL *Env_Map
               ; +12: INT xo           +16: INT yo

	Enter

	mov edi,[esp+OFF_STCK+0]   ; Dst	
	add edi,304

	mov eax,[esp+OFF_STCK+4]   ; Dst_BpS
	mov [.I4_2+2],eax

	mov eax,[esp+OFF_STCK+8]  ; Env_Map
	mov [.I3+0x02],eax			; =>Env_Map
	mov [.I3_2+0x02],eax		; =>Env_Map

	mov eax,[esp+OFF_STCK+12]  ; 	xo
	sub eax,304
	mov [.I1+0x01],eax
	mov [.I1_2+0x01],eax

	mov eax,[esp+OFF_STCK+16]  ; 	yo
	sub eax,184
	mov [.j3+0x02],eax		; =>(yo-j)   = jo

	xor ebx,ebx
	mov ecx,-184

.j3:
	push ecx
	mov edx, 0x1234578
	sub edx, ecx					; edx = jo
	mov ecx, -304
	mov [.I2+0x01],edx
	mov [.I2_2+0x01],edx

.i3:
	mov dx,[edi+ecx]
	mov al,dh
	sub al,dl
	movsx eax,al
.I1:
	add eax, 0x12345678
	sub eax, ecx
	mov bl,[CLAMP_OFF+I_Table+eax]

	mov al,[edi+ecx+320]
	sub al,dl
	movsx eax,al
.I2:
	add eax, 0x12345678
	mov bh,[CLAMP_OFF+I_Table+eax]
.I3:
	mov dl,[0x12345678+ebx]				; Env_Map

	inc ecx

	mov al,[edi+ecx+1]
	sub al,dh
	movsx eax,al
.I1_2:
	add eax, 0x12345678
	sub eax, ecx
	mov bl,[CLAMP_OFF+I_Table+eax]

	mov al,[edi+ecx+320]
	sub al,dh
	movsx eax,al
.I2_2:
	add eax, 0x12345678
	mov bh,[CLAMP_OFF+I_Table+eax]
.I3_2:
	mov dh,[0x12345678+ebx]				; Env_Map
	mov [edi+ecx-1],dx

	inc ecx

	jl near .i3

.I4_2:
	add edi,0x12345678
	pop ecx
	add ebp, 320
	inc ecx
	jl near .j3

	Leave

;//////////////////////////////////////////////////////////////////////////

Paste_III: 			; Dst, Dst_BpS, Src, W, H, Src_BpS

	Enter

	mov edi,[esp+OFF_STCK+0]   ; Dst	
	mov eax,[esp+OFF_STCK+4]   ; Dst_BpS
	mov [.III+2], eax
	mov esi,[esp+OFF_STCK+8]   ; Src
	mov ebx,[esp+OFF_STCK+12]  ; W
	mov ecx,[esp+OFF_STCK+16]  ; H
	mov eax,[esp+OFF_STCK+20]  ; Src_BpS
	mov [.III+8],eax

	add esi,ebx
	add edi,ebx
	neg ebx

.y:

	mov edx,ebx
.x:
	mov al, [esi+edx]
	test al,al
	je .Skip
	mov [edi+edx],al
.Skip:
	inc edx
	jl .x

.III:
	add edi,0x12345678
	add esi,0x12345678
	dec ecx
	jg .y
	Leave

;//////////////////////////////////////////////////////////////////////////

Paste_III_Map: 			; Dst, Dst_BpS, Src, W, H, Src_BpS, CMap

	Enter

	mov edi,[esp+OFF_STCK+0]   ; Dst	
	mov eax,[esp+OFF_STCK+4]   ; Dst_BpS
	mov [.III_O+2], eax
	mov esi,[esp+OFF_STCK+8]   ; Src	
	mov edx,[esp+OFF_STCK+12]  ; W
	mov ebp,[esp+OFF_STCK+16]  ; H
	mov eax,[esp+OFF_STCK+20]  ; Src_BpS
	mov [.III_O+8],eax
	mov eax,[esp+OFF_STCK+20]  ; CMap

	add esi,edx
	add edi,edx
	xor eax,eax
	neg edx
	xor ebx,ebx

.C_y:
   push edx

.C_x:
	mov al, [esi+edx]
	test al,al
	je .C_Skip
	mov bx,[ecx+eax*2]      ; USHORT *CMap
	mov [edi+edx*2],bx
.C_Skip:
	inc edx
	jl .C_x
   pop edx

.III_O:
	add edi,0x12345678
	add esi,0x12345678
	dec ebp
	jg .C_y
	Leave

;//////////////////////////////////////////////////////////////////////////

