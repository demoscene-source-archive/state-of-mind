;//////////////////////////////////////////////////////////////////////////

[BITS 32]
globl _Draw_Flat_16
globl _Draw_Flat2_16
globl _Draw_16b_256_16
globl _Draw_Gouraud_16
globl _Draw_Gouraud_Ramp_16
globl _Draw_zGouraud_16
globl _Draw_UV_16b
globl _Draw_UV_Offset_16

;//////////////////////////////////////////////////////////////////////////

extrn _RCst_
extrn ZBuffer
extrn ZBuffer_Front
extrn ZBuffer_Back
extrn UV_Src
extrn Flat_Color
extrn dyS
extrn dyU
extrn dyV
extrn Scan_S
extrn Scan_U
extrn Scan_V
extrn Scan_Start
extrn Scan_H
extrn Scan_Pt1
extrn Scan_Pt2

;//////////////////////////////////////////////////////////////////////////

DATA

_Draw_Flat_16:

	Enter

	mov edi,[Scan_Start]
	mov edx,edi
	mov ebx,[_RCst_+PIX_BPS]
	inc edi
	mov ebp,[Scan_H]
	imul edi,ebx
	mov eax,[Flat_Color]
	add edi,[_RCst_+BASE_PTR]

align4
Loop_FLAT_Y:	; %eax=Col, %ebx=BpS, %ecx=Len, %edx=y
		; %ebp=dy, %edi=_RCst_+BASE_PTR, %esi=Dst

	mov esi,[Scan_Pt1+edx*4]
	mov ecx,[Scan_Pt2+edx*4]
	sub ecx,esi
	jle FLAT_Next_Y

	neg esi
	lea esi,[edi+esi*2]

	xor ecx, 0xFFFFFFFF
	inc ecx
align4
Loop_FLAT_X:
	mov [esi+ecx*2],ax
	inc ecx
	jl Loop_FLAT_X

FLAT_Next_Y:
	add edi,ebx
	inc edx
	dec ebp
	jg Loop_FLAT_Y

	Leave

;///////////////////////

_Draw_Flat2_16:

	Enter

	mov edi,[Scan_Start]
	mov edx,edi
	mov ebx,[_RCst_+PIX_BPS]
	inc edi
	mov ebp,[Scan_H]
	imul edi,ebx
	mov eax,[Flat_Color]
	add edi,[_RCst_+BASE_PTR]


align4
Loop_FLAT2_Y:	; %eax=Col, %ebx=BpS, %ecx=Len, %edx=y
		; %ebp=dy, %edi=_RCst_+BASE_PTR, %esi=Dst

	mov esi,[Scan_Pt1+edx*4]
	mov ecx,[Scan_Pt2+edx*4]
	sub ecx,esi
	jle FLAT2_Next_Y

	neg esi
	lea esi,[edi+esi*2]

	xor ecx, 0xFFFFFFFF
	inc ecx
align4
Loop_FLAT2_X:
	mov [esi+ecx*2],ax
	inc ecx
	jl Loop_FLAT2_X

FLAT2_Next_Y:
	add edi,ebx
	inc edx
	dec ebp
	jg Loop_FLAT2_Y

	Leave

;//////////////////////////////////////////////////////////////////////////

_Draw_Gouraud_16:

	Enter

	mov edi,[Scan_Start]
	mov eax,edi					; %eax = y
	inc edi						; y++
	imul edi,[_RCst_+PIX_BPS]
	add edi,[_RCst_+BASE_PTR]     	; edi = _RCst_+BASE_PTR + y*BpS - 1
	mov ebp,[Scan_H]

	mov word dx,[dyS]       ; dS-lo
	mov word bx,[dyS+2]     ; dS-hi

align2
Loop_GRD_Y:
	mov esi,[Scan_Pt1+eax*4]	; X1
	mov ecx,[Scan_Pt2+eax*4]	; X2
	sub ecx,esi						; %ecx: X2-X1 = Len
	jle GRD_Next_Y

	push ebp
	neg esi
	push eax
	lea esi,[edi+esi*2]				; %esi: Dst

	mov word bp,[Scan_S+eax*4]		; S-Lo
	mov word ax,[Scan_S+2+eax*4]	; S-Hi
	xor ecx,0xFFFFFFFF
	inc ecx

align2
Loop_GRD_X:
	sub word bp,dx
	mov byte [esi+ecx*2],al
	sbb word ax,bx
	inc ecx
	jl Loop_GRD_X
	pop eax
	pop ebp
GRD_Next_Y:
	add edi,[_RCst_+PIX_BPS]
	inc eax
	dec ebp
	jg Loop_GRD_Y

	Leave


;//////////////////////////////////////////////////////////////////////////

_Draw_Gouraud_Ramp_16:
	Enter

	mov edi,[Scan_Start]
	mov edx,edi				; %edx = y
	inc edi					; y++
	mov ebx,[_RCst_+PIX_BPS]
	mov ebp,[Scan_H]
	imul edi,ebx
	add edi,[_RCst_+BASE_PTR]	; %edi = _RCst_+BASE_PTR + y*BpS - 1

