;//////////////////////////////////////////////////////////////////////////

;%include "nasm.h"

;//////////////////////////////////////////////////////////////////////////

[BITS 32]
globl Format_Scanlines
globl Rasterize_Edges
globl Rasterize_UV
globl Rasterize_UV2
globl Rasterize_UVc
globl Rasterize_Grd
globl Rasterize_iZ

globl Exit_Engine_Asm
globl Enter_Engine_Asm

;%define SKIP_Edges
;%define SKIP_iZ
;%define SKIP_UV
;%define SKIP_UV2
;%define SKIP_Grd
%define SKIP_UVc   ;; UNFINISHED!!

;//////////////////////////////////////////////////////////////////////////

extrn Edges_x
extrn Edges_dx
extrn Edges_y
extrn Edges_dy
extrn Edges_z
extrn Edges_dz
extrn Slopes
extrn Slope_Full
extrn Edges_ey
extrn Edges_yf
extrn Edges_yo
extrn Edges_Xe
extrn Edges_dXe
extrn Scan_Start
extrn Scan_H
extrn Nb_Right_Edges
extrn Right_Edges
extrn Nb_Left_Edges
extrn Left_Edges
extrn _RCst_
extrn Nb_Out_Edges
extrn Scan_Pt1
extrn Scan_Pt2

extrn Mip_Scale
extrn Uo_Mip
extrn Vo_Mip
extrn DyU
extrn DxU
extrn dyU
extrn Scan_U
extrn DyV
extrn DxV
extrn dyV
extrn Scan_V
extrn DyS
extrn DxS
extrn dyS
extrn Scan_S
extrn DyiZ
extrn DxiZ
extrn dyiZ
extrn Scan_UiZ
extrn Scan_ViZ
extrn Scan_iZ
extrn aZ2

extrn Out_Vtx
extrn _CST_65536_
extrn _CST_1_

;//////////////////////////////////////////////////////////////////////////

DATA

align4
_P_EPSILON_: dd 0.001      ; SLOPE_EPSILON
_M_EPSILON_: dd -0.001      ; -SLOPE_EPSILON

;%include "../../include/rdtsc.h"

;//////////////////////////////////////////////////////////////////////////

align4

FPUCW_Save: dw 0
FPUCW: dw 0

align4
Buf: times 16 dd 0

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

TEXT

%ifndef SKIP_Edges

; Format_Scanlines:
;   ret

;//////////////////////////////////////////////////////////////////////////

Rasterize_Edges:

;RDTSC_IN
   push ebp
   push edi

   push esi
   push ebx

   sub esp,8
   xor eax,eax

   fld dword [_RCst_+CLIPS+12]
   fistp dword [Scan_Start]
   mov [Scan_H], eax
   mov dword [Nb_Right_Edges],eax
   mov dword [Nb_Left_Edges],eax

   mov edi,[Nb_Out_Edges]      ; edi = Cur_S

.Finish2:

   dec edi
   jge near .Loop_Raster         ; js near .End
   jmp .End

align4
.Loop_Raster:

   fld dword [Edges_dy+edi*4]   ; dy[0]
   fcom dword [_P_EPSILON_]     ; dy[0]
   fstsw ax
   and ah,0x45
   jnz near .Left_Edge

   fld dword [_RCst_+CLIPS+12]      ; Clips[3] | dy[0]
   fld dword [Edges_y+edi*4]        ; y[0] | Clips[3] | dy[0]
   fcom st1
   fstsw ax
   and ah,0x45
   fist dword [esp+4]      ; => y
   jz .Right_Clear1

   fld dword [_RCst_+CLIPS+8]   ; Clips[2] | y[0] | Clips[3] | dy[0]
   fld dword [Edges_y+4+edi*4]  ; y[1] | Clips[2] | y[0] | Clips[3] | dy[0]
   fcom st1
   fstsw ax
   and ah,0x01
   fist dword [esp+0]      ; =>y[1] | Clips[2] | y[0] | Clips[3] | dy[0]
   jz .Right_Ok

.Right_Clear:
   fcompp
.Right_Clear1:
   fcompp
.Right_Clear2:
   fstp st0
   jmp .Finish2

