;//////////////////////////////////////////////////////////////////////

globl Map_Scale_Up_8_Bits

DATA

;//////////////////////////////////////////////////////////////////////

Map_Scale_Up_8_Bits:    ; +0: VBUFFER *Out  +4: VBUFFER *In
                           ; +16: USHORT *Cvrt
   Enter

   mov eax,[esp+OFF_STCK+0]   ; Out
   mov edi,[eax+VB_BITS]
   mov ebp,[eax+VB_W]         ; ebp = Out->W
   lea edi,[edi+ebp*2]        ; edi = Dst
   mov ebx,[eax+VB_BPS]
   mov [LoopU8_Cont2+0x02],ebx   ; Dst_BpS

   mov ecx,[esp+OFF_STCK+4]   ; In1
   mov esi,[ecx+VB_BITS]

   mov ebx,[ecx+VB_BPS]       ; Src_BpS
   mov [LoopU8_j1+0x09],ebx

   mov edx,[ecx+VB_CMAP]
   mov ebx,[eax+VB_H]   ; ebx = Out->H
   sub esp,8

   mov [LoopU8_i1+0x0a],edx  ; CMap

   xor edx,edx
   mov eax,[ecx+VB_W]   ; In->W   
   shl eax,16   
   idiv ebp             ; eax = ki
   mov [esp+0], eax     ; esp+0 = di

   xor edx,edx
   mov eax,[ecx+VB_H]   ; In->H
   shl eax,16
   idiv ebx
   mov [esp+4],eax      ; esp+4 = dj

   mov ecx,ebx
   xor edx,edx          ; edx = kj
   neg ebp              ; -W
   xor eax,eax
   dec ecx
   jge OkU8_j1
   add esp,8
   Leave

align4
LoopU8_j1:
   add dx,[esp+4]     ; kj+=dj
   jnc LoopU8_Cont2
   add esi,0x12345678   ; Src += Src_BpS
LoopU8_Cont2:
   add edi,0x12345678   ; Dst += Dst_BpS

OkU8_j1:
   push edx
   push esi
   push edi
   xor ebx,ebx
   push ebp            ; saves i=-W

   xor edx,edx         ;edx = ki = 0
align4
LoopU8_i1:

   mov al, [esi]   ; Src[i]
   test al,al
   jz LoopU8_Cont
   mov bx,[0x12345678+eax*2]
   mov [edi+ebp*2],bx
LoopU8_Cont:
   add dx,[esp+4*4+0]     ; ki+=di
   adc esi,0x0   
   inc ebp
   jl near LoopU8_i1

   pop ebp
   pop edi
   pop esi
   pop edx
   dec ecx
   jge near LoopU8_j1
   add esp,8
   Leave

;//////////////////////////////////////////////////////////////////////
