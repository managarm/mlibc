
#ifndef MLIBC_ENSURE_H
#define MLIBC_ENSURE_H

#ifdef __cplusplus
extern "C" {
#endif

void __ensure_fail(const char *assertion, const char *file, unsigned int line,
		const char *function);

#define __ensure(assertion) do { if(!(assertion)) \
		__ensure_fail(#assertion, __FILE__, __LINE__, __func__); } while(0)

#define MLIBC_UNIMPLEMENTED() __ensure_fail("Functionality is not implemented", \
		__FILE__, __LINE__, __func__)

#ifdef __cplusplus
}
#endif

#endif // MLIBC_ENSURE_H

