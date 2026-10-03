#include <sys/auxv.h>
#include <unistd.h>

int issetugid(void) {
	unsigned long secure;
	if (!peekauxval(AT_SECURE, &secure) && secure)
		return 1;

	return getuid() != geteuid() || getgid() != getegid();
}
