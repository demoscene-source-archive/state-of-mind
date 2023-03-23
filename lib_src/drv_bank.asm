;///////////////////////////////////////////////////////////////

[BITS 32]
globl Set_Bank_Int10h
globl Set_Bank_PM_Func
globl Set_Bank_RM_Func


%ifdef __WATCOMC__

	; WATCOM struct offsets
%define WIN_NO			196
%define CUR_BNK		200
%define PM_FUNC		212
%define RM_FUNC		216
%define SVGA_REGS		220

%else

extrn __djgpp_ds_alias
extrn __dpmi_error

	; DJGPP struct offsets
%define WIN_NO			204
%define CUR_BNK		208
%define PM_FUNC		220
%define RM_FUNC		224
%define SVGA_REGS		228

%endif

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////

TEXT

align4
Set_Bank_Int10h:		; args: *VBE, Bank_Nb
	push ebx
	push esi
	push edi
	mov eax,[esp+16]	; VBE
	mov dx,[20+esp]
	mov bx,[eax+WIN_NO]	; Win_No
	mov [CUR_BNK+eax],dx
	mov ax,0x4f05
	int 0x10
	pop edi 
   pop esi
   pop ebx
   ret

;///////////////////////////////////////////////////////////////

align4
Set_Bank_PM_Func:		; args: *VBE, Bank_Nb
	push ebx
	push esi
	push edi
	mov eax,[16+esp]			; VBE
	mov dx,[20+esp]			; Bank_Nb
	mov bx,[WIN_NO+eax]		; Win_No
	mov [CUR_BNK+eax],dx
	mov ecx,[PM_FUNC+eax]	; PM_WinFuncPtr
	mov ax,0x4f05
	call [ecx]
	pop edi 
   pop esi
   pop ebx
   ret

;///////////////////////////////////////////////////////////////

align4
Set_Bank_RM_Func:		; args: *VBE, Bank_Nb

%ifndef __WATCOMC__			; DISABLED, for now
	push ebx
	push esi
	push edi
	mov eax,[16+esp]	; VBE
	mov dx,[20+esp]		; Nb
	mov [CUR_BNK+eax],dx
	push es
	lea esi,[SVGA_REGS+eax]	; SVGA_Regs
	sub esp,128
	mov edi,esp
	mov ecx,13
	cld
	REP movsd
	mov ax,[___djgpp_ds_alias]
	mov edi,esp
	mov es,ax
	mov [20+edi],dx
	xor ebx,ebx
	xor ecx,ecx
	mov ax,0x0301
	int 0x31
	jnc No_Err
	mov [___dpmi_error],ax
	mov eax,-1
No_Err:
	add esp,128
	pop es
	pop edi
	pop esi
	pop ebx
%endif		; __WATCOMC__
	ret

;///////////////////////////////////////////////////////////////

%ifdef USE_S3				; S3 support. *DISABLED*

globl Set_Bank_S3
align4
Set_Bank_S3:
	push ebx
	push esi
	push edi
	mov eax,[16+esp]	; VBE
	mov bx,[20+esp]	; Nb
	mov [208+eax],bx
	mov dx,0x03d4
	mov ax,0x4838
	out  dx,ax		; S3_EXT_ON
	mov cl,0x31		; Enable write to bank regs.
	mov al,cl
	out dx,al
	inc dx
	in al,dx
	mov ah,al
	dec dx
	mov al,cl
	or ah,0x09
	out dx,ax
	mov cl,0x35 		; Set page bx
	mov al,cl
	out dx,al
	inc dx
	in al,dx
	and al,0xF0
	or al,bl
	mov ah,al
	dec dx
	mov al,cl
	out dx,ax
	mov ax,0x0038		; S3_EXT_OFF
	out dx,ax
        pop edi 
        pop esi
        pop ebx
        ret

%endif	; USE_S3

;///////////////////////////////////////////////////////////////

