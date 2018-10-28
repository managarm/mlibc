
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <frigg/glue-hel.hpp>
#include <frigg/support.hpp>

#include <hel.h>
#include <hel-syscalls.h>

VirtualAlloc virtualAlloc;
frigg::LazyInitializer<Allocator> allocator;

void *memcpy(void *dest, const void *src, size_t n) {
	for(size_t i = 0; i < n; i++)
		((char *)dest)[i] = ((const char *)src)[i];
	return dest;
}

void *memset(void *dest, int byte, size_t count) {
	for(size_t i = 0; i < count; i++)
		((char *)dest)[i] = (char)byte;
	return dest;
}

size_t strlen(const char *str) {
	size_t length = 0;
	while(*str++ != 0)
		length++;
	return length;
}

void friggBeginLog() { }
void friggEndLog() { }

void friggPrintCritical(char c) {
	helLog(&c, 1);
}

void friggPrintCritical(const char *str) {
	int len = 0;
	for(int i = 0; str[i]; i++)
		len++;
	helLog(str, len);
}

void friggPanic() {
	const char *str = "Panic in mlibc";
	int len = 0;
	for(int i = 0; str[i]; i++)
		len++;
	helPanic(str, len);
}

extern "C" void frg_panic(const char *mstr) {
	int mlen = 0;
	while(mstr[mlen])
		mlen++;
	helLog(mstr, mlen);

	const char *str = "Panic in mlibc";
	int len = 0;
	while(str[len])
		len++;
	helPanic(str, len);
}

extern "C" void __assert_fail(const char *assertion,
	const char *file, unsigned int line, const char *function) {
	frg_panic(assertion);
}

uintptr_t VirtualAlloc::map(size_t length) {
	assert((length % 0x1000) == 0);

	HelHandle memory;
	void *actual_ptr;
	HEL_CHECK(helAllocateMemory(length, 0, &memory));
	HEL_CHECK(helMapMemory(memory, kHelNullHandle, nullptr, 0, length,
			kHelMapProtRead | kHelMapProtWrite | kHelMapCopyOnWriteAtFork, &actual_ptr));
	HEL_CHECK(helCloseDescriptor(memory));
	return (uintptr_t)actual_ptr;
}

void VirtualAlloc::unmap(uintptr_t address, size_t length) {
	HEL_CHECK(helUnmapMemory(kHelNullHandle, (void *)address, length));
}

