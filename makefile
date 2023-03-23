#
# Makefile
#          Skal98
##########################

.EXPORT_ALL_VARIABLES:

##############################################################
###################### drivers to be used ####################
##############################################################

USE_DGA = 1
#USE_SVGALIB = 1
USE_X11 = 1

#USE_DDRAW = 1
USE_VBE = 1

#USE_3DFX = 1

#NO_SOUND = 1

## additionnal flags
BUILD_FLAGS = 
#BUILD_FLAGS += -DSKL_NO_CVRT -DSKL_LIGHT_CODE -DFINAL
BUILD_FLAGS += -D$(TARGET) # -D_LIGHT_CODE_ 
OPTIMIZE = -g #-Wall -pedantic

###################### Target OSes #################################
####################################################################

linux:
	$(MAKE) TARGET="LNX" all
so:
	$(MAKE) TARGET="LNX" _so_

unix:
	$(MAKE) TARGET="UNIX" all
dos:
	$(MAKE) TARGET="DOS" all

####################################################################

WFLAGS = -zq -j -ei -ot -or -5s -fp5
WAT = wcc386
wat:
	$(MAKE) -C $(3D_SRC) TARGET="WATC" wat
	$(MAKE) -C $(BSP_SRC) TARGET="WATC" wat
	$(MAKE) -C $(JPG_SRC) TARGET="WATC" wat
	$(MAKE) -C $(RAY_SRC) TARGET="WATC" wat
	$(MAKE) -C $(LIB_SRC) TARGET="WATC" wat
	$(MAKE) -C $(DEMO_SRC) TARGET="WATC" wat
	$(MAKE) -C $(AUDIO_SRC) TARGET="WATC" wat
	$(MAKE) -C $(MPG_SRC) TARGET="WATC" wat
#	$(MAKE) -C $(SW_SRC) TARGET="WATC" wat

####################################################################

all:
	$(MAKE) _lib_
	$(MAKE) _3d_
	$(MAKE) _ray_
	$(MAKE) _demo_
	$(MAKE) _aud_
	$(MAKE) _jpg_
	$(MAKE) _bsp_
	$(MAKE) _mpg_

_so_:
	$(MAKE) -C $(LIB_SRC) so
	$(MAKE) -C $(3D_SRC) so
	$(MAKE) -C $(BSP_SRC) so
	$(MAKE) -C $(RAY_SRC) so
	$(MAKE) -C $(DEMO_SRC) so
	$(MAKE) -C $(AUDIO_SRC) so
	$(MAKE) -C $(JPG_SRC) so
	$(MAKE) -C $(MPG_SRC) so
#	$(MAKE) -C $(SW_SRC) so
#	$(MAKE) -C $(MIKM_SRC) so
#	strip ./lib/*.so


mikmod:
	$(MAKE) -C music $(LIBMIKM)

####################################################################

ifeq ($(TARGET),UNIX)

ROOT_DIR  := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
#CC	  = gcc 
#LD	  = gcc
#CC	  = /bin/CC  -mips4 -n32 #-multigot
#LD	  = /bin/CC  -mips4 -n32 #-multigot
CC	  = gcc # -mips4
LD	  = gcc # -mips4
#_LD_	  = ld -r -o
AS	  = as
AR	  = ar r
#AR	  = /usr/ccs/bin/ar r
#RANL	  = /usr/ccs/bin/ranlib
#RANL	  = ranlib
RANL	  = echo
LIB       = -lm
#_NASM_      = echo		#?!

OPTIMIZE += -O6 -DSGI
#OPTIMIZE += -O6
OPTIMIZE += -ffast-math #-fomit-frame-pointer -malign-loops=2 -malign-jumps=2 -malign-functions=2
OPTIMIZE += #-fpic

GFX_LIB	  = $(LIB_DIR)/lib_slu.a

ifdef USE_X11
LIB      += -L/usr/X11R6/lib -lX11 -lXext # -lsocket
LIB_SO   += -L/usr/X11R6/lib -lX11 -lXext # -lsocket
BUILD_FLAGS += -DUSE_X11
endif

LIBMIKM	= libmikm.a

endif	# UNIX

####################################################################

ifeq ($(TARGET),LNX)

