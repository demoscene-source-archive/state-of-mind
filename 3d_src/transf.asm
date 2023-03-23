;//////////////////////////////////////////////////////////////////////////

;%include "nasm.h"

;//////////////////////////////////////////////////////////////////////////

[BITS 32]
globl Raw_A_V
globl Raw_tA_V
globl A_V
globl A_Inv_V
globl nA_V
globl nA_Inv_V

; These functions are jamming eax & ecx

;//////////////////////////////////////////////////////////////////////////

TEXT

A_V:			; M = A.V

   mov eax, [esp+4+8]	; V

   fld dword [eax]   ; Vo
   fld dword [eax+4]	; V1|Vo
   fld dword [eax+8]	; V2|V1|Vo

   mov eax, [esp+4+4] ; A

   fld st2			      ; Vo |V2|V1|V0
   fmul dword [eax+0]	; VoAo*|V2|V1|V0
   fld st3              ; Vo|VoAo*|V2|V1|V0
   fmul dword [eax+3*4]	; VoA3*|VoAo*|V2|V1|V0
   fxch st4             ; Vo|VoAo*|V2|V1|VoA3*
   fmul dword [eax+6*4] ; VoA6*|VoAo*|V2|V1|VoA3*
   fxch st1             ; VoAo*|VoA6*|V2|V1|VoA3*
   fadd dword [eax+18*4]; Mo*|VoA6*|V2|V1|VoA3*
   fxch st4             ; VoA3*|VoA6*|V2|V1|Mo*
   fadd dword [eax+19*4]; M1*|VoA6*|V2|V1|Mo*
   fxch st1             ; VoA6*|M1*|V2|V1|Mo*
   fadd dword [eax+20*4]; M2*|M1*|V2|V1|Mo*

   fld st3              ; V1|M2*|M1*|V2|V1|Mo*
   fmul dword [eax+1*4]	; A1V1*|M2*|M1*|V2|V1|Mo*
   fld st4              ; V1|A1V1*|M2*|M1*|V2|V1|Mo*
   fmul dword [eax+4*4]	; A4V1*|A1V1*|M2*|M1*|V2|V1|Mo*
   fxch st5             ; V1|A1V1*|M2*|M1*|V2|A4V1*|Mo*
   fmul dword [eax+7*4]	; A7V1*|A1V1*|M2*|M1*|V2|A4V1*|Mo*
   fxch st1             ; A1V1*|A7V1*|M2*|M1*|V2|A4V1*|Mo*
   faddp st6            ; A7V1*|M2*|M1*|V2|A4V1*|Mo**
   fxch st4             ; A4V1*|M2*|M1*|V2|A7V1*|Mo*
   faddp st2            ; M2*|M1**|V2|A7V1*|Mo*
   faddp st3            ; M1**|V2|M2**|Mo*

   fld st1              ; V2|M1**|V2|M2**|Mo**
   fmul dword [eax+2*4] ; A2V2|M1**|V2|M2**|Mo**
   fld st2              ; V2|A2V2*|M1**|V2|M2**|Mo**
   fmul dword [eax+5*4] ; A5V2*|A2V2*|M1**|V2|M2**|Mo**
   fxch st3             ; V2|A2V2*|M1**|A5V2*|M2**|Mo**
   fmul dword [eax+8*4] ; A8V2*|A2V2*|M1**|A5V2*|M2**|Mo**
   fxch st1             ; A2V2*|A8V2*|M1**|A5V2*|M2**|Mo**
   faddp st5            ; A8V2*|M1**|A5V2*|M2**|Mo***
   fxch st2             ; A5V2*|M1**|A8V2*|M2**|Mo*
   faddp st1            ; M1***|A8V2*|M2**|Mo*
   fxch st1             ; A8V2*|M1***|M2**|Mo*
   faddp st2            ; M1***|M2***|Mo*
	mov ecx, [esp+4]     ; M
	fstp dword [ecx+4]
	fstp dword [ecx+8]
	fstp dword [ecx+0]

	ret

