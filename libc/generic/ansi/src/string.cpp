
#include <string.h>

#include <mlibc/ensure.h>

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
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *strncpy(char *__restrict dest, const char *src, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *strcat(char *__restrict dest, char *__restrict src) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *strncat(char *__restrict dest, char *__restrict src, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int memcmp(const void *a, const void *b, size_t size) {
	char *a_bytes = (char *)a;
	char *b_bytes = (char *)b;
	for(size_t i = 0; i < size; i++) {
		if(a_bytes[i] < b_bytes[i])
			return -1;
		if(a_bytes[i] > b_bytes[i])
			return 1;
	}
	return 0;
}
int strcmp(const char *a, const char *b) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int strcoll(const char *a, const char *b) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int strncmp(const char *a, const char *b, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
size_t strxfrm(char *__restrict dest, const char *__restrict src, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *memchr(const void *s, int c, size_t size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *strchr(const char *s, int c) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
size_t strcspn(const char *s, const char *chrs) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *strpbrk(const char *s, const char *chrs) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *strrchr(const char *s, int c) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
size_t strspn(const char *s, const char *chrs) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *strstr(const char *pattern, const char *s) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *strtok(char *__restrict s, const char *__restrict delimiter) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *memset(void *dest, int c, size_t size) {
	char *dest_bytes = (char *)dest;
	for(size_t i = 0; i < size; i++)
		dest_bytes[i] = c;
	return dest;
}
char *strerror(int errnum) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
size_t strlen(const char *s) {
	size_t len = 0;
	for(size_t i = 0; s[i]; i++)
		len++;
	return len;
}

