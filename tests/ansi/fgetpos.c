#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	(void) argc;

	FILE *f = fopen(argv[0], "r");
	assert(f);

	int ret = fseek(f, 0, SEEK_SET);
	assert(!ret);

	fpos_t start_pos;
	ret = fgetpos(f, &start_pos);
	assert(!ret);

	ret = fseek(f, 0, SEEK_END);
	assert(!ret);

	long end_off = ftell(f);

	fpos_t end_pos;
	ret = fgetpos(f, &end_pos);
	assert(!ret);

	ret = fsetpos(f, &start_pos);
	assert(!ret);
	assert(ftell(f) == 0);

	ret = fsetpos(f, &end_pos);
	assert(!ret);
	assert(ftell(f) == end_off);

	return 0;
}
