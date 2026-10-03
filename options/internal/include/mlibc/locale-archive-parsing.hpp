#pragma once

#include <array>
#include <frg/span.hpp>
#include <frg/string.hpp>
#include <limits.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/locale.hpp>
#include <mlibc/strings.hpp>
#include <stddef.h>
#include <stdint.h>

#pragma mark - locale-archive format description

namespace LocaleArchive {

constexpr uint32_t HEADER_MAGIC = 0xde020109;

struct Header {
	uint32_t magic;
	/* Serial number. */
	uint32_t serial;
	/* Name hash table. */
	uint32_t namehash_offset;
	uint32_t namehash_used;
	uint32_t namehash_size;
	/* String table. */
	uint32_t string_offset;
	uint32_t string_used;
	uint32_t string_size;
	/* Table with locale records. */
	uint32_t locrectab_offset;
	uint32_t locrectab_used;
	uint32_t locrectab_size;
	/* MD5 sum hash table. */
	uint32_t sumhash_offset;
	uint32_t sumhash_used;
	uint32_t sumhash_size;
};

struct NameHashEntry {
	/* Hash value of the name. */
	uint32_t hashval;
	/* Offset of the name in the string table. */
	uint32_t name_offset;
	/* Offset of the LocaleRecord. */
	uint32_t locrec_offset;
};

struct LocaleRecord {
	/* # of NameHashEntry records that point here */
	uint32_t refs;
	struct {
		uint32_t offset;
		uint32_t len;
	} record[LC_IDENTIFICATION + 1];
};

struct SumHashEntry {
	/* MD5 sum. */
	char sum[16];
	/* Offset of the file in the archive. */
	uint32_t file_offset;
};

} // namespace LocaleArchive

#pragma mark - hash and magic calculations
template <typename T>
constexpr T nameHashVal(const frg::span<char> data) {
	T hval = data.size_bytes();

    for (auto val : data) {
		hval = (hval << 9) | (hval >> (sizeof(hval) * CHAR_BIT - 9));
        hval += static_cast<unsigned char>(val);
	}

	return hval != 0 ? hval : ~(T{0});
}

constexpr uint32_t categoryMagic(int category) {
	if (category == LC_COLLATE)
		return 0x20051014 ^ category;
	else if (category == LC_CTYPE)
		return 0x20090720 ^ category;
	else
		return 0x20031115 ^ category;
}

#pragma mark - locale definition field parsers
frg::string_view parse_string(frg::span<const uint8_t> data) {
	auto ptr = reinterpret_cast<const char *>(data.data());
	auto len = mlibc::strnlen(ptr, data.size());
	return frg::string_view{ptr, frg::min(len + 1, data.size())};
}

frg::basic_string_view<wchar_t> parse_wstring(frg::span<const uint8_t> data) {
    __ensure(reinterpret_cast<uintptr_t>(data.data()) % alignof(wchar_t) == 0);
	auto ptr = reinterpret_cast<const wchar_t *>(data.data());
	auto len = frg::generic_strnlen(ptr, data.size() / sizeof(wchar_t));
	return frg::basic_string_view<wchar_t>{ptr, frg::min(len + 1, data.size() / sizeof(wchar_t))};
}

template <size_t Min, size_t Max>
frg::string_view parse_stringlist(frg::span<const uint8_t> data) {
	size_t termIndex = 0;
	size_t i = 0;
	frg::string_view area{reinterpret_cast<const char *>(data.data()), data.size()};

	for (; i < Min; i++) {
		auto next = area.find_first('\0', termIndex);
		if (next == size_t(-1))
			return {};
		termIndex = next + 1;
		if (termIndex >= area.size() || area[termIndex] == '\0')
			return {};
	}

	for (; i < Max; i++) {
		if (termIndex >= area.size() || area[termIndex] == '\0')
			break;
		auto next = area.find_first('\0', termIndex);
		if (next == size_t(-1))
			break;
		termIndex = next + 1;
	}

	if (termIndex == area.size())
		return area;
	else if (termIndex < area.size()) {
		if (area[termIndex] != '\0')
			return {};
		return area.sub_string(0, termIndex + 1);
	}

	return {};
}

frg::span<const uint8_t> parse_bytearray(frg::span<const uint8_t> data) {
	auto ptr = reinterpret_cast<const char *>(data.data());
	auto len = mlibc::strnlen(ptr, data.size());
	return frg::span{data.data(), len};
}

