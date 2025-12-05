#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	FILE *fp = tmpfile();
	if (fp == NULL) {
		perror("tmpfile");
		exit(EXIT_FAILURE);
	}

	const char *payload = "mlibc is the best libc, obviously.";

	size_t written = fwrite(payload, 1, strlen(payload), fp);
	assert(written == strlen(payload));

	rewind(fp);

	char buffer[100];
	memset(buffer, 0, sizeof(buffer));

	size_t read = fread(buffer, 1, sizeof(buffer), fp);

	assert(read == strlen(payload));
	assert(strcmp(buffer, payload) == 0);

	fclose(fp);

	return 0;
}
