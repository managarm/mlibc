#include <bits/sigset_t.h>
#include <bits/ensure.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stddef.h>

namespace {

template<class T> struct remove_reference { typedef T type; };
template<class T> struct remove_reference<T&> { typedef T type; };

// Assume that the struct has a member named 'sig'.
template<typename T>
struct sigset_type_helper {
	using type = typename remove_reference<decltype(T::sig[0])>::type;
	static_assert(offsetof(T, sig) == 0);
};

template<>
struct sigset_type_helper<unsigned long> { using type = unsigned long; };

template<>
struct sigset_type_helper<unsigned long long> { using type = unsigned long long; };

template<>
struct sigset_type_helper<long> { using type = long; };

template<>
struct sigset_type_helper<long long> { using type = long long; };

// Some ABIs define sigset_t as a simple integer (e.g unsigned long),
// while others define it as a struct containing an array of integers.
using sigset_underlying_type = sigset_type_helper<sigset_t>::type;

size_t signo_to_field(int signo) {
	return signo / (sizeof(sigset_underlying_type) * CHAR_BIT);
}

size_t signo_to_bit(int signo) {
	return signo % (sizeof(sigset_underlying_type) * CHAR_BIT);
}

} // namespace

int sigemptyset(sigset_t *sigset) {
	memset(sigset, 0, sizeof(*sigset));
	return 0;
}

int sigfillset(sigset_t *sigset) {
	memset(sigset, ~0, sizeof(*sigset));
	return 0;
}

int sigaddset(sigset_t *sigset, int sig) {
	int signo = sig - 1;
	if(signo < 0 || static_cast<unsigned int>(signo) >= (sizeof(sigset_t) * CHAR_BIT)) {
		errno = EINVAL;
		return -1;
	}
	auto ptr = reinterpret_cast<sigset_underlying_type *>(sigset);
	auto field = signo_to_field(signo);
	auto bit = signo_to_bit(signo);
	ptr[field] |= (1UL << bit);
	return 0;
}

int sigdelset(sigset_t *sigset, int sig) {
	int signo = sig - 1;
	if(signo < 0 || static_cast<unsigned int>(signo) >= (sizeof(sigset_t) * CHAR_BIT)) {
		errno = EINVAL;
		return -1;
	}
	auto ptr = reinterpret_cast<sigset_underlying_type *>(sigset);
	auto field = signo_to_field(signo);
	auto bit = signo_to_bit(signo);
	ptr[field] &= ~(1UL << bit);
	return 0;
}

int sigismember(const sigset_t *sigset, int sig) {
	int signo = sig - 1;
	if(signo < 0 || static_cast<unsigned int>(signo) >= (sizeof(sigset_t) * CHAR_BIT)) {
		errno = EINVAL;
		return -1;
	}
	auto ptr = reinterpret_cast<const sigset_underlying_type *>(sigset);
	auto field = signo_to_field(signo);
	auto bit = signo_to_bit(signo);
	return (ptr[field] & (1UL << bit)) != 0;
}
