;//////////////////////////////////////////////////////////////////////////

;	must be the same as in mcube.h

%define EMIT_MC_POLYS_ASM
%define FILL_SLICE3_ASM
%define EMIT_ALL_MC_ASM

%define GROW_NEIGHBOURS_ASM
%define EMIT_MC_POLYS2_ASM
%define EMIT_MC_POLYS3_ASM
%define DO_V_ASM
%define COMPUTE_MC_NORMAL_ASM

%define DO_V2_ASM
;%define COMPUTE_MC_NORMAL3_ASM

%define BLOB_TABLE_SIZE	256

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

[BITS 32]

DATA

globl MC_Grow_Neighbours
globl Emit_MC_Polys2

%ifdef COMPUTE_MC_NORMAL_ASM
globl Compute_MC_Normal
%else
extrn Compute_MC_Normal
%endif

%ifdef DO_V_ASM
globl Do_V
%else
extrn Do_V
%endif

%ifdef COMPUTE_MC_NORMAL2_ASM
globl Compute_MC_Normal2
%else
extrn Compute_MC_Normal2
%endif

%ifdef DO_V2_ASM
globl Do_V2
%else
extrn Do_V2
%endif

%ifdef EMIT_MC_POLYS3_ASM
Jmp_Tab: dd case_0,case_1,case_2,case_3,case_4,case_5,case_6,case_7,case_8,case_9,case_10,case_11
globl Emit_MC_Polys3
%endif

;//////////////////////////////////////////////////////////////////////////

extrn _Do_V_
extrn _Compute_MC_Normal_
extrn Ngh_Tab
extrn Flags1
extrn Flags2
extrn MC_Mask

extrn X_Edge
extrn Y_Edge
extrn Z_Edge
extrn Top_yEdge
extrn Vtx_6
extrn Vtx_10

extrn Slice_B
extrn Slice_A
extrn Flags_B
extrn Flags_A
extrn Slice
extrn MC_Blob
extrn O_Blob
extrn Edge_Order
extrn Poly_Order
extrn Blob_Table
extrn Blob_Table2

%define Edge_T	esp+16

;//////////////////////////////////////////////////////////////////////////

%define MCX	32
%define MCY	32
%define MCZ	32

extrn MC_SCALE_X
extrn MC_SCALE_Y
extrn MC_SCALE_Z
extrn MC_OFF_X
extrn MC_OFF_Y
extrn MC_OFF_Z

;//////////////////////////////////////////////////////////////////////////

;DATA

align4
_CST1_: dd 0.1			; MAGIK Constant :)

TEXT

%ifdef DO_V2_ASM

align4
Do_V2:		; => Out: V
		; trashes eax & ecx

	fld dword [Slice+8]
	fld dword [Slice+4]
	fld dword [Slice+0]

	mov ecx,[O_Blob]	; ecx=O_Blob
	fld dword [ecx+OFF_K]	; O_Blob->K	OFFSET: 8
	fchs			; -V | X | Y | Z

	fld st1		; X | -V | X | Y | Z
	fmul st0	; X2 | -V | X | Y | Z
	fld st3		; Y | X2 | -V | X | Y | Z
	fmul st0	; Y2 | X2 | -V | X | Y | Z
	fxch st1	; X2 | Y2 | -V | X | Y | Z
	fadd st1	; X2+Y2 | Y2 | -V | X | Y | Z
	fld st5		; Z | Y2+X2 | Y2 | -V | X | Y | Z
	fmul st0	; Z2 | X2+Y2 | Y2 | -V | X | Y | Z
	fadd st1,st0	; Z2 | r2 | Y2 | -V | X | Y | Z
	faddp st2	; r2 | D | -V | X | Y | Z

	fmul dword [ecx+OFF_INV_RAD2];	; r2/R2 | D | -V | X | Y | Z
	fld1
	sub esp,4
	fsubrp st1	; 1-r2 | D | -V | X | Y | Z

	fst dword [esp]
	pop eax
	fxch st3	; X | D | -V | 1-r2 | Y | Z

	test eax, 0x80000000
	jz V2_Go
	fstp st0
	fstp st0
	fstp st1
	jmp V2_End
