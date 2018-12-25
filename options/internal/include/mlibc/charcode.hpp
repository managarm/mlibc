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

// The following class deals with decoding/encoding "code units" (of type char)
// to "code points" that are defined by unicode (of type codepoint).
// It also offers convenience functions to transcode to wchar_t, char16_t and char32_t.
// We assume that the encoding of wchar_t (and char16_t, char32_t) is fixed.
// char is allowed to have an arbitrary encoding.
// TODO: char16_t and char32_t variants are missing.
// TODO: For iconv(), first decode and then encode to the destination encoding.
struct polymorphic_charcode {
	virtual charcode_error decode_wtranscode(code_seq<const char> &nseq, code_seq<wchar_t> &wseq,
			__mlibc_mbstate &st) = 0;

	virtual charcode_error decode_wtranscode_length(code_seq<const char> &nseq,
			__mlibc_mbstate &st) = 0;
};

polymorphic_charcode *current_charcode();

} // namespace mlibc

#endif // MLIBC_CHARCODE_HPP
