;//////////////////////////////////////////////////////////////////////////

;%include "nasm.h"

[BITS 32]

globl ASM_Mesh_Intersect
globl ASM_Mesh_Intersect_Backface

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

DATA

align4

M_RAY_EPSILON: dd -0.00001
P_RAY_EPSILON: dd  0.00001

;//////////////////////////////////////////////////////////////////////////
;
;*** if (st0<=A) => jmp toto ****
;
;  fcom [A]
;  fstsw ax
;  and ah,0x45
;  jnz Toto
;
;*** if (st0<=A) break ****
;
;  fcom [A]
;  fstsw ax
;  and ah,0x01
;  jz Skip
;  jmp break
;

align4

TEXT

ASM_Mesh_Intersect:  ; *Msh, *Ray

	Enter

	mov ebx,[esp+OFF_STCK]		; Mesh
	mov esi,[ebx+OFF_POLYS]    ; esi = Cur
	mov ebp,[esp+OFF_STCK+4]	; Ray

   xor ecx,ecx
   mov edx,[ebx+OFF_CST]      ; edx = *Cst

.Loop_I:     ; ebp = Ray; edx = Cst; esi = Cur   ; ebx = Msh

   fld dword [esi+POLY_NO]    ; Nox
   fld dword [ebp+RAY_DIR]
   fmul st1                   ; Nox.Dx | Nox
   fld dword [esi+POLY_NO+4]  ; Noy |  Nox.Dx | Nox
   fld dword [ebp+RAY_DIR+4]
   fmul st1                   ; Noy.Dy | Noy |  Nox.Dx | Nox
   faddp st2                  ; Noy |  NxDx+NyDy | Nox
   fld dword [esi+POLY_NO+8]
   fld dword [ebp+RAY_DIR+8]
   fmul st1                   ; Noz.Dz | Noz | Noy |  NxDx+NyDy | Nox
   faddp st3
   fxch st2                   ; Dot | Noy | Noz | Nox

   fcom dword [M_RAY_EPSILON] ; Dot<-Ray_EPSILON?
   fstsw ax
   and ah,0x01
   jnz .Cont_I      ; st0 < -RAY_EPSILON => .Cont_I

.End_I:
	fcompp
.End_II:
	fcompp
	jmp .End
	
.Cont_I:

   fxch st3                   ; Nox | Noy | Noz | Dot
   fmul dword [ebp+RAY_ORIG]
   fxch st1                   ; Noy | Nx.Ox | Noz | Dot
   fmul dword [ebp+RAY_ORIG+4]
   faddp st1                  ; NxOx+NyOy | Noz | Dot
   fxch st1                   ; Noz | NxOx+NyOy | Dot
   fmul dword [ebp+RAY_ORIG+8]; NzOz | NxOx+NyOy | Dot
   fld dword [esi+POLY_NDC]   ; NDC | NzOz| Nxy | Dot
   fxch st1
   faddp st2                  ; NDC | N.D | Dot
   fsubrp st1                 ; Z | Dot

   ftst                       ; Z | Dot
   fstsw ax
	test ah,0x01
	je .End_II    ; Z>0 => .End_II

   fld dword [ebp+RAY_Z]      ; z | Z | Dot
   fmul st2                   ; z' | Z | Dot
   fcomp st1
   fstsw ax
	and ah,0x45
	jz .End_II      ; Z>last Z => .End_II

   fld dword [ebp+RAY_ZMIN]      ; z | Z | Dot
   fmul st2                   ; z' | Z | Dot
   fcomp st1
   fstsw ax
	and ah,0x01
	jnz .End_II      ; Z<ZMin => .End_II

   fdivrp st1                   ; Z/Dot
   fld dword [ebp+RAY_DIR]
   fmul st1          ; Ix | Z
   fadd dword [ebp+RAY_ORIG]
   fld dword [ebp+RAY_DIR+4]
   fmul st2          ; Iy | Ix | Z 
   fadd dword [ebp+RAY_ORIG+4]
   fld dword [ebp+RAY_DIR+8]
   fmul st3          ; Iz | Iy | Ix | Z 
   fadd dword [ebp+RAY_ORIG+8]

   fld dword [edx+CST_POP1]      ; cx | Iz | Iy | Ix | Z 
   fmul st3
   fld dword [edx+CST_POP1+4]    ; cy | UI | Iz | Iy | Ix | Z 
   fmul st3
   faddp st1                     ; UI | Iz | Iy | Ix | Z 
   fld dword [edx+CST_POP1+8]    ; cz | UI | Iz | Iy | Ix | Z 
   fmul st2
   faddp st1                     ; UI | Iz | Iy | Ix | Z 
   fxch st3                      ; Ix | Iz | Iy | UI | Z 

   fmul dword [edx+CST_POP2]     ; VI | Iz | Iy | UI | Z 
   fxch st2                      ; Iy | Iz | VI | UI | Z 
   fmul dword [edx+CST_POP2+4]   ; Cy | Iz | VI | UI | Z 
   faddp st2                     ; Iz | VI | UI | Z 
   fmul dword [edx+CST_POP2+8]   ; Cz | VI | UI | Z 
   faddp st1                     ; VI | UI | Z 

   fld dword [edx+CST_V2]        ; a | VI | UI | Z 
   fmul st2                      ; a | VI | UI | Z 
   fld dword [edx+CST_UV]        ; b | a | VI | UI | Z 
   fmul st2                      ; b | a | VI | UI | Z 
   fsubp st1                     ; x | VI | UI | Z 
   fadd dword [edx+CST_X0]       ; x | VI | UI | Z 

   ftst
   fstsw ax
	test ah,0x01
	jne near .End_I          ; x<0 => .End_I

   fld dword [edx+CST_UV]        ; a | x | VI | UI | Z 
   fmulp st3                     ; x | VI | a | Z 
   fld dword [edx+CST_U2]        ; b | x | VI | a | Z 
   fmulp st2                     ; x | b | a | Z 
   fxch st2                      ; a | b | x | Z 
   fsubp st1                     ; y | x | Z 
   fadd dword [edx+CST_Y0]       ; y | x | Z 

   ftst
   fstsw ax
	test ah,0x01
	jne .End_III          ; y<0 ?

   fld st0           ; y | y | x | Z 
   fadd st2          ; w | y | x | Z 

   fld1
   fcompp             ; y | x | Z 
   fstsw ax
	and ah,0x45
   jnz .End_III      ; x+y<1 ?

