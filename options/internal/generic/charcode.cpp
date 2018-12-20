
#include <bits/ensure.h>
#include <frg/string.hpp>
#include <mlibc/charcode.hpp>

namespace mlibc {

struct utf8_charcode {
	struct decode_state {
		decode_state()
		: _shift{0}, _cpoint{0} { }

		auto shift() { return _shift; }
		auto cpoint() { return _cpoint; }

		charcode_error operator() (code_seq<const char> &seq) {
			if(!_shift) {
				if(!(*seq.it & 0b1000'0000)) {
					// ASCII-compatible.
					_cpoint = *seq.it;
				}else if((*seq.it & 0b1110'0000) == 0b1100'0000) {
					_cpoint = *seq.it & 0b1'1111;
					_shift = -1;
				}else if((*seq.it & 0b1111'0000) == 0b1110'0000) {
					_cpoint = *seq.it & 0b1111;
					_shift = -2;
				}else if((*seq.it & 0b1111'1000) == 0b1111'0000) {
					_cpoint = *seq.it & 0b111;
					_shift = -3;
				}else{
					// If the highest two bits are 0b10, this is the second (or later) unit.
					__ensure(!((*seq.it & 0b1100'0000) == 0b1000'0000));
					// Units with highest five bits = 0b11111 do not occur in valid UTF-8.
					__ensure(!"Unit cannot occur in valid UTF-8 character");
				}
			}else{
				__ensure((*seq.it & 0b1100'0000) == 0b1000'0000);
				_cpoint = (_cpoint << 6) | ((*seq.it) & 0x3F);
				++_shift;
			}
			++seq.it;
			return charcode_error::null;
		}

	private:
		int _shift;
		codepoint _cpoint;
	};

	static charcode_error get_length(const char *s, size_t n, size_t *units) {
		// TODO: Error handling.
		__ensure(n);
		if(!(*s & 0b1000'0000)) {
			// ASCII-compatible.
			*units = 1;
		}else if((*s & 0b1110'0000) == 0b1100'0000) {
			*units = 2;
		}else if((*s & 0b1111'0000) == 0b1110'0000) {
			*units = 3;
		}else if((*s & 0b1111'1000) == 0b1111'0000) {
			*units = 4;
		}else{
			// If the highest two bits are 0b10, this is the second (or later) unit.
			__ensure(!((*s & 0b1100'0000) == 0b1000'0000));
			// Units with highest five bits = 0b11111 do not occur in valid UTF-8.
			__ensure(!"Unit cannot occur in valid UTF-8 character");
		}
		return charcode_error::null;
	}
};

template<typename G>
struct polymorphic_charcode_adapter : polymorphic_charcode {
	charcode_error get_length(const char *s, size_t n, size_t *units) override {
		return G::get_length(s, n, units);
	}

	charcode_error wdecode(code_seq<const char> &cus, code_seq<wchar_t> &cps,
			__mlibc_mbstate &st) override {
		__ensure(!st.__shift);

		typename G::decode_state d;

		while(cus && cps) {
			// Consume the next code unit.
			if(auto e = d(cus); e != charcode_error::null)
				return e;

			// Produce a new code point.
			if(!d.shift()) {
				if(!d.cpoint()) // Stop on null characters.
					return charcode_error::null;
				*cps.it = d.cpoint();
				++cps.it;
			}
		}

		if(d.shift())
			return charcode_error::not_enough_units;
		return charcode_error::null;
	}

	charcode_error wdecode_length(code_seq<const char> &cus,
			__mlibc_mbstate &st) override {
		__ensure(!st.__shift);

		typename G::decode_state d;

		while(cus) {
			// Consume the next code unit.
			if(auto e = d(cus); e != charcode_error::null)
				return e;

			// Stop on null characters.
			if(!d.shift() && !d.cpoint())
				return charcode_error::null;
		}

		if(d.shift())
			return charcode_error::not_enough_units;
		return charcode_error::null;
	}
};

polymorphic_charcode *current_charcode() {
	static polymorphic_charcode_adapter<utf8_charcode> global_charcode;
	return &global_charcode;
}

} // namespace mlibc

