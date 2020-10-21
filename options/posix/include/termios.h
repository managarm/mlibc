
#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <abi-bits/pid_t.h>
#include <abi-bits/termios.h>

#ifdef __cplusplus
extern "C" {
#endif

// bitwise flags for c_iflag in struct termios
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

// bitwise flags for c_oflag in struct termios
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

// baud rate constants for speed_t
#define B0       0
#define B50      1
#define B75      2
#define B110     3
#define B134     4
#define B150     5
#define B200     6
#define B300     7
#define B600     8
#define B1200    9
#define B1800   10
#define B2400   11
#define B4800   12
#define B9600   13
#define B19200  14
#define B38400  15

// bitwise constants for c_cflag in struct termios
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

// bitwise constants for c_lflag in struct termios
#define ECHO    0x0001
#define ECHOE   0x0002
#define ECHOK   0x0004
#define ECHONL  0x0008
#define ICANON  0x0010
#define IEXTEN  0x0020
#define ISIG    0x0040
#define NOFLSH  0x0080
#define TOSTOP  0x0100

// constants for tcsetattr()
#define TCSANOW 1
#define TCSADRAIN 2
#define TCSAFLUSH 3

// constants for tcflush()
#define TCIFLUSH 1
#define TCIOFLUSH 2
#define TCOFLUSH 3

// constants for tcflow()
#define TCIOFF 1
#define TCION 2
#define TCOOFF 3
#define TCOON 4

speed_t cfgetispeed(const struct termios *);
speed_t cfgetospeed(const struct termios *);
int cfsetispeed(struct termios *, speed_t);
int cfsetospeed(struct termios *, speed_t);
void cfmakeraw(struct termios *);
int tcdrain(int);
int tcflow(int, int);
int tcflush(int, int);
int tcgetattr(int fd, struct termios *attr);
pid_t tcgetsid(int);
int tcsendbreak(int, int);
int tcsetattr(int, int, const struct termios *);

// This is a linux extension

#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414

struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

#define ECHOCTL 0001000
#define FLUSHO  0010000
#define IMAXBEL 0020000
#define ECHOKE  0040000

#ifdef __cplusplus
}
#endif

#endif // _TERMIOS_H

