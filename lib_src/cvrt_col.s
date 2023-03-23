	.file	"cvrt_col.S"
	.version	"01.01"
gcc2_compiled.:
.text
//////////////////////////////////////////////////////////////////
	.align 4
.globl CMap8_To_RGB32
	.type	 CMap8_To_RGB32,@function
CMap8_To_RGB32:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 12(%esp),%eax
	movl 16(%esp),%esi
	movl 20(%esp),%ebx
	movl (%eax),%ecx
	addl $1036,%ecx
	movl 24(%esp),%edx
	decl %edx
	js .L8
	xorl %eax,%eax
	.align 4
.L9:
	movb (%edx,%ebx),%al
	movl (%ecx,%eax,4),%edi
	movl %edi,(%esi,%edx,4)
	decl %edx
	jns .L9
.L8:
	popl %ebx
	popl %esi
	popl %edi
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl CMap8_To_RGB24
	.type	 CMap8_To_RGB24,@function
CMap8_To_RGB24:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 16(%esp),%eax
	movl 20(%esp),%ecx
	movl 24(%esp),%esi
	movl (%eax),%edi
	addl $1036,%edi
	movl 28(%esp),%ebx
	decl %ebx
	js .L18
	.align 4
.L19:
	movzbl (%esi),%eax
	leal (%edi,%eax,4),%eax
	incl %esi
	movb (%eax),%dl
	movb %dl,(%ecx)
	incl %eax
	incl %ecx
	movb (%eax),%dl
	movb %dl,(%ecx)
	incl %ecx
	movb 1(%eax),%al
	movb %al,(%ecx)
	incl %ecx
	decl %ebx
	jns .L19
.L18:
	popl %ebx
	popl %esi
	popl %edi
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl CMap8_To_RGB16
	.type	 CMap8_To_RGB16,@function
CMap8_To_RGB16:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 12(%esp),%eax
	movl 16(%esp),%edi
	movl 20(%esp),%esi
	movl (%eax),%ecx
	addl $1036,%ecx
	movl 24(%esp),%edx
	decl %edx
	js .L28
	xorl %eax,%eax
	.align 4
.L29:
	movb (%edx,%esi),%al
	movw (%ecx,%eax,4),%bx
	movw %bx,(%edi,%edx,2)
	decl %edx
	jns .L29
.L28:
	popl %ebx
	popl %esi
	popl %edi
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB8_To_RGB8
	.type	 RGB8_To_RGB8,@function
RGB8_To_RGB8:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 16(%esp),%eax
	movl 20(%esp),%edi
	movl 24(%esp),%esi
	movl (%eax),%ebx
	addl $8,%ebx
	movl 28(%esp),%ecx
	decl %ecx
	js .L38
	.align 4
.L39:
	movb (%esi),%al
	incl %esi
	andl $255,%eax
	movb 1024(%ebx,%eax,4),%dl
	orb (%ebx,%eax,4),%dl
	orb 3072(%ebx,%eax,4),%dl
	movb %dl,(%edi)
	incl %edi
	decl %ecx
	jns .L39
.L38:
	popl %ebx
	popl %esi
	popl %edi
	ret

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB8_To_RGB16
	.type	 RGB8_To_RGB16,@function
RGB8_To_RGB16:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 16(%esp),%eax
	movl 24(%esp),%edi
	movl (%eax),%ecx
	addl $8,%ecx
	movl 20(%esp),%esi
	movl 28(%esp),%ebx
	decl %ebx
	js .L48
	.align 4
.L49:
	movb (%edi),%al
	incl %edi
	andl $255,%eax
	movw 1024(%ecx,%eax,4),%dx
	orw (%ecx,%eax,4),%dx
	orw 2048(%ecx,%eax,4),%dx
	orw 3072(%ecx,%eax,4),%dx
	movw %dx,(%esi)
	addl $2,%esi
	decl %ebx
	jns .L49
.L48:
	popl %ebx
	popl %esi
	popl %edi
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB8_To_RGB24
	.type	 RGB8_To_RGB24,@function
RGB8_To_RGB24:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 16(%esp),%eax
	movl 20(%esp),%ecx
	movl 24(%esp),%edi
	movl (%eax),%esi
	addl $8,%esi
	movl 28(%esp),%ebx
	decl %ebx
	js .L58
	.align 4
.L59:
	movb (%edi),%al
	incl %edi
	andl $255,%eax
	movl (%esi,%eax,4),%edx
	orl 1024(%esi,%eax,4),%edx
	orl 3072(%esi,%eax,4),%edx
	movl %edx,%eax
	shrl $16,%eax
	movb %al,(%ecx)
	incl %ecx
	movl %edx,%eax
	shrl $8,%eax
	movb %al,(%ecx)
	incl %ecx
	movb %dl,%al
	movb %al,(%ecx)
	incl %ecx
	decl %ebx
	jns .L59
