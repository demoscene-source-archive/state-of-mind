;//////////////////////////////////////////////////////////////////////////

[BITS 32]

%macro Put 2
   mov ecx, [ebx+%1]
   mov [eax+%2],ecx
%endmacro

%macro Put_Sat 2
   mov ecx, [ebx+%1]
   add ecx,[eax+%2]
   mov [eax+%2],ecx
%endmacro

;//////////////////////////////////////////////////////////////////////

%if 0    ; UNUSED NOW

extrn I_Table

globl Paste_Sprite_Raw_8x8
globl Paste_Sprite_Raw_4x4
globl Paste_Sprite_Sat_8x8
globl Paste_Sprite_Sat_4x4


;//////////////////////////////////////////////////////////////////////////

TEXT

Paste_Sprite_Raw_8x8: ; +0: PIXEL *Dst, +4: PIXEL *Src

   push ebx   
   mov eax,[esp+8]   ; Dst
   mov ebx,[esp+12]   ; Src
   Put 0,0
   Put 4,4
   Put 8,320
   Put 12,324
   Put 16,640
   Put 20,644
   Put 24,960
   Put 28,964
   pop ebx
   ret

Paste_Sprite_Raw_4x4: ; +0: PIXEL *Dst, +4: PIXEL *Src

   push ebx   
   mov eax,[esp+8]   ; Dst
   mov ebx,[esp+12]   ; Src
   Put 0,0
   Put 4,320
   Put 12,640
   Put 16,960
   pop ebx
   ret

Paste_Sprite_Sat_8x8: ; +0: PIXEL *Dst, +4: PIXEL *Src

   push ebx   
   mov eax,[esp+8]   ; Dst
   mov ebx,[esp+12]   ; Src
   Put_Sat 0,0
   Put_Sat 4,4
   Put_Sat 8,320
   Put_Sat 12,324
   Put_Sat 16,640
   Put_Sat 20,644
   Put_Sat 24,960
   Put_Sat 28,964
   pop ebx
   ret

Paste_Sprite_Sat_4x4: ; +0: PIXEL *Dst, +4: PIXEL *Src

   push ebx
   mov eax,[esp+8]   ; Dst
   mov ebx,[esp+12]   ; Src
   Put_Sat 0,0
   Put_Sat 4,320
   Put_Sat 12,640
   Put_Sat 16,960
   pop ebx
   ret

%endif      ; UNUSED

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

globl Do_Blur_Y_Loop

DATA

Do_Blur_Y_Loop:      ; +0:  BYTE *Dst   +4:  INT Dst_BpS,
                     ; +8:  BYTE *Src   
                     ; +12: INT W       +16: INT H    +20: INT Src_BpS
                     ; +24: USHORT *Sat1 +26: INT *Int1
   Enter
   mov edi,[esp+OFF_STCK+0]   ; Dst
   mov eax,[esp+OFF_STCK+4]   ; Dst_BpS
   mov [.End+0x08],eax
   mov esi,[esp+OFF_STCK+8]   ; Src
   mov eax,[esp+OFF_STCK+20]  ; Src_BpS
   mov [.End+0x02],eax

   mov eax,[esp+OFF_STCK+24]  ; Sat1
   mov [.Y_X+0x13],eax
   mov eax,[esp+OFF_STCK+28]  ; Int1
   mov [.Y_X+0x0b],eax

.Y_Y:
   mov ebp,[esp+OFF_STCK+12]   ; W
   xor eax,eax
   neg ebp
.Y_X:
   mov al,[esi+ebp]
   xor ebx,ebx
   mov bl,[edi+ebp]
   add ebx,[0x12345678+4*eax]
   mov bx, [0x12345678+2*ebx]
   xor eax,eax
   mov [edi+ebp],bl
   inc ebp
   jl .Y_X

.End:
   add esi,0x12345678
   add edi,0x12345678
   dec dword [esp+OFF_STCK+16]   ; H--
   jg .Y_Y
   Leave

;//////////////////////////////////////////////////////////////////////////

