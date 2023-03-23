;//////////////////////////////////////////////////////////////////////////

[BITS 32]

globl Mix_Sat16
globl Mix_Sat16_Clear12
globl Mix_Sat16_Clear1
globl Mix_Sat16_Clear2
globl Mix_Sat16_8
globl Mix_Sat16_8_Clear12
globl Mix_Sat16_8_Clear1
globl Mix_Sat16_8_Clear2
globl Mix_Sat16_8_M
globl Mix_Sat8_8
globl Mix_Sat8_8_Clear12
globl Mix_Sat8_8_Clear1
globl Mix_Sat8_8_Clear2
globl Mix_Sat8_8_Zero
globl Mix_Sat8_8_Blend
globl Mix_16_16_Blend
globl Mix_8_8_Blend_Mask
globl Mix_8_8_Blend_Mask_II
globl Mix_16_8_Blend_Mask
globl Mix_16_8_Blend_Mask_II
globl Mix_16_To_16
globl Mix_To_Fade
globl Mix_To_Fade_Feedback
globl Mix_8_Mask
globl Mix_16_Mask
globl Mix_16_16_Mix
globl Mix_16_16_Mix_Copy
globl Mix_Sat16_16_Feedback

globl Do_Plasma_256
globl Paste_256
globl Paste_256_Y

extrn Lo16_To_777
extrn Hi16_To_777
extrn RGB71_To_16
extrn Lo16_To_777_M
extrn Hi16_To_777_M
extrn RGB71_To_16_M
extrn RGB71_To_16_Remap

; extrn Big_16_To_777
; extrn Big_777_To_16_GB

;//////////////////////////////////////////////////////////////////////////

%macro RGB_777_TO_16 3      ; %1=b %2=c %3=a   _BX:a,c,b
   mov %{1}l,%{2}l
   mov %{3}x, [RGB71_To_16+256*0+e%{1}x*2]
   mov %{1}l,%{2}h
   or  %{3}x, [RGB71_To_16+256*2+e%{1}x*2]
   shr e%{2}x,16
   or  %{3}x, [RGB71_To_16+256*4+e%{2}x*2]
%endmacro

%macro RGB_777_TO_16_M 3      ; %1=b %2=c %3=a   _BX:a,c,b
   mov %{1}l,%{2}l
   mov %{3}x, [RGB71_To_16_M+256*0+e%{1}x*2]
   mov %{1}l,%{2}h
   or  %{3}x, [RGB71_To_16_M+256*2+e%{1}x*2]
   shr e%{2}x,16
   or  %{3}x, [RGB71_To_16_M+256*4+e%{2}x*2]
%endmacro

%macro RGB_777_TO_16_REMAP 3      ; %1=b %2=c %3=a   _BX:a,c,b
   mov %{1}l,%{2}l
   mov %{3}x, [RGB71_To_16_Remap+256*0+e%{1}x*2]
   mov %{1}l,%{2}h
   or  %{3}x, [RGB71_To_16_Remap+256*2+e%{1}x*2]
   shr e%{2}x,16
   or  %{3}x, [RGB71_To_16_Remap+256*4+e%{2}x*2]
%endmacro

;//////////////////////////////////////////////////////////////////////////

%macro RGB16_TO_777 3      ; %1=b  %2=a %3=c
   mov %{1}l,%{2}l
   mov e%{3}x,[Lo16_To_777+e%{1}x*4]      ; Lo16_To_777[In&0xFF]
   mov %{1}l,%{2}h
   add e%{3}x,[Hi16_To_777+e%{1}x*4]      ; Hi16_To_777[In>>8]
%endmacro

%macro RGB16_TO_777_Add 3      ; %1=b  %2=a %3=c
   mov %{1}l,%{2}l
   add e%{3}x,[Lo16_To_777+e%{1}x*4]      ; Lo16_To_777[In&0xFF]
   mov %{1}l,%{2}h
   add e%{3}x,[Hi16_To_777+e%{1}x*4]      ; Hi16_To_777[In>>8]
%endmacro

%macro RGB16_TO_777_M  3      ; %1=b  %2=a %3=c
   mov %{1}l,%{2}l
   add e%{3}x,[Lo16_To_777_M+e%{1}x*4]      ; Lo16_To_777[In&0xFF]
   mov %{1}l,%{2}h
   add e%{3}x,[Hi16_To_777_M+e%{1}x*4]      ; Hi16_To_777[In>>8]
%endmacro
%macro RGB16_TO_777_M_Add 3      ; %1=b  %2=a %3=c
   mov %{1}l,%{2}l
   add e%{3}x,[Lo16_To_777_M+e%{1}x*4]      ; Lo16_To_777[In&0xFF]
   mov %{1}l,%{2}h
   add e%{3}x,[Hi16_To_777_M+e%{1}x*4]      ; Hi16_To_777[In>>8]
