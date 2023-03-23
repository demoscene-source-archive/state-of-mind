;//////////////////////////////////////////////////////////////////////

globl Do_Chen_IDCT
globl Do_Chen_DCT

;//////////////////////////////////////////////////////////////////////

%macro XCHG_REV 0   ; b | a   => a-b|a+b
   fld st1
   fadd st1
   fxch st2
   fsubrp st1
%endmacro

%macro XCHG 0   ; b | a   => b-a|b+a
   fld st1
   fadd st1
   fxch st2
   fsubp st1
%endmacro

%macro LD_P 4
   fld dword [esi+%1*4]
   fmul dword [%2]
   fld dword [esi+%3*4]
   fmul dword [%4]         
   faddp st1
%endmacro
%macro LD_M 4
   fld dword [esi+%1*4]
   fmul dword [%2]
   fld dword [esi+%3*4]
   fmul dword [%4]
   fsubp st1
%endmacro
%macro LDD_P 3
   fld dword [esi+%1*4]
   fadd dword [esi+%2*4]
   fmul dword [%3]
%endmacro
%macro LDD_M 3
   fld dword [esi+%1*4]
   fsub dword [esi+%2*4]
   fmul dword [%3]
%endmacro

%macro LDf_P 2
   fld dword [esi+%1*4]
   fadd dword [esi+%2*4]
%endmacro
%macro LDf_M 2
   fld dword [esi+%1*4]
   fsub dword [esi+%2*4]
%endmacro

;//////////////////////////////////////////////////////////////////////

DATA

align4
fc1d4:  dd 0.353515625
fc1d4p: dd 0.70703125
fc1d8:  dd 0.4619140625
fc3d8:  dd 0.19140625
fc1d16: dd 0.490234375
fc3d16: dd 0.416015625
fc5d16: dd 0.27734375
fc7d16: dd 0.09765625


TEXT

align4
Do_Chen_IDCT:      ; +0: FLT Matrix[64]

   Enter
   mov esi,[esp+OFF_STCK]
   sub esp,64*4
   mov eax,8
   mov edi,esp

Loop_IDCT_1:

   LDD_P 0,32,fc1d4         ; a1
   LD_P 16,fc1d8,48,fc3d8   ; a2 | a1
   XCHG_REV      
   LD_P 24,fc3d16,40,fc5d16 ; c0 | a2 | a1
   LD_P 8,fc1d16,56,fc7d16  ; c3 | c0 | a2 | a1
   XCHG
   fxch st3                  ; a1 | c0 | a2 | c3
   XCHG                      ; a1-c0|a1+c0| a2| c3
   fstp dword [edi+56*4]
   fstp dword [edi+0*4]      ; a2 | c3
   LD_M 8,fc7d16,56,fc1d16  ; c0 | a2 | c3
   LD_M 40,fc3d16,24,fc5d16 ; c1 | c0 | a2 | c3
   XCHG
   fxch st1                  ; c0 | c1 | a2 | c3
   fxch st3                  ; c3 | c1 | a2 | c0
   XCHG
   fmul dword [fc1d4p]       ; c3 | c1 | a2 | c0
   fxch st1                  ; c1 | c3 | a2 | c0
   fmul dword [fc1d4p]

   fxch st3                  ; c0 | c3 | a2 | c1
   fld st2                   ; a2 | c0 | c3 | a2 | c1
   fadd st1
   fxch st1                  ; c0 | a2 | c3 | a2 | c1
   fsubp st3                 ; a2 | c3 | a2-c0 | c1
   fstp dword [edi+24*4]     ; c3 | a2-c0 | c1
   fxch st1                  ; x | c3 | c1
   fstp dword [edi+32*4]



   LDD_M 0,32,fc1d4   ; a1 | c3 | c1
   LD_M 16,fc3d8,48,fc1d8
   XCHG_REV            ; a2 | a1 | c3 | c1
   fxch st2            ; c3 | a1 | a2 | c1
   XCHG_REV            ; a1-c3|a1+c3|a2|c1
   fstp dword [edi+48*4]
   fstp dword [edi+8*4]   ; a2 | c1
   XCHG                   ; a2-c1|a2+c1
   fstp dword [edi+40*4]
   fstp dword [edi+16*4] 

   add esi,4
   add edi,4
   dec eax
   jg near Loop_IDCT_1

   mov esi,esp
   mov edi,[esp+64*4 + OFF_STCK ]
   mov eax,8