.Right_Ok:
   mov edx, [esp+0]      ; edx = y[1]
   sub edx, [esp+4]      ; edx = yf-y
   jle .Right_Clear

   fld dword [_CST_1_]  ; 1 | y[1] | Clips[2] | y[0] | Clips[3] | dy[0]
   fdivrp st5,st0       ; y[1] | Clips[2] | y[0] | Clips[3] | 1/dy[0]

   mov eax,[Nb_Right_Edges]
   mov [Right_Edges+eax*2],di
   inc eax
   mov [Nb_Right_Edges],eax

   fld st4
   ffree st5      ; 1/dy[0] | y[1] | Clips[2] | y[0] | Clips[3]

   fst dword [Slopes+edi*4]
   fmul dword [Edges_dx+edi*4]   ; Slope | y[1] | Clips[2] | y[0] | Clips[3]
   fst dword [Slope_Full+edi*4] 
   fxch st2                      ; Clips[2] | y[1] | Slope | y[0] | Clips[3]

   fcom st3
   fstsw ax
   and ah,0x01
   jz .No_Right_Clip_Yo

   fxch st3      ; Yo | y[1] | Slope | Clips[2] | Clips[3]

.No_Right_Clip_Yo:
   fistp dword [Edges_yo+edi*4]   ; (INT)Yo=>y 
   fcom st3                       ; y[1] | Slope | Clips[2] | Clips[3]
   fstsw ax
   fstp st2                       ; Slope | y[1] | Clips[3]
   and ah,0x01
   fxch st1                       ; y[1] | Slope | Clips[3]
   jz .No_Right_Clip_Y1

   fxch st2      ; Clips[3] | Slope | y[1]

.No_Right_Clip_Y1:

   fstp st0                      ; Slope | yf
   fxch st1                      ; yf | Slope
   fistp dword [Edges_yf+edi*4]  ; =>yf      ; Slope
   mov edx, [Edges_yo+edi*4]     ; edx = y
   mov eax, [Edges_yf+edi*4]

   sub eax, edx                  ; eax = yf-y
   jle near .Right_Clear2

   fld dword [_CST_65536_]       ; 1<<16 | Slope
   fmul st1,st0                  ; 1<<16 | Slope*
   fild dword [Edges_yo+edi*4]   ; y | 1<<16 | Slope*
   fsub dword [Edges_y+edi*4]    ; ey | 1<<16 | Slope
   fxch st1
   fmul dword [Edges_x+edi*4]    ; aX |ey|Slope*
   fxch st1
   fst dword [Edges_ey+edi*4]    ; ey|aX|Slope*
   fmul st2                      ; X|aX|Slope*
   fxch st2                      ; Slope|aX|X
   fistp dword [Edges_dXe+edi*4] ; => _Slope
   faddp st1

   mov ebp, [Edges_dXe+edi*4]
   add [Scan_H], eax             ; eax = dH

   fistp dword [Edges_Xe+edi*4]  ; => _Xo_ ; 1<<16 | Slope

   mov ecx, [Edges_Xe+edi*4]
   cmp [Scan_Start], edx
   jle .Right_Loop
   mov [Scan_Start], edx

.Right_Loop:
   mov esi,ecx
   inc edx
   shr esi,16
   add ecx,ebp
   mov [Scan_Pt2-4+edx*4],esi
   dec eax

   jg .Right_Loop

   dec edi            ; Cur_S-- 
   jge near .Loop_Raster
   jmp .End

%ifdef DSAKDLASDASDSA
align4
.Worst_Edge:
   fstp st0
   fld dword [Edges_y+edi*4]     ; y[1]
   fcom dword [_RCst_+CLIPS+8]
   fstsw ax
   and ah,0x01
   jnz near .Right_Clear2
   fcom dword [_RCst_+CLIPS+12]
   fstsw ax
   and ah,0x45
   jz near .Right_Clear2
   fistp dword [Edges_yo+edi*4]   ; (INT)Yo=>y
   jmp .Finish2
%endif

