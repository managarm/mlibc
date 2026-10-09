#pragma once

#include <abi-bits/clockid_t.h>
#include <array>
#include <bits/ansi/timespec.h>
#include <bits/ensure.h>
#include <frg/safe_int.hpp>
#include <frg/span.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/locale.hpp>
#include <ranges>
#include <time.h>

namespace mlibc {

// Converts the absolute time `abstime` to a relative time `reltime`.
// Returns false if the conversion failed (e.g. due to over-/underflow).
// If the absolute time has already passed, `reltime` is set to 0.
inline bool time_absolute_to_relative(const clockid_t clock, const struct timespec *__restrict abstime, struct timespec *reltime) {
	constexpr long nanos_per_second = 1'000'000'000;

	if (clock != CLOCK_REALTIME && clock != CLOCK_MONOTONIC) {
		mlibc::infoLogger() << "mlibc: time_absolute_to_relative() only supports CLOCK_REALTIME and CLOCK_MONOTONIC"
			<< frg::endlog;
		return false;
	}

	struct timespec now;
	if (mlibc::sysdep<ClockGet>(clock, &now.tv_sec, &now.tv_nsec))
		__ensure(!"sys_clock_get() failed");

	if (!frg::checked_sub(abstime->tv_sec, now.tv_sec, reltime->tv_sec))
		return false;
	if (!frg::checked_sub(abstime->tv_nsec, now.tv_nsec, reltime->tv_nsec))
		return false;

	// Check if abstime has already passed.
	if (reltime->tv_sec < 0 || (reltime->tv_sec == 0 && reltime->tv_nsec < 0)) {
		reltime->tv_sec = 0;
		reltime->tv_nsec = 0;
		return true;
	} else if (reltime->tv_nsec >= nanos_per_second) {
		reltime->tv_nsec -= nanos_per_second;
		if (!frg::checked_add(reltime->tv_sec, time_t{1}, reltime->tv_sec))
			return false;
		if (reltime->tv_nsec >= nanos_per_second)
			return false;
	} else if (reltime->tv_nsec < 0) {
		reltime->tv_nsec += nanos_per_second;
		if (!frg::checked_sub(reltime->tv_sec, time_t{1}, reltime->tv_sec))
			return false;
		if (reltime->tv_nsec < 0)
			return false;
	}

	return true;
}

// Converts the relative time `reltime` to an absolute time `abstime`.
// Returns false if the conversion failed (e.g. due to over-/underflow).
inline bool time_relative_to_absolute(
    const clockid_t clock, const struct timespec *__restrict reltime, struct timespec *abstime
) {
	constexpr long nanos_per_second = 1'000'000'000;

	if (clock != CLOCK_REALTIME && clock != CLOCK_MONOTONIC) {
		mlibc::infoLogger()
		    << "mlibc: time_relative_to_absolute() only supports CLOCK_REALTIME and CLOCK_MONOTONIC"
		    << frg::endlog;
		return false;
	}

	struct timespec now;
	if (mlibc::sysdep<ClockGet>(clock, &now.tv_sec, &now.tv_nsec))
		__ensure(!"sys_clock_get() failed");

	// Add relative time to current time
	if (!frg::checked_add(now.tv_sec, reltime->tv_sec, abstime->tv_sec))
		return false;
	if (!frg::checked_add(now.tv_nsec, reltime->tv_nsec, abstime->tv_nsec))
		return false;

	// Normalize nanoseconds
	if (abstime->tv_nsec >= nanos_per_second) {
		abstime->tv_nsec -= nanos_per_second;
		if (!frg::checked_add(abstime->tv_sec, time_t{1}, abstime->tv_sec))
			return false;
		if (abstime->tv_nsec >= nanos_per_second)
			return false;
	} else if (abstime->tv_nsec < 0) {
		abstime->tv_nsec += nanos_per_second;
		if (!frg::checked_sub(abstime->tv_sec, time_t{1}, abstime->tv_sec))
			return false;
		if (abstime->tv_nsec < 0)
			return false;
	}

	if (now.tv_sec > 0 && reltime->tv_sec > 0 && abstime->tv_sec < 0)
		return false;

	return true;
}

struct era_entry {
	struct date : std::array<int32_t, 3> {
		[[nodiscard]] constexpr auto operator<=>(const date &) const noexcept = default;
	};

	uint32_t direction{};
	int32_t offset{};
	date start_date{};
	date stop_date{};
	frg::string_view era_name{};
	frg::string_view era_format{};
	frg::basic_string_view<wchar_t> era_wname{};
	frg::basic_string_view<wchar_t> era_wformat{};
	int absolute_direction{1};

	template <typename Char>
	auto name() const {
		if constexpr (std::is_same_v<wchar_t, Char>)
			return era_wname;
		else
			return era_name;
	}

	template <typename Char>
	auto format() const {
		if constexpr (std::is_same_v<wchar_t, Char>)
			return era_wformat;
		else
			return era_format;
	}
};

struct era_view : public std::ranges::view_interface<era_view> {
	struct iterator {
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = era_entry;
		using difference_type = ptrdiff_t;
		using pointer = const era_entry *;
		using reference = const era_entry &;

