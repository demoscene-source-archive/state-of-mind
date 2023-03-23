;//////////////////////////////////////////////////////////////////////////

[BITS 32]
DATA

%include "../../include/rdtsc.h"

%define QUANTUM 2
%define TQUANTUM 2

%macro DO_PIXEL 1
   mov ax, [ebp+ebx*TQUANTUM]
	add ecx, edx

	adc bh,dl
	add esi,eax

	adc bl,dh
   mov [edi+%{1}*QUANTUM],ax

   mov ax, [ebp+ebx*TQUANTUM]
	add ecx, edx

	adc bh,dl
	add esi,eax

	adc bl,dh
	mov [edi+%{1}*QUANTUM+QUANTUM],ax
%endmacro

      ; 9 ticks
%macro DO_LAST_PIXEL 1
   mov ax, [ebp+ebx*TQUANTUM]
	add ecx, edx

	adc bh,dl
	add esi,eax

	adc bl,dh
   mov [edi+%{1}*QUANTUM],ax

   pop eax
   mov bx, [ebp+ebx*TQUANTUM]    ; AGI Stall

	mov [edi+%{1}*QUANTUM+QUANTUM],bx

%endmacro

%macro DO_ONE_PIXEL 0
   mov ax,[0x1234567+ebx*TQUANTUM]
   add esi,ecx

   adc bl,cl
   add edx,eax

   adc bh,ch
   mov [edi+ebp*QUANTUM],ax
%endmacro

;//////////////////////////////////////////////////////////////////////////

extrn _RCst_
extrn UV_Src
extrn DyU
extrn DyV
extrn DyiZ
extrn Scan_UiZ
extrn Scan_ViZ
extrn Scan_iZ
extrn Scan_Start
extrn Scan_H
extrn Scan_Pt1
extrn Scan_Pt2
extrn _Div_Tab_1_
extrn _CST_1_
extrn _CST_65536_

_dy_: dd 0
_U2_: dd 0,0
_V2_: dd 0,0
_U_: dd 0,0
_V_: dd 0,0

;DENORM_32: dd 12582912.0             ; 0x4b400000
;DENORM_64: dq 6755399441055744.0     ; 0x4338000000000000

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

RDTSC_IN
   mov edi, [Scan_Start]
   mov ebx, edi     ;%ebx = y
      ; we ASSUME that y is <16384, so that upper 16 bits of ebx
      ; remain zeroed, in the inner loop. How dirty...:)

   inc edi                 ; y++
   fld dword [ %{4} ]      ; Cst
   imul edi, [_RCst_+PIX_BPS]
   fld dword [DyV]         ; Cst|dv
   mov esi, [Scan_H]
   fxch st1                ; dv|Cst
   fmul st1,st0            ; Cst | dv
   add edi, [_RCst_+BASE_PTR]
   fld dword [DyU]         ; du | Cst|dv
   mov [_dy_], esi
   fxch st1                ; Cst |du|dv
   fmul st1,st0            ; Cst | du | dv
   mov eax,[UV_Src]
   fld dword [DyiZ]        ; dw| Cst|du|dv
   mov [.Loop_Left+4],eax
   mov [UV_Src],eax
   fmulp st1               ; dw|du|dv

align2
.Loop_Y:

   mov ebp, [Scan_Pt2+ebx*4]
   mov eax, [Scan_Pt1+ebx*4]
   sub eax, ebp            ; eax = -Len
   jl .Loop_X

.Loop_Y_Cont:

   add edi,[_RCst_+PIX_BPS]
   inc ebx                 ; y++
   dec dword [_dy_]
   jg .Loop_Y

   fcompp      ; dw|du|dv
   fstp st0
RDTSC_OUT
   Leave