align4
V2_Go:
	add ecx, OFF_Gn		; ecx = &Blob->G[0]
	mov eax, 0x02

align4
V2_Ok:	; X | D | -V | r2 | Y | Z

	fmul st0,st3	; X*r2 | D | -V | r2 | Y | Z
	fxch st4	; y | D | -V | r2 | x | Z
	fmul st0,st4	; y*x | D | -V | r2 | x | Z
	fxch st5	; z | D | -V | r2 | x | y*x
	fmul st0,st4	; z*x | D | -V | r2 | x | y*x
	fxch st4	; x | D | -V | r2 | z*x | y*x
	fmul st0	; x*x | D | -V | r2 | z*x | y*x
	fsubrp st1	; x*x-D | -V | r2 | z*x | y*x

	fadd dword [ecx] 	; x'| -V | r2 | z*x | y*x 
	fxch st3		; z*x | -V | r2 | x' | y*x
	fadd dword [ecx+8]	; z' | -V | r2 | x' | y*x
	fxch st4		; y*x | -V | r2 | x' | z'
	fadd dword [ecx+4]	; y' | -V | r2 | x' | z'
	fxch st4		; z' | -V | r2 | x' | y'
	fld st0
	fmul st0		; z*z | z' | -V | r2 | x' | y'
	fld st5			; y' | z*z | z' | -V | r2 | x' | y'
	fmul st0		; y*y | z*z | z' | -V | r2 | x' | y'
	faddp st1		; D | z' | -V | r2 | x' | y'

	fxch st1		; z' | D | -V | r2 | x' | y'
	fxch st5		; y' | D | -V | r2 | x' | z'
	fxch st4		; x' | D | -V | r2 | y' | z'

	dec eax
	jge V2_Ok

	fmul st0		; x2 | D | -V | r2 | y' | z'
	fmulp st1		; D*x*x | -V | r2 | y' |  z'
	fadd dword [_CST1_]	; .1+D*x*x | -V | r2 | y' | z'
	fmulp st2		; -V | dV | y' | z'
	faddp st1		; V | y' | z'

V2_End:
	sub esp,4
	fst dword [esp] ; V|y|z
	pop eax
	fstp st2	; y|V
	and eax,0x80000000
	fstp st0	; V
	ret

%endif		; DO_V2_ASM

;//////////////////////////////////////////////////////////////////////////

%ifdef COMPUTE_MC_NORMAL2_ASM

align4
Compute_MC_Normal2:
	; cx = Vtx_ID,	edx = &Vertex[Vtx_ID]
	; edi = MC_Blob
	; ebx = Vtx_ID*12	(trashable)
	; esi = Off		(trashable)
	; don't touch ebp, ecx

	push ecx

	mov ecx, [O_Blob]	; ecx = O_Blob
	add ebx, [edi+OFF_NORMALS]	; ebx = &MC_Blob->Normals[Vtx_ID] OFFSET:
	fldz			; Nx
	fst dword [ebx]
	fst dword [ebx+4]
	fstp dword [ebx+8]

	fld dword [edx+0]	; x
	mov esi, [ecx]		; Nb_Center
	fld dword [edx+4]	; y | x
	dec esi
	fld dword [edx+8]	; z | y | x
	jl near MC_N2_End1

