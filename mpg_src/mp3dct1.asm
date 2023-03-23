;//////////////////////////////////////////////////////////////////////

globl ASM_dct12
globl ASM_Shift36
globl ASM_dct36

%macro do_add 1
   fld dword[edx+4*%1]
   fadd dword[eax+4*%1]
   fld dword[eax+4*(%1+18)]
   fstp dword[edx+4*%1]
   fstp dword[eax+4*%1]   
%endmacro

%macro do_save 2
   fld dword [esp+4*%1]    ; a
   fsub dword [esp+4*%2]   ; a-b
   fld dword [esp+4*%1]    ; a
   fadd dword [esp+4*%2]   ; b+a
   fxch st1                ; a-b|b+a
   fstp dword [esp+4*%2]   ; b+a
   fstp dword [esp+4*%1]
%endmacro

;//////////////////////////////////////////////////////////////////////

DATA

;%include "../include/rdtsc.h"

align4   
_CST_866_: dd 0.866025403
_CST_050_: dd 0.500000000
_CST_707_: dd 0.707106781
_CST_517_: dd 0.517638090
_CST_193_: dd 1.931851653

_CST_00_: dd  0.107206358
_CST_01_: dd  0.500000000
_CST_02_: dd  2.331951230
_CST_03_: dd -3.039058010
_CST_04_: dd -1.207106780
_CST_05_: dd -0.814313138
_CST_06_: dd -0.624844448
_CST_07_: dd -0.500000000
_CST_08_: dd -0.400099577
_CST_09_: dd -0.307007203
_CST_10_: dd -0.207106780
_CST_11_: dd -0.082262332

TEXT

align4
ASM_dct12:   ; FLT *In, FLT *res_p, FLT *s_p

   mov eax,[esp+4]    ; In
   mov edx,[esp+8]    ; Out = res_p
   sub esp,6*4        ; Tmp[]...
   mov ecx,3