%endmacro
DATA

Mix_Sat16:     ; +0: Out    +4: In1  +8: In2

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp*2]
   mov [.j1+0x02],ebx

   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   sar ebp,1   ; W /= 2
   inc ecx
   jle .Ok_J1
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok_J1:
   push ecx
   xor ebx,ebx
   push ebp            ; saves i=-W

align4
.i1:

   mov ax, word [esi+ebp*4+2]   ; Src1[i]
   RGB16_TO_777 b,a,c
   mov ax, word [edx+ebp*4+2]   ; Src2[i]
   RGB16_TO_777_Add b,a,c

   RGB_777_TO_16 b,c,a
   shl eax,16

   mov ax, word [esi+ebp*4]   ; Src1[i]
   RGB16_TO_777 b,a,c
   mov ax, word [edx+ebp*4]   ; Src2[i]
   RGB16_TO_777_Add b,a,c

   RGB_777_TO_16 b,c,a

   mov [edi+ebp*4],eax
   inc ebp
   jne near .i1

   pop ebp
   pop ecx
   inc ecx
   jle near .j1
   Leave

Mix_Sat16_Clear12:     ; +0: Out    +4: In1  +8: In2

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp*2]
   mov [.j1+0x02],ebx

   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   sar ebp,1   ; W /= 2
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   xor ebx,ebx
   push ebp            ; saves i=-W

align4
.i1:

   mov ax, word [esi+ebp*4+2]   ; Src1[i]
   RGB16_TO_777 b,a,c
   mov ax, word [edx+ebp*4+2]   ; Src2[i]
   RGB16_TO_777_Add b,a,c

   RGB_777_TO_16 b,c,a
   shl eax,16

   mov ax, word [esi+ebp*4]   ; Src1[i]
   RGB16_TO_777 b,a,c
   mov ax, word [edx+ebp*4]   ; Src2[i]
   RGB16_TO_777_Add b,a,c

   RGB_777_TO_16 b,c,a
   xor ebx,ebx
   mov [edi+ebp*4],eax
   mov [esi+ebp*4],ebx
   mov [edx+ebp*4],ebx
   inc ebp
   jne near .i1

   pop ebp
   pop ecx
   inc ecx
   jle near .j1
   Leave

Mix_Sat16_Clear1:     ; +0: Out    +4: In1  +8: In2

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp*2]
   mov [.j1+0x02],ebx

   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   sar ebp,1   ; W /= 2
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   xor ebx,ebx
   push ebp            ; saves i=-W

align4
.i1:

   mov ax, word [esi+ebp*4+2]   ; Src1[i]
   RGB16_TO_777 b,a,c
   mov bl,al
   mov ecx,[Lo16_To_777+ebx*4]      ; Lo16_To_777[In&0xFF]
   mov bl,ah
   add ecx,[Hi16_To_777+ebx*4]      ; Hi16_To_777[In>>8]

   mov ax, word [edx+ebp*4+2]   ; Src2[i]
   RGB16_TO_777_Add b,a,c

   RGB_777_TO_16 b,c,a
   shl eax,16

   mov ax, word [esi+ebp*4]   ; Src1[i]
   RGB16_TO_777 b,a,c
   mov ax, word [edx+ebp*4]   ; Src2[i]
   RGB16_TO_777_Add b,a,c

   RGB_777_TO_16 b,c,a
   xor ebx,ebx
   mov [edi+ebp*4],eax
   mov [esi+ebp*4],ebx
   inc ebp
   jne near .i1

   pop ebp
   pop ecx
   inc ecx
   jle near .j1
   Leave

Mix_Sat16_Clear2:     ; +0: Out    +4: In1  +8: In2

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp*2]
   mov [.j1+0x02],ebx

   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   sar ebp,1   ; W /= 2
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src2
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   xor ebx,ebx
   push ebp            ; saves i=-W

align4
.i1:

   mov ax, word [esi+ebp*4+2]   ; Src1[i]
   RGB16_TO_777 b,a,c

   mov ax, word [edx+ebp*4+2]   ; Src2[i]
   RGB16_TO_777_Add b,a,c

   RGB_777_TO_16 b,c,a
   shl eax,16

   mov ax, word [esi+ebp*4]   ; Src1[i]
   RGB16_TO_777 b,a,c

   mov ax, word [edx+ebp*4]   ; Src2[i]
   RGB16_TO_777_Add b,a,c

   RGB_777_TO_16 b,c,a
   xor ebx,ebx
   mov [edi+ebp*4],eax
   mov [edx+ebp*4],ebx
   inc ebp
   jne near .i1

   pop ebp
   pop ecx
   inc ecx
   jle near .j1
   Leave

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

