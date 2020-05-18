#ifndef _ABIBITS_TERMIOS_H
#define _ABIBITS_TERMIOS_H

// indices for the c_cc array in struct termios
#define NCCS    11
#define VEOF     0
#define VEOL     1
#define VERASE   2
#define VINTR    3
#define VKILL    4
#define VMIN     5
#define VQUIT    6
#define VSTART   7
#define VSTOP    8
#define VSUSP    9
#define VTIME   10
#define VLNEXT  15
#define VEOL2   16

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_cc[NCCS];
	speed_t ibaud;
	speed_t obaud;
};

#endif
