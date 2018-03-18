
#include <string.h>
#include <errno.h>

#include <bits/ensure.h>

void *memcpy(void *__restrict dest, const void *__restrict src, size_t size) {
	char *dest_bytes = (char *)dest;
	char *src_bytes = (char *)src;
	for(size_t i = 0; i < size; i++)
		dest_bytes[i] = src_bytes[i];
	return dest;
}
void *memmove(void *dest, const void *src, size_t size) {
	char *dest_bytes = (char *)dest;
	char *src_bytes = (char *)src;
	if(dest_bytes < src_bytes) {
		for(size_t i = 0; i < size; i++)
			dest_bytes[i] = src_bytes[i];
	}else{
		for(size_t i = 0; i < size; i++)
			dest_bytes[size - i - 1] = src_bytes[size - i - 1];
	}
	return dest;
}
char *strcpy(char *__restrict dest, const char *src) {
	char *dest_bytes = (char *)dest;
	char *src_bytes = (char *)src;
	for(size_t i = 0; *src_bytes; i++)
		*(dest_bytes++) = *(src_bytes++);
	*dest_bytes = 0;
	return dest;
}
char *strncpy(char *__restrict dest, const char *src, size_t max_size) {
	auto dest_bytes = static_cast<char *>(dest);
	auto src_bytes = static_cast<const char *>(src);
	size_t i = 0;
	while(src_bytes[i]) {
		dest_bytes[i] = src_bytes[i];
		i++;
	}
	for(size_t j = i; j < max_size; j++)
		dest_bytes[j] = 0;
	return &dest[i];
}

char *strcat(char *__restrict dest, const char *__restrict src) {
	strcpy(dest + strlen(dest), src);
	return dest;
}
char *strncat(char *__restrict dest, const char *__restrict src, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
size_t strxfrm(char *__restrict dest, const char *__restrict src, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *memchr(const void *s, int c, size_t size) {
	auto s_bytes = static_cast<const unsigned char *>(s);
	for(size_t i = 0; i < size; i++)
		if(s_bytes[i] == static_cast<unsigned char>(c))
			return const_cast<unsigned char *>(&s_bytes[i]);
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
char *strtok(char *__restrict s, const char *__restrict delimiter) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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

void *memset(void *dest, int c, size_t size) {
	char *dest_bytes = (char *)dest;
	for(size_t i = 0; i < size; i++)
		dest_bytes[i] = c;
	return dest;
}
char *strerror(int errnum) {
	const char *string;
	switch(errnum) {
	case EACCES:
		string = "Access denied"; break;
	case ENOENT:
		string = "File not found"; break;
	default:
		string = "Illegal error code";
	}
	return const_cast<char *>(string);
}
size_t strlen(const char *s) {
	size_t len = 0;
	for(size_t i = 0; s[i]; i++)
		len++;
	return len;
}

// POSIX extensions.

int strerror_r(int, char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *mempcpy(void *dest, const void *src, size_t len) {
	return (char *)memcpy(dest, src, len) + len;
}

char *stpcpy(char *__restrict dest, const char *__restrict src) {
	auto n = strlen(src);
	memcpy(dest, src, n + 1);
	return dest + n;
}

