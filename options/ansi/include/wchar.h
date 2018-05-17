
#ifndef _WCHAR_H
#define _WCHAR_H

#include <bits/wchar_t.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __mbstate_t {
	unsigned int opaque1;
	unsigned int opaque2;
} mbstate_t;

int wcwidth(wchar_t wc);

#ifdef __cplusplus
}
#endif

#endif // _WCHAR_H

