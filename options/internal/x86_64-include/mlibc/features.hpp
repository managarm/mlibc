#pragma once

#include <atomic>
#include <stdint.h>
#include <immintrin.h>
#include <utility>

enum class x86Feature {
	OSXSAVE,
	SSE2,
	SSE4_1,
	SSE4_2,
	AVX2,
	BMI2,
	__FEATURE_COUNT,
};

namespace detail_ {

template <x86Feature F>
struct x86FeatureTag {
	static constexpr x86Feature value = F;
};

struct cpuid_regs {
	uint32_t eax, ebx, ecx, edx;
};

[[gnu::always_inline]]
inline cpuid_regs cpuid(uint32_t leaf, uint32_t subleaf = 0) noexcept {
	cpuid_regs regs{};
	__asm__ volatile(
		"cpuid"
		: "=a"(regs.eax), "=b"(regs.ebx), "=c"(regs.ecx), "=d"(regs.edx)
		: "a"(leaf), "c"(subleaf)
	);
	return regs;
}

[[gnu::always_inline]]
inline uint32_t cpuid_max_leaf() noexcept {
	return cpuid(0).eax;
}

[[gnu::always_inline]]
inline uint64_t xcr0(void) noexcept {
	uint32_t eax, edx;
	__asm__ ("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));
	return (static_cast<uint64_t>(edx) << 32) | eax;
}

// SSE2: Leaf 1, EDX, Bit 26
[[gnu::always_inline]]
inline bool feature_supported(x86FeatureTag<x86Feature::SSE2>) noexcept {
	if (cpuid_max_leaf() < 1) [[unlikely]]
		return false;
	return (cpuid(1).edx & (1U << 26)) != 0;
}

// SSE4.1: Leaf 1, ECX, Bit 19
[[gnu::always_inline]]
inline bool feature_supported(x86FeatureTag<x86Feature::SSE4_1>) noexcept {
	if (cpuid_max_leaf() < 1) [[unlikely]]
		return false;
	return (cpuid(1).ecx & (1U << 19)) != 0;
}

// SSE4.2: Leaf 1, ECX, Bit 20
[[gnu::always_inline]]
inline bool feature_supported(x86FeatureTag<x86Feature::SSE4_2>) noexcept {
	if (cpuid_max_leaf() < 1) [[unlikely]]
		return false;
	return (cpuid(1).ecx & (1U << 20)) != 0;
}

// OSXSAVE: Leaf 1, ECX, Bit 27
[[gnu::always_inline]]
inline bool feature_supported(x86FeatureTag<x86Feature::OSXSAVE>) noexcept {
	if (cpuid_max_leaf() < 1) [[unlikely]]
		return false;
	return (cpuid(1).ecx & (1U << 27)) != 0;
}

// AVX2: Leaf 7, Subleaf 0, EBX, Bit 5
[[gnu::always_inline]]
inline bool feature_supported(x86FeatureTag<x86Feature::AVX2>) noexcept {
	if (cpuid_max_leaf() < 7)
		return false;

	// Check OSXSAVE
	if (!(cpuid(1).ecx & (1U << 27)))
		return false;
	// xcr0 bit1=XSAVE for xmm enabled, bit2=XSAVE for ymm enabled
	if ((xcr0() & 0b110) != 0b110)
		return false;

	return (cpuid(7, 0).ebx & (1U << 5)) != 0;
}

// BMI1: Leaf 7, Subleaf 0, EBX, Bit 8
[[gnu::always_inline]]
inline bool feature_supported(x86FeatureTag<x86Feature::BMI2>) noexcept {
	if (cpuid_max_leaf() < 7)
		return false;
	return (cpuid(7, 0).ebx & (1U << 8)) != 0;
}

enum class CachedSupport : uint8_t {
	Uninitialized,
	Unsupported,
	Supported,
};

inline std::atomic<CachedSupport> cache_[std::to_underlying(x86Feature::__FEATURE_COUNT)]{};

} // namespace detail_

template <x86Feature F>
[[nodiscard]] inline bool cpu_supports() noexcept {
	auto index = std::to_underlying(F);
	auto state = detail_::cache_[index].load(std::memory_order_acquire);

	if (state != detail_::CachedSupport::Uninitialized)
		return state == detail_::CachedSupport::Supported;

	bool supported = detail_::feature_supported(detail_::x86FeatureTag<F>{});
	detail_::cache_[index].store(
	    supported ? detail_::CachedSupport::Supported : detail_::CachedSupport::Unsupported,
	    std::memory_order_release
	);
	return supported;
}
