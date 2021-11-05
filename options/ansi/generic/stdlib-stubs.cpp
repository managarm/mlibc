
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <stdio.h>
#include <wchar.h>
#include <setjmp.h>
#include <limits.h>

#include <frg/random.hpp>
#include <mlibc/debug.hpp>
#include <bits/ensure.h>
#include <bits/sigset_t.h>

#include <mlibc/allocator.hpp>
#include <mlibc/charcode.hpp>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/strtofp.hpp>

#ifdef __MLIBC_POSIX_OPTION
#include <pthread.h>
#endif // __MLIBC_POSIX_OPTION

extern "C" int __cxa_atexit(void (*function)(void *), void *argument, void *dso_tag);
void __mlibc_do_finalize();

namespace {
	// According to the first paragraph of [C11 7.22.7],
	// mblen(), mbtowc() and wctomb() have an internal state.
	// The string functions mbstowcs() and wcstombs() do *not* have this state.
	thread_local __mlibc_mbstate mblen_state = __MLIBC_MBSTATE_INITIALIZER;
}

double atof(const char *string) {
	return strtod(string, NULL);
}
int atoi(const char *string) {
	return strtol(string, nullptr, 10);
}
long atol(const char *string) {
	return strtol(string, nullptr, 10);
}
long long atoll(const char *string) {
	return strtoll(string, nullptr, 10);
}

// POSIX extensions but are here for simplicities sake. Forward declaration is here
// to avoid exporting sigprocmask when posix is disabled.
int sigprocmask(int, const sigset_t *__restrict, sigset_t *__restrict);
extern "C" {
	__attribute__ (( returns_twice )) int __sigsetjmp(sigjmp_buf buffer, int savesigs) {
		buffer[0].savesigs = savesigs;
		if (savesigs)
			sigprocmask(0, NULL, &buffer[0].sigset);
		return 0;
	}
}

__attribute__ (( noreturn )) void siglongjmp(sigjmp_buf buffer, int value) {
	if (buffer[0].savesigs)
		sigprocmask(SIG_SETMASK, &buffer[0].sigset, NULL);
	jmp_buf b;
	b[0].reg_state = buffer[0].reg_state;
	longjmp(b, value);
}

double strtod(const char *__restrict string, char **__restrict end) {
	return mlibc::strtofp<double>(string, end);
}
float strtof(const char *__restrict string, char **__restrict end) {
	return mlibc::strtofp<float>(string, end);
}
long double strtold(const char *__restrict string, char **__restrict end) {
	return mlibc::strtofp<long double>(string, end);
}
long strtol(const char *__restrict string, char **__restrict end, int base) {
//	mlibc::infoLogger() << "mlibc: strtol() called on string '" << string << "'" << frg::endlog;

	// skip leading space
	while(*string) {
		if(*string == '+')
			string++;
		if(!isspace(*string))
			break;
		string++;
	}

	bool negative = false;
	if(*string == '-') {
		negative = true;
		string++;
	}

	unsigned long number = strtoul(string, end, base);
	if(negative)
		return static_cast<long>((~number) + 1);
	return number;
}
long long strtoll(const char *__restrict string, char **__restrict end, int base) {
	static_assert(sizeof(long long) == sizeof(long));
	return strtol(string, end, base);
}
// this function is copied from newlib and available under a BSD license
unsigned long strtoul(const char *__restrict nptr, char **__restrict endptr, int base) {
	const unsigned char *s = (const unsigned char *)nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;

	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
               if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = (unsigned long)ULONG_MAX;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *) (any ? (char *)s - 1 : nptr);
	return (acc);
}
unsigned long long strtoull(const char *__restrict string, char **__restrict end, int base) {
	static_assert(sizeof(unsigned long long) == sizeof(unsigned long));
	return strtoul(string, end, base);
}

frg::mt19937 __mlibc_rand_engine;

int rand() {
	// rand() is specified to return a positive number so we discard the MSB.
	return static_cast<int>(__mlibc_rand_engine() & 0x7FFFFFFF);
}

static unsigned temper(unsigned x) {
	x ^= x >> 11;
	x ^= x << 7 & 0x9D2C5680;
	x ^= x << 15 & 0xEFC60000;
	x ^= x >> 18;
	return x;
}

int rand_r(unsigned *seed) {
	return temper(*seed = *seed * 1103515245 + 12345) / 2;
}

void srand(unsigned int s) {
	__mlibc_rand_engine.seed(s);
}

void *aligned_alloc(size_t alignment, size_t size) {
	void *ptr;
	int ret = posix_memalign(&ptr, alignment, size);
	if (ret) {
		errno = ret;
		return nullptr;
	}
	return ptr;

}
void *calloc(size_t count, size_t size) {
	// TODO: this could be done more efficient if the OS gives us already zero'd pages
	void *ptr = malloc(count * size);
	if(!ptr)
		return nullptr;
	memset(ptr, 0, count * size);
	return ptr;
}
// free() is provided by the platform
// malloc() is provided by the platform
// realloc() is provided by the platform

