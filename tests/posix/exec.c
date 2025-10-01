#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char **argv) {
	if (argc == 1) {
		int pid = fork();
		if (!pid)
			exit(69);
	} else {
		exit(0);
	}

	size_t len = strlen(argv[0]);
	char *program = calloc(len + 1, sizeof(char));
	memcpy(program, argv[0], len);

	char foo[] = "foo";
	char *args[] = {program, foo, NULL};

	return execve(program, args, NULL);
}