.L58:
	popl %ebx
	popl %esi
	popl %edi
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB8_To_RGB32
	.type	 RGB8_To_RGB32,@function
RGB8_To_RGB32:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 16(%esp),%eax
	movl 24(%esp),%edi
	movl (%eax),%esi
	addl $8,%esi
	movl 20(%esp),%ebx
	movl 28(%esp),%ecx
	decl %ecx
	js .L68
	.align 4
.L69:
	movb (%edi),%al
	incl %edi
	andl $255,%eax
	movl (%esi,%eax,4),%edx
	orl 1024(%esi,%eax,4),%edx
	orl 3072(%esi,%eax,4),%edx
	movl %edx,(%ebx)
	addl $4,%ebx
	decl %ecx
	jns .L69
.L68:
	popl %ebx
	popl %esi
	popl %edi
	ret
.Lfe7:
	.size	 RGB8_To_RGB32,.Lfe7-RGB8_To_RGB32
	.align 4
.globl RGB16_To_RGB8
	.type	 RGB16_To_RGB8,@function
RGB16_To_RGB8:
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 20(%esp),%eax
	movl 24(%esp),%ebp
	movl (%eax),%ebx
	addl $8,%ebx
	movl 28(%esp),%edi
	movl 32(%esp),%esi
	decl %esi
	js .L78
	.align 4
.L79:
	movw (%edi),%dx
	addl $2,%edi
	movl %edx,%eax
	shrw $8,%ax
	andl $65535,%eax
	movb 1024(%ebx,%eax,4),%cl
	orb (%ebx,%eax,4),%cl
	movzbl %dl,%eax
	orb 2048(%ebx,%eax,4),%cl
	orb 3072(%ebx,%eax,4),%cl
	movb %cl,(%ebp)
	incl %ebp
	decl %esi
	jns .L79
.L78:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB16_To_RGB16
	.type	 RGB16_To_RGB16,@function
RGB16_To_RGB16:
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 20(%esp),%eax
	movl (%eax),%ebx
	addl $8,%ebx
	movl 28(%esp),%ebp
	movl 24(%esp),%edi
	movl 32(%esp),%esi
	decl %esi
	js .L88
	.align 4
.L89:
	movw (%ebp),%dx
	addl $2,%ebp
	movl %edx,%eax
	shrw $8,%ax
	andl $65535,%eax
	movw 1024(%ebx,%eax,4),%cx
	orw (%ebx,%eax,4),%cx
	movzbl %dl,%eax
	orw 2048(%ebx,%eax,4),%cx
	orw 3072(%ebx,%eax,4),%cx
	movw %cx,(%edi)
	addl $2,%edi
	decl %esi
	jns .L89
.L88:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB16_To_RGB24
	.type	 RGB16_To_RGB24,@function
RGB16_To_RGB24:
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 20(%esp),%eax
	movl 24(%esp),%ebx
	movl (%eax),%esi
	addl $8,%esi
	movl 28(%esp),%ebp
	movl 32(%esp),%edi
	decl %edi
	js .L98
	.align 4
.L99:
	movw (%ebp),%dx
	addl $2,%ebp
	movl %edx,%eax
	shrw $8,%ax
	andl $65535,%eax
	movl (%esi,%eax,4),%ecx
	orl 1024(%esi,%eax,4),%ecx
	movzbl %dl,%eax
	orl 2048(%esi,%eax,4),%ecx
	orl 3072(%esi,%eax,4),%ecx
	movl %ecx,%eax
	shrl $16,%eax
	movb %al,(%ebx)
	incl %ebx
	movl %ecx,%eax
	shrl $8,%eax
	movb %al,(%ebx)
	incl %ebx
	movb %cl,%al
	movb %al,(%ebx)
	incl %ebx
	decl %edi
	jns .L99
.L98:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB16_To_RGB32
	.type	 RGB16_To_RGB32,@function
RGB16_To_RGB32:
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 20(%esp),%eax
	movl (%eax),%ebx
	addl $8,%ebx
	movl 24(%esp),%ebp
	movl 28(%esp),%edi
	movl 32(%esp),%esi
	decl %esi
	js .L108
	.align 4
