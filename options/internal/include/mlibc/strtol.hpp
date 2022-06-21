#ifndef MLIBC_STRTOL_HPP
#define MLIBC_STRTOL_HPP

#include <type_traits>
#include <ctype.h>
#include <wctype.h>
#include <limits.h>

namespace mlibc {

template<typename T> struct int_limits {};

template<>
struct int_limits<long> {
	static long max() { return LONG_MAX; }
	static long min() { return LONG_MIN; }
};

template<>
struct int_limits<unsigned long> {
	static unsigned long max() { return ULONG_MAX; }
	static unsigned long min() { return 0; }
};

template<>
struct int_limits<long long> {
	static long long max() { return LLONG_MAX; }
	static long long min() { return LLONG_MIN; }
};

template<>
struct int_limits<unsigned long long> {
	static unsigned long long max() { return ULLONG_MAX; }
	static unsigned long long min() { return 0; }
};

template<typename T> struct char_detail {};

template<>
struct char_detail<char> {
	static bool isSpace(char c) { return isspace(c); }
	static bool isDigit(char c) { return isdigit(c); }
	static bool isLower(char c) { return islower(c); }
	static bool isUpper(char c) { return isupper(c); }
};

template<>
struct char_detail<wchar_t> {
	static bool isSpace(wchar_t c) { return iswspace(c); }
	static bool isDigit(wchar_t c) { return iswdigit(c); }
	static bool isLower(wchar_t c) { return iswlower(c); }
	static bool isUpper(wchar_t c) { return iswupper(c); }
};

template<typename Char> Char widen(char c) { return static_cast<Char>(c); }

template<typename Return, typename Char>
Return stringToInteger(const Char *__restrict nptr, Char **__restrict endptr, int baseInt) {
	using UnsignedReturn = std::make_unsigned_t<Return>;

	auto base = static_cast<Return>(baseInt);
	auto s = nptr;

	if (base < 0 || base == 1) {
		if (endptr)
			*endptr = const_cast<Char *>(nptr);
		return 0;
	}

	while (char_detail<Char>::isSpace(*s))
		s++;

	bool negative = false;
	if (*s == widen<Char>('-')) {
		negative = true;
		s++;
	} else if (*s == widen<Char>('+')) {
		s++;
	}

	bool hasOctalPrefix = s[0] == widen<Char>('0');
	bool hasHexPrefix = hasOctalPrefix && (s[1] == widen<Char>('x') || s[1] == widen<Char>('X'));
	if ((base == 0 || base == 16) && hasHexPrefix) {
		s += 2;
		base = 16;
	} else if ((base == 0 || base == 8) && hasOctalPrefix) {
		base = 8;
	} else if (base == 0) { 
		base = 10;
	}

	// Compute the range of acceptable values.
	UnsignedReturn cutoff, cutlim;
	if (std::is_unsigned_v<Return>) {
		cutoff = int_limits<Return>::max() / base;
		cutlim = int_limits<Return>::max() % base;
	} else {
		Return co = negative ? int_limits<Return>::min() : int_limits<Return>::max();
		cutlim = negative ? -(co % base) : co % base;
		co /= negative ? -base : base;
		cutoff = co;
	}

	UnsignedReturn totalValue = 0;
	bool convertedAny = false;
	bool outOfRange = false;
	for (Char c = *s; c != widen<Char>('\0'); c = *++s) {
		UnsignedReturn digitValue;
		if (char_detail<Char>::isDigit(c))
			digitValue = c - widen<Char>('0');
		else if (char_detail<Char>::isUpper(c))
			digitValue = c - widen<Char>('A') + 10;
		else if (char_detail<Char>::isLower(c))
			digitValue = c - widen<Char>('a') + 10;
		else
			break;

		if (digitValue >= static_cast<UnsignedReturn>(base))
			break;

		if (outOfRange) {
			// The value is already known to be out of range, but we need to keep
			// consuming characters until we can't (to set endptr correctly).
		} else if (totalValue > cutoff || (totalValue == cutoff && digitValue > cutlim)) {
			// The value will be out of range if we accumulate digitValue.
			outOfRange = true;
		} else {
			totalValue = (totalValue * base) + digitValue;
			convertedAny = true;
		}
	}

	if (endptr)
		*endptr = const_cast<Char *>(convertedAny ? s : nptr);

	if (outOfRange) {
		errno = ERANGE;

		if (std::is_unsigned_v<Return>) {
			return int_limits<Return>::max();
		} else {
			return negative ? int_limits<Return>::min() : int_limits<Return>::max();
		}
	}

	return negative ? -totalValue : totalValue;
}

}

#endif // MLIBC_STRTOL_HPP