align4
.Left_Edge:
   fcom dword [_M_EPSILON_]
   fstsw ax
   and ah,0x01
   jz near .Right_Clear2

   fld dword [_RCst_+CLIPS+8]  ; Clips[2] | dy[0]
   fld dword [Edges_y+edi*4]   ; y[1] | Clips[2] | dy[0]
   fcom st1
   fstsw ax
   and ah,0x01
   fist dword [esp+0]          ; => yf
   jnz near .Right_Clear1

   fld dword [_RCst_+CLIPS+12] ; Clips[3] | y[1] | Clips[2] | dy[0]
   fld dword [Edges_y+4+edi*4] ; y[0] | Clips[3] | y[1] | Clips[2] | dy[0]
   fcom st1
   fstsw ax
   fist dword [esp+4]     ; =>y[0] | Clips[3] | y[1] | Clips[2] | dy[0]
   and ah,0x45
   jz near .Right_Clear

   mov edx, [esp+0]       ; edx = y[1]
   fxch st2               ; y[1] | Clips[3] | y[0] | Clips[2] | dy[0]
   sub edx, [esp+4]       ; edx = yf-y
   jle near .Right_Clear

   fld dword [_CST_1_]    ; 1 | y[1] | Clips[3] | y[0] | Clips[2] | dy[0]
   fdivrp st5,st0         ; y[1] | Clips[3] | y[0] | Clips[2] | 1/dy[0]

   mov eax,[Nb_Left_Edges]
   mov [Left_Edges+eax*2],di
   inc eax
   mov [Nb_Left_Edges],eax

   fld st4
   ffree st5              ; 1/dy[0] | y[1] | Clips[3] | y[0] | Clips[2]

   fst dword [Slopes+edi*4]
   fmul dword [Edges_dx+edi*4]   ; Slope | y[1] | Clips[3] | y[0] | Clips[2]
   ; fst dword [Slope_Full+edi*4] 
   fxch st2                      ; Clips[3] | y[1] | Slope | y[0] | Clips[2]
   fxch st4                      ; Clips[2] | y[1] | Slope | y[0] | Clips[3]

   fcom st3
   fstsw ax
   and ah,0x01
   jz .No_Left_Clip_Yo

   fxch st3       ; Yo | y[1] | Slope | Clips[2] | Clips[3]

.No_Left_Clip_Yo:
   fistp dword [Edges_yo+edi*4]  ; (INT)Yo=>y

   fcom st3                      ; y[1] | Slope | Clips[2] | Clips[3]
   fstsw ax
   fstp st2                      ; Slope | y[1] | Clips[3]
   and ah,0x01
   fxch st1                      ; y[1] | Slope | Clips[3]
   jz .No_Left_Clip_Y1

   fxch st2                      ; Clips[3] | Slope | y[1]

.No_Left_Clip_Y1:
   fstp st0                      ; Slope | yf
   fxch st1                      ; yf | Slope
   fistp dword [Edges_yf+edi*4]  ; =>yf      ; Slope
   mov edx, [Edges_yo+edi*4]     ; edx = y
   mov eax, [Edges_yf+edi*4]     ; yf
   sub eax, edx                  ; eax = yf-y
   jle near .Right_Clear2

   fld dword [_CST_65536_]       ; 1<<16 | Slope
   fmul st1,st0                  ; 1<<16 | Slope*
   fild dword [Edges_yo+edi*4]   ; y | 1<<16 | Slope*
   fsub dword [Edges_y+edi*4]    ; ey | 1<<16 | Slope
   fxch st1
   fmul dword [Edges_x+edi*4]    ; aX |ey|Slope*
   fxch st1
   fst dword [Edges_ey+edi*4]    ; ey|aX|Slope*
   fmul st2                      ; X|aX|Slope*
   fxch st2                      ; Slope|aX|X
   fistp dword [Edges_dXe+edi*4] ; => _Slope
   faddp st1

   fistp dword [Edges_Xe+edi*4]  ; => _Xo_ ; 1<<16 | Slope

   mov ecx, [Edges_Xe+edi*4]
   mov ebp, [Edges_dXe+edi*4]

.Left_Loop:
   mov esi,ecx
   inc edx
   shr esi,16
   add ecx,ebp
   mov [Scan_Pt1-4+edx*4],esi
   dec eax
   jg near .Left_Loop

   dec edi            ; Cur_S-- 
   jge near .Loop_Raster
.End:
   add esp,8
   pop ebx
   pop esi
   pop edi
   pop ebp
;RDTSC_OUT

   ret

%endif      ; SKIP

;//////////////////////////////////////////////////////////////////////////

%ifndef SKIP_UV

Rasterize_UV:

   Enter

   mov edi,[Nb_Right_Edges]
   dec edi
   jl near End_R_UV

align4
Loop_R_UV:   ; edi = i => j
   mov [Buf],edi
   movzx edi,word [Right_Edges+edi*2]
   mov ebx,[Edges_yf+edi*4]   ;yf
   mov esi,[Edges_yo+edi*4]   ;esi = y
   mov [Buf+4],ebx

   fld dword [Edges_ey+edi*4]   ; dy
   fild dword [Scan_Pt2+esi*4]
   fsub dword [Edges_x+edi*4]   ; dx | dy

   mov eax,[Edges_dXe+edi*4]   ; eax = dXe
   push eax
   test eax,eax
   jg R_UV_Not_Neg
   neg dword [esp]
   sar dword [esp],16
   fild dword [esp]
   fchs 
   jmp R_UV_Slope_Ok
