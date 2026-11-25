#include <assert.h>
#include <stdio.h>

int main(void) {
	FILE *fp = tmpfile();
	assert(fp);
	assert(fputs("data", fp) >= 0);
	rewind(fp);

	assert(fgetc(fp) == 'd');
	assert(fscanf(fp, "_") == 0);
	assert(fgetc(fp) == 'a');

	assert(fscanf(fp, "%%") == 0);
	assert(fgetc(fp) == 't');

	fclose(fp);
	return 0;
}
