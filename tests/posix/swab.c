#include <assert.h>
#include <stdint.h>
#include <unistd.h>

void test_swab_even_bytes() {
	uint8_t input[] = {0x01, 0x02, 0x03, 0x04};
	uint8_t output[4];

	swab(input, output, sizeof(input));

	assert(output[0] == 0x02);
	assert(output[1] == 0x01);
	assert(output[2] == 0x04);
	assert(output[3] == 0x03);
}

void test_swab_odd_bytes() {
	uint8_t input[] = {0x01, 0x02, 0x03, 0x04, 0x05};
	uint8_t output[5] = {0, 0, 0, 0, 0};

	swab(input, output, sizeof(input));

	assert(output[0] == 0x02);
	assert(output[1] == 0x01);
	assert(output[2] == 0x04);
	assert(output[3] == 0x03);

	// Last byte is UB, assume unchanged?
	assert(output[4] == 0);
}

void test_swab_negative_bytes() {
	uint8_t input[] = {0x01, 0x02, 0x03, 0x04};
	uint8_t output[4] = {0, 0, 0, 0};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
	swab(input, output, -1); // Should change nothing
#pragma GCC diagnostic pop

	assert(output[0] == 0);
	assert(output[1] == 0);
	assert(output[2] == 0);
	assert(output[3] == 0);
}

void test_swab_zero_bytes() {
	uint8_t input[] = {0x01, 0x02, 0x03, 0x04};
	uint8_t output[4] = {0, 0, 0, 0};

	swab(input, output, 0); // Should change nothing

	assert(output[0] == 0);
	assert(output[1] == 0);
	assert(output[2] == 0);
	assert(output[3] == 0);
}

int main() {
	test_swab_even_bytes();
	test_swab_odd_bytes();
	test_swab_negative_bytes();
	test_swab_zero_bytes();

	return 0;
}
