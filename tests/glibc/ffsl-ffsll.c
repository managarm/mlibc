#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

int main(void){
	// ffsl
	assert(ffsl(0x8000) == 16);
	assert(ffsl(0) == 0);
#if UINTPTR_MAX == UINT64_MAX
	assert(ffsl(LLONG_MAX - 1) ==  2);
	assert(ffsl(LLONG_MAX) == 1);
#endif
	assert(ffsl(LONG_MIN) == (long)(sizeof(long) * CHAR_BIT));
	assert(ffsl(LONG_MIN + 1) == 1);

	for (int i = 1; i < 0x1000; i++) {
		assert(ffsl(i) - 1 == __builtin_ctz(i));
	}

	// ffsll
	assert(ffsll(0x8000) == 16);
	assert(ffsll(0) == 0);
#if UINTPTR_MAX == UINT64_MAX
	assert(ffsll(LLONG_MAX - 1) ==  2);
	assert(ffsll(LLONG_MAX) == 1);
#endif
	assert(ffsll(LLONG_MIN) == (long long)(sizeof(long long) * CHAR_BIT));
	assert(ffsll(LLONG_MIN + 1) == 1);

	for (int i = 1; i < 0x1000; i++) {
		assert(ffsll(i) - 1 == __builtin_ctz(i));
	}

	return 0;
}