;//////////////////////////////////////////////////////////////////////////

A_Inv_V:			; M = A.V

   mov eax, [esp+4+8]	; V

   fld dword [eax]		; Vo
   fld dword [eax+4]		; V1|Vo
   fld dword [eax+8]		; V2|V1|Vo

   mov eax, [esp+4+4]	; A

   fld st2			      ; Vo |V2|V1|V0
   fmul dword [eax+9*4]	; VoAo*|V2|V1|V0
   fld st3              ; Vo|VoAo*|V2|V1|V0
   fmul dword [eax+12*4]; VoA3*|VoAo*|V2|V1|V0
   fxch st4             ; Vo|VoAo*|V2|V1|VoA3*
   fmul dword [eax+15*4]; VoA6*|VoAo*|V2|V1|VoA3*
   fxch st1             ; VoAo*|VoA6*|V2|V1|VoA3*
   fadd dword [eax+21*4]; Mo*|VoA6*|V2|V1|VoA3*
   fxch st4             ; VoA3*|VoA6*|V2|V1|Mo*
   fadd dword [eax+22*4]; M1*|VoA6*|V2|V1|Mo*
   fxch st1             ; VoA6*|M1*|V2|V1|Mo*
   fadd dword [eax+23*4]; M2*|M1*|V2|V1|Mo*

   fld st3              ; V1|M2*|M1*|V2|V1|Mo*
   fmul dword [eax+10*4]; A1V1*|M2*|M1*|V2|V1|Mo*
   fld st4              ; V1|A1V1*|M2*|M1*|V2|V1|Mo*
   fmul dword [eax+13*4]; A4V1*|A1V1*|M2*|M1*|V2|V1|Mo*
   fxch st5             ; V1|A1V1*|M2*|M1*|V2|A4V1*|Mo*
   fmul dword [eax+16*4]; A7V1*|A1V1*|M2*|M1*|V2|A4V1*|Mo*
   fxch st1             ; A1V1*|A7V1*|M2*|M1*|V2|A4V1*|Mo*
   faddp st6            ; A7V1*|M2*|M1*|V2|A4V1*|Mo**
   fxch st4             ; A4V1*|M2*|M1*|V2|A7V1*|Mo*
   faddp st2            ; M2*|M1**|V2|A7V1*|Mo*
   faddp st3            ; M1**|V2|M2**|Mo*

   fld st1              ; V2|M1**|V2|M2**|Mo**
   fmul dword [eax+11*4]; A2V2|M1**|V2|M2**|Mo**
   fld st2              ; V2|A2V2*|M1**|V2|M2**|Mo**
   fmul dword [eax+14*4]; A5V2*|A2V2*|M1**|V2|M2**|Mo**
   fxch st3             ; V2|A2V2*|M1**|A5V2*|M2**|Mo**
   fmul dword [eax+17*4]; A8V2*|A2V2*|M1**|A5V2*|M2**|Mo**
   fxch st1             ; A2V2*|A8V2*|M1**|A5V2*|M2**|Mo**
   faddp st5            ; A8V2*|M1**|A5V2*|M2**|Mo***
   fxch st2             ; A5V2*|M1**|A8V2*|M2**|Mo*
   faddp st1            ; M1***|A8V2*|M2**|Mo*
   fxch st1             ; A8V2*|M1***|M2**|Mo*
   faddp st2            ; M1***|M2***|Mo*

	mov eax, [esp+4]		; M
	fstp dword [eax+4]
	fstp dword [eax+8]
	fstp dword [eax+0]
	ret

;//////////////////////////////////////////////////////////////////////////


