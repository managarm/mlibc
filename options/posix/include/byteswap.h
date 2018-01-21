
#ifndef _BYTESWAP_H
#define _BYTESWAP_H

#ifdef __cplusplus
extern "C" {
#endif

#define bswap_16(x) __builtin_bswap_16(x)
#define bswap_32(x) __builtin_bswap_32(x)
#define bswap_64(x) __builtin_bswap_64(x)

#ifdef __cplusplus
}
#endif

#endif // _BYTESWAP_H

