
#ifndef _BYTESWAP_H
#define _BYTESWAP_H

#ifdef __cplusplus
extern "C" {
#endif

#define bswap_16(x) __builtin_bswap16(x)
#define bswap_32(x) __builtin_bswap32(x)
#define bswap_64(x) __builtin_bswap64(x)

/* Some programs like eudev call these functions instead */
#define __bswap_16(x) __builtin_bswap16(x)
#define __bswap_32(x) __builtin_bswap32(x)
#define __bswap_64(x) __builtin_bswap64(x)

#ifdef __cplusplus
}
#endif

#endif /* _BYTESWAP_H */

