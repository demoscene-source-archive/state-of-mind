#
# Lib Makefile
#
##########################

####################################################################

OBJ   = blur.o btm.o btm_util.o colfader.o remapbtm.o btm_aa.o io.o ppm.o \
	compress.o ld_btm.o sv_btm.o ld_gif.o sv_gif.o tga.o bmp.o raw.o \
	sv_any.o ld_any.o ld_pak.o sv_pak.o zblur.o truecol.o \
	lzw.o lzw_dec.o lzw_enc.o lzw_strm.o \
	cpu.o bump.o bump2.o bump3.o clamp.o color.o	\
        global.o demo_drv.o music.o rand.o pak.o mem_btm.o \
	clock.o timers.o sprite.o sprite3.o spark.o mix_tab.o \
	vbuf.o dmask.o dmaskout.o

ifneq ($(TARGET),UNIX)
OBJ_FX	= cpu2.o sprite2.o sprite4.o fire.o vbuf2.o dmask2.o 
else 
OBJ_FX  = indy.o
endif

SHARED = libdemo.so

all:	$(LIB_DIR)/$(LIB_DEMO)
so:	$(LIB_DIR)/$(SHARED)

####################################################################

$(LIB_DIR)/$(LIB_DEMO): $(OBJ) $(OBJ_FX)
	$(AR) $(LIB_DEMO) $(OBJ) $(OBJ_FX)
	$(RANL) $(LIB_DEMO) 
	mv $(LIB_DEMO) $(LIB_DIR)

$(LIB_DIR)/$(SHARED): $(OBJ) $(OBJ_FX)
	$(CC) -shared -Wl,-soname,$(SHARED) -o $(SHARED) $(OBJ) $(OBJ_FX)
	mv $(SHARED) $(LIB_DIR)

####################################################################

%.o:	%.c
	$(CC) $(CFLAGS)  -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################

ifeq ($(TARGET),WATC)
WINC = -i..\\include
WOBJECTS = cpu.obj fire.obj colfader.obj timers.obj mem_btm.obj	\
 btm_util.obj pak.obj blur.obj  sprite.obj sprite2.obj sprite3.obj \
 sprite4.obj rand.obj  music.obj  demo_drv.obj  global.obj 	\
 color.obj  clamp.obj  spark.obj  zblur.obj  truecol.obj 	\
 bump3.obj  bump2.obj  bump.obj  lzw_enc.obj  lzw_dec.obj 	\
 lzw.obj  lzw_strm.obj  sv_pak.obj  ld_pak.obj  tga.obj 	\
 bmp.obj  raw.obj  ld_any.obj  sv_gif.obj  ld_gif.obj 		\
 compress.obj  ld_btm.obj  sv_btm.obj ppm.obj io.obj		\
 btm_aa.obj mix_tab.obj remapbtm.obj btm.obj vbuf.obj		\
 dmask.obj dmask2.obj dmaskout.obj vbuf2.obj
endif

wat:	$(WOBJECTS)
	wlib -q -c -n wdemo.lib +cpu +fire +timers +btm_util +mem_btm 
	wlib -q -c wdemo.lib +pak +blur +colfader +sprite +sprite2
	wlib -q -c wdemo.lib +sprite3 +sprite4 +rand +music +demo_drv 
	wlib -q -c wdemo.lib +color +clamp +spark +zblur +truecol 
	wlib -q -c wdemo.lib +bump3 +bump2 +bump +lzw_enc +lzw_dec
	wlib -q -c wdemo.lib +lzw +lzw_strm +sv_pak +ld_pak +tga
	wlib -q -c wdemo.lib +bmp +raw +ld_any +sv_gif +ld_gif +global
	wlib -q -c wdemo.lib +compress +ld_btm +sv_btm +ppm +io
	wlib -q -c wdemo.lib +btm_aa +mix_tab +remapbtm +vbuf +vbuf2
	wlib -q -c wdemo.lib +dmask +dmask2 +dmaskout +btm
	move wdemo.lib ..\\wlib


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

#bump3.o: bump3.asm
#	$(_NASM_)  $(NFLAGS) $< -o $@

sprite2.o: sprite2.asm
	$(_NASM_)  $(NFLAGS) $< -o $@
sprite4.o: sprite4.asm
	$(_NASM_)  $(NFLAGS) $< -o $@

cpu2.o: cpu2.asm
	$(_NASM_)  $(NFLAGS) $< -o $@
fire.o: fire.asm
	$(_NASM_)  $(NFLAGS) $< -o $@
dmask2.o: dmask2.asm
	$(_NASM_)  $(NFLAGS) $< -o $@
vbuf2.o: vbuf2.asm
	$(_NASM_)  $(NFLAGS)  $< -o $@

sprite2.obj: sprite2.asm
	$(_NASM_) $(NFLAGS)  $< -o $@
sprite4.obj: sprite4.asm
	$(_NASM_) $(NFLAGS)  $< -o $@
cpu2.obj: cpu2.asm
	$(_NASM_)  $(NFLAGS)  $< -o $@
fire.obj: fire.asm
	$(_NASM_)  $(NFLAGS)  $< -o $@
dmask2.obj: dmask2.asm
	$(_NASM_)  $(NFLAGS)  $< -o $@
vbuf2.obj: vbuf2.asm
	$(_NASM_)  $(NFLAGS)  $< -o $@

####################################################################

clean:
	rm -f *~ *.o *.obj *.bak core toto sta?????

####################################################################
