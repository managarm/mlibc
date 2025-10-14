#ifndef MLIBC_LOCALE
#define MLIBC_LOCALE

#include <array>
#include <bits/nl_item.h>
#include <frg/span.hpp>
#include <locale.h>
#include <mlibc/allocator.hpp>
#include <variant>

namespace mlibc {

struct category_item {
	using value_types = std::variant<
		std::monostate,
		uint32_t,
		frg::span<const uint8_t>,
		frg::string_view,
		frg::basic_string_view<wchar_t>
	>;

	category_item() = default;

	category_item(uint32_t v) {
		value = v;
	}

	category_item(frg::span<const uint8_t> v) {
		value = v;
	}

	category_item(frg::span<const uint32_t> v) {
		value = frg::span{reinterpret_cast<const uint8_t *>(v.data()), v.size() * sizeof(uint32_t)};
	}

	category_item(frg::string_view v) {
		value = v;
	}

	category_item(frg::basic_string_view<wchar_t> v) {
		value = v;
	}

	frg::string_view asString() const {
		if (std::holds_alternative<std::monostate>(value))
			return {""};
		return std::get<frg::string_view>(value);
	}

	uint32_t asUint32() const {
		return std::get<uint32_t>(value);
	}

	frg::span<const uint8_t> asByteSpan() const {
		if (std::holds_alternative<std::monostate>(value))
			return {};
		return std::get<frg::span<const uint8_t>>(value);
	}

	frg::span<const uint16_t> asUint16Span() const {
		if (std::holds_alternative<std::monostate>(value))
			return {};
		auto span = std::get<frg::span<const uint8_t>>(value);
		return frg::span{reinterpret_cast<const uint16_t *>(span.data()), span.size() / sizeof(uint16_t)};
	}

	frg::span<const uint32_t> asUint32Span() const {
		if (std::holds_alternative<std::monostate>(value))
			return {};
		auto span = std::get<frg::span<const uint8_t>>(value);
		return frg::span{reinterpret_cast<const uint32_t *>(span.data()), span.size() / sizeof(uint32_t)};
	}

	value_types value;
};

template<int Category, size_t N>
struct nl_category {
	std::array<category_item, (N & 0xFFFF)> members;

	const category_item &get(nl_item item) {
		__ensure(item >> 16 == Category);
		return members[item & 0xFFFF];
	}

protected:
	void set(nl_item item, category_item value) {
		__ensure(item >> 16 == Category);
		members[item & 0xFFFF] = value;
	}
};

extern frg::array<uint8_t, 768> cLocaleCtypeClass;
extern frg::array<uint8_t, 1024> cLocaleCtypeClass32;
extern frg::array<uint32_t, 384> cLocaleCtypeToUpper;
extern frg::array<uint32_t, 384> cLocaleCtypeToLower;
extern frg::array<uint32_t, 10> cLocaleClassUpper;
extern frg::array<uint32_t, 10> cLocaleClassLower;
extern frg::array<uint32_t, 10> cLocaleClassAlpha;
extern frg::array<uint32_t, 9> cLocaleClassDigit;
extern frg::array<uint32_t, 11> cLocaleClassXdigit;
extern frg::array<uint32_t, 9> cLocaleClassSpace;
extern frg::array<uint32_t, 11> cLocaleClassPrint;
extern frg::array<uint32_t, 11> cLocaleClassGraph;
extern frg::array<uint32_t, 9> cLocaleClassBlank;
extern frg::array<uint32_t, 11> cLocaleClassCntrl;
extern frg::array<uint32_t, 11> cLocaleClassPunct;
extern frg::array<uint32_t, 11> cLocaleClassAlnum;
extern frg::array<uint32_t, 42> cLocaleMapToUpper;
extern frg::array<uint32_t, 42> cLocaleMapToLower;

struct nl_ctype : nl_category<LC_CTYPE, _NL_NUM_LC_CTYPE> {
	nl_ctype();

	frg::span<const uint32_t> class_table(size_t index) {
		if (index < _NL_NUM_LC_CTYPE)
			return get(index).asUint32Span();

		__ensure(index < offsets.size());
		size_t distanceToEnd = data.size() - offsets[index];
		size_t entrySize = (index + 1 < offsets.size()) ? offsets[index + 1] - offsets[index] : distanceToEnd;
		entrySize &= ~3;

		return frg::span(reinterpret_cast<const uint32_t *>(data.data() + offsets[index]), entrySize);
	}

	frg::span<const uint16_t> ctype_class() {
		return get(_NL_CTYPE_CLASS).asUint16Span();
	}

	frg::span<const uint32_t> map_tolower() {
		return get(_NL_CTYPE_TOLOWER).asUint32Span();
	}

	frg::span<const uint32_t> map_toupper() {
		return get(_NL_CTYPE_TOUPPER).asUint32Span();
	}

	frg::span<const uint8_t> ctype_class32() {
		return get(_NL_CTYPE_CLASS32).asByteSpan();
	}

	uint32_t class_offset() {
		return get(_NL_CTYPE_CLASS_OFFSET).asUint32();
	}

	uint32_t map_offset() {
		return get(_NL_CTYPE_MAP_OFFSET).asUint32();
	}

	frg::span<const uint8_t> data;
	frg::span<const uint32_t> offsets;
};

struct nl_numeric : nl_category<LC_NUMERIC, _NL_NUM_LC_NUMERIC> {
	nl_numeric();
};

struct nl_time : nl_category<LC_TIME, _NL_NUM_LC_TIME> {
	nl_time();
};

struct nl_collate : nl_category<LC_COLLATE, _NL_NUM_LC_COLLATE> {
	nl_collate();
};

struct nl_monetary : nl_category<LC_MONETARY, _NL_NUM_LC_MONETARY> {
	nl_monetary();
};

struct nl_messages : nl_category<LC_MESSAGES, _NL_NUM_LC_MESSAGES> {
	nl_messages();
};

struct nl_paper : nl_category<LC_PAPER, _NL_NUM_LC_PAPER> {
	nl_paper();
};

struct nl_name : nl_category<LC_NAME, _NL_NUM_LC_NAME> {
	nl_name();
};

struct nl_address : nl_category<LC_ADDRESS, _NL_NUM_LC_ADDRESS> {
	nl_address();
};

struct nl_telephone : nl_category<LC_TELEPHONE, _NL_NUM_LC_TELEPHONE> {
	nl_telephone();
};

struct nl_measurement : nl_category<LC_MEASUREMENT, _NL_NUM_LC_MEASUREMENT> {
	nl_measurement();
};

struct nl_identification : nl_category<LC_IDENTIFICATION, _NL_NUM_LC_IDENTIFICATION> {
	nl_identification();
};

char *nl_langinfo(nl_item item);

} // namespace mlibc

#endif // MLIBC_LOCALE
