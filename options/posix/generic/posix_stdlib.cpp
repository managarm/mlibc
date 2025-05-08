
#include <abi-bits/fcntl.h>
#include <bits/ensure.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <frg/small_vector.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/rtld-config.hpp>

namespace {
	constexpr bool debugPathResolution = false;
}

// Borrowed from musl
static uint32_t init[] = {
0x00000000,0x5851f42d,0xc0b18ccf,0xcbb5f646,
0xc7033129,0x30705b04,0x20fd5db4,0x9a8b7f78,
0x502959d8,0xab894868,0x6c0356a7,0x88cdb7ff,
0xb477d43f,0x70a3a52b,0xa8e4baf1,0xfd8341fc,
0x8ae16fd9,0x742d2f7a,0x0d1f0796,0x76035e09,
0x40f7702c,0x6fa72ca5,0xaaa84157,0x58a0df74,
0xc74a0364,0xae533cc4,0x04185faf,0x6de3b115,
0x0cab8628,0xf043bfa4,0x398150e9,0x37521657};

static int n = 31;
static int i = 3;
static int j = 0;
static uint32_t *x = init + 1;


static uint32_t lcg31(uint32_t x) {
	return (1103515245 * x + 12345) & 0x7fffffff;
}

static uint64_t lcg64(uint64_t x) {
	return 6364136223846793005ull * x + 1;
}

static void *savestate(void) {
	x[-1] = (n << 16) | (i << 8) | j;
	return x - 1;
}

static void loadstate(uint32_t *state) {
	x = state + 1;
	n = x[-1] >> 16;
	i = (x[-1] >> 8) & 0xff;
	j = x[-1] & 0xff;
}

long random(void) {
	long k;

	if(n == 0) {
		k = x[0] = lcg31(x[0]);
		return k;
	}
	x[i] += x[j];
	k = x[i] >> 1;
	if(++i == n)
		i = 0;
	if(++j == n)
		j = 0;

	return k;
}

// erand, drand and srand are borrowed from musl
namespace {

unsigned short seed_48[7] = { 0, 0, 0, 0xe66d, 0xdeec, 0x5, 0xb };

uint64_t eand48_step(unsigned short *xi, unsigned short *lc) {
	uint64_t x = xi[0] | (xi[1] + 0U) << 16 | (xi[2] + 0ULL) << 32;
	uint64_t a = lc[0] | (lc[1] + 0U) << 16 | (lc[2] + 0ULL) << 32;
	x = a*x + lc[3];
	xi[0] = x;
	xi[1] = x>>16;
	xi[2] = x>>32;
	return x & 0xffffffffffffull;
}

} // namespace

double erand48(unsigned short s[3]) {
	union {
		uint64_t u;
		double f;
	} x = { 0x3ff0000000000000ULL | eand48_step(s, seed_48+3)<<4 };
	return x.f - 1.0;
}

double drand48(void) {
	return erand48(seed_48);
}

unsigned short *seed48(unsigned short *s) {
	static unsigned short p[3];
	memcpy(p, seed_48, sizeof p);
	memcpy(seed_48, s, sizeof p);
	return p;
}

void srand48(long int seed) {
	unsigned short arr[3] = { 0x330e, (unsigned short) seed, (unsigned short) (seed>>16) };
	seed48(arr);
}