Mix_Sat16_8:     ; +0: Out    +4: In1  +8: In2

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp]
   mov [.j1+0x02],ebx

   mov ecx,[ecx+VB_CMAP]   ; CMap
   mov [.Mod1+0x03],ecx

   mov ecx,[eax+VB_H]   ; H
   neg ebp
   xor eax,eax
   neg ecx
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   xor ebx,ebx
   push ebp            ; saves i=-W

align4
.i2:

   mov ax, word [esi+ebp*2]       ; Src1[i]
   RGB16_TO_777 b,a,c

   mov bl, [edx+ebp]            ; Src2[i]
.Mod1:
   add ecx,[0x1234567+ebx*4]        ; += CMap[]

   RGB_777_TO_16 b,c,a
   mov [edi+ebp*2],ax

   inc ebp
   jne near .i2

   pop ebp
   pop ecx
   inc ecx
   jle near .j1
   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_Sat16_8_Clear12:
   ret

Mix_Sat16_8_Clear1:
   ret

Mix_Sat16_8_Clear2:
   ret

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

Mix_Sat16_8_M:     ; +0: Out    +4: In1  +8: In2

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp]
   mov [.j1+0x02],ebx

   mov ecx,[ecx+VB_CMAP]   ; CMap
   mov [.Mod1+0x03],ecx
   mov [.Mod2+0x03],ecx

   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   sar ebp,1   ; W /= 2
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   xor ebx,ebx
   push ebp            ; saves i=-W

align4
.i2:

   mov ax, word [esi+ebp*4+2]       ; Src1[i]
   RGB16_TO_777_M b,a,c

   mov bl, [edx+ebp*2+1]            ; Src2[i]
.Mod1:
   sub ecx,[0x1234567+ebx*4]        ; += CMap[]

   RGB_777_TO_16_M b,c,a
   shl eax,16

   mov ax, word [esi+ebp*4]         ; Src1[i]
   RGB16_TO_777_M b,a,c

   mov bl, [edx+ebp*2]              ; Src2[i]
.Mod2:
   sub ecx,[0x1234567+ebx*4]        ; += CMap[]

   RGB_777_TO_16_M b,c,a

   mov [edi+ebp*4],eax
   inc ebp
   jne near .i2

   pop ebp
   pop ecx
   inc ecx
   jle near .j1
   Leave

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

Mix_Sat8_8:

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp]
   mov [.j1+0x08],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap1
   mov [.Mod11+0x03],ecx
   mov [.Mod21+0x03],ecx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp]
   mov [.j1+0x02],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap2
   mov [.Mod12+0x03],ecx
   mov [.Mod22+0x03],ecx

   xor ebx,ebx
   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   sar ebp,1   ; W /= 2
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   push ebp            ; saves i=-W

align4
.i2:

   mov bl, [esi+ebp*2+1]            ; Src1[i]
.Mod11:
   mov ecx,[0x1234567+ebx*4]        ; CMap1[]

   mov bl, [edx+ebp*2+1]            ; Src2[i]
.Mod12:
   add ecx,[0x1234567+ebx*4]        ; CMap2[]

   RGB_777_TO_16 b,c,a
   shl eax,16

   mov bl, [esi+ebp*2+0]            ; Src1[i]
.Mod21:
   mov ecx,[0x1234567+ebx*4]        ; CMap1[]

   mov bl, [edx+ebp*2+0]            ; Src2[i]
.Mod22:
   add ecx,[0x1234567+ebx*4]        ; CMap2[]

   RGB_777_TO_16 b,c,a

   mov [edi+ebp*4],eax

   inc ebp
   jne near .i2

   pop ebp
   pop ecx
   inc ecx
   jle near .j1
   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_Sat8_8_Clear12:
   ret
Mix_Sat8_8_Clear1:
   ret
Mix_Sat8_8_Clear2:
   ret

;//////////////////////////////////////////////////////////////////////////

Mix_Sat8_8_Zero:

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp]
   mov [.j1+0x08],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap1
   mov [.Mod11+0x03],ecx
   mov [.Mod21+0x03],ecx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp]
   mov [.j1+0x02],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap2
   mov [.Mod12+0x03],ecx
   mov [.Mod22+0x03],ecx

   xor ebx,ebx
   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   sar ebp,1   ; W /= 2
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   push ebp            ; saves i=-W

align4
.i2:
   xor eax,eax
   mov bl, [edx+ebp*2+1]            ; Src2[i]
   test bl,bl
   jz .Skip11
.Mod12:
   mov ecx,[0x1234567+ebx*4]        ; CMap2[]

   mov bl, [esi+ebp*2+1]            ; Src1[i]
.Mod11:
   add ecx,[0x1234567+ebx*4]        ; CMap1[]
   ;and ecx,[0x1234567+ebx*4]       ; CMap1[]

   RGB_777_TO_16 b,c,a
   shl eax,16

