
#include <bits/ensure.h>
#include <mlibc/charcode.hpp>

namespace mlibc {

struct utf8_charcode : charcode {
	charcode_error get_length(const char *s, size_t n, size_t *units) override {
		// TODO: Error handling.
		__ensure(n);
		if(!(*s & 0x80)) {
			// ASCII-compatible.
			*units = 1;
			return charcode_error::null;
		}
		// If the highest two bits are 0b10, this is the second (or later) byte of a UTF-8 sequence.
		__ensure(!((*s & 0b1100'0000) == 0b1000'0000));
		if((*s & 0b1110'0000) == 0b1100'0000) {
			*units = 2;
		}else if((*s & 0b1111'0000) == 0b1110'0000) {
			*units = 3;
		}else if((*s & 0b1111'1000) == 0b1111'0000) {
			*units = 4;
		}else{
			// Bytes with the highest fives bits equal to 0b11111 do not occur in valid UTF-8.
			__ensure(!"Byte cannot occur in valid UTF-8 character");
		}
		return charcode_error::null;
	}
};

charcode *current_charcode() {
	static utf8_charcode global_charcode;
	return &global_charcode;
}

} // namespace mlibc

