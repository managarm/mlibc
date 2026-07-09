#include <bit>
#include <immintrin.h>
#include <stdint.h>

#include <mlibc/string.hpp>
#include <mlibc/features.hpp>

extern "C" [[gnu::visibility("protected")]]
__attribute__((target("sse2")))
int __mlibc_strcmp_sse2(const char *s1, const char *s2) {
	auto nearPageBoundary = [](uintptr_t addr) { return (addr & 0xFFF) > 4080; };

	const __m128i zero = _mm_setzero_si128();

	while (true) {
		// fast path: are both pointers safe from crossing a 4k page boundary?
		if (!nearPageBoundary(reinterpret_cast<uintptr_t>(s1))
		    && !nearPageBoundary(reinterpret_cast<uintptr_t>(s2))) [[likely]] {
			while (true) {
				auto chunk1 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(s1));
				auto chunk2 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(s2));

				auto eq_mask = _mm_cmpeq_epi8(chunk1, chunk2);
				auto null_mask_vec = _mm_cmpeq_epi8(chunk1, zero);

				// Combine: we look for any position that is NOT equal OR is a NULL terminator
				unsigned int eq = _mm_movemask_epi8(eq_mask);
				unsigned int null_mask = _mm_movemask_epi8(null_mask_vec);
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
strcmp_signature __mlibc_resolve_strcmp() {
	if (cpu_supports<x86Feature::SSE2>())
		return __mlibc_strcmp_sse2;

	return __mlibc_strcmp_default;
}

extern "C" int strcmp(const char *a, const char *b) __attribute__((ifunc("__mlibc_resolve_strcmp")));
