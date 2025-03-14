
#include <arpa/inet.h>
#include <bits/ensure.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <mlibc/bitutil.hpp>
#include <mlibc/debug.hpp>

const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;

uint32_t htonl(uint32_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return mlibc::bit_util<uint32_t>::byteswap(x);
#else
	return x;
#endif
}
uint16_t htons(uint16_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return mlibc::bit_util<uint16_t>::byteswap(x);
#else
	return x;
#endif
}
uint32_t ntohl(uint32_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return mlibc::bit_util<uint32_t>::byteswap(x);
#else
	return x;
#endif
}
uint16_t ntohs(uint16_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return mlibc::bit_util<uint16_t>::byteswap(x);
#else
	return x;
#endif
}

// ----------------------------------------------------------------------------
// IPv4 address manipulation.
// ----------------------------------------------------------------------------
in_addr_t inet_addr(const char *p) {
	struct in_addr a;
	if(!inet_aton(p, &a))
		return -1;
	return a.s_addr;
}

in_addr_t inet_network(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *inet_ntoa(struct in_addr addr) {
	// string: xxx.yyy.zzz.aaa
	// 4 * 3 + 3 + 1 = 12 + 4 = 16
	thread_local static char buffer[16];
	uint32_t proper = htonl(addr.s_addr);
	snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d",
		(proper >> 24) & 0xff, ((proper >> 16) & 0xff),
		(proper >> 8) & 0xff, proper & 0xff);
	return buffer;
}
int inet_aton(const char *string, struct in_addr *dest) {
	int array[4];
	int i = 0;
	char *end;

	for (; i < 4; i++) {
		array[i] = strtoul(string, &end, 0);
		if (*end && *end != '.')
			return 0;
		if (!*end)
			break;
		string = end + 1;
	}

	switch (i) {
		case 0:
			dest->s_addr = htonl(array[0]);
			break;
		case 1:
			if (array[0] > 255 || array[1] > 0xffffff)
				return 0;
			dest->s_addr = htonl((array[0] << 24) | array[1]);
			break;
		case 2:
			if (array[0] > 255 || array[1] > 255 ||
					array[2] > 0xffff)
				return 0;
			dest->s_addr = htonl((array[0] << 24) | (array[1] << 16) |
				array[2]);
			break;
		case 3:
			if (array[0] > 255 || array[1] > 255 ||
					array[2] > 255 || array[3] > 255)
				return 0;
			dest->s_addr = htonl((array[0] << 24) | (array[1] << 16) |
				(array[2] << 8) | array[3]);
			break;
	}

	return 1;
}

// ----------------------------------------------------------------------------
// Generic IP address manipulation.
// ----------------------------------------------------------------------------
const char *inet_ntop(int af, const void *__restrict src, char *__restrict dst,
		socklen_t size) {
	switch (af) {
		case AF_INET: {
			auto source = reinterpret_cast<const struct in_addr*>(src);
			uint32_t addr = ntohl(source->s_addr);
			if (snprintf(dst, size, "%d.%d.%d.%d",
					(addr >> 24) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 8) & 0xff,
					addr & 0xff) < (int)size)
				return dst;
			break;
		}
		case AF_INET6: {
			auto source = reinterpret_cast<const struct in6_addr*>(src);
			size_t cur_zeroes_off = 0;
			size_t cur_zeroes_len = 0;
			size_t max_zeroes_off = 0;
			size_t max_zeroes_len = 0;

			/* we look for the largest block of zeroed quartet(s) */
			for(size_t i = 0; i < 8; i++) {
				auto ptr = source->s6_addr + (i * 2);
				if(!ptr[0] && !ptr[1]) {
					cur_zeroes_len++;
					if(max_zeroes_len < cur_zeroes_len) {
						max_zeroes_len = cur_zeroes_len;
						max_zeroes_off = cur_zeroes_off;
					}
				} else {
					/* advance the offset to the next quartet to check */
					cur_zeroes_len = 0;
					cur_zeroes_off = i + 1;
				}
			}

			size_t off = 0;
			for(size_t i = 0; i < 8; i++) {
				auto ptr = source->s6_addr + (i * 2);

				/* if we are at the beginning of the largest block of zeroed quartets, place "::" */
				if(i == max_zeroes_off && max_zeroes_len >= 2) {
					if(off < size) {
						dst[off++] = ':';
					}
					if(off < size) {
						dst[off++] = ':';
					}
					i += max_zeroes_len - 1;

					continue;
				}

				/* place a colon if we're not at the beginning of the string and it is not already there */
				if(off && dst[off - 1] != ':') {
					if(off < size) {
						dst[off++] = ':';
					}
				}

				off += snprintf(dst + off, size - off, "%x", ptr[0] << 8 | ptr[1]);
			}

			dst[off] = 0;

			return dst;
		}
		default:
			errno = EAFNOSUPPORT;
			return NULL;
	}

	errno = ENOSPC;
	return NULL;
}

