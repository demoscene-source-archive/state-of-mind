;//////////////////////////////////////////////////////////////////////

globl ASM_Poly1
globl ASM_Poly2

DATA

;%include "../include/rdtsc.h"

align4   
MP3_b: dd 0.0, 1.997590912, 1.990369453, 1.978353019, 1.961570560, 1.940062506, 1.913880671, 1.883088130, 1.847759065, 1.807978586, 1.763842529, 1.715457220, 1.662939225,   1.606415063, 1.546020907, 1.481902251, 1.414213562, 1.343117910, 1.268786568, 1.191398609, 1.111140466, 1.028205488, 0.942793474, 0.855110187, 0.765366865,   0.673779707, 0.580569355, 0.485960360, 0.390180644, 0.293460949, 0.196034281, 0.098135349

;//////////////////////////////////////////////////////////////////////

%macro M8 1
   fld dword [esp+%1*4]       ; c0
   fadd dword [esp+(1+%1)*4]  ; c1+c0
   fld dword [esp+%1*4]       ; c0
   fsub dword [esp+(1+%1)*4]  ; c0-c1|c1+c0
   fxch st1                   ; c0+c1|c0-c1
%endmacro

%macro M1 2
   fld dword [ecx+%1*(18*4)]        ; d0 = r0
   fsub dword [ecx+(31-%1)*(18*4)]   ;d0-r31
   fld dword [ecx+%1*(18*4)]        ; d0 = r0
   fadd dword [ecx+(31-%1)*(18*4)]   ;d0+r31|d0-r31
   fxch st1
   fmul dword [MP3_b+(1+%1*2)*4]
   fxch st1                         ; d0+r31|(d0-r31)*Mb1
   fstp dword [eax+%2*4]            ; =>d0
   fstp dword [eax+(16+%2)*4]       ;=>d16   r31|d0
%endmacro

%macro M2 3
   fld dword [eax+(%3+%1)*4]    ; d0
   fsub dword [eax+(%3+8+%1)*4] ; d8-d0
   fld dword [eax+(%3+%1)*4]    ; d0
   fadd dword [eax+(%3+8+%1)*4] ; d8+d0|d8-d0
   fxch st1
   fmul dword [MP3_b+%2*4]     ; x|d0+d8
   fxch st1
   fstp dword [esp+%1*4]       ; =>c0
   fstp dword [esp+(8+%1)*4]   ; =>c8
%endmacro

%macro M3 3
   fld dword [esp+%1*4]        ; c0
   fsub dword [esp+(4+%1)*4]   ; c4-c0
   fld dword [esp+%1*4]        ; c0
   fadd dword [esp+(4+%1)*4]   ; c4-c0
   fxch st1
   fmul dword [MP3_b+%2*4]     ; x|c0+c4
   fxch st1
   fstp dword [eax+(%3+%1)*4]   ; =>d0
   fstp dword [eax+(%3+4+%1)*4] ; =>d4
%endmacro

%macro M4 2
   fld dword [eax+(%2+%1)*4]    ; d0       |b24|b8
   fsub dword [eax+(%2+2+%1)*4] ; d0-d2    | b24|b8
   fld dword [eax+(%2+%1)*4]    ; d0|d0-d2 |b24|b8
   fadd dword [eax+(%2+2+%1)*4] ; d0+d2|d0-d2| b24|b8
   fxch st1
   fmul st3                   ; x|d0+d2 | b24|b8
   fxch st1                   ; d0+d2|x | b24|b8
   fstp dword [esp+%1*4]      ; =>c0   
   fstp dword [esp+(2+%1)*4]  ; =>c2

   fld dword [eax+(%2+1+%1)*4]  ; c0|b24|b8
   fsub dword [eax+(%2+3+%1)*4] ; c0-c1|b24|b8
   fld dword [eax+(%2+1+%1)*4]  ; c0|b24|b8
   fadd dword [eax+(%2+3+%1)*4] ; c0+c1|c0-c1|b24|b8
   fxch st1
   fmul st2                   ; -*
   fxch st1                   ; +|-*
   fstp dword [esp+(1+%1)*4]  ; =>c1
   fstp dword [esp+(3+%1)*4]  ; =>c3
%endmacro

;//////////////////////////////////////////////////////////////////////

TEXT

ASM_Poly1:   ; FLT *d, FLT *res_p

