

	Hi,

	these are the sources I used for State Of Mind,
	as I left them a while back in 1999. I don't
	remember exactly which code/effects made it into
	the final version. Lotsa fully working code is commented
	out (#if 0 / #endif) as I tried to get the smallest
	possible final .exe. But it should be working. Actually,
	I don't remember what I coded exactly :) All I know
	is that typing 'make linux' into the main dir fills
	the /lib rep with some libs, and them 'make linux' into
	the /dem repertory builds an exe called 'mind'
	into /dem/bin. For the rest, well...
	There's also a lot of patches for other weird
	platforms: sparcs, dec alpha, sgi, whatever...
	Just ignore. I've removed the gus/sb-flavors old
	drivers I'm pretty sure no-one will ever use
	again.

	Now if someone wants to fix the WIN32 version, please
	be my guest :)_ People seems to like this demo
	(thanks a lot!), so if it could survive computer
	aging, I'd be pleased. Besides, it may also give
	coders some ideas, and I'd be pleased to keep the
	demoscene rolling that way.

	Problem with sound seems to come from the Direct Sound
	driver (au_src/drv_dsd.c -> Flush() function): I coded
	this on a broken Win95+MFC+VC. No wonder it's bugging
	as hell. Also: while porting under SunOS, i've somehow
	trashed the bilinear interpolation code in 'bilin2.c'.
	I need to dig further in my pre-historic stuff to
	recover the correct code. I've also lost my .dsp/.dsw.
	Geee... Expect some better sources release soon...



	A final word: why a commercial mp3 in this demo?
	not that I can't track (i've done it in previous
	demos), not that I can't cover the song with my
	guitar and bass, no no no. It's far more simple:

                  I just love Senser's track

	and wanted to make a demo of it. I like to Try
	Things. You should too.


	take care,

                  		Skal


Greetz fly to: Orion, Mfx, Move, Haujobb, Cocoon, 3State, Calodox,
Popsy Team, Aardbei, Exceed, Digital Murder, TPolm, TBL and all I forgot!

