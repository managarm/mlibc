#ifndef MLIBC_CHARCODE_HPP
#define MLIBC_CHARCODE_HPP

#include <stddef.h>
#include <stdint.h>
#include <bits/ensure.h>

namespace mlibc {

enum class charcode_error {
	null,
	illegal_units,
	not_enough_units
};

typedef uint32_t codepoint;

template<typename U>
struct basic_charcode {
	// Determines the length of a sequence of units encoding a character.
	// In contrast to mblen()/mbrlen(), this only fails if it cannot determine the length
	// from the first n units.
	virtual charcode_error get_length(const U *s, size_t n, size_t *units) = 0;

	// TODO: decode() (and also encode()) are not used yet;
	// we discuss the a possible design for future reference:
	// Possible design that fits into mbstate_t:
	// sst: "Sequence state": Stores internal decoder/encoder state
	//      (e.g. number of units missing for UTF-8).
	// pcp: "Partial code point": Stores a (partial) code point that is being processed.
	virtual void decode(const U *s, size_t n, codepoint *out,
			unsigned int *sst, codepoint *pcp) { };

	// Determines the length of a sequence *and* check if there are enough
	// units left to parse the character.
	charcode_error validate_length(const U *s, size_t n, size_t *units) {
		if(auto e = get_length(s, n, units); e != charcode_error::null)
			return e;
		if(*units > n)
			return charcode_error::not_enough_units;
		return charcode_error::null;
	}
};

using charcode = basic_charcode<char>;
using wcharcode = basic_charcode<wchar_t>;

charcode *current_charcode();

} // namespace mlibc

#endif // MLIBC_CHARCODE_HPP
