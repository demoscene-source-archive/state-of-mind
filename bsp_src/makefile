#
# Lib Makefile
#
##########################

####################################################################

OBJ   = bsp.o bsp_splt.o bsp_tree.o bsp_ld.o bsp_sv.o bsp_cst.o

ifneq ($(TARGET),UNIX)
OBJ  += 
else 
OBJ  += 
endif

SHARED = libbsp.so

all:	$(LIB_DIR)/$(LIB_BSP)
so:	$(LIB_DIR)/$(SHARED)

####################################################################

$(LIB_DIR)/$(LIB_BSP): $(OBJ)
	$(AR) $(LIB_BSP) $(OBJ)
	$(RANL) $(LIB_BSP) 
	mv $(LIB_BSP) $(LIB_DIR)

$(LIB_DIR)/$(SHARED): $(OBJ)
	$(CC) -shared -Wl,-soname,$(SHARED) -o $(SHARED) $(OBJ)
	mv $(SHARED) $(LIB_DIR)

####################################################################

%.o:	%.c
	$(CC) $(CFLAGS)  -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################

ifeq ($(TARGET),WATC)
WINC = -i..\\include
WOBJECTS = bsp.obj bsp_tree.obj bsp_splt.obj bsp_ld.obj bsp_sv.obj bsp_cst.obj
endif

wat:	$(WOBJECTS)
	wlib -q -c -n wbsp.lib +bsp +bsp_tree +bsp_splt
	wlib -q -c wbsp.lib +bsp_ld +bsp_sv +bsp_cst
	move wbsp.lib ..\\wlib


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

sprite2.o: sprite2.asm
	nasm  $(NFLAGS) $< -o $@

sprite2.obj: sprite2.asm
	nasm $(NFLAGS)  $< -o $@

####################################################################

clean:
	rm -f *~ *.o *.obj *.bak core toto sta?????

####################################################################