		constexpr iterator() = default;
		constexpr iterator(std::span<const uint8_t> data, size_t remaining_eras) noexcept
		: span_{data},
		  remaining_{remaining_eras} {
			if (remaining_ > 0 && !span_.empty())
				parse_current();
			else
				remaining_ = 0;
		}

		constexpr reference operator*() const noexcept {
			return current_entry_;
		}
		constexpr pointer operator->() const noexcept {
			return &current_entry_;
		}

		constexpr iterator &operator++() noexcept {
			if (remaining_ > 0) {
				--remaining_;
				if (remaining_ > 0 && !span_.empty()) {
					parse_current();
				} else {
					remaining_ = 0;
					span_ = {};
				}
			}
			return *this;
		}

		constexpr iterator operator++(int) noexcept {
			iterator tmp = *this;
			++(*this);
			return tmp;
		}

		constexpr bool operator==(const iterator &other) const noexcept {
			if (remaining_ == 0 && other.remaining_ == 0)
				return true;
			return remaining_ == other.remaining_ && span_.data() == other.span_.data() && span_.size() == other.span_.size();
		}

	private:
		void parse_current() noexcept {
			constexpr size_t header_size = sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t) * 3 + sizeof(int32_t) * 3;
			if (span_.size() < header_size) {
				remaining_ = 0;
				span_ = {};
				return;
			}

			const uint8_t *entry_start = span_.data();

			memcpy(&current_entry_.direction, span_.data(), sizeof(uint32_t));
			span_ = span_.subspan(sizeof(uint32_t));

			memcpy(&current_entry_.offset, span_.data(), sizeof(int32_t));
			span_ = span_.subspan(sizeof(int32_t));

			memcpy(current_entry_.start_date.data(), span_.data(), sizeof(int32_t) * 3);
			span_ = span_.subspan(sizeof(int32_t) * 3);

			memcpy(current_entry_.stop_date.data(), span_.data(), sizeof(int32_t) * 3);
			span_ = span_.subspan(sizeof(int32_t) * 3);

			if (current_entry_.start_date <= current_entry_.stop_date)
				current_entry_.absolute_direction = (current_entry_.direction == '+') ? 1 : -1;
			else
				current_entry_.absolute_direction = (current_entry_.direction == '+') ? -1 : 1;

			const char *name_str = reinterpret_cast<const char *>(span_.data());
			size_t name_len = frg::generic_strnlen(name_str, span_.size());
			if (name_len >= span_.size()) {
				remaining_ = 0;
				span_ = {};
				return;
			}
			current_entry_.era_name = frg::string_view(name_str, name_len);
			span_ = span_.subspan(name_len + 1);

			const char *format_str = reinterpret_cast<const char *>(span_.data());
			size_t format_len = frg::generic_strnlen(format_str, span_.size());
			if (format_len >= span_.size()) {
				remaining_ = 0;
				span_ = {};
				return;
			}
			current_entry_.era_format = frg::string_view(format_str, format_len);
			span_ = span_.subspan(format_len + 1);

			// 4-byte alignment
			uintptr_t offset_from_base = static_cast<uintptr_t>(span_.data() - entry_start);
			auto offset = (4 - (offset_from_base & 3)) & 3;
			if (span_.size() < offset) {
				remaining_ = 0;
				span_ = {};
				return;
			}
			span_ = span_.subspan(offset);

			size_t max_wchars = span_.size() / sizeof(wchar_t);
			if (max_wchars == 0) {
				remaining_ = 0;
				span_ = {};
				return;
			}
			const auto *wname_str = reinterpret_cast<const wchar_t *>(span_.data());
			size_t wname_len = frg::generic_strnlen(wname_str, max_wchars);
			if (wname_len >= max_wchars) {
				remaining_ = 0;
				span_ = {};
				return;
			}
			current_entry_.era_wname = frg::basic_string_view<wchar_t>(wname_str, wname_len);
			span_ = span_.subspan((wname_len + 1) * sizeof(wchar_t));

			max_wchars = span_.size() / sizeof(wchar_t);
			if (max_wchars == 0) {
				remaining_ = 0;
				span_ = {};
				return;
			}
			const auto *wformat_str = reinterpret_cast<const wchar_t *>(span_.data());
			size_t wformat_len = frg::generic_strnlen(wformat_str, max_wchars);
			if (wformat_len >= max_wchars) {
				remaining_ = 0;
				span_ = {};
				return;
			}
			current_entry_.era_wformat = frg::basic_string_view<wchar_t>(wformat_str, wformat_len);
			span_ = span_.subspan((wformat_len + 1) * sizeof(wchar_t));
		}

		std::span<const uint8_t> span_{};
		size_t remaining_ = 0;
		era_entry current_entry_ = {};
	};

	constexpr era_view(localeinfo *l)
	: data_{l->time.get(_NL_TIME_ERA_ENTRIES).asByteSpan()},
	  num_eras_{l->time.get(_NL_TIME_ERA_NUM_ENTRIES).asUint32()} {}

	[[nodiscard]] constexpr iterator begin() const noexcept {
		return iterator(data_, num_eras_);
	}

	[[nodiscard]] constexpr iterator end() const noexcept {
		return iterator({}, 0);
	}

	[[nodiscard]] constexpr size_t size() const noexcept {
		return num_eras_;
	}

private:
	std::span<const uint8_t> data_{};
	size_t num_eras_{0};
};

} // namespace mlibc
