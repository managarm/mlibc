#include <ctype.h>
#include <langinfo.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <bits/ensure.h>
#include <mlibc/strings.hpp>

namespace {

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

char *strptime_internal(const char *__restrict input, const char *__restrict format,
	struct tm *__restrict tm, struct strptime_internal_state *__restrict state) {
	auto matchLanginfoItem = [&] (int start, size_t num, int &dest, bool &flag) -> bool {
		for(size_t i = start; i < (start + num); i++) {
			const char *mon = nl_langinfo(i);
			size_t len = strlen(mon);
			if(mlibc::strncasecmp(&input[state->input_index], mon, len))
				continue;
			state->input_index += len;
			dest = i - start;
			flag = true;
			return true;
		}
		return false;
	};

	auto matchNumericRange = [&] (int start, int end, int &dest, bool *flag) -> bool {
		int product = 0, n = 0;
		sscanf(&input[state->input_index], "%d%n", &product, &n);
		if(n == 0 || 2 < n)
			return false;
		if(product < start || product > end)
			return false;
		state->input_index += n;
		dest = product;
		if(flag) *flag = true;
		return true;
	};

	while(isspace(input[state->input_index]))
		state->input_index++;

	if(input[state->input_index] == '\0')
		return NULL;

	while(format[state->format_index] != '\0'){
		if(format[state->format_index] != '%'){
			if(isspace(format[state->format_index])){
				while(isspace(input[state->input_index++]));
				state->input_index--;
			}
			else {
				if(format[state->format_index] != input[state->input_index++])
					return NULL;
			}
			state->format_index++;
			continue;
		}
		state->format_index++;
		switch(format[state->format_index]){
			case '%':
				if(input[state->input_index++] != '%')
					return NULL;
				break;
			case 'a':
			case 'A': {
				if (!matchLanginfoItem(DAY_1, 7, tm->tm_wday, state->has_day_of_week) && \
					!matchLanginfoItem(ABDAY_1, 7, tm->tm_wday, state->has_day_of_week))
					return NULL;
				break;
			}
			case 'b':
			case 'B':
			case 'h': {
				if (!matchLanginfoItem(MON_1, 12, tm->tm_mon, state->has_month) && \
					!matchLanginfoItem(ABMON_1, 12, tm->tm_mon, state->has_month))
					return NULL;
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
					return NULL;
				state->input_index += n;
				state->century = product;
				state->has_century = true;
				break;
			}
			case 'd': //`%d` and `%e` are equivalent
			case 'e': {
				if(!matchNumericRange(1, 31, tm->tm_mday, &state->has_day_of_month))
					return NULL;
				break;
			}
			case 'D': { //equivalent to `%m/%d/%y`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%m/%d/%y", tm, state);
				if(result == NULL)
					return NULL;

				state->format_index = pre_fi;
				break;
			}
			case 'H': {
				if(!matchNumericRange(0, 23, tm->tm_hour, nullptr))
					return NULL;
				break;
			}
			case 'I': {
				if(!matchNumericRange(1, 12, tm->tm_hour, nullptr))
					return NULL;
				break;
			}
			case 'j': {
				if(!matchNumericRange(1, 366, tm->tm_yday, &state->has_day_of_year))
					return NULL;
				tm->tm_yday--;
				break;
			}
			case 'm': {
				if(!matchNumericRange(1, 12, tm->tm_mon, &state->has_month))
					return NULL;
				tm->tm_mon--;
				break;
			}
			case 'M': {
				if(!matchNumericRange(0, 59, tm->tm_min, nullptr))
					return NULL;
				break;
			}
			case 'n':
			case 't': {
				size_t n = 0;
				while(isspace(input[state->input_index++]))
					n++;
				if(n == 0)
					return NULL;
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
				if(result == NULL)
					return NULL;

				state->format_index = pre_fi;
				break;
			}
			case 'R': { //equivalent to `%H:%M`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%H:%M", tm, state);
				if(result == NULL)
					return NULL;

				state->format_index = pre_fi;
				break;
			}
			case 'S': {
				if(!matchNumericRange(0, 60, tm->tm_sec, nullptr))
					return NULL;
				break;
			}
			case 'T': { //equivalent to `%H:%M:%S`
				size_t pre_fi = state->format_index;
				state->format_index = 0;

				char *result = strptime_internal(input, "%H:%M:%S", tm, state);
				if(result == NULL)
					return NULL;

				state->format_index = pre_fi;
				break;
			}
			case 'U':
				__ensure(!"strptime() %U directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'w': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 1 < n)
					return NULL;
				state->input_index += n;
				tm->tm_wday = product;
				state->has_day_of_week = true;
				break;
			}
			case 'W':
				__ensure(!"strptime() %W directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'x':
				__ensure(!"strptime() %x directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'X':
				__ensure(!"strptime() %X directive unimplemented.");
				__builtin_unreachable();
				break;
			case 'y': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 2 < n)
					return NULL;
				if(product < 69)
					product += 100;
				state->input_index += n;
				tm->tm_year = product;
				state->has_year = true;
				break;
			}
			case 'Y': {
				int product = 0, n = 0;
				sscanf(&input[state->input_index], "%d%n", &product, &n);
				if(n == 0 || 4 < n)
					return NULL;
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
				if(result == NULL)
					return NULL;

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
				if(!matchNumericRange(1, 7, tm->tm_wday, nullptr))
					return NULL;
				tm->tm_wday--;
				break;
			}
			case 'V':
				__ensure(!"strptime() %V directive unimplemented.");
				__builtin_unreachable();
				break;
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
			case 'E': { //locale-dependent date & time representation
				__ensure(!"strptime() %E* directives unimplemented.");
				__builtin_unreachable();
				/*
				state->format_index++;
				switch(format[state->format_index]){
					case 'c':
						break;
					case 'C':
						break;
					case 'x':
						break;
					case 'X':
						break;
					case 'y':
						break;
					case 'Y':
						break;
					default:
						return NULL;
				}
				*/
			}
			case 'O': { //locale-dependent numeric symbols
				__ensure(!"strptime() %O* directives unimplemented.");
				__builtin_unreachable();
				/*
				state->format_index++;
				switch(format[state->format_index]){
					case 'd':
					case 'e':
						break;
					case 'H':
						break;
					case 'I':
						break;
					case 'm':
						break;
					case 'M':
						break;
					case 'S':
						break;
					case 'U':
						break;
					case 'w':
						break;
					case 'W':
						break;
					case 'y':
						break;
					default:
						return NULL;
				}
				*/
			}
			default:
				return NULL;
		}
		state->format_index++;
	}

	return (char*)input + state->input_index;
}

} //anonymous namespace

char *strptime(const char *__restrict s, const char *__restrict format, struct tm *__restrict tm){
	struct strptime_internal_state state = {};

	char *result = strptime_internal(s, format, tm, &state);

	if(result == NULL)
		return NULL;

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

int clock_getcpuclockid(pid_t, clockid_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