align4
MC_N2_Loop1:		; z | y | x
	lea eax,[esi*3]
	fld dword [ecx+OFF_Gn+eax*4]	; O_Blob->G[n][0]
	fsubr st0,st3			; x-X | z | y | x
	fld dword [ecx+OFF_Gn+eax*4+4]	; O_Blob->G[n][1]
	fsubr st0,st3			; y-Y | x-X | z | y | x
	fld dword [ecx+OFF_Gn+eax*4+8]	; O_Blob->G[n][2]
	fsubr st0,st3			; z-Z | y-Y | x-X | z | y | x
	fld st0
	fmul st0			; Z'^2 | z-Z | y-Y | x-X | z | y | x
	fld st2				; Y' | Z'^2 | z-Z | y-Y | x-X | z | y | x
	fmul st0
	faddp st1			; Y'^2 + Z'^2 | z-Z | y-Y | x-X | z | y | x
	fld st3
	fmul st0
	faddp st1			; N'^2 |  z-Z | y-Y | x-X | z | y | x

	fmul dword [ecx+OFF_INV_RAD2+esi*4]	; *O_Blob->Inv_Radius2[i]
	fistp dword [Blob_Table2]		; !!! Hack! Blob_Table2[0] is trashable :)

	mov eax, [Blob_Table2]
	cmp eax, BLOB_TABLE_SIZE
	jge MC_N2_Clear

	fld dword [Blob_Table2+eax*4]	; Tmp | x-X | y-Y | z-Z | z | y | x
	fmul st3, st0
	fmul st2, st0
	fmulp st1, st0			; Nx | Ny | Nz | z | y | x
	fadd dword [ebx+0]
	fxch st1			; Ny | Nx | Nz | z | y | x
	fadd dword [ebx+4]
	fxch st2			; Nz | Nx | Ny | z | y | x
	fadd dword [ebx+8]		; Nz | Nx | Ny | z | y | x
	fstp dword [ebx+8]
	fstp dword [ebx+0]
	dec esi
	fstp dword [ebx+4]
	jge near MC_N2_Loop1
	jmp MC_N2_Finish
align4
MC_N2_Clear
	fstp st0
	fstp st0
	dec esi
	fstp st0
	jge near MC_N2_Loop1

align4
MC_N2_Finish:
	fstp st0
	fstp st0
	fstp st0

	fld dword [ebx+0]		; Nx
	fld dword [ebx+4]		; Ny | Nx
	fld dword [ebx+8]		; Nz | Ny | Nx
	fld st0
	fmul st0			; Nz^2 | Nz | Ny | Nx
	fxch st2
	fld st0				; Ny | Ny | Nz | Nz^2 | Nx
	fmul st0			; Ny^2 | Ny | Nz | Nz^2 | Nx
	faddp st3			; Ny | Nz | Nz^2+Ny^2 | Nx
	fxch st3			; Nx | Nz | Nz^2+Ny^2 | Ny
	fld st0
	fmul st0			; Nx^2 | Nx | Nz | Nz^2+Ny^2 | Ny
	faddp st3			; Nx | Nz | N^2 | Ny
	fxch st2			; N^2 | Nz | Nx | Ny
	fsqrt
	fld1				; 1 | N | Nz | Nx | Ny
	fdivrp st1,st0			; 1/N | Nz | Nx | Ny
	fxch st3			; Ny | Nz | Nx | 1/N
	fmul st3
	fstp dword [ebx+4]		; Nz | Nx | 1/N
	fmul st2
	fstp dword [ebx+8]		; Nx | 1/N
	fmulp st1
	fstp dword [ebx+0]
	
MC_N2_End1:
	pop ecx
	ret

%endif ; COMPUTE_MC_NORMAL2_ASM

;//////////////////////////////////////////////////////////////////////////

%ifdef DO_V_ASM

align4
Do_V:		; => Out: V
		; trashes eax & ecx

	fld dword [Slice+8]	; Z
	push ebx
	fld dword [Slice+0]	; Y | Z
	fld dword [Slice+4]	; X | Y | Z

	mov ecx,[O_Blob]	; ecx=O_Blob
	fld dword [ecx+OFF_K]	; O_Blob->K	OFFSET: 8
	mov ebx,[ecx]		; Nb_Center
	fchs			; -V | Y | X | Z

