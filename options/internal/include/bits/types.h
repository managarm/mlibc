#ifndef _MLIBC_INTERNAL_TYPES_H
#define _MLIBC_INTERNAL_TYPES_H

typedef __UINT8_TYPE__  __mlibc_uint8;
typedef __UINT16_TYPE__ __mlibc_uint16;
typedef __UINT32_TYPE__ __mlibc_uint32;
typedef __UINT64_TYPE__ __mlibc_uint64;

typedef __INT8_TYPE__  __mlibc_int8;
typedef __INT16_TYPE__ __mlibc_int16;
typedef __INT32_TYPE__ __mlibc_int32;
typedef __INT64_TYPE__ __mlibc_int64;

/* Clang and GCC have different mechanisms for INT32_C and friends. */
#ifdef __clang__
#	define __MLIBC_C_EXPAND_JOIN(x, suffix) x ## suffix
#	define __MLIBC_C_JOIN(x, suffix) __MLIBC_C_EXPAND_JOIN(x, suffix)

#	define __MLIBC_INT8_C(x)  __MLIBC_C_JOIN(x, __INT8_C_SUFFIX__)
#	define __MLIBC_INT16_C(x) __MLIBC_C_JOIN(x, __INT16_C_SUFFIX__)
#	define __MLIBC_INT32_C(x) __MLIBC_C_JOIN(x, __INT32_C_SUFFIX__)
#	define __MLIBC_INT64_C(x) __MLIBC_C_JOIN(x, __INT64_C_SUFFIX__)

#	define __MLIBC_UINT8_C(x)  __MLIBC_C_JOIN(x, __UINT8_C_SUFFIX__)
#	define __MLIBC_UINT16_C(x) __MLIBC_C_JOIN(x, __UINT16_C_SUFFIX__)
#	define __MLIBC_UINT32_C(x) __MLIBC_C_JOIN(x, __UINT32_C_SUFFIX__)
#	define __MLIBC_UINT64_C(x) __MLIBC_C_JOIN(x, __UINT64_C_SUFFIX__)

#	define __MLIBC_INTMAX_C(x) __MLIBC_C_JOIN(x, __INTMAX_C_SUFFIX__)
#	define __MLIBC_UINTMAX_C(x) __MLIBC_C_JOIN(x, __UINTMAX_C_SUFFIX__)
#else
#	define __MLIBC_INT8_C(x)  __INT8_C(x)
#	define __MLIBC_INT16_C(x) __INT16_C(x)
#	define __MLIBC_INT32_C(x) __INT32_C(x)
#	define __MLIBC_INT64_C(x) __INT64_C(x)

#	define __MLIBC_UINT8_C(x)  __UINT8_C(x)
#	define __MLIBC_UINT16_C(x) __UINT16_C(x)
#	define __MLIBC_UINT32_C(x) __UINT32_C(x)
#	define __MLIBC_UINT64_C(x) __UINT64_C(x)

#	define __MLIBC_INTMAX_C(x) __INTMAX_C(x)
#	define __MLIBC_UINTMAX_C(x) __UINTMAX_C(x)
#endif

#define __MLIBC_INT8_MAX  __INT8_MAX__
#define __MLIBC_INT16_MAX __INT16_MAX__
#define __MLIBC_INT32_MAX __INT32_MAX__
#define __MLIBC_INT64_MAX __INT64_MAX__

#define __MLIBC_INT8_MIN  (-__MLIBC_INT8_MAX - 1)
#define __MLIBC_INT16_MIN (-__MLIBC_INT16_MAX - 1)
#define __MLIBC_INT32_MIN (-__MLIBC_INT32_MAX - 1)
#define __MLIBC_INT64_MIN (-__MLIBC_INT64_MAX - 1)

#define __MLIBC_UINT8_MAX  __UINT8_MAX__
#define __MLIBC_UINT16_MAX __UINT16_MAX__
#define __MLIBC_UINT32_MAX __UINT32_MAX__
#define __MLIBC_UINT64_MAX __UINT64_MAX__

/* Fast types (signed). */

#if defined (__i386__)

typedef __mlibc_int8 __mlibc_int_fast8;
#define __MLIBC_INT_FAST8_C(x) __MLIBC_INT8_C(x)
#define __MLIBC_INT_FAST8_MAX __MLIBC_INT8_MAX
#define __MLIBC_INT_FAST8_MIN __MLIBC_INT8_MIN

