#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <bits/ensure.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>

#include <mlibc/debug.hpp>

char *strdup(const char *string) {
	auto num_bytes = strlen(string);

	char *new_string = (char *)malloc(num_bytes + 1);
	if(!new_string) // TODO: set errno
		return nullptr;

	memcpy(new_string, string, num_bytes);
	new_string[num_bytes] = 0;
	return new_string;
}

char *strndup(const char *string, size_t max_size) {
	auto num_bytes = strnlen(string, max_size);
	char *new_string = (char *)malloc(num_bytes + 1);
	if(!new_string) // TODO: set errno
		return nullptr;

	memcpy(new_string, string, num_bytes);
	new_string[num_bytes] = 0;
	return new_string;
}

char *stpcpy(char *__restrict dest, const char *__restrict src) {
	auto n = strlen(src);
	memcpy(dest, src, n + 1);
	return dest + n;
}

char *stpncpy(char *__restrict dest, const char *__restrict src, size_t n) {
	size_t nulls, copied, srcLen = strlen(src);
	if (n >= srcLen) {
		nulls = n - srcLen;
		copied = srcLen;
	} else {
		nulls = 0;
		copied = n;
	}

	memcpy(dest, src, copied);
	memset(dest + srcLen, 0, nulls);
	return dest + n - nulls;
}

size_t strnlen(const char *s, size_t n) {
	size_t len = 0;
	while(len < n && s[len])
		++len;
	return len;
}

char *strsep(char **m, const char *del) {
	__ensure(m);

	auto tok = *m;
	if(!tok)
		return nullptr;

	// Replace the following delimiter by a null-terminator.
	// After this loop: *p is null iff we reached the end of the string.
	auto p = tok;
	while(*p && !strchr(del, *p))
		p++;

	if(*p) {
		*p = 0;
		*m = p + 1;
	}else{
		*m = nullptr;
	}
	return tok;
}

char *strsignal(int sig) {
	#define CASE_FOR(sigconst) case sigconst: s = #sigconst; break;
	const char *s;
	switch(sig) {
	CASE_FOR(SIGABRT)
	CASE_FOR(SIGFPE)
	CASE_FOR(SIGILL)
	CASE_FOR(SIGINT)
	CASE_FOR(SIGSEGV)
	CASE_FOR(SIGTERM)
	CASE_FOR(SIGPROF)
	CASE_FOR(SIGIO)
	CASE_FOR(SIGPWR)
	CASE_FOR(SIGALRM)
	CASE_FOR(SIGBUS)
	CASE_FOR(SIGCHLD)
	CASE_FOR(SIGCONT)
	CASE_FOR(SIGHUP)
	CASE_FOR(SIGKILL)
	CASE_FOR(SIGPIPE)
	CASE_FOR(SIGQUIT)
	CASE_FOR(SIGSTOP)
	CASE_FOR(SIGTSTP)
	CASE_FOR(SIGTTIN)
	CASE_FOR(SIGTTOU)
	CASE_FOR(SIGUSR1)
	CASE_FOR(SIGUSR2)
	CASE_FOR(SIGSYS)
	CASE_FOR(SIGTRAP)
	CASE_FOR(SIGURG)
	CASE_FOR(SIGVTALRM)
	CASE_FOR(SIGXCPU)
	CASE_FOR(SIGXFSZ)
	CASE_FOR(SIGWINCH)
	default:
		mlibc::infoLogger() << "mlibc: Unknown signal number " << sig << frg::endlog;
		s = "Unknown signal number";
	}
	return const_cast<char *>(s);
}

char *strcasestr(const char *s, const char *pattern) {
	size_t plen = strlen(pattern);
	const char *p = s;
	while(*p) {
		// Need strncasecmp() to avoid checking past the end of a successful match.
		if(!strncasecmp(p, pattern, plen))
			return const_cast<char *>(p);
		++p;
	}
	return nullptr;
}

void *memccpy(void *__restrict, const void *__restrict, int, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// This implementation was taken from musl
void *memrchr(const void *m, int c, size_t n) {
	const unsigned char *s = (const unsigned char *)m;
	c = (unsigned char)c;
	while(n--) {
		if(s[n] == c)
			return (void *)(s + n);
	}
	return 0;
}

char *strerror_l(int errnum, locale_t) {
	mlibc::infoLogger() << "mlibc: strerror_l locale is ignored!" << frg::endlog;
	return strerror(errnum);
}

// BSD extensions.
// Taken from musl
size_t strlcpy(char *d, const char *s, size_t n) {
	char *d0 = d;

	if(!n--)
		goto finish;
	for(; n && (*d=*s); n--, s++, d++);
	*d = 0;
finish:
	return d-d0 + strlen(s);
}

size_t strlcat(char *d, const char *s, size_t n) {
	size_t l = strnlen(d, n);
	if(l == n) {
		return l + strlen(s);
	}
	return l + strlcpy(d + l, s, n - l);
}
