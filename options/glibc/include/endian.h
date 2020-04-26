
#ifndef _ENDIAN_H
#define _ENDIAN_H

#ifdef __GNUC__
# 	define BYTE_ORDER __BYTE_ORDER__
#	define LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#	define BIG_ENDIAN __ORDER_BIG_ENDIAN__
#	define PDP_ENDIAN __ORDER_PDP_ENDIAN__
#else
#	error "Unsupported compiler"
#endif

#endif // _ENDIAN_H
