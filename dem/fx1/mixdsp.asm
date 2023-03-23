;//////////////////////////////////////////////////////////////////////////

[BITS 32]

globl Mix_Dsp_Off
globl Mix_Dsp_Off_Safe_X
globl Mix_Dsp_Off_Safe_Y
globl Mix_Dsp_Off_Blur_X
globl Mix_Dsp_Off_Blur_Y

extrn RGB71_To_16

%macro RGB_777_TO_16_BX 0
   mov al,cl
   mov bx, [RGB71_To_16+256*0+eax*2]
   mov al,ch
   shr ecx,16
   or  bx, [RGB71_To_16+256*2+eax*2]
   or  bx, [RGB71_To_16+256*4+ecx*2]
%endmacro

%define THE_W        320         ; WARNING!!!

;//////////////////////////////////////////////////////////////////////////

DATA

Mix_Dsp_Off:   ; +0: Out  +4: In   +8 BYTE *Dsp   +12: INT *Off

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = (USHORT*)Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x08],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]        ; esi = (USHORT*)Src
   mov [.j1+0x02],ebx

   mov ecx,[esp+OFF_STCK+12]   ; Off
   mov [.i2+0x08],ecx

   mov ecx,[esp+OFF_STCK+8]   ; Dsp
   lea ecx,[ecx+ebp]

   mov edx,[eax+VB_H]   ; H
   neg ebp
   xor ebx,ebx
   dec edx
   jl .End

.j2:
   push ebp            ; saves i=-W

