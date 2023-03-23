;//////////////////////////////////////////////////////////////////////////

[BITS 32]
DATA

%define QUANTUM 1
%define TQUANTUM 1

%macro DO_PIXEL 1
   mov al, [ebp+ebx*TQUANTUM]
	add ecx, edx

	adc bh,dl	
	add esi,eax

	adc bl,dh
   mov ah, [ebp+ebx*TQUANTUM]

	add ecx, edx
	adc bh,dl

	add esi,eax
	adc bl,dh

	mov [edi+%{1}*QUANTUM],ax
%endmacro

%macro DO_LAST_PIXEL 1
   mov al, [ebp+ebx*TQUANTUM]
	add ecx, edx

	adc bh,dl	
	add esi,eax

	adc bl,dh
   mov ah, [ebp+ebx*TQUANTUM]
	mov [edi+%{1}*QUANTUM],ax

%endmacro

%macro DO_ONE_PIXEL 0
   mov al,[0x1234567+ebx*TQUANTUM]
   add esi,ecx

   adc bl,cl
   add edx,eax

   adc bh,ch
   mov [edi+ebp*QUANTUM],al
%endmacro

;//////////////////////////////////////////////////////////////////////////

extrn _RCst_
extrn UV_Src
extrn DyU
extrn DyV
extrn DyiZ
extrn Scan_U
extrn Scan_V
extrn Scan_UiZ
extrn Scan_ViZ
extrn Scan_iZ
extrn Scan_Start
extrn Scan_H
extrn Scan_Pt1
extrn Scan_Pt2
extrn _Div_Tab_1_
extrn Mip_Mask

%define SIZE   (4+4*4)
%define _V2_   esp+20+4*3
%define _U2_   esp+20+4*2
%define _V_    esp+20+4*1
%define _U_    esp+20+4*0

;DENORM:  dd 12582912.0             ; 0x4b400000
;DENORM2: dq 6755399441055744.0     ; 0x4338000000000000

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

      ; This is *the* big macro  
      ; %1=COUNT (64/32/16/8..)  %2=SHIFT (6/5/4/3..) 
      ; %3=Div_Table %4=Constant.
      ; %5=func name. 

%macro MKLOOP 5

;//////////////////////////////////////////////////////////////////////////

extrn %{3}
extrn %{4}
globl %{5}

%{5}:

   Enter

   sub esp, SIZE      ; [ Scan_H | U | V | U2 | V2 ]

   mov edi, [Scan_Start]
   mov ebx, edi     ;%ebx = y
      ; we ASSUME that y is <16384, so that upper 16 bits of ebx
      ; remain zeroed, in the inner loop. How dirty...:)

   inc edi                 ; y++
   fld dword [ %{4} ]      ; Cst
   imul edi, [_RCst_+PIX_BPS]
   fld dword [DyV]
   mov esi, [Scan_H]
   fxch st1
   fmul st1,st0            ; Cst | dv
   add edi, [_RCst_+BASE_PTR]
   fld dword [DyU]
   mov [esp], esi
   fxch st1
   fmul st1,st0            ; Cst | du | dv
   mov eax,[UV_Src]
   fld dword [DyiZ]        ; dw|du|dv
   mov dword [.Loop_Left+2],eax
   fxch st1
   fmulp st1,st0           ; dw|du|dv

align2
.Loop_Y:

   mov ebp, [Scan_Pt2+ebx*4]
   mov ecx, [Scan_Pt1+ebx*4]
   sub ecx, ebp            ; ecx = -Len
   jl .Loop_X

.Loop_Y_Cont:

   add edi,[_RCst_+PIX_BPS]
   inc ebx               ; y++
   dec dword [esp]
   jg .Loop_Y

   fcompp      ; dw|du|dv
   add esp, SIZE
   fstp st0
   Leave

