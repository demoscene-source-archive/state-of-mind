;//////////////////////////////////////////////////////////////////////////

          ; UNUSED IN THE DEMO !!

[BITS 32]
globl Map_Tunnel
globl Map_Tunnel_Add
globl Map_Tunnel_II

%define CLAMP_OFF	320
extrn I_Table

%define The_W  320
%define The_W2  640

;//////////////////////////////////////////////////////////////////////

DATA

;//////////////////////////////////////////////////////////////////////

Map_Tunnel:
	Enter
	mov edi,[esp+OFF_STCK+0]   ; Dst
	mov ebp,[esp+OFF_STCK+4]   ; Map
	mov esi,[esp+OFF_STCK+8]   ; Src

	xor eax,eax
	mov bx,[esp+OFF_STCK+12]   ; UoVo

	add edi,2872			; 8*The_W+8 + (The_W-16)
;	add esi,11472			; (8*The_W2+8 + (The_W2-32))*2
   add esi,640

	mov ecx, 200-16				; j
.j1:
	push ecx
	mov ecx, -152			; -(320-16)/2
.i1:
	mov ax,[esi+4*ecx]
;	add ax,bx
	add al,bl
	add ah,bh
	mov dl,[ebp+eax]
	mov ax,[esi+4*ecx+2]
;	add ax,bx
	add al,bl
	add ah,bh
	mov dh,[ebp+eax]
	mov [edi+2*ecx],dx

	inc ecx
	jl .i1	

	add edi, The_W
	pop ecx
	add esi, The_W2*2

	dec ecx
	jge .j1

	Leave

;//////////////////////////////////////////////////////////////////////////

Map_Tunnel_Add:

	Enter
	mov edi,[esp+OFF_STCK+0]   ; Dst

	mov ebp,[esp+OFF_STCK+4]   ; Map
	mov dword [.Add_i1+0x13], ebp
	mov dword [.Add_i1+0x2b], ebp

	mov esi,[esp+OFF_STCK+8]   ; Src1
	xor eax,eax

	mov ebx,[esp+OFF_STCK+16]   ; Src2

	mov dx,[esp+OFF_STCK+12]   ; UoVo1
	mov cx,[esp+OFF_STCK+20]   ; UoVo2
	add dl,cl
	mov [.Add_i1+0x05], dl
	mov [.Add_i1+0x1d], dl
	add dh,ch
	mov [.Add_i1+0x0c], dh
	mov [.Add_i1+0x24], dh

	add edi,2872			; 8*The_W+8 + (The_W-16)
   add esi,The_W2
   add ebx,The_W2
	mov ecx, 200-16				; j

.Add_j1:
   push ecx
	mov ebp, -152			; -(320-16)/2
.Add_i1:
	mov ax,[esi+4*ebp]   ; +Src1
   add al,0x12
   mov cx,[ebx+4*ebp]   ; +Src2
   add ah,0x34
   add al,cl
   add ah,ch
	mov dl,[0x12345678+eax] ; Map[uv]

	mov ax,[esi+4*ebp+2]
	add al,0x12
	mov cx,[ebx+4*ebp]   ; +Src2
	add ah,0x34
	add al,cl
   add ah,ch
	mov dh,[0x12345678+eax]
	mov [edi+2*ebp],dx

	inc ebp
	jl .Add_i1	

	add edi, The_W
	pop ecx
	add esi, The_W2*2
	add ebx, The_W2*2
	dec ecx
	jge .Add_j1

	Leave

;//////////////////////////////////////////////////////////////////////////

Map_Tunnel_II:

	Enter
	mov edi,[esp+OFF_STCK+0]   ; Dst
	add edi,2872					; 8*320+8 + (320-16)
	mov [.Map_22+0x10],edi

	mov edi,[esp+OFF_STCK+4]   ; Dst2
	add edi,2872					; 8*320+8 + (320-16)

	mov ebp,[esp+OFF_STCK+8]   ; Map
	mov [.Map_21+0x02], ebp
	mov [.Map_22+0x02], ebp

	mov ebp,[esp+OFF_STCK+12]  ; Map2
	mov [.Map_21+0x08], ebp
	mov [.Map_22+0x08], ebp

	mov esi,[esp+OFF_STCK+16]   ; Src
;	add esi,5744					 ; (8*320+8 + (320-16))*2

	xor eax,eax
	mov bp, [esp+OFF_STCK+20]   ; UoVo

	mov ecx, 200-16				; j
.j2:
	push ecx
	mov ecx, -152					; -(320-16)/2
.i2:
	mov ax, [esi+4*ecx]					; esi		+0x04		Src
	add ax,bp
.Map_21:
	mov dl,[0x12345678+eax]				;			+0x0d		Map
	mov bl,[0x12345678+eax]				;  		+0x13    Map2
	mov ax, [esi+4*ecx+2]					; esi		+0x1b		Src
	add ax,bp
.Map_22:
	mov dh,[0x12345678+eax]				; 			+0x24    Map
	mov bh,[0x12345678+eax]				; 			+0x2a		Map2
	mov [0x12345678+2*ecx],dx			; edi		+0x32		Dst
	mov [edi+2*ecx],bx					; edi		+0x3a		Dst2

	inc ecx
	jl .i2

	add dword [.Map_22+0x10],320	; Dst
	add edi,The_W					; Dst2
	pop ecx
	add esi,The_W2*2

	dec ecx
	jge near .j2

	Leave

;//////////////////////////////////////////////////////////////////////////

