/* Mouse interface for svgalib. */
/* Can be used independently. */

#ifndef VGAMOUSE_H
#define VGAMOUSE_H

#ifdef __cplusplus
extern "C"
{
#endif

#define	MOUSE_MICROSOFT 0
#define MOUSE_MOUSESYSTEMS 1
#define MOUSE_MMSERIES 2
#define MOUSE_LOGITECH 3
#define MOUSE_BUSMOUSE 4
#define MOUSE_PS2 5
#define MOUSE_LOGIMAN 6

/* Logical or the following values to one of the above at will and give that for
   type in mouse_init (still they make only sense for serial mice) */

#define MOUSE_CHG_DTR	0x80000000	/* CLEAR (!) the DTR line */
#define MOUSE_DTR_HIGH	0x40000000	/* when used with MOUSE_CHG_DTR set DTR not clear it */
#define MOUSE_CHG_RTS	0x20000000	/* CLEAR (!) the RTS line */
#define MOUSE_RTS_HIGH	0x10000000	/* when used with MOUSE_CHG_RTS set RTS not clear it */

/* If MOUSE_CHG_DTR is not given the DTR state is not touched.. same with RTS resp. */
/* So I use MOUSE_MOUSESYSTEMS|MOUSE_CHG_RTS to force my
   multiprotocol mouse to MOUSESYSTEMS and use init the driver to MOUSESYSTEMS */

#define MOUSE_TYPE_MASK 0xffff	/* the upper 16bit are reserved for future flags */

#define MOUSE_LEFTBUTTON 4
#define MOUSE_MIDDLEBUTTON 2
#define MOUSE_RIGHTBUTTON 1

#define MOUSE_DEFAULTSAMPLERATE 150

/* Initialize mouse device. Returns 0 if succesful, -1 otherwise. */
/* (Get the svgalib configured type with vga_getmousetype()). */
    int mouse_init(char *dev, int type, int samplerate);
/* Similar but returns the mouse fd if succesful. */
    int mouse_init_return_fd(char *dev, int type, int samplerate);
/* Set event handler invoked by mouse_update(). */
    typedef void (*__mouse_handler) (int button, int dx, int dy);
    void mouse_seteventhandler(__mouse_handler handler);
/* Close mouse device. */
    void mouse_close(void);
/* Read mouse and handle events. Returns 0 if no event. */
    int mouse_update(void);
/* Similar to mouse_update, but wait for an event to happen. */
    void mouse_waitforupdate(void);

/* mouse_init sets default event handler that keeps track of absolute */
/* coordinates: */

#define MOUSE_NOWRAP 0
#define MOUSE_WRAPX 1
#define MOUSE_WRAPY 2
#define MOUSE_WRAP 3

/* Revert to default handler. */
    void mouse_setdefaulteventhandler(void);
/* Set position of mouse. */
    void mouse_setposition(int x, int y);
/* Set horizontal range of mouse to [x1, x2] incl. */
    void mouse_setxrange(int x1, int x2);
/* Set vertical range of mouse to [y1, y2] incl. */
    void mouse_setyrange(int y1, int y2);
/* Set scale factor by which raw mouse coordinates are divided. */
    void mouse_setscale(int s);
/* Enable/disable wrapping of mouse in horizontal and/or vertical range. */
    void mouse_setwrap(int w);

/* Get current mouse x-coordinate. */
    int mouse_getx(void);
/* Get current mouse y-coordinate. */
    int mouse_gety(void);
/* Get current mouse button status. */
    int mouse_getbutton(void);

#ifdef __cplusplus
}

#endif
#endif
