
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <mlibc/locale.hpp>
#include <frg/optional.hpp>

char *setlocale(int category, const char *name) {
	const char *usedName = name;

	if (*name == '\0') {
		auto lc_all = getenv("LC_ALL");

		if (lc_all) {
			usedName = lc_all;
		} else {
			int categoryEnvMask = 0;

			auto applyEnvCategory = [&](const char *categoryName, int val) {
				if (auto e = getenv(categoryName); e && strlen(e)) {
					if (!mlibc::applyCategory(val, e, mlibc::getGlobalLocale())) {
						return false;
					} else {
						categoryEnvMask |= (1 << val);
					}
				}
				return true;
			};

			auto applyLang = [&](int val, const char *locale) {
				if (!((1 << val) & categoryEnvMask))
					if (!mlibc::applyCategory(val, locale, mlibc::getGlobalLocale()))
						return false;

				return true;
			};

			if (!applyEnvCategory("LC_CTYPE", LC_CTYPE)) return nullptr;
			if (!applyEnvCategory("LC_NUMERIC", LC_NUMERIC)) return nullptr;
			if (!applyEnvCategory("LC_TIME", LC_TIME)) return nullptr;
			if (!applyEnvCategory("LC_COLLATE", LC_COLLATE)) return nullptr;
			if (!applyEnvCategory("LC_MONETARY", LC_MONETARY)) return nullptr;
			if (!applyEnvCategory("LC_MESSAGES", LC_MESSAGES)) return nullptr;
			if (!applyEnvCategory("LC_PAPER", LC_PAPER)) return nullptr;
			if (!applyEnvCategory("LC_NAME", LC_NAME)) return nullptr;
			if (!applyEnvCategory("LC_ADDRESS", LC_ADDRESS)) return nullptr;
			if (!applyEnvCategory("LC_TELEPHONE", LC_TELEPHONE)) return nullptr;
			if (!applyEnvCategory("LC_MEASUREMENT", LC_MEASUREMENT)) return nullptr;
			if (!applyEnvCategory("LC_IDENTIFICATION", LC_IDENTIFICATION)) return nullptr;

			auto lang = getenv("LANG");
			if (lang) {
				if (!applyLang(LC_CTYPE, lang)) return nullptr;
				if (!applyLang(LC_NUMERIC, lang)) return nullptr;
				if (!applyLang(LC_TIME, lang)) return nullptr;
				if (!applyLang(LC_COLLATE, lang)) return nullptr;
				if (!applyLang(LC_MONETARY, lang)) return nullptr;
				if (!applyLang(LC_MESSAGES, lang)) return nullptr;
				if (!applyLang(LC_PAPER, lang)) return nullptr;
				if (!applyLang(LC_NAME, lang)) return nullptr;
				if (!applyLang(LC_ADDRESS, lang)) return nullptr;
				if (!applyLang(LC_TELEPHONE, lang)) return nullptr;
				if (!applyLang(LC_MEASUREMENT, lang)) return nullptr;
				if (!applyLang(LC_IDENTIFICATION, lang)) return nullptr;
			} else {
				return nullptr;
			}
		}

		return mlibc::getGlobalLocale()->localeName.data();
	}

	if(category == LC_ALL) {
		if (!mlibc::applyCategory(LC_CTYPE, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_NUMERIC, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_TIME, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_COLLATE, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_MONETARY, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_MESSAGES, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_PAPER, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_NAME, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_ADDRESS, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_TELEPHONE, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_MEASUREMENT, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		if (!mlibc::applyCategory(LC_IDENTIFICATION, usedName, mlibc::getGlobalLocale()))
			return nullptr;

		mlibc::getGlobalLocale()->localeName = {usedName, getAllocator()};
		return mlibc::getGlobalLocale()->localeName.data();
	}else{
		if (!mlibc::applyCategory(category, usedName, mlibc::getGlobalLocale()))
			return nullptr;
		return mlibc::getGlobalLocale()->localeName.data();
	}
}

namespace {
	lconv effective_lc;
} // namespace

struct lconv *localeconv(void) {
	auto cur = mlibc::getActiveLocale();

	// Numeric locale.
	effective_lc.decimal_point = const_cast<char *>(cur->numeric.get(DECIMAL_POINT).asString().data());
	effective_lc.thousands_sep = const_cast<char *>(cur->numeric.get(THOUSANDS_SEP).asString().data());
	effective_lc.grouping = const_cast<char *>(reinterpret_cast<const char *>(cur->monetary.get(GROUPING).asByteSpan().data()));

	// Monetary locale.
	effective_lc.mon_decimal_point = const_cast<char *>(cur->monetary.get(MON_DECIMAL_POINT).asString().data());
	effective_lc.mon_thousands_sep = const_cast<char *>(cur->monetary.get(MON_THOUSANDS_SEP).asString().data());
	effective_lc.mon_grouping = const_cast<char *>(reinterpret_cast<const char *>(cur->monetary.get(MON_GROUPING).asByteSpan().data()));
	effective_lc.positive_sign = const_cast<char *>(cur->monetary.get(POSITIVE_SIGN).asString().data());
	effective_lc.negative_sign = const_cast<char *>(cur->monetary.get(NEGATIVE_SIGN).asString().data());
	effective_lc.currency_symbol = const_cast<char *>(cur->monetary.get(CURRENCY_SYMBOL).asString().data());
	effective_lc.frac_digits = cur->monetary.get(FRAC_DIGITS).asUint32();
	effective_lc.p_cs_precedes = cur->monetary.get(P_CS_PRECEDES).asUint32();
	effective_lc.n_cs_precedes = cur->monetary.get(N_CS_PRECEDES).asUint32();
	effective_lc.p_sep_by_space = cur->monetary.get(P_SEP_BY_SPACE).asUint32();
	effective_lc.n_sep_by_space = cur->monetary.get(N_SEP_BY_SPACE).asUint32();
	effective_lc.p_sign_posn = cur->monetary.get(P_SIGN_POSN).asUint32();
	effective_lc.n_sign_posn = cur->monetary.get(N_SIGN_POSN).asUint32();
	effective_lc.int_curr_symbol = const_cast<char *>(cur->monetary.get(INT_CURR_SYMBOL).asString().data());
	effective_lc.int_frac_digits = cur->monetary.get(INT_FRAC_DIGITS).asUint32();
	effective_lc.int_p_cs_precedes = cur->monetary.get(INT_P_CS_PRECEDES).asUint32();
	effective_lc.int_n_cs_precedes = cur->monetary.get(INT_N_CS_PRECEDES).asUint32();
	effective_lc.int_p_sep_by_space = cur->monetary.get(INT_P_SEP_BY_SPACE).asUint32();
	effective_lc.int_n_sep_by_space = cur->monetary.get(INT_N_SEP_BY_SPACE).asUint32();
	effective_lc.int_p_sign_posn = cur->monetary.get(INT_P_SIGN_POSN).asUint32();
	effective_lc.int_n_sign_posn = cur->monetary.get(INT_N_SIGN_POSN).asUint32();

	return &effective_lc;
}
