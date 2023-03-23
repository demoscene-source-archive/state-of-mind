;//////////////////////////////////////////////////////////////////////////

[BITS 32]

globl Do_Bilin_Line_4
globl Do_Bilin_Line_UV_Map_8

%define SLOPE_U   (esp)
%define SLOPE_V   (esp+4)
%define CUR_U     (esp+8)
%define CUR_V     (esp+12)

DATA

;/////////////////////////////////////////////////////////////////

Do_Bilin_Line_4:   ; +0: PIXEL *Out  +4: USHORT *V 
                   ; +8: USHORT *dV +12: INT W

   Enter

   xor ebx, ebx
   mov edi,[esp+OFF_STCK+0]   ; Out
   mov esi,[esp+OFF_STCK+4]   ; V

   mov ebx,[esi]      ; ebx = V
   mov ebp, [esp+OFF_STCK+12]   ; W
   mov edx,ebx    ; edx = Vo

.I_X:
   add esi,2
   mov ebx,[esi]
   mov ecx,ebx
   sub ecx,edx
   sar ecx,2      ; ecx=dV

   mov al,dh
   add edx,ecx
   mov ah,dh
   mov [edi],ax
   add edx,ecx

   mov al,dh
   add edx,ecx
   mov ah,dh
   mov [edi+2],ax
   mov edx,ebx    ; edx = Vo

   add edi,4
   dec ebp
   jg .I_X

   mov esi,[esp+OFF_STCK+4]   ; V
   mov edi,[esp+OFF_STCK+8]   ; dV

   mov eax, [esp+OFF_STCK+12]   ; W
.II_X:
   mov bx,[edi+eax*2]
   add [esi+eax*2],bx
   dec eax
   jge .II_X

   Leave

;/////////////////////////////////////////////////////////////////

Do_Bilin_Line_UV_Map_8:   ; +0: PIXEL *Out 
                          ; +4: SHORT *UV  +8: SHORT *dUV
                          ; +12: INT W   +16: PIXEL *Map

   Enter
   sub esp,16

   mov edi,[esp+OFF_STCK+0+16]    ; Out
   mov esi,[esp+OFF_STCK+4+16]    ; UV
   mov ebp,[esp+OFF_STCK+12+16]   ; W
   mov eax,[esp+OFF_STCK+16+16]   ; *Map
   mov [.Mod1+0x02],eax
   mov [.Mod2+0x02],eax
   mov [.Mod3+0x02],eax
   mov [.Mod4+0x02],eax
   mov [.Mod5+0x02],eax
   mov [.Mod6+0x02],eax
   mov [.Mod7+0x02],eax
   mov [.Mod8+0x02],eax

   mov ax,[esi]
   mov bx,[esi+2]
   mov [CUR_U],ax
   mov [CUR_V],bx

.I_X:  ; edx = U, ebx = V

   add esi,4
   mov ax,[esi]
   mov cx,ax
   mov dx,[CUR_U]
   sub cx,dx
   mov [CUR_U],ax
   sar cx,3      ; ecx=dU
   mov [SLOPE_U],cx

   mov ax,[esi+2]
   mov cx,ax
   mov bx,[CUR_V]
   sub cx,bx
   mov [CUR_V],ax
   sar cx,3      ; ecx=dV
   xor eax,eax

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod1:
   mov al,[0x12345678+eax]
   mov [edi],al
   add bx,cx

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod2:
   mov al,[0x12345678+eax]
   mov [edi+1],al
   add bx,cx

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod3:
   mov al,[0x12345678+eax]
   mov [edi+2],al
   add bx,cx

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod4:
   mov al,[0x12345678+eax]
   mov [edi+3],al
   add bx,cx

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod5:
   mov al,[0x12345678+eax]
   mov [edi+4],al
   add bx,cx

   mov al,dh
   add edx,[SLOPE_U]
   mov ah,bh
.Mod6:
   mov al,[0x12345678+eax]
   mov [edi+5],al
   add ebx,ecx

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod7:
   mov al,[0x12345678+eax]
   mov [edi+6],al
   add bx,cx

   mov al,dh
   mov ah,bh