.L109:
	movw (%edi),%dx
	addl $2,%edi
	movl %edx,%eax
	shrw $8,%ax
	andl $65535,%eax
	movl (%ebx,%eax,4),%ecx
	orl 1024(%ebx,%eax,4),%ecx
	movzbl %dl,%eax
	orl 2048(%ebx,%eax,4),%ecx
	orl 3072(%ebx,%eax,4),%ecx
	movl %ecx,(%ebp)
	addl $4,%ebp
	decl %esi
	jns .L109
.L108:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB24_To_RGB8
	.type	 RGB24_To_RGB8,@function
RGB24_To_RGB8:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 16(%esp),%eax
	movl 20(%esp),%edi
	movl 24(%esp),%ecx
	movl (%eax),%esi
	addl $8,%esi
	movl 28(%esp),%ebx
	decl %ebx
	js .L118
	.align 4
.L119:
	movzbl (%ecx),%eax
	incl %ecx
	movzbl (%ecx),%edx
	movb 1024(%esi,%edx,4),%dl
	orb 3072(%esi,%eax,4),%dl
	incl %ecx
	movzbl (%ecx),%eax
	incl %ecx
	orb (%esi,%eax,4),%dl
	movb %dl,(%edi)
	incl %edi
	decl %ebx
	jns .L119
.L118:
	popl %ebx
	popl %esi
	popl %edi
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB24_To_RGB16
	.type	 RGB24_To_RGB16,@function
RGB24_To_RGB16:
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 20(%esp),%eax
	movl 28(%esp),%ecx
	movl 24(%esp),%ebp
	movl (%eax),%ebx
	addl $8,%ebx
	movl 32(%esp),%esi
	decl %esi
	js .L128
	.align 4
.L129:
	movzbl (%ecx),%edx
	incl %ecx
	movb (%ecx),%al
	incl %ecx
	andl $255,%eax
	movw 2048(%ebx,%eax,4),%di
	orw 3072(%ebx,%edx,4),%di
	movl %edi,%edx
	orw 1024(%ebx,%eax,4),%dx
	movzbl (%ecx),%eax
	orw (%ebx,%eax,4),%dx
	movw %dx,(%ebp)
	incl %ecx
	addl $2,%ebp
	decl %esi
	jns .L129
.L128:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB24_To_RGB32
	.type	 RGB24_To_RGB32,@function
RGB24_To_RGB32:
	pushl %ebx
	movl 12(%esp),%edx
	movl 16(%esp),%ecx
	movl 20(%esp),%ebx
	decl %ebx
	js .L138
	.align 4
.L139:
	movb (%ecx),%al
	movb %al,(%edx)
	incl %ecx
	incl %edx
	movb (%ecx),%al
	movb %al,(%edx)
	incl %ecx
	incl %edx
	movb (%ecx),%al
	movb %al,(%edx)
	incl %ecx
	addl $2,%edx
	decl %ebx
	jns .L139
.L138:
	popl %ebx
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB32_To_RGB8
	.type	 RGB32_To_RGB8,@function
RGB32_To_RGB8:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 16(%esp),%eax
	movl 20(%esp),%edi
	movl 24(%esp),%ecx
	movl (%eax),%esi
	addl $8,%esi
	movl 28(%esp),%ebx
	decl %ebx
	js .L148
	.align 4
.L149:
	movzbl (%ecx),%eax
	incl %ecx
	movzbl (%ecx),%edx
	movb 1024(%esi,%edx,4),%dl
	orb 3072(%esi,%eax,4),%dl
	incl %ecx
	movzbl (%ecx),%eax
	orb (%esi,%eax,4),%dl
	movb %dl,(%edi)
	incl %edi
	addl $2,%ecx
	decl %ebx
	jns .L149
.L148:
	popl %ebx
	popl %esi
	popl %edi
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB32_To_RGB16
	.type	 RGB32_To_RGB16,@function
RGB32_To_RGB16:
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 20(%esp),%eax
	movl 28(%esp),%ecx
	movl (%eax),%ebx
	addl $8,%ebx
	movl 24(%esp),%ebp
	movl 32(%esp),%esi
	decl %esi
	js .L158
	.align 4
.L159:
	movzbl (%ecx),%edx
	incl %ecx
	movb (%ecx),%al
	incl %ecx
	andl $255,%eax
	movw 2048(%ebx,%eax,4),%di
	orw 3072(%ebx,%edx,4),%di
	movl %edi,%edx
	orw 1024(%ebx,%eax,4),%dx
	movzbl (%ecx),%eax
	orw (%ebx,%eax,4),%dx
	movw %dx,(%ebp)
	addl $2,%ebp
	addl $2,%ecx
	decl %esi
	jns .L159
.L158:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB32_To_RGB24
	.type	 RGB32_To_RGB24,@function
