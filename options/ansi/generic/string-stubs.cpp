#include <string.h>
#include <errno.h>
#include <wchar.h>
#include <ctype.h>

#include <bits/ensure.h>

// memset() is defined in options/internals.
// memcpy() is defined in options/internals.
// memmove() is defined in options/internals.
// strlen() is defined in options/internals.

char *strcpy(char *__restrict dest, const char *src) {
	char *dest_bytes = (char *)dest;
	char *src_bytes = (char *)src;
	while(*src_bytes)
		*(dest_bytes++) = *(src_bytes++);
	*dest_bytes = 0;
	return dest;
}
char *strncpy(char *__restrict dest, const char *src, size_t max_size) {
	auto dest_bytes = static_cast<char *>(dest);
	auto src_bytes = static_cast<const char *>(src);
	size_t i = 0;
	while(*src_bytes && i < max_size) {
		*(dest_bytes++) = *(src_bytes++);
		i++;
	}
	while(i < max_size) {
		*(dest_bytes++) = 0;
		i++;
	}
	return dest;
}

char *strcat(char *__restrict dest, const char *__restrict src) {
	strcpy(dest + strlen(dest), src);
	return dest;
}
char *strncat(char *__restrict dest, const char *__restrict src, size_t max_size) {
	auto dest_bytes = static_cast<char *>(dest);
	auto src_bytes = static_cast<const char *>(src);
	dest_bytes += strlen(dest);
	size_t i = 0;
	while(*src_bytes && i < max_size) {
		*(dest_bytes++) = *(src_bytes++);
		i++;
	}
	*dest_bytes = 0;
	return dest;
}

int memcmp(const void *a, const void *b, size_t size) {
	for(size_t i = 0; i < size; i++) {
		auto a_byte = static_cast<const unsigned char *>(a)[i];
		auto b_byte = static_cast<const unsigned char *>(b)[i];
		if(a_byte < b_byte)
			return -1;
		if(a_byte > b_byte)
			return 1;
	}
	return 0;
}
int strcmp(const char *a, const char *b) {
	size_t i = 0;
	while(true) {
		unsigned char a_byte = a[i];
		unsigned char b_byte = b[i];
		if(!a_byte && !b_byte)
			return 0;
		// If only one char is null, one of the following cases applies.
		if(a_byte < b_byte)
			return -1;
		if(a_byte > b_byte)
			return 1;
		i++;
	}
}

int strcoll(const char *a, const char *b) {
	// TODO: strcoll should take "LC_COLLATE" into account.
	return strcmp(a, b);
}

int strncmp(const char *a, const char *b, size_t max_size) {
	size_t i = 0;
	while(true) {
		if(!(i < max_size))
			return 0;
		unsigned char a_byte = a[i];
		unsigned char b_byte = b[i];
		if(!a_byte && !b_byte)
			return 0;
		// If only one char is null, one of the following cases applies.
		if(a_byte < b_byte)
			return -1;
		if(a_byte > b_byte)
			return 1;
		i++;
	}
}