frg::span<const uint8_t> parse_raw_bytearray(frg::span<const uint8_t> data) {
	return data;
}

template <size_t N>
frg::span<const uint32_t> parse_uint32array(frg::span<const uint8_t> data) {
	auto ptr = reinterpret_cast<const uint32_t *>(data.data());
	auto len = frg::min(data.size() & ~(sizeof(uint32_t) - 1), N * sizeof(uint32_t));
	return frg::span{ptr, len / sizeof(uint32_t)};
}

void parse_ignore(frg::span<const uint8_t>) { return; }

char parse_int_elem(frg::span<const uint8_t> data) {
	if (data.empty())
		return 0;
	return data[0] == 255 ? CHAR_MAX : data[0];
}

// parse generic types, typically `uint*_t`
template <typename T>
    requires std::is_trivially_copyable_v<T>
T parse(frg::span<const uint8_t> data) {
	T v{};
	if (data.size() >= sizeof(T))
		memcpy(&v, data.data(), sizeof(v));
	return v;
}

#pragma mark - parser execution internals
template <typename T>
struct is_std_optional : std::false_type {};

template <typename T>
struct is_std_optional<std::optional<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_optional_v = is_std_optional<T>::value;

template <typename T>
using parser_result_t = std::invoke_result_t<T, frg::span<const uint8_t>>;

template <typename T>
auto
apply_parsers(const T &parser, frg::span<const uint8_t> base, frg::span<const uint32_t> offsets) {
	constexpr size_t Parsers = std::tuple_size_v<decltype(parser.parsers)>;

	return [&]<size_t... I>(std::index_sequence<I...>) {
		return std::make_tuple((I < offsets.size() ? [&] {
			if constexpr (std::is_void_v<parser_result_t<decltype(std::get<I>(parser.parsers))>>)
				return std::nullopt;
			else {
				if (offsets[I] > base.size())
					return std::optional<parser_result_t<decltype(std::get<I>(parser.parsers))>>{std::nullopt};

				size_t item_size = (I + 1 == offsets.size()) ? base.size() - offsets[I] : (offsets[I + 1] - offsets[I]);
				if (offsets[I] + item_size > base.size())
					return std::optional<parser_result_t<decltype(std::get<I>(parser.parsers))>>{std::nullopt};

				return std::optional{std::get<I>(parser.parsers)(
				    base.subspan(offsets[I], item_size)
				)};
			}
		}()
		                                           : std::nullopt)...);
	}(std::make_index_sequence<Parsers>{});
}

template <typename TupleTie, typename TupleIn, size_t... Is>
void assign_if_present(TupleTie &tie, const TupleIn &in, std::index_sequence<Is...>) {
	(
	    []<typename T>(auto &var, const T &opt) {
		    if constexpr (is_std_optional_v<T>) {
			    if (opt) {
				    var = *opt;

				    if constexpr (std::is_same_v<
				                      std::remove_cvref_t<decltype(*opt)>,
				                      frg::string_view>)
					    __ensure(var.asString().ends_with("\0"));
			    }
		    }
	    }(std::get<Is>(tie), std::get<Is>(in)),
	    ...
	);
}

template <typename T, size_t Opts>
void parse_category_array(
    std::array<mlibc::category_item, Opts> &opts,
    const T &parser,
    frg::span<const uint8_t> base,
    frg::span<const uint32_t> offsets
) {
	auto res = apply_parsers(parser, base, offsets);

	constexpr size_t tuple_size = std::tuple_size_v<decltype(res)>;
	constexpr size_t assign_count = std::min(tuple_size, Opts);

	assign_if_present(opts, res, std::make_index_sequence<assign_count>{});
}

#pragma mark - locale category parser definitions
template <typename... Parsers>
struct category {
	const char *name;
	int glibc_val;
	std::tuple<Parsers...> parsers;
};

auto numeric_parser = category{
    .name = "LC_NUMERIC",
    .glibc_val = LC_NUMERIC,
    .parsers = std::make_tuple(
        parse_string,    // decimal_point
        parse_string,    // thousands_sep
        parse_bytearray, // grouping
        parse<wchar_t>,  // numeric_decimal_point_wc
        parse<wchar_t>,  // numeric_thousands_sep_wc,
        parse_string     // codeset
    )
};

auto messages_parser = category{
    .name = "LC_MESSAGES",
    .glibc_val = LC_MESSAGES,
    .parsers = std::make_tuple(
        parse_string, // yesexpr
        parse_string, // noexpr
        parse_string, // yesstr
        parse_string, // nostr
        parse_string  // codeset
    )
};

