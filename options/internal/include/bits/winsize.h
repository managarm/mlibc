
#ifndef MLIBC_WINSIZE_H
#define MLIBC_WINSIZE_H

struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

#endif /* MLIBC_WINSIZE_H */

