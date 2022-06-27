#include <assert.h>
#include <arpa/nameser.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
	uint8_t *buf = malloc(256);
	uint16_t original16 = 4891;
	uint32_t original32 = 4861984;
	memset(buf, 0x0, sizeof(*buf));
	ns_put16(original16, buf);
	uint16_t result16 = ns_get16(buf);
	assert(result16 == original16);
	memset(buf, 0x0, sizeof(*buf));
	ns_put32(original32, buf);
	uint32_t result32 = ns_get32(buf);
	assert(result32 == original32);
	free(buf);
	return 0;
}
