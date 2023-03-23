;
; MACRO for timing. NASM.
;  Total additional code size is 0xb0.
;  this keep code alignment right.

;//////////////////////////////////////////////////////////////////////////

DATA

extrn _RCount_
extrn _Tics_
_Cur_Count_:   dd 0
extrn _EAX_Sv_
extrn _EBX_Sv_
extrn _ECX_Sv_
extrn _EDX_Sv_
extrn _EDI_Sv_
extrn _ESI_Sv_
extrn _EBP_Sv_;
extrn _ESP_Sv_;
extrn _Print_Tics_
extrn _Extra1_;
extrn _Extra2_;
extrn _Extra3_;
extrn _Extra4_;
extrn _f1_;
extrn _f2_;
extrn _f3_;
extrn _f4_;
extrn _f5_;
extrn _f6_;
extrn _f7_;
extrn _f8_;

%macro RDTSC_IN   0
   mov [_EAX_Sv_],eax
   mov [_EBX_Sv_],ebx
   mov [_ECX_Sv_],ecx
   mov [_EDX_Sv_],edx
   mov [_EDI_Sv_],edi
   mov [_ESI_Sv_],esi
   mov [_EBP_Sv_],ebp
   mov [_ESP_Sv_],esp
   xor eax,eax   
   mov [_Cur_Count_],eax
.RDTSC_Loop_:
   db 0x0f,0x31
   mov [_Tics_],eax
   mov eax, [_EAX_Sv_]
   cld
   nop
   nop
   nop
   nop
   nop
   nop
   nop
   nop
   nop
   nop
%endmacro

%macro RDTSC_OUT   0
   clc
   db 0x0f,0x31
   sub eax,[_Tics_]
   sub eax,15+8
   mov edx,[_Cur_Count_]
   mov [_RCount_+edx*4],eax
   inc edx
   mov [_Cur_Count_],edx
   cmp edx,16
   jne .RDTSC_Keep_
   fstp qword [_f1_]
   fstp qword [_f2_]
   fstp qword [_f3_]
   fstp qword [_f4_]
   fstp qword [_f5_]
   fstp qword [_f6_]
   fstp qword [_f7_]
   fstp qword [_f8_]
   jmp _Print_Tics_
.RDTSC_Keep_:
   mov esp, [_ESP_Sv_]
   mov ebp, [_EBP_Sv_]
   mov esi, [_ESI_Sv_]
   mov edi, [_EDI_Sv_]
   mov edx, [_EDX_Sv_]
   mov ecx, [_ECX_Sv_]
   mov ebx, [_EBX_Sv_]
   mov eax, [_EAX_Sv_]
   jmp .RDTSC_Loop_   
%endmacro
 
;//////////////////////////////////////////////////////////////////////////
