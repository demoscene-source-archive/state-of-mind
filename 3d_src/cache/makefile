#
# Makefile
#
#################### LIBS ##########################################

CFLAGS += -I../../include

#################### 3D Lib ########################################

cache.o: $(CACHE_OBJ)
	$(_LD_) cache.o $(CACHE_OBJ)
cache.a: $(CACHE_OBJ)
	$(AR) cache.a $(CACHE_OBJ)

####################################################################

WINC = -i..\\..\\include
WOBJECTS = mip_cst.obj mat.obj cache1.obj cache2.obj cache3.obj \
	ld_mat.obj sv_mat.obj ld_mat2.obj sv_mat2.obj ld_mat3.obj sv_mat3.obj \
	sv_mat4.obj

wat:	$(WOBJECTS)
	wlib -q -c -n wcache.lib +mat+cache1 +cache2 +cache3
	wlib -q -c wcache.lib +ld_mat +sv_mat +ld_mat2 +sv_mat2
	wlib -q -c wcache.lib +ld_mat3 +sv_mat3 +sv_mat4 +mip_cst
	move wcache.lib ..

####################################################################

ifeq ($(TARGET),DOS)
NFLAGS = -f coff -p../include/nasmdj.h
endif
ifeq ($(TARGET),LNX)
NFLAGS = -f elf -p../include/nasmlnx.h
endif
ifeq ($(TARGET),WATC)
NFLAGS = -f obj -d__WATCOMC__ -p../include/nasmwc.h
endif

####################################################################
####################################################################

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################
####################################################################

clean:
	rm -f *~ *.o *.a *.obj *.bak core toto $(SHADERS_OBJ)

####################################################################
