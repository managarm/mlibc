#ifndef _STDINT_H
#define _STDINT_H

#include <stdint.h>

// TODO: This is extremly unelegant and fragile.
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
#define PRId64 "ld"
#define PRIi64 "li"
#define PRIdLEAST64 "ld"
#define PRIiLEAST64 "li"
#define PRIdFAST64 "ld"
#define PRIiFAST64 "li"
#define PRIdMAX "ld"
#define PRIiMAX "li"
#define PRIdPTR "ld"
#define PRIiPTR "li"
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
#define PRIo64 "lo"
#define PRIu64 "lu"
#define PRIx64 "lx"
#define PRIX64 "lX"
#define PRIoLEAST64 "lo"
#define PRIuLEAST64 "lu"
#define PRIxLEAST64 "lx"
#define PRIXLEAST64 "lX"
#define PRIoFAST64 "lo"
#define PRIuFAST64 "lu"
#define PRIxFAST64 "lx"
#define PRIXFAST64 "lX"
#define PRIoMAX "lo"
#define PRIuMAX "lu"
#define PRIxMAX "lx"
#define PRIXMAX "lX"
#define PRIoPTR "lo"
#define PRIuPTR "lu"
#define PRIxPTR "lx"
#define PRIXPTR "lX"

#define SCNu64 "lu"
#define SCNuMAX "lu"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	intmax_t quot;
	intmax_t rem;
} imaxdiv_t;

intmax_t imaxabs(intmax_t);
imaxdiv_t imaxdiv(intmax_t, intmax_t);
intmax_t strtoimax(const char *__restrict, char **__restrict, int);
uintmax_t strtoumax(const char *__restrict, char **__restrict, int);
intmax_t wcstoimax(const __WCHAR_TYPE__ *__restrict, __WCHAR_TYPE__ **__restrict, int);
uintmax_t wcstoumax(const __WCHAR_TYPE__ *__restrict, __WCHAR_TYPE__ **__restrict, int);

#ifdef __cplusplus
}
#endif

#endif // _STDINT_H
