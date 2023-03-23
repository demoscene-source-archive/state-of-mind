;//////////////////////////////////////////////////////////////////////////

[BITS 32]
DATA

;%include "../../include/rdtsc.h"

%define QUANTUM 2
%define TQUANTUM 2

%macro DO_PIXEL2 1
   mov cl,[ebp+ebx*TQUANTUM]    ; 1 ticks
   add esi,eax

   mov al,[ebp+ebx*TQUANTUM+1]    ; 1 ticks
   mov [edi+%{1}*QUANTUM],cl

   adc bl,dl                           ; 1 tick
   add ecx,edx

   adc bh,dh                           ; 1 tick
   mov [edi+%{1}*QUANTUM+1],al
   
   mov cl,[ebp+ebx*TQUANTUM]    ; 1 ticks
   add esi,eax

   mov al,[ebp+ebx*TQUANTUM+1]    ; 1 ticks
   mov [edi+%{1}*QUANTUM+2],cl

   adc bl,dl                           ; 1 tick
   add ecx,edx

   adc bh,dh                           ; 1 tick
   mov [edi+%{1}*QUANTUM+3],al
%endmacro

%macro DO_PIXEL 1
   mov ax, [ebp+ebx*TQUANTUM]
	add ecx, edx      ; Vf += dVf

	adc bh,dh         ; Vi += dVi
	add esi,eax       ; Uf += dUf

	adc bl,dl         ; Ui += dUi
	add ecx, edx

   mov [edi+%{1}*QUANTUM],ax
   mov cx, [ebp+ebx*TQUANTUM]

	adc bh,dh
	add esi,eax

	adc bl,dl
	mov [edi+%{1}*QUANTUM+QUANTUM],cx
%endmacro

      ; 9 ticks
%macro DO_LAST_PIXEL 1
   mov ax, [ebp+ebx*TQUANTUM]
	add ecx, edx

	adc bh,dh
	add esi,eax

	adc bl,dl
	add ecx, edx

   mov [edi+%{1}*QUANTUM],ax
   mov cx, [ebp+ebx*TQUANTUM]    ; AGI Stall

	adc bh,dh
	add esi,eax

	adc bl,dl
	mov [edi+%{1}*QUANTUM+QUANTUM],cx
	
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
extrn Mip_Mask
extrn Span_Len
extrn Span_Y

_dy_: dd 0
_U2_: dd 0,0
_V2_: dd 0,0
_U_: dd 0,0
_V_: dd 0,0

DENORM_32: dd 12582912.0             ; 0x4b400000
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

;RDTSC_IN

   mov edi, [Scan_Start]
   mov esi, [Scan_H]

      ; we ASSUME that y is <16384, so that upper 16 bits of ebx
      ; remain zeroed, in the inner loop. How dirty...:)

   mov ebx, edi     ;%ebx = y
   inc edi                 ; y++

   imul edi, [_RCst_+PIX_BPS]
   mov [_dy_], esi

   fld dword [DyiZ]         ; dw
   fmul dword [%{4}]        ; L.dw
   fld dword [DyU]          ; du|L.dw
   fld dword [_CST_65536_]  ; F|du|L.dw
   fmul st1,st0             ; F|F.du|L.dw
   fmul dword [DyV]         ; F.dv|F.du|L.dw
   fxch st2                 ; dw|du|dv

   add edi, [_RCst_+BASE_PTR]
   mov eax,[UV_Src]

   mov [.Loop_Left+0x04],eax
   mov [UV_Src],eax         ; write-cache flush

align2
.Loop_Y:
   mov ebp, [Scan_Pt2+ebx*4]
   mov eax, [Scan_Pt1+ebx*4]
   sub eax, ebp            ; eax = -Len
   jl near .Loop_X

.Loop_Y_Cont:
   add edi,[_RCst_+PIX_BPS]
   inc ebx                 ; y++
   dec dword [_dy_]
   jg near .Loop_Y

   fcompp      ; dw|du|dv
   fstp st0
;RDTSC_OUT
   Leave

align2
.Loop_X:

            ; First, compute U & V in 8:32 fixed point

   fld dword [Scan_iZ+ebx*4]  ; w |F.dw|F.du|L.dv
   fld dword [_CST_65536_]
   fdiv st1                   ; =>1/w | w |L.dw|F.du|F.dv

;/////////////////////// 12 ticks //////////////////////////

   neg ebp
   neg eax                    ; eax = Len > 0
   mov [Span_Y], ebx
   push edi
   mov [Span_Len], ebp

   mov edx, eax
   lea edi, [edi+ebp*QUANTUM] ; %edi: Dst

   and edx, %{1}-1            ; edx = Left_Over
   mov ebp,[UV_Src]

   shr eax, %{2}              ; eax = Len in span
   push edx

   mov cl,[edi]
   dec eax                    ; <= !! test for the 'jge' below
   mov [edi],cl

