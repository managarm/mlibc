
#include <bits/ensure.h>
#include <mlibc/charset.hpp>
#include <mlibc/debug.hpp>
#include <wchar.h>

namespace {

enum class class_bits : unsigned {
	upper,
	lower,
	alpha,
	digit,
	xdigit,
	space,
	print,
	graph,
	blank,
	cntrl,
	punct,
	alnum,
};

constexpr int ctype_class_bit(class_bits bit) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return 1 << std::to_underlying(bit);
#else
	return std::to_underlying(bit) < 8
		? ((1 << std::to_underlying(bit)) << 8)
		: ((1 << std::to_underlying(bit)) >> 8);
#endif
}

// sparse table lookup for ctype properties of a wide char
int wctype_table_lookup(frg::span<const uint32_t> table, uint32_t wc) {
	uint32_t shift1 = table[0];
	uint32_t index1 = wc >> shift1;
	uint32_t bound = table[1];
	if (index1 < bound) {
		uint32_t lookup1 = table[5 + index1];
		if (lookup1 != 0) {
			uint32_t shift2 = table[2];
			uint32_t mask2 = table[3];
			uint32_t index2 = (wc >> shift2) & mask2;
			uint32_t lookup2 = table[(lookup1 / sizeof(uint32_t)) + index2];
			if (lookup2 != 0) {
				uint32_t mask3 = table[4];
				uint32_t index3 = (wc >> 5) & mask3;
				uint32_t lookup3 = table[(lookup2 / sizeof(uint32_t)) + index3];

				return (lookup3 >> (wc & 0x1f)) & 1;
			}
		}
	}
	return 0;
}

// sparse translation table lookup of a wide char
uint32_t wctrans_table_lookup(frg::span<const uint32_t> table, uint32_t wc) {
	uint32_t shift1 = table[0];
	uint32_t index1 = wc >> shift1;
	uint32_t bound = table[1];
	if (index1 < bound) {
		uint32_t lookup1 = table[5 + index1];
		if (lookup1 != 0) {
			uint32_t shift2 = table[2];
			uint32_t mask2 = table[3];
			uint32_t index2 = (wc >> shift2) & mask2;
			uint32_t lookup2 = table[(lookup1 / sizeof(uint32_t)) + index2];
			if (lookup2 != 0) {
				uint32_t mask3 = table[4];
				uint32_t index3 = wc & mask3;
				int32_t lookup3 = table[(lookup2 / sizeof(uint32_t)) + index3];

				return wc + lookup3;
			}
		}
	}
	return wc;
}

int ctype_class_check(mlibc::codepoint c, class_bits b, mlibc::localeinfo *l) {
	if (c <= 0x7F)
		return l->ctype.ctype_class()[c + 128] & ctype_class_bit(b);

	auto index = l->ctype.class_offset() + std::to_underlying(b);
	auto entry = l->ctype.class_table(index);

	return wctype_table_lookup(entry, c);
}

} // namespace

namespace mlibc {

bool charset::is_ascii_superset() {
	// TODO: For locales that change the meaning of ASCII chars, this needs to be changed.
	return true;
}

bool charset::is_alpha(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::alpha, l);
}

bool charset::is_digit(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::digit, l);
}

bool charset::is_xdigit(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::xdigit, l);
}

bool charset::is_alnum(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::alnum, l);
}

bool charset::is_cntrl(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::cntrl, l);
}

bool charset::is_punct(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::punct, l);
}

bool charset::is_graph(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::graph, l);
}

bool charset::is_blank(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::blank, l);
}

bool charset::is_space(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::space, l);
}

bool charset::is_print(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::print, l);
}

bool charset::is_lower(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::lower, l);
}

bool charset::is_upper(codepoint c, mlibc::localeinfo *l) {
	return ctype_class_check(c, class_bits::upper, l);
}

codepoint charset::to_lower(codepoint c, mlibc::localeinfo *l) {
	auto index = l->ctype.map_offset() + 1;
	auto entry = l->ctype.class_table(index);
	return wctrans_table_lookup(entry, c);
}

codepoint charset::to_upper(codepoint c, mlibc::localeinfo *l) {
	auto index = l->ctype.map_offset();
	auto entry = l->ctype.class_table(index);
	return wctrans_table_lookup(entry, c);
}

wctype_t charset::wctype(frg::string_view name, mlibc::localeinfo *l) {
	size_t offset = 0;
	auto class_names = l->ctype.class_names();

	for (size_t i = 0; offset < class_names.size(); i++) {
		auto end = class_names.find_first('\0', offset);

		if (end == size_t(-1) || end == offset)
			break;

		if (name == class_names.sub_string(offset, end - offset))
			return l->ctype.class_offset() + i;

		offset = end + 1;
	}

	return 0;
}

wint_t charset::towctrans(wint_t wc, wctrans_t index, mlibc::localeinfo *l) {
	auto entry = l->ctype.class_table(index);
	return wctrans_table_lookup(entry, wc);
}

bool charset::iswctype(wint_t wc, wctype_t t, mlibc::localeinfo *l) {
	if (t == 0 || wc == static_cast<wint_t>(WEOF))
		return 0;

	auto table = l->ctype.get(__NL_ITEM(LC_CTYPE, t)).asUint32Span();

	return wctype_table_lookup(table, wc);
}

charset *current_charset() {
	static charset global_charset;
	return &global_charset;
}

} // namespace mlibc

