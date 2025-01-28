
#include <limits.h>
#include <locale.h>
#include <string.h>

#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <frg/optional.hpp>

namespace {
	// Values of the C locale are defined by the C standard.
	constexpr lconv c_lconv = {
		const_cast<char *>("."), // decimal_point
		const_cast<char *>(""), // thousands_sep
		const_cast<char *>(""), // grouping
		const_cast<char *>(""), // mon_decimal_point
		const_cast<char *>(""), // mon_thousands_sep
		const_cast<char *>(""), // mon_grouping
		const_cast<char *>(""), // positive_sign
		const_cast<char *>(""), // negative_sign
		const_cast<char *>(""), // currency_symbol
		CHAR_MAX, // frac_digits
		CHAR_MAX, // p_cs_precedes
		CHAR_MAX, // n_cs_precedes
		CHAR_MAX, // p_sep_by_space
		CHAR_MAX, // n_sep_by_space
		CHAR_MAX, // p_sign_posn
		CHAR_MAX, // n_sign_posn
		const_cast<char *>(""), // int_curr_symbol
		CHAR_MAX, // int_frac_digits
		CHAR_MAX, // int_p_cs_precedes
		CHAR_MAX, // int_n_cs_precedes
		CHAR_MAX, // int_p_sep_by_space
		CHAR_MAX, // int_n_sep_by_space
		CHAR_MAX, // int_p_sign_posn
		CHAR_MAX // int_n_sign_posn
	};
}

namespace mlibc {
	struct locale_description {
		// Identifier of this locale. used in setlocale().
		const char *name;
		lconv lc;
	};

	constinit const locale_description c_locale{
		.name = "C",
		.lc = c_lconv
	};

	constinit const locale_description posix_locale{
		.name = "POSIX",
		.lc = c_lconv
	};

	const locale_description *query_locale_description(const char *name) {
		if(!strcmp(name, "C"))
			return &c_locale;
		if(!strcmp(name, "POSIX"))
			return &posix_locale;
		return nullptr;
	}

	const locale_description *collate_facet;
	const locale_description *ctype_facet;
	const locale_description *monetary_facet;
	const locale_description *numeric_facet;
	const locale_description *time_facet;
	const locale_description *messages_facet;
}

[[gnu::constructor]]
static void init_locale() {
	mlibc::collate_facet = &mlibc::c_locale;
	mlibc::ctype_facet = &mlibc::c_locale;
	mlibc::monetary_facet = &mlibc::c_locale;
	mlibc::numeric_facet = &mlibc::c_locale;
	mlibc::time_facet = &mlibc::c_locale;
	mlibc::messages_facet = &mlibc::c_locale;
}

char *setlocale(int category, const char *name) {
	if(category == LC_ALL) {
		// ´TODO: Implement correct return value when categories differ.
		auto current_desc = mlibc::collate_facet;
		__ensure(current_desc == mlibc::ctype_facet);
		__ensure(current_desc == mlibc::monetary_facet);
		__ensure(current_desc == mlibc::numeric_facet);
		__ensure(current_desc == mlibc::time_facet);
		__ensure(current_desc == mlibc::messages_facet);

		if(name) {
			// Our default C locale is the C locale.
			if(!strlen(name))
				name = "C";

			auto new_desc = mlibc::query_locale_description(name);
			if(!new_desc) {
				mlibc::infoLogger() << "mlibc: Locale " << name
						<< " is not supported" << frg::endlog;
				return nullptr;
			}

			mlibc::collate_facet = new_desc;
			mlibc::ctype_facet = new_desc;
			mlibc::monetary_facet = new_desc;
			mlibc::numeric_facet = new_desc;
			mlibc::time_facet = new_desc;
			mlibc::messages_facet = new_desc;
		}
		return const_cast<char *>(current_desc->name);
	}else{
		const mlibc::locale_description **facet_ptr;
		switch(category) {
		case LC_COLLATE:
			facet_ptr = &mlibc::collate_facet;
			break;
		case LC_CTYPE:
			facet_ptr = &mlibc::ctype_facet;
			break;
		case LC_MONETARY:
			facet_ptr = &mlibc::monetary_facet;
			break;
		case LC_NUMERIC:
			facet_ptr = &mlibc::numeric_facet;
			break;
		case LC_TIME:
			facet_ptr = &mlibc::time_facet;
			break;
		case LC_MESSAGES:
			facet_ptr = &mlibc::messages_facet;
			break;
		default:
			mlibc::infoLogger() << "mlibc: Unexpected value " << category
					<< " for category in setlocale()" << frg::endlog;
			return nullptr;
		}

		auto current_desc = *facet_ptr;
		if(name) {
			// Our default C locale is the C locale.
			if(!strlen(name))
				name = "C";

			auto new_desc = mlibc::query_locale_description(name);
			if(!new_desc) {
				mlibc::infoLogger() << "mlibc: Locale " << name
						<< " is not supported" << frg::endlog;
				return nullptr;
			}

			*facet_ptr = new_desc;
		}
		return const_cast<char *>(current_desc->name);
	}
}

namespace {
	lconv effective_lc;
}

struct lconv *localeconv(void) {
	// Numeric locale.
	const auto &numeric_lc = mlibc::numeric_facet->lc;
	effective_lc.decimal_point = numeric_lc.decimal_point;
	effective_lc.thousands_sep = numeric_lc.thousands_sep;
	effective_lc.grouping = numeric_lc.grouping;

	// Monetary locale.
	const auto &monetary_lc = mlibc::monetary_facet->lc;
	effective_lc.mon_decimal_point = monetary_lc.mon_decimal_point;
	effective_lc.mon_thousands_sep = monetary_lc.mon_thousands_sep;
	effective_lc.mon_grouping = monetary_lc.mon_grouping;
	effective_lc.positive_sign = monetary_lc.positive_sign;
	effective_lc.negative_sign = monetary_lc.negative_sign;
	effective_lc.currency_symbol = monetary_lc.currency_symbol;
	effective_lc.frac_digits = monetary_lc.frac_digits;
	effective_lc.p_cs_precedes = monetary_lc.p_cs_precedes;
	effective_lc.n_cs_precedes = monetary_lc.n_cs_precedes;
	effective_lc.p_sep_by_space = monetary_lc.p_sep_by_space;
	effective_lc.n_sep_by_space = monetary_lc.n_sep_by_space;
	effective_lc.p_sign_posn = monetary_lc.p_sign_posn;
	effective_lc.n_sign_posn = monetary_lc.n_sign_posn;
	effective_lc.int_curr_symbol = monetary_lc.int_curr_symbol;
	effective_lc.int_frac_digits = monetary_lc.int_frac_digits;
	effective_lc.int_p_cs_precedes = monetary_lc.int_p_cs_precedes;
	effective_lc.int_n_cs_precedes = monetary_lc.int_n_cs_precedes;
	effective_lc.int_p_sep_by_space = monetary_lc.int_p_sep_by_space;
	effective_lc.int_n_sep_by_space = monetary_lc.int_n_sep_by_space;
	effective_lc.int_p_sign_posn = monetary_lc.int_p_sign_posn;
	effective_lc.int_n_sign_posn = monetary_lc.int_n_sign_posn;

	return &effective_lc;
}
