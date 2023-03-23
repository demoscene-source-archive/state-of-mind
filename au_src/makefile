#
# Lib Makefile
##########################

####################################################################

OBJ   = audio.o init.o drv_wav.o

ifneq ($(TARGET),UNIX)

OBJ_FX	= 

endif


ifeq ($(TARGET),LNX)

OBJ_FX  = drv_oss.o

endif

ifeq ($(TARGET),UNIX)

OBJ_FX  = drv_oss.o

endif

ifeq ($(TARGET),DOS)

OBJ_FX = 

endif

SHARED = libaudio.so
STATIC = $(LIB_AUDIO)

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

ifeq ($(TARGET),WATC)
WINC = -i..\\include
WOBJECTS = drv_sb.obj drv_wav.obj drv_sb16.obj drv_wss.obj \
 drvsbpro.obj audio.obj init.obj irq.obj hdma.obj 
endif

wat:	$(WOBJECTS)
	wlib -q -c -n waudio.lib +audio +init +irq +hdma
	wlib -q -c waudio.lib +drv_sb +drv_sb16 +drv_wss +drv_wav +drvsbpro
	move waudio.lib ..\\wlib

####################################################################

%.o:	%.c
	$(CC) $(CFLAGS)  -c $< -o $@

%.obj:  %.c
	$(WAT) $(WFLAGS) $(WINC) $< -fo=$@

####################################################################
####################################################################

clean:
	rm -f *~ *.o *.obj *.bak core toto

####################################################################
