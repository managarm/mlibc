
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/internal-sysdeps.hpp>

namespace mlibc {

frg::stack_buffer_logger<InfoSink, 512> infoLogger;
frg::stack_buffer_logger<PanicSink, 512> panicLogger;

void InfoSink::operator() (const char *message) {
	sys_libc_log(message);
}

void PanicSink::operator() (const char *message) {
//	sys_libc_log("mlibc: Write to PanicSink");
	sys_libc_log(message);
	sys_libc_panic();
}

} // namespace mlibc