typedef __mlibc_int32 __mlibc_int_fast16;
#define __MLIBC_INT_FAST16_C(x) __MLIBC_INT32_C(x)
#define __MLIBC_INT_FAST16_MAX __MLIBC_INT32_MAX
#define __MLIBC_INT_FAST16_MIN __MLIBC_INT32_MIN

typedef __mlibc_int32 __mlibc_int_fast32;
#define __MLIBC_INT_FAST32_C(x) __MLIBC_INT32_C(x)
#define __MLIBC_INT_FAST32_MAX __MLIBC_INT32_MAX
#define __MLIBC_INT_FAST32_MIN __MLIBC_INT32_MIN

typedef __mlibc_int64 __mlibc_int_fast64;
#define __MLIBC_INT_FAST64_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST64_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST64_MIN __MLIBC_INT64_MIN

#elif defined (__x86_64__)

typedef __mlibc_int8 __mlibc_int_fast8;
#define __MLIBC_INT_FAST8_C(x) __MLIBC_INT8_C(x)
#define __MLIBC_INT_FAST8_MAX __MLIBC_INT8_MAX
#define __MLIBC_INT_FAST8_MIN __MLIBC_INT8_MIN

typedef __mlibc_int64 __mlibc_int_fast16;
#define __MLIBC_INT_FAST16_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST16_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST16_MIN __MLIBC_INT64_MIN

typedef __mlibc_int64 __mlibc_int_fast32;
#define __MLIBC_INT_FAST32_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST32_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST32_MIN __MLIBC_INT64_MIN

typedef __mlibc_int64 __mlibc_int_fast64;
#define __MLIBC_INT_FAST64_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST64_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST64_MIN __MLIBC_INT64_MIN

#elif defined (__aarch64__)

typedef __mlibc_int8 __mlibc_int_fast8;
#define __MLIBC_INT_FAST8_C(x) __MLIBC_INT8_C(x)
#define __MLIBC_INT_FAST8_MAX __MLIBC_INT8_MAX
#define __MLIBC_INT_FAST8_MIN __MLIBC_INT8_MIN

typedef __mlibc_int64 __mlibc_int_fast16;
#define __MLIBC_INT_FAST16_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST16_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST16_MIN __MLIBC_INT64_MIN

typedef __mlibc_int64 __mlibc_int_fast32;
#define __MLIBC_INT_FAST32_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST32_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST32_MIN __MLIBC_INT64_MIN

typedef __mlibc_int64 __mlibc_int_fast64;
#define __MLIBC_INT_FAST64_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST64_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST64_MIN __MLIBC_INT64_MIN

#elif defined (__riscv) && __riscv_xlen == 64

typedef __mlibc_int8 __mlibc_int_fast8;
#define __MLIBC_INT_FAST8_C(x) __MLIBC_INT8_C(x)
#define __MLIBC_INT_FAST8_MAX __MLIBC_INT8_MAX
#define __MLIBC_INT_FAST8_MIN __MLIBC_INT8_MIN

typedef __mlibc_int64 __mlibc_int_fast16;
#define __MLIBC_INT_FAST16_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST16_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST16_MIN __MLIBC_INT64_MIN

typedef __mlibc_int64 __mlibc_int_fast32;
#define __MLIBC_INT_FAST32_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST32_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST32_MIN __MLIBC_INT64_MIN

typedef __mlibc_int64 __mlibc_int_fast64;
#define __MLIBC_INT_FAST64_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST64_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST64_MIN __MLIBC_INT64_MIN

#elif defined (__m68k__)

typedef __mlibc_int8 __mlibc_int_fast8;
#define __MLIBC_INT_FAST8_C(x) __MLIBC_INT8_C(x)
#define __MLIBC_INT_FAST8_MAX __MLIBC_INT8_MAX
#define __MLIBC_INT_FAST8_MIN __MLIBC_INT8_MIN

typedef __mlibc_int32 __mlibc_int_fast16;
#define __MLIBC_INT_FAST16_C(x) __MLIBC_INT16_C(x)
#define __MLIBC_INT_FAST16_MAX __MLIBC_INT16_MAX
#define __MLIBC_INT_FAST16_MIN __MLIBC_INT16_MIN

