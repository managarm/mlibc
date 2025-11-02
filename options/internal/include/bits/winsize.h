
#ifndef _MLIBC_INTERNAL_WINSIZE_H
#define _MLIBC_INTERNAL_WINSIZE_H

struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

#endif /* _MLIBC_INTERNAL_WINSIZE_H */

