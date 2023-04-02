
#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <abi-bits/pid_t.h>
#include <abi-bits/termios.h>

#ifdef __cplusplus
extern "C" {
#endif

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
#define B57600  16
#define B115200 17
#define B230400 18

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

#define TIOCM_DTR 0x002
#define TIOCM_RTS 0x004

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

#define TIOCGPGRP 0x540F
#define TIOCSPGRP 0x5410
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
#define TIOCGSID 0x5429

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

#define IUCLC 0001000
#define IUTF8 0040000

#ifdef __cplusplus
}
#endif

#endif // _TERMIOS_H

