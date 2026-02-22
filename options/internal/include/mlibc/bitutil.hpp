#ifndef MLIBC_BITUTIL
#define MLIBC_BITUTIL

#include <stdint.h>

namespace mlibc {

template<typename T>
struct bit_util;

template<>
struct bit_util<uint64_t> {
	static uint64_t byteswap(uint64_t x) {
		return __builtin_bswap64(x);
	}
	static uint64_t be_to_host(uint64_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		return byteswap(x);
#else
		return x;
#endif
	}
};

template<>
struct bit_util<uint32_t> {
	static uint32_t byteswap(uint32_t x) {
		return __builtin_bswap32(x);
	}
	static uint32_t be_to_host(uint32_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		return byteswap(x);
#else
		return x;
#endif
	}
};

template<>
struct bit_util<uint16_t> {
	static uint16_t byteswap(uint16_t x) {
		return __builtin_bswap16(x);
	}
	static uint16_t be_to_host(uint16_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		return byteswap(x);
#else
		return x;
#endif
	}
};

} // namespace mlibc

#endif // MLIBC_BITUTIL
