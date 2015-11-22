
#include <string.h>

#include <mlibc/ensure.h>

void *memcpy(void *__restrict dest, const void *__restrict src, size_t size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void *memmove(void *dest, const void *src, size_t size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
size_t strpbrk(const char *s, const char *chrs) {
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
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *strerror(int errnum) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
size_t strlen(const char *s) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

