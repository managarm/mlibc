
#include <string.h>

#include <mlibc/ensure.h>

void *memcpy(void *__restrict dest, const void *__restrict src, size_t size) {
	__ensure(!"Not implemented");
}
void *memmove(void *dest, const void *src, size_t size) {
	__ensure(!"Not implemented");
}
char *strcpy(char *__restrict dest, const char *src) {
	__ensure(!"Not implemented");
}
char *strncpy(char *__restrict dest, const char *src, size_t max_size) {
	__ensure(!"Not implemented");
}

char *strcat(char *__restrict dest, char *__restrict src) {
	__ensure(!"Not implemented");
}
char *strncat(char *__restrict dest, char *__restrict src, size_t max_size) {
	__ensure(!"Not implemented");
}

int memcmp(const void *a, const void *b, size_t size) {
	__ensure(!"Not implemented");
}
int strcmp(const char *a, const char *b) {
	__ensure(!"Not implemented");
}
int strcoll(const char *a, const char *b) {
	__ensure(!"Not implemented");
}
int strncmp(const char *a, const char *b, size_t max_size) {
	__ensure(!"Not implemented");
}
size_t strxfrm(char *__restrict dest, const char *__restrict src, size_t max_size) {
	__ensure(!"Not implemented");
}

void *memchr(const void *s, int c, size_t size) {
	__ensure(!"Not implemented");
}
char *strchr(const char *s, int c) {
	__ensure(!"Not implemented");
}
size_t strcspn(const char *s, const char *chrs) {
	__ensure(!"Not implemented");
}
size_t strpbrk(const char *s, const char *chrs) {
	__ensure(!"Not implemented");
}
char *strrchr(const char *s, int c) {
	__ensure(!"Not implemented");
}
size_t strspn(const char *s, const char *chrs) {
	__ensure(!"Not implemented");
}
char *strstr(const char *pattern, const char *s) {
	__ensure(!"Not implemented");
}
char *strtok(char *__restrict s, const char *__restrict delimiter) {
	__ensure(!"Not implemented");
}

void *memset(void *dest, int c, size_t size) {
	__ensure(!"Not implemented");
}
char *strerror(int errnum) {
	__ensure(!"Not implemented");
}
size_t strlen(const char *s) {
	__ensure(!"Not implemented");
}