size_t strxfrm(char *__restrict, const char *__restrict, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *memchr(const void *s, int c, size_t size) {
	auto s_bytes = static_cast<const unsigned char *>(s);
	for(size_t i = 0; i < size; i++)
		if(s_bytes[i] == static_cast<unsigned char>(c))
			return const_cast<unsigned char *>(s_bytes + i);
	return nullptr;
}
char *strchr(const char *s, int c) {
	size_t i = 0;
	while(s[i]) {
		if(s[i] == c)
			return const_cast<char *>(&s[i]);
		i++;
	}
	if(c == 0)
		return const_cast<char *>(&s[i]);
	return nullptr;
}
size_t strcspn(const char *s, const char *chrs) {
	size_t n = 0;
	while(true) {
		if(!s[n] || strchr(chrs, s[n]))
			return n;
		n++;
	}
}
char *strpbrk(const char *s, const char *chrs) {
	size_t n = 0;
	while(s[n]) {
		if(strchr(chrs, s[n]))
			return const_cast<char *>(s + n);
		n++;
	}
	return nullptr;
}
char *strrchr(const char *s, int c) {
	// The null-terminator is considered to be part of the string.
	size_t length = strlen(s);
	for(size_t i = 0; i <= length; i++) {
		if(s[length - i] == c)
			return const_cast<char *>(s + (length - i));
	}
	return nullptr;
}
size_t strspn(const char *s, const char *chrs) {
	size_t n = 0;
	while(true) {
		if(!s[n] || !strchr(chrs, s[n]))
			return n;
		n++;
	}
}
char *strstr(const char *s, const char *pattern) {
	for(size_t i = 0; s[i]; i++) {
		bool found = true;
		for(size_t j = 0; pattern[j]; j++) {
			if(!pattern[j] || s[i + j] == pattern[j])
				continue;

			found = false;
			break;
		}

		if(found)
			return const_cast<char *>(&s[i]);
	}

	return nullptr;
}
char *strtok_r(char *__restrict s, const char *__restrict del, char **__restrict m) {
	__ensure(m);

	// We use *m = null to memorize that the entire string was consumed.
	char *tok;
	if(s) {
		tok = s;
	}else if(*m) {
		tok = *m;
	}else {
		return nullptr;
	}

	// Skip initial delimiters.
	// After this loop: *tok is non-null iff we return a token.
	while(*tok && strchr(del, *tok))
		tok++;

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
	if(p == tok)
		return nullptr;
	return tok;
}
char *strtok(char *__restrict s, const char *__restrict delimiter) {
        static char *saved;
        return strtok_r(s, delimiter, &saved);
}

// This is a GNU extension.
char *strchrnul(const char *s, int c) {
	size_t i = 0;
	while(s[i]) {
		if(s[i] == c)
			return const_cast<char *>(s + i);
		i++;
	}
	return const_cast<char *>(s + i);
}

double wcstod(const wchar_t *__restrict, wchar_t **__restrict) MLIBC_STUB_BODY
float wcstof(const wchar_t *__restrict, wchar_t **__restrict) MLIBC_STUB_BODY
long double wcstold(const wchar_t *__restrict, wchar_t **__restrict) MLIBC_STUB_BODY

long wcstol(const wchar_t *__restrict, wchar_t **__restrict, int) MLIBC_STUB_BODY
long long wcstoll(const wchar_t *__restrict, wchar_t **__restrict, int) MLIBC_STUB_BODY
unsigned long wcstoul(const wchar_t *__restrict, wchar_t **__restrict, int) MLIBC_STUB_BODY
unsigned long long wcstoull(const wchar_t *__restrict, wchar_t **__restrict, int) MLIBC_STUB_BODY

wchar_t *wcscpy(wchar_t *__restrict dest, const wchar_t *__restrict src) {
	wchar_t *a = dest;
	while((*dest++ = *src++));
	return a;
}

wchar_t *wcsncpy(wchar_t *__restrict dest, const wchar_t *__restrict src, size_t n) {
	wchar_t *a = dest;
	while(n && *src)
		n--, *dest++ = *src++;
	wmemset(dest, 0, n);
	return a;
}

wchar_t *wmemcpy(wchar_t *__restrict dest, const wchar_t *__restrict src, size_t n) {
	memcpy(dest, src, n * sizeof(wchar_t));
	return dest;
}

wchar_t *wmemmove(wchar_t *dest, const wchar_t *src, size_t n) {
	memmove(dest, src, n * sizeof(wchar_t));
	return dest;
}

wchar_t *wcscat(wchar_t *__restrict dest, const wchar_t *__restrict src) {
	wcscpy(dest + wcslen(dest), src);
	return dest;
}

wchar_t *wcsncat(wchar_t *__restrict, const wchar_t *__restrict, size_t) MLIBC_STUB_BODY

int wcscmp(const wchar_t *l, const wchar_t *r) {
	for(; *l == *r && *l && *r; l++, r++);
	return *l - *r;
}

int wcscoll(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY
int wcsncmp(const wchar_t *, const wchar_t *, size_t) MLIBC_STUB_BODY
int wcsxfrm(wchar_t *__restrict, const wchar_t *__restrict, size_t) MLIBC_STUB_BODY

int wmemcmp(const wchar_t *a, const wchar_t *b, size_t size) {
	for(size_t i = 0; i < size; i++) {
		auto a_byte = a[i];
		auto b_byte = b[i];
		if(a_byte < b_byte)
			return -1;
		if(a_byte > b_byte)
			return 1;
	}
	return 0;
}

wchar_t *wcschr(const wchar_t *s, wchar_t c) {
	if(!c)
		return (wchar_t *)s + wcslen(s);
	for(; *s && *s != c; s++);
	return *s ? (wchar_t *)s : 0;
}

size_t wcscspn(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY
wchar_t *wcspbrk(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY

wchar_t *wcsrchr(const wchar_t *s, wchar_t c) {
	const wchar_t *p;
	for(p = s + wcslen(s); p >= s && *p != c; p--);
	return p >= s ? (wchar_t *)p : 0;
}

size_t wcsspn(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY
wchar_t *wcsstr(const wchar_t *, const wchar_t *) MLIBC_STUB_BODY
wchar_t *wcstok(wchar_t *__restrict, const wchar_t *__restrict, wchar_t **__restrict) MLIBC_STUB_BODY

wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t size) {
	auto s_bytes = s;
	for(size_t i = 0; i < size; i++)
		if(s_bytes[i] == c)
			return const_cast<wchar_t *>(s_bytes + i);
	return nullptr;
}

size_t wcslen(const wchar_t *s) {
	const wchar_t *a;
	for(a = s; *s; s++);
	return s-a;
}

wchar_t *wmemset(wchar_t *d, wchar_t c, size_t n) {
	wchar_t *ret = d;
	while(n--)
		*d++ = c;
	return ret;
}

char *strerror(int e) {
	const char *s;
	switch(e) {
	case EAGAIN: s = "Operation would block (EAGAIN)"; break;
	case EACCES: s = "Access denied (EACCESS)"; break;
	case EBADF:  s = "Bad file descriptor (EBADF)"; break;
	case EEXIST: s = "File exists already (EEXIST)"; break;
	case EFAULT: s = "Access violation (EFAULT)"; break;
	case EINTR:  s = "Operation interrupted (EINTR)"; break;
	case EINVAL: s = "Invalid argument (EINVAL)"; break;
	case EIO:    s = "I/O error (EIO)"; break;
	case EISDIR: s = "Resource is directory (EISDIR)"; break;
	case ENOENT: s = "No such file or directory (ENOENT)"; break;
	case ENOMEM: s = "Out of memory (ENOMEM)"; break;
	case ENOTDIR: s = "Expected directory instead of file (ENOTDIR)"; break;
	case ENOSYS: s = "Operation not implemented (ENOSYS)"; break;
	case EPERM:  s = "Operation not permitted (EPERM)"; break;
	case EPIPE:  s = "Broken pipe (EPIPE)"; break;
	case ESPIPE: s = "Seek not possible (ESPIPE)"; break;
	case ENXIO: s = "No such device or address (ENXIO)"; break;
	case ENOEXEC: s = "Exec format error (ENOEXEC)"; break;
	case ENOSPC: s = "No space left on device (ENOSPC)"; break;
	default:
		s = "Unknown error code (?)";
	}
	return const_cast<char *>(s);
}
// strlen() is defined in options/internals.

// POSIX extensions.

int strerror_r(int e, char *buffer, size_t bufsz) {
	auto s = strerror(e);
	strncpy(buffer, s, bufsz);
	// Note that strerror_r does not set errno on error!
	if(strlen(s) >= bufsz)
		return ERANGE;
	return 0;
}

void *mempcpy(void *dest, const void *src, size_t len) {
	return (char *)memcpy(dest, src, len) + len;
}

char *stpcpy(char *__restrict dest, const char *__restrict src) {
	auto n = strlen(src);
	memcpy(dest, src, n + 1);
	return dest + n;
}

// GNU extensions.
// Taken from musl.
int strverscmp(const char *l0, const char *r0) {
	const unsigned char *l = (const unsigned char *)l0;
	const unsigned char *r = (const unsigned char *)r0;
	size_t i, dp, j;
	int z = 1;

	/* Find maximal matching prefix and track its maximal digit
	 * suffix and whether those digits are all zeros. */
	for(dp = i = 0; l[i] == r[i]; i++) {
		int c = l[i];
		if(!c)
			return 0;
		if(!isdigit(c))
			dp = i + 1, z = 1;
		else if(c != '0')
			z = 0;
	}

	if(l[dp] != '0' && r[dp] != '0') {
		/* If we're not looking at a digit sequence that began
		 * with a zero, longest digit string is greater. */
		for(j = i; isdigit(l[j]); j++) {
			if(!isdigit(r[j]))
				return 1;
		}
		if(isdigit(r[j]))
			return -1;
	} else if(z && dp < i && (isdigit(l[i]) || isdigit(r[i]))) {
		/* Otherwise, if common prefix of digit sequence is
		 * all zeros, digits order less than non-digits. */
		return (unsigned char)(l[i] - '0') - (unsigned char)(r[i] - '0');
	}

	return l[i] - r[i];
}

// Taken from musl.
static char *twobyte_memmem(const unsigned char *h, size_t k, const unsigned char *n) {
	uint16_t nw = n[0]<<8 | n[1], hw = h[0]<<8 | h[1];
	for (h+=2, k-=2; k; k--, hw = hw<<8 | *h++)
		if (hw == nw) return (char *)h-2;
	return hw == nw ? (char *)h-2 : 0;
}

static char *threebyte_memmem(const unsigned char *h, size_t k, const unsigned char *n) {
	uint32_t nw = (uint32_t)n[0]<<24 | n[1]<<16 | n[2]<<8;
	uint32_t hw = (uint32_t)h[0]<<24 | h[1]<<16 | h[2]<<8;
	for (h+=3, k-=3; k; k--, hw = (hw|*h++)<<8)
		if (hw == nw) return (char *)h-3;
	return hw == nw ? (char *)h-3 : 0;
}

static char *fourbyte_memmem(const unsigned char *h, size_t k, const unsigned char *n) {
	uint32_t nw = (uint32_t)n[0]<<24 | n[1]<<16 | n[2]<<8 | n[3];
	uint32_t hw = (uint32_t)h[0]<<24 | h[1]<<16 | h[2]<<8 | h[3];
	for (h+=4, k-=4; k; k--, hw = hw<<8 | *h++)
		if (hw == nw) return (char *)h-4;
	return hw == nw ? (char *)h-4 : 0;
}

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define BITOP(a,b,op) \
 ((a)[(size_t)(b)/(8*sizeof *(a))] op (size_t)1<<((size_t)(b)%(8*sizeof *(a))))

static char *twoway_memmem(const unsigned char *h, const unsigned char *z, const unsigned char *n, size_t l) {
	size_t i, ip, jp, k, p, ms, p0, mem, mem0;
	size_t byteset[32 / sizeof(size_t)] = { 0 };
	size_t shift[256];

	/* Computing length of needle and fill shift table */
	for (i=0; i<l; i++)
		BITOP(byteset, n[i], |=), shift[n[i]] = i+1;

	/* Compute maximal suffix */
	ip = -1; jp = 0; k = p = 1;
	while (jp+k<l) {
		if (n[ip+k] == n[jp+k]) {
			if (k == p) {
				jp += p;
				k = 1;
			} else k++;
		} else if (n[ip+k] > n[jp+k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	ms = ip;
	p0 = p;

	/* And with the opposite comparison */
	ip = -1; jp = 0; k = p = 1;
	while (jp+k<l) {
		if (n[ip+k] == n[jp+k]) {
			if (k == p) {
				jp += p;
				k = 1;
			} else k++;
		} else if (n[ip+k] < n[jp+k]) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	if (ip+1 > ms+1) ms = ip;
	else p = p0;

	/* Periodic needle? */
	if (memcmp(n, n+p, ms+1)) {
		mem0 = 0;
		p = MAX(ms, l-ms-1) + 1;
	} else mem0 = l-p;
	mem = 0;

	/* Search loop */
	for (;;) {
		/* If remainder of haystack is shorter than needle, done */
		if (z-h < l) return 0;

		/* Check last byte first; advance by shift on mismatch */
		if (BITOP(byteset, h[l-1], &)) {
			k = l-shift[h[l-1]];
			if (k) {
				if (k < mem) k = mem;
				h += k;
				mem = 0;
				continue;
			}
		} else {
			h += l;
			mem = 0;
			continue;
		}

		/* Compare right half */
		for (k=MAX(ms+1,mem); k<l && n[k] == h[k]; k++);
		if (k < l) {
			h += k-ms;
			mem = 0;
			continue;
		}
		/* Compare left half */
		for (k=ms+1; k>mem && n[k-1] == h[k-1]; k--);
		if (k <= mem) return (char *)h;
		h += p;
		mem = mem0;
	}
}

void *memmem(const void *h0, size_t k, const void *n0, size_t l) {
	const unsigned char *h = h0, *n = n0;

	/* Return immediately on empty needle */
	if (!l) return (void *)h;

	/* Return immediately when needle is longer than haystack */
	if (k<l) return 0;

	/* Use faster algorithms for short needles */
	h = memchr(h0, *n, k);
	if (!h || l==1) return (void *)h;
	k -= h - (const unsigned char *)h0;
	if (k<l) return 0;
	if (l==2) return twobyte_memmem(h, k, n);
	if (l==3) return threebyte_memmem(h, k, n);
	if (l==4) return fourbyte_memmem(h, k, n);

	return twoway_memmem(h, h+k, n, l);
}