;///////////////////////////////////////////////////////////

         ; compute U/U_, V/V_ once for all   

   fld dword [Scan_ViZ+ebx*4]  ; vw | z | w |dw|du|dv
   fmul st1                    ; V | z | w |...
   fxch st1                    ; z | V | w |...
   fmul dword [Scan_UiZ+ebx*4] ; U | V | w |...
   fld dword [DyiZ]            ; dw | U | V | w |...
   fld st1                     ; U | dw | U | V | w |...
   fmul st1                    ; U.dw | dw | U | V | w |...
   fxch st1                    ; dw | U.dw | U | V | w |...
   fmul st3                    ; V.dw | U.dw | U | V | w |...
   fxch st3                    ; V | U.dw | U | V.dw | w |...
   fistp qword [_V_]           ; U.dw | U | V.dw | w |...

   fsub st5                    ; U_ | U | V.dw | w |...
   fxch st1                    ; U | U_ | V.dw | w |...
   fistp qword [_U_]           ; U_ | V.dw | w |...
   fxch st1                    ; V.dw | U_ | w |...
   fsub st5                    ; V_ | U_ | w |...
   fxch st2                    ; w | U_ | V_ |...

         ; Load integer U/V coordinates

   mov esi, [_U_]      ; Uf
   mov bl,[_U_+4]      ; Ui
   mov ecx, [_V_]      ; Vf
   mov bh,[_V_+4]      ; Vi

;   and ebx, dword [Mip_Mask]

   jge .Not_Short      ; => we have at least 1 full span to draw
   dec edx             ; should we really do the fdiv?
   jg .Do_First_FDiv

;////////////// plot the only pixel and exit //////////////

   mov dx,[ebp+ebx*TQUANTUM]
   mov [edi], dx

   pop ebp  ; junk
   pop edi
   fstp st0
   fcompp      ; dw|du|dv
   mov ebx, [Span_Y]
   jmp .Loop_Y_Cont

;////////////// plot fist pixel of short span  ////////////

.Do_First_FDiv:       ; w|U|Vdw|du|dv

   fld dword [%{3}+edx*4]   ;
   fmul st4
   fsubp st1            ; w' | U | V |dw|du|dv
   fld dword [_CST_1_]  ; 1 | w' | U | V |dw|du|dv
   fdiv st1             ; =>1/w' | w' | U | V |dw|du|dv   

      ; lost fdiv here. Warm up the cache for texture,
      ; and plot the first pixel

   mov dx,[ebp+ebx*TQUANTUM]
   mov [edi], dx

   jmp .Left

;//////////// plot fist pixel of regular span /////////////

.Not_Short:
   fsub st3             ; w' | U | V |dw|du|dv
   fld dword [_CST_1_]  ; 1 | w' | U | V |dw|du|dv
   fdiv st1             ; =>1/w' | w' | U | V |dw|du|dv   

      ; lost fdiv here. Warm up the cache for texture,
      ; and plot the first pixel

   mov dx,[ebp+ebx*TQUANTUM]
   mov [edi], dx

align2
.Loop1:

      ; Compute gradients and next U, V        
      ; state: z | w | U | V |dw|du|dv

      ; Don't touch the flags!! we're either coming
      ; from .Loop_X above, or looping back from
      ; inner loop below


   fld st2              ; U | z | w | U | V |dw|du|dv
   fmul st1             ; dU | z | w | U | V |dw|du|dv
   fxch st1             ; z | dU | w | U | V |dw|du|dv
   fmul st4             ; dV | dU | w | U | V |dw|du|dv

   fistp qword [_V_]    ; dU | w | U | V |dw|du|dv
   fld st0              ; dU | dU | w | U | V |dw|du|dv
   fistp qword [_U_]    ; dU | w | U | V |dw|du|dv
   fmul st4             ; dU.dw | w | U | V |dw|du|dv
   faddp st2            ; w | U' | V |dw|du|dv
   fild qword [_V_]     ; dV | w | U' | V |dw|du|dv
   fmul st4
   faddp st3            ; w | U' | V' |dw|du|dv

   mov edx, [esp]       ; <= improves pairing, but not AGI stall
   jg .Not_Last

      ; Last n-pixels span . We should launch the fdiv
      ; for a smaller span that comes just after...

   fld dword [%{3}+edx*4]  ; rescale increment by Left/L
   fmul st4
   fsubp st1               ; w' | U' | V' |dw|du|dv
   fld dword [_CST_1_]  ; 1 | w | U | V | dw|du|dv
   fdiv st1             ; =>1/w | w | U | V | dw|du|dv

      ; FIX: Instead: compute REAL final u and v, to 
      ; avoid overflow

   jmp .Setup

