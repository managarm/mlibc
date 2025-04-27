#include <arpa/inet.h>
#include <assert.h>
#include <string.h>

int main() {
	struct in_addr addr;
	assert(inet_pton(AF_INET, "1.1.1.1", &addr));
	assert((addr.s_addr & 0xFF) == 1);
	assert(((addr.s_addr >> 8) & 0xFF) == 1);
	assert(((addr.s_addr >> 16) & 0xFF) == 1);
	assert(((addr.s_addr >> 24) & 0xFF) == 1);

	assert(!inet_pton(AF_INET, "256.999.1234.555", &addr));
	assert(!inet_pton(AF_INET, "a.b.c.d", &addr));

	struct in6_addr test6;
	{
		struct in6_addr addr6 = {
			.s6_addr = {
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
			},
		};
		int ret = inet_pton(AF_INET6, "::1", &test6);
		assert(ret == 1);
		assert(!memcmp(&addr6, &test6, sizeof(addr6)));
	}

	memset(&test6, 0, sizeof(test6));

	{
		struct in6_addr addr6 = {
			.s6_addr16 = {
				htons(0x2606), htons(0x4700), htons(0x4700), htons(0x00), htons(0x00), htons(0x00), htons(0x00), htons(0x1111)
			},
		};
		int ret = inet_pton(AF_INET6, "2606:4700:4700::1111", &test6);
		assert(ret == 1);
		assert(!memcmp(&addr6, &test6, sizeof(addr6)));
	}

	memset(&test6, 0, sizeof(test6));

	{
		struct in6_addr addr6 = {
			.s6_addr16 = {
				htons(0x2606), htons(0x4700), htons(0x4700), htons(0x00), htons(0x00), htons(0x00), htons(0x00), htons(0x1111)
			},
		};
		int ret = inet_pton(AF_INET6, "2606:4700:4700:0000:0000:0000:0000:1111", &test6);
		assert(ret == 1);
		assert(!memcmp(&addr6, &test6, sizeof(addr6)));
	}

	memset(&test6, 0, sizeof(test6));

	{
		struct in6_addr addr6 = {
			.s6_addr16 = {
				0, 0, 0, 0, 0, 0, 0, 0
			},
		};
		int ret = inet_pton(AF_INET6, "::", &test6);
		assert(ret == 1);
		assert(!memcmp(&addr6, &test6, sizeof(addr6)));
	}

	memset(&test6, 0, sizeof(test6));

	{
		int ret = inet_pton(AF_INET6, "mlibc::", &test6);
		assert(!ret);
	}

	memset(&test6, 0, sizeof(test6));

	{
		struct in6_addr addr6 = {
			.s6_addr = {
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0xFF, 0xFF, 204, 152, 189, 116,
			},
		};
		int ret = inet_pton(AF_INET6, "::FFFF:204.152.189.116", &test6);
		assert(ret == 1);
		assert(!memcmp(&addr6, &test6, sizeof(addr6)));
	}

	memset(&test6, 0, sizeof(test6));

	{
		int ret = inet_pton(AF_INET6, "::FFFF:204.152.189.", &test6);
		assert(!ret);
	}

	memset(&test6, 0, sizeof(test6));

	{
		int ret = inet_pton(AF_INET6, "::FFFF:204.152.189.420", &test6);
		assert(!ret);
	}

	memset(&test6, 0, sizeof(test6));

	{
		int ret = inet_pton(AF_INET6, "::FFFF:204.152.420.69", &test6);
		assert(!ret);
	}

	memset(&test6, 0, sizeof(test6));

	{
		int ret = inet_pton(AF_INET6, "1100::FFFF:204.152.189.116", &test6);
		(void) ret;
#if !defined(USE_HOST_LIBC)
		assert(!ret);
#endif
	}

	memset(&test6, 0, sizeof(test6));

	{
		struct in6_addr addr6 = {
			.s6_addr = {
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0xFF, 0xFF, 204, 152, 189, 116,
			},
		};
		int ret = inet_pton(AF_INET6, "0000:0000:0000:0000:0000:FFFF:204.152.189.116", &test6);
		assert(ret == 1);
		assert(!memcmp(&addr6, &test6, sizeof(addr6)));
	}

	return 0;
}

