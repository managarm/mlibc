
#include <arpa/inet.h>
#include <bits/ensure.h>
#include <ctype.h>
#include <errno.h>
#include <frg/optional.hpp>
#include <frg/string.hpp>
#include <mlibc/bitutil.hpp>
#include <mlibc/debug.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

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

in_addr_t inet_network(const char *p) {
	struct in_addr a;
	if(!inet_aton(p, &a))
		return -1;
	return ntohl(a.s_addr);
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
	const char *s = string;
	unsigned long parts[4];
	size_t count = 0;

	while (true) {
		if (isspace(*s))
			return 0;

		char *end = nullptr;
		auto val = strtoul(s, &end, 0);
		if (s == end)
			return 0;

		if (s > string && *(s - 1) == '-')
			return 0;

		if (count >= 4)
			return 0;
		parts[count++] = val;

		if (*end == '.') {
			s = end + 1;
			if (*s == '\0')
				return 0;
		} else if (*end == '\0') {
			break;
		} else {
			return 0;
		}
	}

	uint32_t addr = 0;

	switch (count) {
		case 1: {
			if (parts[0] > 0xFFFFFFFF)
				return 0;
			addr = parts[0];
			break;
		}
		case 2: {
			if (parts[0] > 0xFF || parts[1] > 0xFFFFFF)
				return 0;
			addr = ((parts[0] & 0xFF) << 24) | (parts[1] & 0xFFFFFF);
			break;
		}
		case 3: {
			if (parts[0] > 0xFF || parts[1] > 0xFF || parts[2] > 0xFFFF)
				return 0;
			addr = ((parts[0] & 0xFF) << 24) | ((parts[1] & 0xFF) << 16) | (parts[2] & 0xFFFF);
			break;
		}
		case 4: {
			if (parts[0] > 0xFF || parts[1] > 0xFF || parts[2] > 0xFF || parts[3] > 0xFF)
				return 0;
			addr = ((parts[0] & 0xFF) << 24) | ((parts[1] & 0xFF) << 16) | ((parts[2] & 0xFF) << 8)
			       | (parts[3] & 0xFF);
			break;
		}
		default:
			__ensure(!"count needs to be in [1, 4]");
	}

	if (dest)
		dest->s_addr = htonl(addr);
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
			int written = snprintf(dst, size, "%u.%u.%u.%u",
					(addr >> 24) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 8) & 0xff,
					addr & 0xff);

			if (written < 0 || static_cast<size_t>(written) >= size) {
				errno = ENOSPC;
				return nullptr;
			}
			return dst;
		}
		case AF_INET6: {
			auto source = reinterpret_cast<const struct in6_addr*>(src);

			/* check if we are dealing with an IPv6-mapped IPv4 address */
			bool mapped = true;
			for (int i = 0; i < 10; ++i)
				if (source->s6_addr[i] != 0)
					mapped = false;

			if (source->s6_addr[10] != 0xff || source->s6_addr[11] != 0xff)
				mapped = false;

			if (mapped) {
				int n = snprintf(dst, size, "::ffff:%u.%u.%u.%u", source->s6_addr[12], source->s6_addr[13], source->s6_addr[14], source->s6_addr[15]);
				if (n >= 0 && static_cast<size_t>(n) < size)
					return dst;
				break;
			}

			size_t cur_zeroes_off = 0;
			size_t cur_zeroes_len = 0;
			size_t max_zeroes_off = 0;
			size_t max_zeroes_len = 0;

			/* we look for the largest block of zeroed quartet(s) */
			for(size_t i = 0; i < 8; i++) {
				auto ptr = source->s6_addr + (i * 2);
				if(!ptr[0] && !ptr[1]) {
					cur_zeroes_len++;
					if(max_zeroes_len < cur_zeroes_len && cur_zeroes_len > 1) {
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

				int ret = snprintf(dst + off, size - off, "%x", ptr[0] << 8 | ptr[1]);
				if (ret < 0 || static_cast<size_t>(ret) >= size - off) {
					errno = ENOSPC;
					return nullptr;
				}
				off += ret;
			}

			dst[off] = 0;

			return dst;
		}
		default:
			errno = EAFNOSUPPORT;
			return nullptr;
	}

	errno = ENOSPC;
	return nullptr;
}

