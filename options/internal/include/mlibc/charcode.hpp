#ifndef MLIBC_CHARCODE_HPP
#define MLIBC_CHARCODE_HPP

#include <stddef.h>
#include <stdint.h>
#include <bits/ensure.h>
#include <bits/mbstate.h>
#include <mlibc/debug.hpp>

namespace mlibc {

enum class charcode_error {
	null,
	dirty,
	illegal_input,
	input_underflow,
	output_overflow
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
	virtual ~polymorphic_charcode();

	// Helper function to decode a single char.
	charcode_error promote(char nc, codepoint &wc) {
		auto uc = static_cast<unsigned char>(nc);
		if(uc <= 0x7F && preserves_7bit_units) {
			wc = uc;
			return charcode_error::null;
		}

		code_seq<const char> nseq{&nc, &nc + 1};
		code_seq<codepoint> wseq{&wc, &wc + 1};
		__mlibc_mbstate st = __MLIBC_MBSTATE_INITIALIZER;

		if(auto e = decode(nseq, wseq, st); e != charcode_error::null)
			return e;
		// This should have read/written exactly one code unit/code point.
		__ensure(nseq.it == nseq.end);
		__ensure(wseq.it == wseq.end);
		return charcode_error::null;
	}

	// Helper function to decode a single char.
	charcode_error promote_wtranscode(char nc, wchar_t &wc) {
		auto uc = static_cast<unsigned char>(nc);
		if(uc <= 0x7F && preserves_7bit_units) { // TODO: Use "wtranscode_preserves_7bit_units".
			wc = uc;
			return charcode_error::null;
		}

		code_seq<const char> nseq{&nc, &nc + 1};
		code_seq<wchar_t> wseq{&wc, &wc + 1};
		__mlibc_mbstate st = __MLIBC_MBSTATE_INITIALIZER;

		if(auto e = decode_wtranscode(nseq, wseq, st); e != charcode_error::null)
			return e;
		// This should have read/written exactly one code unit/code point.
		__ensure(nseq.it == nseq.end);
		__ensure(wseq.it == wseq.end);
		return charcode_error::null;
	}

	polymorphic_charcode(bool preserves_7bit_units_, bool has_shift_states_)
	: preserves_7bit_units{preserves_7bit_units_}, has_shift_states{has_shift_states_} { }

	virtual charcode_error decode(code_seq<const char> &nseq, code_seq<codepoint> &wseq,
			__mlibc_mbstate &st) = 0;

	virtual charcode_error decode_wtranscode(code_seq<const char> &nseq, code_seq<wchar_t> &wseq,
			__mlibc_mbstate &st) = 0;

	virtual charcode_error decode_wtranscode_length(code_seq<const char> &nseq, size_t *n,
			__mlibc_mbstate &st) = 0;

	virtual charcode_error encode_wtranscode(code_seq<char> &nseq, code_seq<const wchar_t> &wseq,
			__mlibc_mbstate &st) = 0;

	virtual charcode_error encode_wtranscode_length(code_seq<const wchar_t> &wseq, size_t *n,
			__mlibc_mbstate &st) = 0;

	// True if promotion only zero-extends units below 0x7F.
	const bool preserves_7bit_units;

	// Whether the encoding has shift states.
	const bool has_shift_states;
};

polymorphic_charcode *current_charcode();

// Similar to polymorphic_charcode but for wchar_t. Note that this encoding is fixed per-platform;
// thus, it does not need to be polymorphic.
struct wide_charcode {
	charcode_error promote(wchar_t nc, codepoint &wc);
};

wide_charcode *platform_wide_charcode();

} // namespace mlibc

#endif // MLIBC_CHARCODE_HPP