long jrand48(unsigned short [3]) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long int mrand48(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// Borrowed from musl
void srandom(unsigned int seed) {
	int k;
	uint64_t s = seed;

	if(n == 0) {
		x[0] = s;
		return;
	}
	i = n == 31 || n == 7 ? 3 : 1;
	j = 0;
	for(k = 0; k < n; k++) {
		s = lcg64(s);
		x[k] = s >> 32;
	}
	// Make sure x contains at least one odd number
	x[0] |= 1;
}

char *initstate(unsigned int seed, char *state, size_t size) {
	void *old;

	if(size < 8)
		return 0;
	old = savestate();
	if(size < 32)
		n = 0;
	else if(size < 64)
		n = 7;
	else if(size < 128)
		n = 15;
	else if(size < 256)
		n = 31;
	else
		n = 63;
	x = (uint32_t *)state + 1;
	srandom(seed);
	savestate();
	return (char *)old;
}

char *setstate(char *state) {
	void *old;

	old = savestate();
	loadstate((uint32_t *)state);
	return (char *)old;
}

// ----------------------------------------------------------------------------
// Path handling.
// ----------------------------------------------------------------------------


int mkostemps(char *pattern, int suffixlen, int flags) {
	auto n = strlen(pattern);
	if(n < (6 + static_cast<size_t>(suffixlen))) {
		errno = EINVAL;
		return -1;
	}

	flags &= ~O_WRONLY;

	for(size_t i = 0; i < 6; i++) {
		if(pattern[n - (6 + suffixlen) + i] == 'X')
			continue;
		errno = EINVAL;
		return -1;
	}

	// TODO: Do an exponential search.
	for(size_t i = 0; i < 999999; i++) {
		char sfx = pattern[n - suffixlen];
		__ensure(sprintf(pattern + (n - (6 + suffixlen)), "%06zu", i) == 6);
		pattern[n - suffixlen] = sfx;

		int fd;
		if(int e = mlibc::sys_open(pattern, O_RDWR | O_CREAT | O_EXCL | flags, S_IRUSR | S_IWUSR, &fd); !e) {
			return fd;
		}else if(e != EEXIST) {
			errno = e;
			return -1;
		}
	}

	errno = EEXIST;
	return -1;
}

int mkostemp(char *pattern, int flags) {
	return mkostemps(pattern, flags, 0);
}

int mkstemp(char *path) {
	return mkostemp(path, 0);
}

int mkstemps(char *pattern, int suffixlen) {
	return mkostemps(pattern, suffixlen, 0);
}

char *mkdtemp(char *pattern) {
	mlibc::infoLogger() << "mlibc mkdtemp(" << pattern << ") called" << frg::endlog;
	auto n = strlen(pattern);
	__ensure(n >= 6);
	if(n < 6) {
		errno = EINVAL;
		return NULL;
	}
	for(size_t i = 0; i < 6; i++) {
		if(pattern[n - 6 + i] == 'X')
			continue;
		errno = EINVAL;
		return NULL;
	}

	// TODO: Do an exponential search.
	for(size_t i = 0; i < 999999; i++) {
		__ensure(sprintf(pattern + (n - 6), "%06zu", i) == 6);
		if(int e = mlibc::sys_mkdir(pattern, S_IRWXU); !e) {
			return pattern;
		}else if(e != EEXIST) {
			errno = e;
			return NULL;
		}
	}

	errno = EEXIST;
	return NULL;
}

char *realpath(const char *path, char *out) {
	if(debugPathResolution)
		mlibc::infoLogger() << "mlibc realpath(): Called on '" << path << "'" << frg::endlog;
	frg::string_view path_view{path};

	// In case of the root, the string only contains the null-terminator.
	frg::small_vector<char, PATH_MAX, MemoryAllocator> resolv{getAllocator()};
	size_t ps;

	// If the path is relative, we have to preprend the working directory.
	if(path[0] == '/') {
		resolv.push_back(0);
		ps = 1;
	}else{
		// Try to getcwd() until the buffer is large enough.
		resolv.resize(128);
		int saved_errno = errno;
		while(true) {
			// getcwd could smash errno on failure + resize (ERANGE) + success,
			// so we have to save and restore errno in that scenario.
			char *ret = getcwd(resolv.data(), resolv.size());
			if(ret != NULL) {
				break;
			}

			if(errno == ERANGE) {
				errno = saved_errno;
				resolv.resize(2 * resolv.size());
			}else{
				return nullptr;
			}
		}
		frg::string_view cwd_view{resolv.data()};
		if(cwd_view == "/") {
			// Restore our invariant that we only store the null-terminator for the root.
			resolv.resize(1);
			resolv[0] = 0;
		}else{
			resolv.resize(cwd_view.size() + 1);
		}
		ps = 0;
	}

	// Contains unresolved links as a relative path compared to resolv.
	frg::small_vector<char, PATH_MAX, MemoryAllocator> lnk{getAllocator()};
	size_t ls = 0;

	auto process_segment = [&] (frg::string_view s_view) -> int {
		if(debugPathResolution)
			mlibc::infoLogger() << "mlibc realpath(): resolv is '" << resolv.data() << "'"
					<< ", segment is " << s_view.data()
					<< ", size: " << s_view.size() << frg::endlog;

		if(!s_view.size() || s_view == ".") {
			// Keep resolv invariant.
			return 0;
		}else if(s_view == "..") {
			// Remove a single segment from resolv.
			if(resolv.size() > 1) {
				auto slash = strrchr(resolv.data(), '/');
				__ensure(slash); // We never remove the leading sla.
				resolv.resize((slash - resolv.data()) + 1);
				*slash = 0; // Replace the slash by a null-terminator.
			}
			return 0;
		}

		// Append the segment to resolv.
		auto rsz = resolv.size();
		resolv[rsz - 1] = '/'; // Replace null-terminator by a slash.
		resolv.resize(rsz + s_view.size() + 1);
		memcpy(resolv.data() + rsz, s_view.data(), s_view.size());
		resolv[rsz + s_view.size()] = 0;

		// stat() the path to (1) see if it exists and (2) see if it is a link.
		if(!mlibc::sys_stat) {
			MLIBC_MISSING_SYSDEP();
			return ENOSYS;
		}
		if(debugPathResolution)
			mlibc::infoLogger() << "mlibc realpath(): stat()ing '"
					<< resolv.data() << "'" << frg::endlog;
		struct stat st;
		if(int e = mlibc::sys_stat(mlibc::fsfd_target::path,
				-1, resolv.data(), AT_SYMLINK_NOFOLLOW, &st); e)
			return e;

		if(S_ISLNK(st.st_mode)) {
			if(debugPathResolution) {
				mlibc::infoLogger() << "mlibc realpath(): Encountered symlink '"
					<< resolv.data() << "'" << frg::endlog;
			}

			if(!mlibc::sys_readlink) {
				MLIBC_MISSING_SYSDEP();
				return ENOSYS;
			}

			ssize_t sz = 0;
			char path[512];

			if (int e = mlibc::sys_readlink(resolv.data(), path, 512, &sz); e)
				return e;

			if(debugPathResolution) {
				mlibc::infoLogger() << "mlibc realpath(): Symlink resolves to '"
					<< frg::string_view{path, static_cast<size_t>(sz)} << "'" << frg::endlog;
			}

			if (path[0] == '/') {
				// Absolute path, replace resolv

				// Ignore any trailing '/' so all results will not have one to keep consistency.
				while(sz > 1 && path[sz - 1] == '/')
					sz -= 1;

				resolv.resize(sz + 1);
				strncpy(resolv.data(), path, sz);
				resolv.data()[sz] = 0;

				if(debugPathResolution) {
					mlibc::infoLogger() << "mlibc realpath(): Symlink is absolute, resolv: '"
						<< resolv.data() << "'" << frg::endlog;
				}
			} else {
				// Relative path, revert changes to resolv, prepend to lnk
				resolv.resize(rsz);
				resolv[rsz - 1] = 0;

				auto lsz = lnk.size();
				lnk.resize((lsz - ls) + sz + 1);
				memmove(lnk.data() + sz, lnk.data() + ls, lsz - ls);
				memcpy(lnk.data(), path, sz);
				lnk[(lsz - ls) + sz] = 0;

				ls = 0;

				if(debugPathResolution) {
					mlibc::infoLogger() << "mlibc realpath(): Symlink is relative, resolv: '"
						<< resolv.data() << "' lnk: '"
						<< frg::string_view{lnk.data(), lnk.size()} << "'" << frg::endlog;
				}
			}
		}

		return 0;
	};

	// Each iteration of this outer loop consumes segment of the input path.
	// This design avoids copying the input path into lnk;
	// the latter could often involve additional allocations.
	while(ps < path_view.size()) {
		frg::string_view ps_view;
		if(auto slash = strchr(path + ps, '/'); slash) {
			ps_view = frg::string_view{path + ps, static_cast<size_t>(slash - (path + ps))};
		}else{
			ps_view = frg::string_view{path + ps, strlen(path) - ps};
		}
		ps += ps_view.size() + 1;

		// Handle one segment from the input path.
		if(int e = process_segment(ps_view); e) {
			errno = e;
			return nullptr;
		}

		// This inner loop consumes segments of lnk.
		while(ls < lnk.size()) {
			frg::string_view ls_view;
			if(auto slash = strchr(lnk.data() + ls, '/'); slash) {
				ls_view = frg::string_view{lnk.data() + ls, static_cast<size_t>(slash - (lnk.data() + ls))};
			}else{
				ls_view = frg::string_view{lnk.data() + ls, strlen(lnk.data()) - ls};
			}
			ls += ls_view.size() + 1;

			// Handle one segment from the link
			if(int e = process_segment(ls_view); e) {
				errno = e;
				return nullptr;
			}
		}

		// All of lnk was consumed, reset it
		lnk.resize(0);
		ls = 0;
	}

	if(resolv.size() == 1) {
		resolv.resize(0);
		resolv.push_back('/');
		resolv.push_back(0);
	}

	if(debugPathResolution)
		mlibc::infoLogger() << "mlibc realpath(): Returns '" << resolv.data() << "'" << frg::endlog;

	if(resolv.size() > PATH_MAX) {
		errno = ENAMETOOLONG;
		return nullptr;
	}

	if(!out)
		out = reinterpret_cast<char *>(getAllocator().allocate(resolv.size()));
	strcpy(out, resolv.data());
	return out;
}

// ----------------------------------------------------------------------------
// Pseudoterminals
// ----------------------------------------------------------------------------

int ptsname_r(int fd, char *buffer, size_t length) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_ptsname, ENOSYS);

	if(int e = sysdep(fd, buffer, length); e)
		return e;

	return 0;
}