align2
.Loop_X:
            ; First, compute U & V in parallel

   fld dword [Scan_iZ+ebx*4]  ; w |dw|du|dv
   fld dword [_CST_65536_]
   fdiv st1                   ; 1/w | w |dw|du|dv

      ; 12 ticks 
   neg ebp
   neg eax                    ; eax = Len > 0   
   push ebx
   push edi

   mov edx, eax
   lea edi, [edi+ebp*QUANTUM] ; %edi: Dst

   and edx, %{1}-1            ; edx = Left_Over
   mov ebp,[UV_Src]

   shr eax, %{2}              ; ecx = Len in span
   push edx

   mov cl,[edi]
   dec eax
   mov [edi],cl

      ; ?22? ticks
   fld dword [Scan_ViZ+ebx*4] ; vw | z | w |dw|du|dv
   fld dword [Scan_UiZ+ebx*4] ; uw | vw | z | w |dw|du|dv
   fld st0                    ; uw | uw | vw | z | w |dw|du|dv
   fmul st3                   ; U | uw | vw | z | w |dw|du|dv
   fxch st3                   ; z | uw | vw | U | w |dw|du|dv
   fmul st2                   ; V | uw | vw | U | w |dw|du|dv
   fistp qword [_V_]          ; uw | vw | U | w |dw|du|dv
   fxch st2                   ; U | vw | uw | w |dw|du|dv
   fistp qword [_U_]          ; vw | uw | w |dw|du|dv

         ; load integer U,V once for all   

   mov bh,[_V_+4]
   fxch st2                   ; w | uw | vw |dw|du|dv
   mov bl,[_U_+4]
   mov ecx, [_V_]             ; V
   mov esi, [_U_]             ; U

   jge .Real_Span             ; ecx==0?

   fld dword [%{3}+edx*4]     ; S | w | uw | vw | dw|du|dv   
   jmp .Small

.Real_Span:

      ; w | uw | vw | dw|du|dv
      ; [advance 1 span + launch div]

   fsub st3       ; w' | uw | vw | dw|du|dv        ; 1-3
   fxch st1       ; uw | w' | vw | dw|du|dv
   fsub st4       ; uw' | w' | vw | dw|du|dv       ; 2-4
   fxch st1       ; w | uw' | vw | dw|du|dv
   fld dword [_CST_65536_]  ; 1 | w | uw' | vw | dw|du|dv ; 3
   fxch st3       ; vw | w | uw' | 1 | dw|du|dv
   fsub st6       ; vw' | w | uw' | 1 | dw|du|dv      ; 4-6
   fxch st3       ; 1 | w | uw' | vw' | dw|du|dv

   fdiv st1       ; =>1/w | w | uw' | vw' | dw|du|dv  ; 6-22

      ; lost fdiv here. Warms up the cache for texture
      ; 4 ticks


   mov cx,[ebp+ebx*TQUANTUM]    ; AGI stall

align2
.Loop1:

      ; Compute next U, V        ; state: 1/w | w | uw | vw |...

      ; Don't touch the flags!! we're either coming
      ; from .Loop_X above, or looping back from
      ; .Setup


   fld st3              ; vw | 1/w | w | uw |  vw 
   fmul st1             ; V | 1/w | w | uw |  vw 
   fxch st1             ; 1/w | V | w | uw |  vw 
   fmul st3             ; U | V | w | uw |  vw 
   fistp qword [_U2_]   ; V | w | uw |  vw 
   fistp qword [_V2_]   ; w | uw |  vw 

   jg .Not_Last

      ; Last n-pixels span. We have to launch the fdiv
      ; for a smaller span. Just rescale th dw,du,dv

   mov edx, [esp]   
   fld dword [%{3}+edx*4] ; Load junk in st0, to keep stack count right
   test edx,edx
   jz near .Setup

.Small:        ; state: S | w | uw | vw | dw|du|dv

   fld st6       ; dv | S | w | uw | vw | dw|du|dv
   fmul st1      ; dv' | S | w | uw | vw | dw|du|dv
   fsubp st4     ; S | w | uw | vw' | dw|du|dv
   fld st5       ; du | S | w | uw | vw' | dw|du|dv
   fmul st1      ; du' | S | w | uw | vw' | dw|du|dv
   fsubp st3     ; S | w | uw' | vw' | dw|du|dv
   fmul st4
   fsubp st1     ; w' | uw' | vw' | dw|du|dv
   fld dword [_CST_65536_]  ; 1 | w' | uw' | vw' | dw|du|dv
   test eax,eax
   fdiv st1      ; =>1/w | w' | uw' | vw' | dw|du|dv
   jl near .Left 
   jmp .Setup    ; Draw this span

.Not_Last:  ; advance 1 regular span + launch fdiv. 23 ticks.


   fsub st3       ; w' | uw | vw | dw|du|dv
   fxch st1       ; uw | w' | vw | dw|du|dv
   fsub st4       ; uw' | w' | vw | dw|du|dv
   fxch st1       ; w | uw' | vw | dw|du|dv
   fld dword [_CST_65536_]  ; 1 | w | uw' | vw | dw|du|dv
   fxch st3       ; vw | w | uw' | 1 | dw|du|dv
   fsub st6       ; vw' | w | uw' | 1 | dw|du|dv
   fxch st3       ; 1 | w | uw' | vw' | dw|du|dv
   fdiv st1       ; =>1/w | w | uw' | vw' | dw|du|dv

