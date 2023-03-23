globl MP3_Init_FPU

TEXT 

MP3_Init_FPU:

		push eax
		sub esp,4
		fstcw [esp]  	; Put FPU in 32-bit prec mode 
      mov ax,[esp]
      and ax,0xF03F        	; switch on 
		or ax,0x0800		; Up
		or ax,0x003f		; mask exceptions
   	mov [esp],ax
      fldcw [esp]
		add esp,4
		pop eax

		ret

;//////////////////////////////////////////////////////////////////////////