.Skip11:
   mov bl, [edx+ebp*2+0]            ; Src2[i]
   test bl,bl
   jz .Skip12
.Mod22:
   mov ecx,[0x1234567+ebx*4]        ; CMap2[]

   mov bl, [esi+ebp*2+0]            ; Src1[i]
.Mod21:
   add ecx,[0x1234567+ebx*4]        ; CMap1[]
   ;and ecx,[0x1234567+ebx*4]       ; CMap1[]
   mov [esi+ebp*2],ax
   RGB_777_TO_16 b,c,a

.Skip12:
   mov [edi+ebp*4],eax
   inc ebp
   jne near .i2

   pop ebp
   pop ecx
   inc ecx
   jle near .j1
   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_Sat8_8_Blend: ; +0: Out +4:In1 +8:In2 +12:Blend

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp]
   mov [.j1+0x08],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap1
   mov [.Mod11+0x04],ecx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp]
   mov [.j1+0x02],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap2
   mov [.Mod12+0x04],ecx

   mov ecx,[esp+OFF_STCK+12]   ; Blend
   mov ebx,[ecx+VB_BITS]
   lea ebx,[ebx+ebp]
   mov [.i2+0x02],ebx
   mov ebx,[ecx+VB_BPS]       ; Blend BpS
   mov [.j1+0x18],ebx

   xor ebx,ebx
   mov ecx,[eax+VB_H]   ; H
   neg ebp
   xor eax,eax
   neg ecx
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst
   add dword [.i2+0x02],0x12345678   ; Blend

.Ok:
   push ecx
   push ebp            ; saves i=-W

align4
.i2:

   mov ah, [0x1234567+ebp]          ; blend[i]
   mov bl, [esi+ebp]                ; Src1[i]
   test ah,ah
.Mod11:
   mov bx,[0x1234567+ebx*2]         ; CMap1[]
   jz .Z
   RGB16_TO_777 a,b,c
   xor ebx,ebx
   xor ah,0x0f
   mov bl, [edx+ebp]                ; Src2[i]
.Mod12:
   mov bx,[0x1234567+ebx*2]         ; CMap2[]
   jz .Z
   RGB16_TO_777_Add a,b,c
   xor eax,eax
   RGB_777_TO_16 a,c,b
.Z:
   mov [edi+ebp*2],bx
   xor ebx,ebx
   inc ebp
   jne near .i2

   pop ebp
   pop ecx
   inc ecx
   jle near .j1
   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_8_8_Blend_Mask:

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap1
   mov [.Mod1+0x04],ecx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp]
   mov [.j1+0x02],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap2
   mov [.Mod2+0x04],ecx

   xor ebx,ebx
   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   xor eax,eax
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   push ebp            ; saves i=-W

align4
.i2:
   mov ax, [esi+ebp*2]                ; Src1[i]
   mov bl,al
   test ah,ah
.Mod1:
   mov bx,[0x1234567+ebx*2]         ; CMap1[]
   jz .Z
   RGB16_TO_777 a,b,c
   xor ebx,ebx
   xor ah,0x0f
   mov bl, [edx+ebp]                ; Src2[i]
.Mod2:
   mov bx,[0x1234567+ebx*2]         ; CMap2[]
   jz .Z
   RGB16_TO_777_Add a,b,c
   xor eax,eax
   RGB_777_TO_16 a,c,b
.Z:
   mov [edi+ebp*2],bx
   xor ebx,ebx
   inc ebp
   jne near .i2

   pop ebp
   pop ecx
   inc ecx
   jle near .j1

   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_8_8_Blend_Mask_II:

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp]
   mov [.j1+0x08],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap1
   mov [.Mod1+0x03],ecx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp]
   mov [.j1+0x02],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap2
   mov [.Mod2+0x04],ecx

   xor ebx,ebx
   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   xor eax,eax
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   push ebp            ; saves i=-W

align4
.i2:
   mov bl, [esi+ebp]                ; Src1[i]
.Mod1:
   mov ecx,[0x1234567+ebx*4]        ; CMap1[]
   mov ah,cl
   shr ecx,8
   mov bl, [edx+ebp]                ; Src2[i]
.Mod2:
   mov bx,[0x1234567+ebx*2]         ; CMap2[]
   RGB16_TO_777_Add a,b,c
   xor eax,eax
   RGB_777_TO_16 a,c,b
   mov [edi+ebp*2],bx
   xor ebx,ebx
   inc ebp
   jne near .i2

   pop ebp
   pop ecx
   inc ecx
   jle near .j1

   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_16_8_Blend_Mask:

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap1
   mov [.Mod1+0x04],ecx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp*2]
   mov [.j1+0x02],ebx

   xor ebx,ebx
   
   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   xor eax,eax
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   push ebp            ; saves i=-W

