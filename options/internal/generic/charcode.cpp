
#include <bits/ensure.h>
#include <frg/string.hpp>
#include <mlibc/charcode.hpp>

namespace mlibc {

struct utf8_charcode {
	struct decode_state {
		decode_state()
		: _progress{0}, _cpoint{0} { }

		auto progress() { return _progress; }
		auto cpoint() { return _cpoint; }

		charcode_error operator() (code_seq<const char> &seq) {
			auto uc = static_cast<unsigned char>(*seq.it);
			if(!_progress) {
				if(!(uc & 0b1000'0000)) {
					// ASCII-compatible.
					_cpoint = uc;
				}else if((uc & 0b1110'0000) == 0b1100'0000) {
					_cpoint = uc & 0b1'1111;
					_progress = 1;
				}else if((uc & 0b1111'0000) == 0b1110'0000) {
					_cpoint = uc & 0b1111;
					_progress = 2;
				}else if((uc & 0b1111'1000) == 0b1111'0000) {
					_cpoint = uc & 0b111;
					_progress = 3;
				}else{
					// If the highest two bits are 0b10, this is the second (or later) unit.
					__ensure(!((uc & 0b1100'0000) == 0b1000'0000));
					// Units with highest five bits = 0b11111 do not occur in valid UTF-8.
					__ensure(!"Unit cannot occur in valid UTF-8 character");
				}
			}else{
				__ensure((uc & 0b1100'0000) == 0b1000'0000);
				_cpoint = (_cpoint << 6) | (uc & 0x3F);
				--_progress;
			}
			++seq.it;
			return charcode_error::null;
		}

	private:
		int _progress;
		codepoint _cpoint;
	};
};

// For *decoding, this class assumes that:
// - G::decode_state has members progress() and cpoint().
// - G::decode_state::progress() >= 0 at all times.
//   TODO: This will be needed on platforms like Windows, where wchar_t is UTF-16.
//   TODO: There, we can use negative __mlibc_mbstate::progress to represent encoding to UTF-16.
// - If G::decode_state::progress() == 0, the code point (given by cpoint())
//   was decoded successfully.
template<typename G>
struct polymorphic_charcode_adapter : polymorphic_charcode {
	charcode_error decode_wtranscode(code_seq<const char> &nseq, code_seq<wchar_t> &wseq,
			__mlibc_mbstate &st) override {
		__ensure(!st.__progress); // TODO: Update st with d.progress() and d.cpoint().

		code_seq<const char> dnseq = nseq;
		typename G::decode_state d;

		while(dnseq && wseq) {
			// Consume the next code unit.
			if(auto e = d(dnseq); e != charcode_error::null)
				return e;

			// Produce a new code point.
			if(!d.progress()) {
				nseq.it = dnseq.it; // "Commit" consumed code units (as there was no decode error).
				if(!d.cpoint()) // Stop on null characters.
					return charcode_error::null;
				*wseq.it = d.cpoint();
				++wseq.it;
			}
		}

		if(d.progress())
			return charcode_error::not_enough_units;
		return charcode_error::null;
	}

	charcode_error decode_wtranscode_length(code_seq<const char> &nseq,
			__mlibc_mbstate &st) override {
		__ensure(!st.__progress); // TODO: Update st with d.progress() and d.cpoint().

		code_seq<const char> dnseq = nseq;
		typename G::decode_state d;

		while(dnseq) {
			// Consume the next code unit.
			if(auto e = d(dnseq); e != charcode_error::null)
				return e;

			if(!d.progress()) {
				nseq.it = dnseq.it; // "Commit" consumed code units (as there was no decode error).
				if(!d.cpoint()) // Stop on null code points.
					return charcode_error::null;
			}
		}

		if(d.progress())
			return charcode_error::not_enough_units;
		return charcode_error::null;
	}
};

polymorphic_charcode *current_charcode() {
	static polymorphic_charcode_adapter<utf8_charcode> global_charcode;
	return &global_charcode;
}

} // namespace mlibc