.Loop12p:

   fld dword [eax+0*4]   ; In0
   fld dword [eax+1*4]   ; In1 | In0
   fld dword [eax+2*4]   ; In2 | In1 | In0
   fld dword [eax+3*4]   ; In3 | In2 | In1 | In0
   fld dword [eax+4*4]   ; In4 | In3 | In2 | In1 | In0
   fld dword [eax+5*4]   ; In5 | In4 | In3 | In2 | In1 | In0
   fadd st1              ; In5+ | In4 | In3 | In2 | In1 | In0
   fxch st1              ; In4 | In5+ | In3 | In2 | In1 | In0
   fadd st2              ; In4+ | In5+ | In3 | In2 | In1 | In0
   fxch st2              ; In3 | In5+ | In4+ | In2 | In1 | In0
   fadd st3              ; In3+ | In5+ | In4+ | In2 | In1 | In0
   fxch st3              ; In2 | In5+ | In4+ | In3+ | In1 | In0
   fadd st4              ; In2+ | In5+ | In4+ | In3+ | In1 | In0
   fxch st4              ; In1 | In5+ | In4+ | In3+ | In2+ | In0
   fadd st5              ; In1+ | In5+ | In4+ | In3+ | In2+ | In0
   fxch st1              ; In5+ | In1+ | In4+ | In3+ | In2+ | In0
   fadd st3              ; In5++ | In1+ | In4+ | In3+ | In2+ | In0
   fxch st3              ; In3+ | In1+ | In4+ | In5++ | In2+ | In0
   fadd st1              ; In3++ | In1+ | In4+ | In5++ | In2+ | In0

   fxch st4               ; In2 | In1| In4| In5| In3| In0
   fmul dword [_CST_866_] ; pp1 | In1| In4| In5| In3| In0
   fld st5                ; In0 | pp1 | In1| In4| In5| In3| In0
   fsub st3               ; Tmp1| pp1 | In1| In4| In5| In3| In0
   fxch st3               ; In4 | pp1 | In1| Tmp1| In5| In3| In0
   fmul dword [_CST_050_] ; pp2 | pp1 | In1| Tmp1| In5| In3| In0
   fxch st3               ; Tmp1| pp1 | In1| pp2 | In5| In3| In0
   fstp dword [esp+1*4]   ; pp1 | In1| pp2| In5| In3| In0
   fxch st2               ; pp2 | In1| pp1| In5| In3| In0
   faddp st5              ; In1 | pp1| In5| In3| Save
      ; nop nop nop
   fld st4                ; Save | In1| pp1 | In5| In3| Save
   fadd st2               ; Tmp0 | In1| pp1 | In5| In3| Save
   fxch st5               ; Save | In1| pp1 | In5| In3| Tmp0
   fsubrp st2             ; In1| Tmp2 | In5| In3| Tmp0
   fxch st4               ; Tmp0| Tmp2 | In5| In3| In1
   fstp dword [esp+0*4]   ; Tmp2|In5| In3| In1
   fstp dword [esp+2*4]   ; In5| In3| In1

   fld st2                ; In1 | In5 | In3 | In1 
   fsub st1               ; Tmp4 | In5 | In3 | In1 
   fxch st2               ; In3 | In5 | Tmp4 | In1 
   fmul dword [_CST_866_] ; pp1 | In5 | Tmp4 | In1 

   fxch st1               ; In5 | pp1 | Tmp4 | In1
   fmul dword [_CST_050_] ; pp2 | pp1 | Tmp4 | In1
   fxch st2               ; Tmp4| pp1 | pp2 | In1
   fmul dword [_CST_707_] ; Tmp4|pp1 | pp2 | In1
   fxch st2               ; pp2 |pp1 |Tmp4 |In1
   faddp st3              ; pp1 |Tmp4|Save
   fxch st1               ; Tmp4|pp1|Save
   fstp dword [esp+4*4]   ; pp1 | Save

   fld st1                ; Save | pp1 | Save
   fadd st1               ; tmp5 | pp1 | Save
   fxch st1               ; pp1  | tmp5 | Save
   fsubp st2              ; tmp5 | tmp3
   fmul dword [_CST_517_] ; Tmp5 | tmp3
   fxch st1
   fmul dword [_CST_193_] ; Tmp3 | Tmp5
   fxch st1               ; Tmp5 | Tmp3
   fstp dword [esp+5*4]   ; =>Tmp[5].   Tmp3
   fstp dword [esp+3*4]   ; =>Tmp[3]

   do_save 0,5
   do_save 1,4
   do_save 2,3

   fld dword [esp+0*4]  ; Tmp0
   fld dword [esp+1*4]  ; Tmp1|Tmp0
   fld dword [esp+2*4]  ; Tmp2|Tmp1|Tmp0
   fld dword [esp+3*4]  ; Tmp3|Tmp2|Tmp1|Tmp0
   fld dword [esp+4*4]  ; Tmp4|Tmp3|Tmp2|Tmp1|Tmp0
   fld dword [esp+5*4]  ; Tmp5|Tmp4|Tmp3|Tmp2|Tmp1|Tmp0

   add eax, 6*4
   add edx, 6*4         ; Out2 += 6

   fld st2               ; Tmp3|Tmp5|Tmp4|Tmp3|Tmp2|Tmp1|Tmp0
   fmul dword [_CST_00_] ; out6|Tmp5|Tmp4|Tmp3|Tmp2|Tmp1|Tmp0
   fld st2               ; Tmp4
   fmul dword [_CST_01_] ; out7|out6
   fxch st1              ; out6|out7
   fadd dword [edx+0*4]  ; out6|out7|Tmp5|Tmp4|Tmp3|Tmp2|Tmp1|Tmp0
   fxch st1
   fadd dword [edx+1*4]  ; out7|out6
   fxch st1
   fstp dword [edx+0*4]  ; Tmp5|Tmp4|Tmp3|Tmp2|Tmp1|Tmp0
   fstp dword [edx+1*4]  ; Tmp5|Tmp4|Tmp3|Tmp2|Tmp1|Tmp0

   fld st0               ; Tmp5|Tmp5|Tmp4|Tmp3|Tmp2|Tmp1|Tmp0
   fmul dword [_CST_02_] ; out8|Tmp5|Tmp4|Tmp3|Tmp2|Tmp1|Tmp0
   fxch st1              ; Tmp5|out8
   fmul dword [_CST_03_] ; out9|out8
   fxch st1
   fadd dword [edx+2*4]  ; out8|out9|Tmp4|Tmp3|Tmp2|Tmp1|Tmp0
   fxch st1
   fadd dword [edx+3*4]  ; out9|out8
   fxch st1
   fstp dword [edx+2*4]  ; out8|Tmp4|Tmp3|Tmp2|Tmp1|Tmp0
   fstp dword [edx+3*4]  ; Tmp4|Tmp3|Tmp2|Tmp1|Tmp0

   fmul dword [_CST_04_] ; out10|Tmp3|Tmp2|Tmp1|Tmp0
   fxch st1
   fmul dword [_CST_05_] ; out11|out10
   fxch st1
   fadd dword [edx+4*4]  ; out10|out11|Tmp2|Tmp1|Tmp0
   fxch st1
   fadd dword [edx+5*4]  ; out11
   fxch st1
   fstp dword [edx+4*4]  ; Tmp3|Tmp2|Tmp1|Tmp0
   fstp dword [edx+5*4]  ; Tmp2|Tmp1|Tmp0

   fld st0               ; Tmp2|Tmp2|Tmp1|Tmp0
   fmul dword [_CST_06_] ; out12|Tmp2|Tmp1|Tmp0
   fld st2               ; Tmp1|out12
   fmul dword [_CST_07_] ; out13
   fxch st1   
   fadd dword [edx+6*4]  ; out12|out13
   fxch st1
   fadd dword [edx+7*4]  ; out13|out12
   fxch st1
   fstp dword [edx+6*4]  ; Tmp2|Tmp1|Tmp0
   fstp dword [edx+7*4]  ; Tmp2|Tmp1|Tmp0

   fld st2               ; Tmp0|Tmp2|Tmp1|Tmp0
   fmul dword [_CST_08_] ; out14|Tmp2|Tmp1|Tmp0
   fxch st3
   fmul dword [_CST_09_] ; out15|Tmp2|Tmp1|out14
   fxch st3
   fadd dword [edx+8*4]  ; out14|Tmp2|Tmp1|out15
   fxch st3
   fadd dword [edx+9*4]  ; out15|..|out14
   fxch st3
   fstp dword [edx+8*4]  ; Tmp2|Tmp1|out15
   fxch st1

   fmul dword [_CST_10_] ; out16|Tmp2|out15
   fxch st1
   fmul dword [_CST_11_] ; out17|out16
   fxch st1
   fadd dword [edx+10*4] ; out16|out17
   fxch st1
   fadd dword [edx+11*4] ; out17|out16
   fxch st1
   fstp dword [edx+10*4] ; out16|out17
   fstp dword [edx+11*4] ; out15
   fstp dword [edx+9*4] ; 

   dec ecx
   jg near .Loop12p

   mov eax,[esp+8+6*4]   ; res_p
   mov edx,[esp+12+6*4]  ; s_p
   add esp,6*4

   do_add(0)
   do_add(1)
   do_add(2)
   do_add(3)
   do_add(4)
   do_add(5)
   do_add(6)
   do_add(7)
   do_add(8)
   do_add(9)
   do_add(10)
   do_add(11)
   do_add(12)
   do_add(13)
   do_add(14)
   do_add(16)
   do_add(17)

   ret


