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

// Some encodings (e.g. the one defined in RFC 1843) have "shift states",
// i.e. escape sequences that switch between different encodings (e.g. between single-byte ASCII
// and 2-byte encoding of Chinese characters).
// TODO: Implement that using the __shift member of __mlibc_mbstate.

typedef uint32_t codepoint;

// The following class deals with decoding/encoding "code units" to/from unicode "code points".
// We assume that wchar_t (and char16_t, char32_t) are represent unicode characters.
// char is allowed to have an arbitrary encoding.
// TODO: For iconv(), first convert to char32_t and then to the destination encoding.
struct polymorphic_charcode {
	virtual charcode_error wdecode(code_seq<const char> &cus, code_seq<wchar_t> &cps,
			__mlibc_mbstate &st) = 0;

	virtual charcode_error wdecode_length(code_seq<const char> &cus,
			__mlibc_mbstate &st) = 0;
};

polymorphic_charcode *current_charcode();

} // namespace mlibc

#endif // MLIBC_CHARCODE_HPP