align4
.i2:
   mov ax, [esi+ebp*2]                ; Src1[i]
   mov bl,al
.Mod1:
   mov bx,[0x1234567+ebx*2]         ; CMap1[]
   RGB16_TO_777 a,b,c
   xor ebx,ebx
   xor ah,0x0f
   mov bx, [edx+ebp*2]                ; Src2[i]
   RGB16_TO_777_Add a,b,c
   xor ebx,ebx
   RGB_777_TO_16 b,c,a

   mov [edi+ebp*2],ax
   inc ebp
   jne near .i2

   pop ebp
   pop ecx
   inc ecx
   jle near .j1

   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_16_8_Blend_Mask_II:

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp]
   mov [.j1+0x08],ebx
   mov ecx,[ecx+VB_CMAP]      ; CMap1
   mov [.Mod1+0x03],ecx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp*2]
   mov [.j1+0x02],ebx

   xor ebx,ebx
   
   mov ecx,[eax+VB_H]   ; H
   neg ebp
   neg ecx
   xor eax,eax
   inc ecx
   jle .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   push ebp            ; saves i=-W

align4
.i2:
   mov bl, [esi+ebp]                ; Src1[i]
.Mod1:
   mov ecx,[0x1234567+ebx*4]        ; CMap1[]
   mov ah,cl
   mov bx, [edx+ebp*2]              ; Src2[i]
   shr ecx,8
   RGB16_TO_777_Add a,b,c
   xor eax,eax
   RGB_777_TO_16 a,c,b
   mov [edi+ebp*2],bx
   xor ebx,ebx
   inc ebp
   jne near .i2

   pop ebp
   pop ecx
   inc ecx
   jle near .j1

   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_16_To_16:     ; +0: Out  +4: TabLo  +8:TabHi

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.Ok+0x02],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; Lo
   mov edx,[esp+OFF_STCK+8]   ; Hi
   mov [.i2+0x0a],ecx   
   mov [.i2+0x14],edx

   mov edx,[eax+VB_H]   ; H

   xor ebx,ebx
   neg ebp
   dec edx
   jl End_1616

.Ok:
   add esi, 0x12345678     ; Src_BpS
   push ebp                ; saves i=-W

align4
.i2:
   mov ax, [esi+ebp*2]              ; Src1[i]
   mov bl,al
   mov cx,[0x12345678+ebx*2]        ; lo
   mov bl,ah
   add cx,[0x12345678+ebx*2]        ; hi
   mov [esi+ebp*2],cx

   inc ebp
   jne near .i2

   pop ebp
   dec edx
   jg near .Ok

End_1616:
   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_16_16_Blend:  ;+0: Out  +4:In1    +8:In2     +12:Blend
                  ;+16: Xor +20:TabLo +24:TabHi

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp*2]
   mov [.j1+0x02],ebx

   mov ecx,[esp+OFF_STCK+12]   ; Blend
   mov ebx,[ecx+VB_BITS]
   lea ebx,[ebx+ebp]
   mov [.i2+0x02],ebx
   mov ebx,[ecx+VB_BPS]
   mov [.j1+0x18],ebx

   mov ecx,[esp+OFF_STCK+16]   ; Xor
   mov [.Mod+0x02],cl

   mov ecx,[esp+OFF_STCK+20]   ; TabLo
   mov [.i2+0x13],ecx
   mov [.Mod+0x0e],ecx

   mov ecx,[esp+OFF_STCK+24]   ; TabHi
   mov [.i2+0x1c],ecx
   mov [.Mod+0x17],ecx

   xor ebx,ebx
   mov ecx,[eax+VB_H]   ; H
   neg ebp
   xor eax,eax
   dec ecx
   jge .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst
   add dword [.i2+0x02],0x12345678

.Ok:
   push ecx
   push ebp            ; saves i=-W

align4
.i2:
   mov ah, [0x12345657+ebp]        ; blend[i]
   mov bx, [esi+ebp*2]             ; Src1[i]
   test ah,ah
   jz .End
   mov al,bl
   mov ecx,[0x12345678+eax*4]      ; TabLo[b][In&0xFF]
   mov al,bh
   add ecx,[0x12345678+eax*4]      ; TabHi[b][In>>8]
.Mod:
   xor ah,0x0f
   mov bx, [edx+ebp*2]             ; Src2[i]
   jz .End
   mov al,bl
   add ecx,[0x12345678+eax*4]      ; TabLo[b][In&0xFF]
   mov al,bh
   add ecx,[0x12345678+eax*4]      ; TabHi[b][In>>8]
   xor eax,eax
   RGB_777_TO_16 a,c,b