int inet_pton(int af, const char *__restrict src, void *__restrict dst) {
	switch (af) {
		case AF_INET: {
			uint8_t array[4] = {};
			for (int i = 0; i < 4; i++) {
				char *end;
				long int value = strtol(src, &end, 10);
				if (value > 255)
					return 0;
				if (*end != '\0' && *end != '.')
					return 0;
				src = end + 1;
				array[i] = value;
			}
			auto addr = reinterpret_cast<struct in_addr*>(dst);
			uint32_t ip = (array[0] << 24) | (array[1] << 16) | (array[2] << 8) | array[3];
			addr->s_addr = htonl(ip);
			break;
		}
		case AF_INET6: {
			size_t i = 0;
			uint16_t array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
			frg::optional<size_t> doubleColonOffset = frg::null_opt;

			auto reservedRange = [&]() -> bool {
				if(i && ((doubleColonOffset && doubleColonOffset.value()) || !doubleColonOffset)) {
					return (ntohs(array[0]) >> 8) == 0;
				}

				return false;
			};

			for(; i < 8; i++) {
				char *end = nullptr;
				auto value = strtol(src, &end, 16);

				if (value > UINT16_MAX)
					return 0;
				if(end[0] != '\0' && end[0] != ':' && end[0] != '.')
					return 0;

				if(end[0] == '.' && reservedRange() && i < 7) {
					char *ipv4end = nullptr;
					auto value0 = strtol(src, &ipv4end, 10);
					if(ipv4end[0] != '.' || value0 > UINT8_MAX || src == ipv4end)
						return 0;
					src = ipv4end + 1;
					auto value1 = strtol(src, &ipv4end, 10);
					if(ipv4end[0] != '.' || value1 > UINT8_MAX || src == ipv4end)
						return 0;
					array[i++] = htons((value0 << 8) | value1);
					src = ipv4end + 1;

					auto value2 = strtol(src, &ipv4end, 10);
					if(ipv4end[0] != '.' || value2 > UINT8_MAX || src == ipv4end)
						return 0;
					src = ipv4end + 1;
					auto value3 = strtol(src, &ipv4end, 10);
					if(value3 > UINT8_MAX || src == ipv4end)
						return 0;
					array[i] = htons((value2 << 8) | value3);
					break;
				} else if(end[0] == ':' && end[1] == ':') {
					if(doubleColonOffset)
						return 0;
					doubleColonOffset = i + 1;
					src = end + 2;
				} else if(end[0] == ':' || end[0] == '\0') {
					src = end + 1;
				} else {
					return 0;
				}

				array[i] = htons(value);

				if(end[0] == '\0')
					break;
			}

			auto addr = reinterpret_cast<struct in6_addr *>(dst);

			if(doubleColonOffset) {
				size_t suffix = i - doubleColonOffset.value() + 1;
				memset(addr->s6_addr, 0, 16);

				for(size_t j = 0; j < doubleColonOffset.value(); j++) {
					addr->s6_addr16[j] = array[j];
				}

				for(size_t j = 0; j < suffix; j++) {
					addr->s6_addr16[8 - suffix + j] = array[doubleColonOffset.value() + j];
				}
			} else {
				for(size_t j = 0; j < 8; j++) {
					addr->s6_addr16[j] = array[j];
				}
			}

			break;
		}
		default:
			errno = EAFNOSUPPORT;
			return -1;
	}

	return 1;
}

struct in_addr inet_makeaddr(in_addr_t, in_addr_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

in_addr_t inet_netof(struct in_addr) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
