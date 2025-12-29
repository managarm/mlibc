#include <assert.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

int main() {
#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
	// glibc does not support sig2str and str2sig yet
	int sigs[] = {
		SIGHUP,
		SIGINT,
		SIGQUIT,
		SIGILL,
		SIGTRAP,
		SIGABRT,
		SIGBUS,
		SIGFPE,
		SIGKILL,
		SIGUSR1,
		SIGSEGV,
		SIGUSR2,
		SIGPIPE,
		SIGALRM,
		SIGTERM,
#ifdef SIGSTKFLT
		SIGSTKFLT,
#endif
		SIGCHLD,
		SIGCONT,
		SIGSTOP,
		SIGTSTP,
		SIGTTIN,
		SIGTTOU,
		SIGURG,
		SIGXCPU,
		SIGXFSZ,
		SIGVTALRM,
		SIGPROF,
		SIGWINCH,
		SIGPOLL,
		SIGPWR,
		SIGSYS,
		SIGRTMIN,
		SIGRTMAX,
		0
	};

	const char *names[] = {
		"HUP",
		"INT",
		"QUIT",
		"ILL",
		"TRAP",
		"ABRT",
		"BUS",
		"FPE",
		"KILL",
		"USR1",
		"SEGV",
		"USR2",
		"PIPE",
		"ALRM",
		"TERM",
#ifdef SIGSTKFLT
		"STKFLT",
#endif
		"CHLD",
		"CONT",
		"STOP",
		"TSTP",
		"TTIN",
		"TTOU",
		"URG",
		"XCPU",
		"XFSZ",
		"VTALRM",
		"PROF",
		"WINCH",
		"POLL",
		"PWR",
		"SYS",
		"RTMIN",
		"RTMAX",
		0
	};

	for (size_t i = 0; sigs[i]; i++) {
		char str[SIG2STR_MAX];
		int ret = sig2str(sigs[i], str);
		assert(ret == 0);
		assert(!strcmp(names[i], str));
		int outnum = 0;
		ret = str2sig(str, &outnum);
		assert(ret == 0);
		fprintf(stderr, "%d -> '%s' -> %d\n", sigs[i], str, outnum);
		assert(outnum == sigs[i]);
	}

	char str[SIG2STR_MAX];
	int ret = sig2str(SIGRTMIN + 2, str);
	assert(ret == 0);
	assert(!strcmp("RTMIN+2", str));
	int outnum = 0;
	ret = str2sig(str, &outnum);
	assert(ret == 0);
	assert(outnum == SIGRTMIN + 2);

	ret = sig2str(SIGRTMAX + 1, str);
	assert(ret == -1);

	ret = str2sig("RTMAX-2", &outnum);
	assert(ret == 0);
	assert(outnum == SIGRTMAX - 2);

	ret = str2sig("RTMIN+1337", &outnum);
	assert(ret == -1);
	ret = str2sig("RTMAX-1337", &outnum);
	assert(ret == -1);
#endif

	return 0;
}
