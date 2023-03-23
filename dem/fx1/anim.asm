;////////////////////////////////////////////////////////////////////

[BITS 32]
globl JPaste_I
globl JPaste_II
globl JPaste_III
globl JPaste_IV

;////////////////////////////////////////////////////////////////////

DATA

JPaste_I:    ; +0: VBUFFER *Out   +4:  BYTE *Src
             ; +8: INT W          +12: INT H

	Enter
	mov eax,[esp+OFF_STCK+0]   ; Out
	mov ecx, [esp+OFF_STCK+8]   ; W
	mov edi,[eax+VB_BITS]
	mov esi, [esp+OFF_STCK+4]   ; Src
	add edi,ecx
	shr dword [esp+OFF_STCK+12],1     ; H /= 2
	shr ecx,1                   ; W /= 2
	add esi,ecx
	mov edx,[eax+VB_BPS]        ; edx = BpS
   neg ecx
   mov [esp+OFF_STCK+8],ecx
.J_X
   mov al,[esi+ecx]
   mov [edi+2*ecx],al
   inc ecx
   jl .J_X
   mov al,[edi-2]
   mov [edi-1],al
   mov ecx,[esp+OFF_STCK+8]
   lea edi,[edi+2*edx]
   sub esi,ecx                ; Bits -= (-W)
   dec dword [esp+OFF_STCK+12]
   jg .J_X
   lea edi,[edi+2*ecx]        ; Dst += 2*(-W)
   sub edi,edx
   mov esi,edi
   sub esi,edx
   neg ecx
   cld
   rep movsw
   Leave

;////////////////////////////////////////////////////////////////////

JPaste_II:    ; +0: VBUFFER *Out   +4:  BYTE *Src
             ; +8: INT W          +12: INT H

	Enter
	mov eax,[esp+OFF_STCK+0]   ; Out
	mov ecx, [esp+OFF_STCK+8]   ; W
	mov edi,[eax+VB_BITS]
	mov esi, [esp+OFF_STCK+4]   ; Src
	add edi,ecx
	shr dword [esp+OFF_STCK+12],1     ; H /= 2
	shr ecx,1                   ; W /= 2
	add esi,ecx
	mov edx,[eax+VB_BPS]        ; edx = BpS
   neg ecx
   xor eax,eax
   xor ebx,ebx
   mov [esp+OFF_STCK+8],ecx
.J_X
   mov al,[esi+ecx]
   mov bl,[edi+2*ecx]
   mov [edi+2*ecx],al
   mov al,[edi+2*ecx+2]
   add ax,bx
   shr ax,1
   mov [edi+2*ecx+1],al
   inc ecx
   jl .J_X
   mov al,[edi-2]
   mov [edi-1],al
   mov ecx,[esp+OFF_STCK+8]
   lea edi,[edi+2*edx]
   sub esi,ecx                ; Bits -= (-W)
   dec dword [esp+OFF_STCK+12]
   jg .J_X
   Leave

;////////////////////////////////////////////////////////////////////

JPaste_III:    ; +0: VBUFFER *Out   +4:  BYTE *Src
             ; +8: INT W          +12: INT H

   Enter
   mov eax,[esp+OFF_STCK+0]   ; Out
   mov ecx, [esp+OFF_STCK+8]   ; W
   mov edi,[eax+VB_BITS]
   mov esi, [esp+OFF_STCK+4]   ; Src
   add edi,ecx
   shr dword [esp+OFF_STCK+12],1     ; H /= 2

   shr ecx,1                   ; W = W/2-1
   sub edi,2		       ; edi -=2
   dec ecx		
   add esi,ecx
   mov edx,[eax+VB_BPS]        ; edx = BpS
   neg ecx
   xor eax,eax
   xor ebx,ebx
   mov [esp+OFF_STCK+8],ecx
   mov [.J_X+0x06],edx
   inc edx
   mov [.J_X+0x1d],edx
   inc edx
   mov [.J_X+0x10],edx
   sub edx,2
.J_X
   mov al,[esi]
   inc esi
   mov bl,[edi+2*ecx+0x12345678]    ; +320
   mov [edi+2*ecx],al
   mov al,[edi+2*ecx+0x12345678]    ; +322
   add ax,bx
   shr ax,1
   mov [edi+2*ecx+0x12345678],al    ; +321
   inc ecx
   jl .J_X
   mov al,[edi+edx+0];
   mov bl,[edi+edx+1]
   add ax,bx
   mov bl,[esi]
   shr ax,1
   mov [edi],bl
   inc esi
   mov [edi+edx+1],al
   mov ecx,[esp+OFF_STCK+8]
;   mov [edi-1],al
   lea edi,[edi+2*edx]
;   sub esi,ecx                ; Bits -= (-W)
   dec dword [esp+OFF_STCK+12]
   jg .J_X
   Leave

;////////////////////////////////////////////////////////////////////

JPaste_IV:    ; +0: VBUFFER *Out   +4:  BYTE *Src
             ; +8: INT W          +12: INT H

	Enter
	mov eax,[esp+OFF_STCK+0]   ; Out
	mov ecx, [esp+OFF_STCK+8]   ; W
	mov edi,[eax+VB_BITS]
	mov esi, [esp+OFF_STCK+4]   ; Src
	add edi,ecx
	shr dword [esp+OFF_STCK+12],1     ; H /= 2
	shr ecx,1                   ; W /= 2
	dec dword [esp+OFF_STCK+12]
	add esi,ecx
	mov edx,[eax+VB_BPS]        ; edx = BpS
   neg ecx
   xor ebx,ebx
   mov [esp+OFF_STCK+8],ecx
   mov [.J_X+0x19],edx
   lea eax,[edx*2]
   mov [.J_X+0x0c],eax
   xor eax,eax
.J_X
   mov al,[esi+ecx]
   mov bl,[edi+2*ecx]    ; 0
   mov [edi+2*ecx],al
   mov al,[edi+2*ecx+2+0x12345678]  ; +640
   add ax,bx
   shr ax,1
   mov [edi+2*ecx+0x12345678],al    ; +320
   inc ecx
   jl .J_X
   mov al,[edi-2]
   mov ecx,[esp+OFF_STCK+8]
   mov [edi-1],al
   lea edi,[edi+2*edx]
   sub esi,ecx                ; Bits -= (-W)
   dec dword [esp+OFF_STCK+12]
   jg .J_X
   Leave

;////////////////////////////////////////////////////////////////////