.End:
   mov [edi+ebp*2],bx
   inc ebp
   jne .i2

   pop ebp
   pop ecx
   dec ecx
   jge near .j1

   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_To_Fade:   ; +0: Out  +4: In   +8:USHORT *TabLo +12:TabHi

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x08],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x02],ebx

   mov ecx,[esp+OFF_STCK+8]   ; TabLo
   mov [.i2+0x0a],ecx

   mov ecx,[esp+OFF_STCK+12]   ; TabHi
   mov [.i2+0x14],ecx

   xor ebx,ebx
   mov edx,[eax+VB_H]   ; H
   neg ebp
   dec edx
   jl .End
   jmp .Ok

align4
.j1:

   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ebp            ; saves i=-W

align4
.i2:
   mov ax, [esi+ebp*2]              ; Src1[i]
   mov bl,al
   mov cx,[0x12345678+ebx*2]        ; lo
   mov bl,ah
   add cx,[0x12345678+ebx*2]        ; hi
   mov [edi+ebp*2],cx

   inc ebp
   jne near .i2

   pop ebp
   dec edx
   jge near .j1

.End:
   Leave

Mix_To_Fade_Feedback:   ; +0: Out  +4: In   +8:USHORT *TabLo +12:TabHi

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x08],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x02],ebx

   mov ecx,[esp+OFF_STCK+8]   ; TabLo
   mov [.i2+0x0a],ecx

   mov ecx,[esp+OFF_STCK+12]   ; TabHi
   mov [.i2+0x14],ecx

   xor ebx,ebx
   mov edx,[eax+VB_H]   ; H
   neg ebp
   dec edx
   jl .End
   jmp .Ok

align4
.j1:
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ebp            ; saves i=-W

align4
.i2:
   mov ax, [esi+ebp*2]              ; Src1[i]
   mov bl,al
   mov cx,[0x12345678+ebx*2]        ; lo
   mov bl,ah
   add cx,[0x12345678+ebx*2]        ; hi
   mov [esi+ebp*2],cx
   mov [edi+ebp*2],cx

   inc ebp
   jne near .i2

   pop ebp
   dec edx
   jge near .j1

.End:
   Leave

;////////////////////////////////////////////////////////////////////

Mix_16_16_Mix:     ; +0: Out    +4: In1  +8: In2 
                   ; +12: UINT *TabLo1, +16: UINT *TabHi1
                   ; +20: UINT *TabLo2, +24: UINT *TabHi2

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp*2]
   mov [.j1+0x02],ebx

   mov ecx,[esp+OFF_STCK+12]   ; Lo1
   mov [.Mod11+0x03],ecx
   mov ecx,[esp+OFF_STCK+16]   ; Hi1
   mov [.Mod11+0x0c],ecx
   mov ecx,[esp+OFF_STCK+20]   ; Lo2
   mov [.Mod12+0x03],ecx
   mov ecx,[esp+OFF_STCK+24]   ; Hi2
   mov [.Mod12+0x0c],ecx

   mov ecx,[eax+VB_H]   ; H
   neg ebp
   xor eax,eax
   dec ecx
   jge .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   xor ebx,ebx
   push ebp

align4
.i1:

   mov ax, word [esi+ebp*2]   ; Src1[i]
   mov bl,al
.Mod11:
   mov ecx,[0x12345678+ebx*4]      ; Lo1[In&0xFF]
   mov bl,ah
   add ecx,[0x12345678+ebx*4]      ; Hi1[In>>8]

   mov ax, word [edx+ebp*2]   ; Src2[i]
   mov bl,al
.Mod12:
   add ecx,[Lo16_To_777+ebx*4]      ; Lo2[In2&0xFF]
   mov bl,ah
   add ecx,[Hi16_To_777+ebx*4]      ; Lo2[In2>>8]

   RGB_777_TO_16 b,c,a

   mov [edi+ebp*2],ax
   inc ebp
   jne near .i1

   pop ebp
   pop ecx
   dec ecx
   jge near .j1
   Leave

;////////////////////////////////////////////////////////////////////

Mix_16_16_Mix_Copy ; +0: Out    +4: In1  +8: In2 
                   ; +12: UINT *TabLo1, +16: UINT *TabHi1
                   ; +20: UINT *TabLo2, +24: UINT *TabHi2

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.j1+0x0e],ebx          ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea esi,[esi+ebp*2]
   mov [.j1+0x08],ebx

   mov ecx,[esp+OFF_STCK+8]   ; In2
   mov edx,[ecx+VB_BITS]
   mov ebx,[ecx+VB_BPS]
   lea edx,[edx+ebp*2]
   mov [.j1+0x02],ebx

   mov ecx,[esp+OFF_STCK+12]   ; Lo1
   mov [.Mod11+0x03],ecx
   mov ecx,[esp+OFF_STCK+16]   ; Hi1
   mov [.Mod11+0x0c],ecx
   mov ecx,[esp+OFF_STCK+20]   ; Lo2
   mov [.Mod12+0x03],ecx
   mov ecx,[esp+OFF_STCK+24]   ; Hi2
   mov [.Mod12+0x0c],ecx

   mov ecx,[eax+VB_H]   ; H
   neg ebp
   xor eax,eax
   dec ecx
   jge .Ok
   Leave

