#
# Makefile
#

#################### LIBS ##########################################

CFLAGS += -I../../include

#################### 3D Lib ########################################

draw88.o: $(LOOPS_88)
	$(_LD_) draw88.o $(LOOPS_88)
draw88.a: $(LOOPS_88)
	$(AR) draw88.a $(LOOPS_88)

####################################################################

WINC = -i..\\..\\include
WOBJECTS = line88.obj render88.obj flare88b.obj	\
 loops_88.obj loopt_88.obj flare88.obj

wat:	$(WOBJECTS)
	wlib -q -c -n w3d_88.lib +line88 +render88 +flare88b +flare88 
	wlib -q -c w3d_88.lib +loops_88 +loopt_88 
	move w3d_88.lib ..

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

loops_88.o: loops_88.asm
	$(_NASM_) $(NFLAGS) $< -o $@
flare88.o: flare88.asm
	$(_NASM_) $(NFLAGS) $< -o $@
loopt_88.o: loopt_88.asm
	$(_NASM_) $(NFLAGS) $< -o $@

loops_88.obj: loops_88.asm
	$(_NASM_) $(NFLAGS) $< -o $@
flare88.obj: flare88.asm
	$(_NASM_) $(NFLAGS) $< -o $@
loopt_88.obj: loopt_88.asm
	$(_NASM_) $(NFLAGS) $< -o $@

####################################################################
####################################################################

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################
####################################################################

clean:
	rm -f *~ *.o *.a *.obj *.bak core toto $(LOOPS_8)

####################################################################