.Not_Last:  ; launch fdiv.

   fsub st3             ; w' | U' | V' |dw|du|dv
   fld dword [_CST_1_]  ; 1 | w | U | V | dw|du|dv
   fdiv st1             ; =>1/w | w | U | V | dw|du|dv

.Setup:

   push eax
   mov edx, [_V_]      ; dVf

   mov eax, [_U_]      ; dUf
   mov dl, [_U_+4]     ; dUi

   mov dh, [_V_+4]     ; dVi

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

   DO_PIXEL (%{1}-2)
   pop eax
   lea edi, [edi+%{1}*QUANTUM]
   dec eax
   jge near .Loop1

;/////////////  Finish with left over ///////////////

align2
.Left:       ; state: 1/w | w | U | V | dw|du|dv

   pop ebp
   dec ebp
   jge .Finish
   fcompp
   fcompp      ; dw|du|dv

.End:
   pop edi
   mov ebx, [Span_Y]
   jmp .Loop_Y_Cont

.Finish:

      ; 16 ticks
   fmul st2,st0      ; 1-3
   fmulp st3,st0     ; 2-4
   fstp st0          ; 4
   fistp qword [_U_]    ; dV | dw|du|dv   ; 5-11
   fistp qword [_V_]    ; dw|du|dv   ; 12-18

   mov edx, [_V_]      ; dVf
   lea edi, [edi+QUANTUM*ebp+QUANTUM]

   mov dh, [_V_+4]     ; dVi
   xor ebp,-1

   mov eax, [_U_]      ; dUf
   mov dl, [_U_+4]     ; dUi

.Loop_Left:
   mov ax,[0x401d0000+ebx*TQUANTUM]
   add ecx,edx

   adc bh,dh
   add esi,eax

   adc bl,dl
   mov [edi+ebp*QUANTUM],ax

   inc ebp
   jl .Loop_Left

   jmp .End

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

globl _Draw_UVc_16

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
   mov dword [.Loop+4],eax
   mov dword [.First+4],eax
   mov dword [.Only_One+4],eax
   mov dword [.Last+4],eax

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
   mov [_V2_],ebp             ; <=Len
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

   mov bl, [_U_+4]  ; Ui
   xor ebp,-1
   mov bh, [_V_+4]  ; Vi
   add ebp,2        ; <=[neg ebp/inc ebp] <=> [xor ebp,-1/add ebp,2]
                    ; hence, we avoid a second AGI stall below

   jne .No_Early_Out    ; exit if ebp=0 ?

      ; plot the first pixel

.Only_One:
   mov ax,[0x1234567+ebx*TQUANTUM]    ; AGI stall here
   pop ecx
   mov [edi+ebp*QUANTUM-QUANTUM],ax
   mov ebx,ecx
   fcompp
   fstp st0
   pop edi
   je near .Loop_Y_Cont      ; ==jmp

.No_Early_Out:

   fld st3                    ; dw | U | V | w |dw|du|dv
   fimul dword [_V2_]         ; Len.dw | U | V | w | dw|du|dv
   fsubrp st3                 ; U|V| w' | dw|du|dv
   fld dword [_CST_1_]
   fdivrp st3                 ; U|V| =>z' | dw|du|dv

      ; plot the first pixel during the fdiv...
      ; 6 ticks until the 'fmul st3' below
      ; this will warm up the cache for destination too

