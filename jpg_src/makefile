#
# Lib Makefile
##########################

####################################################################

OBJ   = huff.o jpegout.o markers.o ld_jpg.o ycc.o iycc.o 	\
	ihuff.o adct.o

ifneq ($(TARGET),UNIX)

OBJ_FX	= 

else

OBJ_FX  = 

endif

SHARED = lib_jpg.so
STATIC = $(LIB_JPG)

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
WOBJECTS = huff.obj markers.obj ld_jpg.obj ycc.obj iycc.obj 	\
	ihuff.obj jpegout.obj adct.obj
wat:	$(WOBJECTS)
	wlib -q -c -n wjpg.lib +ld_jpg +markers +huff +ihuff +ycc
	wlib -q -c wjpg.lib +iycc +jpegout +adct
	move wjpg.lib ..\\wlib

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

ifneq ($(TARGET),UNIX)
adct.o: adct.asm
	$(_NASM_) $(NFLAGS) $< -o $@

adct.obj: adct.asm
	$(_NASM_) $(NFLAGS) $< -o $@
endif

####################################################################

%.o:	%.c
	$(CC) $(CFLAGS)  -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################
####################################################################

clean:
	rm -f *~ *.o *.obj *.bak core toto

####################################################################