align2
.Loop_X:
            ; First, compute U & V in parallel

   fld dword [Scan_iZ+ebx*4]    ; w |dw|du|dv
   fld1
   fdiv st1                   ; 1/w | w |dw|du|dv

   push ebx
   neg ebp
   push edi
   neg ecx                    ; ecx = Len > 0
   mov edx, ecx
   push esi                   ; esi = Scan_H
   lea edi, [edi+ebp*QUANTUM] ; %edi: Dst
   and edx, %{1}-1            ; edx = Left_Over
   mov ebp,[UV_Src]
   shr ecx, %{2}              ; ecx = Len in span
   push edx
   dec ecx

   fld dword [Scan_ViZ+ebx*4] ; vw | z | w |dw|du|dv
   fld dword [Scan_UiZ+ebx*4] ; uw | vw | z | w |dw|du|dv
   fld st0                    ; uw | uw | vw | z | w |dw|du|dv
   fmul st3
   ;fadd qword [DENORM2]
   ;fstp qword [_U_]
   fistp dword [_U_]          ; uw | vw | z | w |dw|du|dv
   fxch st2                   ; z | vw | uw | w |dw|du|dv
   fmul st1   
   ;fadd qword [DENORM2]
   ;fstp qword [_V_]
   fistp dword [_V_]          ; vw | uw | w |dw|du|dv
   fxch st2                   ; w | uw | vw |dw|du|dv

   jge .Real_Span             ; ecx==0?

   fld dword [%{3}+edx*4]     ; S | w | uw | vw | dw|du|dv   
   jmp .Small

.Real_Span

      ; w | uw | vw | dw|du|dv
      ; [advance 1 span + launch div]

   fsub st3       ; w' | uw | vw | dw|du|dv
   fxch st1       ; uw | w' | vw | dw|du|dv
   fsub st4       ; uw' | w' | vw | dw|du|dv
   fxch st1       ; w | uw' | vw | dw|du|dv
   fld1           ; 1 | w | uw' | vw | dw|du|dv
   fxch st3       ; vw | w | uw' | 1 | dw|du|dv
   fsub st6       ; vw' | w | uw' | 1 | dw|du|dv
   fxch st3       ; 1 | w | uw' | vw' | dw|du|dv
   fdiv st1       ; =>1/w | w | uw' | vw' | dw|du|dv

      ; lost fdiv here. Prewarm the cache

   mov eax,[edi]
;   mov bh,[_V_+2]
;   mov bl,[_U_+2]
;   mov bx,[ebp+ebx*TQUANTUM]
   
align2
.Loop1:

      ; Compute next U, V        ; state: 1/w | w | uw | vw |...

      ; Don't touch the flags!! we're either coming
      ; from .Loop_X above, or looping back from
      ; .Setup


   fld st3              ; vw | 1/w | w | uw |  vw 
   fmul st1             ; V | 1/w | w | uw |  vw 
   fistp dword [_V2_]   ; 1/w | w | uw |  vw 
   fld st2
   fmulp st1            ; U | w | uw |  vw 
   fistp dword [_U2_]   ; w | uw |  vw 

   jg .Not_Last

      ; Last n-pixels span. We have to launch the fdiv
      ; for a smaller span. Just rescale th dw,du,dv

   mov edx, [esp]   
   fld dword [%{3}+edx*4] ; Load junk in st0, to keep stack count right
   test edx,edx
   jz .Setup

.Small:

   fld st6       ; dv | S | w | uw | vw | dw|du|dv
   fmul st1      ; dv' | S | w | uw | vw | dw|du|dv
   fsubp st4     ; S | w | uw | vw' | dw|du|dv
   fld st5       ; du | S | w | uw | vw' | dw|du|dv
   fmul st1      ; du' | S | w | uw | vw' | dw|du|dv
   fsubp st3     ; S | w | uw' | vw' | dw|du|dv
   fmul st4
   fsubp st1     ; w' | uw' | vw' | dw|du|dv
   fld1          ; 1 | w' | uw' | vw' | dw|du|dv
   test ecx,ecx
   fdiv st1      ; =>1/w | w' | uw' | vw' | dw|du|dv
   jl near .Left 
   jmp .Setup    ; Draw this span

.Not_Last:  ; advance 1 regular span + launch fdiv

   fsub st3       ; w' | uw | vw | dw|du|dv
   fxch st1       ; uw | w' | vw | dw|du|dv
   fsub st4       ; uw' | w' | vw | dw|du|dv
   fxch st1       ; w | uw' | vw | dw|du|dv
   fld1           ; 1 | w | uw' | vw | dw|du|dv
   fxch st3       ; vw | w | uw' | 1 | dw|du|dv
   fsub st6       ; vw' | w | uw' | 1 | dw|du|dv
   fxch st3       ; 1 | w | uw' | vw' | dw|du|dv
   fdiv st1       ; =>1/w | w | uw' | vw' | dw|du|dv