align2
.Setup:

   push eax
   mov edx, [_V2_]    ; dVf

   mov eax, [_U2_]    ; dUf   
   mov ecx, [_V_]     ; Vf

   mov [_V_], edx     ; V2 => V
   sub edx, ecx         ; edx = V2-V

   rol ecx,16
   mov esi, [_U_]     ; Uf

   mov [_U_], eax     ; U2 => U
   sub eax, esi       ; eax = U2-U

   rol eax, 16-%{2}   ; eax = dVf
   rol esi, 16        ; esi = Uf

   rol edx, 16-%{2}   ; edx = dUf
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

   add edi, %{1}*QUANTUM
   dec eax
   jge near .Loop1

;/////////////  Finish with left over ///////////////

align2
.Left:       ; state: 1/w | w | uw | vw | dw|du|dv

   pop ebp
   fstp st1              ; 1/w | uw | vw |dw|du|dv
   test ebp, ebp         ; ebp = Left_Over
   jz near .End

   fmul st1, st0         ; 1/w | U2 | vw
   lea edi, [edi+QUANTUM*ebp]
   fmulp st2             ; U2 | V2
   mov esi, [_U_]        ; U
   fisub dword [_U_]     ; U2-U | V2
   fld dword [_Div_Tab_1_+ebp*4] ; S | U2-U | V2
   rol esi, 16           ; esi = Uf
   neg ebp
   fxch st2              ; V2 | U2-U | S
   fisub dword [_V_]     ; V2_V | U2-U | S
   fmul st2              ; dV | U2-U | S
   mov edx, [_V_]        ; V
   mov bx,si
   fistp dword [_V2_]    ; => dV
   fmulp st1             ; dU
   mov eax, [_V2_]       ; eax = dV
   rol edx, 16           ; edx = Vf
   fistp dword [_U2_]    ; => dV
   mov ecx, [_U2_]       ; ecx = dU
   rol eax, 16           ; eax = dVf
   rol ecx, 16           ; ecx = dUf
   mov bh,dl 
   mov ch,al

.Loop_Left:
   DO_ONE_PIXEL
   inc ebp
   jl .Loop_Left

   pop edi
   pop ebx
   jmp .Loop_Y_Cont

align4
.End:
   pop edi
   fcompp
   fstp st0      ; dw|du|dv
   pop ebx
   jmp .Loop_Y_Cont

%endmacro         ; MKLOOP

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////


   ;  Generate all calls.

MKLOOP 64,6, _Div_Tab_64_ , _CST_64_, _Draw_UVc_64_16

MKLOOP 32,5, _Div_Tab_32_ , _CST_32_, _Draw_UVc_32_16

MKLOOP 16,4, _Div_Tab_16_ , _CST_16_, _Draw_UVc_16_16

MKLOOP 8,3, _Div_Tab_8_ , _CST_8_, _Draw_UVc_8_16

MKLOOP 4,2, _Div_Tab_4_ , _CST_4_, _Draw_UVc_4_16

MKLOOP 2,1, _Div_Tab_2_ , _CST_2_, _Draw_UVc_2_16

;//////////////////////////////////////////////////////////////////////////

global _Draw_UVc_16

_Draw_UVc_16:

   Enter

   mov edi, [Scan_Start]
   mov ebx, edi     ;%ebx = y
          ; we ASSUME that y is <16384, so that upper 16 bits of ebx
          ; remain zeroed, in the inner loop. How dirty...:)

   inc edi                 ; y++
   imul edi, [_RCst_+PIX_BPS]
   fld dword [DyV]
   fmul dword [_CST_65536_]
   add edi, [_RCst_+BASE_PTR]
   mov esi, [Scan_H]
   fld dword [DyU]
   fmul dword [_CST_65536_]
   mov [_dy_], esi
   mov eax,[UV_Src]
   fld dword [DyiZ]        ; dw|du|dv
   mov dword [.Loop+3],eax
   mov dword [.First+3],eax
   mov dword [.Last+3],eax
align2
.Loop_Y:

   mov ebp, [Scan_Pt2+ebx*4]
   mov ecx, [Scan_Pt1+ebx*4]
   sub ebp, ecx            ; ebp = Len
   jg .Loop_X

.Loop_Y_Cont:

   add edi,[_RCst_+PIX_BPS]
   inc ebx                 ; y++
   dec dword [_dy_]
   jg .Loop_Y

   fcompp      ; dw|du|dv
   fstp st0
   Leave

