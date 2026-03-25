#ifndef MLIBC_CHARCODE_HPP
#define MLIBC_CHARCODE_HPP

#include <stddef.h>
#include <stdint.h>
#include <bits/ensure.h>
#include <bits/mbstate.h>
#include <mlibc/debug.hpp>

namespace mlibc {

// Resulting status of a decode/encode action of a charcode.
enum class transcode_status {
	// Transcoding ended because we encountered a null terminator.
	null_terminator,
	// A malformed sequence was encountered.
	illegal_input,

	// The multibyte input ended with a incomplete codepoint.
	// The mbstate_t should encode the current conversion state.
	input_underflow,
	// The multibyte output was terminated because the next codepoint won't fit.
	// The mbstate_t should encode the current conversion state.
	output_overflow,

	// The input sequence was transcoded without encountering a null terminator.
	// This means that the mbstate_t will be in its initial state.
	// This condition might coincide with `output_exhausted`; if this happens, input_exhausted
	// takes priority.
	input_exhausted,
	// There is no more room in the output sequence, and no null terminator was encountered.
	// This means that the mbstate_t will be in its initial state.
	output_exhausted,
};

template<typename C>
struct code_seq {
	C *it;
	const C *end;

	// Returns whether there are still characters left to consume.
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

	// Helper function to decode a single char to its codepoint.
	transcode_status promote(char nc, codepoint &wc) {
		auto uc = static_cast<unsigned char>(nc);
		if(uc <= 0x7F && preserves_7bit_units) {
			wc = uc;
			return transcode_status::input_exhausted;
		}

		code_seq<const char> nseq{&nc, &nc + 1};
		code_seq<codepoint> wseq{&wc, &wc + 1};
		__mlibc_mbstate st = __MLIBC_MBSTATE_INITIALIZER;

		auto e = decode(nseq, wseq, st);
		if(e != transcode_status::input_exhausted)
			return e;
		// This should have read/written exactly one code unit/code point.
		__ensure(nseq.it == nseq.end);
		__ensure(wseq.it == wseq.end);
		return transcode_status::input_exhausted;
	}

	// Helper function to decode a single char to wchar_t.
	transcode_status promote_wtranscode(char nc, wchar_t &wc) {
		auto uc = static_cast<unsigned char>(nc);
		if(uc <= 0x7F && preserves_7bit_units) { // TODO: Use "wtranscode_preserves_7bit_units".
			wc = uc;
			return transcode_status::input_exhausted;
		}

		code_seq<const char> nseq{&nc, &nc + 1};
		code_seq<wchar_t> wseq{&wc, &wc + 1};
		__mlibc_mbstate st = __MLIBC_MBSTATE_INITIALIZER;

		auto e = decode_wtranscode(nseq, wseq, st);
		if(e != transcode_status::null_terminator && e != transcode_status::input_exhausted)
			return e;
		// This should have read/written exactly one code unit/code point.
		__ensure(nseq.it == nseq.end);
		__ensure(wseq.it == wseq.end);
		return transcode_status::input_exhausted;
	}

	polymorphic_charcode(bool preserves_7bit_units_, bool has_shift_states_)
	: preserves_7bit_units{preserves_7bit_units_}, has_shift_states{has_shift_states_} { }

	virtual transcode_status decode(code_seq<const char> &nseq, code_seq<codepoint> &wseq,
			__mlibc_mbstate &st) = 0;

	virtual transcode_status decode_wtranscode(code_seq<const char> &nseq, code_seq<wchar_t> &wseq,
			__mlibc_mbstate &st) = 0;

	virtual transcode_status decode_wtranscode_length(code_seq<const char> &nseq, size_t *n,
			__mlibc_mbstate &st) = 0;

	virtual transcode_status encode_wtranscode(code_seq<char> &nseq, code_seq<const wchar_t> &wseq,
			__mlibc_mbstate &st) = 0;

	virtual transcode_status encode_wtranscode_length(code_seq<const wchar_t> &wseq, size_t *n,
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
	transcode_status promote(wchar_t nc, codepoint &wc);
};

wide_charcode *platform_wide_charcode();

} // namespace mlibc

#endif // MLIBC_CHARCODE_HPP
