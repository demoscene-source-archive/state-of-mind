#
# Makefile
#
##################

.EXPORT_ALL_VARIABLES:

_EXE_ = mind

USE_DGA = 1
#USE_SVGALIB = 1
USE_X11 = 1

#USE_DDRAW = 1
USE_VBE = 1

#USE_3DFX = 1

USE_SOUND = 1

BUILD_FLAGS = 

OPTIMIZE = -g #-Wall

####################################################################

linux:
	$(MAKE) TARGET="LNX" all
unix:
	$(MAKE) TARGET="UNIX" all
dos:
	$(MAKE) TARGET="DOS" all

####################################################################

OBJ = 	demo.o debug.o ctrl.o panim.o parts.o \
	part0.o part1.o part2.o part6.o part14.o \
	appear.o part10.o part14.o blb.o dsp.o \
	world2.o world3.o world4.o caustics.o 	\
	fonts2.o fonts.o spans.o 

FX1_OBJ = fx1.o fx2.o fade.o scratch.o janim.o mmask.o mix4.o bilin2.o 
FX1_OBJ +=  mixdsp.o
#flare16.o 

ifneq ($(TARGET),UNIX)
FX1_OBJ += anim.o mix.o paste.o mix2.o mix3.o mix777.o 
FX1_OBJ += bilin.o
#flare16s.o 
endif

LIB_BASE = lib_base.a
LIB_FX1 = lib_fx1.a

####################################################################

ifeq ($(TARGET),LNX)

## additionnal flags
BUILD_FLAGS += -DSKL_NO_CVRT -DSKL_LIGHT_CODE -D$(TARGET)

ROOT_DIR  = ../..
CC	  = gcc -mcpu=pentium -march=i686
LD	  = gcc
#LD	  = gcc
#CC	  = gcc -m486
AS	  = as
AR	  = ar rus
RANL	  = ranlib
_NASM_	  = nasm 

LIB_SO     = -L../../lib -l_sll -l_3d -l_demo -l_au -l_jpg -l_mpx \
		-L/usr/X11R6/lib -lm -lXext -lX11
LIB_INCLUDE = $(ROOT_DIR)/include
#OPTIMIZE += -O6  -ffast-math -malign-loops=2 -malign-jumps=2 -malign-functions=2 -fomit-frame-pointer

LIB = -lm
FINAL = final

################## Add necessary libs #################

ifdef USE_X11
LIB      += -L/usr/X11R6/lib -lXext -lX11
LIB_SO   += -L/usr/X11R6/lib -lXext -lX11
BUILD_FLAGS += -DUSE_X11
else
undefine USE_DGA
endif

ifdef USE_DGA
LIB      += -lXxf86dga
LIB_SO   += -lXxf86dga
BUILD_FLAGS += -DUSE_DGA
endif

ifdef USE_SVGALIB
LIB   += -L../lib -lvgal -lvga
LIB_SO += -lvgagl -lvga
BUILD_FLAGS += -DUSE_SVGALIB
endif

ifdef USE_3DFX
LIB   += -lglide2x
LIB_SO += -lglide2x
BUILD_FLAGS += -DUSE_3DFX
endif

endif		# LNX

####################################################################
####################################################################

ifeq ($(TARGET),UNIX)

BUILD_FLAGS += -DSKL_NO_CVRT -DSKL_LIGHT_CODE -D$(TARGET)
ROOT_DIR  = ../..
CC        = gcc -mips2
LD        = gcc -mips2 #-Wl,-N,-Bstatic
#CC        = /bin/CC -mips2  -multigot
#LD        = /bin/CC -mips2  -multigot
#CC        = /bin/CC -mips4 -n32 # -multigot
#LD        = /bin/CC -mips4 -n32 # -multigot
_LD_      = ld -r -o
AS        = as
AR        = ar r
#AR	  = /usr/ccs/bin/ar r
#RANL	  = /usr/ccs/bin/ranlib
#RANL	  = ranlib
RANL	  = echo
LIB       = -lm
#_NASM_      = echo             #?!

OPTIMIZE += -O6 -DSGI
#OPTIMIZE += -O6
OPTIMIZE += -ffast-math #-fomit-frame-pointer -malign-loops=2 -malign-jumps=2
OPTIMIZE += #-fpic

GFX_LIB   = $(LIB_DIR)/lib_slu.a
LIB_INCLUDE = $(ROOT_DIR)/include

LIB_SO     = -L../../lib -l_slu -l_3d -l_demo -l_au -l_jpg -l_mpx \
		-L/usr/X11R6/lib -lm -lX11 -lXext 
#-lsocket  

ifdef USE_X11
LIB      += -L/usr/X11R6/lib  -lX11 #-lXext -lsocket
LIB_SO   += -L/usr/X11R6/lib -lX11 #-lXext -lsocket
BUILD_FLAGS += -DUSE_X11
endif

LIBMIKM = libmikm.a
FINAL = final


endif   # UNIX

####################################################################
####################################################################

ifeq ($(TARGET),DOS)

ROOT_DIR  = ../..
CC	  = gcc #-mpentium #-m486
LD	  = gcc
AS	  = as
AR	  = ar rs
RANL	  = ranlib
LIB       = #-lm -lpc
GFX_LIB	  = ../lib/lib_sl.a
LIB_INCLUDE = $(ROOT_DIR)/include
_NASM_	  = nasm
OPTIMIZE += -O6 -ffast-math # -fomit-frame-pointer # -malign-loops=2 -malign-jumps=2 -malign-functions=2

LIB = -lm
FINAL = final_a

################## Add necessary libs #################

ifdef USE_VBE
BUILD_FLAGS += -DUSE_VBE
endif

ifdef USE_DDRAW
BUILD_FLAGS += -DUSE_DDRAW
endif

endif		#DOS

####################################################################

LDFLAGS   = -shared
CFLAGS 	  = $(OPTIMIZE) -I$(LIB_INCLUDE) -I../include $(BUILD_FLAGS)

####################################################################

all:
	$(MAKE) -C ./fx1 all
	$(MAKE) -C ./src all
	$(MAKE) -C bin $(FINAL)

pak:
	tar czf deumo.tgz include fx1 src \
	bin/make* bin/wmake.bat make*
dos_pak:
	tar cf deumo.tar include fx1 src \
	bin/make* bin/wmake.bat make*
	gzip deumo.tar

############################################################

clean:
	$(MAKE) -C ./src clean
	$(MAKE) -C ./fx1 clean
	$(MAKE) -C bin clean
	rm -f include/*~ include/*.bak
	rm -f *~ *.bak

#################################################################

