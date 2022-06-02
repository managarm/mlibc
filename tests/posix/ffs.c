#include <assert.h>
#include <strings.h>
#include <limits.h>

int main(void){
    assert(ffs(0x8000) == 16);
    assert(ffs(0) == 0);
	assert(ffs(INT_MAX - 1) ==  2);
	assert(ffs(INT_MAX) == 1);
	assert(ffs(INT_MIN) == (int)(sizeof(int) * CHAR_BIT));
	assert(ffs(INT_MIN + 1) == 1);

	for (int i = 1; i < 0x1000; i++) {
		assert(ffs(i) - 1 == __builtin_ctz(i));
	}

    return 0;
}