typedef __mlibc_int32 __mlibc_int_fast32;
#define __MLIBC_INT_FAST32_C(x) __MLIBC_INT32_C(x)
#define __MLIBC_INT_FAST32_MAX __MLIBC_INT32_MAX
#define __MLIBC_INT_FAST32_MIN __MLIBC_INT32_MIN

typedef __mlibc_int64 __mlibc_int_fast64;
#define __MLIBC_INT_FAST64_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST64_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST64_MIN __MLIBC_INT64_MIN

#elif defined (__loongarch64)

typedef __mlibc_int8 __mlibc_int_fast8;
#define __MLIBC_INT_FAST8_C(x) __MLIBC_INT8_C(x)
#define __MLIBC_INT_FAST8_MAX __MLIBC_INT8_MAX
#define __MLIBC_INT_FAST8_MIN __MLIBC_INT8_MIN

typedef __mlibc_int64 __mlibc_int_fast16;
#define __MLIBC_INT_FAST16_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST16_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST16_MIN __MLIBC_INT64_MIN

typedef __mlibc_int64 __mlibc_int_fast32;
#define __MLIBC_INT_FAST32_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST32_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST32_MIN __MLIBC_INT64_MIN

typedef __mlibc_int64 __mlibc_int_fast64;
#define __MLIBC_INT_FAST64_C(x) __MLIBC_INT64_C(x)
#define __MLIBC_INT_FAST64_MAX __MLIBC_INT64_MAX
#define __MLIBC_INT_FAST64_MIN __MLIBC_INT64_MIN

#else
#  error "Missing architecture specific code"
#endif

/* Fast types (unsigned). */

#if defined (__i386__)

typedef __mlibc_uint8 __mlibc_uint_fast8;
#define __MLIBC_UINT_FAST8_C(x) __MLIBC_UINT8_C(x)
#define __MLIBC_UINT_FAST8_MAX __MLIBC_UINT8_MAX
#define __MLIBC_UINT_FAST8_MIN __MLIBC_UINT8_MIN

typedef __mlibc_uint32 __mlibc_uint_fast16;
#define __MLIBC_UINT_FAST16_C(x) __MLIBC_UINT32_C(x)
#define __MLIBC_UINT_FAST16_MAX __MLIBC_UINT32_MAX
#define __MLIBC_UINT_FAST16_MIN __MLIBC_UINT32_MIN

typedef __mlibc_uint32 __mlibc_uint_fast32;
#define __MLIBC_UINT_FAST32_C(x) __MLIBC_UINT32_C(x)
#define __MLIBC_UINT_FAST32_MAX __MLIBC_UINT32_MAX
#define __MLIBC_UINT_FAST32_MIN __MLIBC_UINT32_MIN

typedef __mlibc_uint64 __mlibc_uint_fast64;
#define __MLIBC_UINT_FAST64_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST64_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST64_MIN __MLIBC_UINT64_MIN

#elif defined (__x86_64__)

typedef __mlibc_uint8 __mlibc_uint_fast8;
#define __MLIBC_UINT_FAST8_C(x) __MLIBC_UINT8_C(x)
#define __MLIBC_UINT_FAST8_MAX __MLIBC_UINT8_MAX
#define __MLIBC_UINT_FAST8_MIN __MLIBC_UINT8_MIN

typedef __mlibc_uint64 __mlibc_uint_fast16;
#define __MLIBC_UINT_FAST16_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST16_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST16_MIN __MLIBC_UINT64_MIN

typedef __mlibc_uint64 __mlibc_uint_fast32;
#define __MLIBC_UINT_FAST32_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST32_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST32_MIN __MLIBC_UINT64_MIN

typedef __mlibc_uint64 __mlibc_uint_fast64;
#define __MLIBC_UINT_FAST64_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST64_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST64_MIN __MLIBC_UINT64_MIN

#elif defined (__aarch64__)

typedef __mlibc_uint8 __mlibc_uint_fast8;
#define __MLIBC_UINT_FAST8_C(x) __MLIBC_UINT8_C(x)
#define __MLIBC_UINT_FAST8_MAX __MLIBC_UINT8_MAX
#define __MLIBC_UINT_FAST8_MIN __MLIBC_UINT8_MIN

