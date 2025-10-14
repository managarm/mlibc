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

constexpr bool debugLocale = false;

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
constexpr T nameHashVal(const void *key, size_t keylen) {
	size_t cnt = 0;
	T hval = keylen;

	while (cnt < keylen) {
		hval = (hval << 9) | (hval >> (sizeof hval * CHAR_BIT - 9));
		hval += (T)((const unsigned char *)key)[cnt++];
	}
	return hval != 0 ? hval : ~((T)0);
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

template <size_t Min, size_t Max>
frg::string_view parse_stringlist(frg::span<const uint8_t> data) {
	size_t termIndex = 0;
	size_t i = 0;
	frg::string_view area{reinterpret_cast<const char *>(data.data()), data.size()};

	for (; i < Min; i++) {
		termIndex = area.find_first('\0', termIndex) + 1;
		__ensure(area[termIndex] != '\0');
	}

	for (; i < Max; i++) {
		if (area[termIndex] == '\0')
			break;
		termIndex = area.find_first('\0', termIndex) + 1;
	}

	__ensure(area[termIndex] == '\0');

	return area.sub_string(0, termIndex + 1);
}

frg::span<const uint8_t> parse_bytearray(frg::span<const uint8_t> data) {
	auto ptr = reinterpret_cast<const char *>(data.data());
	auto len = mlibc::strnlen(ptr, data.size());
	return frg::span{data.data(), len};
}

template <size_t N>
frg::span<const uint32_t> parse_uint32array(frg::span<const uint8_t> data) {
	auto ptr = reinterpret_cast<const uint32_t *>(data.data());
	auto len = frg::min(data.size() & ~(sizeof(uint32_t) - 1), N * sizeof(uint32_t));
	return frg::span{ptr, len};
}

void parse_ignore(frg::span<const uint8_t>) { return; }

char parse_int_elem(frg::span<const uint8_t> data) { return data[0] == 255 ? CHAR_MAX : data[0]; }

// parse generic types, typically `uint*_t`
template <typename T>
    requires std::is_trivially_copyable_v<T>
T parse(frg::span<const uint8_t> data) {
	T v{};
	memcpy(&v, data.data(), frg::min(sizeof(v), data.size()));
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
				return std::optional{std::get<I>(parser.parsers)(
				    base.subspan(offsets[I], frg::min(sizeof(T), base.size() - offsets[I]))
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
