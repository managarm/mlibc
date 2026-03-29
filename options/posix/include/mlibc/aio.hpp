#pragma once

#include <aio.h>
#include <bits/ensure.h>
#include <errno.h>
#include <expected>
#include <mlibc/all-sysdeps.hpp>
#include <optional>
#include <utility>

namespace mlibc {

enum class AioOpState : int {
	invalid,
	success,
	in_progress,
	cancelled,
};

template <typename T>
struct BasicAioCbView {
	T *cb_;

	explicit constexpr BasicAioCbView(T *cb) noexcept : cb_{cb} {}

	[[nodiscard]] inline AioOpState get_state() const noexcept {
		return AioOpState(__atomic_load_n(&cb_->__state, __ATOMIC_ACQUIRE));
	}

	// Reset the result and error fields.
	inline void reset_operation_result() const noexcept
	    requires(!std::is_const_v<T>)
	{
		cb_->__res = 0;
		cb_->__err = 0;
	}

	// Set the outcome (return value and error code) of an operation in the aiocb.
	inline void set_operation_result(std::expected<ssize_t, int> result) const noexcept
	    requires(!std::is_const_v<T>)
	{
		cb_->__res = result.value_or(-1);
		cb_->__err = result.error_or(0);
	}

	// Atomically set the state with release semantics.
	inline void set_state(AioOpState state, bool wake = false) const noexcept
	    requires(!std::is_const_v<T>)
	{
		__atomic_store_n(&cb_->__state, std::to_underlying(state), __ATOMIC_RELEASE);
		if (wake)
			sysdep<FutexWake>(&cb_->__state, true);
	}

	inline bool
	exchange_state(AioOpState state, AioOpState expected, bool wake = true) const noexcept {
		int expected_val = std::to_underlying(expected);

		if (__atomic_compare_exchange_n(
		        &cb_->__state,
		        &expected_val,
		        std::to_underlying(state),
		        false,
		        __ATOMIC_RELEASE,
		        __ATOMIC_RELAXED
		    )) {
			if (wake)
				sysdep<FutexWake>((int *)&cb_->__state, true);
			return true;
		}
		return false;
	}
};

using AioCbView = BasicAioCbView<struct aiocb>;
using ConstAioCbView = BasicAioCbView<const struct aiocb>;

} // namespace mlibc
