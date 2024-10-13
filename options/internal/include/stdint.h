#ifndef _MLIBC_STDINT_H
#define _MLIBC_STDINT_H

#include <bits/types.h>
#include <bits/wchar.h>

/* ---------------------------------------------------------------------------- */
/* Type definitions. */
/* ---------------------------------------------------------------------------- */

/* Fixed-width (signed). */
typedef __mlibc_int8  int8_t;
typedef __mlibc_int16 int16_t;
typedef __mlibc_int32 int32_t;
typedef __mlibc_int64 int64_t;

/* Fixed-width (unsigned). */
typedef __mlibc_uint8  uint8_t;
typedef __mlibc_uint16 uint16_t;
typedef __mlibc_uint32 uint32_t;
typedef __mlibc_uint64 uint64_t;

/* Least-width (signed). */
typedef __mlibc_int8  int_least8_t;
typedef __mlibc_int16 int_least16_t;
typedef __mlibc_int32 int_least32_t;
typedef __mlibc_int64 int_least64_t;

/* Least-width (unsigned). */
typedef __mlibc_uint8  uint_least8_t;
typedef __mlibc_uint16 uint_least16_t;
typedef __mlibc_uint32 uint_least32_t;
typedef __mlibc_uint64 uint_least64_t;

/* Fast-width (signed). */
typedef __mlibc_int_fast8  int_fast8_t;
typedef __mlibc_int_fast16 int_fast16_t;
typedef __mlibc_int_fast32 int_fast32_t;
typedef __mlibc_int_fast64 int_fast64_t;

/* Fast-width (unsigned). */
typedef __mlibc_uint_fast8  uint_fast8_t;
typedef __mlibc_uint_fast16 uint_fast16_t;
typedef __mlibc_uint_fast32 uint_fast32_t;
typedef __mlibc_uint_fast64 uint_fast64_t;

/* Miscellaneous (signed). */
typedef __mlibc_intmax intmax_t;
typedef __mlibc_intptr intptr_t;

/* Miscellaneous (unsigned). */
typedef __mlibc_uintmax uintmax_t;
typedef __mlibc_uintptr uintptr_t;

/* ---------------------------------------------------------------------------- */
/* Constants. */
/* ---------------------------------------------------------------------------- */

/* Fixed-width (signed). */
#define INT8_C(x)  __MLIBC_INT8_C(x)
#define INT16_C(x) __MLIBC_INT16_C(x)
#define INT32_C(x) __MLIBC_INT32_C(x)
#define INT64_C(x) __MLIBC_INT64_C(x)
#define INTMAX_C(x) __MLIBC_INTMAX_C(x)

/* Fixed-width (unsigned). */
#define UINT8_C(x)  __MLIBC_UINT8_C(x)
#define UINT16_C(x) __MLIBC_UINT16_C(x)
#define UINT32_C(x) __MLIBC_UINT32_C(x)
#define UINT64_C(x) __MLIBC_UINT64_C(x)
#define UINTMAX_C(x) __MLIBC_UINTMAX_C(x)

/* ---------------------------------------------------------------------------- */
/* Limits. */
/* ---------------------------------------------------------------------------- */

/* Fixed-width (signed). */
#define INT8_MAX  __MLIBC_INT8_MAX
#define INT16_MAX __MLIBC_INT16_MAX
#define INT32_MAX __MLIBC_INT32_MAX
#define INT64_MAX __MLIBC_INT64_MAX

#define INT8_MIN  __MLIBC_INT8_MIN
#define INT16_MIN __MLIBC_INT16_MIN
#define INT32_MIN __MLIBC_INT32_MIN
#define INT64_MIN __MLIBC_INT64_MIN

/* Fixed-width (unsigned). */
#define UINT8_MAX  __MLIBC_UINT8_MAX
#define UINT16_MAX __MLIBC_UINT16_MAX
#define UINT32_MAX __MLIBC_UINT32_MAX
#define UINT64_MAX __MLIBC_UINT64_MAX

/* Least-width (signed). */
#define INT_LEAST8_MAX  __MLIBC_INT8_MAX
#define INT_LEAST16_MAX __MLIBC_INT16_MAX
#define INT_LEAST32_MAX __MLIBC_INT32_MAX
#define INT_LEAST64_MAX __MLIBC_INT64_MAX

#define INT_LEAST8_MIN  __MLIBC_INT8_MIN
#define INT_LEAST16_MIN __MLIBC_INT16_MIN
#define INT_LEAST32_MIN __MLIBC_INT32_MIN
#define INT_LEAST64_MIN __MLIBC_INT64_MIN

/* Least-width (unsigned). */
#define UINT_LEAST8_MAX  __MLIBC_UINT8_MAX
#define UINT_LEAST16_MAX __MLIBC_UINT16_MAX
#define UINT_LEAST32_MAX __MLIBC_UINT32_MAX
#define UINT_LEAST64_MAX __MLIBC_UINT64_MAX

/* Fast-width (signed). */
#define INT_FAST8_MAX  __MLIBC_INT_FAST8_MAX
#define INT_FAST16_MAX __MLIBC_INT_FAST16_MAX
#define INT_FAST32_MAX __MLIBC_INT_FAST32_MAX
#define INT_FAST64_MAX __MLIBC_INT_FAST64_MAX

#define INT_FAST8_MIN  __MLIBC_INT_FAST8_MIN
#define INT_FAST16_MIN __MLIBC_INT_FAST16_MIN
#define INT_FAST32_MIN __MLIBC_INT_FAST32_MIN
#define INT_FAST64_MIN __MLIBC_INT_FAST64_MIN

/* Fast-width (unsigned). */
#define UINT_FAST8_MAX  __MLIBC_UINT_FAST8_MAX
#define UINT_FAST16_MAX __MLIBC_UINT_FAST16_MAX
#define UINT_FAST32_MAX __MLIBC_UINT_FAST32_MAX
#define UINT_FAST64_MAX __MLIBC_UINT_FAST64_MAX

/* Miscellaneous (signed). */
#define INTMAX_MAX __MLIBC_INTMAX_MAX
#define INTPTR_MAX __MLIBC_INTPTR_MAX

#define INTMAX_MIN __MLIBC_INTMAX_MIN
#define INTPTR_MIN __MLIBC_INTPTR_MIN

/* Miscellaneous (unsigned). */
#define UINTMAX_MAX __MLIBC_UINTMAX_MAX
#define UINTPTR_MAX __MLIBC_UINTPTR_MAX

/* Other limits (signed). */
#define PTRDIFF_MAX    __MLIBC_PTRDIFF_MAX
#define PTRDIFF_MIN    __MLIBC_PTRDIFF_MIN
#define SIG_ATOMIC_MAX __MLIBC_SIG_ATOMIC_MAX
#define SIG_ATOMIC_MIN __MLIBC_SIG_ATOMIC_MIN
#define WINT_MAX       __MLIBC_WINT_MAX
#define WINT_MIN       __MLIBC_WINT_MIN

/* Other limits (unsigned). */
#define SIZE_MAX __MLIBC_SIZE_MAX

#endif /* _MLIBC_STDINT_H */
