/* Keyboard interface for svgalib. */
/* Can be used independently. */

#ifndef VGAKEYBOARD_H
#define VGAKEYBOARD_H

#ifdef __cplusplus
extern "C"
{
#endif

#define SCANCODE_CURSORUP	72	/* Principal cursor keys. */
#define SCANCODE_CURSORLEFT	75	/* (On numeric keypad.) */
#define SCANCODE_CURSORRIGHT	77
#define SCANCODE_CURSORDOWN	80

#define SCANCODE_CURSORUPLEFT	71	/* Diagonal keypad keys. */
#define SCANCODE_CURSORUPRIGHT	73
#define SCANCODE_CURSORDOWNLEFT	79
#define SCANCODE_CURSORDOWNRIGHT 81

#define SCANCODE_CURSORBLOCKUP	103	/* Cursor key block. */
#define SCANCODE_CURSORBLOCKLEFT 105
#define SCANCODE_CURSORBLOCKRIGHT 106
#define SCANCODE_CURSORBLOCKDOWN 108

#define SCANCODE_KEYPAD0	82
#define SCANCODE_KEYPAD1	79
#define SCANCODE_KEYPAD2	80
#define SCANCODE_KEYPAD3	81
#define SCANCODE_KEYPAD4	75
#define SCANCODE_KEYPAD5	76
#define SCANCODE_KEYPAD6	77
#define SCANCODE_KEYPAD7	71
#define SCANCODE_KEYPAD8	72
#define SCANCODE_KEYPAD9	73
#define SCANCODE_KEYPADENTER	96
#define SCANCODE_KEYPADPLUS	78
#define SCANCODE_KEYPADMINUS	74

#define SCANCODE_Q		16
#define SCANCODE_W		17
#define SCANCODE_E		18
#define SCANCODE_R		19
#define SCANCODE_T		20
#define SCANCODE_Y		21
#define SCANCODE_U		22
#define SCANCODE_I		23
#define SCANCODE_O		24
#define SCANCODE_P		25

#define SCANCODE_A		30
#define SCANCODE_S		31
#define SCANCODE_D		32
#define SCANCODE_F		33
#define SCANCODE_G		34
#define SCANCODE_H		35
#define SCANCODE_J		36
#define SCANCODE_K		37
#define SCANCODE_L		38

#define SCANCODE_Z		44
#define SCANCODE_X		45
#define SCANCODE_C		46
#define SCANCODE_V		47
#define SCANCODE_B		48
#define SCANCODE_N		49
#define SCANCODE_M		50

#define SCANCODE_ESCAPE		1
#define SCANCODE_ENTER		28
#define SCANCODE_RIGHTCONTROL	97
#define SCANCODE_CONTROL	97
#define SCANCODE_RIGHTALT	100
#define SCANCODE_LEFTCONTROL	29
#define SCANCODE_LEFTALT	56
#define SCANCODE_SPACE		57

#define SCANCODE_F1		59
#define SCANCODE_F2		60
#define SCANCODE_F3		61
#define SCANCODE_F4		62
#define SCANCODE_F5		63
#define SCANCODE_F6		64
#define SCANCODE_F7		65
#define SCANCODE_F8		66
#define SCANCODE_F9		67
#define SCANCODE_F10		68

#define KEY_EVENTRELEASE 0
#define KEY_EVENTPRESS 1

/* Initialize keyboard handler (brings keyboard into RAW mode). Returns */
/* 0 if succesful, -1 otherwise. */
    int keyboard_init(void);
/* Similar, but returns console fd if succesful. */
    int keyboard_init_return_fd(void);
/* Set event handler invoked by keyboard_update(). */
    typedef void (*__keyboard_handler) (int scancode, int press);
    void keyboard_seteventhandler(__keyboard_handler handler);
/* Return keyboard to normal state. */
    void keyboard_close(void);
/* Read raw keyboard device and handle events. Returns 0 if no event. */
    int keyboard_update(void);
/* Similar to keyboard_update, but wait for an event to happen. */
/* [This doesn't seem to work very well -- use select on fd] */
    void keyboard_waitforupdate(void);

/* keyboard_init sets default event handler that keeps track of complete */
/* keyboard state: */

/* Result of keypressed. */
#define KEY_NOTPRESSED 0
#define KEY_PRESSED 1

/* Modes for translatekeys. */
#define TRANSLATE_CURSORKEYS 1	/* Map cursor block to keypad cursor. */
#define TRANSLATE_DIAGONAL 2	/* Map keypad diagonal to keypad cursor. */
#define TRANSLATE_KEYPADENTER 4	/* Map keypad enter to main enter key. */
#define DONT_CATCH_CTRLC 8	/* Disable Crtl-C check. */

/* Revert to default handler. */
    void keyboard_setdefaulteventhandler(void);
/* Return pointer to buffer holding state of each key (scancode). */
/* Value 1 corresponds to key that is pressed, 0 means not pressed. */
    char *keyboard_getstate(void);
/* Force keyboard state to nothing pressed (all zeroes). */
    void keyboard_clearstate(void);
/* Let default handler translate cursor key block events to numeric keypad */
/* cursor key events and other translations. */
    void keyboard_translatekeys(int mask);

/* Return nonzero if key is depressed. */
    int keyboard_keypressed(int scancode);

#ifdef __cplusplus
}

#endif
#endif
