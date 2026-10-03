#include <ctype.h>
#include <errno.h>
#include <langinfo.h>
#include <ranges>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/time.hpp>
#include <mlibc/strings.hpp>

namespace {

template <std::integral auto N>
consteval int count_decimal_digits() noexcept {
	using U = std::make_unsigned_t<decltype(N)>;
	U val = (N < 0) ? (0 - static_cast<U>(N)) : static_cast<U>(N);
	int digits = 1;
	while (val >= 10) {
		val /= 10;
		++digits;
	}
	return digits;
}

int month_to_day(int month) {
	switch(month){
		case  0: return 0;
		case  1: return 31;
		case  2: return 59;
		case  3: return 90;
		case  4: return 120;
		case  5: return 151;
		case  6: return 181;
		case  7: return 212;
		case  8: return 243;
		case  9: return 273;
		case 10: return 304;
		case 11: return 334;
	}
	return -1;
}

int is_leapyear(int year) {
	return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

int month_and_year_to_day_in_year(int month, int year){
	int day = month_to_day(month);
	if(is_leapyear(year) && month < 2)
		return day + 1;

	return day;
}

int target_determination(int month) {
	switch(month){
		case 0: return 3;
		case 1: return 14;
		case 2: return 14;
		case 3: return 4;
		case 4: return 9;
		case 5: return 6;
		case 6: return 11;
		case 7: return 8;
		case 8: return 5;
		case 9: return 10;
		case 10: return 7;
		case 11: return 12;
	}

	return -1;
}

int doom_determination(int full_year) {
	int century = full_year / 100;
	int anchor = 2 + 5 * (century % 4) % 7;

	int year = full_year % 100;

	if(year % 2)
		year += 11;

	year /= 2;

	if(year % 2)
		year += 11;

	return 7 - (year % 7) + anchor;
}

//Determine day of week through the doomsday algorithm.
int day_determination(int day, int month, int year) {
	int doom = doom_determination(year);
	bool leap = is_leapyear(year);

	int target = target_determination(month);
	if(leap && month < 2)
		target++;

	int doom_dif = (day - target) % 7;
	return (doom + doom_dif) % 7;
}

struct strptime_internal_state {
	bool has_century;
	bool has_year;
	bool has_month;
	bool has_day_of_month;
	bool has_day_of_year;
	bool has_day_of_week;

	bool full_year_given;

	int century;

	size_t format_index;
	size_t input_index;
};

char *strptime_internal(
    const char *__restrict input,
    const char *__restrict format,
    struct tm *__restrict tm,
    struct strptime_internal_state *__restrict state,
    mlibc::localeinfo *l = mlibc::getActiveLocale()
) {
	auto matchLanginfoItem = [&]<int Nlitem, size_t Num>(int &dest, bool &flag) -> bool {
		const char *current_input = &input[state->input_index];

		struct Match {
			size_t len;
			int relative_idx;
		};
		std::optional<Match> best_match;
		const int count = std::cmp_less_equal(Num, std::numeric_limits<int>::max())
		                      ? static_cast<int>(Num)
		                      : std::numeric_limits<int>::max();

		for (int offset : std::views::iota(0, count)) {
			auto item = l->time.get(Nlitem + offset).asString();
			if (item.empty())
				continue;
			if (item[item.size() - 1] == '\0')
				item = item.sub_string(0, item.size() - 1);
			if (best_match && item.size() <= best_match->len)
				continue;

			if (mlibc::strncasecmp(current_input, item.data(), item.size()) == 0)
				best_match = {item.size(), offset};
		}

		if (best_match) {
			state->input_index += best_match->len;
			dest = best_match->relative_idx;
			flag = true;
			return true;
		}

		return false;
	};

	auto matchNumericRange = [&]<int Start, int End>(int &dest, bool *flag) -> bool {
		int product = 0, n = 0;
		sscanf(&input[state->input_index], "%d%n", &product, &n);
		if (n == 0 || count_decimal_digits<End>() < n)
			return false;
		if (product < Start || product > End)
			return false;
		state->input_index += n;
		dest = product;
		if (flag)
			*flag = true;
		return true;
	};

	auto matchAltDigits = [&]<int Start, int End>(int &dest, bool *flag) -> bool {
		auto altdigits = mlibc::getActiveLocale()->time.get(ALT_DIGITS).asString();
		if (altdigits.empty())
			return false;

		frg::string_view current_input{&input[state->input_index]};

		auto matches = altdigits
			| std::views::split('\0')
			| std::views::transform([](auto &&subrange) {
				return frg::string_view{
					std::ranges::data(subrange),
					static_cast<size_t>(std::ranges::distance(subrange))
				};
			})
			| std::views::enumerate
			| std::views::filter([&](auto &&entry) {
				auto [val, digit] = entry;
				return val >= Start && val <= End && !digit.empty() && current_input.starts_with(digit);
			});

		auto it = std::ranges::max_element(matches, {}, [](auto &&entry) {
			auto [val, digit] = entry;
			return digit.size();
		});

		if (it != matches.end()) {
			auto [val, digit] = *it;
			state->input_index += digit.size();
			dest = val;
			if (flag)
				*flag = true;
			return true;
		}

		return false;
	};

	while(isspace(input[state->input_index]))
		state->input_index++;

	if(input[state->input_index] == '\0')
		return nullptr;

	while(format[state->format_index] != '\0'){
		if(format[state->format_index] != '%'){
			if(isspace(format[state->format_index])){
				while(isspace(input[state->input_index++]));
				state->input_index--;
			}
			else {
				if(format[state->format_index] != input[state->input_index++])
					return nullptr;
			}
			state->format_index++;
			continue;
		}
		state->format_index++;

		bool alternate_symbols = false;
		[[maybe_unused]] bool alternate_era = false;

		if (format[state->format_index] == 'O') {
			alternate_symbols = true;
			state->format_index++;
		} else if (format[state->format_index] == 'E') {
			alternate_era = true;
			state->format_index++;
			__ensure(!"strptime() %E* directives unimplemented.");
			__builtin_unreachable();
		}

		auto matchDigits = [&]<int Start, int End>(int &dest, bool *flag = nullptr) -> bool {
			if (alternate_symbols)
				return matchAltDigits.operator()<Start, End>(dest, flag);
			else
				return matchNumericRange.operator()<Start, End>(dest, flag);
		};

		switch(format[state->format_index]){
			case '%':
				if(input[state->input_index++] != '%')
					return nullptr;
				break;
			case 'a':
			case 'A': {
				if (!matchLanginfoItem.operator()<DAY_1, 7>(tm->tm_wday, state->has_day_of_week) && \
					!matchLanginfoItem.operator()<ABDAY_1, 7>(tm->tm_wday, state->has_day_of_week))
					return nullptr;
				break;
			}
			case 'b':
			case 'B':
			case 'h': {
				if (!matchLanginfoItem.operator()<MON_1, 12>(tm->tm_mon, state->has_month) && \
					!matchLanginfoItem.operator()<ALTMON_1, 12>(tm->tm_mon, state->has_month) && \
					!matchLanginfoItem.operator()<ABMON_1, 12>(tm->tm_mon, state->has_month) && \
					!matchLanginfoItem.operator()<ABALTMON_1, 12>(tm->tm_mon, state->has_month))
					return nullptr;
				break;
			}
			case 'c':
				__ensure(!"strptime() %c directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'C': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 2 < n)
					return nullptr;
				state->input_index += n;
				state->century = product;
				state->has_century = true;
				break;
			}
			case 'd': //`%d` and `%e` are equivalent
			case 'e': {
				if(!matchDigits.operator()<1, 31>(tm->tm_mday, &state->has_day_of_month))
					return nullptr;
				break;
			}
			case 'D': { //equivalent to `%m/%d/%y`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%m/%d/%y", tm, state);
				if(result == nullptr)
					return nullptr;

				state->format_index = pre_fi;
				break;
			}
			case 'H': {
				if (!matchDigits.operator()<0, 23>(tm->tm_hour))
					return nullptr;
				break;
			}
			case 'I': {
				if (!matchDigits.operator()<1, 12>(tm->tm_hour))
					return nullptr;
				break;
			}
			case 'j': {
				if(!matchNumericRange.operator()<1, 366>(tm->tm_yday, &state->has_day_of_year))
					return nullptr;
				tm->tm_yday--;
				break;
			}
			case 'm': {
				if (!matchDigits.operator()<0, 23>(tm->tm_mon, &state->has_month))
					return nullptr;
				tm->tm_mon--;
				break;
			}
			case 'M': {
				if (!matchDigits.operator()<0, 59>(tm->tm_min))
					return nullptr;
				break;
			}
			case 'n':
			case 't': {
				size_t n = 0;
				while(isspace(input[state->input_index++]))
					n++;
				if(n == 0)
					return nullptr;
				state->input_index--;
				break;
			}
			case 'p': {
				const char *meridian_str = nl_langinfo(AM_STR);
				size_t len = strlen(meridian_str);
				if (!mlibc::strncasecmp(&input[state->input_index], meridian_str, len)) {
					tm->tm_hour %= 12;
					state->input_index += len;
					break;
				}
				meridian_str = nl_langinfo(PM_STR);
				len = strlen(meridian_str);
				if (!mlibc::strncasecmp(&input[state->input_index], meridian_str, len)) {
					tm->tm_hour %= 12;
					tm->tm_hour += 12;
					state->input_index += len;
					break;
				}
				break;
			}
			case 'r': {  //equivalent to `%I:%M:%S %p`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%I:%M:%S %p", tm, state);
				if(result == nullptr)
					return nullptr;

				state->format_index = pre_fi;
				break;
			}
			case 'R': { //equivalent to `%H:%M`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%H:%M", tm, state);
				if(result == nullptr)
					return nullptr;

				state->format_index = pre_fi;
				break;
			}
			case 'S': {
				if (!matchDigits.operator()<0, 60>(tm->tm_sec, nullptr))
					return nullptr;
				break;
			}
			case 'T': { //equivalent to `%H:%M:%S`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%H:%M:%S", tm, state);
				if(result == nullptr)
					return nullptr;

				state->format_index = pre_fi;
				break;
			}
			case 'U':
				__ensure(!"strptime() %U directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'w': {
				if (alternate_symbols) {
					if (!matchAltDigits.operator()<0, 6>(tm->tm_wday, &state->has_day_of_week))
						return nullptr;
				} else {
					int product = 0, n = 0;
					sscanf(&input[state->input_index], "%d%n", &product, &n);
					if(n == 0 || 1 < n)
						return nullptr;
					state->input_index += n;
					tm->tm_wday = product;
					state->has_day_of_week = true;
				}
				break;
			}
			case 'W': {
				// POSIX: The effect of this conversion, if any, on the tm structure is unspecified.
				int dummy;
				if (!matchDigits.operator()<0, 53>(dummy, nullptr))
					return nullptr;
				break;
			}
			case 'x':
				__ensure(!"strptime() %x directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'X':
				__ensure(!"strptime() %X directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'y': {
				if (alternate_symbols) {
					int product = 0;
					if (!matchAltDigits.operator()<0, 99>(product, nullptr))
						return nullptr;
					if(product < 69)
						product += 100;
					tm->tm_year = product;
					state->has_year = true;
				} else {
					int product = 0, n = 0;
					sscanf(&input[state->input_index], "%d%n", &product, &n);
					if(n == 0 || 2 < n)
						return nullptr;
					if(product < 69)
						product += 100;
					state->input_index += n;
					tm->tm_year = product;
					state->has_year = true;
				}
				break;
			}
			case 'Y': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 4 < n)
					return nullptr;
				state->input_index += n;
				tm->tm_year = product - 1900;
				state->has_year = true;
				state->has_century = true;
				state->full_year_given = true;
				state->century = product / 100;
				break;
			}
			case 'F': { //GNU extensions
				//equivalent to `%Y-%m-%d`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%Y-%m-%d", tm, state);
				if(result == nullptr)
					return nullptr;

				state->format_index = pre_fi;
				break;
			}
			case 'g':
				__ensure(!"strptime() %g directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'G':
				__ensure(!"strptime() %G directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'u': {
				if(!matchNumericRange.operator()<1, 7>(tm->tm_wday, nullptr))
					return nullptr;
				tm->tm_wday--;
				break;
			}
			case 'V': {
				// POSIX: The effect of this conversion, if any, on the tm structure is unspecified.
				int dummy;
				if (!matchDigits.operator()<1, 53>(dummy, nullptr))
					return nullptr;
				break;
			}
			case 'z':
				__ensure(!"strptime() %z directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'Z':
				__ensure(!"strptime() %Z directive unimplemented.");
				__builtin_unreachable();
				break;
			case 's': //end of GNU extensions
				__ensure(!"strptime() %s directive unimplemented.");
				__builtin_unreachable();
				break;
			default:
				return nullptr;
		}
		state->format_index++;
	}

	return (char*)input + state->input_index;
}

} //anonymous namespace

char *strptime(const char *__restrict s, const char *__restrict format, struct tm *__restrict tm){
	struct strptime_internal_state state = {};

	char *result = strptime_internal(s, format, tm, &state);

	if(result == nullptr)
		return nullptr;

	if(state.has_century && !state.full_year_given){
		int full_year = state.century * 100;

		if(state.has_year){
			//Compensate for default century-adjustment of `%j` operand
			if(tm->tm_year >= 100)
				full_year += tm->tm_year - 100;
			else
				full_year += tm->tm_year;
		}

		tm->tm_year = full_year - 1900;

		state.has_year = true;
	}

	if(state.has_month && !state.has_day_of_year){
		int day = 0;
		if(state.has_year)
			day = month_and_year_to_day_in_year(tm->tm_mon, tm->tm_year);
		else
			day = month_to_day(tm->tm_mon);

		tm->tm_yday = day + tm->tm_mday - 1;
		state.has_day_of_year = true;
	}

	if(state.has_year && !state.has_day_of_week){
		if(!state.has_month && !state.has_day_of_month){
			tm->tm_wday = day_determination(0, 0, tm->tm_year + 1900);
		}
		else if(state.has_month && state.has_day_of_month){
			tm->tm_wday = day_determination(tm->tm_mday, tm->tm_mon, tm->tm_year + 1900);
		}
		state.has_day_of_week = true;
	}

	return result;
}

size_t strftime_l(char *__restrict s, size_t maxsize, const char *__restrict format, const struct tm *__restrict timeptr, locale_t l) {
	return mlibc::strftime(s, maxsize, format, timeptr, static_cast<mlibc::localeinfo *>(l));
}

int clock_getcpuclockid(pid_t, clockid_t *) {
	mlibc::infoLogger() << "mlibc: clock_getcpuclockid unconditionally returns ENOSYS" << frg::endlog;
	return ENOSYS;
}