R_UV_Not_Neg
   sar dword [esp],16
   fild dword [esp]   ; Slope | dx | dy
R_UV_Slope_Ok:
   fld dword [Mip_Scale]
   fld dword [DyU]
   fld dword [DxU]      ; DxU | DyU | Mip_Scale | Slope | dx | dy
   fld st1         ; DyU | DxU | DyU | Mip_Scale | Slope | dx | dy
   fmul st4
   fadd st1      ; _dU_ | DxU | DyU | Mip_Scale | Slope | dx | dy
   mov ecx,[Out_Vtx+edi*4]
   fistp dword [8+Buf]   ; DxU | DyU | Mip_Scale | Slope | dx | dy

   fld dword [12+ecx]   ; UV[0] | DxU | DyU | Mip_Scale | Slope | dx | dy
   fmul st3
   fld st6         ; dy | UV[0]*Mip_Scale | DxU | DyU | Mip_Scale | Slope | dx | dy
   fmulp st2      ; UV[0]*Mip_Scale | DxU*dy | DyU | Mip_Scale | Slope | dx | dy
   faddp st1      ; dy.DxU+UV[0] | DyU | Mip_Scale | Slope | dx | dy
   fld st4         ; dx | dy.DxU+UV[0] | DyU | Mip_Scale | Slope | dx | dy
   fmulp st2      ; dy.DxU+UV[0] | dx.DyU | Mip_Scale | Slope | dx | dy
   fadd dword [Uo_Mip]
   faddp st1      ; U | Mip_Scale | Slope | dx | dy
   fistp dword [12+Buf]   ; => _U_   ;  Mip_Scale | Slope | dx | dy
   ; fistp dword [dyU]   ; Mip_Scale | Slope | dx | dy

   fld dword [DyV]
   fld dword [DxV]      ; DxV | DyV | Mip_Scale | Slope | dx | dy
   fld st1         ; DyV | DxV | DyV | Mip_Scale | Slope | dx | dy
   fmul st4
   fadd st1      ;  DxV+Slope*DyV | DxV | DyV | Mip_Scale | Slope | dx | dy
   fistp dword [16+Buf]   ; => _dV_

   fld dword [16+ecx]   ; UV[1] | DxV | DyV | Mip_Scale | Slope | dx | dy
   fmul st3
   fld st6         ; dy | UV[1] | DxV | DyV | Mip_Scale | Slope | dx | dy
   fmulp st2      ; UV[1] | DxV.dy | DyV | Mip_Scale | Slope | dx | dy
   faddp st1      ; dy.DxV+UV[1] | DyV | Mip_Scale | Slope | dx | dy
   fld st4         ; dx | dy.DxV+UV[1] | DyV | Mip_Scale | Slope | dx | dy
   fmulp st2      ; dy.DxV+UV[1] | dx*DyV | Mip_Scale | Slope | dx | dy
   fadd dword [Vo_Mip]
   faddp st1      ; V | Mip_Scale | Slope | dx | dy
   fistp dword [20+Buf]   ; => _V_   ;  Mip_Scale | Slope | dx | dy
   ;fistp dword [dyV]   ; Mip_Scale | Slope | dx | dy
   fcompp
   fcompp

   pop ebp         ; dXe>>16   eax = full dXe 
   mov ebx,[dyU]
   mov ecx,[dyV]
   mov edx,[Edges_Xe+edi*4]
   test eax,eax
   jge R_UV_dXe_Not_Neg
   neg eax
   neg ebx
   neg ecx
   neg edx
R_UV_dXe_Not_Neg:
   mov ebp,[20+Buf]   ;_V_
   mov edi,[12+Buf]   ;_U_
Loop2_R_UV:
   mov [Scan_U+esi*4],edi
   mov [Scan_V+esi*4],ebp
   add edi,[8+Buf]      ;_dU_
   add dx,ax
   jnc No_Carry_R_UV
   add edi,ebx
   add ebp,ecx
No_Carry_R_UV:
   inc esi
   add ebp,[16+Buf]   ;_dV_
   cmp [Buf+4],esi
   jg Loop2_R_UV

   mov edi,[Buf]
   dec edi
   jge near Loop_R_UV

End_R_UV:
   Leave

%endif   ; SKIP_UV

;//////////////////////////////////////////////////////////////////////////

%ifndef SKIP_UV2

Rasterize_UV2:

   Enter

   mov edi,[Nb_Right_Edges]
   dec edi
   jl near End_R_UV

