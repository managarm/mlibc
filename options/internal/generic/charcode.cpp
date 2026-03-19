#include <array>
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

		// Decode a single code unit.
		// This saves the current decoding state internally, so that it can be resumed by passing
		// the next code unit. Nulls are translated into a null codepoint.
		// For malformed input, transcode_status::illegal_input is returned; otherwise, the return
		// is transcode_status::input_exhausted.
		transcode_status operator() (code_seq<const char> &seq) {
			auto uc = static_cast<unsigned char>(*seq.it);
			if(!_progress) {
				if(!(uc & 0b1000'0000)) {
					// ASCII-compatible.
					_cpoint = uc;
				}else if((uc & 0b1110'0000) == 0b1100'0000) {
					// exclude overlong encodings
					if (uc < 0xC2)
						return transcode_status::illegal_input;
					_cpoint = uc & 0b1'1111;
					_progress = 1;
				}else if((uc & 0b1111'0000) == 0b1110'0000) {
					_cpoint = uc & 0b1111;
					_progress = 2;
				}else if((uc & 0b1111'1000) == 0b1111'0000) {
					// exclude 4-byte sequences greater than U+10FFFF
					if (uc >= 0xF5)
						return transcode_status::illegal_input;
					_cpoint = uc & 0b111;
					_progress = 3;
				}else{
					// If the highest two bits are 0b10, this is the second (or later) unit.
					// Units with highest five bits = 0b11111 do not occur in valid UTF-8.
					__ensure((uc & 0b1100'0000) == 0b1000'0000
							|| (uc & 0b1111'1000) == 0b1111'1000);
					return transcode_status::illegal_input;
				}
			}else{
				if((uc & 0b1100'0000) == 0b1000'0000) {
					if (_progress == 2) {
						// exclude overlong 3-byte encodings
						if (_cpoint == 0 && uc < 0xA0)
							return transcode_status::illegal_input;
						// exclude surrogate pairs (U+D800 - U+DFFF)
						if (_cpoint == 0x0D && uc >= 0xA0)
							return transcode_status::illegal_input;
					}
					if (_progress == 3) {
						// exclude overlong 4-byte encodings
						if (_cpoint == 0 && uc < 0x90)
							return transcode_status::illegal_input;
						// exclude 4-byte sequences greater than U+10FFFF
						if (_cpoint == 4 && uc >= 0x90)
							return transcode_status::illegal_input;
					}

					_cpoint = (_cpoint << 6) | (uc & 0x3F);
					--_progress;
				} else {
					return transcode_status::illegal_input;
				}
			}
			++seq.it;
			return transcode_status::input_exhausted;
		}

	private:
		int _progress;
		codepoint _cpoint;
	};

	struct encode_state {
		// Encodes a single code unit from wseq + the current state and stores it in nseq.
		transcode_status operator()(code_seq<char> &nseq, code_seq<const codepoint> &wseq) {
			auto const wc = *wseq.it;

			// Helper to encode continuation bytes
			auto cont = [](auto val) -> char { return static_cast<char>(0x80 | (val & 0x3F)); };

			// Helper to push the encoded multi-byte UTF-8 representation.
			// Returns true if all bytes were written, otherwise to false.
			auto push = [&nseq]<std::size_t N>(std::array<char, N> const &bytes) -> bool {
				for (char b : bytes) {
					if (!nseq)
						return false;
					*nseq.it = b;
					++nseq.it;
				}
				return true;
			};

			bool success = false;
			if (wc <= 0x7F) {
				success = push(std::array<char, 1>{static_cast<char>(wc)});
			} else if (wc <= 0x7FF) {
				success = push(std::array<char, 2>{static_cast<char>(0xC0 | (wc >> 6)), cont(wc)});
			} else if (wc <= 0xFFFF) {
				success = push(
				    std::array<char, 3>{
				        static_cast<char>(0xE0 | (wc >> 12)), cont(wc >> 6), cont(wc)
				    }
				);
			} else if (wc <= 0x10FFFF) {
				success = push(
				    std::array<char, 4>{
				        static_cast<char>(0xF0 | (wc >> 18)),
				        cont(wc >> 12),
				        cont(wc >> 6),
				        cont(wc)
				    }
				);
			} else {
				return transcode_status::illegal_input;
			}

			if (!success)
				return transcode_status::output_overflow;

			++wseq.it;
			return transcode_status::input_exhausted;
		}
	};
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

	// Decode the mb string `nseq` to a sequence of codepoints `wseq`.
	// Nulls are not written into wseq; instead, transcode_status::null_terminator is returned.
	transcode_status decode(code_seq<const char> &nseq, code_seq<codepoint> &wseq,
			__mlibc_mbstate &st) override {
		// Enforce that no empty output buffers are passed.
		__ensure(wseq);
		code_seq<const char> decode_nseq = nseq;
		typename G::decode_state ds(st.__progress, st.__cpoint);

		while(decode_nseq && wseq) {
			// Consume the next code unit.
			if(auto e = ds(decode_nseq); e != transcode_status::input_exhausted)
				return e;

			// Produce a new code point.
			if(!ds.progress()) {
				// "Commit" consumed code units (as there was no decode error).
				nseq.it = decode_nseq.it;
				if(!ds.cpoint()) // Stop on null characters.
					return transcode_status::null_terminator;
				*wseq.it++ = ds.cpoint();
			}
		}

		st.__cpoint = ds.cpoint();
		st.__progress = ds.progress();
		if(ds.progress())
			return transcode_status::input_underflow;

		if (!decode_nseq)
			return transcode_status::input_exhausted;
		return transcode_status::output_exhausted;
	}

	// Decode the mb string `nseq` to a sequence of wchar_t `wseq`.
	// This function advances `nseq` to one past the last transcoded char. This might point to the
	// middle of a multibyte sequence!
	// Nulls are not written into wseq; instead, `transcode_status::null_terminator` is returned.
	transcode_status decode_wtranscode(code_seq<const char> &nseq, code_seq<wchar_t> &wseq,
			__mlibc_mbstate &st) override {
		// Enforce that no empty output buffers are passed.
		__ensure(wseq);
		typename G::decode_state ds(st.__progress, st.__cpoint);

		while(nseq && wseq) {
			// Consume the next code unit.
			if(auto e = ds(nseq); e != transcode_status::input_exhausted)
				return e;

			// Produce a new code point.
			if(!ds.progress()) {
				// "Commit" consumed code units (as there was no decode error).
				if(!ds.cpoint()) // Stop on null characters.
					return transcode_status::null_terminator;
				*wseq.it++ = ds.cpoint();
			}
		}

		st.__cpoint = ds.cpoint();
		st.__progress = ds.progress();
		if(ds.progress())
			return transcode_status::input_underflow;

		if (!nseq)
			return transcode_status::input_exhausted;
		return transcode_status::output_exhausted;
	}

	// Writes the number of characters in `nseq` that form valid codepoints to `n`. Nulls are
	// not counted; instead, `transcode_status::null_terminator` gets returned.
	transcode_status decode_wtranscode_length(code_seq<const char> &nseq, size_t *n,
			__mlibc_mbstate &st) override {
		code_seq<const char> decode_nseq = nseq;
		typename G::decode_state ds(st.__progress, st.__cpoint);

		*n = 0;
		while(decode_nseq) {
			// Consume the next code unit.
			if(auto e = ds(decode_nseq); e != transcode_status::input_exhausted)
				return e;

			if(!ds.progress()) {
				// "Commit" consumed code units (as there was no decode error).
				nseq.it = decode_nseq.it;
				if(!ds.cpoint()) // Stop on null code points.
					return transcode_status::null_terminator;
				++(*n);
			}
		}

		if(ds.progress())
			return transcode_status::input_underflow;
		return transcode_status::input_exhausted;
	}

	// Transcode wide characters from `wseq` to `nseq`. Nulls are not written out; instead,
	// `transcode_status::null_terminator` is returned.
	transcode_status encode_wtranscode(code_seq<char> &nseq, code_seq<const wchar_t> &wseq,
			__mlibc_mbstate &st) override {
		__ensure(!st.__progress); // TODO: Update st with es.progress() and es.cpoint().

		code_seq<char> encode_nseq = nseq;
		typename G::encode_state es;

		while(encode_nseq && wseq) {
			codepoint cp = *wseq.it;
			if(!cp)
				return transcode_status::null_terminator;

			code_seq<const codepoint> cps{&cp, &cp + 1};
			if(auto e = es(encode_nseq, cps); e != transcode_status::input_exhausted)
				return e;

			__ensure(cps.it == cps.end);
			++wseq.it;

			// "Commit" produced code units (as there was no encode error).
			nseq.it = encode_nseq.it;
		}

		__ensure(encode_nseq.it == nseq.it);
		if (!wseq)
			return transcode_status::input_exhausted;
		return transcode_status::output_exhausted;
	}


	// Write the number of characters needed to represent the wide sequence `wseq` in a narrow
	// sequence to `n`. Nulls are not counted; instead, `transcode_status::null_terminator` is
	// returned.
	transcode_status encode_wtranscode_length(code_seq<const wchar_t> &wseq, size_t *n,
			__mlibc_mbstate &st) override {
		__ensure(!st.__progress); // TODO: Update st with es.progress() and es.cpoint().

		typename G::encode_state es;

		*n = 0;
		while(wseq) {
			char temp[MB_LEN_MAX];
			code_seq<char> encode_nseq{temp, temp + sizeof(temp)};
			codepoint cp = *wseq.it;
			if(!cp)
				return transcode_status::null_terminator;
			// Consume the next code unit.
			code_seq<const codepoint> cps{&cp, &cp + 1};
			if(auto e = es(encode_nseq, cps); e != transcode_status::input_exhausted)
				return e;

			*n += encode_nseq.it - temp;
			++wseq.it;
		}

		return transcode_status::input_exhausted;
	}
};

polymorphic_charcode *current_charcode() {
	static polymorphic_charcode_adapter<utf8_charcode> global_charcode;
	return &global_charcode;
}

transcode_status wide_charcode::promote(wchar_t nc, codepoint &wc) {
	// TODO: Allow non-identity encodings of wchar_t.
	wc = nc;
	return transcode_status::input_exhausted;
}

wide_charcode *platform_wide_charcode() {
	static wide_charcode global_wide_charcode;
	return &global_wide_charcode;
}

} // namespace mlibc