nA_V:			; M = A.V

	mov eax, [esp+4+8]	; V

	fld dword [eax]	; V[0]
	fld dword [eax+4]	; V[1] | V[0]
	fld dword [eax+8]	; V[2] | V[1] | V[0]

	mov eax, [esp+4+4]	; A

   fld st2			      ; Vo |V2|V1|V0
   fmul dword [eax+9*4]	; VoAo*|V2|V1|V0
   fld st3              ; Vo|VoAo*|V2|V1|V0
   fmul dword [eax+10*4]; VoA3*|VoAo*|V2|V1|V0
   fxch st4             ; Vo|Mo*|V2|V1|M1*
   fmul dword [eax+11*4]; M2*|Mo*|V2|V1|M1*

   fld st3              ; V1|M2*|Mo*|V2|V1|M1*
   fmul dword [eax+12*4]; A1V1*|M2*|Mo*|V2|V1|M1*
   fld st4              ; V1|A1V1*|M2*|Mo*|V2|V1|M1*
   fmul dword [eax+13*4]; A4V1*|A1V1*|M2*|Mo*|V2|V1|M1*
   fxch st5             ; V1|A1V1*|M2*|Mo*|V2|A4V1*|M1*
   fmul dword [eax+14*4]; A7V1*|A1V1*|M2*|Mo*|V2|A4V1*|M1*
   fxch st1             ; A1V1*|A7V1*|M2*|Mo*|V2|A4V1*|M1*
   faddp st3            ; A7V1*|M2*|Mo**|V2|A4V1*|M1*
   fxch st4             ; A4V1*|M2*|Mo**|V2|A7V1*|M1*
   faddp st5            ; M2*|Mo**|V2|A7V1*|M1**
   faddp st3            ; Mo**|V2|M2**|M1**

   fld st1              ; V2|Mo**|V2|M2**|M1**
   fmul dword [eax+15*4]; A2V2|Mo**|V2|M2**|M1**
   fld st2              ; V2|A2V2*|Mo**|V2|M2**|M1**
   fmul dword [eax+16*4]; A5V2*|A2V2*|Mo**|V2|M2**|M1**
   fxch st3             ; V2|A2V2*|Mo**|A5V2*|M2**|M1**
   fmul dword [eax+17*4]; A8V2*|A2V2*|Mo**|A5V2*|M2**|M1**
   fxch st1             ; A2V2*|A8V2*|Mo**|A5V2*|M2**|M1**
   faddp st2            ; A8V2*|Mo***|A5V2*|M2**|M1**
   fxch st2             ; A5V2*|Mo***|A8V2*|M2**|M1**
   faddp st4            ; Mo***|A8V2*|M2**|M1***
   fxch st1             ; A8V2*|Mo***|M2**|M1***
   faddp st2            ; Mo***|M2***|M1***

   mov eax, [esp+4]		; M
   fstp dword [eax+0]
   fstp dword [eax+8]
   fstp dword [eax+4]

   ret

;//////////////////////////////////////////////////////////////////////////