align4
Loop_R_UV2:   ; edi = i => j
   mov [Buf],edi
   movzx edi,word [Right_Edges+edi*2]
   mov ebx,[Edges_yf+edi*4]   ;yf
   mov esi,[Edges_yo+edi*4]   ;esi = y
   mov [Buf+4],ebx

   fld dword [Edges_ey+edi*4]   ; dy
   fild dword [Scan_Pt2+esi*4]
   fsub dword [Edges_x+edi*4]   ; dx | dy

   mov eax,[Edges_dXe+edi*4]   ; eax = dXe
   push eax
   test eax,eax
   jg R_UV2_Not_Neg
   neg dword [esp]
   sar dword [esp],16
   fild dword [esp]
   fchs 
   jmp R_UV2_Slope_Ok
R_UV2_Not_Neg
   sar dword [esp],16
   fild dword [esp]   ; Slope | dx | dy
R_UV2_Slope_Ok:
   fld dword [Mip_Scale]
   fld dword [DyU]
   fld dword [DxU]      ; DxU | DyU | Mip_Scale | Slope | dx | dy
   fld st1         ; DyU | DxU | DyU | Mip_Scale | Slope | dx | dy
   fmul st4
   fadd st1      ; _dU_ | DxU | DyU | Mip_Scale | Slope | dx | dy
   mov ecx,[Out_Vtx+edi*4]
   fistp dword [8+Buf]   ; DxU | DyU | Mip_Scale | Slope | dx | dy

   fld dword [20+ecx]   ; N[0] | DxU | DyU | Mip_Scale | Slope | dx | dy
   fmul st3
   fld st6         ; dy | N[0]*Mip_Scale | DxU | DyU | Mip_Scale | Slope | dx | dy
   fmulp st2      ; N[0]*Mip_Scale | DxU*dy | DyU | Mip_Scale | Slope | dx | dy
   faddp st1      ; dy.DxU+N[0] | DyU | Mip_Scale | Slope | dx | dy
   fld st4         ; dx | dy.DxU+N[0] | DyU | Mip_Scale | Slope | dx | dy
   fmulp st2      ; dy.DxU+N[0] | dx.DyU | Mip_Scale | Slope | dx | dy
   fadd dword [Uo_Mip]
   faddp st1      ; U | Mip_Scale | Slope | dx | dy
   fistp dword [12+Buf]   ; => _U_   ;  Mip_Scale | Slope | dx | dy
   ; fistp dword [dyU]   ; Mip_Scale | Slope | dx | dy

   fld dword [DyV]
   fld dword [DxV]      ; DxV | DyV | Mip_Scale | Slope | dx | dy
   fld st1         ; DyV | DxV | DyV | Mip_Scale | Slope | dx | dy
   fmul st4
   fadd st1      ;  DxV+Slope*DyV | DxV | DyV | Mip_Scale | Slope | dx | dy
   fistp dword [16+Buf]   ; => _dV_

   fld dword [24+ecx]   ; N[1] | DxV | DyV | Mip_Scale | Slope | dx | dy
   fmul st3
   fld st6         ; dy | N[1] | DxV | DyV | Mip_Scale | Slope | dx | dy
   fmulp st2      ; N[1] | DxV.dy | DyV | Mip_Scale | Slope | dx | dy
   faddp st1      ; dy.DxV+N[1] | DyV | Mip_Scale | Slope | dx | dy
   fld st4         ; dx | dy.DxV+N[1] | DyV | Mip_Scale | Slope | dx | dy
   fmulp st2      ; dy.DxV+N[1] | dx*DyV | Mip_Scale | Slope | dx | dy
   fadd dword [Vo_Mip]
   faddp st1      ; V | Mip_Scale | Slope | dx | dy
   fistp dword [20+Buf]   ; => _V_   ;  Mip_Scale | Slope | dx | dy
   ;fistp dword [dyV]   ; Mip_Scale | Slope | dx | dy
   fcompp
   fcompp

   pop ebp         ; dXe>>16   eax = full dXe 
   mov ebx,[dyU]
   mov ecx,[dyV]
   mov edx,[Edges_Xe+edi*4]
   test eax,eax
   jge R_UV2_dXe_Not_Neg
   neg eax
   neg ebx
   neg ecx
   neg edx
R_UV2_dXe_Not_Neg:
   mov ebp,[20+Buf]   ;_V_
   mov edi,[12+Buf]   ;_U_
Loop2_R_UV2:
   mov [Scan_U+esi*4],edi
   mov [Scan_V+esi*4],ebp
   add edi,[8+Buf]      ;_dU_
   add dx,ax
   jnc No_Carry_R_UV2
   add edi,ebx
   add ebp,ecx
