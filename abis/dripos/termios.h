#ifndef _ABIBITS_TERMIOS_H
#define _ABIBITS_TERMIOS_H

#if __MLIBC_BUILDING_MLIBC
#warning termios.h is a broken header originally from the deprecated "mlibc" ABI. \
    We suggest to use abis/linux/termios.h instead. \
    Note that this will potentially require kernel changes.
#endif

typedef unsigned int cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

/* indices for the c_cc array in struct termios */
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

/* bitwise flags for c_iflag in struct termios */
#define BRKINT  0x0001
#define ICRNL   0x0002
#define IGNBRK  0x0004
#define IGNCR   0x0008
#define IGNPAR  0x0010
#define INLCR   0x0020
#define INPCK   0x0040
#define ISTRIP  0x0080
#define IXANY   0x0100
#define IXOFF   0x0200
#define IXON    0x0400
#define PARMRK  0x0800

/* bitwise flags for c_oflag in struct termios */
#define OPOST   0x0001
#define ONLCR   0x0002
#define OCRNL   0x0004
#define ONOCR   0x0008
#define ONLRET  0x0010
#define OFDEL   0x0020
#define OFILL   0x0040

#define NLDLY   0x0080
#define NL0     0x0000
#define NL1     0x0080

#define CRDLY   0x0300
#define CR0     0x0000
#define CR1     0x0100
#define CR2     0x0200
#define CR3     0x0300

#define TABDLY  0x0C00
#define TAB0    0x0000
#define TAB1    0x0400
#define TAB2    0x0800
#define TAB3    0x0C00

#define BSDLY   0x1000
#define BS0     0x0000
#define BS1     0x1000

#define VTDLY   0x2000
#define VT0     0x0000
#define VT1     0x2000

#define FFDLY   0x4000
#define FF0     0x0000
#define FF1     0x4000

/* bitwise constants for c_cflag in struct termios */
#define CSIZE   0x0003
#define CS5     0x0000
#define CS6     0x0001
#define CS7     0x0002
#define CS8     0x0003

#define CSTOPB  0x0004
#define CREAD   0x0008
#define PARENB  0x0010
#define PARODD  0x0020
#define HUPCL   0x0040
#define CLOCAL  0x0080

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define CBAUD   0x100F
#endif

/* bitwise constants for c_lflag in struct termios */
#define ECHO    0x0001
#define ECHOE   0x0002
#define ECHOK   0x0004
#define ECHONL  0x0008
#define ICANON  0x0010
#define IEXTEN  0x0020
#define ISIG    0x0040
#define NOFLSH  0x0080
#define TOSTOP  0x0100
#define ECHOPRT 0x0200

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
