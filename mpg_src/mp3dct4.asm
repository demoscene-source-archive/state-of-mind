;//////////////////////////////////////////////////////////////////////

globl ASM_dct36_2
globl ASM_Copy_And_Zero

DATA
; %include "../include/rdtsc.h"

align4   
CST00: dd 1.8793852415718
CST01: dd 1.532088886238
CST02: dd 0.34729635533386

CST10: dd -0.34729635533386
CST11: dd -1.8793852415718
CST12: dd  1.532088886238

CST20: dd -1.532088886238
CST21: dd  0.34729635533386
CST22: dd -1.8793852415718

CST30: dd 1.9696155060244
CST31: dd 1.2855752193731
CST32: dd 0.68404028665134

CST40: dd  1.2855752193731
CST41: dd -0.68404028665134
CST42: dd  1.9696155060244

CST50: dd  0.68404028665134
CST51: dd  1.9696155060244
CST52: dd -1.2855752193731

CST60: dd 1.8793852415718
CST61: dd 1.532088886238
CST62: dd 0.34729635533386

CST70: dd -0.34729635533386
CST71: dd -1.8793852415718
CST72: dd  1.532088886238

CST80: dd -1.532088886238
CST81: dd  0.34729635533386
CST82: dd -1.8793852415718

CST3_: dd 1.732050808
CST2_: dd 0.707106781
CST4_: dd -0.541196100
CST5_: dd -1.306562965

%macro M8 1
	fld dword [TMP+%1*4]			; tmp
	fadd dword [TMP_+%1*4]		; e
	fld dword [TMPO+%1*4]		; tmpo
	fadd dword [TMPO_+%1*4]		; o|e
%endmacro
%macro M10 1
	fld dword [TMP+%1*4]			; tmp
	fsub dword [TMP_+%1*4]		; e
	fld dword [TMPO+%1*4]		; tmpo
	fsub dword [TMPO_+%1*4]		; o|e
%endmacro
%macro M9 0
	fld st0		; o|o|e
	fsubr st2	; e-o|o|e
	fxch st2		; e|o|e-o
	faddp st1	; +|-
%endmacro

CST_EO0: dd 0.501909918
CST_EO1: dd 0.517638090
CST_EO2: dd 0.551688959
CST_EO3: dd 0.610387294

CST_EO4: dd 0.871723397
CST_EO5: dd 1.183100792
CST_EO6: dd 1.931851653
CST_EO7: dd 5.736856623

CST_P0: dd -0.250238171
CST_M0: dd -5.731396405
CST_P1: dd -0.25215724
CST_M1: dd -1.915324394
CST_P2: dd -0.2560698785
CST_M2: dd -1.155056579
CST_P3: dd -0.262132281
CST_M3: dd -0.831377381

CST_P4: dd -0.2818454865
CST_M4: dd -0.5414201425
CST_P5: dd -0.2964222615
CST_M5: dd -0.465289749
CST_P6: dd -0.3151181035
CST_M6: dd -0.4106699075
CST_P7: dd -0.339085426
CST_M7: dd -0.370046808

;//////////////////////////////////////////////////////////////////////

%define TMP4  (esp+64)
%define TMP	  (esp+48)
%define TMPO_ (esp+32)
%define TMPO  (esp+16)
%define TMP_  (esp+0)

TEXT