Loop_IDCT_2:

   LD_M 1,fc7d16,7,fc1d16
   LD_M 5,fc3d16,3,fc5d16
   XCHG                       ; c1 | c0
   LD_P 3,fc3d16,5,fc5d16
   LD_P 1,fc1d16,7,fc7d16
   XCHG                       ; c3 | c2 | c1 | c0
   fxch st1                   ; c2 | c3 | c1 | c0
   fxch st2                   ; c1 | c3 | c2 | c0
   XCHG_REV                   ; c3 | c1 | c2 | c0
   fmul dword [fc1d4p]
   fxch st1
   fmul dword [fc1d4p]        ; c1 | c3 | c2 | c0

   LDD_M 0,4,fc1d4
   LDD_P 0,4,fc1d4         ; a2 | a1 | c1 | c3 | c2 | c0
   LD_P 2,fc1d8,6,fc3d8    ; a3 | a2 | a1 | c1 | c3 | c2 | c0
   XCHG_REV
   fxch st5                ; c2 | a2 | a1 | c1 | c3 | a3 | c0
   XCHG_REV                ; a2-c2|a2+c2| a1 | c1 | c3 | a3 | c0
   fstp dword [edi+7*4] 
   fstp dword [edi+0*4]    ; a1 | c1 | c3 | a3 | c0
   fxch st4                ; c0 | c1 | c3 | a3 | a1
   fld st3                 ; a3 | c0 | c1 | c3 | a3 | a1
   fadd st1                ; a3+c0 | c0 | c1 | c3 | a3 | a1
   fxch st1                ; c0 | a3+c0 | c1 | c3 | a3 | a1
   fsubp st4               ; a3+c0 | c1 | c3 | a3-c0 | a1
   fstp dword [edi+3*4]    ; c1 | c3 | a3-c0 | a1
   fxch st2               ; x | c3 | c1 | a1
   fstp dword [edi+4*4]  ; c3 | c1 | a1
   fxch st2               ; a1 | c1 | c3

   LD_M 2,fc3d8,6,fc1d8   ; a2 | a1 | c1 | c3
   XCHG_REV
   fxch st3               ; c3 | a1 | c1 | a2
   XCHG_REV               ; a1-c3|a1+c3 | c1 | a2
   fstp dword [edi+6*4]
   fstp dword [edi+1*4]  ; c1 | a2
   fld st1                ; a2 | c1 | a2
   fsub st1               ; a2-c1|c1|a2
   fxch st1
   faddp st2
   fstp dword [edi+5*4]  ; c1 | a2
   fstp dword [edi+2*4]

   add esi,8*4
   add edi,8*4
   dec eax
   jg near Loop_IDCT_2

   add esp,64*4
   Leave

;//////////////////////////////////////////////////////////////////////

align4
Do_Chen_DCT:      ; +0: FLT Matrix[64]

   Enter
   mov esi,[esp+OFF_STCK]
   sub esp,64*4
   mov eax,8
   mov edi,esp
