#include <bit>
#include <internal-config.h>
#include <stdint.h>

#include <internal-config.h>
#include <mlibc/string.hpp>
#include <mlibc/features.hpp>

namespace {

// Vectors of bytes, with the vector extensions of GCC and Clang and the builtins that their
// <immintrin.h> wraps, as we do not use the headers of the compiler.
typedef char v16qi __attribute__((vector_size(16)));
typedef char v32qi __attribute__((vector_size(32)));

// For unaligned loads from the strings.
typedef char v16qi_u __attribute__((vector_size(16), aligned(1), may_alias));
typedef char v32qi_u __attribute__((vector_size(32), aligned(1), may_alias));

// The modes of the SSE 4.2 string comparisons, as the _SIDD_* macros of <nmmintrin.h> have them.
constexpr int siddSbyteOps = 0x02;
constexpr int siddCmpEqualEach = 0x08;
constexpr int siddNegativePolarity = 0x10;
constexpr int siddLeastSignificant = 0x00;

} // namespace

extern "C" [[gnu::visibility("protected")]]
__attribute__((target("sse2")))
int __mlibc_strcmp_sse2(const char *s1, const char *s2) {
	auto nearPageBoundary = [](uintptr_t addr) { return (addr & 0xFFF) > 4080; };

	const v16qi zero = {};

	while (true) {
		// fast path: are both pointers safe from crossing a 4k page boundary?
		if (!nearPageBoundary(reinterpret_cast<uintptr_t>(s1))
		    && !nearPageBoundary(reinterpret_cast<uintptr_t>(s2))) [[likely]] {
			while (true) {
				v16qi chunk1 = *reinterpret_cast<const v16qi_u *>(s1);
				v16qi chunk2 = *reinterpret_cast<const v16qi_u *>(s2);

				v16qi eq_mask = (v16qi)(chunk1 == chunk2);
				v16qi null_mask_vec = (v16qi)(chunk1 == zero);

				// Combine: we look for any position that is NOT equal OR is a NULL terminator
				unsigned int eq = __builtin_ia32_pmovmskb128(eq_mask);
				unsigned int null_mask = __builtin_ia32_pmovmskb128(null_mask_vec);
				unsigned int mask = (~eq & 0xFFFF) | null_mask;

				if (mask != 0) [[unlikely]] {
					int index = std::countr_zero(mask);
					return static_cast<unsigned char>(s1[index])
					       - static_cast<unsigned char>(s2[index]);
				}

				s1 += 16;
				s2 += 16;

				if (nearPageBoundary(reinterpret_cast<uintptr_t>(s1))
				    || nearPageBoundary(reinterpret_cast<uintptr_t>(s2))) [[unlikely]]
					break;
			}
		} else {
			unsigned char c1 = *s1;
			unsigned char c2 = *s2;
			if (c1 != c2)
				return c1 - c2;
			if (c1 == '\0')
				return 0;
			s1++;
			s2++;
		}
	}
}

extern "C" [[gnu::visibility("protected")]]
__attribute__((target("sse4.2")))
int __mlibc_strcmp_sse4_2(const char *s1, const char *s2) {
	auto nearPageBoundary = [](uintptr_t addr) { return (addr & 0xFFF) > 4080; };

	while (true) {
		// fast path: are both pointers safe from crossing a 4k page boundary?
		if (!nearPageBoundary(reinterpret_cast<uintptr_t>(s1))
		    && !nearPageBoundary(reinterpret_cast<uintptr_t>(s2))) [[likely]] {

			while (true) {
				v16qi chunk1 = *reinterpret_cast<const v16qi_u *>(s1);
				v16qi chunk2 = *reinterpret_cast<const v16qi_u *>(s2);

				constexpr int modeFlags = siddSbyteOps
										| siddCmpEqualEach
										| siddNegativePolarity
										| siddLeastSignificant;

				int index = __builtin_ia32_pcmpistri128(chunk1, chunk2, modeFlags);
				int mismatch_found = __builtin_ia32_pcmpistric128(chunk1, chunk2, modeFlags);
				int null_found = __builtin_ia32_pcmpistriz128(chunk1, chunk2, modeFlags);

				if (mismatch_found) {
					unsigned char c1 = s1[index];
					unsigned char c2 = s2[index];
					return c1 - c2;
				}

				if (null_found)
					return 0;

				s1 += 16;
				s2 += 16;

				if (nearPageBoundary(reinterpret_cast<uintptr_t>(s1))
				    || nearPageBoundary(reinterpret_cast<uintptr_t>(s2))) [[unlikely]]
					break;
			}
		} else {
			unsigned char c1 = *s1;
			unsigned char c2 = *s2;
			if (c1 != c2)
				return c1 - c2;
			if (c1 == '\0')
				return 0;
			s1++;
			s2++;
		}
	}
}

extern "C" [[gnu::visibility("protected")]]
__attribute__((target("avx2")))
int __mlibc_strcmp_avx2(const char *s1, const char *s2) {
	auto nearPageBoundary = [](uintptr_t addr) { return (addr & 0xFFF) > 4064; };
	const v32qi zero = {};

	while (true) {
		// fast path: are both pointers safe from crossing a 4k page boundary?
		if (!nearPageBoundary(reinterpret_cast<uintptr_t>(s1))
		    && !nearPageBoundary(reinterpret_cast<uintptr_t>(s2))) [[likely]] {

			while (true) {
				v32qi chunk1 = *reinterpret_cast<const v32qi_u *>(s1);
				v32qi chunk2 = *reinterpret_cast<const v32qi_u *>(s2);

				v32qi eq_mask = (v32qi)(chunk1 == chunk2);
				v32qi null_mask_vec = (v32qi)(chunk1 == zero);

				uint32_t eq = __builtin_ia32_pmovmskb256(eq_mask);
				uint32_t null_mask = __builtin_ia32_pmovmskb256(null_mask_vec);
				uint32_t mask = (~eq & 0xFFFFFFFF) | null_mask;

				if (mask != 0) [[unlikely]] {
					int index = std::countr_zero(mask);
					return static_cast<unsigned char>(s1[index])
					       - static_cast<unsigned char>(s2[index]);
				}

				s1 += 32;
				s2 += 32;

				if (nearPageBoundary(reinterpret_cast<uintptr_t>(s1))
				    || nearPageBoundary(reinterpret_cast<uintptr_t>(s2))) [[unlikely]] {
					break;
				}
			}
		} else {
			unsigned char c1 = *s1;
			unsigned char c2 = *s2;
			if (c1 != c2)
				return c1 - c2;
			if (c1 == '\0')
				return 0;
			s1++;
			s2++;
		}
	}
}

extern "C" [[gnu::visibility("protected")]]
strcmp_signature __mlibc_resolve_strcmp() {
	if (cpu_supports<x86Feature::AVX2>())
		return __mlibc_strcmp_avx2;
	if (cpu_supports<x86Feature::SSE4_2>())
		return __mlibc_strcmp_sse4_2;
	if (cpu_supports<x86Feature::SSE2>())
		return __mlibc_strcmp_sse2;

	return __mlibc_strcmp_default;
}

#if MLIBC_IFUNCS_SUPPORTED
extern "C" int strcmp(const char *a, const char *b) __attribute__((ifunc("__mlibc_resolve_strcmp")));
#endif
