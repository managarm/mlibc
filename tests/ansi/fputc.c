#include <assert.h>
#include <stdio.h>

int main(void) {
	FILE *file = tmpfile();
	assert(file);
	int ret = fputc('X', file);
	assert(ret == 'X');
	ret = fputc_unlocked(0x1FF, file);
	assert(ret == 0xFF);
	ret = fclose(file);
	assert(ret == 0);
	return 0;
}
