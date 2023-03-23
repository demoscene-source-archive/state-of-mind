#
# Makefile
#

#################### LIBS ##########################################

CFLAGS += -I../../include

#################### 3D Lib ########################################

draw8.o: $(LOOPS_8)
	$(_LD_) draw8.o $(LOOPS_8)
draw8.a: $(LOOPS_8)
	$(AR) draw8.a $(LOOPS_8)

####################################################################

WINC = -i..\\..\\include
WOBJECTS = line8.obj render8.obj flare8b.obj	\
 loops_8.obj loopt_8.obj flare8.obj
wat:	$(WOBJECTS)
	wlib -q -c -n w3d_8.lib +line8 +render8 +flare8b +flare8 
	wlib -q -c w3d_8.lib +loops_8 +loopt_8
	move w3d_8.lib ..

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

loops_8.o: loops_8.asm
	$(_NASM_) $(NFLAGS) $< -o $@
flare8.o: flare8.asm
	$(_NASM_) $(NFLAGS) $< -o $@
loopt_8.o: loopt_8.asm
	$(_NASM_) $(NFLAGS) $< -o $@


loops_8.obj: loops_8.asm
	$(_NASM_) $(NFLAGS) $< -o $@
flare8.obj: flare8.asm
	$(_NASM_) $(NFLAGS) $< -o $@
loopt_8.obj: loopt_8.asm
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