align4
V_Loop:
	dec ebx
	jl V_End

	lea eax,[ebx*3]
	fld st1				; Y | V | Y | X | Z
	lea eax,[ecx+eax*4]
	fsub dword [eax+OFF_Gn+4]	; O_Blob->G[n] OFFSET: 12+n*12
	fmul st0			; Y'^2 | V | Y | X | Z
	fld st3				; X | Y'2 | V | Y | X | Z
	fsub dword [eax+OFF_Gn+0]	;
	fmul st0
	faddp st1			; y2+x2 | V | Y | X | Z
	fld st4
	fsub dword [eax+OFF_Gn+8]	; z' | x2+z2 | V | Y | X | Z
	fmul st0			;
	faddp st1			; r2 | V | Y | X | Z
	fmul dword [ecx+ebx*4+OFF_INV_RAD2]	;Inv_Radius2[n] OFFSET : 
	fistp dword [Blob_Table2]	; !!! Hack! Blob_Table2[0] is trashable :)

	mov eax, [Blob_Table2]
	cmp eax, BLOB_TABLE_SIZE
	jge V_Loop
	fadd dword [Blob_Table+eax*4]	; V | Y | X | Z
	jmp V_Loop

V_End:
	pop ebx
	fstp st1	; V|X|Z
	sub esp,4
	fst dword [esp] ; V|X|Z
	pop eax
	fstp st2	; X|V
	and eax,0x80000000
	fstp st0	; V
	ret

%endif		; DO_V_ASM

;//////////////////////////////////////////////////////////////////////////

%ifdef COMPUTE_MC_NORMAL_ASM

align4
Compute_MC_Normal:
	; cx = Vtx_ID,	edx = &Vertex[Vtx_ID]
	; edi = MC_Blob
	; ebx = Vtx_ID*12	(trashable)
	; esi = Off		(trashable)
	; don't touch ebp, ecx

	push ecx

	mov ecx, [O_Blob]	; ecx = O_Blob
	add ebx, [edi+OFF_NORMALS]	; ebx = &MC_Blob->Normals[Vtx_ID] OFFSET:
	mov esi, [ecx]		; Nb_Center
	dec esi
	jl near MC_N_End1
	fldz			; 0
	fst dword [ebx]         ; ->Ns[0]
	fst dword [ebx+4]	; ->Ns[1]
	fstp dword [ebx+8]	; ->Ns[2]
	fld dword [edx+0]	; x
	fld dword [edx+4]	; y | x
	fld dword [edx+8]	; z | y | x

align4
MC_N_Loop1:		; z | y | x
	lea eax,[esi*3]
	fld dword [ecx+OFF_Gn+eax*4]	; O_Blob->G[n][0]
	fsubr st0,st3			; x-X | z | y | x
	fld dword [ecx+OFF_Gn+eax*4+4]	; O_Blob->G[n][1]
	fsubr st0,st3			; y-Y | x-X | z | y | x
	fld dword [ecx+OFF_Gn+eax*4+8]	; O_Blob->G[n][2]
	fsubr st0,st3			; z-Z | y-Y | x-X | z | y | x
	fld st0
	fmul st0			; Z'^2 | z-Z | y-Y | x-X | z | y | x
	fld st2				; Y' | Z'^2 | z-Z | y-Y | x-X | z | y | x
	fmul st0
	faddp st1			; Y'^2 + Z'^2 | z-Z | y-Y | x-X | z | y | x
	fld st3
	fmul st0
	faddp st1			; N'^2 |  z-Z | y-Y | x-X | z | y | x

	fmul dword [ecx+OFF_INV_RAD2+esi*4]	; *O_Blob->Inv_Radius2[i]
	fistp dword [Blob_Table2]		; !!! Hack! Blob_Table2[0] is trashable :)

	mov eax, [Blob_Table2]
	cmp eax, BLOB_TABLE_SIZE
	jge MC_N_Clear

	fld dword [Blob_Table2+eax*4]	; Tmp | z-X | y-Y | x-Z | z | y | x
	fmul st3, st0
	fmul st2, st0
	fmulp st1, st0			; Nz | Ny | Nx | z | y | x
	fadd dword [ebx+8]
	fxch st1			; Ny | Nz | Nx | z | y | x
	fadd dword [ebx+4]
	fxch st2			; Nx | Nz | Ny | z | y | x
	fadd dword [ebx+0]		; Nx | Nz | Ny | z | y | x
	fstp dword [ebx+0]
	fstp dword [ebx+8]
	dec esi
	fstp dword [ebx+4]
	jge near MC_N_Loop1
	jmp MC_N_Finish