No_Carry_R_UV2:
   inc esi
   add ebp,[16+Buf]   ;_dV_
   cmp [Buf+4],esi
   jg Loop2_R_UV2

   mov edi,[Buf]
   dec edi
   jge near Loop_R_UV2

End_R_UV2:
   Leave

%endif   ; SKIP_UV2

;//////////////////////////////////////////////////////////////////////////

%ifndef SKIP_UVc

Rasterize_UVc:

   push edi
   push esi

   mov edi,[Nb_Right_Edges]   ; <= assumed being < 65536 :)
   dec edi
   jge near .Loop
.End:
   pop esi
   pop edi
   ret

.Loop:

   push edi

   mov di, word [Right_Edges+edi*2]    ; edi = j

   mov ecx, [Edges_yf+edi*4]               ; yf

   mov esi, [Edges_yo+edi*4]               ; esi = y
   sub ecx,esi                            ; yf-yo = deltaH

   fld dword [Edges_ey+edi*4]      ; dy
   fild dword [Scan_Pt2+esi*4]
   fsub dword [Edges_x+edi*4]      ; dx | dy

   mov eax, [Edges_dXe+edi*4]      ; eax = dXe
   mov edx, [Out_Vtx+edi*4]      ; edx = Out_Vtx[j]   
   mov [Buf],eax

   test eax,eax
   jge .Not_Neg
   neg dword [Buf]
   sar dword [Buf], 16
   fild dword [Buf]     ; Slope|dx|dy
   fchs
   jmp .Slope_Ok  
.Not_Neg:
   sar dword [Buf], 16
   fild dword [Buf]     ; Slope|dx|dy
.Slope_Ok:
   fld dword [DyU]      ; DyU | Slope | dx | dy
   fst dword [Buf+12]   ; incU
   fmul st1             ; * | Slope | dx | dy
   fld dword [DyV]      ; DyV | * | Slope | dx | dy
   fst dword [Buf+24]   ; incV
   fmul st2             ; *V | * | Slope | dx | dy
   fxch st1             ; * | *V | Slope | dx | dy
   fadd dword [DxU]     ; _dU_* | *V | Slope | dx | dy
   fxch st1
   fadd dword[ DxV]     ; _dV_* |_dU_|Slope | dx | dy
   fxch st1
   fstp dword [4+Buf]   ; _dV_* | Slope | dx | dy
   fstp dword [16+Buf]   ; Slope | dx | dy

   fld dword [edx+8]        ; Inv_Z | Slope | dx | dy
   fmul dword [aZ2]       ; *aZ2
   fld dword [edx+12]     ; UV[0] | iZ|Slope | dx | dy
   fmul dword [Mip_Scale] ; U* | iZ|Slope | dx | dy
   fld dword [edx+16]     ; UV[1] | U* | iZ|Slope | dx | dy
   fmul dword [Mip_Scale] ; V* | U* | iZ|Slope | dx | dy
   fxch st1
   fadd dword [Uo_Mip]    ; U* | V* | iZ|Slope | dx | dy
   fld st4
   fmul dword [DyU]     ; dx.dyu | U* | V* | iZ|Slope | dx | dy
   fxch st2             ; V* | U* | dx.dyu | iZ|Slope | dx | dy
   fadd dword [Vo_Mip]  ; V* | U* | dx.dyu | iZ|Slope | dx | dy
   fxch st1
   fmul st3             ; U**|V*| dx.dyu | iZ|Slope | dx | dy
   fld st6
   fmul dword [DxU]     ; dy.dxU|U**|V*| dx.dyu | iZ|Slope | dx | dy
   fxch st2
   fmul st4             ; V**|U**|dydxU| dx.dyu | iZ|Slope | dx | dy
   fxch st1
   faddp st3            ; V**|dydxU| U* | iZ|Slope | dx | dy
   fld st5
   fmul dword [DyV]     ; dx.dyv|V**|dydxU| U* | iZ|Slope | dx | dy
   fxch st2             ; dy.dyu|V**|dx.dyV| U* | iZ|Slope | dx | dy
   faddp st3            ; V**|dx.dyV| U* | iZ|Slope | dx | dy
   fld st6
   fmul dword [DxV]     ; dy.dxV|V**|dx.dyV| U* | iZ|Slope | dx | dy
   fxch st1
   faddp st2            ; dy.dxV|V**| U* | iZ|Slope | dx | dy
   fxch st2             ; U*|V**| dy.dxV | iZ|Slope | dx | dy
   fstp dword [8+Buf]   
   faddp st1            ; V**| iZ|Slope | dx | dy
   fxch st3             ; dx| iZ|Slope | V** | dy
   fld dword [DyiZ]     ; dyw|dx| iZ|Slope | V** | dy
   fmul st1,st0
   fstp dword [Buf+32]  ; dx.dyw| iZ|Slope | V** | dy
   fxch st4
   fmul dword [DxiZ]    ; dy.dxw|iZ|Slope | V**|dx.dyw
   fxch st3             ; V*|iZ|Slope | dy.dxw|dx.dyw
   fstp dword [20+Buf]  ; iZ|Slope | dy.dxw|dx.dyw
   faddp st3            ; Slope | dy.dxw|iZ*
   fmul dword [DyiZ]    ; *| dy.dxw|iZ*
   fxch st1
   faddp st2            ; Slope.DyiZ|iZ*
   fadd dword [DxiZ]    ; diZ | iZ*
   fstp dword [28+Buf]  ; iZ

      ; Buf+4 : _dU_ / _U_ / incU
      ; Buf+16: _dV_ / _V_ / incV
      ; Buf+28: _dW_ / incW

   mov edx, [Edges_Xe+edi*4]
   fld dword [20+Buf]   ; _V_ | _iZ_
   fld dword [8+Buf]    ; _U_ | _V_ | _iZ_
   test eax,eax
   jge .Loop2
   neg eax
   neg edx
   xor dword [Buf+12],0x80000000
   xor dword [Buf+24],0x80000000
   xor dword [Buf+32],0x80000000
