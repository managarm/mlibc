
#include <string.h>

void *memset(void *dest, int c, size_t size) {
	char *dest_bytes = (char *)dest;
	for(size_t i = 0; i < size; i++)
		dest_bytes[i] = c;
	return dest;
}

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

size_t strlen(const char *s) {
	size_t len = 0;
	for(size_t i = 0; s[i]; i++)
		len++;
	return len;
}

