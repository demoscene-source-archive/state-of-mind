;//////////////////////////////////////////////////////////////////////////

[BITS 32]
globl _Draw_zBuf_ID

;//////////////////////////////////////////////////////////////////////////

extrn ZBuffer
extrn ZBuffer_Front
extrn ZBuffer_Back
extrn _RCst_
extrn Flat_Color
extrn Flat_Color2
extrn dyS
extrn Scan_S
extrn Scan_Start
extrn Scan_H
extrn Scan_Pt1
extrn Scan_Pt2

;//////////////////////////////////////////////////////////////////////////

DATA

_Draw_zBuf_ID:

	Enter

	mov edi,[Scan_Start]
	mov eax,edi				; %eax = y
	inc edi					; y++
	mov ebx,[_RCst_+PIX_BPS]
	shl ebx,1
	imul edi,ebx
	mov [ZBUF_Next_Y+1],ebx

	mov ebx,[ZBuffer_Back]
	add ebx,edi
	mov edx,[ZBuffer_Front]
	add edx,edi

	add edi,[ZBuffer]		; edi = ZBuffer + (y+1)*Pix_Width

	mov bp,[Flat_Color2]
	mov [Blah_ID+8], bp
	mov bp,[Flat_Color]
	mov [Blah_ID+18], bp

	mov bp,[dyS]    		; dS-hi
	mov word [No_Z_ID+3],bp
	mov bp,[dyS+2]  		; dS-lo
	mov word [No_Z_ID+7],bp
	
	mov ebp,[Scan_H]

align2
Loop_ZBUF_Y:
	mov esi,[Scan_Pt1+eax*4]	; X1
	mov ecx,[Scan_Pt2+eax*4]	; X2
	sub ecx,esi						; %ecx: X2-X1 = Len
	jle ZBUF_Next_Y

	push ebp
	neg esi
	push eax
	lea ebp,[edx+esi*2]			;
	mov [Blah_ID+4],ebp
	lea ebp,[ebx+esi*2]			;
	mov [Blah_ID+14],ebp
	lea esi,[edi+esi*2]			; %esi: Dst

	mov word bp,[Scan_S+eax*4]		; S-Lo
	xor ecx,0xFFFFFFFF
	mov word ax,[Scan_S+2+eax*4]	; S-Hi
	inc ecx

align2
Loop_ZBUF_X:
	cmp [esi+ecx*2],ax
	jge No_Z_ID
	mov [esi+ecx*2],ax
Blah_ID:
	mov word [0x12345678+ecx*2],0x1234
	mov word [0x12345678+ecx*2],0x1234
No_Z_ID:
	sub bp,0x1234
	sbb ax,0x1234
	inc ecx
	jl Loop_ZBUF_X
	pop eax
	pop ebp
ZBUF_Next_Y:
	mov ecx,0x1234		; <= Pix_BpS*2
	inc eax
	add edi,ecx
	add edx,ecx
	add ebx,ecx
	dec ebp
	jg Loop_ZBUF_Y

	Leave

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////