typedef __mlibc_uint64 __mlibc_uint_fast16;
#define __MLIBC_UINT_FAST16_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST16_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST16_MIN __MLIBC_UINT64_MIN

typedef __mlibc_uint64 __mlibc_uint_fast32;
#define __MLIBC_UINT_FAST32_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST32_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST32_MIN __MLIBC_UINT64_MIN

typedef __mlibc_uint64 __mlibc_uint_fast64;
#define __MLIBC_UINT_FAST64_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST64_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST64_MIN __MLIBC_UINT64_MIN

#elif defined (__riscv) && __riscv_xlen == 64

typedef __mlibc_uint8 __mlibc_uint_fast8;
#define __MLIBC_UINT_FAST8_C(x) __MLIBC_UINT8_C(x)
#define __MLIBC_UINT_FAST8_MAX __MLIBC_UINT8_MAX
#define __MLIBC_UINT_FAST8_MIN __MLIBC_UINT8_MIN

typedef __mlibc_uint64 __mlibc_uint_fast16;
#define __MLIBC_UINT_FAST16_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST16_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST16_MIN __MLIBC_UINT64_MIN

typedef __mlibc_uint64 __mlibc_uint_fast32;
#define __MLIBC_UINT_FAST32_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST32_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST32_MIN __MLIBC_UINT64_MIN

typedef __mlibc_uint64 __mlibc_uint_fast64;
#define __MLIBC_UINT_FAST64_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST64_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST64_MIN __MLIBC_UINT64_MIN

#elif defined (__m68k__)

typedef __mlibc_uint8 __mlibc_uint_fast8;
#define __MLIBC_UINT_FAST8_C(x) __MLIBC_UINT8_C(x)
#define __MLIBC_UINT_FAST8_MAX __MLIBC_UINT8_MAX
#define __MLIBC_UINT_FAST8_MIN __MLIBC_UINT8_MIN

typedef __mlibc_uint32 __mlibc_uint_fast16;
#define __MLIBC_UINT_FAST16_C(x) __MLIBC_UINT16_C(x)
#define __MLIBC_UINT_FAST16_MAX __MLIBC_UINT16_MAX
#define __MLIBC_UINT_FAST16_MIN __MLIBC_UINT16_MIN

typedef __mlibc_uint32 __mlibc_uint_fast32;
#define __MLIBC_UINT_FAST32_C(x) __MLIBC_UINT32_C(x)
#define __MLIBC_UINT_FAST32_MAX __MLIBC_UINT32_MAX
#define __MLIBC_UINT_FAST32_MIN __MLIBC_UINT32_MIN

typedef __mlibc_uint64 __mlibc_uint_fast64;
#define __MLIBC_UINT_FAST64_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST64_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST64_MIN __MLIBC_UINT64_MIN

#elif defined (__loongarch64)

typedef __mlibc_uint8 __mlibc_uint_fast8;
#define __MLIBC_UINT_FAST8_C(x) __MLIBC_UINT8_C(x)
#define __MLIBC_UINT_FAST8_MAX __MLIBC_UINT8_MAX
#define __MLIBC_UINT_FAST8_MIN __MLIBC_UINT8_MIN

typedef __mlibc_uint64 __mlibc_uint_fast16;
#define __MLIBC_UINT_FAST16_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST16_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST16_MIN __MLIBC_UINT64_MIN

typedef __mlibc_uint64 __mlibc_uint_fast32;
#define __MLIBC_UINT_FAST32_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST32_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST32_MIN __MLIBC_UINT64_MIN

typedef __mlibc_uint64 __mlibc_uint_fast64;
#define __MLIBC_UINT_FAST64_C(x) __MLIBC_UINT64_C(x)
#define __MLIBC_UINT_FAST64_MAX __MLIBC_UINT64_MAX
#define __MLIBC_UINT_FAST64_MIN __MLIBC_UINT64_MIN

#else
#  error "Missing architecture specific code"
#endif

/* Special types. */

