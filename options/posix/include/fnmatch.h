
#ifndef _FNMATCH_H
#define _FNMATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#define FNM_NOMATCH 0
#define FNM_PATHNAME 1
#define FNM_PERIOD 2
#define FNM_NOESCAPE 3

int fnmatch(const char *, const char *, int);

#ifdef __cplusplus
}
#endif

#endif // _FNMATCH_H

