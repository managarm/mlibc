
#include <bits/ensure.h>
#include <frg/string.hpp>
#include <mlibc/charcode.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {

struct utf8_charcode {
	static constexpr bool preserves_7bit_units = true;
	static constexpr bool has_shift_states = false;

	struct decode_state {
		decode_state()
		: _progress{0}, _cpoint{0} { }

		decode_state(int progress, codepoint cpoint)
		: _progress{progress}, _cpoint{cpoint} { }

		// Zero means we are not in the middle of parsing a codepoint.
		auto progress() { return _progress; }
		// Unicode codepoint number; only valid if progress == 0, otherwise opaque value.
		auto cpoint() { return _cpoint; }

		charcode_error operator() (code_seq<const char> &seq) {
			auto uc = static_cast<unsigned char>(*seq.it);
			if(!_progress) {
				if(!(uc & 0b1000'0000)) {
					// ASCII-compatible.
					_cpoint = uc;
				}else if((uc & 0b1110'0000) == 0b1100'0000) {
					// exclude overlong encodings
					if (uc < 0xC2)
						return charcode_error::illegal_input;
					_cpoint = uc & 0b1'1111;
					_progress = 1;
				}else if((uc & 0b1111'0000) == 0b1110'0000) {
					_cpoint = uc & 0b1111;
					_progress = 2;
				}else if((uc & 0b1111'1000) == 0b1111'0000) {
					// exclude 4-byte sequences greater than U+10FFFF
					if (uc >= 0xF5)
						return charcode_error::illegal_input;
					_cpoint = uc & 0b111;
					_progress = 3;
				}else{
					// If the highest two bits are 0b10, this is the second (or later) unit.
					// Units with highest five bits = 0b11111 do not occur in valid UTF-8.
					__ensure((uc & 0b1100'0000) == 0b1000'0000
							|| (uc & 0b1111'1000) == 0b1111'1000);
					return charcode_error::illegal_input;
				}
			}else{
				if((uc & 0b1100'0000) == 0b1000'0000) {
					if (_progress == 2) {
						// exclude overlong 3-byte encodings
						if (_cpoint == 0 && uc < 0xA0)
							return charcode_error::illegal_input;
						// exclude surrogate pairs (U+D800 - U+DFFF)
						if (_cpoint == 0x0D && uc >= 0xA0)
							return charcode_error::illegal_input;
					}
					if (_progress == 3) {
						// exclude overlong 4-byte encodings
						if (_cpoint == 0 && uc < 0x90)
							return charcode_error::illegal_input;
						// exclude 4-byte sequences greater than U+10FFFF
						if (_cpoint == 4 && uc >= 0x90)
							return charcode_error::illegal_input;
					}

					_cpoint = (_cpoint << 6) | (uc & 0x3F);
					--_progress;
				} else {
					return charcode_error::illegal_input;
				}
			}
			++seq.it;
			return charcode_error::null;
		}

	private:
		int _progress;
		codepoint _cpoint;
	};

#define NSEQ_STORE(VAL) do { \
	if (!static_cast<bool>(nseq)) { \
		return charcode_error::output_overflow; \
	} \
	*nseq.it = (VAL); \
	++nseq.it; \
} while (0)

	struct encode_state {
		// Encodes a single character from wseq + the current state and stores it in nseq.
		// TODO: Convert decode_state to the same strategy.
		charcode_error operator() (code_seq<char> &nseq, code_seq<const codepoint> &wseq) {
			auto wc = *wseq.it;
			if (wc <= 0x7F) {
				NSEQ_STORE(wc);
			} else if (wc <= 0x7FF) {
				NSEQ_STORE(0xC0 | (wc >> 6));
				NSEQ_STORE(0x80 | (wc & 0x3f));
			} else if (wc <= 0xFFFF) {
				NSEQ_STORE(0xE0 | (wc >> 12));
				NSEQ_STORE(0x80 | ((wc >> 6) & 0x3f));
				NSEQ_STORE(0x80 | (wc & 0x3f));
			} else if (wc <= 0x10FFFF) {
				NSEQ_STORE(0xF0 | (wc >> 18));
				NSEQ_STORE(0x80 | ((wc >> 12) & 0x3f));
				NSEQ_STORE(0x80 | ((wc >> 6) & 0x3f));
				NSEQ_STORE(0x80 | (wc & 0x3f));
			} else {
				return charcode_error::illegal_input;
			}
			++wseq.it;
			return charcode_error::null;
		}
	};

#undef NSEQ_STORE
};

polymorphic_charcode::~polymorphic_charcode() = default;

// For *decoding, this class assumes that:
// - G::decode_state has members progress() and cpoint().
// - G::decode_state::progress() >= 0 at all times.
//   TODO: This will be needed on platforms like Windows, where wchar_t is UTF-16.
//   TODO: There, we can use negative __mlibc_mbstate::progress to represent encoding to UTF-16.
// - If G::decode_state::progress() == 0, the code point (given by cpoint())
//   was decoded successfully.
template<typename G>
struct polymorphic_charcode_adapter : polymorphic_charcode {
	polymorphic_charcode_adapter()
	: polymorphic_charcode{G::preserves_7bit_units, G::has_shift_states} { }

	charcode_error decode(code_seq<const char> &nseq, code_seq<codepoint> &wseq,
			__mlibc_mbstate &st) override {
		code_seq<const char> decode_nseq = nseq;
		typename G::decode_state ds(st.__progress, st.__cpoint);

		while(decode_nseq && wseq) {
			// Consume the next code unit.
			if(auto e = ds(decode_nseq); e != charcode_error::null)
				return e;

			// Produce a new code point.
			if(!ds.progress()) {
				// "Commit" consumed code units (as there was no decode error).
				nseq.it = decode_nseq.it;
				if(!ds.cpoint()) // Stop on null characters.
					return charcode_error::null;
				*wseq.it++ = ds.cpoint();
			}
		}

		if(ds.progress()) {
			st.__cpoint = ds.cpoint();
			st.__progress = ds.progress();
			return charcode_error::input_underflow;
		}
		return charcode_error::input_exhausted;
	}

	charcode_error decode_wtranscode(code_seq<const char> &nseq, code_seq<wchar_t> &wseq,
			__mlibc_mbstate &st) override {
		typename G::decode_state ds(st.__progress, st.__cpoint);

		while(nseq && wseq) {
			// Consume the next code unit.
			if(auto e = ds(nseq); e != charcode_error::null)
				return e;

			// Produce a new code point.
			if(!ds.progress()) {
				// "Commit" consumed code units (as there was no decode error).
				if(!ds.cpoint()) // Stop on null characters.
					return charcode_error::null;
				*wseq.it++ = ds.cpoint();
			}
		}

		st.__cpoint = ds.cpoint();
		st.__progress = ds.progress();
		if(ds.progress())
			return charcode_error::input_underflow;

		return charcode_error::input_exhausted;
	}

	charcode_error decode_wtranscode_length(code_seq<const char> &nseq, size_t *n,
			__mlibc_mbstate &st) override {
		code_seq<const char> decode_nseq = nseq;
		typename G::decode_state ds(st.__progress, st.__cpoint);

		*n = 0;
		while(decode_nseq) {
			// Consume the next code unit.
			if(auto e = ds(decode_nseq); e != charcode_error::null)
				return e;

			if(!ds.progress()) {
				nseq.it = decode_nseq.it;
				// "Commit" consumed code units (as there was no decode error).
				if(!ds.cpoint()) // Stop on null code points.
					return charcode_error::null;
				++(*n);
			}
		}

		if(ds.progress())
			return charcode_error::input_underflow;
		return charcode_error::input_exhausted;
	}

	charcode_error encode_wtranscode(code_seq<char> &nseq, code_seq<const wchar_t> &wseq,
			__mlibc_mbstate &st) override {
		__ensure(!st.__progress); // TODO: Update st with es.progress() and es.cpoint().

		code_seq<char> encode_nseq = nseq;
		typename G::encode_state es;

		while(encode_nseq && wseq) {
			codepoint cp = *wseq.it;
			if(!cp)
				return charcode_error::null;

			code_seq<const codepoint> cps{&cp, &cp + 1};
			if(auto e = es(encode_nseq, cps); e == charcode_error::dirty) {
				continue;
			}else if(e != charcode_error::null) {
				return e;
			}
			__ensure(cps.it == cps.end);
			++wseq.it;

			// "Commit" produced code units (as there was no encode error).
			nseq.it = encode_nseq.it;
		}

		if(encode_nseq.it != nseq.it)
			return charcode_error::output_overflow;
		return charcode_error::null;
	}

	charcode_error encode_wtranscode_length(code_seq<const wchar_t> &wseq, size_t *n,
			__mlibc_mbstate &st) override {
		__ensure(!st.__progress); // TODO: Update st with es.progress() and es.cpoint().

		typename G::encode_state es;

		*n = 0;
		while(wseq) {
			char temp[4];
			code_seq<char> encode_nseq{temp, temp + 4};
			codepoint cp = *wseq.it;
			if(!cp)
				return charcode_error::null;
			// Consume the next code unit.
			code_seq<const codepoint> cps{&cp, &cp + 1};
			if(auto e = es(encode_nseq, cps); e == charcode_error::dirty) {
				continue;
			}else if(e != charcode_error::null) {
				return e;
			}

			*n += encode_nseq.it - temp;
			++wseq.it;
		}

		return charcode_error::null;
	}
};

polymorphic_charcode *current_charcode() {
	static polymorphic_charcode_adapter<utf8_charcode> global_charcode;
	return &global_charcode;
}

charcode_error wide_charcode::promote(wchar_t nc, codepoint &wc) {
	// TODO: Allow non-identity encodings of wchar_t.
	wc = nc;
	return charcode_error::null;
}

wide_charcode *platform_wide_charcode() {
	static wide_charcode global_wide_charcode;
	return &global_wide_charcode;
}

} // namespace mlibc

