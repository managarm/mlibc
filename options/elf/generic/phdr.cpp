#include <link.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

int dl_iterate_phdr(int (*callback)(struct dl_phdr_info*, size_t, void*), void* data){
	mlibc::infoLogger() << "mlibc: dl_iterate_phdr is a stub" << frg::endlog;
	return 0;
}