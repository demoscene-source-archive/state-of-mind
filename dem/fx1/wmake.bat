echo off
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include fx1.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include fx2.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include fade.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include scratch.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include janim.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include mmask.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include bilin2.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include mix4.c
wcc386 -j -ei -ot -or -5s -fp5 -i..\..\include -i..\include flare16.c
nasm  -f obj -p../../include/nasmwc.h flare16s.asm -o flare16s.obj
nasm  -f obj -p../../include/nasmwc.h bilin.asm -o bilin.obj
nasm  -f obj -p../../include/nasmwc.h paste.asm -o paste.obj
nasm  -f obj -p../../include/nasmwc.h mix777.asm -o mix777.obj
nasm  -f obj -p../../include/nasmwc.h mixdsp.asm -o mixdsp.obj
nasm  -f obj -p../../include/nasmwc.h mix.asm -o mix.obj
nasm  -f obj -p../../include/nasmwc.h mix2.asm -o mix2.obj
nasm  -f obj -p../../include/nasmwc.h mix3.asm -o mix3.obj
nasm  -f obj -p../../include/nasmwc.h anim.asm -o anim.obj
wlib -c -n fx1.lib +fx1 +fx2 +fade +scratch +mmask
wlib -c fx1.lib +bilin +bilin2 +mix4 +janim +anim
wlib -c fx1.lib +mix +mix2 +mix3 +mix777 +mixdsp +paste
wlib -c fx1.lib +flare16 +flare16s

move fx1.lib ..\bin

