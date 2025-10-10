#ifndef MLIBC_LOCALE
#define MLIBC_LOCALE

#include <array>
#include <bits/nl_item.h>
#include <mlibc/allocator.hpp>

namespace mlibc {

struct nl_numeric {
	frg::string<MemoryAllocator> decimal_point{getAllocator(), "."};
	frg::string<MemoryAllocator> thousands_sep{getAllocator(), ""};
	unsigned grouping = 0;
	wchar_t numeric_decimal_point_wc = L'.';
	wchar_t numeric_thousands_sep_wc = L'\0';
	frg::string<MemoryAllocator> codeset{getAllocator(), "UTF-8"};
};

struct nl_messages {
	frg::string<MemoryAllocator> yesexpr{getAllocator(), "^[yY]"};
	frg::string<MemoryAllocator> noexpr{getAllocator(), "^[nN]"};
	frg::string<MemoryAllocator> yesstr{getAllocator()};
	frg::string<MemoryAllocator> nostr{getAllocator()};
	frg::string<MemoryAllocator> codeset{getAllocator()};
};

struct nl_monetary {
	frg::string<MemoryAllocator> int_curr_symbol{getAllocator()};
	frg::string<MemoryAllocator> currency_symbol{getAllocator(), ""};
	frg::string<MemoryAllocator> mon_decimal_point{getAllocator()};
	frg::string<MemoryAllocator> mon_thousands_sep{getAllocator()};
	unsigned mon_grouping;
	frg::string<MemoryAllocator> positive_sign{getAllocator()};
	frg::string<MemoryAllocator> negative_sign{getAllocator()};
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	uint16_t p_sep_by_space;
	char n_cs_precedes;
	uint16_t n_sep_by_space;
	uint32_t p_sign_posn;
	uint32_t n_sign_posn;
	frg::string<MemoryAllocator> crncystr{getAllocator(), "-"};
};

struct nl_time {
	std::array<frg::string_view, 7> abday{{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}};
	std::array<frg::string_view, 7> day{
		"Sunday",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday",
	};
	std::array<frg::string_view, 12> abmon{};
	std::array<frg::string_view, 12> mon{};
	std::array<frg::string_view, 2> am_pm{};
	frg::string<MemoryAllocator> d_t_fmt{getAllocator()};
	frg::string<MemoryAllocator> d_fmt{getAllocator()};
	frg::string<MemoryAllocator> t_fmt{getAllocator()};
	frg::string<MemoryAllocator> t_fmt_ampm{getAllocator()};
	frg::string<MemoryAllocator> era{getAllocator()};
	frg::string<MemoryAllocator> era_year{getAllocator()};
	frg::string<MemoryAllocator> era_d_fmt{getAllocator()};
	frg::string<MemoryAllocator> alt_digits{getAllocator()};
	frg::string<MemoryAllocator> era_d_t_fmt{getAllocator()};
	frg::string<MemoryAllocator> era_t_fmt{getAllocator()};
};

locale_t loadLocale(int category, const char *name, locale_t base);
locale_t useThreadLocalLocale(locale_t loc);
void freeLocale(locale_t loc);

char *nl_langinfo(nl_item item);
char *nl_langinfo_l(nl_item item, locale_t l);

} // namespace mlibc

#endif // MLIBC_LOCALE