.Mod8:
   mov al,[0x12345678+eax]
   mov [edi+7],al

   add edi,8
   dec ebp
   jg near .I_X

   add esp,16
   mov esi,[esp+OFF_STCK+4]    ; UV
   mov edi,[esp+OFF_STCK+8]    ; dUV
   mov eax, [esp+OFF_STCK+12]  ; W
   lea eax,[eax*2+2]
.II_X:
   mov bx,[edi+eax*2]
   add [esi+eax*2],bx
   dec eax
   jge .II_X

   Leave

;/////////////////////////////////////////////////////////////////

%ifdef USELESS_NOW

globl Do_Bilin_Line_UV_Map_32b_8
globl Do_Bilin_Line_8
globl Do_Bilin_Line_UV_8

;/////////////////////////////////////////////////////////////////

Do_Bilin_Line_UV_8:   ; +0: PIXEL *Out 
                      ; +4: UINT *UV  +8: UINT *dUV
                      ; +12: INT W

   Enter
   sub esp,16

   mov edi,[esp+OFF_STCK+0+16]   ; Out
   mov esi,[esp+OFF_STCK+4+16]   ; UV
   mov ebp,[esp+OFF_STCK+12+16]   ; W

   mov ebx,[esi]
   mov [CUR_U],ebx
   mov ebx,[esi+4]      ; ebx = V
   mov [CUR_V],ebx

.I_X:  ; edx = U, ebx = V
   add esi,8
   mov eax,[esi]
   mov ecx,eax
   mov edx,[CUR_U]
   sub ecx,edx
   mov [CUR_U],eax
   shr ecx,3      ; ecx=dU
   mov [SLOPE_U],ecx

   mov eax,[esi+4]
   mov ecx,eax
   mov ebx,[CUR_V]
   sub ecx,ebx
   mov [CUR_V],eax
   shr ecx,3      ; ecx=dV
   xor eax,eax

   mov al,dh
   add edx,[SLOPE_U]
   mov ah,bh
   mov [edi],ax
   add ebx,ecx

   mov al,dh
   add edx,[SLOPE_U]
   mov ah,bh
   mov [edi+2],ax
   add ebx,ecx

   mov al,dh
   add edx,[SLOPE_U]
   mov ah,bh
   mov [edi+4],ax
   add ebx,ecx

   mov al,dh
   add edx,[SLOPE_U]
   mov ah,bh
   mov [edi+6],ax
   add ebx,ecx

   mov al,dh
   add edx,[SLOPE_U]
   mov ah,bh
   mov [edi+8],ax
   add ebx,ecx

   mov al,dh
   add edx,[SLOPE_U]
   mov ah,bh
   mov [edi+10],ax
   add ebx,ecx

   mov al,dh
   add edx,[SLOPE_U]
   mov ah,bh
   mov [edi+12],ax
   add ebx,ecx

   mov al,dh
   mov ah,bh
   mov [edi+14],ax

   add edi,16
   dec ebp
   jg near .I_X

   add esp,16
   mov esi,[esp+OFF_STCK+4]   ; UV
   mov edi,[esp+OFF_STCK+8]   ; dUV
   mov eax,[esp+OFF_STCK+12]  ; W
   lea eax,[eax*2+4]
.II_X:
   mov ebx,[edi+eax*4]
   add [esi+eax*4],ebx
   dec eax
   jge .II_X

   Leave

;/////////////////////////////////////////////////////////////////

Do_Bilin_Line_UV_Map_32b_8:   ; +0: PIXEL *Out 
                          ; +4: SHORT *UV  +8: SHORT *dUV
                          ; +12: INT W   +16: PIXEL *Map

   Enter
   sub esp,16

   mov edi,[esp+OFF_STCK+0+16]    ; Out
   mov esi,[esp+OFF_STCK+4+16]    ; UV