;RDTSC_IN
   mov eax,[esp+4]   ; *d
   mov ecx,[esp+8]   ; *res_p

   M1 0,0
   M1 1,1
   M1 2,3
   M1 3,2
   M1 4,6
   M1 5,7
   M1 6,5
   M1 7,4
   M1 8,12
   M1 9,13
   M1 10,15
   M1 11,14
   M1 12,10
   M1 13,11
   M1 14,9
   M1 15,8

   sub esp,16*4      ; room for FLT c[16]

      ; c[] = [esp]
   M2 0,2,0
   M2 1,6,0
   M2 2,14,0
   M2 3,10,0
   M2 4,30,0
   M2 5,26,0
   M2 6,18,0
   M2 7,22,0

   M3 0,4,0
   M3 1,12,0
   M3 2,28,0
   M3 3,20,0
   M3 8,4,0
   M3 9,12,0
   M3 10,28,0
   M3 11,20,0

   fld dword [MP3_b+8*4]   ; b8
   fld dword [MP3_b+24*4]  ; b24|b8
   M4 0,0
   M4 4,0
   M4 8,0
   M4 12,0
   fcompp

   fld dword [MP3_b+16*4]  ; b16

   M8 0           ; +0 | -0
   fadd st0
   fxch st1       ; -0 | 2*+0 | b16
   fmul st2       ; -0*| 2*+0 | b16
   fxch st1
   fchs           ; -2*+0 |-0|b16

   M8 2                  ; +|-|x|-0|b16
   fxch st2              ; x|-|+|-0|b16
   fstp dword [eax+0*4]  ; =>d0  -|+| -0 | b16
   fmul st3              ; -2*|+2|-0|b16
   fxch st2              ; -0|+2|-2|b16
   fstp dword [eax+1*4]  ; =>d1   [+2|-2|b16]
   fsub st1,st0          ; +2|-2*|b16
   fstp dword [eax+2*4]  ; -2|b16
   fstp dword [eax+3*4]  ; b16

   M8 4
   fxch st1              ; -4|+4|b16
   fmul st2              ; -4*|+4|b16
   M8 6                  ;+6|-6|-4*|+4|b16
   fstp dword [eax+6*4]  ;-6|-4*|+4|b16
   fmul st3              ;-6*|-4*|+4|b16
   fxch st2              ;+4|-4*|-6*|b16
   fadd st1,st0
   fstp dword [eax+4*4]  ;-4*|-6*|b16
   fsub st1,st0          ;-4*|-6**|b16
   fstp dword [eax+5*4]  ;-6**|b16
   fstp dword [eax+7*4]  ;b16   

   M8 8
   M8 10                 ; +10|-10|+8|-8|b16
   fstp dword [eax+10*4] ; -10|+8|-8|b16
   fmul st3              ; -10*|+8|-8|b16
   fxch st2              ; -8|+8|-10*|b16
   fmul st3              ; -8*|+8|-10*|b16
   fxch st1              ; +8|-8*|-10*|b16
   fst dword [eax+8*4]
   fxch st1              ; -8*|+8|-10*|b16
   fst dword [eax+9*4]
   fadd st1              ; d8_9|d8|-10*|b16
   M8 12                 ; +12|-12|d8_9|d8|-10*|b16
   fxch st1              ; -12|+12|d8_9|d8|-10*|b16
   fmul st5              ; -12*|+12|d8_9|d8|-10*|b16
   fxch st4              ; -10*|+12|d8_9|d8|-12*|b16
   fadd st2              ; d11|+12|d8_9|d8|-12*|b16
   fxch st1              ; +12|d11|d8_9|d8|-12*|b16
   fsubr st2,st0         ; +12|d11|12-d8_9|d8|-12*|b16
   fstp dword [eax+12*4] ; d11|12-d8_9|d8|-12*|b16
   fst dword [eax+11*4]  ; d11|12-d8_9|d8|-12*|b16
   fxch st3              ; -12*|12-d8_9|d8|d11|b16
   faddp st1             ; -12*+12-d8_9|d8|d11|b16
   M8 14                 ; +14|-14|d13|d8|d11|b16
   fxch st2              ; d13|-14|+14|d8|d11|b16
   fstp dword [eax+13*4] ; -14|+14|d8|d11|b16
   fmulp st4             ; +14|d8|d11|-14*b16
   fsubrp st1            ; +14-d8|d11|-14*b16
   fxch st1              ; d11|+14-d8|-14*b16
   fsubp st2             ; +14-d8|-14*b16-d11
   fsub dword [eax+10*4]
   fxch st1
   fstp dword [eax+15*4]   ; d11
   fstp dword [eax+14*4]

   add esp,16*4
;RDTSC_OUT
   ret

;//////////////////////////////////////////////////////////////////////

