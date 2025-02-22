#undef _GNU_SOURCE

#include <string.h>
#include <errno.h>
#include <wchar.h>
#include <ctype.h>

#include <bits/ensure.h>
#include <mlibc/strtol.hpp>

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

size_t strxfrm(char *__restrict dest, const char *__restrict src, size_t n) {
	// NOTE: This might not work for non ANSI charsets.
	size_t l = strlen(src);

	// man page: If the value returned is n or more, the contents of dest are indeterminate.
	if(n > l)
		strncpy(dest, src, n);

	return l;
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

double wcstod(const wchar_t *__restrict, wchar_t **__restrict) { MLIBC_STUB_BODY; }
float wcstof(const wchar_t *__restrict, wchar_t **__restrict) { MLIBC_STUB_BODY; }
long double wcstold(const wchar_t *__restrict, wchar_t **__restrict) { MLIBC_STUB_BODY; }

long wcstol(const wchar_t *__restrict nptr, wchar_t **__restrict endptr, int base)  {
	return mlibc::stringToInteger<long, wchar_t>(nptr, endptr, base);
}
unsigned long wcstoul(const wchar_t *__restrict nptr, wchar_t **__restrict endptr, int base)  {
	return mlibc::stringToInteger<unsigned long, wchar_t>(nptr, endptr, base);
}
long long wcstoll(const wchar_t *__restrict nptr, wchar_t **__restrict endptr, int base)  {
	return mlibc::stringToInteger<long long, wchar_t>(nptr, endptr, base);
}
unsigned long long wcstoull(const wchar_t *__restrict nptr, wchar_t **__restrict endptr, int base)  {
	return mlibc::stringToInteger<unsigned long long, wchar_t>(nptr, endptr, base);
}

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

wchar_t *wcsncat(wchar_t *__restrict, const wchar_t *__restrict, size_t) { MLIBC_STUB_BODY; }

int wcscmp(const wchar_t *l, const wchar_t *r) {
	for(; *l == *r && *l && *r; l++, r++);
	return *l - *r;
}

int wcscoll(const wchar_t *, const wchar_t *) { MLIBC_STUB_BODY; }
int wcsncmp(const wchar_t *, const wchar_t *, size_t) { MLIBC_STUB_BODY; }
int wcsxfrm(wchar_t *__restrict, const wchar_t *__restrict, size_t) { MLIBC_STUB_BODY; }

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

size_t wcscspn(const wchar_t *, const wchar_t *) { MLIBC_STUB_BODY; }
wchar_t *wcspbrk(const wchar_t *, const wchar_t *) { MLIBC_STUB_BODY; }

wchar_t *wcsrchr(const wchar_t *s, wchar_t c) {
	const wchar_t *p;
	for(p = s + wcslen(s); p >= s && *p != c; p--);
	return p >= s ? (wchar_t *)p : 0;
}

size_t wcsspn(const wchar_t *, const wchar_t *) { MLIBC_STUB_BODY; }
wchar_t *wcsstr(const wchar_t *, const wchar_t *) { MLIBC_STUB_BODY; }
wchar_t *wcstok(wchar_t *__restrict, const wchar_t *__restrict, wchar_t **__restrict) { MLIBC_STUB_BODY; }

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
	case ENOTSOCK: s = "Socket operation on non-socket (ENOTSOCK)"; break;
	case ENOTCONN: s = "Transport endpoint is not connected (ENOTCONN)"; break;
	case EDOM: s = "Numerical argument out of domain (EDOM)"; break;
	case EILSEQ: s = "Invalid or incomplete multibyte or wide character (EILSEQ)"; break;
	case ERANGE: s = "Numerical result out of range (ERANGE)"; break;
	case E2BIG: s = "Argument list too long (E2BIG)"; break;
	case EADDRINUSE: s = "Address already in use (EADDRINUSE)"; break;
	case EADDRNOTAVAIL: s = "Cannot assign requested address (EADDRNOTAVAIL)"; break;
	case EAFNOSUPPORT: s = "Address family not supported by protocol (EAFNOSUPPORT)"; break;
	case EALREADY: s = "Operation already in progress (EALREADY)"; break;
	case EBADMSG: s = "Bad message (EBADMSG)"; break;
	case EBUSY: s = "Device or resource busy (EBUSY)"; break;
	case ECANCELED: s = "Operation canceled (ECANCELED)"; break;
	case ECHILD: s = "No child processes (ECHILD)"; break;
	case ECONNABORTED: s = "Software caused connection abort (ECONNABORTED)"; break;
	case ECONNREFUSED: s = "Connection refused (ECONNREFUSED)"; break;
	case ECONNRESET: s = "Connection reset by peer (ECONNRESET)"; break;
	case EDEADLK: s = "Resource deadlock avoided (EDEADLK)"; break;
	case EDESTADDRREQ: s = "Destination address required (EDESTADDRREQ)"; break;
	case EDQUOT: s = "Disk quota exceeded (EDQUOT)"; break;
	case EFBIG: s = "File too large (EFBIG)"; break;
	case EHOSTUNREACH: s = "No route to host (EHOSTUNREACH)"; break;
	case EIDRM: s = "Identifier removed (EIDRM)"; break;
	case EINPROGRESS: s = "Operation now in progress (EINPROGRESS)"; break;
	case EISCONN: s = "Transport endpoint is already connected (EISCONN)"; break;
	case ELOOP: s = "Too many levels of symbolic links (ELOOP)"; break;
	case EMFILE: s = "Too many open files (EMFILE)"; break;
	case EMLINK: s = "Too many links (EMLINK)"; break;
	case EMSGSIZE: s = "Message too long (EMSGSIZE)"; break;
	case EMULTIHOP: s = "Multihop attempted (EMULTIHOP)"; break;
	case ENAMETOOLONG: s = "File name too long (ENAMETOOLONG)"; break;
	case ENETDOWN: s = "Network is down (ENETDOWN)"; break;
	case ENETRESET: s = "Network dropped connection on reset (ENETRESET)"; break;
	case ENETUNREACH: s = "Network is unreachable (ENETUNREACH)"; break;
	case ENFILE: s = "Too many open files in system (ENFILE)"; break;
	case ENOBUFS: s = "No buffer space available (ENOBUFS)"; break;
	case ENODEV: s = "No such device (ENODEV)"; break;
	case ENOLCK: s = "No locks available (ENOLCK)"; break;
	case ENOLINK: s = "Link has been severed (ENOLINK)"; break;
	case ENOMSG: s = "No message of desired type (ENOMSG)"; break;
	case ENOPROTOOPT: s = "Protocol not available (ENOPROTOOPT)"; break;
	case ENOTEMPTY: s = "Directory not empty (ENOTEMPTY)"; break;
	case ENOTRECOVERABLE: s = "Sate not recoverable (ENOTRECOVERABLE)"; break;
	case ENOTSUP: s = "Operation not supported (ENOTSUP)"; break;
	case ENOTTY: s = "Inappropriate ioctl for device (ENOTTY)"; break;
	case EOVERFLOW: s = "Value too large for defined datatype (EOVERFLOW)"; break;
#if EOPNOTSUPP != ENOTSUP
	/* these are aliases on the mlibc abi */
	case EOPNOTSUPP: s = "Operation not supported (EOPNOTSUP)"; break;
#endif
	case EOWNERDEAD: s = "Owner died (EOWNERDEAD)"; break;
	case EPROTO: s = "Protocol error (EPROTO)"; break;
	case EPROTONOSUPPORT: s = "Protocol not supported (EPROTONOSUPPORT)"; break;
	case EPROTOTYPE: s = "Protocol wrong type for socket (EPROTOTYPE)"; break;
	case EROFS: s = "Read-only file system (EROFS)"; break;
	case ESRCH: s = "No such process (ESRCH)"; break;
	case ESTALE: s = "Stale file handle (ESTALE)"; break;
	case ETIMEDOUT: s = "Connection timed out (ETIMEDOUT)"; break;
	case ETXTBSY: s = "Text file busy (ETXTBSY)"; break;
	case EXDEV: s = "Invalid cross-device link (EXDEV)"; break;
	case ENODATA: s = "No data available (ENODATA)"; break;
	case ETIME: s = "Timer expired (ETIME)"; break;
	case ENOKEY: s = "Required key not available (ENOKEY)"; break;
	case ESHUTDOWN: s = "Cannot send after transport endpoint shutdown (ESHUTDOWN)"; break;
	case EHOSTDOWN: s = "Host is down (EHOSTDOWN)"; break;
	case EBADFD: s = "File descriptor in bad state (EBADFD)"; break;
	case ENOMEDIUM: s = "No medium found (ENOMEDIUM)"; break;
	case ENOTBLK: s = "Block device required (ENOTBLK)"; break;
	case ENONET: s = "Machine is not on the network (ENONET)"; break;
	case EPFNOSUPPORT: s = "Protocol family not supported (EPFNOSUPPORT)"; break;
	case ESOCKTNOSUPPORT: s = "Socket type not supported (ESOCKTNOSUPPORT)"; break;
	case ESTRPIPE: s = "Streams pipe error (ESTRPIPE)"; break;
	case EREMOTEIO: s = "Remote I/O error (EREMOTEIO)"; break;
	case ERFKILL: s = "Operation not possible due to RF-kill (ERFKILL)"; break;
	case EBADR: s = "Invalid request descriptor (EBADR)"; break;
	case EUNATCH: s = "Protocol driver not attached (EUNATCH)"; break;
	case EMEDIUMTYPE: s = "Wrong medium type (EMEDIUMTYPE)"; break;
	case EREMOTE: s = "Object is remote (EREMOTE)"; break;
	case EKEYREJECTED: s = "Key was rejected by service (EKEYREJECTED)"; break;
	case EUCLEAN: s = "Structure needs cleaning (EUCLEAN)"; break;
	case EBADSLT: s = "Invalid slot (EBADSLT)"; break;
	case ENOANO: s = "No anode (ENOANO)"; break;
	case ENOCSI: s = "No CSI structure available (ENOCSI)"; break;
	case ENOSTR: s = "Device not a stream (ENOSTR)"; break;
	case ETOOMANYREFS: s = "Too many references: cannot splice (ETOOMANYREFS)"; break;
	case ENOPKG: s = "Package not installed (ENOPKG)"; break;
	case EKEYREVOKED: s = "Key has been revoked (EKEYREVOKED)"; break;
	case EXFULL: s = "Exchange full (EXFULL)"; break;
	case ELNRNG: s = "Link number out of range (ELNRNG)"; break;
	case ENOTUNIQ: s = "Name not unique on network (ENOTUNIQ)"; break;
	case ERESTART: s = "Interrupted system call should be restarted (ERESTART)"; break;
	case EUSERS: s = "Too many users (EUSERS)"; break;

#ifdef EIEIO
	case EIEIO: s = "Computer bought the farm; OS internal error (EIEIO)"; break;
#endif

	default:
		s = "Unknown error code (?)";
	}
	return const_cast<char *>(s);
}
// strlen() is defined in options/internals.

extern "C" char *__gnu_strerror_r(int e, char *buffer, size_t bufsz) {
	auto s = strerror(e);
	strncpy(buffer, s, bufsz);
	return buffer;
}

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

void *memmem(const void *hs, size_t haystackLen, const void *nd, size_t needleLen) {
	const char *haystack = static_cast<const char *>(hs);
	const char *needle = static_cast<const char *>(nd);

	for (size_t i = 0; i < haystackLen; i++) {
		bool found = true;

		for (size_t j = 0; j < needleLen; j++) {
			if (i + j >= haystackLen || haystack[i + j] != needle[j]) {
				found = false;
				break;
			}
		}

		if(found)
			return const_cast<char *>(&haystack[i]);
	}

	return nullptr;
}
