#ifndef _MLIBC_STDINT_H
#define _MLIBC_STDINT_H

#if defined(__cpp_static_assert)
#	define __MLIBC_STATIC_ASSERT(c, text) static_assert(c, text)
#elif !defined(__cplusplus)
#	define __MLIBC_STATIC_ASSERT(c, text) _Static_assert(c, text)
#else
#	define __MLIBC_STATIC_ASSERT(c, text)
#endif

#define __MLIBC_CHECK_TYPE(T1, T2) __MLIBC_STATIC_ASSERT(sizeof(T1) == sizeof(T2),\
	#T1 " != " #T2);

#undef __UINT_FAST8_MAX__
#undef __UINT_FAST16_MAX__
#undef __UINT_FAST32_MAX__
#undef __UINT_FAST64_MAX__

#undef __INT_FAST8_MAX__
#undef __INT_FAST16_MAX__
#undef __INT_FAST32_MAX__
#undef __INT_FAST64_MAX__

#undef __INT_FAST8_WIDTH__
#undef __INT_FAST16_WIDTH__
#undef __INT_FAST32_WIDTH__
#undef __INT_FAST64_WIDTH__

#if defined (__x86_64__) || defined (__aarch64__) || (defined (__riscv) && __riscv_xlen == 64)

#if defined (__GNUC__) && !defined (__clang__)
	__MLIBC_CHECK_TYPE(__INT8_TYPE__, __INT_FAST8_TYPE__);
	__MLIBC_CHECK_TYPE(__INT64_TYPE__, __INT_FAST16_TYPE__);
	__MLIBC_CHECK_TYPE(__INT64_TYPE__, __INT_FAST32_TYPE__);
	__MLIBC_CHECK_TYPE(__INT64_TYPE__, __INT_FAST64_TYPE__);

	__MLIBC_CHECK_TYPE(__UINT8_TYPE__, __UINT_FAST8_TYPE__);
	__MLIBC_CHECK_TYPE(__UINT64_TYPE__, __UINT_FAST16_TYPE__);
	__MLIBC_CHECK_TYPE(__UINT64_TYPE__, __UINT_FAST32_TYPE__);
	__MLIBC_CHECK_TYPE(__UINT64_TYPE__, __UINT_FAST64_TYPE__);
#endif

#undef __UINT_FAST8_TYPE__
#undef __UINT_FAST16_TYPE__
#undef __UINT_FAST32_TYPE__
#undef __UINT_FAST64_TYPE__

#undef __INT_FAST8_TYPE__
#undef __INT_FAST16_TYPE__
#undef __INT_FAST32_TYPE__
#undef __INT_FAST64_TYPE__

#define __UINT_FAST8_TYPE__ __UINT8_TYPE__
#define __UINT_FAST16_TYPE__ __UINT64_TYPE__
#define __UINT_FAST32_TYPE__ __UINT64_TYPE__
#define __UINT_FAST64_TYPE__ __UINT64_TYPE__

#define __INT_FAST8_TYPE__ __INT8_TYPE__
#define __INT_FAST16_TYPE__ __INT64_TYPE__
#define __INT_FAST32_TYPE__ __INT64_TYPE__
#define __INT_FAST64_TYPE__ __INT64_TYPE__

#define __UINT_FAST8_MAX__ __UINT8_MAX__
#define __UINT_FAST16_MAX__ __UINT64_MAX__
#define __UINT_FAST32_MAX__ __UINT64_MAX__
#define __UINT_FAST64_MAX__ __UINT64_MAX__

#define __INT_FAST8_MAX__ __INT8_MAX__
#define __INT_FAST16_MAX__ __INT64_MAX__
#define __INT_FAST32_MAX__ __INT64_MAX__
#define __INT_FAST64_MAX__ __INT64_MAX__

#define __INT_FAST8_WIDTH__ __INT8_WIDTH__
#define __INT_FAST16_WIDTH__ __INT64_WIDTH__
#define __INT_FAST32_WIDTH__ __INT64_WIDTH__
#define __INT_FAST64_WIDTH__ __INT64_WIDTH__

#elif defined (__i386__)

#if defined (__GNUC__) && !defined (__clang__)
	__MLIBC_CHECK_TYPE(__INT8_TYPE__, __INT_FAST8_TYPE__);
	__MLIBC_CHECK_TYPE(__INT32_TYPE__, __INT_FAST16_TYPE__);
	__MLIBC_CHECK_TYPE(__INT32_TYPE__, __INT_FAST32_TYPE__);
	__MLIBC_CHECK_TYPE(__INT64_TYPE__, __INT_FAST64_TYPE__);

	__MLIBC_CHECK_TYPE(__UINT8_TYPE__, __UINT_FAST8_TYPE__);
	__MLIBC_CHECK_TYPE(__UINT32_TYPE__, __UINT_FAST16_TYPE__);
	__MLIBC_CHECK_TYPE(__UINT32_TYPE__, __UINT_FAST32_TYPE__);
	__MLIBC_CHECK_TYPE(__UINT64_TYPE__, __UINT_FAST64_TYPE__);
#endif

#undef __UINT_FAST8_TYPE__
#undef __UINT_FAST16_TYPE__
#undef __UINT_FAST32_TYPE__
#undef __UINT_FAST64_TYPE__

#undef __INT_FAST8_TYPE__
#undef __INT_FAST16_TYPE__
#undef __INT_FAST32_TYPE__
#undef __INT_FAST64_TYPE__

#define __UINT_FAST8_TYPE__ __UINT8_TYPE__
#define __UINT_FAST16_TYPE__ __UINT32_TYPE__
#define __UINT_FAST32_TYPE__ __UINT32_TYPE__
#define __UINT_FAST64_TYPE__ __UINT64_TYPE__

#define __INT_FAST8_TYPE__ __INT8_TYPE__
#define __INT_FAST16_TYPE__ __INT32_TYPE__
#define __INT_FAST32_TYPE__ __INT32_TYPE__
#define __INT_FAST64_TYPE__ __INT64_TYPE__

#define __UINT_FAST8_MAX__ __UINT8_MAX__
#define __UINT_FAST16_MAX__ __UINT32_MAX__
#define __UINT_FAST32_MAX__ __UINT32_MAX__
#define __UINT_FAST64_MAX__ __UINT64_MAX__

#define __INT_FAST8_MAX__ __INT8_MAX__
#define __INT_FAST16_MAX__ __INT32_MAX__
#define __INT_FAST32_MAX__ __INT32_MAX__
#define __INT_FAST64_MAX__ __INT64_MAX__

#define __INT_FAST8_WIDTH__ __INT8_WIDTH__
#define __INT_FAST16_WIDTH__ __INT32_WIDTH__
#define __INT_FAST32_WIDTH__ __INT32_WIDTH__
#define __INT_FAST64_WIDTH__ __INT64_WIDTH__

#else

# error "Missing architecture specific code"

#endif

#if __has_include(<stdint-gcc.h>)
# include <stdint-gcc.h>
#else
# include_next <stdint.h>
#endif

#endif // _MLIBC_STDINT_H
