;//////////////////////////////////////////////////////////////////////////

;	Same as in mcube.h

%define EMIT_MC_POLYS_ASM
%define FILL_SLICE3_ASM
%define EMIT_ALL_MC_ASM

%define BLOB_TABLE_SIZE	256

;//////////////////////////////////////////////////////////////////////////

[BITS 32]

%ifdef EMIT_MC_POLYS_ASM
globl Emit_MC_Polys
%else
extrn Emit_MC_Polys
%endif
extrn Compute_MC_Normal

%ifdef EMIT_ALL_MC_ASM
globl Emit_All_MC
%else
extrn Emit_All_MC
%endif

%ifdef FILL_SLICE3_ASM
globl Fill_Slice3
%else
extrn Fill_Slice3
%endif

globl Clear_Slice_A

;//////////////////////////////////////////////////////////////////////////

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

DATA
align4

extrn MC_SCALE_X
extrn MC_SCALE_Y
extrn MC_SCALE_Z
extrn I_MC_SCALE_X
extrn I_MC_SCALE_Y
extrn I_MC_SCALE_Z
extrn MC_OFF_X
extrn MC_OFF_Y
extrn MC_OFF_Z
extrn MC_OFF_END_X
extrn MC_OFF_END_Y
extrn MC_OFF_END_Z

%define MCX	32
%define MCY	32
%define MCZ	32

%ifdef EMIT_MC_POLYS_ASM
Jmp_Table: dd case_0,case_1,case_2,case_3,case_4,case_5,case_6,case_7,case_8,case_9,case_10,case_11
%endif

;//////////////////////////////////////////////////////////////////////////

TEXT

%ifdef EMIT_MC_POLYS_ASM

align4
Emit_MC_Polys:
	sub esp,24		; ..+16= 40
	push ebp
	push edi
	push esi
	push ebx

	mov ebp, [esp+40+8]	; Index
	shl ebp,4		; *16
	add ebp, Edge_Order
	mov edi, [MC_Blob]	; edi = MC_Blob
	xor ecx,ecx
Loop1:

; base_esp = 40     Edge_Table = esp+16. Size:24
; esi = Off  edi = MC_Blob
; output: cx = Vtx_ID

	xor eax,eax
	mov al, byte [ebp]
	cmp al,255
	je near End2

	mov esi, [esp+40+4]	; esi = Off
	jmp [Jmp_Table+eax*4]	; Jmp_Table+Edge*4

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
	mov cx, word [edi+272]		; MC_Blob->Nb_Vertex OFFSET = 272
	inc dword [edi+272]
	mov eax,[esp+40+16]		; jo
	mov [Top_yEdge+eax*2],cx

	lea ebx,[ecx*3]			; Vtx_ID*3
	lea ebx,[ebx*4]			; ebx = VTX_ID*12
	mov edx,[edi+252]		; MC_Blob->Vertex OFFSET=252
	add edx, ebx

	fld dword [MC_SCALE_X]
	fimul dword [esp+40+12]			;*io
	fadd dword [MC_OFF_X]
	fadd dword [MC_SCALE_X]
	fstp dword [edx]			; Vertex[Vtx_Id][0]

	mov eax,[Slice_B]
	fld dword [MCY*4+eax+esi*4]
	fld st0				; Slice_B[Off+MCY] | ./. | 
	fsub dword [MCY*4+4+eax+esi*4]
	;fxch st1
	fdivp st1

	mov eax,[Slice+8]
	mov [8+edx],eax			; Vertex[Vtx_Id][2]

	fiadd dword [esp+40+16]		; +jo
	fmul dword [MC_SCALE_Y]
	fadd dword [MC_OFF_Y]
	fstp dword [4+edx]

	call Compute_MC_Normal		; cx = Vtx_Id    ebx = Vtx_ID*12
					; edx = &Vertex[Vtx_Id]
	jmp End1
align4
case_6:
	mov cx, word [edi+272]		; MC_Blob->Nb_Vertex OFFSET = 272
	inc dword [edi+272]
	mov [Vtx_6],cx

	lea ebx,[ecx*3]			; Vtx_ID*3
	lea ebx,[ebx*4]			; ebx = VTX_ID*12
	mov edx,[edi+252]		; MC_Blob->Vertex OFFSET=252
	add edx, ebx

	mov eax,[Slice_B]
	fld dword [4+eax+esi*4]
	fld st0				; Slice_B[Off+MCY] | ./. | 
	fsub dword [MCY*4+4+eax+esi*4]
	;fxch st1
	fdivp st1
	fiadd dword [esp+40+12]		; +io
	fmul dword [MC_SCALE_X]
	fadd dword [MC_OFF_X]
	fstp dword [edx]

	fild dword [esp+40+16]		; *jo
	fmul dword [MC_SCALE_Y]
	fadd dword [MC_OFF_Y]
	fadd dword [MC_SCALE_Y]
	fstp dword [4+edx]		; Vertex[Vtx_Id][1]

	mov eax,[Slice+8]
	mov [8+edx],eax			; Vertex[Vtx_Id][2]

	call Compute_MC_Normal		; cx = Vtx_Id    ebx = Vtx_ID*12
					; edx = &Vertex[Vtx_Id]
	jmp End1