.Loop2:
   fst dword [Scan_UiZ+esi*4]
   fadd dword [4+Buf]
   fxch st1               ; _V_ | _U_* | _iZ_
   fst dword [Scan_ViZ+esi*4]
   fadd dword [16+Buf]  ; _V_*|_U_*| _iZ_
   fxch st1
   fxch st2               ; _iZ_|_V_| _U_
   fst dword [Scan_iZ+esi*4]
   fadd dword [28+Buf]
   fxch st2             ; U|V|iZ
   inc esi
   add edx,eax
   jnc .No_Carry
   fadd dword [12+Buf]
   fxch st1             ; V|U|iZ
   fadd dword [24+Buf]  ; V|U|iZ
   fxch st1
   fxch st2
   fadd dword [32+Buf] ; iZ|U|V
   fxch st2
.No_Carry:
   dec ecx
   jge .Loop2

   fcompp
   pop edi
   fstp st0
   dec edi
   jge near .Loop
   pop esi
   pop edi
   ret

%endif   ; SKIP_UVc

;//////////////////////////////////////////////////////////////////////////

%ifndef SKIP_Grd

Rasterize_Grd:

   Enter

   mov edi,[Nb_Right_Edges]
   dec edi
   jl near End_R_GRD

align4
Loop_R_GRD:   ; edi = i => j
   mov [Buf],edi
   movzx edi,word [Right_Edges+edi*2]
   mov ecx,[Edges_yf+edi*4]   ;ecx = yf
   mov esi,[Edges_yo+edi*4]   ;esi = y

   fld dword [Edges_ey+edi*4]   ; dy
   fild dword [Scan_Pt2+esi*4]
   fsub dword [Edges_x+edi*4]   ; dx | dy

   mov eax,[Edges_dXe+edi*4]   ; eax = dXe
   push eax
   test eax,eax
   jg R_GRD_Not_Neg
   neg dword [esp]
   sar dword [esp],16
   fild dword [esp]
   fchs
   jmp R_GRD_Slope_Ok
R_GRD_Not_Neg
   sar dword [esp],16
   fild dword [esp]      ; Slope | dx | dy
R_GRD_Slope_Ok:
   fld dword [DyS]
   fld dword [DxS]         ; DxS | DyS | Slope | dx | dy
   fld st1            ; DyS | DxS | DyS | Slope | dx | dy
   fmul st3
   fadd st1
   mov ebx,[Out_Vtx+edi*4]
   fistp dword [8+Buf]      ; _dS_

   fld dword [20+ebx]      ; N[0] | DxS | DyS | Slope | dx | dy
   fld st5            ; dy | N[0]*_CST_65536_ | DxS | DyS | Slope | dx | dy
   fmulp st2
   faddp st1         ; dy.DxS+N[0] | DyS | Slope | dx | dy
   fld st3            ; dx | dy.DxS+N[0] | DyS | Slope | dx | dy
   fmul st2
   faddp st1         ; S | DyS | Slope | dx | dy
   fistp dword [4+Buf]      ; _S_
   fistp dword [dyS]      ; Slope | dx | dy

   fcompp
   fstp st0

   pop ebp         ; dXe>>16   eax = full dXe 
   mov ebx,[dyS]
   mov edx,[Edges_Xe+edi*4]
   test eax,eax
   jge R_GRD_dXe_Not_Neg
   neg eax
   neg ebx
   neg edx
