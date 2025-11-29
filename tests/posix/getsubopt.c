#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *options = strdup(", ro,,,	name=x yz");
    char *value;
    char *subopts = options;
    char *const tokens[] = {"ro", "rw", "name", NULL};

    fprintf(stderr, "Testing getsubopt with string: \"%s\"\n", options);

    while (*subopts != '\0') {
        int opt = getsubopt(&subopts, tokens, &value);

        switch (opt) {
            case -1: // Error or unknown option
                fprintf(stderr, "  Unknown option or error: %s\n", value);

				// glibc does not skip commas, nor whitespace.
#if USE_HOST_LIBC || USE_CROSS_LIBC
				break;
#else
				exit(1);
#endif
            default:
                fprintf(stderr, "  Option '%s' with ", tokens[opt]);
				if (value)
					fprintf(stderr, "value '%s'\n", value);
				else
					fprintf(stderr, "no value\n");
                break;
        }
    }

    free(options);

    return 0;
}
