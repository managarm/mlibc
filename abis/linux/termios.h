#ifndef _ABIBITS_TERMIOS_H
#define _ABIBITS_TERMIOS_H

typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

/* indices for the c_cc array in struct termios */
#define NCCS     32
#define VINTR    0
#define VQUIT    1
#define VERASE   2
#define VKILL    3
#define VEOF     4
#define VTIME    5
#define VMIN     6
#define VSWTC    7
#define VSTART   8
#define VSTOP    9
#define VSUSP    10
#define VEOL     11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE  14
#define VLNEXT   15
#define VEOL2    16

/* bitwise flags for c_iflag in struct termios */
#define IGNBRK 0000001
#define BRKINT 0000002
#define IGNPAR 0000004
#define PARMRK 0000010
#define INPCK 0000020
#define ISTRIP 0000040
#define INLCR 0000100
#define IGNCR 0000200
#define ICRNL 0000400
#define IUCLC 0001000
#define IXON 0002000
#define IXANY 0004000
#define IXOFF 0010000
#define IMAXBEL 0020000
#define IUTF8 0040000

/* bitwise flags for c_oflag in struct termios */
#define OPOST 0000001
#define OLCUC 0000002
#define ONLCR 0000004
#define OCRNL 0000010
#define ONOCR 0000020
#define ONLRET 0000040
#define OFILL 0000100
#define OFDEL 0000200

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || defined(_XOPEN_SOURCE)

#define NLDLY 0000400
#define NL0 0000000
#define NL1 0000400

#define CRDLY 0003000
#define CR0 0000000
#define CR1 0001000
#define CR2 0002000
#define CR3 0003000

#define TABDLY 0014000
#define TAB0 0000000
#define TAB1 0004000
#define TAB2 0010000
#define TAB3 0014000

#define BSDLY 0020000
#define BS0 0000000
#define BS1 0020000

#define FFDLY 0100000
#define FF0 0000000
#define FF1 0100000

#endif

#define VTDLY 0040000
#define VT0 0000000
#define VT1 0040000

/* bitwise constants for c_cflag in struct termios */
#define CSIZE 0000060
#define CS5 0000000
#define CS6 0000020
#define CS7 0000040
#define CS8 0000060

#define CSTOPB 0000100
#define CREAD 0000200
#define PARENB 0000400
#define PARODD 0001000
#define HUPCL 0002000
#define CLOCAL 0004000

/* bitwise constants for c_lflag in struct termios */
#define ISIG 0000001
#define ICANON 0000002
#define ECHO 0000010
#define ECHOE 0000020
#define ECHOK 0000040
#define ECHONL 0000100
#define NOFLSH 0000200
#define TOSTOP 0000400
#define IEXTEN 0100000

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)

#define EXTA    0000016
#define EXTB    0000017
#define CBAUD   0010017
#define CBAUDEX 0010000
#define CIBAUD  002003600000
#define CMSPAR  010000000000
#define CRTSCTS 020000000000

#define XCASE   0000004
#define ECHOCTL 0001000
#define ECHOPRT 0002000
#define ECHOKE  0004000
#define FLUSHO  0010000
#define PENDIN  0040000
#define EXTPROC 0200000

#define XTABS 0014000

#endif

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_line;
	cc_t c_cc[NCCS];
	speed_t ibaud;
	speed_t obaud;
};

#define NCC 8
struct termio {
	unsigned short c_iflag;
	unsigned short c_oflag;
	unsigned short c_cflag;
	unsigned short c_lflag;
	unsigned char c_line;
	unsigned char c_cc[NCC];
};

#endif
