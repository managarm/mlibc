#ifndef MLIBC_DEBUG_HPP
#define MLIBC_DEBUG_HPP

#include <frg/logging.hpp>

namespace mlibc {

struct InfoSink {
	// constexpr so that this can be initialized statically.
	constexpr InfoSink() = default;

	void operator() (const char *message);
};

struct PanicSink {
	// constexpr so that this can be initialized statically.
	constexpr PanicSink() = default;

	void operator() (const char *message);
};

extern frg::stack_buffer_logger<InfoSink, 512> infoLogger;
extern frg::stack_buffer_logger<PanicSink, 512> panicLogger;

} // namespace mlibc

#endif // MLIBC_DEBUG_HPP
