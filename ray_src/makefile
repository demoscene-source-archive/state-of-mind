#
# Makefile
#

#################### LIBS ##########################################

RAY_OBJ   = slabs.o rendray.o rlight.o rmat.o octree.o \
	rad.o rmesh.o sh_ray.o

ifneq ($(TARGET),UNIX)
RAY_OBJ += imesh.o
endif

all: $(RAY_OBJ)

CFLAGS += -I../include

SHARED = lib_ray.so

all:	$(LIB_DIR)/$(LIB_RAY)
so:	$(LIB_DIR)/$(SHARED)

####################################################################

$(LIB_DIR)/$(LIB_RAY): $(RAY_OBJ)
	$(AR) $(LIB_RAY) $(RAY_OBJ)
	$(RANL) $(LIB_RAY) 
	mv $(LIB_RAY) $(LIB_DIR)

$(LIB_DIR)/$(SHARED): $(RAY_OBJ)
	$(CC) -shared -Wl,-soname,$(SHARED) -o $(SHARED) $(RAY_OBJ)
	mv $(SHARED) $(LIB_DIR)

####################################################################

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) -fo=$@ $<

####################################################################
####################################################################

WINC = -i..\\include
WOBJECTS = slabs.obj rendray.obj rad.obj rlight.obj \
 rmat.obj octree.obj rmesh.obj imesh.obj
wat:	$(WOBJECTS)
	wlib -q -c -n wray.lib +slabs +rendray +rad +rlight
	wlib -q -c wray.lib +rmat +octree +rmesh +imesh
	move wray.lib ..\\wlib

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

imesh.o: imesh.asm
	$(_NASM_) $(NFLAGS) $< -o $@

imesh.obj: imesh.asm
	$(_NASM_) $(NFLAGS) $< -o $@

####################################################################
####################################################################

clean:
	rm -f *~ *.o *.a *.obj *.bak core toto

####################################################################
