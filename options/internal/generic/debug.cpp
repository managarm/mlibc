
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc {

frg::stack_buffer_logger<InfoSink, 512> infoLogger;
frg::stack_buffer_logger<PanicSink, 512> panicLogger;

void InfoSink::operator() (const char *message) {
	sysdep<LibcLog>(message);
}

void PanicSink::operator() (const char *message) {
//	sysdep<LibcLog>("mlibc: Write to PanicSink");
	sysdep<LibcLog>(message);
	sysdep<LibcPanic>();
}

} // namespace mlibc

