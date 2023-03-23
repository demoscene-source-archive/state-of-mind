#
# Makefile
#

#################### LIBS ##########################################

CFLAGS += -I../../include

#################### 3D Lib ########################################

shaders.o: $(SHADERS_OBJ)
	$(_LD_) shaders.o $(SHADERS_OBJ)
shaders.a: $(SHADERS_OBJ)
	$(AR) shaders.a $(SHADERS_OBJ)

####################################################################

WINC = -i..\\..\\include
WOBJECTS = raster.obj shadow.obj vshadow.obj vshadow2.obj vshadow3.obj   \
        vshadow4.obj sh_flat.obj sh_line.obj sh_shdw.obj sh_grd.obj sh_env.obj \
        sh_uv.obj sh_uvc.obj sh_z.obj sh_z2.obj sh_bump.obj line.obj line2.obj \
        raster2.obj loops1.obj loops2.obj sh_vshdw.obj vshadow5.obj

wat:	$(WOBJECTS)
	wlib -q -c -n w3d_2.lib +raster2 +shadow +vshadow +vshadow2 +vshadow4
	wlib -q -c w3d_2.lib +vshadow3 +sh_flat +sh_line +sh_shdw +sh_grd
	wlib -q -c w3d_2.lib +sh_uv +sh_vshdw +vshadow5 +sh_vshdw
	wlib -q -c w3d_2.lib +sh_env +sh_uvc +sh_z +sh_z2 +sh_bump
	wlib -q -c w3d_2.lib +line +line2 +raster +loops1 +loops2
	rem rem wlib -q -c w3d_2.lib +3dfx
	move w3d_2.lib ..

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

raster2.obj: raster.c
	$(WAT) $(WFLAGS) $(WINC) -fo=raster2.obj -nm=raster2 raster.c

raster2.o: raster.c
	$(CC) $(CFLAGS) -I../../include -c -o raster2.o raster.c

loops1.obj: loops1.asm
	$(_NASM_) $(NFLAGS) $< -o $@
raster.obj: raster.asm
	$(_NASM_) $(NFLAGS) $< -o $@


ifeq ($(TARGET),UNIX)
raster.o: raster.c
	$(CC) $(CFLAGS) -I../../include -c -o raster.o raster.c
else
raster.o: raster.asm
	$(_NASM_) $(NFLAGS) $< -o $@
loops1.o: loops1.asm
	$(_NASM_) $(NFLAGS) $< -o $@
endif

####################################################################
####################################################################

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################
####################################################################

clean:
	rm -f *~ *.o *.a *.obj *.bak core toto $(SHADERS_OBJ)

####################################################################
