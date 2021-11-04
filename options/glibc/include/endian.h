#ifndef _ENDIAN_H
#define _ENDIAN_H

#ifdef __GNUC__
# 	define BYTE_ORDER __BYTE_ORDER__
#	define LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#	define BIG_ENDIAN __ORDER_BIG_ENDIAN__
#	define PDP_ENDIAN __ORDER_PDP_ENDIAN__
#else
#	error "Unsupported compiler"
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
#	define htobe16(x) __bswap_16(x)
#	define htole16(x) (uint16_t)(x)
#	define be16toh(x) __bswap_16(x)
#	define le16toh(x) (uint16_t)(x)

#	define htobe32(x) __bswap_32(x)
#	define htole32(x) (uint32_t)(x)
#	define be32toh(x) __bswap_32(x)
#	define le32toh(x) (uint32_t)(x)

#	define htobe64(x) __bswap_64(x)
#	define htole64(x) (uint64_t)(x)
#	define be64toh(x) __bswap_64(x)
#	define le64toh(x) (uint64_t)(x)
#else
#	error "Big endian support is missing here"
#endif

#endif // _ENDIAN_H
