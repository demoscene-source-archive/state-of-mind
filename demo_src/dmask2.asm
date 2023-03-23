;//////////////////////////////////////////////////////////////////////

globl Uncompress_DMask_Lines

DATA

;//////////////////////////////////////////////////////////////////////

Uncompress_DMask_Lines:    ; +0:  USHORT *Dst +4:  PIXEL *Src,
                           ; +8:  INT BpS     +12: INT H
                           ; +16: USHORT *Cvrt
   Enter
   mov edi,[esp+OFF_STCK+0]   ; Dst
   mov esi,[esp+OFF_STCK+4]   ; Src
   mov edx,[esp+OFF_STCK+16]  ; Cvrt
   xor ebx,ebx
   xor eax,eax

Loop_DM:
   mov al,[esi]
   inc esi
   cmp al,0xFF    ; EOL
   je Loop_DM_EOL
   cmp al,0xFD    ; FILL
   jne Loop_DM_1

   ; FILL

Loop_DM_1:
   cmp al,0xFE ; SKIP
   jne Loop_DM_2
   mov al,[esi]
   inc esi
   inc eax
   lea edi,[edi+2*eax]
   xor eax,eax
   jmp Loop_DM

Loop_DM_2:
   cmp al,0xFC
   jne Loop_DM_4
      ; BLOCK

Loop_DM_4:
   cmp al,0xfB
   jne Loop_DM_3
   mov bl,[esi]
   inc esi
   mov bx,[edx+2*ebx]
   mov [edi],bx
   add edi,2
   jmp Loop_DM

Loop_DM_3:
   mov bx,[edx+2*eax]
   mov [edi],bx
   add edi,2
   xor ebx,ebx
   jmp Loop_DM
align 4


Loop_DM_EOL:
   add edi,[esp+OFF_STCK+8]
   dec dword [esp+OFF_STCK+12]   ; H--
   jge Loop_DM

   Leave

;//////////////////////////////////////////////////////////////////////
