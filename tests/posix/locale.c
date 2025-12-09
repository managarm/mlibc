#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <langinfo.h>
#include <limits.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

int main() {
	char buf[64] = { 0 };
	wint_t c = 0xC9;
	char *locale = setlocale(LC_ALL, "");
	assert(locale && strlen(locale));
	if (snprintf(buf, MB_LEN_MAX, "%lc", c) < 0)
		return -1;

	assert((unsigned char) buf[0] == 0xc3 && (unsigned char) buf[1] == 0x89
			&& buf[2] == '\0' && buf[3] == '\0');

	locale_t fake = newlocale(LC_ALL_MASK, "not german", 0);
	assert(fake == 0);
	assert(errno == ENOENT);

	fake = newlocale(INT_MAX, "irrelevant", 0);
	assert(fake == 0);
	assert(errno == EINVAL);

	locale_t german = newlocale(LC_ALL_MASK, "de_DE.utf8", 0);
	assert(german != 0);

	locale_t duploc = duplocale(german);
	assert(duploc);

	duploc = newlocale(LC_ALL_MASK, "en_US.utf8", duploc);
	assert(duploc);
#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
	assert(!strcmp(getlocalename_l(LC_ALL, duploc), "en_US.utf8"));
#endif
	freelocale(duploc);

	locale_t posix = newlocale(LC_ALL_MASK, "POSIX", 0);
	assert(posix != 0);

	locale = setlocale(LC_ALL, "C");
	assert(locale && strlen(locale));

	char *decimal_point = nl_langinfo_l(DECIMAL_POINT, german);
	assert(!strcmp(",", decimal_point));
	char *thousands_sep = nl_langinfo_l(THOUSEP, german);
	assert(!strcmp(".", thousands_sep));
	char *yesexpr = nl_langinfo_l(YESEXPR, german);
	assert(!strcmp("^[+1jJyY]", yesexpr));
	char *noexpr = nl_langinfo_l(NOEXPR, german);
	assert(!strcmp("^[-0nN]", noexpr));
	char *currency_symbol = nl_langinfo_l(CURRENCY_SYMBOL, german);
	assert(!strcmp("€", currency_symbol));
	char *crncystr = nl_langinfo_l(CRNCYSTR, german);
	assert(!strcmp("+€", crncystr));
	char *abday_1 = nl_langinfo_l(ABDAY_1, german);
	assert(!strcmp("So", abday_1));
	char *day_1 = nl_langinfo_l(DAY_1, german);
	assert(!strcmp("Sonntag", day_1));
	char *day_7 = nl_langinfo_l(DAY_7, german);
	assert(!strcmp("Samstag", day_7));
	char *pm = nl_langinfo_l(PM_STR, german);
	assert(!strcmp("", pm));

	char lower = tolower_l('A', german);
	assert(lower == 'a');
	char upper = toupper_l(lower, german);
	assert(upper == 'A');
	wchar_t wlower = towlower_l(L'Ä', german);
	assert(wlower == L'ä');

	assert(isblank_l(' ', german) != 0);
	assert(isblank_l('\t', german) != 0);
	assert(isblank_l('\n', german) == 0);
	assert(isblank_l('a', german) == 0);
	assert(isblank_l('1', german) == 0);
	assert(isblank_l('\v', german) == 0);
	assert(isblank_l('\r', german) == 0);

	assert(isalpha_l('a', german) != 0);
	assert(islower_l('a', german) != 0);
	assert(isupper_l('a', german) == 0);
	assert(isalpha_l('1', german) == 0);

	assert(isblank_l(' ', posix) != 0);
	assert(isblank_l('\t', posix) != 0);
	assert(isblank_l('\n', posix) == 0);
	assert(isblank_l('a', posix) == 0);
	assert(isblank_l('1', posix) == 0);
	assert(isblank_l('\v', posix) == 0);
	assert(isblank_l('\r', posix) == 0);

	uselocale(german);

	assert(iswalpha_l(L'ß', german));
	assert(iswlower_l(L'ß', german));
	assert(!iswalpha_l(L'ß', posix));

	wlower = towlower_l(L'Ä', german);
	assert(wlower == L'ä');

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%2.2f", 12.34);
	assert(!strcmp("12,34", buf));

	assert(strtod_l("    	12,34", NULL, german) == 12.34);

	wctype_t wctype = wctype_l("alpha", german);
	assert(wctype);
	assert(iswctype_l(L'ß', wctype, german));
	assert(!iswctype_l(L'÷', wctype, german));
	assert(!iswctype_l(L'❔', wctype, german));
	wctype = wctype_l("graph", german);
	assert(wctype);
	assert(iswctype_l(L'ß', wctype, german));
	assert(iswctype_l(L'÷', wctype, german));
	assert(iswctype_l(L'❔', wctype, german));
	assert(!iswctype_l(L'\x91', wctype, german));
	assert(!iswctype_l(WEOF, wctype, german));

	uselocale(LC_GLOBAL_LOCALE);
	freelocale(german);

	snprintf(buf, sizeof(buf), "%2.2f", 12.34);
	assert(!strcmp("12.34", buf));

	decimal_point = nl_langinfo_l(DECIMAL_POINT, posix);
	assert(!strcmp(".", decimal_point));
	thousands_sep = nl_langinfo_l(THOUSEP, posix);
	assert(!strcmp("", thousands_sep));
	yesexpr = nl_langinfo_l(YESEXPR, posix);
	assert(!strcmp("^[yY]", yesexpr));
	noexpr = nl_langinfo_l(NOEXPR, posix);
	assert(!strcmp("^[nN]", noexpr));
	currency_symbol = nl_langinfo_l(CURRENCY_SYMBOL, posix);
	assert(!strcmp("", currency_symbol));
	crncystr = nl_langinfo_l(CRNCYSTR, posix);
	assert(!strcmp("-", crncystr));
	abday_1 = nl_langinfo_l(ABDAY_1, posix);
	assert(!strcmp("Sun", abday_1));
	day_1 = nl_langinfo_l(DAY_1, posix);
	assert(!strcmp("Sunday", day_1));
	day_7 = nl_langinfo_l(DAY_7, posix);
	assert(!strcmp("Saturday", day_7));
	pm = nl_langinfo_l(PM_STR, posix);
	assert(!strcmp("PM", pm));

	posix = newlocale(LC_NUMERIC_MASK, "de_DE", posix);
	assert(posix != (locale_t) 0);

	decimal_point = nl_langinfo_l(DECIMAL_POINT, posix);
	assert(!strcmp(",", decimal_point));
	thousands_sep = nl_langinfo_l(THOUSEP, posix);
	assert(!strcmp(".", thousands_sep));
	yesexpr = nl_langinfo_l(YESEXPR, posix);
	assert(!strcmp("^[yY]", yesexpr));
	noexpr = nl_langinfo_l(NOEXPR, posix);
	assert(!strcmp("^[nN]", noexpr));

	freelocale(posix);

	locale = setlocale(LC_ALL, "de_DE");
	assert(locale && strlen(locale));

	thousands_sep = nl_langinfo(THOUSEP);
	assert(!strcmp(".", thousands_sep));
	decimal_point = nl_langinfo(DECIMAL_POINT);
	assert(!strcmp(",", decimal_point));
	pm = nl_langinfo(PM_STR);
	assert(!strcmp("", pm));

	lower = tolower('A');
	assert(lower == 'a');
	upper = toupper(lower);
	assert(upper == 'A');

	assert(isalpha('z'));
	assert(!isalpha('z' + 1));

	snprintf(buf, sizeof(buf), "%'d", 1234567);
	assert(!strcmp("1.234.567", buf));

	snprintf(buf, sizeof(buf), "%'g", 12345.67);
	// assert(!strcmp("12.345,7", buf));

	locale = setlocale(LC_NUMERIC, "C");
	assert(locale && strlen(locale));

	thousands_sep = nl_langinfo(THOUSEP);
	assert(!strcmp("", thousands_sep));
	decimal_point = nl_langinfo(DECIMAL_POINT);
	assert(!strcmp(".", decimal_point));
	pm = nl_langinfo(PM_STR);
	assert(!strcmp("", pm));

	locale = setlocale(LC_TIME, "ru_RU.utf8");
	assert(locale && strlen(locale));

	day_7 = nl_langinfo(DAY_7);
	assert(!strcmp("Суббота", day_7));

	locale = setlocale(LC_MONETARY, "en_US.utf8");
	assert(locale && strlen(locale));

	locale = setlocale(LC_MONETARY, NULL);
	assert(!strcmp(locale, "en_US.utf8"));

	locale = setlocale(LC_ALL, NULL);
	assert(!strcmp(locale, "LC_CTYPE=de_DE;LC_NUMERIC=C;LC_TIME=ru_RU.utf8;LC_COLLATE=de_DE;LC_MONETARY=en_US.utf8;LC_MESSAGES=de_DE;LC_PAPER=de_DE;LC_NAME=de_DE;LC_ADDRESS=de_DE;LC_TELEPHONE=de_DE;LC_MEASUREMENT=de_DE;LC_IDENTIFICATION=de_DE"));
#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
	assert(!strcmp(locale, getlocalename_l(LC_ALL, LC_GLOBAL_LOCALE)));
#endif

	locale_t roundtrip = newlocale(LC_ALL, locale, NULL);
	assert(roundtrip);
	freelocale(roundtrip);

	currency_symbol = nl_langinfo(CURRENCY_SYMBOL);
	assert(!strcmp("$", currency_symbol));

	locale = setlocale(LC_ALL, "C");
	assert(locale && strlen(locale));

	pm = nl_langinfo(PM_STR);
	assert(!strcmp("PM", pm));

	return 0;
}
