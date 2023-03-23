#
# Makefile
#
#############################################

.EXPORT_ALL_VARIABLES:

#################### LIBS ##########################################

SHARED = lib3d.so

#################### 3D Lib ########################################


3D_OBJ   = node.o tree.o world.o wrld_rsc.o transf.o camera.o \
	 light.o group.o box.o mesh.o vmesh.o 3d_io.o \
	parts.o clip.o shader.o poly.o edges.o flare.o wld_info.o 	\
	quat.o vista.o vista2.o roto.o frustum.o \
	move.o zlight.o sv_world.o convex.o normal.o  \
	optimesh.o optim2.o sort.o anim.o mk.o mk2.o tables.o render.o \
	flare16.o
#array.o

CACHE_OBJ = mip_cst.o mat.o cache1.o cache2.o cache3.o \
	ld_mat.o sv_mat.o  sv_mat2.o ld_mat3.o sv_mat3.o \
	sv_mat4.o ld_mat2.o ld_mat2b.o
MCUBE_OBJ = mcube.o mcube_a2.o mcube3.o
LOOPS_16  = line16.o render16.o flare16b.o bilin16.o
LOOPS_8   = line8.o render8.o flare8b.o
LOOPS_88  = line88.o render88.o flare88b.o
SHADERS_OBJ = raster.o shadow.o vshadow.o vshadow2.o vshadow3.o   \
        vshadow4.o sh_flat.o sh_line.o sh_shdw.o sh_grd.o sh_env.o \
        sh_uv.o sh_uvc.o sh_z.o sh_z2.o sh_bump.o line.o line2.o \
        raster2.o loops2.o sh_vshdw.o vshadow5.o 


ifneq ($(TARGET),UNIX)

3D_OBJ += transf2.o sort2.o vista3.o normal2.o flare16s.o
MCUBE_OBJ += mcube_a.o mcube_a3.o
LOOPS_16 += loops_16.o loopt_16.o flare16.o
LOOPS_8 += loops_8.o loopt_8.o flare8.o
LOOPS_88 += loops_88.o loopt_88.o
#       flare88.o
SHADERS_OBJ += loops1.o

else

LOOPS_16  += loops216.o
LOOPS_8   += loops28.o
LOOPS_88  += loops288.o

ifdef USE_3DFX
SHADERS_OBJ += 3dfx.o
endif

endif


####################################################################
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

ifeq ($(TARGET),UNIX)

