#ifndef MLIBC_FIELD_PADDING_H
#define MLIBC_FIELD_PADDING_H

#ifdef __GNUC__

#define __MLIBC_FIELD_PADDED(T, F, PT) \
	PT : (sizeof(PT)-sizeof(T))*8*(BYTE_ORDER == BIG_ENDIAN); \
	T F; \
	PT : (sizeof(PT)-sizeof(T))*8*(BYTE_ORDER == LITTLE_ENDIAN)

#else
#error "Unsupported compiler"
#endif

#endif