R_GRD_dXe_Not_Neg:
   mov ebp,[4+Buf]      ;_S_
   mov edi,[8+Buf]      ;_dS_
Loop2_R_GRD:
   mov [Scan_S+esi*4],ebp
   add dx,ax
   jnc No_Carry_R_GRD
   add ebp,ebx
No_Carry_R_GRD:
   inc esi
   add ebp,edi      ;_S_ += _dS_
   cmp ecx,esi
   jg Loop2_R_GRD

   mov edi,[Buf]
   dec edi
   jge near Loop_R_GRD

End_R_GRD:
   Leave

%endif   ; SKIP_Grd

;//////////////////////////////////////////////////////////////////////////

%ifndef SKIP_iZ

Rasterize_iZ:

   Enter

   mov edi,[Nb_Right_Edges]
   dec edi
   jl near End_R_IZ

align4
Loop_R_IZ:   ; edi = i => j
   mov [Buf],edi
   movzx edi,word [Right_Edges+edi*2]
   mov ecx,[Edges_yf+edi*4]     ;ecx = yf
   mov esi,[Edges_yo+edi*4]     ;esi = y

   fld dword [Edges_ey+edi*4]   ; dy
   fild dword [Scan_Pt2+esi*4]
   fsub dword [Edges_x+edi*4]   ; dx | dy

   mov eax,[Edges_dXe+edi*4]    ; eax = dXe
   push eax
   test eax,eax
   jg R_IZ_Not_Neg
   neg dword [esp]
   sar dword [esp],16
   fild dword [esp]
   fchs
   jmp R_IZ_Slope_Ok
R_IZ_Not_Neg
   sar dword [esp],16
   fild dword [esp]       ; Slope | dx | dy
R_IZ_Slope_Ok:
   fld dword [DyS]
   fld dword [DxS]        ; DxS | DyS | Slope | dx | dy
   fld st1                ; DyS | DxS | DyS | Slope | dx | dy
   fmul st3
   fadd st1
   mov ebx,[Out_Vtx+edi*4]
   fistp dword [8+Buf]    ; _dS_

   fld dword [8+ebx]      ; Inv_Z | DxS | DyS | Slope | dx | dy
   fmul dword [_RCst_+Z_SCALE]
   fld st5                ; dy | iZ | DxS | DyS | Slope | dx | dy
   fmulp st2
   fadd dword [_RCst_+Z_OFF]
   faddp st1              ; dy.DxS+iZ | DyS | Slope | dx | dy
   fld st3                ; dx | dy.DxS+iZ | DyS | Slope | dx | dy
   fmul st2
   faddp st1              ; S | DyS | Slope | dx | dy
   fistp dword [4+Buf]    ; _S_
   fistp dword [dyS]      ; Slope | dx | dy

   fcompp
   fstp st0

   pop ebp                ; dXe>>16   eax = full dXe 
   mov ebx,[dyS]
   mov edx,[Edges_Xe+edi*4]
   test eax,eax
   jge R_IZ_dXe_Not_Neg
   neg eax
   neg ebx
   neg edx
R_IZ_dXe_Not_Neg:
   mov ebp,[4+Buf]      ;_S_
   mov edi,[8+Buf]      ;_dS_
Loop2_R_IZ:
   mov [Scan_S+esi*4],ebp
   add dx,ax
   jnc No_Carry_R_IZ
   add ebp,ebx
No_Carry_R_IZ:
   inc esi
   add ebp,edi          ;_S_ += _dS_
   cmp ecx,esi
   jg Loop2_R_IZ

   mov edi,[Buf]
   dec edi
   jge near Loop_R_IZ

End_R_IZ:
   Leave

%endif   ; SKIP_iZ

;//////////////////////////////////////////////////////////////////////////


Enter_Engine_Asm:
      push eax
      fstcw [FPUCW_Save]
      mov ax,[FPUCW_Save]

      and ax,0xF03F      ; switch on 32-bit prec mode   (0xFFCF)

;   or ax,0x0C00         ; Chop 
      or ax,0x0800       ; Up
;   or ax,0x0000         ; Near
      or ax,0x003f       ; mask exceptions
      mov [FPUCW],ax
      fldcw [FPUCW]
      pop eax
      ret

Exit_Engine_Asm:

      fldcw [FPUCW_Save] ; Restore state
      ret

;//////////////////////////////////////////////////////////////////////////