DRAW16_LIB = $(LOOPS_16)
DRAW8_LIB = $(LOOPS_8)
DRAW88_LIB = $(LOOPS_88)
SHADER_LIB = $(SHADERS_OBJ)
MCUBE_LIB = $(MCUBE_OBJ)
CACHE_LIB = $(CACHE_OBJ)
SUBLIB =  cache/*.o draw8/*.o draw88/*.o draw16/*.o shaders/*.o mcube/*.o

#DRAW16_LIB = draw16.o
#DRAW8_LIB = draw8.o
#DRAW88_LIB = draw88.o
#SHADER_LIB = shaders.o
#CACHE_LIB = cache.o
#MCUBE_LIB = mc.o
#SUBLIB =  draw8/$(DRAW8_LIB) draw88/$(DRAW88_LIB) cache/$(CACHE_LIB) \
#  draw16/$(DRAW16_LIB) shaders/$(SHADER_LIB) mcube/$(MCUBE_LIB)

endif

####################################################################

ifeq ($(TARGET),DOS)

DRAW16_LIB = $(LOOPS_16)
DRAW8_LIB = $(LOOPS_8)
DRAW88_LIB = $(LOOPS_88)
SHADER_LIB = $(SHADERS_OBJ)
MCUBE_LIB = $(MCUBE_OBJ)
CACHE_LIB = $(CACHE_OBJ)
SUBLIB =  cache/*.o draw8/*.o draw88/*.o draw16/*.o shaders/*.o mcube/*.o

endif

####################################################################

ifeq ($(TARGET),LNX)

DRAW16_LIB = $(LOOPS_16)
DRAW8_LIB = $(LOOPS_8)
DRAW88_LIB = $(LOOPS_88)
SHADER_LIB = $(SHADERS_OBJ)
MCUBE_LIB = $(MCUBE_OBJ)
CACHE_LIB = $(CACHE_OBJ)
SUBLIB =  cache/*.o draw8/*.o draw88/*.o draw16/*.o shaders/*.o mcube/*.o

#DRAW16_LIB = draw16.o
#DRAW8_LIB = draw8.o
#DRAW88_LIB = draw88.o
#SHADER_LIB = shaders.o
#CACHE_LIB = cache.o
#MCUBE_LIB = mc.o
#SUBLIB =  cache/$(CACHE_LIB) draw8/$(DRAW8_LIB) draw88/$(DRAW88_LIB)	\
#  draw16/$(DRAW16_LIB) shaders/$(SHADER_LIB) mcube/$(MCUBE_LIB)

endif

####################################################################

normal.o: normal.c
	$(CC) $(CFLAGS) -c $< -o $@
transf2.o: transf.c
	$(CC) $(CFLAGS) -c $< -o $@

normal2.obj: normal.c
	$(WAT) $(WFLAGS) $(WINC) $< -fo=normal2.obj -nm=normal2
transf2.obj: transf.c
	$(WAT) $(WFLAGS) $(WINC) $< -fo=transf2.obj -nm=transf2

ifneq ($(TARGET),UNIX)

sort2.o: sort2.asm
	$(_NASM_) $(NFLAGS) $< -o $@
vista3.o: vista3.asm
	$(_NASM_) $(NFLAGS) $< -o $@
normal2.o: normal.asm
	$(_NASM_) $(NFLAGS) $< -o $@
transf.o: transf.asm
	$(_NASM_) $(NFLAGS) $< -o $@
flare16s.o: flare16s.asm
	$(_NASM_)  $(NFLAGS) $< -o $@

####################################################################

sort2.obj: sort2.asm
	$(_NASM_) $(NFLAGS) $< -o $@
vista3.obj: vista3.asm
	$(_NASM_) $(NFLAGS) $< -o $@
normal.obj: normal.asm
	$(_NASM_) $(NFLAGS) $< -o $@
transf.obj: transf.asm
	$(_NASM_) $(NFLAGS) $< -o $@
flare16s.obj: flare16s.asm
	$(_NASM_)  $(NFLAGS) $< -o $@

endif

####################################################################

all: $(3D_OBJ)
	$(MAKE) -C shaders $(SHADER_LIB)
	$(MAKE) -C draw16 $(DRAW16_LIB)
	$(MAKE) -C draw8 $(DRAW8_LIB)
	$(MAKE) -C draw88 $(DRAW88_LIB)
	$(MAKE) -C mcube $(MCUBE_LIB)
	$(MAKE) -C cache $(CACHE_LIB)
	$(AR) $(LIB_3D) $(SUBLIB) $(3D_OBJ)
#	ranlib $(LIB_3D)
	mv $(LIB_3D) $(LIB_DIR)

so: $(3D_OBJ) _shaders.a _draw16.a _draw8.a \
		_draw88.a _mcube.a
	$(CC) -shared -Wl,-soname,$(SHARED) -o $(SHARED) \
	shaders/*.o draw16/*.o draw8/*.o draw88/*.o mcube/*.o $(3D_OBJ)
	mv $(SHARED) $(LIB_DIR)

####################################################################

WINC = -i..\\include
WOBJECTS = node.obj tree.obj world.obj wrld_rsc.obj transf.obj camera.obj \
	 light.obj group.obj box.obj mesh.obj vmesh.obj 3d_io.obj \
	parts.obj clip.obj shader.obj poly.obj flare.obj wld_info.obj 	\
	quat.obj vista.obj vista2.obj edges.obj vista3.obj roto.obj \
	move.obj zlight.obj sv_world.obj convex.obj normal.obj normal2.obj  \
	optimesh.obj optim2.obj sort.obj anim.obj mk.obj mk2.obj tables.obj render.obj \
	transf2.obj sort2.obj frustum.obj
#array.obj

wat:	$(WOBJECTS)
	$(MAKE) -C shaders wat
	$(MAKE) -C draw16 wat
	$(MAKE) -C draw8 wat
	$(MAKE) -C draw88 wat
	$(MAKE) -C mcube wat
	$(MAKE) -C cache wat
	wlib -q -c -n w3d.lib +3d_io +world +wrld_rsc +transf +camera +vista +vista2 +vista3
	wlib -q -c w3d.lib +parts +box +group +optimesh +optim2 +mesh +vmesh
	wlib -q -c w3d.lib +anim +mk +mk2 +clip +render +tables +frustum
	wlib -q -c w3d.lib +flare +quat +move +light +zlight +sv_world +convex
	wlib -q -c w3d.lib +normal +normal2 +transf2 +sort2
	wlib -q -c w3d.lib +shader +poly +edges +node +tree +wld_info +roto
	wlib -q -c w3d.lib +w3d_16.lib +w3d_8.lib +w3d_88.lib +w3d_2.lib
	wlib -q -c w3d.lib +wmcube.lib +wcache.lib
	move w3d.lib ..\\wlib

####################################################################
####################################################################

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################

clean:
	rm -f *~ *.o *.a *.obj *.lib *.bak core toto
	rm -f sta?????				#warning!!
	$(MAKE) -C shaders clean
	$(MAKE) -C cache clean
	$(MAKE) -C draw16 clean
	$(MAKE) -C draw8 clean
	$(MAKE) -C draw88 clean
	$(MAKE) -C mcube clean

####################################################################