align4
ASM_dct36_2:	; *In, *Tmp;	//, *tmp, *tmp_

	push eax
	push ecx

	mov eax,[esp+12]	;eax = In[]
	mov ecx,[esp+12+4]	;ecx = Tmp[]

	sub esp, 68		; tmp_ = esp, tmpo = esp+16 tmpo_ =32 tmp=48 tmp4=64

	fld dword [eax+0*4]	; In0
	fld dword [eax+12*4]	; In12|In0
	fld st1					; In0|In12|In0
	fadd st0					; 2*In0|In12|In0
	fxch st2             ; In0|In12|2.In0
	fsub st1             ; i0|
	fxch st2             ; 2.In0|In12|i0
	faddp st1			   ; i0p12|i0
   fxch st1

	fld dword [eax+4*4]	; i4|i0|i0p12
	fld dword [eax+8*4]	; i8|i4|i0|i0p12
	fld dword [eax+16*4]	; i16|i8|i4|i0|i0p12
	fld st2					; i4|i16|i8|i4|i0|i0p12
	fsub st1
	fld st4              ; i0|i4'|i16|i8|i4|i0|i0p12
	fadd st0             ; 2*i0|i4'|i16|i8|i4|i0|i0p12
	fxch st1             ; i4'|2*i0|i16|i8|i4|i0|i0p12
	fsub st3					;tmp4|2*i0|i16|i8|i4|i0|i0p12
	fxch st1             ; 2*i0|tmp4|i16|i8|i4|i0|i0p12
	fadd st1					;tmp1|tmp4|i16|i8|i4|i0|i0p12
	fxch st1             ;tmp4|tmp1|i16|i8|i4|i0|i0p12
	fsubp st5            ;tmp1|i16|i8|i4|tmp4|i0p12
	fstp dword [TMP+1*4] ;tmp4|i16|i8|i4|i0|i0p12
	fxch st3					;tmp4|i8|i4|i16|i0p12
	fstp dword [TMP4] 	;i8|i4|16|i0p12

	fld st1					;i4|i8|i4|16|i0p12
	fmul dword [CST00]	;i4*1.87|i8|i4|16|i0p12
	fld st1					;i8|i4*1.87|i8|i4|16|i0p12
	fmul dword [CST01]	;i8*1.53|i4*1.87|i8|i4|i16|i0p12
	fld st4					;i16|i8*1.53|i4*1.87|i8|i4|i16|i0p12
	fmul dword [CST02]   ;i16*0.34|i8*1.53|i4*1.87|i8|i4|i16|i0p12
	fxch st1
	faddp st2            ;i16*0.34|tmp0|i8|i4|i16|i0p12
	fadd st5					;x|tmp0|i8|i4|i16|i0p12
	faddp st1				;tmp0|i8|i4|i16|i0p12
   fld st2
   fxch st1             ;tmp0|i4|i8|i4|i16|i0p12
	fstp dword [TMP+0*4] ;i4|i8|i4|i16|i0p12

	fmul dword [CST10]   ;i4x|i8|i4|i16|i0p12
	fld st1              ;i8|i4x|i8|i4|i16|i0p12
	fmul dword [CST11]   ;i8x|i4x|i8|i4|i16|i0p12
	fld st4              ;i16|i8x|i4x|i8|i4|i16|i0p12
	fmul dword [CST12]   ;i16x|i8x|i4x|i8|i4|i16|i0p12
	fxch st1             ;i8x|i16x|i4x|i8|i4|i16|i0p12
	faddp st2            ;i16x|tmp2|i8|i4|i16|i0p12
	fadd st5
	faddp st1            ;tmp2|i8|i4|i16|i0p12
   fxch st1
   fmul dword [CST21]	;i8*|tmp2|i4|i16|i0p12
   fxch st1
	fstp dword [TMP+2*4]	;i8*|i4|i16|i0p12

	fxch st1					;i4|i8*|i16|i0p12
	fmul dword [CST20]   ;i4*|i8*|i16|i0p12
	fxch st2					;i16|i8*|i4*|i0p12
	fmul dword [CST22]	;i16*|i8*|i4*|i0p12
	fxch st1             ;i8*|i16*|i4*|i0p12
	faddp st3
	faddp st1
	faddp st1				;tmp3
	fstp dword [TMP+3*4]