align4
case_7:
	mov eax, [esp+40+16]		; jo
	mov cx, [Top_yEdge + eax*2]
	mov [Y_Edge + esi*2], cx
	jmp End1
align4
case_8:
	mov eax, [esp+40+16]		; jo
	mov cx, [Z_Edge + eax*2]
	jmp End1
align4
case_9:
	mov cx, [Vtx_10]
	mov eax, [esp+40+16]		; jo
	mov [Z_Edge + eax*2], cx
	jmp End1
align4
case_10:
	mov cx, word [edi+272]		; MC_Blob->Nb_Vertex OFFSET = 272
	inc dword [edi+272]
	mov [Vtx_10],cx

	lea ebx,[ecx*3]			; Vtx_ID*3
	lea ebx,[ebx*4]			; ebx = VTX_ID*12
	mov edx,[edi+252]		; MC_Blob->Vertex OFFSET=252
	add edx, ebx

	fld dword [MC_SCALE_X]
	fimul dword [esp+40+12]			;*io
	fadd dword [MC_OFF_X]
	fadd dword [MC_SCALE_X]
	fstp dword [edx]		; Vertex[Vtx_Id][0]

	fld dword [MC_SCALE_Y]
	fimul dword [esp+40+16]			;*jo
	fadd dword [MC_OFF_Y]
	fadd dword [MC_SCALE_Y]
	fstp dword [4+edx]		; Vertex[Vtx_Id][1]

	mov eax,[Slice_B]
	fld dword [MCY*4+4 + eax+esi*4]
	fld st0				; Slice_B[Off+MCY] | ./. | 
	mov eax,[Slice_A]
	fsubr dword [MCY*4+4 + eax+esi*4]
	;fxch st1
	fdivp st1
	fmul dword [MC_SCALE_Z]
	fadd dword [Slice+8]		; +Slice[2]
	fstp dword [8+edx]		; Vertex[Vtx_Id][2]

	call Compute_MC_Normal		; cx = Vtx_Id    ebx = Vtx_ID*12
					; edx = &Vertex[Vtx_Id]
	jmp End1
align4
case_11:
	mov eax, [esp+40+16]		; jo
	mov cx, [Z_Edge+2 + eax*2]
align4
End1:		;ebx = Edges_Table[..]      ecx = Vtx_ID
	xor eax,eax
	mov al, byte [ebp]
	mov [Edge_T+eax*2], cx			; [Edge_Table+Edge*2] = cx

	inc ebp
	mov al, byte [ebp]
	mov esi, [esp+40+4]	; esi = Off

	cmp al,255
	je End2
	jmp [Jmp_Table+eax*4]	; Jmp_Table+Edge*4

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

	mov ebx, [edi+276]	; Blob->Nb_Polys = OFFSET 276
	inc dword [edi+276]	; Blob->Nb_Polys++

	imul ebx,SIZEOF_POLY	; sizeof(POLY)=48
	add ebx, [edi+264]	; MC_Blob->Polys = OFFSET 264
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

%endif	; EMIT_MC_POLYS_ASM

;//////////////////////////////////////////////////////////////////////////

align4
Clear_Slice_A:
	push eax
	push ebx
	mov ebx,[Slice_A]
	fld dword [esp+12]
	mov eax,MCX*MCY-1	;=1023
align4
Loop_Clear_A:
	fst dword [ebx+eax*4]
	dec eax
	jge Loop_Clear_A
	fstp st0
	pop ebx
	pop eax
	ret
;//////////////////////////////////////////////////////////////////////////

%ifdef EMIT_ALL_MC_ASM

align4
Emit_All_MC:
	Enter
	xor edx,edx		;edx = k

align4
Loop_k:
	fld dword [MC_SCALE_Z]
	fadd dword [Slice+8]
	fstp dword [Slice+8]
	push edx

	mov eax,MCX*MCX-1	; MCX*MCY-1	= 1023
	mov ecx,[esp+20+4]	; ecx = Blob
	fld dword [ecx+8]	; Blob->K
	mov ebx,[Slice_B]	; ebx = Slice_B
	fchs			; -Blob->K