;//////////////////////////////////////////////////////////////////////

%macro do_bt 1
   fld dword [ecx+(9+%1)*4]    ; tmp[9]
   fld st0                     ; tmp|tmp
   fmul dword [edx+%1*4]       ; *Win_bt[0]|tmp
   fxch st1                    ; tmp|x
   fmul dword [edx+(17-%1)*4]  ; *Win_bt[17]|x
   fxch st1
   fsubr dword [ebx+%1*4]      ; +s_p[0] | x
   fxch st1
   fadd dword [ebx+(17-%1)*4]  ; + s_p[17] | x
   fxch st1
   fstp dword [eax+%1*4]       ; ->res_p[0]
   fstp dword [eax+(17-%1)*4]  ; ->res_p[17]

   fld dword [ecx+(8-%1)*4]    ; tmp[8]
   fld st0                     ; tmp|tmp
   fmul dword [edx+(18+%1)*4]  ; *Win_bt[18]|tmp
   fxch st1
   fmul dword [edx+(35-%1)*4]  ; *Win_bt[35] | x
   fxch st1
   fstp dword [ebx+%1*4]       ; ->s_p[0]   
   fstp dword [ebx+(17-%1)*4]  ; ->s_p[17]
%endmacro

align4
ASM_Shift36:   ; FLT *res_p, FLT *s_p, FLT *Tmp, FLT *Win_bt

   push ebx
   push ecx
   push edx
   mov eax,[esp+12+4]    ; res_p
   mov ebx,[esp+12+8]    ; s_p
   mov ecx,[esp+12+12]   ; Tmp
   mov edx,[esp+12+16]   ; Win_bt

   do_bt 0
   do_bt 1
   do_bt 2
   do_bt 3
   do_bt 4
   do_bt 5
   do_bt 6
   do_bt 7
   do_bt 8

   pop edx
   pop ecx
   pop ebx
   ret

;//////////////////////////////////////////////////////////////////////

