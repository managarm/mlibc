#ifndef _STDINT_H
#define _STDINT_H

#include <stdint.h>
#include <bits/wchar_t.h>

/* Even though this is not strictly not-ABI, it is mlibc-printf specific therefore */
/* gate behind !__MLIBC_ABI_ONLY */
#ifndef __MLIBC_ABI_ONLY

#if UINTPTR_MAX == UINT64_MAX
#	define __PRI64 "l"
#	define __PRIPTR "l"
#else
#	define __PRI64 "ll"
#	define __PRIPTR ""
#endif

/* TODO: This is extremly unelegant and fragile. */
#define PRId8 "d"
#define PRIi8 "i"
#define PRIdLEAST8 "d"
#define PRIiLEAST8 "i"
#define PRIdFAST8 "d"
#define PRIiFAST8 "i"
#define PRId16 "d"
#define PRIi16 "i"
#define PRIdLEAST16 "d"
#define PRIiLEAST16 "i"
#define PRIdFAST16 "ld"
#define PRIiFAST16 "li"
#define PRId32 "d"
#define PRIi32 "i"
#define PRIdLEAST32 "d"
#define PRIiLEAST32 "i"
#define PRIdFAST32 "ld"
#define PRIiFAST32 "li"
#define PRId64 __PRI64 "d"
#define PRIi64 __PRI64 "i"
#define PRIdLEAST64 __PRI64 "d"
#define PRIiLEAST64 __PRI64 "i"
#define PRIdFAST64 __PRI64 "d"
#define PRIiFAST64 __PRI64 "i"
#define PRIdMAX __PRI64 "d"
#define PRIiMAX __PRI64 "i"
#define PRIdPTR __PRIPTR "d"
#define PRIiPTR __PRIPTR "i"
#define PRIo8 "o"
#define PRIu8 "u"
#define PRIx8 "x"
#define PRIX8 "X"
#define PRIoLEAST8 "o"
#define PRIuLEAST8 "u"
#define PRIxLEAST8 "x"
#define PRIXLEAST8 "X"
#define PRIoFAST8 "o"
#define PRIuFAST8 "u"
#define PRIxFAST8 "x"
#define PRIXFAST8 "X"
#define PRIo16 "o"
#define PRIu16 "u"
#define PRIx16 "x"
#define PRIX16 "X"
#define PRIoLEAST16 "o"
#define PRIuLEAST16 "u"
#define PRIxLEAST16 "x"
#define PRIXLEAST16 "X"
#define PRIoFAST16 "lo"
#define PRIuFAST16 "lu"
#define PRIxFAST16 "lx"
#define PRIXFAST16 "lX"
#define PRIo32 "o"
#define PRIu32 "u"
#define PRIx32 "x"
#define PRIX32 "X"
#define PRIoLEAST32 "o"
#define PRIuLEAST32 "u"
#define PRIxLEAST32 "x"
#define PRIXLEAST32 "X"
#define PRIoFAST32 "lo"
#define PRIuFAST32 "lu"
#define PRIxFAST32 "lx"
#define PRIXFAST32 "lX"
#define PRIo64 __PRI64 "o"
#define PRIu64 __PRI64 "u"
#define PRIx64 __PRI64 "x"
#define PRIX64 __PRI64 "X"
#define PRIoLEAST64 __PRI64 "o"
#define PRIuLEAST64 __PRI64 "u"
#define PRIxLEAST64 __PRI64 "x"
#define PRIXLEAST64 __PRI64 "X"
#define PRIoFAST64 __PRI64 "o"
#define PRIuFAST64 __PRI64 "u"
#define PRIxFAST64 __PRI64 "x"
#define PRIXFAST64 __PRI64 "X"
#define PRIoMAX __PRI64 "o"
#define PRIuMAX __PRI64 "u"
#define PRIxMAX __PRI64 "x"
#define PRIXMAX __PRI64 "X"
#define PRIoPTR __PRIPTR "o"
#define PRIuPTR __PRIPTR "u"
#define PRIxPTR __PRIPTR "x"
#define PRIXPTR __PRIPTR "X"

#define SCNu32 "u"
#define SCNu64 __PRI64 "u"
#define SCNuMAX __PRI64 "u"
#define SCNx16 "hx"
#define SCNx32 "x"
#define SCNx64 __PRI64 "x"
#define SCNxMAX __PRI64 "x"
#define SCNi8 "hhi"
#define SCNxPTR __PRIPTR "x"

#define SCNi8 "hhi"
#define SCNi64 __PRI64 "i"

#define SCNd32 "d"
#define SCNd64 __PRI64 "d"
#define SCNdFAST64 __PRI64 "d"

#define SCNu16 "hu"

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	intmax_t quot;
	intmax_t rem;
} imaxdiv_t;

#ifndef __MLIBC_ABI_ONLY

intmax_t imaxabs(intmax_t __x);
imaxdiv_t imaxdiv(intmax_t __x, intmax_t __y);
intmax_t strtoimax(const char *__restrict __string, char **__restrict __end, int __base);
uintmax_t strtoumax(const char *__restrict __string, char **__restrict __end, int __base);
intmax_t wcstoimax(const wchar_t *__restrict __string, wchar_t **__restrict __end, int __base);
uintmax_t wcstoumax(const wchar_t *__restrict __string, wchar_t **__restrict __end, int __base);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _STDINT_H */