align4
Loop_Clear:
	fst dword [ebx+eax*4]
	dec eax
	jge Loop_Clear
	fstp st0

	mov eax,[Flags_B]
	push ecx		; Blob
	push eax		; Flags_B
	push ebx		; Slice_B
	call Fill_Slice3
	add esp,12
	mov eax,[Flags_B]
	xor edx,edx		; clear upper bits of Index (=dl)

	;
	; call Fill_Z_yTop_Edge
	;

	mov ebp,[Flags_A]	; ebp=Flags_A, eax = Flags_B
	xor ecx,ecx		; ecx = Off
	xor edi,edi		; edi = i;

align4
Loop_i:
	;
	; call Fill_Vtx_6_10()
	;
	xor esi,esi		; esi = j

align4
Loop_j:
	mov dl,[ebp+ecx]	; dl = Index = Flags_low[Off]

	mov bl,[eax+ecx]
	shl ebx,4
	or dl,bl		; Index |= Flags_B[Off]<<4;

	mov bl,[ebp+ecx+MCY]
	shl ebx,1
	or dl,bl		; Flags_A[Off + MCY]<<1 ;

	mov bl,[eax+ecx+MCY]
	shl ebx,5
	or dl,bl		; Flags_B[Off + MCY]<<5;

	mov bl,[ebp+ecx+1]
	shl ebx,3
	or dl,bl		; Flags_A[Off + 1]<<3 ;

	mov bl,[eax+ecx+1]
	shl ebx,7
	or dl,bl		; Flags_B[Off + 1]<<7;

	mov bl,[ebp+ecx+MCY+1]
	shl ebx,2
	or dl,bl		; Flags_A[Off + 1+MCY]<<2;

	mov bl,[eax+ecx+MCY+1]
	shl ebx,6
	or dl,bl		; Flags_B[Off + 1+MCY]<<6;

	cmp dl,0
	je Next_Index
	cmp dl,255
	je Next_Index

	push eax
	push esi
	push edi
	push edx
	push ecx
	call Emit_MC_Polys
	pop ecx
	pop edx
	pop edi
	pop esi
	pop eax

align4
Next_Index:
	inc ecx			; Off++
	inc esi
	cmp esi,MCY-1
	jl Loop_j

	inc ecx			; Off++
	inc edi
	cmp edi,MCX-1
	jl Loop_i

	mov eax,[Slice_A]
	xchg [Slice_B],eax
	mov [Slice_A],eax
	mov eax,[Flags_A]
	xchg [Flags_B],eax
	mov [Flags_A],eax
	pop edx
	inc edx
	cmp edx,31
	jl near Loop_k

	Leave

%endif	; EMIT_ALL_MC_ASM

;//////////////////////////////////////////////////////////////////////////

%ifdef FILL_SLICE3_ASM

align4
Fill_Slice3:
	Enter

	sub esp,24
	mov esi, [esp+40+12]		; Blob
	mov edx, [esi]			; edx = Nb_Centers-1
	dec edx
	jl near End_Slice3
		; Blob->Radius[n] = OFFSET 140 + n*4
		; Blob->Radius2[n] = OFFSET 204 + n*4
		; Blob->Inv_Radius2[n] = OFFSET 172 + n*4
		; Blob->G[n] = OFFSET 12 + n*3*4
		; Blob->Blob = OFFSET 4

	lea edi,[esi+12]		
	lea eax,[edx*3]
	lea edi,[edi+eax*4]		; edi = &Blob->G[n]

	;mov ebx,[esp+40+4]		; ebx = Ptr
