#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void testfor(const char *expected, char *shell, int *count) {
	if (strcmp(shell, expected) == 0)
		*count += 1;
}

#define BUFFER_SIZE (1024 * 512)

int main(void) {
	setusershell();

	FILE *f = fopen("/etc/shells", "r");
	if (f == NULL) {
		/* /etc/shells file is unreadable, test for the existance of /bin/sh and /bin/csh */
		char *shell = getusershell();
		int csh = 0;
		int sh = 0;

		testfor("/bin/sh", shell, &sh);
		testfor("/bin/csh", shell, &csh);

		shell = getusershell();
		assert(shell);

		testfor("/bin/sh", shell, &sh);
		testfor("/bin/csh", shell, &csh);

		assert(sh == 1 && csh == 1);
		assert(getusershell() == NULL);
	} else {
		/* /etc/shells is there, read the whole file into a buffer and change all '\n's to '\0'. */
		char buffer[BUFFER_SIZE + 1];
		int nbytes = fread(buffer, 1, BUFFER_SIZE, f);
		buffer[nbytes] = '\0';

		for (int i = 0; i < nbytes; ++i) {
			if (buffer[i] == '\n')
				buffer[i] = '\0';
		}

		/* loop through each shell and make sure the result is right */
		char *shell = buffer;
		while (shell < &buffer[nbytes]) {
			char *usershell = getusershell();
			while (usershell && *usershell == '#')
				usershell = getusershell();

			while (*shell == '#')
				shell += strlen(shell) + 1;

			assert(strcmp(shell, usershell) == 0);
			shell += strlen(shell) + 1;
		}
	}

	endusershell();

	return 0;
}
