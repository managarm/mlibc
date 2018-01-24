
#ifndef _STRINGS_H
#define _STRINGS_H

#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

int ffs(int word);
int strcasecmp(const char *a, const char *b);
int strncasecmp(const char *a, const char *b, size_t size);

#ifdef __cplusplus
}
#endif

#endif // _STRINGS_H

