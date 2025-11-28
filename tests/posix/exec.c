#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc == 1) {
		pid_t pid = fork();
		assert(pid >= 0);
		if (!pid)
			exit(69);
	} else {
		exit(0);
	}

	char *program = strdup(argv[0]);
	char foo[] = "foo";

	char *args[4];
	int i = 0;
	char *wrapper_env = getenv("MESON_EXE_WRAPPER");

	if (wrapper_env && strlen(wrapper_env) > 0)
		args[i++] = strdup(wrapper_env);

	args[i++] = program;
	args[i++] = foo;
	args[i++] = NULL;

	execve(program, args, NULL);
	perror("execve");
	return 1;
}