align4
MC_N_Clear
	fcompp
	dec esi
	fstp st0
	jge near MC_N_Loop1

align4
MC_N_Finish:
	fcompp
	fstp st0

	fld dword [ebx+0]		; Nx
	fld dword [ebx+4]		; Ny | Nx
	fld dword [ebx+8]		; Nz | Ny | Nx
	fld st0
	fmul st0			; Nz^2 | Nz | Ny | Nx
	fxch st2
	fld st0				; Ny | Ny | Nz | Nz^2 | Nx
	fmul st0			; Ny^2 | Ny | Nz | Nz^2 | Nx
	faddp st3			; Ny | Nz | Nz^2+Ny^2 | Nx
	fxch st3			; Nx | Nz | Nz^2+Ny^2 | Ny
	fld st0
	fmul st0			; Nx^2 | Nx | Nz | Nz^2+Ny^2 | Ny
	faddp st3			; Nx | Nz | N^2 | Ny
	fxch st2			; N^2 | Nz | Nx | Ny
	fsqrt
	fld1				; 1 | N | Nz | Nx | Ny
	fdivrp st1,st0			; 1/N | Nz | Nx | Ny
	fxch st3			; Ny | Nz | Nx | 1/N
	fmul st3
	fstp dword [ebx+4]		; Nz | Nx | 1/N
	fmul st2
	fstp dword [ebx+8]		; Nx | 1/N
	fmulp st1
	fstp dword [ebx+0]
	
MC_N_End1:
	pop ecx
	ret

%endif ; COMPUTE_MC_NORMAL_ASM

;//////////////////////////////////////////////////////////////////////////

%ifdef GROW_NEIGHBOURS_ASM

MC_Grow_Neighbours:

	Enter

	mov ebp,4*(MCX*MCY-MCY-2-MCY-1)	; ebp = k

	mov edi,[Flags2]		; edi = Flags2
	mov esi,edi
	xchg [Flags1],esi		; esi = Flags1
	xor ebx,ebx			; Prev
	mov [Flags2],esi
	add esi,4*(MCY+1)
	add edi,4*(MCY+1)
Ngh_1:
	mov eax,[esi+ebp]		; Flags1[k]
	test eax,eax
	jne Ngh_2
Ngh_Loop:
	sub ebp,4
	jge Ngh_1

	Leave

align4
Ngh_2:
	mov bl,al
	mov edx,[Ngh_Tab + 4*ebx]	; edx = Column
	shr edx,8
	mov bl,ah
	or  edx,[Ngh_Tab + 4*ebx]
	shr eax,16
	mov bl,al
	mov ecx,[Ngh_Tab + 4*ebx]
	shl ecx,8
	mov bl,ah
	or edx,ecx
	mov ecx,[Ngh_Tab + 4*ebx]
	shl ecx,16
	or edx,ecx

	or [edi+ebp -4-MCY*4],edx
	or [edi+ebp   -MCY*4],edx
	or [edi+ebp +4-MCY*4],edx
	or [edi+ebp -4 ],edx
	or [edi+ebp    ],edx
	or [edi+ebp +4 ],edx
	or [edi+ebp -4+MCY*4],edx
	or [edi+ebp   +MCY*4],edx
	or [edi+ebp +4+MCY*4],edx
	jmp Ngh_Loop
