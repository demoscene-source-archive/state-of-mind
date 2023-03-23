# Lib Makefile
##########################

####################################################################

OBJ   = mpeg.o mpeg_pic.o mp3_dec.o bit_strm.o \
	mp3huff.o mp3debug.o mp3quant.o mp3dct.o msamples.o \
	mp3_strm.o mp3_mikm.o mp3sgnl.o	\
	wlt_enc.o wlt_dec.o wlt_strm.o

ifneq ($(TARGET),UNIX)

OBJ_FX	= wlt_d4.o mp3dct1.o mp3dct2.o mp3dct3.o mp3dct4.o aalias.o
OBJ_FX += initfpu.o

else

OBJ_FX  = 

endif

SHARED = libmpx.so
STATIC = $(LIB_MPG)

all:	$(LIB_DIR)/$(STATIC)
so:	$(LIB_DIR)/$(SHARED)

####################################################################

$(LIB_DIR)/$(STATIC): $(OBJ) $(OBJ_FX)
	$(AR) $(STATIC) $(OBJ) $(OBJ_FX)
	$(RANL) $(STATIC) 
	mv $(STATIC) $(LIB_DIR)

$(LIB_DIR)/$(SHARED): $(OBJ) $(OBJ_FX)
	$(CC) -shared -Wl,-soname,$(SHARED) -o $(SHARED) $(OBJ) $(OBJ_FX)
	mv $(SHARED) $(LIB_DIR)

####################################################################

WINC = -i..\\include
WOBJECTS = mpeg.obj mpeg_pic.obj mp3_dec.obj bit_strm.obj 	\
	mp3huff.obj mp3debug.obj mp3quant.obj mp3dct.obj msamples.obj 	\
	mp3_strm.obj mp3_mikm.obj wlt_enc.obj wlt_dec.obj wlt_strm.obj	\
	wlt_d4.obj mp3dct1.obj mp3dct2.obj mp3dct3.obj mp3dct4.obj 	\
	aalias.obj initfpu.obj mp3sgnl.obj

wat:	$(WOBJECTS)
	wlib -q -c -n wmpx.lib +mpeg +mpeg_pic +mp3debug +mp3huff +aalias +mp3sgnl.obj
	wlib -q -c wmpx.lib +mp3_dec +mp3quant +mp3dct +bit_strm +msamples +mp3_strm
	wlib -q -c wmpx.lib +wlt_enc +wlt_dec +wlt_strm +mp3_mikm +wlt_d4
	wlib -q -c wmpx.lib +initfpu +mp3dct1 +mp3dct2 +mp3dct3 +mp3dct4
	move wmpx.lib ..\\wlib

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

initfpu.o: initfpu.asm
	$(_NASM_) $(NFLAGS) $< -o $@
wlt_d4.o: wlt_d4.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mp3dct1.o: mp3dct1.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mp3dct2.o: mp3dct2.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mp3dct3.o: mp3dct3.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mp3dct4.o: mp3dct4.asm
	$(_NASM_) $(NFLAGS) $< -o $@
aalias.o: aalias.asm
	$(_NASM_) $(NFLAGS) $< -o $@

####################################################################

initfpu.obj: initfpu.asm
	$(_NASM_) $(NFLAGS) $< -o $@
wlt_d4.obj: wlt_d4.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mp3dct1.obj: mp3dct1.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mp3dct2.obj: mp3dct2.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mp3dct3.obj: mp3dct3.asm
	$(_NASM_) $(NFLAGS) $< -o $@
mp3dct4.obj: mp3dct4.asm
	$(_NASM_) $(NFLAGS) $< -o $@
aalias.obj: aalias.asm
	$(_NASM_) $(NFLAGS) $< -o $@

####################################################################

%.o:	%.c
	$(CC) $(CFLAGS)  -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################

clean:
	rm -f *~ *.o *.obj *.bak core toto

####################################################################
