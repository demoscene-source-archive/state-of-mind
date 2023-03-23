;//////////////////////////////////////////////////////////////////////////
;/// Common loops for special shaders. No 8/16bpp difference here.
;//////////////////////////////////////////////////////////////////////////

[BITS 32]

globl _Draw_UV_Offset
globl _Draw_zBuf
globl _Draw_Flat_zBuf

;//////////////////////////////////////////////////////////////////////////

;%include "../../include/rdtsc.h"

extrn ZBuffer
extrn ZBuffer_Front
extrn ZBuffer_Back
extrn _RCst_
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

TEXT

_Draw_UV_Offset:
	Enter

	mov edi,[Scan_Start]
	mov ecx,edi				; %ecx = y
	inc edi					; y++
	imul edi,[_RCst_+PIX_BPS]
	add edi,[_RCst_+BASE_PTR]	; %edi = Base_Ptr + y*BpS - 1
	mov esi,[Scan_H]

	mov ebx,[UV_Src]	; Base_Txt
	mov edx,[dyV]		; dyVf
	mov eax,[dyU]		; dyUf
	shl edx,16
	mov dh,[dyV+2]		; dyVi
	shl eax,16
	mov dl,[dyU+2]		; dyUi

align2
Loop_UV_Off_Y:
	mov bx,[Scan_Pt2+ecx*4]
	movzx ebp,word [Scan_Pt1+ecx*4]
	sub word bx,bp
	jle UV_Off_Next_Y
	push ecx
	push edi
	push esi

	neg ebp
	push bx
	
	lea edi,[edi+ebp*2]	; %edi: Dst

	mov ebp,[Scan_V+ecx*4]
	mov bh,[Scan_V+2+ecx*4]
	shl ebp,16
	mov esi,[Scan_U+ecx*4]
	mov bl,[Scan_U+2+ecx*4]
	shl esi,16

	pop cx
	xor ecx,-1
	inc ecx
align2
Loop_UV_Off_X: 
	mov [edi+ecx*2], bx
	sub esi, eax
	sbb bl, dl
	sub ebp, edx
	sbb bh, dh
	inc ecx
	jl Loop_UV_Off_X
	pop esi
	pop edi
	pop ecx

UV_Off_Next_Y:
	inc ecx
	add edi,[_RCst_+PIX_BPS]
	dec esi
	jg Loop_UV_Off_Y

	Leave

;//////////////////////////////////////////////////////////////////////////
;		(same as _Draw_Gouraud_16...)
;//////////////////////////////////////////////////////////////////////////

_Draw_zBuf:
	Enter

	mov edx,[Scan_Start]
	mov eax,edx				; %eax = y
	inc edx					; y++
	mov edi,[ZBuffer]    ; edi = ZBuffer + (y+1)*Pix_BpS
	imul edx,[_RCst_+PIX_WIDTH]
	mov ebp,[Scan_H]
	lea edi,[edi+2*edx]

	mov dx,[dyS]    ; dS-lo
	mov bx,[dyS+2]  ; dS-hi

align2
.Loop_Y:

	mov esi,[Scan_Pt1+eax*4]	; X1
	mov ecx,[Scan_Pt2+eax*4]	; X2
	sub ecx,esi						; %ecx: X2-X1 = Len
	jle .Next_Y

	neg esi
	neg ecx        ; ecx = Len>0
	push eax
	push ebp
	lea esi,[edi+esi*2]			; %esi: Dst

	mov word bp,[Scan_S+eax*4]		; S-Lo
	mov word ax,[Scan_S+2+eax*4]	; S-Hi

align2
.Loop_X:
	sub bp,dx
	mov word [esi+ecx*2],ax
	sbb ax,bx
	inc ecx
	jl .Loop_X
	pop ebp
	pop eax
.Next_Y:
	add edi,[_RCst_+PIX_WIDTH]
	inc eax
	add edi,[_RCst_+PIX_WIDTH]
	dec ebp
	jg .Loop_Y

	Leave

;//////////////////////////////////////////////////////////////////////

_Draw_Flat_zBuf:
   ret
	Enter

	mov edi,[Scan_Start]
	mov edx,edi
	mov ebx,[_RCst_+PIX_WIDTH]
	inc edi
	add ebx,ebx       ; W*sizeof(USHORT)
	mov ebp,[Scan_H]
	imul edi,ebx
	mov eax,[Flat_Color]
	add edi,[ZBuffer]

align4
.Y:	; %eax=Col, %ebx=BpS, %ecx=Len, %edx=y
						; %ebp=dy, %edi=Base_Ptr, %esi=Dst

	mov esi,[Scan_Pt1+edx*4]
	mov ecx,[Scan_Pt2+edx*4]
	sub ecx,esi
	jle .Next_Y

	neg esi
	lea esi,[edi+esi*2]
	neg ecx
align4
.X:
	mov word [esi+ecx*2],ax
	inc ecx
	jl .X

.Next_Y:
	add edi,ebx
	inc edx
	dec ebp
	jg .Y

	Leave

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////
