
#ifndef _GLOB_H
#define _GLOB_H

#define RTLD_LAZY 0
#define RTLD_NOW 1
#define RTLD_GLOBAL 2
#define RTLD_LOCAL 0

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/size_t.h>

#define GLOB_APPEND 0x01
#define GLOB_DOOFFS 0x02
#define GLOB_ERR 0x04
#define GLOB_MARK 0x08
#define GLOB_NOCHECK 0x10
#define GLOB_NOESCAPE 0x20
#define GLOB_NOSORT 0x40

#define GLOB_PERIOD 0x80
#define GLOB_TILDE 0x100
#define GLOB_TILDE_CHECK 0x200

#define GLOB_ABORTED 1
#define GLOB_NOMATCH 2
#define GLOB_NOSPACE 3

typedef struct glob_t {
	size_t gl_pathc;
	char **gl_pathv;
	size_t gl_offs;
} glob_t;

int glob(const char *__restirct, int, int(*)(const char *, int), struct glob_t *__restrict);
void globfree(struct glob_t *);

#ifdef __cplusplus
}
#endif

#endif // _GLOB_H


