#ifndef MLIBC_CHARCODE_HPP
#define MLIBC_CHARCODE_HPP

#include <stddef.h>
#include <stdint.h>
#include <bits/ensure.h>
#include <bits/mbstate.h>

namespace mlibc {

enum class charcode_error {
	null,
	illegal_units,
	not_enough_units
};

template<typename C>
struct code_seq {
	C *it;
	const C *end;

	explicit operator bool () {
		return it != end;
	}
};

typedef uint32_t codepoint;

// The following class deals with encoding/decoding "code points" to/from "units".
// It does not actually care about what code points represent -- they are only numeric values.
struct polymorphic_charcode {
	// decode() and encode() operate on the following state:
	// sst: "Sequence state": Stores internal decoder/encoder state
	//      (e.g. number of units missing for UTF-8).
	// pcp: "Partial code point": Stores a (partial) code point that is being processed.

	// Determines the length of a sequence of units encoding a code point.
	// In contrast to mblen()/mbrlen(), this only fails if it cannot determine the length
	// from the first n units.
	virtual charcode_error get_length(const char *s, size_t n, size_t *units) = 0;

	virtual charcode_error wdecode(code_seq<const char> &cus, code_seq<wchar_t> &cps,
			__mlibc_mbstate &st) = 0;

	virtual charcode_error wdecode_length(code_seq<const char> &cus,
			__mlibc_mbstate &st) = 0;

	// Determines the length of a sequence *and* check if there are enough
	// units left to parse the code point.
	charcode_error validate_length(const char *s, size_t n, size_t *units) {
		if(auto e = get_length(s, n, units); e != charcode_error::null)
			return e;
		if(*units > n)
			return charcode_error::not_enough_units;
		return charcode_error::null;
	}
};

polymorphic_charcode *current_charcode();

} // namespace mlibc

#endif // MLIBC_CHARCODE_HPP
