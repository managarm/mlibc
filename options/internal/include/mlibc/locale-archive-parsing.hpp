#pragma once

#include <array>
#include <frg/span.hpp>
#include <frg/string.hpp>
#include <limits.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <stddef.h>
#include <stdint.h>

constexpr bool debugLocale = true;

#pragma mark - glibc definitions of LC_* macros
constexpr int GLIBC_LC_CTYPE = 0;
constexpr int GLIBC_LC_NUMERIC = 1;
constexpr int GLIBC_LC_TIME = 2;
constexpr int GLIBC_LC_COLLATE = 3;
constexpr int GLIBC_LC_MONETARY = 4;
constexpr int GLIBC_LC_MESSAGES = 5;
constexpr int GLIBC_LC_ALL = 6;
constexpr int GLIBC_LC_PAPER = 7;
constexpr int GLIBC_LC_NAME = 8;
constexpr int GLIBC_LC_ADDRESS = 9;
constexpr int GLIBC_LC_TELEPHONE = 10;
constexpr int GLIBC_LC_MEASUREMENT = 11;
constexpr int GLIBC_LC_IDENTIFICATION = 12;
constexpr int GLIBC_LC_COUNT = 13;

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
	} record[GLIBC_LC_COUNT];
};

struct SumHashEntry {
	/* MD5 sum. */
	char sum[16];
	/* Offset of the file in the archive. */
	uint32_t file_offset;
};

} // namespace LocaleArchive

#pragma mark - hash and magic calculations
template<typename T>
constexpr T nameHashVal(const void *key, size_t keylen) {
	size_t cnt = 0;
	T hval = keylen;

	while (cnt < keylen) {
		hval = (hval << 9) | (hval >> (sizeof hval * CHAR_BIT - 9));
		hval += (T) ((const unsigned char *) key)[cnt++];
	}
	return hval != 0 ? hval : ~((T) 0);
}

constexpr uint32_t categoryMagic(int category) {
	if (category == GLIBC_LC_COLLATE)
		return 0x20051014 ^ category;
	else if (category == GLIBC_LC_CTYPE)
		return 0x20090720 ^ category;
	else
		return 0x20031115 ^ category;
}

#pragma mark - locale definition field parsers
frg::string_view parse_string(frg::span<uint8_t> data, frg::span<const uint32_t>) {
	auto ptr = reinterpret_cast<const char *>(data.data());
	auto len = strnlen(ptr, data.size());
	return frg::string_view{ptr, len};
}

// parse a string array, returning an array of N string_views
template<size_t N>
std::array<frg::string_view, N> parse_string_array(frg::span<uint8_t> data, frg::span<const uint32_t> offs) {
	std::array<frg::string_view, N> result{};

	([&]<size_t... Is>(std::index_sequence<Is...>) {
		(([&] {
			// each string array member gets its own offset
			const char *start = reinterpret_cast<const char*>(data.data() + offs[Is]);
			// AFAICT the offsets are strictly increasing
			// then determine the length of a data field by looking at the next offset/end of buffer
			size_t size = [&]() {
				if constexpr (Is + 1 < N)
					return size_t(offs[Is + 1] - offs[Is]);
				else
					return data.size() - size_t(offs[Is]);
			}();

			// Find null terminator
			const char *nul = static_cast<const char *>(memchr(start, '\0', size));

			if (nul)
				result[Is] = frg::string_view{start, static_cast<size_t>(nul - start)};
		}()), ...);
	})(std::make_index_sequence<N>{});

	return result;
}

// parse generic types, typically `uint*_t`
template<typename T>
requires std::is_trivially_copyable_v<T>
T parse(frg::span<uint8_t> data, frg::span<const uint32_t>) {
	T v{};
	memcpy(&v, data.data(), frg::min(sizeof(v), data.size()));
	return v;
}

#pragma mark - parser execution internals
template<typename T>
struct std_array_size {
	static constexpr std::size_t value = 1; // default for non-array
};

template<typename T, std::size_t N>
struct std_array_size<std::array<T, N>> {
	static constexpr std::size_t value = N;
};

template<typename T>
constexpr std::size_t std_array_size_v = std_array_size<T>::value;

template<typename T>
struct is_std_array : std::false_type {};

template<typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template<typename T>
constexpr bool is_std_array_v = is_std_array<T>::value;

template<typename T>
concept is_std_optional = requires(T t) {
	typename T::value_type;
	requires std::same_as<T, std::optional<typename T::value_type>>;
};

template<typename T>
using parser_result_t = std::invoke_result_t<T, frg::span<uint8_t>, frg::span<const uint32_t>>;

// given a tuple of parsers, return a tuple of up to how many offsets each parser consumes
template<typename Tuple>
constexpr auto offset_indices(const Tuple &funcs) {
	constexpr std::size_t N = std::tuple_size_v<Tuple>;

	return ([&]<size_t... I>(std::index_sequence<I...>) {
		return std::make_tuple((std_array_size_v<parser_result_t<decltype(std::get<I>(funcs))>>)...);
	}(std::make_index_sequence<N>()));
}

// given a tuple, return a prefix-summed tuple of same size
template <typename... Sizes>
constexpr auto prefix_sum_tuple(const std::tuple<Sizes...>& input) {
	return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
		std::size_t running_sum = 0;
		return std::make_tuple(
			([&]() constexpr {
				std::size_t val = std::get<Is>(input);
				std::size_t out = running_sum;
				running_sum += val;
				return out;
			}())...
		);
	}(std::index_sequence_for<Sizes...>{});
}