.First:

   mov ax,[0x1234567+ebx*TQUANTUM]    ; AGI stall here
   mov ecx,[_V_]      ; ecx = Vf
   mov [edi+ebp*QUANTUM-QUANTUM],ax
   mov esi,[_U_]      ; esi = Uf

   fmul st3           ; U.dw |V| z' | dw|du|dv
   fxch st1           ; V | U.dw | z' | dw|du|dv
   fmul st3           ; V.dw | U.dw | z' | dw|du|dv

   fsubr st5          ; Vw|U.dw| z' | dw|du|dv
   fxch st1
   fsubr st4          ; Uw|Vw| z' | dw|du|dv
   fxch st1
   fmul st2           ; dV|Uw| z' | dw|du|dv
   fxch st1           ; Uw|dV| z' | dw|du|dv
   fmulp st2          ; dV| dU | dw|du|dv

   ;fadd qword [DENORM_64] ; <= you can use this trick to save some ticks
   ;fstp qword [_V2_]      ; if you don't use the FPU in 32-bits
                           ; precision mode. I do use it (so fdiv are
                           ; faster). We could use "fadd dword [DENORM_32]
                           ; but since I need the result in 8:32 fixed
                           ; point precision, it won't work. I really
                           ; need a 64-bits integer output. 
   fistp qword [_V2_]      ; Too bad.:)
   ;fadd qword [DENORM_64]
   ;fstp qword [_U2_]
   fistp qword [_U2_]

   mov edx, [_V2_]         ; dVf
   mov al,[_V2_+4]         ; dVi

   add ecx,edx
   mov dh,al               ; ch = dVi

   adc bh,dh
   mov eax,[_U2_]          ; dUf

   add esi,eax             ; Uf += dUf
   mov dl,[_U2_+4]         ; dUi

   adc bl,dl
   inc ebp
   jne near .Pre_Loop

.Last:
   mov ax,[0x1234567+ebx*TQUANTUM]
   pop ebx

   mov [edi-QUANTUM],ax

   pop edi
   jmp .Loop_Y_Cont

.Pre_Loop:
;   cmp ebp,-16
;   jl .Loop
;   push [J_Tab+16*4+ebp*4]
;   mov ebp,[UV_Src]
;   mov ax,bx
;   mov ebx,eax
;   xor ax,ax
;   sub ebp,eax
;   pop eax
;   call [eax]
;   jmp Last

.Loop:
   mov ax,[0x1234567+ebx*TQUANTUM]     ; (2c if non-aligned)
   add ecx,edx

   adc bh,dh
   add esi,eax

   adc bl,dl
   mov [edi+ebp*QUANTUM-QUANTUM],ax    ; 1c. (2c if non-aligned)

   inc ebp
   jl .Loop
   jge .Last      ; ==jmp

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

%if 1 ;%ifdef DASJADSKL

extrn UV_Mult_Tab_I   ;EXTERN float UV_Mult_Tab_I[64*256][2];
extrn UV_Mult_Tab_II  ;EXTERN float UV_Mult_Tab_II[64*256][2];

extrn Cf_32_RB        ; USHORT Cf_32_RB[65536];
extrn Cf_32_G         ; USHORT Cf_32_RB[65536];

global _Draw_UVc_Bilin_16

_Draw_UVc_Bilin_16:

   Enter

   mov edi, [Scan_Start]
   mov ebx, edi     ;%ebx = y
          ; we ASSUME that y is <16384, so that upper 16 bits of ebx
          ; remain zeroed, in the inner loop. How dirty...:)

   inc edi                    ; y++
   imul edi, [_RCst_+PIX_BPS]
   fld dword [DyV]
   fmul dword [_CST_65536_]
   add edi, [_RCst_+BASE_PTR]
   mov esi, [Scan_H]
   fld dword [DyU]
   fmul dword [_CST_65536_]
   mov [_dy_], esi
   mov eax,[UV_Src]
   fld dword [DyiZ]          ; dw|du|dv
   mov dword [.Loop+3],eax
   mov dword [.First+4],eax
   mov dword [.Only_One+4],eax
   mov dword [.Last+4],eax
   add eax,256*TQUANTUM
   mov dword [.Loop2+3],eax
   xor esi,esi    ; <= zeroed once for all

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
   mov [_V2_],ebp             ; <=Len
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

   mov bl, [_U_+4]  ; Ui
   xor ebp,-1
   mov bh, [_V_+4]  ; Vi
   add ebp,2        ; <=[neg ebp/inc ebp] <=> [xor ebp,-1/add ebp,2]
                    ; hence, we avoid a second AGI stall below

   jne .No_Early_Out    ; exit if ebp=0 ?

      ; plot the first pixel

.Only_One:
   mov ax,[0x1234567+ebx*TQUANTUM]    ; AGI stall here
   pop ecx
   mov [edi+ebp*QUANTUM-QUANTUM],ax
   mov ebx,ecx
   fcompp
   fstp st0
   pop edi
   je near .Loop_Y_Cont      ; ==jmp

.No_Early_Out:

   fld st3                    ; dw | U | V | w |dw|du|dv
   fimul dword [_V2_]         ; Len.dw | U | V | w | dw|du|dv
   fsubrp st3                 ; U|V| w' | dw|du|dv
   fld dword [_CST_1_]
   fdivrp st3                 ; U|V| =>z' | dw|du|dv

      ; plot the first pixel during the fdiv...
      ; 6 ticks until the 'fmul st3' below
      ; this will warm up the cache for destination too