ASM_Poly2:   ; FLT *d

   mov eax,[esp+4]   ; *d

   sub esp,16*4      ; room for FLT c[16]

   M2 0,2,16
   M2 1,6,16
   M2 2,14,16
   M2 3,10,16
   M2 4,30,16
   M2 5,26,16
   M2 6,18,16
   M2 7,22,16

   M3 0,4,16
   M3 1,12,16
   M3 2,28,16
   M3 3,20,16
   M3 8,4,16
   M3 9,12,16
   M3 10,28,16
   M3 11,20,16

   fld dword [MP3_b+8*4]   ; b8
   fld dword [MP3_b+24*4]  ; b24|b8
   M4 0,16
   M4 4,16
   M4 8,16
   M4 12,16
   fcompp

   fld dword [MP3_b+16*4]  ; b16

   M8 0                    ; d16|-0|b16
   fxch st1                ; -0|d16|b16
   fmul st2                ; d17|d16|b16
   fxch st1                ; d16|d17|b16
   fst dword [eax+16*4]    ; d16|d17|b16
   fxch st1                ; d17|d16|b16
   fst dword [eax+17*4]    ; d17|d16|b16

   M8 2                    ; d18|-2|d17|d16|b16
   fxch st1                ; -2|d18|d17|d16|b16
   fmul st4                ; x|d18|d17|d16|b16
   fxch st1                ; d18|x|d17|d16|b16
   fst dword [eax+18*4]    ; d18|-2|d17|d16|b16
   fxch st1                ; -2|d18|d17|d16|b16
   fst dword [eax+19*4]    ; d19|d18|d17|d16|b16

   M8 4                    ; +4|-4|d19|d18|d17|d16|b16
   fadd st5                ; d20|-4|d19|d18|d17|d16|b16
   fxch st1                ; -4|d20|d19|d18|d17|d16|b16
   fmul st6                ; x|d20|d19|d18|d17|d16|b16
   fxch st1                ; d20|x|d19|d18|d17|d16|b16
   fstp dword [eax+20*4]   ; x|d19|d18|d17|d16|b16
   fadd st3                ; d21|d19|d18|d17|d16|b16
   fxch st3                ; d17|d19|d18|d21|d16|b16
   fadd st4
   fxch st3                ; d21|d19|d18|d17|d16|b16
   fstp dword [eax+21*4]   ; d19|d18|d17|d16|b16
   faddp st2               ; d18|d16_17_19|d16|b16
   faddp st2               ; d16_17_19|d16_18|b16

   M8 6                    ; +6|-6|d16_17_19|d16_18|b16
   faddp st3               ; -6|d16_17_19|d22|b16
   fmul st3                  ; -6*|d16_17_19|d22|b16
   faddp st1               ; d23|d22|b16
   fxch st1
   fst dword [eax+22*4]    ; d22|d23|b16
   fxch st1
   fst dword [eax+23*4]    ; d23|d22|b16

   M8 8                    ;d24|-8|d23|d22|b16
   M8 10                   ;+10|-10|d24|-8|d23|d22|b16
   fadd st2                ;d26|-10|d24|-8|d23|d22|b16
   fstp dword [eax+26*4]   ;-10|d24|-8|d23|d22|b16
   fmul st5                ;-10*|d24|-8|d23|d22|b16
   fxch st1                ;d24|-10*|-8|d23|d22|b16
   fst dword [eax+24*4]    ;d24|-10*|-8|d23|d22|b16
   faddp st1               ;-10*+d24|-8|d23|d22|b16
   fxch st1                ;-8|-10*+d24|d23|d22|b16
   fmul st4                ;d25|-10*+d24|d23|d22|b16
   fst dword [eax+25*4]    ;d25|-10*+d24|d23|d22|b16
   faddp st1               ;d27|d23|d22|b16
   fstp dword [eax+27*4]   ;d23|d22|b16

   M8 12                   ;+12|-12|d23|d22|b16
   fsub dword [eax+20*4]   ;d28|-12|d23|d22|b16
   fst dword [eax+28*4]    ;d28|-12|d23|d22|b16
   fxch st1                ;-12|d28|d23|d22|b16
   fmul st4                ;-12*|d28|d23|d22|b16
   faddp st1               ;-12*+d28|d23|d22|b16
   fsub dword [eax+21*4]   ;d29|d23|d22|b16 
   fstp dword [eax+29*4]   ;d23|d22|b16

   M8 14                   ;+14|-14|d23|d22|b16
   fxch st1                ;-14|+14|d23|d22|b16
   fmulp st4               ;+14|d23|d22|-14*
   fsubrp st2              ;d23|d30|-14*
   fsubp st2               ;d30|d31
   fstp dword [eax+30*4]   ;d31
   fstp dword [eax+31*4]

   add esp,16*4
   ret

;//////////////////////////////////////////////////////////////////////