.Cont_IIII:
   fstp dword [ebp+RAY_Y]
   fstp dword [ebp+RAY_X]
   fstp dword [ebp+RAY_Z]
   mov [ebp+RAY_ID],ecx
   jmp .End

.End_III:	
	fcompp
	fstp st0

.End:
   add esi, SIZEOF_POLY
   inc ecx
   add edx, SIZEOF_CST
   cmp ecx,[ebx+OFF_NB_POLYS]       ; Nb_Polys
   jl near .Loop_I

   Leave

;//////////////////////////////////////////////////////////////////////////

ASM_Mesh_Intersect_Backface:  ; *Msh, *Ray

	Enter

	mov ebx,[esp+OFF_STCK]		; Mesh
	mov esi,[ebx+OFF_POLYS]    ; esi = Cur
	mov ebp,[esp+OFF_STCK+4]	; Ray

   xor ecx,ecx
   mov edx,[ebx+OFF_CST]      ; edx = *Cst
   sub esp,4

.Loop_I:     ; ebp = Ray; edx = Cst; esi = Cur   ; ebx = Msh

   fld dword [esi+POLY_NO]    ; Nox
   fld dword [ebp+RAY_DIR]
   fmul st1                   ; Nox.Dx | Nox
   fld dword [esi+POLY_NO+4]  ; Noy |  Nox.Dx | Nox
   fld dword [ebp+RAY_DIR+4]
   fmul st1                   ; Noy.Dy | Noy |  Nox.Dx | Nox
   faddp st2                  ; Noy |  NxDx+NyDy | Nox
   fld dword [esi+POLY_NO+8]
   fld dword [ebp+RAY_DIR+8]
   fmul st1                   ; Noz.Dz | Noz | Noy |  NxDx+NyDy | Nox
   faddp st3
   fxch st2                   ; Dot | Noy | Noz | Nox

   fst dword [esp]
   mov eax, [esp]
   and eax,0x7fffffff
   cmp eax,dword [P_RAY_EPSILON]
   jge .Cont_I

.End_I:
	fcompp
.End_II:
	fcompp
	jmp .End
	
.Cont_I:

   fxch st3                   ; Nox | Noy | Noz | Dot
   fmul dword [ebp+RAY_ORIG]
   fxch st1                   ; Noy | Nx.Ox | Noz | Dot
   fmul dword [ebp+RAY_ORIG+4]
   faddp st1                  ; NxOx+NyOy | Noz | Dot
   fxch st1                   ; Noz | NxOx+NyOy | Dot
   fmul dword [ebp+RAY_ORIG+8]; NzOz | NxOx+NyOy | Dot
   fld dword [esi+POLY_NDC]   ; NDC | NzOz| Nxy | Dot
   fxch st1
   faddp st2                  ; NDC | N.D | Dot

   test dword [esp],0x80000000
   jnz .Dot_M     ; Dot_P<0

      ; Dot is >0. Reverse signs of comparisons

   fchs
   fsubrp st1                 ; Z | Dot

   fld dword [ebp+RAY_Z]      ; z | Z | Dot
   fmul st2                   ; z' | Z | Dot
   fcomp st1
   fstsw ax
	and ah,0x01
	jnz .End_II      ; Z>last Z => .End_II

   fld dword [ebp+RAY_ZMIN]      ; z | Z | Dot
   fmul st2                   ; z' | Z | Dot
   fcomp st1
   fstsw ax
	and ah,0x45
	jz .End_II      ; Z<ZMin => .End_II
   jmp .Dot_C

