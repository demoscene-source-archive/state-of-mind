#
# Lib Makefile
#
##########################

####################################################################

ifeq ($(TARGET),UNIX)
GFX_LIB	  = lib_slu.a
DRV_OBJ   = drv_lnx.o
endif

ifeq ($(TARGET),LNX)
GFX_LIB	  = lib_sll.a
DRV_OBJ   = drv_lnx.o drv_dga.o

ifdef USE_SVGALIB
DRV_OBJ  += drv_svgl.o
endif

endif

ifeq ($(TARGET),DOS)
GFX_LIB	  = lib_sl.a
DRV_OBJ   = drv_vbe.o v_cards.o drv_bank.o dos_mem.o out_regs.o drv_vga.o
endif

OBJECTS   = cvrt_col.o driver.o mem_map.o masks.o drv_col.o \
	    mem.o msg.o

OBJECTS += $(DRV_OBJ)

SHARED = libskal.so

all:	gfx_lib
so:	$(LIB_DIR)/$(SHARED)

####################################################################

gfx_lib: $(OBJECTS)
	$(AR) $(GFX_LIB) $(OBJECTS)
	$(RANL) $(GFX_LIB) 
	mv $(GFX_LIB) $(LIB_DIR)

$(LIB_DIR)/$(SHARED):	$(OBJECTS)
	$(CC) -shared -Wl,-soname,$(SHARED) -o $(SHARED) $(OBJECTS)
	mv $(SHARED) $(LIB_DIR)

####################################################################

ifeq ($(TARGET),WATC)
WINC = -i..\\include
WOBJECTS = drv_vbe.obj v_cards.obj dos_mem.obj out_regs.obj	\
 drv_vga.obj driver.obj mem_map.obj masks.obj drv_col.obj	\
 mem.obj msg.obj cvrt_col.obj drv_bank.obj
endif

wat:	$(WOBJECTS)
	wlib -q -c -n wgfx.lib +drv_vbe +v_cards +dos_mem +out_regs
	wlib -q -c wgfx.lib +drv_vga +driver +mem_map +masks +drv_col
	wlib -q -c wgfx.lib +mem +msg +drv_bank +cvrt_col
	move wgfx.lib ..\\wlib

####################################################################

ifeq ($(TARGET),WATC)
NFLAGS = -f obj -d__WATCOMC__ -p../include/nasmwc.h
endif

ifeq ($(TARGET),DOS)
NFLAGS = -f coff -p../include/nasmdj.h 
endif

drv_bank.o: drv_bank.asm
	$(_NASM_)  $(NFLAGS) $< -o $@

drv_bank.obj: drv_bank.asm
	$(_NASM_) $(NFLAGS) $< -o $@

####################################################################

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) $< -fo=$@

####################################################################

clean:
	rm -f *~ *.o *.obj *.bak core toto
	rm -f $(OBJECTS)

####################################################################
