# Makefile
##################

OBJ = $(LIB_BASE) $(LIB_FX1)

####################################################################

final:
	$(LD) -o $(_EXE_) $(OBJ) $(LIB_SO) #../../lib/libirix_audio32.a
#	purify $(LD) -o $(_EXE_).pure $(OBJ) $(LIB_SO)

#	strip --discard-locals --strip-all --strip-unneeded --discard-all $(_EXE_)
#	strip -s $(_EXE_)
#	chmod u+s $(_EXE_)

DOS_LIB = ../../lib/lib_3d.a ../../lib/lib_mpx.a ../../lib/lib_demo.a \
	 ../../lib/lib_sl.a \
 \
	 ../../lib/lib_au.a ../../lib/lib_jpg.a

final_a:
	$(LD) -o $(_EXE_) $(OBJ) $(DOS_LIB) $(LIB) 
#	strip mind
	copy /B c:\djgpp\bin\pmodstub.exe + mind pmind.exe

#	strip --discard-locals --strip-all --strip-unneeded --discard-all $(_EXE_)
#	strip -s $(_EXE_)
#	chmod u+s $(_EXE_)

#################################################################

clean:
	rm -f *~ *.o *.a *.obj *.lib *.bak core toto $(_EXE_)
	rm -f ctab*.lzw flr01tab.lzw ztab.lzw

#################################################################