ROOT_DIR  := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
#CC	  = gcc -mcpu=pentium 
#CC	  = gcc -m486
CC	= gcc -mcpu=pentium -march=i686
LD	  = /usr/local/bin/gcc #-Wl,-N,-Bstatic
#_LD_	  = ld -r -o
AS	  = as
AR	  = ar rus
RANL	  = ranlib
_NASM_	  = nasm
GFX_LIB	  = $(LIB_DIR)/lib_sll.a
LIB       = -lm
LIB_SO     = -L../lib -lskal -l3d -ldemo -lm
#MIKM_SRC    = $(ROOT_DIR)/mik2_src

OPTIMIZE += -O6
OPTIMIZE += -ffast-math #-fomit-frame-pointer -malign-loops=2 -malign-jumps=2 -malign-functions=2
OPTIMIZE += #-fpic

################## Add necessary libs ##############################

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
LIB   += -L../lib -lvgagl -lvga
LIB_SO += -lvgagl -lvga
BUILD_FLAGS += -DUSE_SVGALIB
endif

ifdef USE_3DFX
LIB   += -lglide2x
LIB_SO += -lglide2x
BUILD_FLAGS += -DUSE_3DFX
endif

LIBMIKM	= libmikm.a

endif		# LNX

####################################################################

ifeq ($(TARGET),DOS)

#MIKM_SRC    = $(ROOT_DIR)/mik2_src
ROOT_DIR = .
CC	  = gcc #-mpentium #-m486
LD	  = gcc
#_LD_	  = ld -i -o
AS	  = as
AR	  = ar rus
RANL	  = ranlib
_NASM_	  = nasm
LIB       = #-lm -lpc
GFX_LIB	  = $(LIB_DIR)/lib_sl.a
ROOT_DIR  = .

OPTIMIZE += -O3
OPTIMIZE += -ffast-math # -fomit-frame-pointer # -malign-loops=2 -malign-jumps=2 -malign-functions=2

WAT	  = wcc386
WAT_FLAGS = -ot -or -5s -fp5

ifdef USE_VBE
BUILD_FLAGS += -DUSE_VBE
endif

ifdef USE_DDRAW
BUILD_FLAGS += -DUSE_DDRAW
endif

LIBMIKM	= libmikmd.a

endif		#DOS

####################################################################

LIB_DIR   = ../lib
LIB_INCLUDE = ../include #-I/usr/include

LIB_SRC     = $(ROOT_DIR)/lib_src
EXAMPLE_SRC = $(ROOT_DIR)/example
DEMO_SRC    = $(ROOT_DIR)/demo_src
SW_SRC	    = $(ROOT_DIR)/sw_src
SW          = $(ROOT_DIR)/sw
3D_EX       = $(ROOT_DIR)/3dx
3D_SRC      = $(ROOT_DIR)/3d_src
BSP_SRC      = $(ROOT_DIR)/bsp_src
RAY_SRC     = $(ROOT_DIR)/ray_src
RAY_EX      = $(ROOT_DIR)/ray_ex
MPG_SRC     = $(ROOT_DIR)/mpg_src
JPG_SRC     = $(ROOT_DIR)/jpg_src
AUDIO_SRC   = $(ROOT_DIR)/au_src

LIB_SW   = lib_sw.a
LIB_DEMO = lib_demo.a
LIB_BSP = lib_bsp.a
LIB_3D   = lib_3d.a
LIB_RAY   = lib_ray.a
LIB_MPG  = lib_mpx.a
LIB_JPG  = lib_jpg.a
LIB_AUDIO  = lib_au.a 

ifndef ROOT_DIR
ROOT_DIR = .
endif

SUBDIRS = $(LIB_SRC) $(DEMO_SRC) $(SW_SRC) $(3D_SRC) $(BSP_SRC) $(AUDIO_SRC) \
	$(MPG_SRC) $(JPG_SRC) $(RAY_SRC) $(EXAMPLE_SRC) $(SW) $(3D_EX) \
	$(RAY_EX) music deumo

ifdef NO_SOUND

#SUBDIRS += $(MIKM_SRC)
BUILD_FLAGS += 	-DNO_SOUND

endif

