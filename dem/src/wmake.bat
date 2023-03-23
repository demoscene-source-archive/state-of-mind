echo off
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include demo.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include debug.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include ctrl.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include part0.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include part1.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include part2.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include part6.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include part10.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include part14.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include panim.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include parts.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include appear.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include blb.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include dsp.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include world2.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include world3.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include caustics.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include spectrum.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include fonts.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include fonts2.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include spans.c
nasm  -f obj -p../../include/nasmwc.h flare16s.asm -o flare16s.obj

wlib -c -n base.lib +demo +debug +ctrl +panim +parts +spectrum
wlib -c base.lib +part0 +part1 +part2 +part6 +part10 +part14
wlib -c base.lib +appear +blb +dsp +world2 +world3 +caustics
wlib -c base.lib +fonts +fonts2 +spans
rem  don't include demo.obj	( =main() )
move base.lib ..\bin

