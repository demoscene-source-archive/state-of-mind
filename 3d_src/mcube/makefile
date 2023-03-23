#
# Makefile
#

#################### LIBS ##########################################

CFLAGS += -I../../include

####################################################################

mc.o: $(MCUBE_OBJ)
	$(_LD_) mc.o $(MCUBE_OBJ)
mc.a: $(MCUBE_OBJ)
	$(AR) mc.a $(MCUBE_OBJ)

####################################################################
####################################################################

ifneq ($(TARGET),UNIX)

mcube_a.o: mcube_a.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mcube_a3.o: mcube_a3.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mcube3.o: mcube3.c
	$(CC) $(CFLAGS) -c $< -o $@

mcube_a.obj: mcube_a.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mcube_a3.obj: mcube_a3.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mcube_a2.obj: mcube_a2.c
	$(WAT) $(WFLAGS) $(WINC) $< -fo=mcube_a2.obj -nm=mcube_a2
mcube3.obj: mcube3.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

endif

####################################################################
####################################################################

WINC = -i..\\..\\include
WOBJECTS = blob.obj mcube.obj mcube_a2.obj mcube_a3.obj mcube_a.obj \
 mcube3.obj

wat:	$(WOBJECTS)
	wlib -q -c -n wmcube.lib +blob +mcube +mcube_a2 +mcube_a3 +mcube_a +mcube3
	move wmcube.lib ..

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

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################

clean:
	rm -f *~ *.o *.a *.obj *.bak core toto $(MCUBE_OBJ)

####################################################################
