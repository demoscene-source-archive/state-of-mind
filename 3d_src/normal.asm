;//////////////////////////////////////////////////////////////////////////

;%include "nasm.h"

[BITS 32]
globl Mesh_Store_Normals2
globl Average_Normals

extrn Mesh_Store_Normals
extrn _CST_1_

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

TEXT
Mesh_Store_Normals2:

	Enter

	mov eax,[esp+OFF_STCK]		; Mesh
	mov ebp,[eax+OFF_NB_POLYS]	; Nb_Polys	OFFSET 

	or dword [eax+OFF_FLAGS],64	; =>OBJ_HAS_NORMALS
	dec ebp
	jl near End

	mov edi,[eax+OFF_POLYS]	; M->Polys	OFFSET 
	mov eax,[eax+OFF_VTX]	; M->Vertex	OFFSET 

Ze_Loop:	; ebx=P->Pt[0] ecx=P-Pt[1] edx=P->Pt[2]

	xor ebx,ebx
	mov bx, word [edi+6]
	lea ebx, [ebx*3]
	xor ecx,ecx
	mov cx, word [edi+8]
	lea ecx, [ecx*3]
	xor edx,edx
	mov dx, word [edi+10]
	lea edx, [edx*3]

	fld dword [eax+ebx*4]	; xo
	fld dword [eax+ecx*4]	; x1 | xo
	inc ebx
	fsub st1		; dx1 | xo
	inc ecx
	fld dword [eax+edx*4]	; x2 | dx1 | xo
	fsubrp st2		; dx1 | dx2
	inc edx

	fld dword [eax+ebx*4]	; yo | dx1 | dx2
	fld dword [eax+ecx*4]	; y1 | yo | dx1 | dx2
 	inc ebx
	fsub st1		; dy1 | yo | dx1 | dx2
	inc ecx
	fld dword [eax+edx*4]   ; y2 | dy1 | yo | dx1 | dx2
	fsubrp st2       	; dy1 | dy2 | dx1 | dx2
	inc edx

	fld dword [eax+ebx*4]	; zo | dy1 | dy2 | dx1 | dx2
	fld dword [eax+ecx*4]	; z1 | zo | dy1 | dy2 | dx1 | dx2
	fsub st1		; dz1 | zo | dy1 | dy2 | dx1 | dx2
	fld dword [eax+edx*4]	; z2 | dz1 | zo | dy1 | dy2 | dx1 | dx2
	fsubrp st2		; dz1 | dz2 | dy1 | dy2 | dx1 | dx2

      ; Nx = dy1*dz2 - dy2*dz1;
      ; Ny = dx2*dz1 - dx1*dz2;
      ; Nz = dx1*dy2 - dx2*dy1;

	fld st3        ; dy2 | dz1 | dz2 | dy1 | dy2 | dx1 | dx2
	fmul st1       ; dy2*dz1 | dz1 | dz2 | dy1 | dy2 | dx1 | dx2
	fld st3     	; dy1 | dy2*dz1 | dz1 | dz2 | dy1 | dy2 | dx1 | dx2
	fmul st3    	; dy1*dz2 | dy2*dz1 | dz1 | dz2 | dy1 | dy2 | dx1 | dx2		
	fsubrp st1     ; Nx | dz1 | dz2 | dy1 | dy2 | dx1 | dx2

	fxch st1       ; dz1 | Nx | dz2 | dy1 | dy2 | dx1 | dx2
	fmul st6       ; dz1*dx2 | Nx | dz2 | dy1 | dy2 | dx1 | dx2
	fxch st2    	; dz2 | Nx | dz1*dx2 | dy1 | dy2 | dx1 | dx2
	fmul st5       ; dz2*dx1 | Nx | dz1*dx2 | dy1 | dy2 | dx1 | dx2
	fsubp st2   	; Nx | Ny | dy1 | dy2 | dx1 | dx2

	fxch st2    	; dy1 | Ny | Nx | dy2 | dx1 | dx2
	fmulp st5   	; Ny | Nx | dy2 | dx1 | dx2*dy1
	fxch st3    	; dx1 | Nx | dy2 | Ny | dx2*dy1
	fmulp st2      ; Nx | dy2*dx1 | Ny | dx2*dy1
	fxch st1       ; dy2*dx1 | Nx | Ny | dx2*dy1
	fsubrp st3     ; Nx | Ny | Nz

	fld st0
	fmul st1	; Nx*Nx | Nx | Ny | Nz
	fld st2
	fmul st3	; Ny*Ny | Nx*Nx | Nx | Ny | Nz
	faddp st1	; Ny*Ny + Nx*Nx | Nx | Ny | Nz
	fld st3
	fmul st4	; Nz*Nz | Ny*Ny + Nx*Nx | Nx | Ny | Nz
	faddp st1	; N^2 | Nx | Ny | Nz

	fst dword [edi+16]
	fsqrt
	mov edx,[edi+16]
	test edx,edx
	jne Non_Zero
	add edi,SIZEOF_POLY
	fcompp 
	mov [edi+20-SIZEOF_POLY],edx
	fcompp 
	mov [edi+24-SIZEOF_POLY],edx
	jmp Finish

Non_Zero:

	fld dword [_CST_1_]	; 1 | N | Nx | Ny | Nz
	add edi,SIZEOF_POLY
	fdivrp st1,st0	      ; 1/N | Nx | Ny | Nz
	fxch st3             ; Nz | Nx | Ny | 1/N
	fmul st3          	; Nz/N | Nx | Ny | 1/N

	fstp dword [edi+24-SIZEOF_POLY]  ; Nx | Ny | 1/N
	fmul st2
	fstp dword [edi+16-SIZEOF_POLY]	; Ny | 1/N
	fmulp st1		; Ny/N
	fstp dword [edi+20-SIZEOF_POLY]

