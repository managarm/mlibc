#ifndef MLIBC_CHARSET_HPP
#define MLIBC_CHARSET_HPP

#include <mlibc/charcode.hpp>
#include <mlibc/locale.hpp>
#include <wctype.h>

namespace mlibc {

// Represents the charset of a certain locale. We define the charset as
// a set of characters, together with their properties and conversion rules
// *but not* their encoding (e.g. to UTF-8 or UTF-16).
struct charset {
	// Returns true iif the meaning of the first 0x7F characters matches ASCII.
	bool is_ascii_superset();

	bool is_alpha(codepoint c, mlibc::localeinfo *l);
	bool is_digit(codepoint c, mlibc::localeinfo *l);
	bool is_xdigit(codepoint c, mlibc::localeinfo *l);
	bool is_alnum(codepoint c, mlibc::localeinfo *l);
	bool is_cntrl(codepoint c, mlibc::localeinfo *l);
	bool is_punct(codepoint c, mlibc::localeinfo *l);
	bool is_graph(codepoint c, mlibc::localeinfo *l);
	bool is_blank(codepoint c, mlibc::localeinfo *l);
	bool is_space(codepoint c, mlibc::localeinfo *l);
	bool is_print(codepoint c, mlibc::localeinfo *l);

	bool is_lower(codepoint c, mlibc::localeinfo *l);
	bool is_upper(codepoint c, mlibc::localeinfo *l);
	codepoint to_lower(codepoint c, mlibc::localeinfo *l);
	codepoint to_upper(codepoint c, mlibc::localeinfo *l);

	wint_t towctrans(wint_t wc, wctrans_t index, mlibc::localeinfo *l);

	wctype_t wctype(frg::string_view name, mlibc::localeinfo *l);
	bool iswctype(wint_t wc, wctype_t, mlibc::localeinfo *l);
};

charset *current_charset();

// The property if a character is a control character is locale-independent.
inline bool generic_is_control(codepoint c) {
	return (c <= 0x1F) || (c == 0x7F) || (c >= 0x80 && c <= 0x9F);
}

} // namespace mlibc

#endif // MLIBC_CHARSET_HPP
