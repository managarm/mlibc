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
	virtual charcode_error wdecode(code_seq<const char> &cus, code_seq<wchar_t> &cps,
			__mlibc_mbstate &st) = 0;

	virtual charcode_error wdecode_length(code_seq<const char> &cus,
			__mlibc_mbstate &st) = 0;
};

polymorphic_charcode *current_charcode();

} // namespace mlibc

#endif // MLIBC_CHARCODE_HPP
