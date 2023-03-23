#
# Makefile
#
##################

####################################################################


####################################################################

ifeq ($(TARGET),DOS)

mix.o: mix.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h mix.asm -o mix.o
mix2.o: mix2.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h mix2.asm -o mix2.o
mix3.o: mix3.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h mix3.asm -o mix3.o
mix777.o: mix777.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h mix777.asm -o mix777.o
mixdsp.o: mixdsp.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h mixdsp.asm -o mixdsp.o
paste.o: paste.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h paste.asm -o paste.o
bilin.o: bilin.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h bilin.asm -o bilin.o
anim.o: anim.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h anim.asm -o anim.o
spr2.o: spr2.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h spr2.asm -o spr2.o
#flare16s.o: flare16s.asm
#	$(_NASM_)  -f coff -p../../include/nasmdj.h flare16s.asm -o flare16s.o

endif

############################################################

ifeq ($(TARGET),LNX)

mix.o: mix.asm
	$(_NASM_) -f elf -p../../include/nasmlnx.h mix.asm -o mix.o
mix2.o: mix2.asm
	$(_NASM_) -f elf -p../../include/nasmlnx.h mix2.asm -o mix2.o
mix3.o: mix3.asm
	$(_NASM_) -f elf -p../../include/nasmlnx.h mix3.asm -o mix3.o
mix777.o: mix777.asm
	$(_NASM_) -f elf -p../../include/nasmlnx.h mix777.asm -o mix777.o
mixdsp.o: mixdsp.asm
	$(_NASM_) -f elf -p../../include/nasmlnx.h mixdsp.asm -o mixdsp.o
paste.o: paste.asm
	$(_NASM_) -f elf -p../../include/nasmlnx.h paste.asm -o paste.o
bilin.o: bilin.asm
	$(_NASM_) -f elf -p../../include/nasmlnx.h bilin.asm -o bilin.o
anim.o: anim.asm
	$(_NASM_) -f elf -p../../include/nasmlnx.h anim.asm -o anim.o
spr2.o: spr2.asm
	$(_NASM_) -f elf -p../../include/nasmlnx.h spr2.asm -o spr2.o
#flare16s.o: flare16s.asm
#	$(_NASM_) -f elf -p../../include/nasmlnx.h flare16s.asm -o flare16s.o

endif

############################################################

all:	$(FX1_OBJ) 
	$(AR) $(LIB_FX1) $(FX1_OBJ)
	$(RANL) $(LIB_FX1)
	mv $(LIB_FX1) ../bin

clean:
	rm -f *~ *.o *.bak *.obj core toto sta?????

#################################################################