auto monetary_parser = category{
    .name = "LC_MONETARY",
    .glibc_val = LC_MONETARY,
    .parsers = std::make_tuple(
        parse_string,    // int_curr_symbol
        parse_string,    // currency_symbol
        parse_string,    // mon_decimal_point
        parse_string,    // mon_thousands_sep
        parse_bytearray, // mon_grouping
        parse_string,    // positive_sign
        parse_string,    // negative_sign
        parse_int_elem,  // int_frac_digits
        parse_int_elem,  // frac_digits
        parse_int_elem,  // p_cs_precedes
        parse_int_elem,  // p_sep_by_space
        parse_int_elem,  // n_cs_precedes
        parse_int_elem,  // n_sep_by_space
        parse_int_elem,  // p_sign_posn
        parse_int_elem,  // n_sign_posn
        parse_string,    // crncystr
        parse_int_elem,  // int_p_cs_precedes
        parse_int_elem,  // int_n_cs_precedes
        parse_int_elem,  // int_p_sep_by_space
        parse_int_elem,  // int_n_sep_by_space
        parse_int_elem,  // int_p_sign_posn
        parse_int_elem   // int_n_sign_posn
    )
};

auto time_parser = category{
    .name = "LC_TIME",
    .glibc_val = LC_TIME,
    .parsers = std::make_tuple(
        // abday1..7
        parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string,
        // day1..7
        parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string,
        // abmon1..12
        parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string,
        // mon1..12
        parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string,
        parse_string, parse_string, // am, pm
        parse_string, // d_t_fmt
        parse_string, // d_fmt
        parse_string, // t_fmt
        parse_string, // t_fmt_ampm
        parse_stringlist<0, 100>, // era
        parse_string, // era_year
        parse_string, // era_d_fmt
        parse_stringlist<0, 100>, // alt_digits
        parse_string, // era_d_t_fmt
        parse_string, // era_t_fmt
        parse<uint32_t>, // era_num_entries
        parse_raw_bytearray, // era_entries
        // wabday1..7
        parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring,
        // wday1..7
        parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring,
        // wabmon1..12
        parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring,
        // wmon1..12
        parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring,
        parse_wstring, parse_wstring, // wam_str, wpm_str
        // wd_t_fmt, wd_fmt, wt_fmt, wt_fmt_ampm
        parse_wstring, parse_wstring, parse_wstring, parse_wstring,
        // wera_year, wera_d_fmt, walt_digits, wera_d_t_fmt, wera_t_fmt
        parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring,
        parse_raw_bytearray, // time_week_ndays
        parse<uint32_t>, // time_week_1stday
        parse_raw_bytearray, // time_week_1stweek
        parse_raw_bytearray, // time_first_weekday
        parse_raw_bytearray, // time_first_workday
        parse_raw_bytearray, // time_cal_direction
        parse_string, // timezone
        parse_string, // date_fmt
        parse_wstring, // w_date_fmt
        parse_string, // time_codeset
        // altmon1..12
        parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string,
        // waltmon1..12
        parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring,
        // abaltmon1..12
        parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string,
        // wabaltmon1..12
        parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring
    )
};

auto collate_parser = category{
    .name = "LC_COLLATE",
    .glibc_val = LC_COLLATE,
    .parsers = std::make_tuple(
        parse<uint32_t>,     // nrules
        parse_raw_bytearray, // rulesets
        parse_raw_bytearray, // tablemb
        parse_raw_bytearray, // weightmb
        parse_raw_bytearray, // extramb
        parse_raw_bytearray, // indirectmb
        parse_ignore,        // gap
        parse_ignore,        // gap
        parse_ignore,        // gap
        parse_raw_bytearray, // tablewc
        parse_raw_bytearray, // weightwc
        parse_raw_bytearray, // extrawc
        parse_raw_bytearray, // indirectwc
        parse<uint32_t>,     // symb-hash-sizemb
        parse_string,        // symb-tablemb
        parse_string,        // symb-extramb
        parse_string,        // collseqmb
        parse_string,        // collseqwc
        parse_string         // codeset
    )
};