align4
ASM_dct36:   ; FLT *In

   mov eax,[esp+4]

   fld dword [eax+17*4]   ; 17
   fld dword [eax+16*4]   ; 16|17
   fadd st1,st0           ; 16|17'
   fld dword [eax+15*4]   ; 15 | 16 | 17'
   fadd st1,st0           ; 15| 16' | 17'
   fld dword [eax+14*4]   ; 14 | 15 | 16' | 17'
   fadd st1,st0           ; 14 | 15' | 16' | 17'
   fxch st2               ; 16' | 15' | 14 | 17'
   fstp dword [eax+16*4]  ; 15' | 14 | 17'
   fadd st2,st0           ; 15'|14|17''
   fxch st2               ; 17''|14|15'
   fld dword [eax+13*4]   ; 13|17'' |14|15'
   fxch st1               ; 17''|13|14|15'
   fstp dword[eax+17*4]   ; 13|14|15'
   fadd st1,st0           ; 13|14'|15'
   fld dword [eax+12*4]   ; 12 |13|14'| 15'
   fxch st2               ; 14' |13 | 12 | 15'
   fstp dword [eax+14*4]  ; 13|12 | 15'
   fadd st1               ; 13'|12|15'
   fxch st2
   fadd st2               ; 15''|12|13'
   fld dword [eax+11*4]   ; 11 | 15''|12 | 13'
   fxch st1               ; 15'|11|12 | 13'
   fstp dword[eax+15*4]   ; 11|12|13'
   fadd st1,st0           ; 11|12'| 13'
   fxch st1               ; 12'|11| 13'
   fld dword [eax+10*4]   ; 10 |12' | 11 | 13'
   fxch st1
   fstp dword [eax+12*4]  ; 10 |11 | 13'
   fadd st1,st0           ; 10 | 11' | 13'
   fxch st2               ; 13' | 11' | 10
   fadd st1               ; 13''|11'|10
   fxch st2               ; 10|11'|13''
   fld dword [eax+9*4]    ; 9|10|11'|13''|15 | 13 | 17
   fxch st3
   fstp dword [eax+13*4]  ; 10|11'|9
   fadd st2               ; 10'|11'|9
   fxch st2               ; 9|11'|10'
   fld dword [eax+8*4]    ; 8 | 9 | 11'|10'
   fxch st3
   fstp dword [eax+10*4]   ; 9 | 11'| 8
   fadd st2                ; 9' | 11' | 8
   fadd st1,st0            ; 9'|11''|8
   fld dword [eax+7*4]     ; 7 | 9'|11''| 8
   fxch st2                ; 11''| 9'|7| 8
   fstp dword [eax+11*4]   ; 9' | 7 | 8
   fld dword [eax+6*4]     ; 6|9'|7|8
   fxch st2                ; 7|9'|6|8
   fadd st3,st0            ; 7|9'|6|8'
   fxch st3                ; 8'|9'|6|7
   fstp dword [eax+8*4]    ; 9'|6|7
   fxch st2                ; 7|6|9'
   fadd st1                ; 7'|6|9'
   fxch st2                ; 9'|6|7'
   fadd st2                ; 9"|6|7'
   fld dword [eax+5*4]     ; 5|9"|6|7'
   fxch st1                ; 9"|5|6|7'
   fstp dword [eax+9*4]    ; 5|6|7'
   fadd st1,st0            ; 5|6'|7'
   fxch st1                ; 6'|5|7'
   fld dword [eax+4*4]     ; 4|6'|5|7'
   fxch st1
   fstp dword [eax+6*4]    ; 4|5|7'
   fadd st1,st0            ; 4|5'|7'
   fxch st2                ; 7'|5'|4
   fld dword [eax+3*4]     ; 3|7'|5'|4
   fadd st3,st0
   fxch st1                ; 7'|3|5'|4'
   fadd st2                ; 7"|3|5'|4'
   fld dword [eax+2*4]     ; 2|7"|3|5'|4'
   fadd st2,st0            ; 2|7"|3'|5'|4'
   fxch st1                ; 7"|2|3'|5'|4'
   fstp dword [eax+7*4]    ; 2|3'|5'|4'
   fxch st3             
   fstp dword [eax+4*4]    ; 3'|5'|2
   fld dword [eax+1*4]     ; 1|3'|5'|2
   fadd st3,st0            ; 1|3'|5'|2'
   fxch st2                ; 5'|3'|1|2'
   fadd st1                ; 5"|3'|1|2'
   fxch st2                ; 1|3'|5"|2'
   fadd dword [eax+0*4]    ; 1'|3'|5"|2'
   fxch st3
   fstp dword [eax+2*4]    ; 3'|5"|1'
   fadd st2                ; 3"|5"|1'
   fxch st1
   fstp dword [eax+5*4]    ; 3"|1
   fxch st1
   fstp dword [eax+1*4]	   ; 3"
	fstp dword [eax+3*4] 

   ret

;//////////////////////////////////////////////////////////////////////