template<typename T>
auto apply_parsers(const T &parser, frg::span<uint8_t> base, frg::span<const uint32_t> offsets) {
	constexpr size_t Parsers = std::tuple_size_v<decltype(parser.parsers)>;
	constexpr auto offsetIncrements = offset_indices(parser.parsers);
	constexpr auto offsetIndices = prefix_sum_tuple(offsetIncrements);

	if (debugLocale)
		mlibc::infoLogger() << "mlibc: running locale category parser for "
			<< parser.name << frg::endlog;

	return [&]<size_t... I>(std::index_sequence<I...>) {
		return std::make_tuple(
			(std::get<I>(offsetIndices) < offsets.size() ?
				[&] {
					constexpr size_t FirstOffsetIndex = std::get<I>(offsetIndices);
					constexpr size_t OffsetCount = std::get<I>(offsetIncrements);

					auto dataSpan = base.subspan(offsets[FirstOffsetIndex]);

					// for non-array-returning parsers, contrain the data span to their return type's size
					if constexpr (!is_std_array_v<parser_result_t<decltype(std::get<I>(parser.parsers))>>)
						dataSpan = dataSpan.subspan(0, frg::min(sizeof(T), dataSpan.size()));

					// build up an array of offset values to pass to the parser,
					// shifting them so they are relative to the data span
					auto normalizedOffsets{
						[&]<size_t... J>(std::index_sequence<J...>) {
							return frg::array<const uint32_t, OffsetCount>{
								(offsets[J + FirstOffsetIndex] - offsets[FirstOffsetIndex])...
							};
						}(std::make_index_sequence<OffsetCount>{})
					};

					// run the parser with as-constrained-as-possible subspans
					return std::optional{std::get<I>(parser.parsers)(
						dataSpan,
						frg::span{normalizedOffsets.data(), OffsetCount})
					};
				}() : std::nullopt
			)...
		);
	}(std::make_index_sequence<Parsers>{});
}

template <typename... Ts, typename... Opts>
void assign_if_present(std::tuple<Ts&...> tie, const std::tuple<Opts...>& in)
requires (
	sizeof...(Ts) == sizeof...(Opts) &&
	(... && is_std_optional<Opts>)
)
{
	std::apply([&](Ts&... vars) {
		std::apply([&](const Opts&... opts) {
			([]<typename T>(auto &var, const std::optional<T> &opt) {
				if (opt) {
					if constexpr (std::is_same_v<std::remove_cvref_t<T>, frg::string_view>) {
						var = frg::string<MemoryAllocator>{opt->data(), opt->size(), getAllocator()};
					} else {
						var = *opt;
					}
				}
			}(vars, opts), ...);
		}, in);
	}, tie);
}

template<typename T, typename ...Opts>
void parse_category(std::tuple<Opts&...> opts, const T &parser, frg::span<uint8_t> base, frg::span<const uint32_t> offsets) {
	assign_if_present(opts, apply_parsers(parser, base, offsets));
}

#pragma mark - locale category parser definitions
template<typename... Parsers>
struct category {
	const char *name;
	int glibc_val;
	std::tuple<Parsers...> parsers;
};

auto numeric_parser = category{
	.name = "LC_NUMERIC",
	.glibc_val = GLIBC_LC_NUMERIC,
	.parsers = std::make_tuple(
		parse_string, // decimal_point
		parse_string, // thousands_sep
		parse<uint8_t>,   // grouping
		parse<wchar_t>,  // numeric_decimal_point_wc
		parse<wchar_t>,  // numeric_thousands_sep_wc,
		parse_string  // codeset
	)
};

auto messages_parser = category{
	.name = "LC_MESSAGES",
	.glibc_val = GLIBC_LC_MESSAGES,
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
	.glibc_val = GLIBC_LC_MONETARY,
	.parsers = std::make_tuple(
		parse_string, // int_curr_symbol
		parse_string, // currency_symbol
		parse_string, // mon_decimal_point
		parse_string, // mon_thousands_sep
		parse<uint8_t>,   // mon_grouping
		parse_string, // positive_sign
		parse_string, // negative_sign
		parse<uint8_t>,   // int_frac_digits
		parse<uint8_t>,   // frac_digits
		parse<uint8_t>,   // p_cs_precedes
		parse<uint16_t>, // p_sep_by_space
		parse<uint8_t>,   // n_cs_precedes
		parse<uint16_t>, // n_sep_by_space
		parse<uint32_t>, // p_sign_posn
		parse<uint32_t>, // n_sign_posn
		parse_string  // crncystr
	)
};

auto time_parser = category{
	.name = "LC_TIME",
	.glibc_val = GLIBC_LC_TIME,
	.parsers = std::make_tuple(
		parse_string_array<7>, // abday
		parse_string_array<7>, // day
		parse_string_array<12>, // abmon
		parse_string_array<12>, // mon
		parse_string_array<2>, // am_pm
		parse_string, // d_t_fmt
		parse_string, // d_fmt
		parse_string, // t_fmt
		parse_string, // t_fmt_ampm
		parse_string, // era
		parse_string, // era_year
		parse_string, // era_d_fmt
		parse_string, // alt_digits
		parse_string, // era_d_t_fmt
		parse_string  // era_t_fmt
	)
};