;	mov ecx,[esp+12]	;ecx = tmp_[] , edx=tmp[]

	fld dword [eax+6*4]		;
	fmul dword [CST3_]		;i6_
	fld dword [eax+2*4]		;i2|i6_
	fld dword [eax+10*4]		;i10|i2|i6_
	fld dword [eax+14*4]		;i14|i10|i2|i6_
	fld st2						;i2|i14|i10|i2|i6_
	fsub st1
	fsub st2
	fmul dword [CST3_]		;tmp_1|i14|i10|i2|i6_
   fld st3
   fmul dword [CST30]		;*|*|i14|i10|i2|i6_
   fxch st1   
   fld st3						;i10|*|i14|i10|i2|i6_
   fmul dword [CST31]		;*|*|*|i14|i10|i2|i6_
   fxch st1
	fstp dword [TMP_+1*4]˜	;*|*|i14|i10|i2|i6_

	fld st2                 ;i14|*|*|i14|i10|i2|i6_
	fmul dword [CST32]
	fxch st1
	faddp st2					;i14|*|i14|i10|i2|i6_
	fadd st5
	fld st4						;i2|*|*|i14|i10|i2|i6_
	fmul dword [CST40]		;i2*|*|*|i14|i10|i2|i6_
	fxch st2
	faddp st1					;*|i14|i10|i2|i6_
	fld st2
	fmul dword [CST41]		;*|*|i14|i10|i2|i6_
	fxch st1
	fstp dword [TMP_+0*4]		;i14|i10|i2|i6_

	faddp st1					;*|i14|i10|i2|i6_
	fld st1						;i14|*|i14|i10|i2|i6_
	fmul dword [CST42]
	fxch st1
	fsub st5
	faddp st1					;*|i14|i10|i2|i6_	
   fxch st1
   fmul dword [CST52]	   ;i14*|*|i10|i2|i6_
   fxch st2
   fmul dword [CST51]      ;i10*|*|i14*|i2|i6_
   fxch st3                ;i2|*|i14*|i10*|i6_
	fmul dword [CST50]	   ;i2*|*|i14*|i10*|i6_
	fxch st1
	fstp dword [TMP_+2*4]   ;*|i2*|i14*|i10*|i6_

	faddp st1				;*|i10*|i6_
	fxch st1
	fsubrp st2				;tmp_3
	faddp st1
	fstp dword [TMP_+3*4]

;	mov ecx,[esp+16]	;ecx = tmpo[], edx=tmp[]

	fld dword [eax+1*4]	; In1
	fld dword [eax+13*4]	; In13|In1
	fld st1					; In1|In13|In1
	fadd st0					; 2*In1|In13|In1
	fxch st2             ; In1|In13|2In1
	fsub st1             ; i0|In13|2*In1
	fxch st1             ; In13|i0|2*In1
	faddp st2				; i0|i0p12

	fld dword [eax+5*4]	; i5|i0|i0p12
	fld dword [eax+9*4]	; i9|i5|i0|i0p12
	fld dword [eax+17*4]	; i17|i9|i5|i0|i0p12
	fld st2					; i5|i17|i9|i5|i0|i0p12
	fsub st1             ; i5-i17|i17|i9|i5|i0|i0p12
	fld st4					;i0|i5-i17|i17|i9|i5|i0|i0p12
	fadd st0					;2*i0|i5-i17|i17|i9|i5|i0|i0p12
	fxch st1             ;i5-i17|2.i0|i17|i9|i5|i0|i0p12
	fsub st3					;tmp4o|2.i0|i17|i9|i5|i0|i0p12
	fxch st1
	fadd st1					;tmpo1|tmp4o|i17|i9|i5|i0|i0p12
   fxch st1             ;tmp4o|tmpo1|i17|i9|i5|i0|i0p12
   fsubp st5				;i17|i9|i5|i0-tmp4o|i0p12
   fstp dword [TMPO+1*4];i17|i9|i5|i0-tmp4o|i0p12
	fxch st3					;tmp4o|i9|i5|i17|i0p12
	fmul dword [CST2_]	;tmp4o|i9|i5|i17|i0p12
	fld dword [TMP4]		;tmp4|tmp4o|i9|i5|i17|i0p12
	fld st1					;tmp4o|tmp4|tmp4o|i9|i5|i17|i0p12
	fadd st1					;*|tmp4|tmp4o|i9|i5|i17|i0p12
	fxch st1             ;tmp4|*|tmp4o|i9|i5|i17|i0p12
	fsubrp st2           ;*|*|i9|i5|i17|i0p12
	fmul dword [CST4_]	;*|tmp4|tmp4o|i9|i5|i17|i0p12
	fstp dword [ecx+4*4] ;tmp4|tmp4o|i9|i5|i17|i0p12		; ECX = Tmp[]
	fmul dword [CST5_]	;*|i9|i5|i17|i0p12
	fld st2					;i5|*|i9|i5|17|i0p12
	fmul dword [CST60]	;i5*1.87|*|i9|i5|17|i0p12
	fld st2					;i9|i5*1.87|*|i9|i5|17|i0p12
	fmul dword [CST61]	;i9*1.53|i5*1.87|*|i9|i5|i17|i0p12
   fxch st2	            ;*|i9*1.53|i5*1.87|i9|i5|i17|i0p12
	fstp dword [ecx+13*4];=>Tmp[13]

	faddp st1				;tmp0|i9|i5|i17|i0p12
	fld st3					;i17|tmp0|i9|i5|i17|i0p12
	fmul dword [CST62]   ;i17*0.34|tmp0|i9|i5|i17|i0p12
	fxch st1
	fadd st5					;*|tmp0|i9|i5|i17|i0p12
	fld st3					;i5|*|tmp0|i9|i5|i17|i0p12
	fmul dword [CST70]   ;i5|*|tmp0|i9|i5|i17|i0p12
	fxch st1             ;*|i5|tmp0|i9|i5|i17|i0p12
	faddp st2				;i5|*|i9|i5|i17|i0p12
	fld st2              ;i9|*|x|i9|i5|17|i0p12
	fmul dword [CST71]
	fxch st2
	fstp dword [TMPO+0*4]; i9x|i5x|i9|i5|i17|i0p12

	faddp st1            ; x|i9|i5|i17|i0p12
	fld st3
	fmul dword [CST72]   ; i17x|x|i9|i5|i17|i0p12
	fxch st1
	fadd st5             ; *|i17x|i9|i5|i17|i0p12
	fxch st2
	fmul dword [CST81]	; i9*|i17x|*|i5|i17|i0p12
	fxch st2
	faddp st1            ; *|i9*|i5|i17|i0p12
	fxch st2             ; i5|i9*|*|i17|i0p12
	fmul dword [CST80]   ; i5|i9*|*|i17|i0p12
	fxch st2
	fstp dword [TMPO+2*4];*|i9*|i5*|i17|i0p12

   faddp st1            ; *|i17|i0p12
   fxch st1
   fmul dword [CST82]	; i17*|*|i0p12
   fxch st1             ; *|i17*|i0p12
   faddp st2
	faddp st1				; tmp3
	fstp dword [TMPO+3*4]