.First:

   mov ax,[0x1234567+ebx*TQUANTUM]    ; AGI stall here
   mov ecx,[_V_]           ; ecx = Vf
   mov [edi+ebp*QUANTUM-QUANTUM],ax
   mov eax,0xffff0000      ; mask

   fmul st3           ; U.dw |V| z' | dw|du|dv
   fxch st1           ; V | U.dw | z' | dw|du|dv
   fmul st3           ; V.dw | U.dw | z' | dw|du|dv

   fsubr st5          ; Vw|U.dw| z' | dw|du|dv
   fxch st1
   fsubr st4          ; Uw|Vw| z' | dw|du|dv
   fxch st1
   fmul st2           ; dV|Uw| z' | dw|du|dv
   fxch st1           ; Uw|dV| z' | dw|du|dv
   fmulp st2          ; dV| dU | dw|du|dv

   fistp qword [_V2_]
   fistp qword [_U2_]

   and ecx,eax
   and [_U2_],eax    ; only keep 16 upper bits

   and [_V2_],eax    ; only keep 16 upper bits
   and eax,[_U_]     ; eax = Uf. only keep 16 upper bits

   add ecx,[_V2_]    ; ecx = Vf

   adc bh,[_V2_+4]
   add eax,[_U2_]          ; Uf += dUf

   adc bl,[_U2_+4]

   inc ebp
   jne near .Loop

.Last:
   mov ax,[0x1234567+ebx*TQUANTUM]
   pop ebx

   mov [edi-QUANTUM],ax

   pop edi
   jmp .Loop_Y_Cont

.Loop:
   mov esi,[0x1234567+ebx*TQUANTUM]     ; (2c if non-aligned)
   mov edx,ecx

;RDTSC_IN
   shr edx,18     ; dh=Vf&0x3f00
   mov ax,si

   shr esi,16
   push ebp

   mov ch,dh
   mov edx,eax

   shr edx,24
   lea ebp, [esi*4]

   mov dh,ch         ; edx = 0 | V|U      <= Tab

   fld  dword [Cf_32_RB+ebp]        ;
   fmul dword [UV_Mult_Tab_I+edx*8+4]   ;
   mov si,ax
   fld  dword [UV_Mult_Tab_I+edx*8+4]   ;
   fmul dword [Cf_32_G+ebp]             ; G|RB

   fld  dword [Cf_32_RB+esi*4]        ; RB| Go|RBo
   fmul dword [UV_Mult_Tab_I+edx*8]
   fld  dword [UV_Mult_Tab_I+edx*8]
   fmul dword [Cf_32_G+esi*4]         ; G|RB | Go|RBo
   fxch st1
   faddp st3                          ; G|Go|RB*

.Loop2:
   mov esi,[0x1234567+ebx*TQUANTUM]   ; (2c if non-aligned)

   faddp st1                          ; G*|RB*

   mov ax,si
   shr esi,16

   fld  dword [Cf_32_RB+esi*4]        ; RB|x
   fmul dword [UV_Mult_Tab_II+edx*8+4]; RB |G*|RB*
   fld  dword [UV_Mult_Tab_II+edx*8+4];
   fmul dword [Cf_32_G+esi*4]         ; G*|RB* |G|RB
   fxch st1
   faddp st3                          ; G* | G|RB**

   mov si,ax
   add ecx,[_V2_]

   faddp st1                          ; G**|RB**

   fld  dword [Cf_32_RB+esi*4]        ; RB
   fmul dword [UV_Mult_Tab_II+edx*8]  ;
   fld  dword [UV_Mult_Tab_II+edx*8]  ; x|RB
   fmul dword [Cf_32_G+esi*4]         ; G*|RB* |G**|RB**
   fxch st1                           ; RB|G|G|RB
   faddp st3                          ; G|G|RB*

   adc bh,[_V2_+4]
   pop ebp

   faddp st1                          ; G*|RB
   fxch st1                           ; RB|G*
   fadd dword [DENORM_32]
   fxch st1
   fadd dword [DENORM_32]
   fxch st1
   fstp dword [_U_]    ; =>RB
   fstp dword [_V_]    ; =>G

   mov si, [_V_]
   mov dx, [_U_]

   and si,0x07e0
   and dx,0xf81f

   or si,dx
   add eax,[_U2_]

   adc bl,[_U2_+4]
   mov [edi+ebp*QUANTUM-QUANTUM],si   ; 1c. (2c if non-aligned)

;RDTSC_OUT
   inc ebp
   jl near .Loop
   jge near .Last      ; ==jmp

%endif   ; DSJASKLAS

;//////////////////////////////////////////////////////////////////////////