char *ptsname(int fd) {
	static char buffer[128];

	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_ptsname, NULL);

	if(int e = sysdep(fd, buffer, 128); e) {
		errno = e;
		return NULL;
	}

	return buffer;
}

int posix_openpt(int flags) {
	int fd;
	if(int e = mlibc::sys_open("/dev/ptmx", flags, 0, &fd); e) {
		errno = e;
		return -1;
	}

	return fd;
}

int unlockpt(int fd) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_unlockpt, -1);

	if(int e = sysdep(fd); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int grantpt(int) {
	return 0;
}

double strtod_l(const char *__restrict__ nptr, char ** __restrict__ endptr, locale_t) {
	mlibc::infoLogger() << "mlibc: strtod_l ignores locale!" << frg::endlog;
	return strtod(nptr, endptr);
}

long double strtold_l(const char *__restrict__, char ** __restrict__, locale_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

float strtof_l(const char *__restrict__ nptr, char **__restrict__ endptr, locale_t) {
	mlibc::infoLogger() << "mlibc: strtof_l ignores locales" << frg::endlog;
	return strtof(nptr, endptr);
}

int strcoll_l(const char *, const char *, locale_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getsubopt(char **__restrict__, char *const *__restrict__, char **__restrict__) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *secure_getenv(const char *name) {
	if (mlibc::rtldConfig().secureRequired)
		return NULL;
	else
		return getenv(name);
}

void *reallocarray(void *ptr, size_t m, size_t n) {
	if(n && m > -1 / n) {
		errno = ENOMEM;
		return 0;
	}

	return realloc(ptr, m * n);
}

char *canonicalize_file_name(const char *name) {
	return realpath(name, NULL);
}
