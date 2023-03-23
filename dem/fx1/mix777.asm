;//////////////////////////////////////////////////////////////////////////

[BITS 32]

globl Mix_777_To_16
globl Mix_777_To_16_Remap
globl Mix_777_To_16_Zero

extrn RGB71_To_16
extrn R_Remap777
extrn G_Remap777
extrn B_Remap777

%macro RGB_777_TO_16_BX 0
   mov al,cl
   mov bx, [RGB71_To_16+0*256+eax*2]
   mov al,ch
   shr ecx,16
   or bx, [RGB71_To_16+256*2+eax*2]
   or bx, [RGB71_To_16+256*4+ecx*2]
%endmacro

%macro REMAP_777 0
   mov eax, [B_Remap777+ebx*4]
   mov bl,cl   
   or eax, [R_Remap777+ebx*4]
   mov bl,ch
   or eax, [G_Remap777+ebx*4]
%endmacro

;//////////////////////////////////////////////////////////////////////////

DATA

Mix_777_To_16:   ; +0: Out  +4: In 

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x08],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*4]
   mov [.j1+0x02],ebx

   sar ebp,1   ; W /= 2
   xor ebx,ebx   
   mov edx,[eax+VB_H]   ; H
   neg ebp
   xor eax,eax
   dec edx
   jl .End
   jmp .j2

align4
.j1:
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.j2:
   push ebp            ; saves i=-W

align4
.i2:
   mov ecx, [esi+ebp*8+4]          ; Src[i]
   RGB_777_TO_16_BX
   mov ecx, [esi+ebp*8]        ; Src[i]
   shl ebx,16
   RGB_777_TO_16_BX
   mov [edi+ebp*4],ebx

   inc ebp
   jl near .i2

   pop ebp
   dec edx
   jge near .j1

.End:
   Leave

;////////////////////////////////////////////////////////////////////

%if 0       ; UNUSED NOW

Mix_777_To_16_Zero:   ; +0: Out  +4: In 

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x08],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*4]
   mov [.j1+0x02],ebx

   sar ebp,1   ; W /= 2
   xor ebx,ebx   
   mov edx,[eax+VB_H]   ; H
   neg ebp
   xor eax,eax
   fldz
   dec edx
   jl .End
   jmp .j2

align4
.j1:
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.j2:
   push ebp            ; saves i=-W

align4
.i2:
   mov ecx, [esi+ebp*8+4]          ; Src[i]
   RGB_777_TO_16_BX
   mov ecx, [esi+ebp*8]        ; Src[i]
   shl ebx,16
   RGB_777_TO_16_BX
   mov [edi+ebp*4],ebx
   fst qword [esi+ebp*8]
   inc ebp
   jl near .i2

   pop ebp
   dec edx
   jge near .j1

.End:
   fstp st0
   Leave

%endif

;////////////////////////////////////////////////////////////////////

%if 0       ; UNUSED NOW

Mix_777_To_16_Remap:   ; +0: Out  +4: In 

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x08],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*4]
   mov [.j1+0x02],ebx

   sar ebp,1   ; W /= 2
   xor ebx,ebx   
   mov edx,[eax+VB_H]   ; H
   neg ebp
   xor eax,eax
   dec edx
   jl near .End
   jmp .j2

align4
.j1:
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.j2:
   push ebp            ; saves i=-W

align4
.i2:
   mov ecx, [esi+ebp*8+4]          ; Src[i]
   RGB_777_TO_16_BX
   mov ecx, [esi+ebp*8]        ; Src[i]
   shl ebx,16
   RGB_777_TO_16_BX
   mov [edi+ebp*4],ebx

   inc ebp
   jle near .i2

   mov ebp,[esp]
   xor ecx,ecx
   xor ebx,ebx
.i3:
   mov cx, [esi+ebp*8]
   mov bl, [esi+ebp*8 +2 ]
   REMAP_777
   mov [esi+ebp*8],eax
   mov cx, [esi+ebp*8+4]
   mov bl, [esi+ebp*8+4 +2 ]
   REMAP_777
   mov [esi+ebp*8+4],eax
   inc ebp
   jl near .i3
   xor ebx,ebx
   pop ebp
   xor eax,eax
   dec edx
   jge near .j1

.End:
   Leave

%endif

;////////////////////////////////////////////////////////////////////