align4
Loop_n:

	fld dword [Slice+8]		; Slice[2]
	fsub dword [edi+8]		; Slice[2] - Blob->G[n][2]
	fmul st0			; ro = ro*ro
	fld dword [esi+204+edx*4]	; L = Blob->Radius2[n] | ro^2
	fmul st0
	fsub st1			; L^2-ro^2 | ro^2
	fstp dword [esp]		; ro^2
	test dword [esp],0x80000000
	jnz near Loop_n_Cont

	fld dword [esi+140+edx*4]	; L=Blob->Radius[n] | ro^2	
	fld dword [edi+0]		; G[n][0] | L | ro^2
	fsub dword [MC_OFF_X]		; G[n][0]-OFF_X  | L | ro^2
	fld st0				; G[n][0]-OFF_X  | G[n][0]-OFF_X  | L | ro^2
	fsub st2
	fmul dword [I_MC_SCALE_X]
	fistp dword [esp]		; io	   [ G[n][0]-OFF_X  | L | ro^2 ]

	fadd st1			; G[n][0]-OFF_X+L | L | ro^2
	fmul dword [I_MC_SCALE_X]
	fistp dword [esp+4]		; i2		[ L | ro^2 ]

	fld dword [edi+4]		; G[n][1] | L | ro^2
	fsub dword [MC_OFF_Y]		; G[n][1]-OFF_Y  | L | ro^2
	fld st0
	fsub st2
	fmul dword [I_MC_SCALE_Y]
	fistp dword [esp+8]		; jo

	faddp st1			; G[n][1]-OFF_Y+L | ro^2
	fmul dword [I_MC_SCALE_X]
	fistp dword [esp+12]		; j2			[ ro^2 ]

	fld dword [MC_SCALE_X]
	fild dword [esp]		; io | MC_SCALE_X | ro^2
	fmul st1	
	fadd dword [MC_OFF_X]
	fstp dword [Slice+0]		; Slice[0]	[ MC_SCALE_X | ro^2 ]


	mov ebx,[esp]			; i = io
	dec ebx
	dec dword [esp+8]

Loop_io:
	cmp ebx,[esp+4]
	jge near End_Loop_io

	fld dword [Slice+0]
	fsub dword [edi]		; Slice[0]-G[n][0] | MC_SCALE_X | ro^2
	fmul st0
	fadd st2			; r2 | MC_SCALE_X | ro^2
	fcom dword [esi+204+edx*4]	; Radius2[n]
	fstsw ax
	and ah,0x01
	jz Skip_io
	;and ah,69
	;dec ah
	;cmp ah,64
	;jnb Skip_io

	push ebx

	shl ebx,5+2			; i*MCY*4		(MCY=32)
	mov ebp,[esp+8+4]		; ebp = jo
	lea ebx,[ebx+ebp*4]		; + jo  => ebx = Off
	add ebx,[esp+40+4+4]		; + Ptr

	fld dword [MC_SCALE_Y]
	fimul dword [esp+8+4]		; jo*MC_SCALE_Y
	fadd dword [MC_OFF_Y]
	fst dword [Slice+4]		; => Slice[1]
					; [ Slice[1] | r2 | MC_SCALE_X | ro^2 ]

Loop_jo:
	cmp ebp,[esp+12+4]		; ebp=j.  j < j2?
	jge End_Loop_jo

		; Slice[1] | r2 | MC_SCALE_X | ro^2

	fld st0
	fsub dword [edi+4]
	fmul st0
	fadd st2		; r3 | Slice[1] | r2 | MC_SCALE_X | ro^2
	fcom dword [esi+204+edx*4]	; Radius2[n]
	fstsw ax
	and ah,0x01
	jz Skip_jo
	;and ah,69
	;dec ah
        ;cmp ah,64
	;jnb Skip_jo

	fmul dword [esi+172+edx*4]	; *Inv_Radius2
	fistp dword [esp+16+4]
	mov eax, [esp+16+4]
	fld dword [Blob_Table+eax*4]
	;fmul dword [Strength]
	fadd dword [ebx]
	fst dword [ebx]
Skip_jo:
	fstp st0		; pops r3. => [ Slice[1] | r2 | MC_SCALE_X | ro^2 ]
	inc ebp
	fadd dword [MC_SCALE_Y]		; Slice[1] += MC_SCALE_Y
	add ebx,4
	jmp Loop_jo

End_Loop_jo:
	fstp st0		; pops Slice[1] => [ r2 | MC_SCALE_X | ro^2 ]
	pop ebx
	
Skip_io:
	fstp st0			; pops r2
	fld st0
	fadd dword [Slice+0]		; Slice[0]+MC_SCALE_X | MC_SCALE_X | ro^2
	fstp dword [Slice+0]
	inc ebx
	jmp Loop_io

End_Loop_io:
	fstp st0			; pops MC_SCALE_X

Loop_n_Cont:
	fstp st0			; pops ro^2
	sub edi,12
	dec edx
	jge near Loop_n

End_Slice3:
	mov esi,[esp+40+4]	; esi = Ptr
	mov ecx,MCX*MCY-1		; =1023
	mov edi,[esp+40+8]	; edi = Flags

	mov eax,0
	mov ebx,1
	mov edx,0x80000000
align4
Loop_Flags:
	test dword [esi+4*ecx],edx
	je Flag_Set
	mov [edi+ecx],al
	loop Loop_Flags
	jmp End4
align4
Flag_Set:
	mov [edi+ecx],bl
	loop Loop_Flags

End4:
	add esp,24
	Leave
%endif

;//////////////////////////////////////////////////////////////////////////