align2
.Loop_X:    ; First, compute U & V in parallel

   fld dword [Scan_iZ+ebx*4]  ; w |dw|du|dv
   fld dword [_CST_65536_]
   fdiv st1                   ; =>1/w | w |dw|du|dv

      ; lost ticks here

   push edi
   xor ecx,-1
   push ebx
   inc ecx
   lea edi, [edi+ecx*QUANTUM] ; %edi: Dst.   [AGI stall]
   mov [_V2_],ebp
   cmp [edi],eax  ; warms up the cache for destination

   fld dword [Scan_ViZ+ebx*4]
   fld dword [Scan_UiZ+ebx*4] ; uw | vw | z | w |dw|du|dv
   fmul st2                   ; U | vw | z | w |dw|du|dv
   fxch st1                   ; vw | U | z | w |dw|du|dv
   fmulp st2                  ; U | V | w |dw|du|dv
   fld st0
   fistp qword [_U_]          ; U | V | w |dw|du|dv
   fld st1
   fistp qword [_V_]          ; U | V | w |dw|du|dv
   fld st3                    ; dw | U | V | w |dw|du|dv
   fimul dword [_V2_]         ; Len.dw | U | V | w | dw|du|dv
   fsubrp st3                 ; U|V| w' | dw|du|dv
   fld dword [_CST_1_]
   fdivrp st3                 ; U|V| =>z' | dw|du|dv

times 2 nop       ; <= this align the .Loop code 

      ; plot the first pixel during the fdiv...
      ; 6 ticks until the 'fmul st3' below
      ; this will warm up the cache for destination too


   mov bl, [_U_+4]  ; Ui
   xor ebp,-1

   mov bh, [_V_+4]  ; Vi
   add ebp,2        ; <=[neg ebp/inc ebp] <=> [xor ebp,-1/add ebp,2]
                    ; hence, we avoid a second AGI stall below

.First:
   mov eax,[0x1234567+ebx*TQUANTUM]    ; AGI stall here

   mov [edi+ebp*QUANTUM-QUANTUM],ax
   jne .No_Early_Out    ; exit if ebp=0 ?

      ; the fdiv is finishing here...

   pop ebx
   pop edi
   fcompp
   fstp st0
   jmp .Loop_Y_Cont

.No_Early_Out:

      ; 26 ticks to go until the last fistp

   fmul st3           ; U.dw |V| z' | dw|du|dv
   fxch st1
   fmul st3

   mov edx,[_V_]      ; edx = Vf
   mov esi,[_U_]      ; esi = Uf

   fsubr st5          ; Vw|U.dw| z' | dw|du|dv
   fxch st1
   fsubr st4          ; Uw|Vw| z' | dw|du|dv
   fxch st1
   fmul st2           ; dV|Uw| z' | dw|du|dv

   ;fadd qword [DENORM_64] ; <= you can use this trick to save some ticks
   ;fstp qword [_V2_]      ; if you don't use the FPU in 32-bits
                           ; precision mode. I do use it (so fdiv are
                           ; faster). We could use "fadd dword [DENORM_32]
                           ; but since I need the result in 8:32 fixed
                           ; point precision, it won't work. I really
                           ; need a 64-bits integer output. 
   fistp qword [_V2_]      ; Too bad.:)

   fmulp st1               ; dU| dw|du|dv

   mov ecx, [_V2_]         ; dVf
   mov al,[_V2_+4]         ; dVi

   add edx,ecx
   mov ch,al               ; ch = dVi

   ;fadd qword [DENORM_64]
   ;fstp qword [_U2_]
   fistp qword [_U2_]

      ; 8 ticks until the .Loop

   adc bh,ch
   mov eax,[_U2_]          ; dUf

   add esi,eax             ; Uf += dUf
   mov cl,[_U2_+4]         ; dUi

   adc bl,cl
   mov [.Loop+0x09],eax    ; plug increment. 4 ticks.   
                           ; destination (.Loop+9) is 4-aligned
                           ; so we avoid some (5-ticks) additional delay

   inc ebp
   jne .Loop


.Last:
   mov eax,[0x1234567+ebx*TQUANTUM]
   pop ebx

   mov [edi-QUANTUM],ax

   pop edi
   jmp .Loop_Y_Cont

.Loop:         ; 4ticks/pixel, without misalignments

   mov eax,[0x1234567+ebx*TQUANTUM]     ; (2c if non-aligned)
   add esi,0x12345678

   adc bl,cl
   add edx,ecx

   adc bh,ch
   mov [edi+ebp*QUANTUM-QUANTUM],ax    ; 1c. (2c if non-aligned)

   inc ebp
   jl .Loop
   jmp .Last


;//////////////////////////////////////////////////////////////////////////