align4
.i2:
   mov bl, [ecx+ebp]           ; Dsp[i]
   mov eax,ebp
   add eax, [0x12345678+4*ebx] ; Off[Dsp[i]]
   mov ax, [esi+eax*2]         ; Src[i']
   mov [edi+ebp*2],ax

   inc ebp
   jl near .i2

   pop ebp
.j1:
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst
   add ecx,THE_W        ; Dsp
   dec edx
   jge near .j2

.End:
   Leave

;////////////////////////////////////////////////////////////////////

Mix_Dsp_Off_Safe_X: ; +0: Out  +4: In   +8 BYTE *Dsp  
                    ; +12: BYTE *Off   +16: INT *Sat_Tab

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = (USHORT*)Dst
   mov ebx,[eax+VB_BPS]
   mov [.x_j1+0x08],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   mov [.x_j1+0x02],ebx

   mov ecx,[esp+OFF_STCK+12]   ; Off
   mov [.x_i2+0x08],ecx

   mov ecx,[esp+OFF_STCK+16]   ; Sat_Tab
   lea ecx,[ecx+ebp*4]         ; +Out->W
   mov [.x_i2+0x0f],ecx

   mov ecx,[esp+OFF_STCK+8]   ; Dsp
   lea ecx,[ecx+ebp]

   mov edx,[eax+VB_H]   ; H
   neg ebp
   xor ebx,ebx
   dec edx
   jl .End

.x_j2:
   push ebp            ; saves i=-W

align4
.x_i2:
   mov bl, [ecx+ebp]            ; Dsp[i]
   mov eax,ebp
   add eax, [0x12345678+4*ebx]  ; i'=Off[Dsp[i]]
   mov eax, [0x12345678+4*eax]  ; i'=Sat[i]
   mov ax, [esi+eax*2]     ; Src[i']
   mov [edi+ebp*2],ax

   inc ebp
   jne near .x_i2

   pop ebp
.x_j1:
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst
   add ecx,THE_W        ; Dsp

   dec edx
   jge near .x_j2

.End:
   Leave

;////////////////////////////////////////////////////////////////////

Mix_Dsp_Off_Safe_Y: ; +0: Out  +4: In   +8 BYTE *Dsp  
                    ; +12: BYTE *Off   +16: INT *Sat_Tab

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = (USHORT*)Dst
   mov ebx,[eax+VB_BPS]       ; Dst_BpS
   mov [.y_j1+0x02],ebx          

   mov ecx,[esp+OFF_STCK+4]   ; In
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]        ; esi = (USHORT*)Src
   mov [.y_i2+0x15],esi

   mov ecx,[esp+OFF_STCK+12]   ; Off
   mov [.y_i2+0x06],ecx

   mov edx,[eax+VB_H]   ; H

   mov ecx,[esp+OFF_STCK+8]   ; Dsp
   lea ecx,[ecx+ebp]

   neg ebp
   xor ebx,ebx
   dec edx
   jl .End

   mov esi,[esp+OFF_STCK+16]   ;   esi = Sat_Tab
   lea esi,[esi+edx*4-4]         ; +Out->H

.y_j2:
   push ebp            ; saves i=-W

align4
.y_i2:
   mov bl, [ecx+ebp]            ; Dsp[i]
   mov eax, [0x12345678+4*ebx]  ; i'=Off[Dsp[i]]
   sub eax,edx
   mov eax, [esi+4*eax]  ; i'=Sat[i]
   add eax,ebp
   mov ax, [0x12345678+eax*2]     ; Src[i']
   mov [edi+ebp*2],ax

   inc ebp
   jne near .y_i2

   pop ebp
.y_j1:
   add edi,0x12345678   ; Dst
   add ecx,THE_W        ; Dsp
   dec edx
   jge near .y_j2

.End:
   Leave

;////////////////////////////////////////////////////////////////////
;     Disabled for now, since not used ...
;////////////////////////////////////////////////////////////////////

%ifdef DASDASJKDLASJKDLAS

Mix_Dsp_Off_Blur_X:   ; +0: Out  +4: In   +8 BYTE *Dsp  
                    ; +12: INT *Off   +16: INT *Sat_Tab
                    ; +20: UINT *TabLo +24: UINT *TabHi

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = (USHORT*)Dst
   mov ebx,[eax+VB_BPS]
   mov [.x_j1+0x08],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]        ; esi = (USHORT*)Src
   mov [.x_j1+0x02],ebx

   mov ecx,[esp+OFF_STCK+12]   ; Off
   mov [.x_i2+0x08],ecx

;   mov ecx,[esp+OFF_STCK+20]   ; TabLo
;   mov [.x_i2+0x08],ecx
;   mov ecx,[esp+OFF_STCK+24]   ; TabHi
;   mov [.x_i2+0x08],ecx

   mov ecx,[esp+OFF_STCK+16]   ; Sat_Tab
   lea ecx,[ecx+ebp*4]         ; +Out->W
   mov [.x_i2+0x0f],ecx

   mov ecx,[esp+OFF_STCK+8]   ; Dsp
   lea ecx,[ecx+ebp]

   mov edx,[eax+VB_H]   ; H
   neg ebp
   xor ebx,ebx
   dec edx
   jl .End
   jmp .x_j2

align4
.x_j1:
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst
   add ecx,THE_W        ; Dsp

.x_j2:
   push ebp            ; saves i=-W

align4
.x_i2:
   mov bl, [ecx+ebp]            ; Dsp[i]
   mov eax,ebp
   add eax, [0x12345678+4*ebx]  ; i'=Off[Dsp[i]]
   mov eax, [0x12345678+4*eax]  ; i'=Sat[i]
   mov ax, [esi+eax*2]     ; Src[i']
   mov [edi+ebp*2],ax

   inc ebp
   jne near .x_i2

   pop ebp
   dec edx
   jge near .x_j1

.End:
   Leave

;////////////////////////////////////////////////////////////////////

Mix_Dsp_Off_Blur_Y:   ; +0: Out  +4: In   +8 BYTE *Dsp  
                    ; +12: INT *Off   +16: INT *Sat_Tab

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = (USHORT*)Dst
   mov ebx,[eax+VB_BPS]       ; Dst_BpS
   mov [.y_j1+0x02],ebx          

   mov ecx,[esp+OFF_STCK+4]   ; In
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]        ; esi = (USHORT*)Src
   mov [.y_i2+0x15],esi

   mov ecx,[esp+OFF_STCK+12]   ; Off
   mov [.y_i2+0x06],ecx

   mov edx,[eax+VB_H]   ; H

   mov esi,[esp+OFF_STCK+16]   ; esi = Sat_Tab
   lea esi,[esi+edx*4-4]         ; +Out->H + 1

   mov ecx,[esp+OFF_STCK+8]   ; Dsp
   lea ecx,[ecx+ebp]

   neg ebp
   xor ebx,ebx
   dec edx
   jl .End
   jmp .y_j2

align4
.y_j1:
   add edi,0x12345678   ; Dst
   add ecx,THE_W        ; Dsp

.y_j2:
   push ebp            ; saves i=-W

align4
.y_i2:
   mov bl, [ecx+ebp]            ; Dsp[i]
   mov eax, [0x12345678+4*ebx]  ; i'=Off[Dsp[i]]
   sub eax,edx
   mov eax, [esi+4*eax]  ; i'=Sat[i]
   add eax,ebp
   mov ax, [0x12345678+eax*2]     ; Src[i']
   mov [edi+ebp*2],ax

   inc ebp
   jne near .y_i2

   pop ebp
   dec edx
   jge near .y_j1

.End:
   Leave

;////////////////////////////////////////////////////////////////////

%endif