nA_Inv_V:			; M = tA.V

   mov eax, [esp+4+8]	; V

   fld dword [eax]		; V[0]
   fld dword [eax+4]	; V[1] | V[0]
   fld dword [eax+8]	; V[2] | V[1] | V[0]

   mov eax, [esp+4+4]	; A

   fld st2			      ; Vo |V2|V1|V0
   fmul dword [eax+0*4]	; VoAo*|V2|V1|V0
   fld st3              ; Vo|VoAo*|V2|V1|V0
   fmul dword [eax+1*4] ; VoA1*|VoAo*|V2|V1|V0
   fxch st4             ; Vo|Mo*|V2|V1|M1*
   fmul dword [eax+2*4] ; M2*|Mo*|V2|V1|M1*

   fld st3              ; V1|M2*|Mo*|V2|V1|M1*
   fmul dword [eax+3*4] ; A3V1*|M2*|Mo*|V2|V1|M1*
   fld st4              ; V1|A1V1*|M2*|Mo*|V2|V1|M1*
   fmul dword [eax+4*4] ; A4V1*|A1V1*|M2*|Mo*|V2|V1|M1*
   fxch st5             ; V1|A1V1*|M2*|Mo*|V2|A4V1*|M1*
   fmul dword [eax+5*4] ; A5V1*|A1V1*|M2*|Mo*|V2|A4V1*|M1*
   fxch st1             ; A1V1*|A7V1*|M2*|Mo*|V2|A4V1*|M1*
   faddp st3            ; A7V1*|M2*|Mo**|V2|A4V1*|M1*
   fxch st4             ; A4V1*|M2*|Mo**|V2|A7V1*|M1*
   faddp st5            ; M2*|Mo**|V2|A7V1*|M1**
   faddp st3            ; Mo**|V2|M2**|M1**

   fld st1              ; V2|Mo**|V2|M2**|M1**
   fmul dword [eax+6*4] ; A2V2|Mo**|V2|M2**|M1**
   fld st2              ; V2|A2V2*|Mo**|V2|M2**|M1**
   fmul dword [eax+7*4] ; A5V2*|A2V2*|Mo**|V2|M2**|M1**
   fxch st3             ; V2|A2V2*|Mo**|A5V2*|M2**|M1**
   fmul dword [eax+8*4] ; A8V2*|A2V2*|Mo**|A5V2*|M2**|M1**
   fxch st1             ; A2V2*|A8V2*|Mo**|A5V2*|M2**|M1**
   faddp st2            ; A8V2*|Mo***|A5V2*|M2**|M1**
   fxch st2             ; A5V2*|Mo***|A8V2*|M2**|M1**
   faddp st4            ; Mo***|A8V2*|M2**|M1***
   fxch st1             ; A8V2*|Mo***|M2**|M1***
   faddp st2            ; Mo***|M2***|M1***

   mov eax, [esp+4]		; M
   fstp dword [eax+0]
   fstp dword [eax+8]
   fstp dword [eax+4]

   ret

;//////////////////////////////////////////////////////////////////////////

Raw_A_V:			; M = A.V

	mov eax, [esp+4+8]	; V

	fld dword [eax]		; V[0]
	fld dword [eax+4]	; V[1] | V[0]
	fld dword [eax+8]	; V[2] | V[1] | V[0]

	mov eax, [esp+4+4]	; A

	fld st2			      ; Vo |V2|V1|V0
	fmul dword [eax+0*4]	; VoAo*|V2|V1|V0
	fld st3              ; Vo|VoAo*|V2|V1|V0
	fmul dword [eax+3*4] ; VoA3*|VoAo*|V2|V1|V0
	fxch st4             ; Vo|Mo*|V2|V1|M1*
   fmul dword [eax+6*4] ; M2*|Mo*|V2|V1|M1*

   fld st3              ; V1|M2*|Mo*|V2|V1|M1*
   fmul dword [eax+1*4] ; A1V1*|M2*|Mo*|V2|V1|M1*
   fld st4              ; V1|A1V1*|M2*|Mo*|V2|V1|M1*
   fmul dword [eax+4*4] ; A4V1*|A1V1*|M2*|Mo*|V2|V1|M1*
   fxch st5             ; V1|A1V1*|M2*|Mo*|V2|A4V1*|M1*
   fmul dword [eax+7*4] ; A7V1*|A1V1*|M2*|Mo*|V2|A4V1*|M1*
   fxch st1             ; A1V1*|A7V1*|M2*|Mo*|V2|A4V1*|M1*
   faddp st3            ; A7V1*|M2*|Mo**|V2|A4V1*|M1*
   fxch st4             ; A4V1*|M2*|Mo**|V2|A7V1*|M1*
   faddp st5            ; M2*|Mo**|V2|A7V1*|M1**
   faddp st3            ; Mo**|V2|M2**|M1**

   fld st1              ; V2|Mo**|V2|M2**|M1**
   fmul dword [eax+2*4] ; A2V2|Mo**|V2|M2**|M1**
   fld st2              ; V2|A2V2*|Mo**|V2|M2**|M1**
   fmul dword [eax+5*4] ; A5V2*|A2V2*|Mo**|V2|M2**|M1**
   fxch st3             ; V2|A2V2*|Mo**|A5V2*|M2**|M1**
   fmul dword [eax+8*4] ; A8V2*|A2V2*|Mo**|A5V2*|M2**|M1**
   fxch st1             ; A2V2*|A8V2*|Mo**|A5V2*|M2**|M1**
   faddp st2            ; A8V2*|Mo***|A5V2*|M2**|M1**
   fxch st2             ; A5V2*|Mo***|A8V2*|M2**|M1**
   faddp st4            ; Mo***|A8V2*|M2**|M1***
   fxch st1             ; A8V2*|Mo***|M2**|M1***
   faddp st2            ; Mo***|M2***|M1***

	mov eax, [esp+4]		; M
	fstp dword [eax+0]
	fstp dword [eax+8]
	fstp dword [eax+4]

	ret