align2
Loop_GRD_RAMP_Y: 	; %eax=Col, %ebx=BpS, %ecx=Len, %edx=y
		 				; %ebp=dy, %edi=_RCst_+BASE_PTR, %esi=Dst
	mov esi,[Scan_Pt1+edx*4]
	mov ecx,[Scan_Pt2+edx*4]
	sub ecx,esi
	jle GRD_RAMP_Next_Y

	neg esi
	mov word ax,[Scan_S+2+edx*4]
	lea esi,[edi+esi*2]			;%esi: Dst
	xor ecx,0xFFFFFFFF
	inc ecx

align2
Loop_GRD_RAMP_X:
	mov byte [esi+ecx*2],al
	inc ecx
	jl Loop_GRD_RAMP_X

GRD_RAMP_Next_Y:
	add edi,ebx
	inc edx
	dec ebp
	jg Loop_GRD_RAMP_Y

	Leave


;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

_Draw_UV_16b:

	Enter

	mov edi,[Scan_Start]
	mov ecx,edi		;%ecx = y
	inc edi			; y++
	imul edi,[_RCst_+PIX_BPS]
	add edi,[_RCst_+BASE_PTR]     	; %edi = _RCst_+BASE_PTR + y*BpS - 1
	mov esi,[Scan_H]

	mov eax,[UV_Src]	; Base_Txt
	xor ebx,ebx
	mov [Loop_UV_X+4],eax
	mov edx,[dyV]		; dyVf
	mov eax,[dyU]		; dyUf
	shl edx,16
	mov dh,[dyV+2]		; dyVi
	shl eax,16
	mov dl,[dyU+2]		; dyUi

align2
Loop_UV_Y:
	mov ax,[Scan_Pt2+ecx*4]
	movzx ebp,word [Scan_Pt1+ecx*4]
	sub word ax,bp
	jle UV_Next_Y
	push ecx
	push edi
	push esi

	neg ebp
	
	lea edi,[edi+ebp*2]		; %edi: Dst

	mov ebp,[Scan_V+ecx*4]
	mov bh,[Scan_V+2+ecx*4]
	shl ebp,16
	mov esi,[Scan_U+ecx*4]
	mov bl,[Scan_U+2+ecx*4]
	shl esi,16

	mov cx,ax
	xor ecx,0xFFFFFFFF
	inc ecx
align2
Loop_UV_X: 
	mov ax,[0x12345678+ebx*2]
	sub ebp,edx
	sbb bh,dh
	sub esi,eax
	sbb bl,dl
	mov [edi+ecx*2],ax
	inc ecx
	jl Loop_UV_X
	pop esi
	pop edi
	pop ecx
UV_Next_Y:
	inc ecx
	add edi,[_RCst_+PIX_BPS]
	dec esi
	jg Loop_UV_Y

	Leave

;//////////////////////////////////////////////////////////////////////////
;////////////////////////////// J U N K ///////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

_Draw_16b_256:
%ifdef SDAJKASLDJASK

	Enter

	mov edi,[Scan_Start]
	mov edx,edi
	mov ebp,[Scan_H]
	shl edi,9
	mov word ax,[Flat_Color]
	add edi,[Light_Buf]	; %edi = Light_Buf + y*256 - 1
	sub edi,2
	push word ax
	shl eax,16
	pop word ax

align2
Loop_B16_Y:	; %eax=Col, %ebx=BpS, %ecx=Len, %edx=y
		; %ebp=dy, %edi=_RCst_+BASE_PTR, %esi=Dst
	mov esi,[Scan_Pt1+edx*4]
	mov ecx,[Scan_Pt2+edx*4]
	sub ecx,esi
	jle B16_Next_Y
	lea esi,[edi+4*esi]

	test word si,0x02
	jne B16_No_Align
	add edi,2
	mov word [2*esi],ax
	dec ecx
	je B16_Next_Y
B16_No_Align:
	shr ecx,1
	jnc Loop_B16_X
	mov word [4+esi+ecx*4],ax
	test ecx,ecx
	je B16_Next_Y
align2
Loop_B16_X:
	mov [esi+ecx*4],eax
	loop Loop_B16_X

B16_Next_Y:
	inc edx
	add edi,512
	dec ebp
	jg Loop_B16_Y

%endif
	Leave

%ifdef DSAJDKSALJDKSAL
_Draw_zGouraud_16:
	Enter

	mov edi,[Scan_Start]
	mov eax,edi			; %eax = y
	shl edi,9
	add edi,[Light_Buf]     	; %edi = Light_Buf + y*256*2 - 1
	mov ebx,[Scan_H]
	mov [DY],ebx

	mov word dx,[dyS]       	; dS-lo
	mov word bx,[dyS+2]		; dS-hi

align2
Loop_zGRD_Y:
	mov esi,[Scan_Pt1+eax*4]	;X1
	mov ecx,[Scan_Pt2+eax*4]	;X2
	sub ecx,esi             	;%ecx: X2-X1 = Len
	jle zGRD_Next_Y

	push eax
	mov esi,edi+esi*2		;%esi: Dst

	mov word bp,[Scan_S+eax*4]   	;S-Lo
	mov word ax,[Scan_S+2+eax*4]	;S-Hi

align2
Loop_zGRD_X:
	sub word bp,dx
	mov word [%esi+ecx*2],ax
	sbb word ax,bx
	loop Loop_zGRD_X
	pop eax
zGRD_Next_Y:
	add edi,512
	inc eax
	dec dword [DY]
	jg Loop_zGRD_Y

	Leave
%endif

;//////////////////////////////////////////////////////////////////////////
;////////////////////////////////////////////////////////////////////
