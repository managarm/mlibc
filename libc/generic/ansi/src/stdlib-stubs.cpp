
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <mlibc/ensure.h>

double atof(const char *string) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int atoi(const char *string) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long atol(const char *string) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long long atoll(const char *string) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
double strtod(const char *__restrict string, char **__restrict end) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
float strtof(const char *__restrict string, char **__restrict end) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long double strtold(const char *__restrict string, char **__restrict end) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long strtol(const char *__restrict string, char **__restrict end, int base) {
	__ensure(base == 10);
	
	// skip leading space
	while(*string) {
		if(!isspace(*string))
			break;
		string++;
	}
	
	__ensure(*string != '+' && *string != '-');
	
	// parse the actual digits
	long result = 0;
	while(*string) {
		if(*string >= '0' && *string <= '9') {
			result = result * 10 + (*string - '0');
		}else{
			break;
		}
		string++;
	}
	*end = const_cast<char *>(string);

	return result;
}
long long strtoll(const char *__restrict string, char **__restrict end, int base) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
unsigned long strtoul(const char *__restrict string, char **__restrict end, int base) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
unsigned long long strtoull(const char *__restrict string, char **__restrict end, int base) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int rand(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void srand(unsigned int seed) {
	__ensure(!"Not implemented");
}

void *aligned_alloc(size_t alignment, size_t size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int atexit(void (*func)(void)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int at_quick_exit(void (*func)(void)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void exit(int status) {
	// TODO: should be provided by the LSB sublibrary
	_Exit(status);
}
// _Exit() is provided by the platform
char *getenv(const char *name) {
	// TODO: should be provided by the POSIX sublibrary based on environ
	return nullptr;
}
void quick_exit(int status) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int system(const char *string) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *bsearch(const void *key, const void *base, size_t count, size_t size,
		int (*compare)(const void *, const void *)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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

int abs(int number) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long labs(long number) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long long llabs(long long number) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

div_t div(int number, int denom) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
ldiv_t ldiv(long number, long denom) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
lldiv_t lldiv(long long number, long long denom) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int mblen(const char *mb_chr, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int mbtowc(wchar_t *__restrict wc, const char *__restrict mb_chr, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int wctomb(char *mb_chr, wchar_t wc) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int mbstowcs(wchar_t *__restrict wc_string, const char *__restrict mb_string, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int wcstombs(char *mb_string, const wchar_t *__restrict wc_string, size_t max_size) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

