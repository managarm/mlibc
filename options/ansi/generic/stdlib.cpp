
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
#include <mlibc/strtol.hpp>
#include <mlibc/global-config.hpp>

#if __MLIBC_POSIX_OPTION
#include <pthread.h>
#endif // __MLIBC_POSIX_OPTION

extern "C" int __cxa_atexit(void (*function)(void *), void *argument, void *dso_tag);
void __mlibc_do_finalize();

namespace {
	// According to the first paragraph of [C11 7.22.7],
	// mblen(), mbtowc() and wctomb() have an internal state.
	// The string functions mbstowcs() and wcstombs() do *not* have this state.
	thread_local __mlibc_mbstate mblen_state = __MLIBC_MBSTATE_INITIALIZER;
	thread_local __mlibc_mbstate mbtowc_state = __MLIBC_MBSTATE_INITIALIZER;
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
	__attribute__((__returns_twice__)) int __sigsetjmp(sigjmp_buf buffer, int savesigs) {
		buffer[0].__savesigs = savesigs;
		if (savesigs)
			sigprocmask(0, NULL, &buffer[0].__sigset);
		return 0;
	}
}

__attribute__((__noreturn__)) void siglongjmp(sigjmp_buf buffer, int value) {
	if (buffer[0].__savesigs)
		sigprocmask(SIG_SETMASK, &buffer[0].__sigset, NULL);
	jmp_buf b;
	b[0].__reg_state = buffer[0].__reg_state;
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
	return mlibc::stringToInteger<long, char>(string, end, base);
}
long long strtoll(const char *__restrict string, char **__restrict end, int base) {
	return mlibc::stringToInteger<long long, char>(string, end, base);
}
unsigned long strtoul(const char *__restrict string, char **__restrict end, int base) {
	return mlibc::stringToInteger<unsigned long, char>(string, end, base);
}
unsigned long long strtoull(const char *__restrict string, char **__restrict end, int base) {
	return mlibc::stringToInteger<unsigned long long, char>(string, end, base);
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

	// alignment must be a power of two, and size % alignment must be 0
	if (alignment & (alignment - 1) || size & (alignment - 1)) {
		errno = EINVAL;
		return nullptr;
	}

	// posix_memalign requires that the alignment is a multiple of sizeof(void *)
	if (alignment < sizeof(void *))
		alignment = sizeof(void *);

	int ret = posix_memalign(&ptr, alignment, size);
	if (ret) {
		errno = ret;
		return nullptr;
	}
	return ptr;

}
void *calloc(size_t count, size_t size) {
	// we want to ensure that count*size > SIZE_MAX doesn't happen
	// to prevent overflowing, we divide both sides of the inequality by size and check with that
	if(size && count > (SIZE_MAX / size)) {
		errno = EINVAL;
		return NULL;
	}

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

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fork && mlibc::sys_waitpid &&
			mlibc::sys_execve && mlibc::sys_sigprocmask && mlibc::sys_sigaction, -1);

#if __MLIBC_POSIX_OPTION
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

		while ((err = mlibc::sys_waitpid(child, &status, 0, NULL, &unused)) < 0) {
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

static int qsort_callback(const void *a, const void *b, void *arg) {
	auto compare = reinterpret_cast<int (*)(const void *, const void *)>(arg);

	return compare(a, b);
}

void qsort(void *base, size_t count, size_t size,
		int (*compare)(const void *, const void *)) {
	return qsort_r(base, count, size, qsort_callback, (void *) compare);
}

void qsort_r(void *base, size_t count, size_t size,
		int (*compare)(const void *, const void *, void *),
		void *arg) {
	// TODO: implement a faster sort
	for(size_t i = 0; i < count; i++) {
		void *u = (void *)((uintptr_t)base + i * size);
		for(size_t j = i + 1; j < count; j++) {
			void *v = (void *)((uintptr_t)base + j * size);
			if(compare(u, v, arg) <= 0)
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

int mbtowc(wchar_t *__restrict wc, const char *__restrict mb, size_t max_size) {
	auto cc = mlibc::current_charcode();
	__ensure(max_size);

	// If wc is NULL, decode into a single local character which we discard
	// to obtain the length.
	wchar_t tmp_wc;
	if (!wc)
		wc = &tmp_wc;

	if (mb) {
		if (*mb) {
			mlibc::code_seq<wchar_t> wseq{wc, wc + 1};
			mlibc::code_seq<const char> nseq{mb, mb + frg::min(max_size, MB_CUR_MAX)};
			auto e = cc->decode_wtranscode(nseq, wseq, mbtowc_state);
			switch(e) {
				// We keep the state, so we can simply return here.
				case mlibc::charcode_error::input_underflow:
				case mlibc::charcode_error::null: {
					return nseq.it - mb;
				}
				case mlibc::charcode_error::illegal_input: {
					errno = -EILSEQ;
					return -1;
				}
				case mlibc::charcode_error::dirty: {
					mlibc::panicLogger() << "decode_wtranscode() charcode_error::dirty errors are not handled" << frg::endlog;
					break;
				}
				case mlibc::charcode_error::output_overflow: {
					mlibc::panicLogger() << "decode_wtranscode() charcode_error::output_overflow errors are not handled" << frg::endlog;
					break;
				}
			}
			__builtin_unreachable();
		} else {
			*wc = L'\0';
			return 0; // When mbs is a null byte, return 0
		}
	} else {
		mblen_state = __MLIBC_MBSTATE_INITIALIZER;
		return cc->has_shift_states;
	}
}

int wctomb(char *, wchar_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

size_t mbstowcs(wchar_t *__restrict wcs, const char *__restrict mbs, size_t wc_limit) {
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

size_t wcstombs(char *__restrict mb_string, const wchar_t *__restrict wc_string, size_t max_size) {
	const wchar_t *wcs = wc_string;
	return wcsrtombs(mb_string, &wcs, max_size, 0);
}

void free(void *ptr) {
	// TODO: Print PID only if POSIX option is enabled.
	if (mlibc::globalConfig().debugMalloc) {
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
	if (mlibc::globalConfig().debugMalloc)
		mlibc::infoLogger() << "mlibc (PID ?): malloc() returns "
				<< nptr << frg::endlog;
	return nptr;
}

void *realloc(void *ptr, size_t size) {
	auto nptr = getAllocator().reallocate(ptr, size);
	// TODO: Print PID only if POSIX option is enabled.
	if (mlibc::globalConfig().debugMalloc)
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