Loop_DCT_1:
   LDf_P 0,56       ; a0
   LDf_P 24,32      ; a3|a0
   XCHG_REV
   LDf_P 8,48
   LDf_P 16,40      
   XCHG_REV              ; a2|a1|a3|a0
   fxch st3              ; a0|a1|a3|a2
   XCHG                  ; a1|a0|a3|a2
   fmul dword [fc1d4]
   fstp dword [edi+32*4]
   fmul dword [fc1d4]
   fstp dword [edi+0*4]   ; a3|a2

   fld dword [fc3d8]       ; x|a3|a2
   fld st2                 ; a2|x|a3|a2
   fmul st1                ; c3a2 | x | a3 | a2
   fxch st1                ; x | c3a2 | a3 | a2
   fmul st2                ; c3a3 | c3a2 | a3 | a2
   fld dword [fc1d8]       ; x | c3a3 | c3a2 | a3 | a2
   fxch st3                ; a3 | c3a3 | c3a2 | x | a2
   fmul st3                ; c1a3 | c3a3 | c3a2 | x | a2
   faddp st2               ; c3a3 | 16 | x | a2
   fxch st3                ; a2 | 16 | x | c3a3
   fmulp st2               ; 16 | c1a2 | c3a3
   fstp dword [edi+16*4]   ; c1a2 | c3a3
   fsubp st1
   fstp dword [edi+48*4]

   LDf_M 8,48           ; a2
   LDf_M 16,40          ; a1|a2
   XCHG_REV             ; a1|a2
   fmul dword [fc1d4p]
   fxch st1
   fmul dword [fc1d4p]   ; a2|a1
   fxch st1              ; a1|a2

   LDf_M 24,32           ; a0|a1|a2
   XCHG                  ; a1|a0|a2
   fxch st2              ; a2|a0|a1
   LDf_M 0,56            ; a3|a2|a0|a1
   XCHG                  ; a2|a3|a0|a1
   fxch st1              ; a3|a2|a0|a1

   fld dword [fc7d16] ; c7|a3|a2|a0|a1
   fld st1
   fmul st1           ; c7a3|c7|a3|a2|a0|a1
   fxch st1           ; c7|c7a3|a3|a2|a0|a1
   fmul st4           ; c7a0|c7a3|a3|a2|a0|a1
   fld dword [fc1d16] ; c1|c7a0|c7a3|a3|a2|a0|a1
   fxch st5           ; a0|c7a0|c7a3|a3|a2|c1|a1
   fmul st5           ; c1a0|c7a0|c7a3|a3|a2|c1|a1
   fsubp st2          ; c7a0| 56 |a3|a2|c1|a1
   fxch st4           ; c1| 56 |a3|a2|c7a0|a1
   fmulp st2          ; 56 |c1a3|a2|c7a0|a1
   fstp dword [edi+56*4]  ; c1a3|a2|c7a0|a1
   faddp st2              ; a2|8|a1
   fxch st1               ; 8|a2|a1
   fstp dword [edi+8*4]   ; a2|a1

   fld dword [fc3d16]   ; c3 | a2 | a1
   fld st1              ; a2 | c3 | a2 | a1
   fmul st1             ; c3a2| c3 | a2 | a1
   fxch st1             ; c3 | c3a2 | a2 | a1
   fmul st3             ; c3a1 | c3a2 | a2 | a1
   fld dword [fc5d16]   ; c5 | c3a1 | c3a2 | a2 | a1
   fxch st3             ; a2 | c3a1 | c3a2 | c5 | a1
   fmul st3             ; c5a2 | c3a1 | c3a2 | c5 | a1
   faddp st1            ; 40  | c3a2 | c5 | a1
   fxch st3             ; a1  | c3a2 | c5 | 40
   fmulp st2            ; c3a2 | c5a1 | 40
   fsubrp st1           ; 24 | 40
   fstp dword [edi+24*4]
   fstp dword [edi+40*4]

   add esi,4
   add edi,4
   dec eax
   jg near Loop_DCT_1

   mov esi,esp
   mov edi,[esp+64*4 + OFF_STCK ]
   mov eax,8