align2
.Setup:

   push ecx
   mov edx, [_V2_+4]    ; V2

   mov eax, [_U2_+4]    ; U2   
   mov ecx, [_V_+4]     ; V

   mov esi, [_U_+4]     ; U
   mov [_V_+4], edx     ; V2 => V

   mov [_U_+4], eax     ; U2 => U
   sub edx, ecx         ; edx = V2-V

   sub eax, esi         ; eax = U2-U
   rol ecx,16

   rol eax, 16-%{2}     ; eax = dVf
   rol esi, 16          ; esi = Uf

   rol edx, 16-%{2}     ; edx = dUf
   mov bx,si

   mov dh,al
   mov bh,cl

      ;//// unrolled loop ////

%if (%{1}>2)
   DO_PIXEL 0
%endif
%if (%{1}>4)
   DO_PIXEL 2
   DO_PIXEL 4
%endif
%if (%{1}>8)
   DO_PIXEL 6
   DO_PIXEL 8
   DO_PIXEL 10
   DO_PIXEL 12
%endif
%if (%{1}>16)
   DO_PIXEL 14
   DO_PIXEL 16
   DO_PIXEL 18
   DO_PIXEL 20
   DO_PIXEL 22
   DO_PIXEL 24
   DO_PIXEL 26
   DO_PIXEL 28
%endif
%if (%{1}>32)
   DO_PIXEL 30
   DO_PIXEL 32
   DO_PIXEL 34
   DO_PIXEL 36
   DO_PIXEL 38
   DO_PIXEL 40
   DO_PIXEL 42
   DO_PIXEL 44
   DO_PIXEL 46
   DO_PIXEL 48
   DO_PIXEL 50
   DO_PIXEL 52
   DO_PIXEL 54
   DO_PIXEL 56
   DO_PIXEL 58
   DO_PIXEL 60
%endif
   DO_LAST_PIXEL (%{1}-2)

   pop ecx
   add edi, %{1}*QUANTUM
   dec ecx
   jge near .Loop1

;/////////////  Finish with left over ///////////////

align2
.Left:       ; state: 1/w | w | uw | vw | dw|du|dv

   pop ebp
   fstp st1              ; 1/w | uw | vw |dw|du|dv
   test ebp, ebp         ; ebp = Left_Over
   jz .End

   fmul st1, st0         ; 1/w | U2 | vw
   lea edi, [edi+QUANTUM*ebp]
   fmulp st2             ; U2 | V2
   mov esi, [_U_-4]      ; U
   fisub dword [_U_-4]   ; U2-U | V2
   fld dword [_Div_Tab_1_+ebp*4] ; S | U2-U | V2
   rol esi, 16           ; esi = Uf
   neg ebp
   fxch st2              ; V2 | U2-U | S
   fisub dword [_V_-4]   ; V2_V | U2-U | S
   fmul st2              ; dV | U2-U | S
   mov edx, [_V_-4]      ; V
   mov bx,si
   fistp dword [_V2_-4]  ; => dV
   fmulp st1             ; dU
   mov eax, [_V2_-4]     ; eax = dV
   rol edx, 16           ; edx = Vf
   fistp dword [_U2_-4]  ; => dV
   mov ecx, [_U2_-4]     ; ecx = dU
   rol eax, 16           ; eax = dVf
   rol ecx, 16           ; ecx = dUf
   mov bh,dl 
   mov ch,al
   
.Loop_Left:
   DO_ONE_PIXEL
   inc ebp
   jl .Loop_Left

   pop esi
   pop edi
   pop ebx
   jmp .Loop_Y_Cont

align4
.End:
   pop esi
   pop edi
   fcompp
   fstp st0      ; dw|du|dv
   pop ebx
   jmp .Loop_Y_Cont

%endmacro         ; MKLOOP

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////


   ;  Generate all calls.

MKLOOP 64,6, _Div_Tab_64_ , _CST_64_, _Draw_UVc_64_8

MKLOOP 32,5, _Div_Tab_32_ , _CST_32_, _Draw_UVc_32_8

MKLOOP 16,4, _Div_Tab_16_ , _CST_16_, _Draw_UVc_16_8

MKLOOP 8,3, _Div_Tab_8_ , _CST_8_, _Draw_UVc_8_8

MKLOOP 4,2, _Div_Tab_4_ , _CST_4_, _Draw_UVc_4_8

MKLOOP 2,1, _Div_Tab_2_ , _CST_2_, _Draw_UVc_2_8

;//////////////////////////////////////////////////////////////////////////

%macro DO_LIN_PIXEL 0
   mov al,[0x1234567+ebx*TQUANTUM]
   add esi,ecx

   adc bl,cl
   add edx,eax

   adc bh,ch
   mov [edi+ebp*QUANTUM-QUANTUM],al
%endmacro

globl _Draw_UVc_8

