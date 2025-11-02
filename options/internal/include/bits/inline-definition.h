#ifndef _MLIBC_INTERNAL_INLINE_DEFINITION_H
#define _MLIBC_INTERNAL_INLINE_DEFINITION_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MLIBC_EMIT_INLINE_DEFINITIONS
#define __MLIBC_INLINE_DEFINITION
#else
#define __MLIBC_INLINE_DEFINITION __attribute__((__gnu_inline__)) extern __inline__
#endif

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_INTERNAL_INLINE_DEFINITION_H */