typedef __INTMAX_TYPE__  __mlibc_intmax;
typedef __INTPTR_TYPE__  __mlibc_intptr;
typedef __PTRDIFF_TYPE__ __mlibc_ptrdiff;
#define __MLIBC_INTMAX_MAX __INTMAX_MAX__
#define __MLIBC_INTMAX_MIN (-__INTMAX_MAX__ - 1)
#define __MLIBC_INTPTR_MAX __INTPTR_MAX__
#define __MLIBC_INTPTR_MIN (-__INTPTR_MAX__ - 1)
#define __MLIBC_PTRDIFF_MAX __PTRDIFF_MAX__
#define __MLIBC_PTRDIFF_MIN (-__PTRDIFF_MAX__ - 1)

typedef __UINTMAX_TYPE__ __mlibc_uintmax;
typedef __UINTPTR_TYPE__ __mlibc_uintptr;
typedef __SIZE_TYPE__    __mlibc_size;
#define __MLIBC_UINTMAX_MAX __UINTMAX_MAX__
#define __MLIBC_UINTPTR_MAX __UINTPTR_MAX__
#define __MLIBC_SIZE_MAX __SIZE_MAX__

/* Other limits. */

#define __MLIBC_WCHAR_MAX __WCHAR_MAX__
#define __MLIBC_WCHAR_MIN __WCHAR_MIN__

#define __MLIBC_WINT_MAX __WINT_MAX__
#define __MLIBC_WINT_MIN __WINT_MIN__

#define __MLIBC_SIG_ATOMIC_MAX __SIG_ATOMIC_MAX__
#define __MLIBC_SIG_ATOMIC_MIN __SIG_ATOMIC_MIN__

/* ---------------------------------------------------------------------------- */
/* Sanity checking. Make sure that we agree with the compiler's ABI. */
/* ---------------------------------------------------------------------------- */

#if defined(__cplusplus) && defined(__cpp_static_assert) && __cpp_static_assert >= 200410L
#	define __MLIBC_STATIC_ASSERT(c, text) static_assert(c, text)
#elif !defined(__cplusplus)
/* _Static_assert is an extension in C89/C99. */
#	define __MLIBC_STATIC_ASSERT(c, text) __extension__ _Static_assert(c, text)
#else
#	define __MLIBC_STATIC_ASSERT(c, text) extern int __static_assert_unavailable
#endif

#define __MLIBC_CHECK_TYPE(T1, T2) __MLIBC_STATIC_ASSERT(sizeof(T1) == sizeof(T2),\
	#T1 " != " #T2)

/* Least-width. */
__MLIBC_CHECK_TYPE(__mlibc_int8,  __INT_LEAST8_TYPE__);
__MLIBC_CHECK_TYPE(__mlibc_int16, __INT_LEAST16_TYPE__);
__MLIBC_CHECK_TYPE(__mlibc_int32, __INT_LEAST32_TYPE__);
__MLIBC_CHECK_TYPE(__mlibc_int64, __INT_LEAST64_TYPE__);

__MLIBC_CHECK_TYPE(__mlibc_uint8,  __UINT_LEAST8_TYPE__);
__MLIBC_CHECK_TYPE(__mlibc_uint16, __UINT_LEAST16_TYPE__);
__MLIBC_CHECK_TYPE(__mlibc_uint32, __UINT_LEAST32_TYPE__);
__MLIBC_CHECK_TYPE(__mlibc_uint64, __UINT_LEAST64_TYPE__);

/* Fast-width. */
/* Unfortunately, GCC and Clang disagree about fast types. */
#ifndef __clang__
	__MLIBC_CHECK_TYPE(__mlibc_int_fast8,  __INT_FAST8_TYPE__);
	__MLIBC_CHECK_TYPE(__mlibc_int_fast16, __INT_FAST16_TYPE__);
	__MLIBC_CHECK_TYPE(__mlibc_int_fast32, __INT_FAST32_TYPE__);
	__MLIBC_CHECK_TYPE(__mlibc_int_fast64, __INT_FAST64_TYPE__);

	__MLIBC_CHECK_TYPE(__mlibc_uint_fast8,  __UINT_FAST8_TYPE__);
	__MLIBC_CHECK_TYPE(__mlibc_uint_fast16, __UINT_FAST16_TYPE__);
	__MLIBC_CHECK_TYPE(__mlibc_uint_fast32, __UINT_FAST32_TYPE__);
	__MLIBC_CHECK_TYPE(__mlibc_uint_fast64, __UINT_FAST64_TYPE__);
#endif

#endif /* _MLIBC_INTERNAL_TYPES_H */
