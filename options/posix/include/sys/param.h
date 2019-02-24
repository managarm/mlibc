
#ifndef _SYS_PARAM_H
#define _SYS_PARAM_H

#ifdef __GNUC__
# 	define BYTE_ORDER __BYTE_ORDER__
#	define LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#	define BIG_ENDIAN __ORDER_BIG_ENDIAN__
#	define PDP_ENDIAN __ORDER_PDP_ENDIAN__
#else
#	error "Unsupported compiler"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#undef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#undef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#ifdef __cplusplus
}
#endif

#endif // _SYS_PARAM_H