;//////////////////////////////////////////////////////////////////////////

Raw_tA_V:			; M = A.V

	mov eax, [esp+4+8]	; V

	fld dword [eax]		; V[0]
	fld dword [eax+4]	; V[1] | V[0]
	fld dword [eax+8]	; V[2] | V[1] | V[0]

	mov eax, [esp+4+4]	; A

	fld st2			      ; Vo |V2|V1|V0
	fmul dword [eax+0*4]	; VoAo*|V2|V1|V0
	fld st3              ; Vo|VoAo*|V2|V1|V0
	fmul dword [eax+1*4] ; VoA3*|VoAo*|V2|V1|V0
	fxch st4             ; Vo|Mo*|V2|V1|M1*
   fmul dword [eax+2*4] ; M2*|Mo*|V2|V1|M1*

   fld st3              ; V1|M2*|Mo*|V2|V1|M1*
   fmul dword [eax+3*4] ; A1V1*|M2*|Mo*|V2|V1|M1*
   fld st4              ; V1|A1V1*|M2*|Mo*|V2|V1|M1*
   fmul dword [eax+4*4] ; A4V1*|A1V1*|M2*|Mo*|V2|V1|M1*
   fxch st5             ; V1|A1V1*|M2*|Mo*|V2|A4V1*|M1*
   fmul dword [eax+5*4] ; A7V1*|A1V1*|M2*|Mo*|V2|A4V1*|M1*
   fxch st1             ; A1V1*|A7V1*|M2*|Mo*|V2|A4V1*|M1*
   faddp st3            ; A7V1*|M2*|Mo**|V2|A4V1*|M1*
   fxch st4             ; A4V1*|M2*|Mo**|V2|A7V1*|M1*
   faddp st5            ; M2*|Mo**|V2|A7V1*|M1**
   faddp st3            ; Mo**|V2|M2**|M1**

   fld st1              ; V2|Mo**|V2|M2**|M1**
   fmul dword [eax+6*4] ; A2V2|Mo**|V2|M2**|M1**
   fld st2              ; V2|A2V2*|Mo**|V2|M2**|M1**
   fmul dword [eax+7*4] ; A5V2*|A2V2*|Mo**|V2|M2**|M1**
   fxch st3             ; V2|A2V2*|Mo**|A5V2*|M2**|M1**
   fmul dword [eax+8*4] ; A8V2*|A2V2*|Mo**|A5V2*|M2**|M1**
   fxch st1             ; A2V2*|A8V2*|Mo**|A5V2*|M2**|M1**
   faddp st2            ; A8V2*|Mo***|A5V2*|M2**|M1**
   fxch st2             ; A5V2*|Mo***|A8V2*|M2**|M1**
   faddp st4            ; Mo***|A8V2*|M2**|M1***
   fxch st1             ; A8V2*|Mo***|M2**|M1***
   faddp st2            ; Mo***|M2***|M1***

	mov eax, [esp+4]		; M
	fstp dword [eax+0]
	fstp dword [eax+8]
	fstp dword [eax+4]

	ret

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////
