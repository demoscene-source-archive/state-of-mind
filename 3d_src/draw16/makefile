#
# Makefile
#

#################### LIBS ##########################################

CFLAGS += -I../../include

#################### 3D Lib ########################################

draw16.o: $(LOOPS_16)
	$(_LD_) draw16.o $(LOOPS_16)
draw16.a: $(LOOPS_16)
	$(AR) draw16.a $(LOOPS_16)

####################################################################
####################################################################

WINC = -i..\\..\\include
WOBJECTS = line16.obj render16.obj flare16b.obj	\
 loops_16.obj loopt_16.obj flare16.obj bilin16.obj

wat:	$(WOBJECTS)
	wlib -q -c -n w3d_16.lib +line16 +render16 +flare16b +flare16 
	wlib -q -c w3d_16.lib +loops_16 +loopt_16 +bilin16.obj
	move w3d_16.lib ..

####################################################################
####################################################################

ifeq ($(TARGET),DOS)
NFLAGS = -f coff -p../../include/nasmdj.h
endif
ifeq ($(TARGET),LNX)
NFLAGS = -f elf -p../../include/nasmlnx.h
endif
ifeq ($(TARGET),WATC)
NFLAGS = -f obj -d__WATCOMC__ -p../../include/nasmwc.h
endif

####################################################################
####################################################################

loops_16.o: loops_16.asm
	$(_NASM_) $(NFLAGS) $< -o $@
flare16.o: flare16.asm
	$(_NASM_) $(NFLAGS) $< -o $@
loopt_16.o: loopt_16.asm
	$(_NASM_) $(NFLAGS) $< -o $@


loops_16.obj: loops_16.asm
	$(_NASM_) $(NFLAGS) $< -o $@
flare16.obj: flare16.asm
	$(_NASM_) $(NFLAGS) $< -o $@
loopt_16.obj: loopt_16.asm
	$(_NASM_) $(NFLAGS) $< -o $@



####################################################################
####################################################################

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################

clean:
	rm -f *~ *.o *.a *.obj *.bak core toto $(LOOPS_16)

####################################################################
