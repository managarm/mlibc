#ifndef MLIBC_DEBUG_HPP
#define MLIBC_DEBUG_HPP

#include <frg/logging.hpp>

namespace mlibc {

struct InfoSink {
	void operator() (const char *message);
};

struct PanicSink {
	void operator() (const char *message);
};

extern frg::stack_buffer_logger<InfoSink> infoLogger;
extern frg::stack_buffer_logger<PanicSink> panicLogger;

} // namespace mlibc

#endif // MLIBC_DEBUG_HPP