%endif

;//////////////////////////////////////////////////////////////////////////

%ifdef EMIT_MC_POLYS2_ASM
Emit_MC_Polys2:

	Enter
	sub esp,12

	mov ebp,0x00000001	; => MC_Mask
	fld dword [MC_OFF_Z]
	fst dword [Slice+8]
	mov eax,(MCZ)		; k
	mov [esp+8], eax

	mov ebx, [Flags1]

align4
MCP2_Loop:	; st0 = Slice[2]

	dec dword [esp+8]
	jg near MCP2_Loop2
	fstp st0

	add esp,12
	Leave

align4
MCP2_Loop2:	; st0 = Slice[2]

	mov eax, [Slice_A]
	xchg [Slice_B], eax
	mov [Slice_A], eax

	mov esi, [Flags_A]
	mov edi, esi		; edi = Flags_B
	xchg [Flags_B], esi	; esi = Flags_A
	mov [Flags_A], esi

	fadd dword [MC_SCALE_Z]
	shl ebp, 1
	fstp dword [Slice+8]
	mov eax, (MCX-2)	; i
	fld dword [MC_OFF_X]
	mov [esp+4],eax
	mov edx,(MCY+1)		; Off

align4
MCP2_Loop_i			; st0 = Slice[0]

	fadd dword [MC_SCALE_X]	; Slice[0] += dx
	mov eax, (MCY-2)	; j
	fstp dword [Slice+0]
	mov [esp+0],eax
	fld dword [MC_OFF_Y]	; <= Slice[1]

align4
MCP2_Loop_j	; st0 = Slice[1]
		; edx=Off edi=Flags_B esi=Flags_A 
		; ebx = Flags1

	fadd dword [MC_SCALE_Y]	; Slice[1] += dy
	test [ebx+edx*4],ebp	; Flags1[Off]
	fstp dword [Slice+4]
	je MCP2_T2

%ifndef DASJDKASLAS

	; appel de la fonction en ASM

	; push ebx
        call [_Do_V_]
        mov ecx,[Slice_B]
	test eax, eax
        fstp dword [ecx+edx*4]
	; pop ebx
	jnz MCP2_T1
%else

	; sinon: appel de la fonction en C

	push ebx
	push edx		; Off
	call [_Do_V_]
	pop edx
	pop ebx
	test eax, eax
	jnz MCP2_T1
%endif

	or word [edi+edx+MCY], 0x0108
	or word [edi+edx],     0x0204
	or word [esi+edx+MCY], 0x1080
	or word [esi+edx],     0x2040
	jmp MCP2_T2
align4
MCP2_T1:
	xor [ebx+edx*4], ebp	; Flags1[Off]^=MC_Mask
MCP2_T2:
	xor eax,eax
	xor cl,cl
	mov al, byte [esi+edx]	; Flags_A[Off] = Index
	mov byte [esi+edx], cl		; Flags_A[Off] = 0x00
	cmp al, cl			; cl = 0x00
	je MCP2_T3
	cmp al, 0xFF
	je MCP2_T3

	push eax		; Index
	push edx		; Off
	call Emit_MC_Polys3
	pop edx
	pop eax

MCP2_T3:
	fld dword [Slice+4]
	inc edx			; Off++
	dec dword [esp+0]	; j--
	jg near MCP2_Loop_j
	fstp st0

	add edx,2
	fld dword [Slice+0]
	dec dword [esp+4]	; i--
	jg near MCP2_Loop_i
	fstp st0

	fld dword [Slice+8]
	jmp MCP2_Loop

%endif

;//////////////////////////////////////////////////////////////////////////