Finish:

	dec ebp
	jge near Ze_Loop
End:
	;popad
	;leave
	Leave


;//////////////////////////////////////////////////////////////////////////

Average_Normals:

	Enter

	mov eax,[esp+OFF_STCK]	; Mesh	
	test dword [eax+OFF_FLAGS],64	; OBJ_HAS_NORMALS?
	jne Normals_Ok
	push eax
	call Mesh_Store_Normals2
	pop eax

Normals_Ok:
	mov ecx,[eax+OFF_NB_VTX] ; Nb_Vertex    OFFSET
	test ecx,ecx
	jle near End2
	lea ecx,[ecx*3]
	push eax
	mov edi,[eax+OFF_NORMALS] ; edi = M->Normals   OFFSET
	xor eax,eax
	sub edi,4
StoDW:
	mov [edi+ecx*4],eax
	loop StoDW
	pop eax
	add edi,4

	mov edx,[eax+OFF_NB_POLYS] ; Nb_Polys
	dec edx
	jl near End2

	mov ebx,[eax+OFF_POLYS]	; P = M->Polys

Loop_Add:
	movzx ecx, word [ebx+6]	; P->Pt[0]
	lea ecx, [ecx*3]
	lea ecx, [ecx*4]
	fld dword [edi+ecx]	; M->Normals[ P->Pt[0] ][_X]
	fadd dword [ebx+16]	; + P->No[0]
	fstp dword [edi+ecx]
	fld dword [edi+4+ecx]	; M->Normals[ P->Pt[0] ][_Y]
	fadd dword [ebx+20]	; + P->No[1]
	fstp dword [edi+4+ecx]
	fld dword [edi+8+ecx]	; M->Normals[ P->Pt[0] ][_Z]
	fadd dword [ebx+24]	; + P->No[2]
	fstp dword [edi+8+ecx]

	movzx ecx, word [ebx+8]	; P->Pt[1]
	lea ecx, [ecx*3]
	lea ecx, [ecx*4]
	fld dword [edi+ecx]	; M->Normals[ P->Pt[1] ][_X]
	fadd dword [ebx+16]	; + P->No[0]
	fstp dword [edi+ecx]
	fld dword [edi+4+ecx]	; M->Normals[ P->Pt[1] ][_Y]
	fadd dword [ebx+20]	; + P->No[1]
	fstp dword [edi+4+ecx]
	fld dword [edi+8+ecx]	; M->Normals[ P->Pt[1] ][_Z]
	fadd dword [ebx+24]	; + P->No[2]
	fstp dword [edi+8+ecx]

	movzx ecx, word [ebx+10]; P->Pt[2]
	lea ecx, [ecx*3]
	lea ecx, [ecx*4]
	fld dword [edi+ecx]	; M->Normals[ P->Pt[2] ][_X]
	fadd dword [ebx+16]	; + P->No[0]
	fstp dword [edi+ecx]
	fld dword [edi+4+ecx]	; M->Normals[ P->Pt[2] ][_Y]
	fadd dword [ebx+20]	; + P->No[1]
	fstp dword [edi+4+ecx]
	fld dword [edi+8+ecx]	; M->Normals[ P->Pt[2] ][_Z]
	fadd dword [ebx+24]	; + P->No[2]
	fstp dword [edi+8+ecx]

	add ebx,SIZEOF_POLY	; sizeof(POLY) = 48?
	dec edx
	jge near Loop_Add

	mov eax,[eax+OFF_NB_VTX] ; Nb_Vertex
	dec eax
	lea ecx,[eax*3]
	lea ecx,[ecx*4]
Loop_Norm:
	fld dword [edi+0+ecx]	; Nx
	fld dword [edi+4+ecx]	; Ny | Nx
	fld dword [edi+8+ecx]	; Nz | Ny | Nx
	fld st0
	fmul st1		; Nz*Nz | Nz | Ny | Nx
	fld st2
	fmul st3		; Ny*Ny | Nz*Nz | Nz | Ny | Nx
	faddp st1		; Ny*Ny + Nz*Nz | Nz | Ny | Nx
	fld st3
	fmul st4
	faddp st1	      	; N^2 | Nz | Ny | Nx

	fst dword [edi+0+ecx]
	fsqrt       		; N | Nz | Ny | Nx
	mov ebx,[edi+0+ecx]
	test ebx,ebx
	jne Non_Zero2
	fcompp 
	mov [edi+4+ecx],ebx
	mov [edi+8+ecx],ebx
	fcompp 
	jmp Finish2

Non_Zero2:
	fld dword [_CST_1_]  ; 1 | N | Nz | Ny | Nx
	fdivrp st1,st0		; 1/N | Nz | Ny | Nx
	fmul st1,st0		; 1/N | Nz/N | Ny | Nx
	fxch st2	      	; Ny | Nz/N | 1/N | Nx
	fmul st2		; Ny/N | Nz/N | 1/N | Nx
	fxch st1		; Nz/N | Ny/N | 1/N | Nx
	fstp dword [edi+8+ecx]	; Ny/N | 1/N | Nx
	fstp dword [edi+4+ecx]	; 1/N | Nx
	fmulp st1		; Nx/N
	fstp dword [edi+0+ecx]	; 
Finish2:
	sub ecx,12
	dec eax
	jge Loop_Norm
	
End2:
	Leave

;//////////////////////////////////////////////////////////////////////////