.Dot_M:
   fsubrp st1                 ; Z | Dot

   fld dword [ebp+RAY_Z]      ; z | Z | Dot
   fmul st2                   ; z' | Z | Dot
   fcomp st1
   fstsw ax
	and ah,0x45
	jz .End_II      ; Z>last Z => .End_II

   fld dword [ebp+RAY_ZMIN]      ; z | Z | Dot
   fmul st2                   ; z' | Z | Dot
   fcomp st1
   fstsw ax
	and ah,0x01
	jnz .End_II      ; Z<ZMin => .End_II

.Dot_C:
   fdivrp st1                   ; Z/Dot
   fld dword [ebp+RAY_DIR]
   fmul st1          ; Ix | Z
   fadd dword [ebp+RAY_ORIG]
   fld dword [ebp+RAY_DIR+4]
   fmul st2          ; Iy | Ix | Z 
   fadd dword [ebp+RAY_ORIG+4]
   fld dword [ebp+RAY_DIR+8]
   fmul st3          ; Iz | Iy | Ix | Z 
   fadd dword [ebp+RAY_ORIG+8]

   fld dword [edx+CST_POP1]      ; cx | Iz | Iy | Ix | Z 
   fmul st3
   fld dword [edx+CST_POP1+4]    ; cy | UI | Iz | Iy | Ix | Z 
   fmul st3
   faddp st1                     ; UI | Iz | Iy | Ix | Z 
   fld dword [edx+CST_POP1+8]    ; cz | UI | Iz | Iy | Ix | Z 
   fmul st2
   faddp st1                     ; UI | Iz | Iy | Ix | Z 
   fxch st3                      ; Ix | Iz | Iy | UI | Z 

   fmul dword [edx+CST_POP2]     ; VI | Iz | Iy | UI | Z 
   fxch st2                      ; Iy | Iz | VI | UI | Z 
   fmul dword [edx+CST_POP2+4]   ; Cy | Iz | VI | UI | Z 
   faddp st2                     ; Iz | VI | UI | Z 
   fmul dword [edx+CST_POP2+8]   ; Cz | VI | UI | Z 
   faddp st1                     ; VI | UI | Z 

   fld dword [edx+CST_V2]        ; a | VI | UI | Z 
   fmul st2                      ; a | VI | UI | Z 
   fld dword [edx+CST_UV]        ; b | a | VI | UI | Z 
   fmul st2                      ; b | a | VI | UI | Z 
   fsubp st1                     ; x | VI | UI | Z 
   fadd dword [edx+CST_X0]       ; x | VI | UI | Z 

   ftst
   fstsw ax
	test ah,0x01
	jne near .End_I          ; x<0 => .End_I

   fld dword [edx+CST_UV]        ; a | x | VI | UI | Z 
   fmulp st3                     ; x | VI | a | Z 
   fld dword [edx+CST_U2]        ; b | x | VI | a | Z 
   fmulp st2                     ; x | b | a | Z 
   fxch st2                      ; a | b | x | Z 
   fsubp st1                     ; y | x | Z 
   fadd dword [edx+CST_Y0]       ; y | x | Z 

   ftst
   fstsw ax
	test ah,0x01
	jne .End_III          ; y<0 ?

   fld st0           ; y | y | x | Z 
   fadd st2          ; w | y | x | Z 

   fld1
   fcompp             ; y | x | Z 
   fstsw ax
	and ah,0x45
   jnz .End_III      ; x+y<1 ?

.Cont_IIII:
   fstp dword [ebp+RAY_Y]
   fstp dword [ebp+RAY_X]
   fstp dword [ebp+RAY_Z]
   mov [ebp+RAY_ID],ecx
   jmp .End

.End_III:	
	fcompp
	fstp st0

.End:
   add esi, SIZEOF_POLY
   inc ecx
   add edx, SIZEOF_CST
   cmp ecx,[ebx+OFF_NB_POLYS]       ; Nb_Polys
   jl near .Loop_I

   add esp,4
   Leave

;//////////////////////////////////////////////////////////////////////////