%ifdef EMIT_MC_POLYS3_ASM

align4
Emit_MC_Polys3:
	sub esp,24		; ..+16= 40
	push ebp
	push edi
	push esi
	push ebx

	mov ebp, [esp+40+8]	; Index
	shl ebp,4		; *16
	add ebp, Edge_Order
	mov edi, [MC_Blob]	; edi = MC_Blob
	xor ecx,ecx		; Vtx_ID
Loop1:

; base_esp = 40     Edge_Table = esp+16. Size:24
; esi = Off  edi = MC_Blob
; output: cx = Vtx_ID

	xor eax,eax
	mov al, byte [ebp]
	cmp al,255
	je near End2

	mov esi, [esp+40+4]	; esi = Off
	jmp [Jmp_Tab+eax*4]	; Jmp_Tab+Edge*4

align4
case_0:
	mov cx, [X_Edge+esi*2]
	jmp End1
align4
case_1:
	mov cx, [Y_Edge+MCY*2 + esi*2]
	jmp End1
align4
case_2:
	mov cx, [X_Edge+2 + esi*2]
	jmp End1
align4
case_3:
	mov cx, [Y_Edge + esi*2]
	jmp End1
align4
case_4:
	mov cx, [Vtx_6]
	mov [X_Edge + esi*2], cx
	jmp End1
align4
case_5:
                ; launch div in // before ...
	mov eax,[Slice_B]
	fld dword [eax+esi*4]
	fld st0				; Slice_B[Off] | ./. | 
	fsubr dword [eax+esi*4-4]
	fdivp st1

	mov cx, word [edi+OFF_NB_VTX]	; MC_Blob->Nb_Vertex OFFSET = 264
	inc dword [edi+OFF_NB_VTX]
	mov eax,[esp+40+12]		; jo
	mov [Top_yEdge+eax*2],cx

	lea ebx,[ecx*3]			; Vtx_ID*3
	lea ebx,[ebx*4]			; ebx = VTX_ID*12
	mov edx,[edi+OFF_VTX]		; MC_Blob->Vertex OFFSET=244
	add edx, ebx

	mov eax,[Slice+8]
	mov [edx+8],eax			; Vertex[Vtx_Id][2]
	mov eax,[Slice+0]
	mov [edx],eax			; Vertex[Vtx_Id][0]

	fmul dword [MC_SCALE_Y]
	fadd dword [Slice+4]
	fstp dword [edx+4]		; => Vertex[Vtx_Id][1]

	call Compute_MC_Normal		; cx = Vtx_Id    ebx = Vtx_ID*12
					; edx = &Vertex[Vtx_Id]
	jmp End1
align4
case_6:
	mov eax,[Slice_B]
	fld dword [eax+esi*4]
	fld st0				; Slice_B[Off] | ./. | 
	fsubr dword [eax+esi*4-MCY*4]
	fdivp st1

	mov cx, word [edi+OFF_NB_VTX]	; MC_Blob->Nb_Vertex OFFSET = 264
	inc dword [edi+OFF_NB_VTX]
	mov [Vtx_6],cx

	lea ebx,[ecx*3]			; Vtx_ID*3
	lea ebx,[ebx*4]			; ebx = VTX_ID*12
	mov edx,[edi+OFF_VTX]		; MC_Blob->Vertex OFFSET=244
	add edx, ebx

	mov eax,[Slice+8]
	mov [edx+8],eax			; Vertex[Vtx_Id][2]
	mov eax,[Slice+4]
	mov [edx+4],eax			; Vertex[Vtx_Id][1]

	fmul dword [MC_SCALE_X]
	fadd dword [Slice+0]
	fstp dword [edx]		; Vertex[Vtx_Id][0]

	call Compute_MC_Normal		; cx = Vtx_Id    ebx = Vtx_ID*12
					; edx = &Vertex[Vtx_Id]
	jmp End1