Loop_DCT_2:
   LDf_P 1,6
   LDf_P 2,5             ; a2|a1
   XCHG_REV
   fld dword [esi+0*4]
   fld dword [esi+7*4]   ; 7|0 | a2|a1
   XCHG_REV              ; c3|a0 | a2|a1
   fld dword [esi+3*4]
   fld dword [esi+4*4]   ; 4|3|  c3|a0 | a2|a1
   XCHG_REV              ; c0|a3  |  c3|a0 | a2|a1
   fxch st3              ; a0|a3  |  c3|c0 | a2|a1
   XCHG                  ; a3|a0  |  c3|c0 | a2|a1
   fxch st5              ; a1|a0  |  c3|c0 | a2|a3
   XCHG_REV              ; a1|a0  |  c3|c0 | a2|a3
   fmul dword [fc1d4]
   fstp dword [edi+4*4]
   fmul dword [fc1d4]
   fstp dword [edi+0*4]  ; c3|c0 | a2|a3

   fld dword [fc3d8]     ; x | c3|c0 | a2|a3
   fld st3               ; a2| x | c3|c0 | a2|a3
   fmul st1              ; c3d8a2 | x | c3|c0 | a2|a3
   fxch st1              ; x | c3d8a2 | c3|c0 | a2|a3
   fmul st5              ; c3d8a3 | c3d8a2 | c3|c0 | a2|a3
   fld dword [fc1d8]     ; x | c3d8a3 | c3d8a2 | c3|c0 | a2|a3
   fxch st5              ; a2 | c3d8a3 | c3d8a2 | c3|c0 | x|a3
   fmul st5              ; c1d8a2 | c3d8a3 | c3d8a2 | c3|c0 | x|a3
   fxch st6              ; a3 | c3d8a3 | c3d8a2 | c3|c0 | x|c1d8a2
   fmulp st5             ; c3d8a3 | c3d8a2 | c3|c0 | c1d8a3|c1d8a2
   fsubrp st5            ; c3d8a2 | c3|c0 | c1d8a3| 6
   faddp st3             ; c3|c0 | 2 | 6 
   fxch st2              ; 2 |c0 | c3| 6
   fstp dword [edi+2*4]  ; c0 | c3| 6
   fxch st2              ; 6 | c3| c0
   fstp dword [edi+6*4]  ; c3|c0

   LDf_M 1,6             ; a0| c3|c0
   LDf_M 2,5             ; a1|a0 | c3|c0
   XCHG_REV
   fmul dword [fc1d4p]
   fxch st1
   fmul dword [fc1d4p]   ; a0|a1 | c3|c0
   fxch st3              ; c0|a1 | c3|a0
   XCHG                  ; a1|c0 | c3|a0
   fxch st2              ; c3|c0 | a1|a0
   fxch st1              ; c0|c3 | a1|a0
   fxch st3              ; a0|c3 | a1|c0
   XCHG_REV

   fld dword [fc3d16]    ; x | a0|c3 | a1|c0
   fld st1
   fmul st1              ; c3d16a0 |x|a0|c3 | a1|c0
   fxch st1              ; x| c3d16a0 |a0|c3 | a1|c0
   fmul st4              ; c3d16a1 | c3d16a0 |a0|c3 | a1|c0
   fld dword [fc5d16]    ; x | c3d16a1 | c3d16a0 |a0|c3 | a1|c0
   fxch st3              ; a0 | c3d16a1 | c3d16a0 |x|c3 | a1|c0
   fmul st3              ; c5d16a0 | c3d16a1 | c3d16a0 |x|c3 | a1|c0
   faddp st1             ; 5 | c3d16a0 |x|c3 | a1|c0
   fstp dword [edi+5*4]  ; c3d16a0 |x|c3 | a1|c0
   fxch st1              ; x | c3d16a0 |c3 | a1|c0
   fmulp st3             ; c3d16a0 |c3 | c5d16a1|c0
   fsubrp st2            ; c3 | 3 | c0
   fxch st1
   fstp dword [edi+3*4]  ; c3|c0

   fld dword [fc7d16]    ; x|c3|c0
   fld st1
   fmul st1              ; c7d16c3|x|c3|c0
   fxch st1              ; x|c7d16c3|c3|c0
   fmul st3              ; c7d16c0|c7d16c3|c3|c0
   fld dword [fc1d16]    ; x|c7d16c0|c7d16c3|c3|c0
   fxch st3              ; c3|c7d16c0|c7d16c3|x|c0
   fmul st3              ; c1d16c3|c7d16c0|c7d16c3|x|c0
   faddp st1             ; 1 |c7d16c3|x|c0
   fstp dword [edi+1*4]  ; c7d16c3|x|c0
   fxch st1              ; x | c7d16c3|c0
   fmulp st2             ; c7d16c3|c1d16c0
   fsubrp st1
   fstp dword [edi+7*4]

   add esi,8*4
   add edi,8*4
   dec eax
   jg near Loop_DCT_2

   add esp,64*4
   Leave

;//////////////////////////////////////////////////////////////////////