void abort(void) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGABRT);
	if (mlibc::sys_sigprocmask) {
		mlibc::sys_sigprocmask(SIG_UNBLOCK, &set, nullptr);
	}

	raise(SIGABRT);

	sigfillset(&set);
	sigdelset(&set, SIGABRT);
	if (mlibc::sys_sigprocmask) {
		mlibc::sys_sigprocmask(SIG_SETMASK, &set, nullptr);
	}

	struct sigaction sa;
	sa.sa_handler = SIG_DFL;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	if (mlibc::sys_sigaction(SIGABRT, &sa, nullptr))
		mlibc::panicLogger() << "mlibc: sigaction failed in abort" << frg::endlog;

	if (raise(SIGABRT))
		mlibc::panicLogger() << "mlibc: raise failed in abort" << frg::endlog;

	__builtin_trap();
}

int atexit(void (*func)(void)) {
	// TODO: the function pointer types are not compatible;
	// the conversion here is undefined behavior. its fine to do
	// this on the x86_64 abi though.
	__cxa_atexit((void (*) (void *))func, nullptr, nullptr);
	return 0;
}
int at_quick_exit(void (*func)(void)) {
	(void)func;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void exit(int status) {
	__mlibc_do_finalize();
	mlibc::sys_exit(status);
}

void _Exit(int status) {
	mlibc::sys_exit(status);
}

// getenv() is provided by POSIX
void quick_exit(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

extern char **environ;

int system(const char *command) {
	int status = -1;
	pid_t child;

	if (!mlibc::sys_fork || !mlibc::sys_waitpid || !mlibc::sys_execve
			|| !mlibc::sys_sigprocmask || !mlibc::sys_sigaction) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

#ifdef __MLIBC_POSIX_OPTION
	pthread_testcancel();
#endif // __MLIBC_POSIX_OPTION

	if (!command) {
		return 1;
	}

	struct sigaction new_sa, old_int, old_quit;
	sigset_t new_mask, old_mask;

	new_sa.sa_handler = SIG_IGN;
	new_sa.sa_flags = 0;
	sigemptyset(&new_sa.sa_mask);
	mlibc::sys_sigaction(SIGINT, &new_sa, &old_int);
	mlibc::sys_sigaction(SIGQUIT, &new_sa, &old_quit);

	sigemptyset(&new_mask);
	sigaddset(&new_mask, SIGCHLD);
	mlibc::sys_sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

	if (int e = mlibc::sys_fork(&child)) {
		errno = e;
	} else if (!child) {
		mlibc::sys_sigaction(SIGINT, &old_int, nullptr);
		mlibc::sys_sigaction(SIGQUIT, &old_quit, nullptr);
		mlibc::sys_sigprocmask(SIG_SETMASK, &old_mask, nullptr);

		const char *args[] = {
			"sh", "-c", command, nullptr
		};

		mlibc::sys_execve("/bin/sh", const_cast<char **>(args), environ);
		_Exit(127);
	} else {
		int err;
		pid_t unused;

		while ((err = mlibc::sys_waitpid(child, &status, 0, &unused)) < 0) {
			if (err == EINTR)
				continue;

			errno = err;
			status = -1;
		}
	}

	mlibc::sys_sigaction(SIGINT, &old_int, nullptr);
	mlibc::sys_sigaction(SIGQUIT, &old_quit, nullptr);
	mlibc::sys_sigprocmask(SIG_SETMASK, &old_mask, nullptr);

	return status;
}

char *mktemp(char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *bsearch(const void *key, const void *base, size_t count, size_t size,
		int (*compare)(const void *, const void *)) {
	// Invariant: Element is in the interval [i, j).
	size_t i = 0;
	size_t j = count;

	while(i < j) {
		size_t k = (j - i) / 2;
		auto element = reinterpret_cast<const char *>(base) + (i + k) * size;
		auto res = compare(key, element);
		if(res < 0) {
			j = i + k;
		}else if(res > 0) {
			i = i + k + 1;
		}else{
			return const_cast<char *>(element);
		}
	}
	__ensure(i == j);

	return nullptr;
}
void qsort(void *base, size_t count, size_t size,
		int (*compare)(const void *, const void *)) {
	// TODO: implement a faster sort
	for(size_t i = 0; i < count; i++) {
		void *u = (void *)((uintptr_t)base + i * size);
		for(size_t j = i + 1; j < count; j++) {
			void *v = (void *)((uintptr_t)base + j * size);
			if(compare(u, v) <= 0)
				continue;

			// swap u and v
			char *u_bytes = (char *)u;
			char *v_bytes = (char *)v;
			for(size_t k = 0; k < size; k++) {
				char temp = u_bytes[k];
				u_bytes[k] = v_bytes[k];
				v_bytes[k] = temp;
			}
		}
	}
}

void qsort_r(void *, size_t, size_t,
		int (*compar)(const void *, const void *, void *),
		void *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int abs(int num) {
	return num < 0 ? -num : num;
}

long labs(long num) {
	return num < 0 ? -num : num;
}

long long llabs(long long num) {
	return num < 0 ? -num : num;
}

div_t div(int number, int denom) {
	div_t r;
	r.quot = number / denom;
	r.rem = number % denom;
	return r;
}

ldiv_t ldiv(long number, long denom) {
	ldiv_t r;
	r.quot = number / denom;
	r.rem = number % denom;
	return r;
}

lldiv_t lldiv(long long number, long long denom) {
	lldiv_t r;
	r.quot = number / denom;
	r.rem = number % denom;
	return r;
}

int mblen(const char *mbs, size_t mb_limit) {
	auto cc = mlibc::current_charcode();
	wchar_t wc;
	mlibc::code_seq<const char> nseq{mbs, mbs + mb_limit};
	mlibc::code_seq<wchar_t> wseq{&wc, &wc + 1};

	if(!mbs) {
		mblen_state = __MLIBC_MBSTATE_INITIALIZER;
		return cc->has_shift_states;
	}

	if(auto e = cc->decode_wtranscode(nseq, wseq, mblen_state); e != mlibc::charcode_error::null)
		__ensure(!"decode_wtranscode() errors are not handled");
	return nseq.it - mbs;
}

int mbtowc(wchar_t *__restrict wc, const char *__restrict mbs, size_t max_size) {
	mlibc::infoLogger() << "mlibc: Broken mbtowc() called" << frg::endlog;
	__ensure(max_size);

	if(wc && mbs){
		if(*mbs){
			*wc = *mbs;
		} else {
			return 0; // When mbs is a null byte, return 0
		}
	}
	return 1;
}

int wctomb(char *, wchar_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

size_t mbstowcs(wchar_t *wcs, const char *mbs, size_t wc_limit) {
	auto cc = mlibc::current_charcode();
	__mlibc_mbstate st = __MLIBC_MBSTATE_INITIALIZER;
	mlibc::code_seq<const char> nseq{mbs, nullptr};
	mlibc::code_seq<wchar_t> wseq{wcs, wcs + wc_limit};

	if(!wcs) {
		size_t size;
		if(auto e = cc->decode_wtranscode_length(nseq, &size, st); e != mlibc::charcode_error::null)
			__ensure(!"decode_wtranscode() errors are not handled");
		return size;
	}

	if(auto e = cc->decode_wtranscode(nseq, wseq, st); e != mlibc::charcode_error::null) {
		__ensure(!"decode_wtranscode() errors are not handled");
		__builtin_unreachable();
	}else{
		size_t n = wseq.it - wcs;
		if(n < wc_limit) // Null-terminate resulting wide string.
			wcs[n] = 0;
		return n;
	}
}

size_t wcstombs(char *mb_string, const wchar_t *wc_string, size_t max_size) {
	return wcsrtombs(mb_string, &wc_string, max_size, 0);
}


void free(void *ptr) {
	// TODO: Print PID only if POSIX option is enabled.
	if(getenv("MLIBC_DEBUG_MALLOC")) {
		mlibc::infoLogger() << "mlibc (PID ?): free() on "
				<< ptr << frg::endlog;
		if((uintptr_t)ptr & 1)
			mlibc::infoLogger() << __builtin_return_address(0) << frg::endlog;
	}
	getAllocator().free(ptr);
}

void *malloc(size_t size) {
	auto nptr = getAllocator().allocate(size);
	// TODO: Print PID only if POSIX option is enabled.
	if(getenv("MLIBC_DEBUG_MALLOC"))
		mlibc::infoLogger() << "mlibc (PID ?): malloc() returns "
				<< nptr << frg::endlog;
	return nptr;
}

void *realloc(void *ptr, size_t size) {
	auto nptr = getAllocator().reallocate(ptr, size);
	// TODO: Print PID only if POSIX option is enabled.
	if(getenv("MLIBC_DEBUG_MALLOC"))
		mlibc::infoLogger() << "mlibc (PID ?): realloc() on "
				<< ptr << " returns " << nptr << frg::endlog;
	return nptr;
}

int posix_memalign(void **out, size_t align, size_t size) {
	if(align < sizeof(void *))
		return EINVAL;
	if(align & (align - 1)) // Make sure that align is a power of two.
		return EINVAL;
	auto p = getAllocator().allocate(frg::max(align, size));
	if(!p)
		return ENOMEM;
	// Hope that the alignment was respected. This works on the current allocator.
	// TODO: Make the allocator alignment-aware.
	__ensure(!(reinterpret_cast<uintptr_t>(p) & (align - 1)));
	*out = p;
	return 0;
}
