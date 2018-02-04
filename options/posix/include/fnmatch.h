
#ifndef _FNMATCH_H
#define _FNMATCH_H

#ifdef __cplusplus
extern "C" {
#endif

// POSIX-defined fnmatch() flags.
#define FNM_PATHNAME 0x1
#define FNM_NOESCAPE 0x2
#define FNM_PERIOD 0x4

// GNU extensions for fnmatch() flags.
#define	FNM_LEADING_DIR 0x8
#define	FNM_CASEFOLD 0x10

// fnmatch() return values.
#define FNM_NOMATCH 1

int fnmatch(const char *, const char *, int);

#ifdef __cplusplus
}
#endif

#endif // _FNMATCH_H