auto ctype_parser = category{
    .name = "LC_CTYPE",
    .glibc_val = LC_CTYPE,
    .parsers = std::make_tuple(
        parse_bytearray,              // class
        parse_uint32array<256 + 128>, // toupper
        parse_ignore,                 // gap1
        parse_uint32array<256 + 128>, // tolower
        parse_ignore,                 // gap2
        parse_bytearray,              // class32
        parse_ignore,                 // gap3
        parse_ignore,                 // gap4
        parse_ignore,                 // gap5
        parse_ignore,                 // gap6
        parse_stringlist<10, 32>,     // class_names
        parse_stringlist<2, 32>,      // map_names
        parse_raw_bytearray,          // width
        parse<uint32_t>,              // mb_cur_max
        parse_string,                 // codeset_name
        parse_raw_bytearray,          // toupper32
        parse_raw_bytearray,          // tolower32
        parse<uint32_t>,              // class_offset
        parse<uint32_t>,              // map_offset
        parse<uint32_t>,              // indigits_mb_len
        // indigits0_mb - indigits9_mb
        parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string,
        parse<uint32_t>,              // indigits_wc-len
        // indigits0_wc - indigits9_wc
        parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring, parse_wstring,
        // outdigits0_mb - outdigits9_mb
        parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string, parse_string,
        // outdigit0_wc - // outdigit9_wc
        parse<uint32_t>, parse<uint32_t>, parse<uint32_t>, parse<uint32_t>, parse<uint32_t>, parse<uint32_t>, parse<uint32_t>, parse<uint32_t>, parse<uint32_t>, parse<uint32_t>,
        parse<uint32_t>,              // ctype-translit-tab-size
        parse_raw_bytearray,          // translit-from-idx
        parse_raw_bytearray,          // translit-from-tbl
        parse_raw_bytearray,          // translit-to-idx
        parse_raw_bytearray,          // translit-to-tbl
        parse<uint32_t>,              // ctype-translit-default-missing-len
        parse_wstring,                // translit-default-missing
        parse<uint32_t>,              // ctype-translit-ignore-len
        parse<uint32_t>,              // ctype-translit-ignore
        parse<uint32_t>,              // map-to-nonascii
        parse<uint32_t>,              // nonascii-case
        // entries for class and map name data
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray,
        parse_bytearray
    )
};

auto paper_parser = category{
    .name = "LC_PAPER",
    .glibc_val = LC_PAPER,
    .parsers = std::make_tuple(
        parse<uint32_t>, // height
        parse<uint32_t>, // width
        parse_string     // codeset
    )
};

auto name_parser = category{
    .name = "LC_NAME",
    .glibc_val = LC_NAME,
    .parsers = std::make_tuple(
        parse_string, // fmt
        parse_string, // gen
        parse_string, // mr
        parse_string, // mrs
        parse_string, // miss
        parse_string, // ms
        parse_string  // codeset
    )
};

auto address_parser = category{
    .name = "LC_ADDRESS",
    .glibc_val = LC_ADDRESS,
    .parsers = std::make_tuple(
        parse_string,    // postal_fmt
        parse_string,    // country_name
        parse_string,    // country_post
        parse_string,    // country_ab2
        parse_string,    // country_ab3
        parse_string,    // country_car
        parse<uint32_t>, // country_num
        parse_string,    // country_isbn
        parse_string,    // lang_name
        parse_string,    // lang_ab
        parse_string,    // lang_term
        parse_string,    // lang_lib
        parse_string     // codeset
    )
};

auto telephone_parser = category{
    .name = "LC_TELEPHONE",
    .glibc_val = LC_TELEPHONE,
    .parsers = std::make_tuple(
        parse_string, // tel_int_fmt
        parse_string, // tel_dom_fmt
        parse_string, // int_select
        parse_string, // int_prefix
        parse_string  // codeset
    )
};

auto measurement_parser = category{
    .name = "LC_MEASUREMENT",
    .glibc_val = LC_MEASUREMENT,
    .parsers = std::make_tuple(
        parse<uint8_t>, // measurement
        parse_string    // codeset
    )
};

auto identification_parser = category{
    .name = "LC_IDENTIFICATION",
    .glibc_val = LC_IDENTIFICATION,
    .parsers = std::make_tuple(
        parse_string, // title
        parse_string, // source
        parse_string, // address
        parse_string, // contact
        parse_string, // email
        parse_string, // tel
        parse_string, // fax
        parse_string, // language
        parse_string, // territory
        parse_string, // audience
        parse_string, // application
        parse_string, // abbreviation
        parse_string, // revision
        parse_string, // date
        parse_string, // category
        parse_string  // codeset
    )
};
