#include <mlibc/debug.hpp>

#ifdef __cplusplus
extern "C" {
#endif

int capset(void *, void *) {
	mlibc::infoLogger() << "mlibc: capset is a no-op!" << frg::endlog;
	return 0;
}

int capget(void *, void *) {
	mlibc::infoLogger() << "mlibc: capget is a no-op!" << frg::endlog;
	return 0;
}

#ifdef __cplusplus
}
#endif