align4
.j1:

   add edx,0x12345678   ; Src2
   add esi,0x12345678   ; Src1
   add edi,0x12345678   ; Dst

.Ok:
   push ecx
   xor ebx,ebx
   push ebp            ; saves i=-W

align4
.i1:

   mov ax, word [esi+ebp*2]   ; Src1[i]
   mov bl,al
.Mod11:
   mov ecx,[0x12345678+ebx*4]      ; Lo1[In&0xFF]
   mov bl,ah
   add ecx,[0x12345678+ebx*4]      ; Hi1[In>>8]

   mov ax, word [edx+ebp*2]   ; Src2[i]
   mov bl,al
.Mod12:
   add ecx,[Lo16_To_777+ebx*4]      ; Lo2[In2&0xFF]
   mov bl,ah
   add ecx,[Hi16_To_777+ebx*4]      ; Lo2[In2>>8]

   RGB_777_TO_16 b,c,a

   mov [edi+ebp*2],ax
   mov [esi+ebp*2],ax
   inc ebp
   jne near .i1

   pop ebp
   pop ecx
   dec ecx
   jge near .j1
   Leave

;////////////////////////////////////////////////////////////////////

%ifdef DSADASAJKL

Mix_8_Mask:     ;  +0: USHORT *Dst    +4: INT Dst_BpS,
                ;  +8: PIXEL *Src1
                ; +12: INT W         +16: INT H
                ; +20: INT Src_BpS
                ; +24: UINT *CMap1 

   Enter
   mov edi,[esp+OFF_STCK+0]    ; Dst
   mov esi,[esp+OFF_STCK+8]    ; Src

   mov eax,[esp+OFF_STCK+20]   ; Src_BpS
   mov ebp,[esp+OFF_STCK+4]    ; Dst_BpS
   mov [.Ok+2],eax
   mov [.Ok+8],ebp

   mov ebp,[esp+OFF_STCK+12]   ; W
   mov ebx,[esp+OFF_STCK+16]   ; H
   neg ebp
   neg ebx

   mov eax,[esp+OFF_STCK+24]   ; CMap
   mov [Buh2+4],eax

.j2:
   inc ebx
   jg .End

align4
.Ok:
   add esi,0x12345678   ; Src_BpS
   add edi,0x12345678   ; Dst_BpS

   push ebp            ; saves i=-W

align4
.i2:

   mov al, [esi+ebp]            ; Src[i]
   test al,al
   jz .Skip
Buh2:
   mov cx,[0x1234567+eax*2]      ; CMap[]
   mov [edi+ebp*2],cx
.Skip:
   inc ebp
   jne near .i2

   pop ebp
   inc ebx
   jl near .Ok
.End:
   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_16_Mask:    ;  +0: USHORT *Dst    +4: INT Dst_BpS,
                ;  +8: USHORT *Src1
                ; +12: INT W         +16: INT H
                ; +20: INT Src_BpS

   Enter
   mov edi,[esp+OFF_STCK+0]    ; Dst
   mov esi,[esp+OFF_STCK+8]    ; Src

   mov eax,[esp+OFF_STCK+20]   ; Src_BpS
   mov ebp,[esp+OFF_STCK+4]    ; Dst_BpS
   mov [.Ok+2],eax
   mov [.Ok+8],ebp

   mov ebp,[esp+OFF_STCK+12]   ; W
   mov ebx,[esp+OFF_STCK+16]   ; H
   neg ebp
   neg ebx

.j2:
   inc ebx
   jge .End

align4
.Ok:
   add esi,0x12345678   ; Src_BpS
   add edi,0x12345678   ; Dst_BpS

   push ebp             ; saves i=-W

align4
.i2:

   mov ax, [esi+ebp*2]  ; Src[i]
   test ax,ax
   jz .Skip
   mov [edi+ebp*2],ax
.Skip:
   inc ebp
   jne near .i2

   pop ebp
   inc ebx
   jl near .Ok
.End:
   Leave

;//////////////////////////////////////////////////////////////////////////

Mix_Sat16_16_Feedback:
               ; +0: USHORT *Dst    +4: INT Dst_BpS,
               ; +8: USHORT *Src1  +12: INT Src_BpS
               ;+16: INT W         +20: INT H
   
   Enter

   mov edi,[esp+OFF_STCK+0]    ; Dst
   mov esi,[esp+OFF_STCK+8]    ; Src

   mov ebp,[esp+OFF_STCK+16]   ; W
   mov ecx,[esp+OFF_STCK+20]   ; H
   ;lea edi,[edi+2*ebp]
   ;lea esi,[edi+2*ebp]
   neg ebp
   neg ecx

   inc ecx
   jge .End

