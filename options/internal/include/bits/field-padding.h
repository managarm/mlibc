#ifndef MLIBC_FIELD_PADDING_H
#define MLIBC_FIELD_PADDING_H

#define __MLIBC_FIELD_PADDED(T, AT, F) \
	AT : (sizeof(AT)-sizeof(T))*8*(__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__); \
	T F; \
	AT : (sizeof(AT)-sizeof(T))*8*(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#endif
