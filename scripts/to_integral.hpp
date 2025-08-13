#pragma once

#include <stdio.h>

template<typename E>
constexpr void macro_print(const char *name, E val) {
	printf("%s = %ld\n", name, val);
}

template<>
constexpr void macro_print<signed long long>(const char *name, signed long long val) {
	printf("%s = %lld\n", name, val);
}

template<>
constexpr void macro_print<signed long>(const char *name, signed long val) {
	printf("%s = %ld\n", name, val);
}

template<>
constexpr void macro_print<signed int>(const char *name, signed int val) {
	printf("%s = %d\n", name, val);
}

template<>
constexpr void macro_print<signed short>(const char *name, signed short val) {
	printf("%s = %hd\n", name, val);
}

template<>
constexpr void macro_print<signed char>(const char *name, signed char val) {
	printf("%s = %hhd\n", name, val);
}

template<>
constexpr void macro_print<unsigned long long>(const char *name, unsigned long long val) {
	printf("%s = %llu\n", name, val);
}

template<>
constexpr void macro_print<unsigned long>(const char *name, unsigned long val) {
	printf("%s = %lu\n", name, val);
}

template<>
constexpr void macro_print<unsigned int>(const char *name, unsigned int val) {
	printf("%s = %u\n", name, val);
}

template<>
constexpr void macro_print<unsigned short>(const char *name, unsigned short val) {
	printf("%s = %hu\n", name, val);
}

template<>
constexpr void macro_print<unsigned char>(const char *name, unsigned char val) {
	printf("%s = %hhu ('%c')\n", name, val, val);
}

template<>
constexpr void macro_print<const char *>(const char *name, const char *val) {
	printf("%s = \"%s\"\n", name, val);
}
