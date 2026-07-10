#include <mlibc/features.hpp>
#include <mlibc/string.hpp>

extern "C" [[gnu::visibility("protected")]]
size_t __mlibc_strlen_sse2(const char *s);
extern "C" [[gnu::visibility("protected")]]
size_t __mlibc_strlen_avx2(const char *s);

extern "C" [[gnu::visibility("protected")]]
strlen_signature __mlibc_resolve_strlen() {
	if (cpu_supports<x86Feature::AVX2>() && cpu_supports<x86Feature::BMI2>())
		return __mlibc_strlen_avx2;
	if (cpu_supports<x86Feature::SSE2>())
		return __mlibc_strlen_sse2;

	return __mlibc_strlen_default;
}

#if MLIBC_IFUNCS_SUPPORTED
extern "C" size_t strlen(const char *s) __attribute__((ifunc("__mlibc_resolve_strlen")));
#endif
