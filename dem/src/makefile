# Makefile
##################

####################################################################
####################################################################

demo.o:	demo.c
#	$(CC) $(CFLAGS) -DVERSION=$(_VERSION_) -DSONG_NAME=$(_SONG_NAME_) -DDAT_FILE_NAME=$(_DAT_FILE_NAME_) -o $*.o -c $*.c
	$(CC) $(CFLAGS) -o $*.o -c $*.c

############################################################

ifeq ($(TARGET),DOS)

zblur2.o: zblur2.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h zblur2.asm -o zblur2.o
mix2.o: mix2.asm
	$(_NASM_)  -f coff -p../../include/nasmdj.h mix2.asm -o mix2.o

endif

############################################################

ifeq ($(TARGET),LNX)

zblur2.o: zblur2.asm
	$(_NASM_)  -f elf -p../../include/nasmlnx.h zblur2.asm -o zblur2.o
mix2.o: mix2.asm
	$(_NASM_)  -f elf -p../../include/nasmlnx.h mix2.asm -o mix2.o

endif

############################################################

all:	$(OBJ) 
	$(AR) $(LIB_BASE) $(OBJ)
	$(RANL) $(LIB_BASE)
	mv $(LIB_BASE) ../bin

clean:
	rm -f *~ *.o *.bak *.obj core toto sta?????

#################################################################