align4
case_7:
	mov eax, [esp+40+12]		; jo
	mov cx, [Top_yEdge + eax*2]
	mov [Y_Edge + esi*2], cx
	jmp End1
align4
case_8:
	mov eax, [esp+40+12]		; jo
	mov cx, [Z_Edge + eax*2]
	jmp End1
align4
case_9:
	mov cx, [Vtx_10]
	mov eax, [esp+40+12]		; jo
	mov [Z_Edge + eax*2], cx
	jmp End1
align4
case_10:
	mov eax,[Slice_B]
	fld dword [eax+esi*4]
	mov eax,[Slice_A]
	fld st0				; Slice_B[Off] | ./. | 
	fsubr dword [eax+esi*4]
	fdivp st1

	mov cx, word [edi+OFF_NB_VTX]	; MC_Blob->Nb_Vertex OFFSET = 264
	inc dword [edi+OFF_NB_VTX]
	mov [Vtx_10],cx

	lea ebx,[ecx*3]			; Vtx_ID*3
	lea ebx,[ebx*4]			; ebx = VTX_ID*12
	mov edx,[edi+OFF_VTX]		; MC_Blob->Vertex OFFSET=244
	add edx, ebx

	mov eax,[Slice+0]
	mov [edx+0],eax			; Vertex[Vtx_Id][0]
	mov eax,[Slice+4]
	mov [edx+4],eax			; Vertex[Vtx_Id][1]

	fmul dword [MC_SCALE_Z]
	fadd dword [Slice+8]		; +Slice[2]
	fstp dword [edx+8]		; Vertex[Vtx_Id][2]

	call Compute_MC_Normal		; cx = Vtx_Id    ebx = Vtx_ID*12
					; edx = &Vertex[Vtx_Id]
	jmp End1
align4
case_11:
	mov eax, [esp+40+12]		; jo
	mov cx, [Z_Edge-2 + eax*2]	; Z_Edge[jo-1]
align4
End1:		;ebx = Edges_Table[..]      ecx = Vtx_ID
	xor eax,eax
	mov al, byte [ebp]
	mov [Edge_T+eax*2], cx		; [Edge_Table+Edge*2] = cx

	inc ebp
	mov al, byte [ebp]
	mov esi, [esp+40+4]	; esi = Off

	cmp al,255
	je End2
	jmp [Jmp_Tab+eax*4]	; Jmp_Tab+Edge*4

align4
End2:
	mov edx, [esp+40+8]	; Index
	shl edx,5		; *32	(MC_PTS=32)
	add edx, Poly_Order	; edx = Poly_Order[Index]

	xor eax,eax
align4
Loop2:

	mov al, [edx]		; edi = MC_Blob
	cmp al,255
	je End3

	mov ebx, [edi+OFF_NB_POLYS]	; Blob->Nb_Polys = OFFSET 268
	inc dword [edi+OFF_NB_POLYS]	; Blob->Nb_Polys++

	imul ebx,SIZEOF_POLY		; sizeof(POLY)=48?
	add ebx, [edi+OFF_POLYS]	; MC_Blob->Polys = OFFSET 256
				; ebx = new poly adress

	lea esi, [Edge_T]	; esi = Edge_T = Edge_Table

	mov cx, [esi+2*eax]	; si = Edge_Table[...]
	mov [ebx+6], cx		; P->Pt[0]
	inc edx

	mov al, [edx]
	mov cx, [esi+2*eax]
	mov [ebx+8], cx		; P->Pt[1]
	inc edx

	mov al, [edx]
	mov cx, [esi+2*eax]
	mov [ebx+10], cx	; P->Pt[2]
	inc edx

	jmp Loop2
align4
End3:
	pop ebx
	pop esi
	pop edi
	pop ebp
	add esp,24
	ret

%endif	; EMIT_MC_POLYS3_ASM

;//////////////////////////////////////////////////////////////////////////
