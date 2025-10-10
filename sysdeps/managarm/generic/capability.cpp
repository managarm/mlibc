#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {

int sys_capget(cap_user_header_t hdrp, cap_user_data_t datap) {
	mlibc::infoLogger() << "mlibc: capget is a no-op!" << frg::endlog;
	return 0;
}

int sys_capset(cap_user_header_t hdrp, const cap_user_data_t datap) {
	mlibc::infoLogger() << "mlibc: capset is a no-op!" << frg::endlog;
	return 0;
}

} // namespace mlibc