namespace {

// Parses a dotted-quad IPv4 address that spans the entire string.
bool parseDottedQuad(frg::string_view str, uint8_t *octets, bool allowLeadingZeroes) {
	size_t start = 0;
	for (int i = 0; i < 4; i++) {
		size_t dot = str.find_first('.', start);
		if (i == 3) {
			if (dot != size_t(-1))
				return false;
			dot = str.size();
		} else if (dot == size_t(-1)) {
			return false;
		}

		auto segment = str.sub_string(start, dot - start);
		if (!segment.size() || segment.size() > 3)
			return false;
		if (!allowLeadingZeroes && segment[0] == '0' && segment.size() > 1)
			return false;
		auto value = segment.to_number<unsigned int>();
		if (!value || *value > UINT8_MAX)
			return false;

		octets[i] = *value;
		start = dot + 1;
	}

	return true;
}

unsigned int hexDigitValue(char c) {
	if (isdigit(static_cast<unsigned char>(c)))
		return c - '0';
	return tolower(static_cast<unsigned char>(c)) - 'a' + 10;
}

} // namespace

int inet_pton(int af, const char *__restrict src, void *__restrict dst) {
	switch (af) {
		case AF_INET: {
			uint8_t array[4];
			if (!parseDottedQuad(frg::string_view{src}, array, true))
				return 0;
			auto addr = reinterpret_cast<struct in_addr *>(dst);
			uint32_t ip = (array[0] << 24) | (array[1] << 16) | (array[2] << 8) | array[3];
			addr->s_addr = htonl(ip);
			break;
		}
		case AF_INET6: {
			uint16_t groups[8] = {};
			size_t n = 0;
			// Index into groups at which the "::" was found.
			frg::optional<size_t> gap = frg::null_opt;

			if (src[0] == ':') {
				if (src[1] != ':')
					return 0;
				gap = 0;
				src += 2;
			}

			while (*src) {
				if (n == 8)
					return 0;

				unsigned int value = 0;
				size_t len = 0;
				for (; isxdigit(static_cast<unsigned char>(src[len])); len++) {
					if (len == 4)
						return 0;
					value = value * 16 + hexDigitValue(src[len]);
				}
				if (!len)
					return 0;

				// An embedded IPv4 address occupies the last two groups.
				if (src[len] == '.') {
					uint8_t octets[4];
					if (n > 6 || !parseDottedQuad(frg::string_view{src}, octets, false))
						return 0;
					groups[n++] = (octets[0] << 8) | octets[1];
					groups[n++] = (octets[2] << 8) | octets[3];
					break;
				}

				groups[n++] = value;
				src += len;

				if (!*src)
					break;
				if (*src != ':')
					return 0;
				src++;

				if (*src == ':') {
					if (gap)
						return 0;
					gap = n;
					src++;
				} else if (!*src) {
					return 0;
				}
			}

			uint16_t result[8] = {};
			if (gap) {
				// "::" has to stand for at least one group.
				if (n == 8)
					return 0;
				size_t tail = n - *gap;
				for (size_t j = 0; j < *gap; j++)
					result[j] = groups[j];
				for (size_t j = 0; j < tail; j++)
					result[8 - tail + j] = groups[*gap + j];
			} else {
				if (n != 8)
					return 0;
				for (size_t j = 0; j < 8; j++)
					result[j] = groups[j];
			}

			auto addr = reinterpret_cast<struct in6_addr *>(dst);
			for (size_t j = 0; j < 8; j++) {
				addr->s6_addr[j * 2] = result[j] >> 8;
				addr->s6_addr[j * 2 + 1] = result[j] & 0xFF;
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