_Draw_UVc_8:

   Enter

   sub esp, SIZE      ; [ Scan_H | U | V | U2 | V2 ]

   mov edi, [Scan_Start]
   mov ebx, edi     ;%ebx = y
      ; we ASSUME that y is <16384, so that upper 16 bits of ebx
      ; remain zeroed, in the inner loop. How dirty...:)

   inc edi                 ; y++
   imul edi, [_RCst_+PIX_BPS]
   fld dword [DyV]
   add edi, [_RCst_+BASE_PTR]
   mov esi, [Scan_H]
   fld dword [DyU]
   mov [esp], esi
   mov eax,[UV_Src]
   fld dword [DyiZ]        ; dw|du|dv
   mov dword [.Loop+2],eax
   mov dword [.First+2],eax
   mov dword [.Early_Out+2],eax
   mov dword [.Last+2],eax

align2
.Loop_Y:

   mov ebp, [Scan_Pt2+ebx*4]
   mov ecx, [Scan_Pt1+ebx*4]
   sub ebp, ecx            ; ebp = Len>0
   jg .Loop_X

.Loop_Y_Cont:

   add edi,[_RCst_+PIX_BPS]
   inc ebx                 ; y++
   dec dword [esp]
   jg .Loop_Y

   fcompp      ; dw|du|dv
   add esp, SIZE
   fstp st0
   Leave

align2
.Loop_X:    ; First, compute U & V in parallel

   fld dword [Scan_iZ+ebx*4]  ; w |dw|du|dv
   fld1
   fdiv st1                   ; =>1/w | w |dw|du|dv

;   mov eax,[edi+ecx*QUANTUM]     ; prewarm the cache...

   push edi
   push ebx
   push esi                   ; esi = Scan_H
   neg ecx
   mov [_V2_-4],ebp
   lea edi, [edi+ecx*QUANTUM] ; %edi: Dst

   fld dword [Scan_ViZ+ebx*4]
   fld dword [Scan_UiZ+ebx*4] ; uw | vw | z | w |dw|du|dv
   fmul st2                   ; U | vw | z | w |dw|du|dv
   fxch st1                   ; vw | U | z | w |dw|du|dv
   fmulp st2                  ; U | V | w |dw|du|dv
   fist dword [_U_-4]         ; U | V | w |dw|du|dv
   fxch st1                   ; V | U | w |dw|du|dv
   fist dword [_V_-4]         ; V | U | w |dw|du|dv
   fld st3                    ; dw | V | U | w |dw|du|dv
   fimul dword [_V2_-4]      ; Len.dw | V | U | w | dw|du|dv
   fsubrp st3                 ; V|U| w' | dw|du|dv
   fld1
   fdivrp st3                 ; V|U| =>z' | dw|du|dv

      ; plot the first pixel during the fdiv...

   mov esi, [_U_-4]           ; U
   rol esi, 16                ; esi = Uf
   mov edx, [_V_-4]          ; V
   mov bx,si
   rol edx, 16           ; edx = Vf
   neg ebp
   mov bh,dl

.First:
   mov al,[0x1234567+ebx*TQUANTUM]
   inc ebp
   mov [edi+ebp*QUANTUM-QUANTUM],al
   je .Early_Out

   fld st3
   fmulp st1                  ; V.dw |U| z' | dw|du|dv
   fsubr st5
   fmul st2                   ; DV | U | z' | dw|du|dv
   fistp dword [_V2_-4]       ; U | z' | dw|du|dv
   fmul st2
   mov eax, [_V2_-4]     ; eax = dV
   fsubr st3
   
   fmulp st1                  ; DU | dw|du|dv
   fistp dword [_U2_-4]

   mov ecx, [_U2_-4]     ; ecx = dU
   rol eax, 16           ; eax = dVf
   rol ecx, 16           ; ecx = dUf
   add edx,eax
   adc bh,al

   add esi,ecx
   adc bl,cl

   inc ebp
   mov ch,al
   je .Last

.Loop:
   DO_LIN_PIXEL
   inc ebp
   jl .Loop
.Last:
   mov al,[0x1234567+ebx*TQUANTUM]
   pop esi
   mov [edi-QUANTUM],al
   pop ebx
   pop edi
   jmp .Loop_Y_Cont

.Early_Out:
   mov al,[0x1234567+ebx*TQUANTUM]
   pop esi
   fcompp
   fstp st0
   mov [edi+ebp*QUANTUM-QUANTUM],al
   pop ebx
   pop edi
   jmp .Loop_Y_Cont

;//////////////////////////////////////////////////////////////////////////
