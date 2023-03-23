;//////////////////////////////////////////////////////////////////////

globl MP3_Alias_Reduction

;//////////////////////////////////////////////////////////////////////
DATA

align4   
Cs0: dd 0.857492925712
Cs1: dd 0.881741997318
Cs2: dd 0.949628649103
Cs3: dd 0.983314592492
Cs4: dd 0.995517816065
Cs5: dd 0.999160558175
Cs6: dd 0.999899195243
Cs7: dd 0.999993155067

Ca0: dd -0.5144957554270
Ca1: dd -0.4717319685650
Ca2: dd -0.3133774542040
Ca3: dd -0.18191319961
Ca4: dd -0.0945741925262
Ca5: dd -0.0409655828852
Ca6: dd -0.0141985685725
Ca7: dd -0.00369997467

%macro Mult_4x4 0
	fld st1			; 0 | -1 | 0 | | Ca | Cs
	fmul st4	      ; Cs.0 | -1 | 0 | Ca | Cs
   fld st1        ; -1 | Cs.0 |-1 | 0 | Ca | Cs
   fmul st4       ; Ca.-1 | Cs.0 |-1 | 0 | Ca | Cs
   fxch st2       ; -1 | Cs.0 |Ca.-1| 0 | Ca | Cs
   fmulp st5      ; Cs.0 |Ca.-1| 0 | Ca | Cs.-1
   fxch st2       ; 0 | Ca.-1|Cs.0 | Ca | Cs.-1
   fmulp st3      ; Ca.-1|Cs.0 | Ca.0 | Cs.-1
   faddp st1      ; Tmp | Ca.0 | Cs.-1
   fxch st1
   fsubp st2
%endmacro

%macro do_aa 1
	fld dword [eax+%1*4]
	fld dword [eax-(1+%1)*4] ; -1 | 0 | Ca | Cs
   Mult_4x4
	fstp dword [eax+%1*4]
	fstp dword [eax-(1+%1)*4]
%endmacro

%macro do_aa2 3
	fld dword [eax+%1*4]
	fld dword [eax-(1+%1)*4] ; -1 | 0 | Ca | Cs
   Mult_4x4
   fld dword [%2] ; Cs| Tmp1|Tmp2
   fxch st2       ; Tmp2| Tmp1|Cs
   fld dword [%3] ; Ca|Tmp2|Tmp1|Cs
   fxch st2       ; Tmp1|Tmp2|Ca|Cs
	fstp dword [eax+%1*4]
	fstp dword [eax-(1+%1)*4]
%endmacro

TEXT

align4
MP3_Alias_Reduction: 	; FLT *x

	mov eax,[esp+4]		; x
	push ebx
	mov ebx, 31				; i

align4
.LoopAA:

	fld dword [Cs0]
	fld dword [Ca0]		; Ca0 | Cs
	do_aa2 0,Cs1,Ca1
	do_aa2 1,Cs2,Ca2
	do_aa2 2,Cs3,Ca3
	do_aa2 3,Cs4,Ca4
	do_aa2 4,Cs5,Ca5
	do_aa2 5,Cs6,Ca6
	do_aa2 6,Cs7,Ca7
	do_aa 7

	add eax,18*4
	dec ebx
	jg near .LoopAA

	pop ebx
	ret

;//////////////////////////////////////////////////////////////////////