LDFLAGS   = -shared
CFLAGS 	  = $(OPTIMIZE) -I$(LIB_INCLUDE) $(BUILD_FLAGS)

STAT_LIB = $(LIB_DIR)/$(LIB_3D) $(LIB_DIR)/$(LIB_RAY) $(LIB_DIR)/$(LIB_SW) \
	$(GFX_LIB) $(LIB_DIR)/$(LIB_JPG) $(LIB_DIR)/$(LIB_DEMO) $(LIB_DIR)/$(LIB_MPG)

#STAT_LIB += $(LIB_DIR)/$(LIBMIKM) 

####################################################################
####################################################################

clean:
	for i in $(SUBDIRS); do $(MAKE) -C $$i TARGET=$(TARGET) clean; done
	$(MAKE) -C include clean
	$(MAKE) -C btm_src clean
	$(MAKE) -C 3dfx clean
#	$(MAKE) -C mik2_src clean
#	$(MAKE) -C go clean
#	$(MAKE) -C mpg_src clean
#	$(MAKE) -C au_src clean


dos_clean:
	$(MAKE) -C $(LIB_SRC) clean
	$(MAKE) -C $(DEMO_SRC) clean
	$(MAKE) -C $(3D_EX) clean
	$(MAKE) -C $(RAY_EX) clean
	$(MAKE) -C $(EXAMPLE_SRC) clean
	$(MAKE) -C $(3D_SRC) clean
	$(MAKE) -C $(BSP_SRC) clean
	$(MAKE) -C $(RAY_SRC) clean
	$(MAKE) -C $(MPG_SRC) clean
	$(MAKE) -C $(AUDIO_SRC) clean
	$(MAKE) -C $(RAY_EX) clean
	$(MAKE) -C $(JPG_SRC) clean
	$(MAKE) -C $(SW_SRC) clean
	$(MAKE) -C $(SW) clean
	$(MAKE) -C music clean
	$(MAKE) -C go clean
	$(MAKE) -C include clean

ultra_clean:
	rm -rf 3d_src sw_src demo_src lib_src au_src mpg_src jpg_src
ultra_clean_dos:
	deltree 3d_src
	deltree sw_src
	deltree demo_src
	deltree bsp_src
	deltree lib_src 
	deltree au_src
	deltree mpg_src
	deltree jpg_src
  

####################################################################

music_pak:
	tar czf musiclib.tgz audio music	\
# lib/libmikmd.a lib/libmikm.a wlib/wmikm.lib \

all_pak:
	tar cf demo.tgz 3d_src ray_src btm_src demo_src au_src \
		lib_src sw_src include lib/remove.me bsp_src \
		wlib/remove.me \
		sw 3dx mpg_src jpg_src mpg_ex example  makefile 

all_archive:
	tar czf all_demo.tgz 3d_src ray_src btm_src demo_src include  \
		au_src lib_src sw_src music lib/remove.me wlib/remove.me  \
		sw 3dx mpg_src jpg_src mpg_ex example makefile bsp_src \
		btm 3ds deumo