.Ok:
   push ecx
   add esi,[esp+OFF_STCK+12+4]   ; Src_BpS
   add edi,[esp+OFF_STCK+4 +4]   ; Dst_BpS

   xor ebx,ebx
   push ebp            ; saves i=-W

align4
.i1:

   mov ax, word [esi+ebp*2]      ; Src[i]
   RGB16_TO_777 b,a,c

   mov ax, word [edi+ebp*2]      ; Dst[i]
   RGB16_TO_777_Add b,a,c

   RGB_777_TO_16_REMAP b,c,a

   mov [edi+ebp*2],ax
   shr eax,16
   mov [esi+ebp*2],ax
   inc ebp
   jne near .i1

   pop ebp
   pop ecx
   inc ecx
   jle near .Ok
.End
   Leave

%endif

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

Do_Plasma_256:     ; +0:  PIXEL *Dst +4: PIXEL *In
                   ; +8:  PIXEL U1  +12: PIXEL V1
                   ; +16: PIXEL U2  +20: PIXEL V2
   Enter

   xor ecx,ecx
   xor edx,edx
   mov edi,[esp+OFF_STCK+0]   ; Dst
   mov esi,[esp+OFF_STCK+4]   ; In
   mov cl,[esp+OFF_STCK+8]
   mov ch,[esp+OFF_STCK+12]   ; UV1
   mov dl,[esp+OFF_STCK+16]
   mov dh,[esp+OFF_STCK+20]   ; UV2

   mov ebp,256    ; j
.Y:
   mov ebx,-128    ; i
   add edi,256
.X:
   mov ax,[esi+ecx]
   add ax,[esi+edx]
   add cl,2   
   mov [edi+ebx*2],ax
   add dl,2
   inc ebx
   jl .X

   inc ch
   inc dh
   dec ebp
   jg .Y
   Leave

;/////////////////////////////////////////////////////////////////

Paste_256:     ; +0:  VBUFFER *Out   +4:  PIXEL *Src
               ; +8:  INT U1         +12: USHORT *CMap

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.Y+0x02],ebx          ; Dst_BpS
   mov ecx,[eax+VB_H]   ; H

   mov esi,[esp+OFF_STCK+4]   ; Src

   mov eax,[esp+OFF_STCK+12]   ; CMap
   mov [.X+0x06],eax

   neg ebp              ; -W
   xor ebx,ebx
   neg ecx
   xor eax,eax
   inc ecx
   jle .Ok
   Leave

align4
.Y:
   add edi,0x12345678   ; Dst
   add esi,256   ; Src1

.Ok:
   mov edx,[esp+OFF_STCK+8]   ; U1
   push ecx
   push ebp            ; saves i=-W

align4
.X:
   mov bl,[esi+edx]
   mov ecx,[0x12345678+ebx*4]
   inc dl
   mov ax,[edi+ebp*2]
   RGB16_TO_777_Add b,a,c
   RGB_777_TO_16 b,c,a
   mov [edi+ebp*2],ax
   xor ebx,ebx
   inc ebp
   jne near .X

   pop ebp
   pop ecx
   inc ecx
   jle near .Y
   Leave

;/////////////////////////////////////////////////////////////////

Paste_256_Y:   ; +0:  VBUFFER *Out   +4:  PIXEL *Src
               ; +8:  INT V1         +12: USHORT *CMap
               ; +16: INT Src_Width

   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [.Y+0x02],ebx          ; Dst_BpS
   mov ecx,[eax+VB_H]   ; H

   mov edx,[esp+OFF_STCK+8]   ; V1

   mov eax,[esp+OFF_STCK+12]   ; CMap
   mov [.X+0x06],eax

   neg ebp              ; -W
   xor ebx,ebx
   neg ecx
   xor eax,eax
   inc ecx
   jle .Ok
   Leave

align4
.Y:
   add edi,0x12345678   ; Dst
   inc dl               ; V1++

.Ok:
   push edx
   imul edx,[esp+OFF_STCK+16+4]
   add edx,[esp+OFF_STCK+4+4]   ; Src
   sub edx,ebp
   mov esi,[esp+OFF_STCK+8+4]   ; U1
   push ecx
   push ebp            ; saves i=-W

align4
.X:
   mov bl,[edx+ebp]
   mov ecx,[0x12345678+ebx*4]
   mov ax,[edi+ebp*2]
   RGB16_TO_777_Add b,a,c
   RGB_777_TO_16 b,c,a
   mov [edi+ebp*2],ax
   xor ebx,ebx
   inc ebp
   jne near .X

   pop ebp
   pop ecx
   pop edx
   inc ecx
   jle near .Y
   Leave

;/////////////////////////////////////////////////////////////////