;   mov ebp,[esp+OFF_STCK+12+16]   ; W
   mov eax,[esp+OFF_STCK+16+16]   ; *Map
   mov [.Mod1+0x03],eax
   mov [.Mod2+0x03],eax
   mov [.Mod3+0x03],eax
   mov [.Mod4+0x03],eax
   mov [.Mod5+0x03],eax
   mov [.Mod6+0x03],eax
   mov [.Mod7+0x03],eax
   mov [.Mod8+0x03],eax

   mov ax,[esi]
   mov bx,[esi+2]
   mov [CUR_U],ax
   mov [CUR_V],bx

.I_X:  ; edx = U, ebx = V

   add esi,4
   mov ax,[esi]
   mov cx,ax
   mov dx,[CUR_U]
   sub cx,dx
   mov [CUR_U],ax
   sar cx,3      ; ecx=dU
   mov [SLOPE_U],cx

   mov ax,[esi+2]
   mov cx,ax
   mov bx,[CUR_V]
   sub cx,bx
   mov [CUR_V],ax
   sar cx,3      ; ecx=dV
   xor eax,eax

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod1:
   mov ebp,[0x12345678+eax*4]
   mov [edi],ebp
   add bx,cx

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod2:
   mov ebp,[0x12345678+eax*4]
   mov [edi+4],ebp
   add bx,cx

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod3:
   mov ebp,[0x12345678+eax*4]
   mov [edi+8],ebp
   add bx,cx

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod4:
   mov ebp,[0x12345678+eax*4]
   mov [edi+12],ebp
   add bx,cx

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod5:
   mov ebp,[0x12345678+eax*4]
   mov [edi+16],ebp
   add bx,cx

   mov al,dh
   add edx,[SLOPE_U]
   mov ah,bh
.Mod6:
   mov ebp,[0x12345678+eax*4]
   mov [edi+20],ebp
   add ebx,ecx

   mov al,dh
   add dx,[SLOPE_U]
   mov ah,bh
.Mod7:
   mov ebp,[0x12345678+eax*4]
   mov [edi+24],ebp
   add bx,cx

   mov al,dh
   mov ah,bh
.Mod8:
   mov ebp,[0x12345678+eax*4]
   mov [edi+28],ebp

   add edi,32
   dec dword [esp+OFF_STCK+12+16]   ; W--
   jg near .I_X

   add esp,16
   mov esi,[esp+OFF_STCK+4]    ; UV
   mov edi,[esp+OFF_STCK+8]    ; dUV
   mov eax, [esp+OFF_STCK+12]  ; W
   lea eax,[eax*2+2]
.II_X:
   mov bx,[edi+eax*2]
   add [esi+eax*2],bx
   dec eax
   jge .II_X

   Leave

;/////////////////////////////////////////////////////////////////

Do_Bilin_Line_8:   ; +0: PIXEL *Out +4: USHORT *V
                   ; +8: USHORT *dV +12: INT W

   Enter

   xor ebx, ebx
   mov edi,[esp+OFF_STCK+0]   ; Out
   mov esi,[esp+OFF_STCK+4]   ; V

   mov bx,[esi]      ; ebx = V
   mov ebp, [esp+OFF_STCK+12]   ; W
   mov edx,ebx    ; edx = Vo

.I_X:
   add esi,2
   mov bx,[esi]
   mov ecx,ebx
   sub ecx,edx
   shr ecx,3      ; ecx=dV

   mov al,dh
   add edx,ecx
   mov ah,dh
   mov [edi],ax
   add edx,ecx

   mov al,dh
   add edx,ecx
   mov ah,dh
   mov [edi+2],ax
   add edx,ecx

   mov al,dh
   add edx,ecx
   mov ah,dh
   mov [edi+4],ax
   add edx,ecx

   mov al,dh
   add edx,ecx
   mov ah,dh
   mov [edi+6],ax
   mov edx,ebx    ; edx = Vo

   add edi,8
   dec ebp
   jg .I_X

   mov esi,[esp+OFF_STCK+4]   ; V
   mov edi,[esp+OFF_STCK+8]   ; dV

   mov eax, [esp+OFF_STCK+12]   ; W
.II_X:
   mov bx,[edi+eax*2]
   add [esi+eax*2],bx
   dec eax
   jge .II_X

   Leave

;/////////////////////////////////////////////////////////////////

%endif   ; USELESS