RGB32_To_RGB24:
	pushl %ebx
	movl 12(%esp),%ecx
	movl 16(%esp),%edx
	movl 20(%esp),%ebx
	decl %ebx
	js .L168
	.align 4
.L169:
	movb (%edx),%al
	movb %al,(%ecx)
	incl %edx
	incl %ecx
	movb (%edx),%al
	movb %al,(%ecx)
	incl %edx
	incl %ecx
	movb (%edx),%al
	movb %al,(%ecx)
	incl %ecx
	addl $2,%edx
	decl %ebx
	jns .L169
.L168:
	popl %ebx
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB8_To_CMap8
	.type	 RGB8_To_CMap8,@function
RGB8_To_CMap8:
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 20(%esp),%eax
	movl 24(%esp),%edi
	movl 28(%esp),%esi
	movl (%eax),%eax
	leal 1036(%eax),%ebx
	leal 12(%eax),%ebp
	movl 32(%esp),%ecx
	decl %ecx
	js .L178
	.align 4
.L179:
	movb (%esi),%al
	incl %esi
	andl $255,%eax
	movb (%ebx,%eax,4),%dl
	orb 1(%ebx,%eax,4),%dl
	orb 3(%ebx,%eax,4),%dl
	movzbl %dl,%eax
	movb 3(%ebp,%eax,4),%al
	movb %al,(%edi)
	incl %edi
	decl %ecx
	jns .L179
.L178:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB16_To_CMap8
	.type	 RGB16_To_CMap8,@function
RGB16_To_CMap8:
	subl $8,%esp
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 28(%esp),%eax
	movl 32(%esp),%ebp
	movl (%eax),%eax
	leal 1036(%eax),%ebx
	addl $12,%eax
	movl %eax,20(%esp)
	movl 36(%esp),%edi
	movl 40(%esp),%esi
	decl %esi
	js .L188
	.align 4
.L189:
	movw (%edi),%dx
	movw %dx,16(%esp)
	addl $2,%edi
	movl %edx,%eax
	shrw $8,%ax
	andl $65535,%eax
	movb (%ebx,%eax,4),%cl
	orb 1(%ebx,%eax,4),%cl
	movzbl 16(%esp),%eax
	orb 2(%ebx,%eax,4),%cl
	orb 3(%ebx,%eax,4),%cl
	movzbl %cl,%eax
	movl 20(%esp),%edx
	movb 3(%edx,%eax,4),%al
	movb %al,(%ebp)
	incl %ebp
	decl %esi
	jns .L189
.L188:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	addl $8,%esp
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB24_To_CMap8
	.type	 RGB24_To_CMap8,@function
RGB24_To_CMap8:
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 20(%esp),%eax
	movl 24(%esp),%edi
	movl 28(%esp),%ecx
	movl (%eax),%eax
	leal 1036(%eax),%esi
	leal 12(%eax),%ebp
	movl 32(%esp),%ebx
	decl %ebx
	js .L198
	.align 4
.L199:
	movzbl (%ecx),%edx
	incl %ecx
	movzbl (%ecx),%eax
	movb 3(%esi,%edx,4),%dl
	orb 1(%esi,%eax,4),%dl
	incl %ecx
	movzbl (%ecx),%eax
	orb (%esi,%eax,4),%dl
	incl %ecx
	movzbl %dl,%eax
	movb 3(%ebp,%eax,4),%al
	movb %al,(%edi)
	incl %edi
	decl %ebx
	jns .L199
.L198:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

//////////////////////////////////////////////////////////////////
	.align 4
.globl RGB32_To_CMap8
	.type	 RGB32_To_CMap8,@function
RGB32_To_CMap8:
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 20(%esp),%eax
	movl 24(%esp),%edi
	movl 28(%esp),%ecx
	movl (%eax),%eax
	leal 1036(%eax),%esi
	leal 12(%eax),%ebp
	movl 32(%esp),%ebx
	decl %ebx
	js .L208
	.align 4
.L209:
	movzbl (%ecx),%edx
	incl %ecx
	movzbl (%ecx),%eax
	movb 3(%esi,%edx,4),%dl
	orb 1(%esi,%eax,4),%dl
	incl %ecx
	movzbl (%ecx),%eax
	orb (%esi,%eax,4),%dl
	movzbl %dl,%eax
	movb 3(%ebp,%eax,4),%al
	movb %al,(%edi)
	incl %edi
	addl $2,%ecx
	decl %ebx
	jns .L209
.L208:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

//////////////////////////////////////////////////////////////////
	.ident	"GCC: (GNU) 2.7.2"