src_pak:
	tar czf allsrc.tgz include 3d_src ray_src btm_src demo_src \
		au_src lib_src mpg_src jpg_src bsp_src sw_src music   \
		makefile lib/remove.me  wlib/remove.me \
		sw/*.c sw/*.h sw/makefile 		\
		3dx/*.c 3dx/makefile 3dx/wmake.bat 	\
		mpg_ex example

pak:
	tar czf libsrc.tgz 3d_src ray_src btm_src \
 mpg_src jpg_src demo_src bsp_src \
 au_src ray_ex makefile lib_src include \
 lib/remove.me  wlib/remove.me  \
 3dx mpg_ex example 3dfx/*.c 3dfx/*.h 3dfx/makefile
#sw sw_src 

tar:
	tar cf test2.tar 3d_src ray_src btm_src \
	mpg_src jpg_src demo_src bsp_src \
	au_src ray_ex makefile lib_src include \
	dem/makefile dem/src dem/fx1 dem/include \
	dem/bin/makefile
	gzip test2.tar

dos_pak:
	tar cf libsrc.tar 3d_src ray_src btm_src \
 mpg_src jpg_src demo_src bsp_src \
 au_src ray_ex makefile \
 lib_src include 
	tar cf ex.tar 3dx mpg_ex example sw

####################################################################
####################################################################

3d:
	$(MAKE) TARGET="LNX" _3d_
3d2:
	$(MAKE) TARGET="UNIX" _3d_
3d3:
	$(MAKE) TARGET="DOS" _3d_
_3d_:
	$(MAKE) -C $(3D_SRC) all

####################################################################

bsp:
	$(MAKE) TARGET="LNX" _bsp_
bsp2:
	$(MAKE) TARGET="UNIX" _bsp_
bsp3:
	$(MAKE) TARGET="DOS" _bsp_
_bsp_:
	$(MAKE) -C $(BSP_SRC) all

####################################################################

ray:
	$(MAKE) TARGET="LNX" _ray_
ray2:
	$(MAKE) TARGET="UNIX" _ray_
ray3:
	$(MAKE) TARGET="DOS" _ray_
_ray_:
	$(MAKE) -C $(RAY_SRC) all

####################################################################

demo:
	$(MAKE) TARGET="LNX" _demo_
demo2:
	$(MAKE) TARGET="UNIX" _demo_
demo3:
	$(MAKE) TARGET="DOS" _demo_
_demo_:
	$(MAKE) -C $(DEMO_SRC) all
ifneq ($(TARGET),UNIX)
#	$(MAKE) -C $(MIKM_SRC) all
endif

####################################################################

btm_bin:
	$(MAKE) TARGET="LNX" _btm_
btm_bin2:
	$(MAKE) TARGET="UNIX" _btm_
btm_bin3:
	$(MAKE) TARGET="DOS" _btm_
_btm_:
	$(MAKE) -C btm_src all

####################################################################

wid:
	$(MAKE) TARGET="LNX" _wid_
wid2:
	$(MAKE) TARGET="UNIX" _wid_
wid3:
	$(MAKE) TARGET="DOS" _wid_

_wid_:
#	$(MAKE) -C $(SW_SRC) all

####################################################################

lb:
	$(MAKE) TARGET="LNX" _lib_
lb2:
	$(MAKE) TARGET="UNIX" _lib_
lb3:
	$(MAKE) TARGET="DOS" _lib_
_lib_:
	$(MAKE) -C $(LIB_SRC) all

####################################################################

mpg:
	$(MAKE) TARGET="LNX" _mpg_
mpg2:
	$(MAKE) TARGET="UNIX" _mpg_
mpg3:
	$(MAKE) TARGET="DOS" _mpg_
_mpg_:
	$(MAKE) -C $(MPG_SRC) all
	$(MAKE) -C $(MPG_EX) all

####################################################################

jpg:
	$(MAKE) TARGET="LNX" _jpg_
jpg2:
	$(MAKE) TARGET="UNIX" _jpg_
jpg3:
	$(MAKE) TARGET="DOS" _jpg_
_jpg_:
	$(MAKE) -C $(JPG_SRC) all

####################################################################

ex:
	$(MAKE) TARGET="LNX" _example_
ex2:
	$(MAKE) TARGET="UNIX" _example_
ex3:
	$(MAKE) TARGET="DOS" _example_
_example_:
	$(MAKE) -C $(3D_EX) all
	$(MAKE) -C $(RAY_EX) all
	$(MAKE) -C $(SW) all
	$(MAKE) -C $(EXAMPLE_SRC) all

game:
	$(MAKE) TARGET="LNX" _game_
game2:
	$(MAKE) TARGET="UNIX" _game_
_game_:
	$(MAKE) -C game_src all

hap:
	$(MAKE) TARGET="LNX" _happy_
hap2:
	$(MAKE) TARGET="UNIX" _happy_
_happy_:
	$(MAKE) -C happy all

####################################################################

fx:
	$(MAKE) TARGET="LNX" _fx_
_fx_:
	$(MAKE) -C 3dfx all

####################################################################

audio:
	$(MAKE) TARGET="LNX" _aud_
audio2:
	$(MAKE) TARGET="UNIX" _aud_
audio3:
	$(MAKE) TARGET="DOS" _aud_
_aud_:
	$(MAKE) -C $(AUDIO_SRC) all

####################################################################