;	mov ecx,[esp+16]	;ecx = tmpo_[]

	fld dword [eax+7*4]		;
	fmul dword [CST3_]		;i6_
	fld dword [eax+3*4]		;i3|i6_
	fld dword [eax+11*4]		;i11|i3|i6_
	fld dword [eax+15*4]		;i15|i11|i3|i6_
	fld st2						;i3|i15|i11|i3|i6_
	fsub st1                ;i3-i15|i15|i11|i3|i6_
	fld st3						;i3|i3-i15|i15|i11|i3|i6_
	fxch st1
	fsub st3                ;i3-i15-i11|i3|i15|i11|i3|i6_
	fxch st1
	fmul dword [CST30]		;i3*|i3-i15-i11|i15|i11|i3|i6_
	fxch st1
	fmul dword [CST3_]		;tmpo_1|i3*|i15|i11|i3|i6_
   fld st3                 ;i11|tmpo_1|i3*|i15|i11|i3|i6_
	fmul dword [CST31]		;*|tmpo_1|i3*|i15|i11|i3|i6_
	fxch st1
	fstp dword [TMPO_+1*4]  ;i11*|i3*|i15|i11|i3|i6_

	faddp st1					;*|i15|i11|i3|i6_
	fld st1						;i15|*|i15|i11|i3|i6_
	fmul dword [CST32]      ;i15*|*|i15|i11|i3|i6_
	fxch st1
	fadd st5                ;*|i15*|i15|i11|i3|i6_
   fld st4                 ;i3|*|i15*|i15|i11|i3|i6_
   fxch st1
	faddp st2					;i3|*|i15|i11|i3|i6_
	fmul dword [CST40]		;x|*|i15|i11|i3|i6_
	fxch st1                ;*|x|i15|i11|i3|i6_
	fld st3                 ;i11|*|x|i15|i11|i3|i6_
	fmul dword [CST41]		;x|*|x|i15|i11|i3|i6_
	fxch st1
	fstp dword [TMPO_+0*4]	;x|x|i15|i11|i3|i6_

	faddp st1					;*|i15|i11|i3|i6_
	fld st1						;i15|*|i15|i11|i3|i6_
	fmul dword [CST42]
	fxch st1
	fsub st5                ;*|*|i15|i11|i3|i6_
	fxch st2                ;i15|*|*|i11|i3|i6_
	fmul dword [CST52]	   ;i15*|*|*|i11|i3|i6_
	fxch st2
	faddp st1					;*|i15*|i11|i3|i6_
	fxch st2                ;i11|i15*|*|i3|i6_
	fmul dword [CST51]      ;i11*|i15*|*|i3|i6_
	fxch st2                ;*|i15*|i11*|i3|i6_
	fstp dword [TMPO_+2*4]  ;i15*|i11*|i3|i6_

   faddp st1            ; *|i3|i6_
   fxch st1             ; i3|*|i6
	fmul dword [CST50]	;i3*|*|i6
	fxch st1
	fsubrp st2				; *|i3*
	faddp st1				;tmp_3
	fstp dword [TMPO_+3*4]

	M8	0		; o'|e
	fmul dword [CST_EO0]	;o|e
	M9
	fmul dword [CST_P0]	;+|-
	fxch st1
	fmul dword [CST_M0]
	fxch st1
	fstp dword [ecx+0*4] ;-
	fstp dword [ecx+17*4]

	M8	1		; o'|e
	fmul dword [CST_EO1]	;o|e
	M9
	fmul dword [CST_P1]	;o'|e'
	fxch st1
	fmul dword [CST_M1]
	fxch st1
	fstp dword [ecx+1*4] ;e'
	fstp dword [ecx+16*4]

	M8	2		; o'|e
	fmul dword [CST_EO2]	;o|e
	M9
	fmul dword [CST_P2]	;o'|e'
	fxch st1
	fmul dword [CST_M2]
	fxch st1
	fstp dword [ecx+2*4] ;e'
	fstp dword [ecx+15*4]

	M8	3		; o'|e
	fmul dword [CST_EO3]	;o|e
	M9
	fmul dword [CST_P3]	;o'|e'
	fxch st1
	fmul dword [CST_M3]
	fxch st1
	fstp dword [ecx+3*4] ;e'
	fstp dword [ecx+14*4]

	M10	3		; o'|e
	fmul dword [CST_EO4]	;o|e
	M9
	fmul dword [CST_P4]	;o'|e'
   fxch st1
	fmul dword [CST_M4]
   fxch st1
	fstp dword [ecx+5*4] ;e'
	fstp dword [ecx+12*4]

	M10	2		; o'|e
	fmul dword [CST_EO5]	;o|e
	M9
	fmul dword [CST_P5]	;o'|e'
	fxch st1
	fmul dword [CST_M5]
	fxch st1
	fstp dword [ecx+6*4] ;e'
	fstp dword [ecx+11*4]

	M10	1		; o'|e
	fmul dword [CST_EO6]	;o|e
	M9
	fmul dword [CST_P6]	;o'|e'
	fxch st1
	fmul dword [CST_M6]
	fxch st1
	fstp dword [ecx+7*4] ;e'
	fstp dword [ecx+10*4]

	M10	0		; o'|e
	fmul dword [CST_EO7]	;o|e
	M9
	fmul dword [CST_P7]	;o'|e'
	fxch st1
	fmul dword [CST_M7]
	fxch st1
	fstp dword [ecx+8*4] ;e'
	fstp dword [ecx+9*4]

	add esp,68
	pop ecx
	pop eax
	ret

;//////////////////////////////////////////////////////////////////////

ASM_Copy_And_Zero:   ; FLT *res_p, FLT *s_p, INT Len
   push esi
   push ebx  

   mov eax, [esp+12+8]
   mov edx, [esp+4+8]

   mov esi, [esp+8+8]
   add edx,eax

   add esi,eax
   shr eax,3            ; Len /= 8

   xor eax,-1
   xor ecx,ecx
   inc eax

.Loop:

   mov ebx,[esi+eax*8]
   mov [esi+eax*8], ecx

   mov [edx+eax*8],ebx
   mov ebx,[esi+eax*8+4]

   mov [esi+eax*8+4], ecx
   mov [edx+eax*8+4],ebx
   inc eax
   jl .Loop

   pop ebx
   pop esi
   ret

;//////////////////////////////////////////////////////////////////////

