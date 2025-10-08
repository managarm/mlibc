#include <assert.h>
#include <langinfo.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

int main() {
	wchar_t c = 0xC9;
	char buf[MB_LEN_MAX] = { 0 };
	setlocale(LC_ALL, "");
	if (snprintf(buf, MB_LEN_MAX, "%lc", c) < 0)
		return -1;

	assert((unsigned char) buf[0] == 0xc3 && (unsigned char) buf[1] == 0x89
			&& buf[2] == '\0' && buf[3] == '\0');

	locale_t fake = newlocale(LC_ALL_MASK, "swamp german", 0);
	assert(fake == 0);

	locale_t german = newlocale(LC_ALL_MASK, "de_DE.utf8", 0);
	assert(german != 0);

	char *decimal_point = nl_langinfo_l(DECIMAL_POINT, german);
	fprintf(stderr, "'%s'\n", decimal_point);
	assert(!strcmp(",", decimal_point));
	char *tousands_sep = nl_langinfo_l(THOUSEP, german);
	fprintf(stderr, "'%s'\n", tousands_sep);
	assert(!strcmp(".", tousands_sep));
	char *yesexpr = nl_langinfo_l(YESEXPR, german);
	fprintf(stderr, "'%s'\n", yesexpr);
	assert(!strcmp("^[+1jJyY]", yesexpr));
	char *noexpr = nl_langinfo_l(NOEXPR, german);
	fprintf(stderr, "'%s'\n", noexpr);
	assert(!strcmp("^[-0nN]", noexpr));
	char *currency_symbol = nl_langinfo_l(CURRENCY_SYMBOL, german);
	fprintf(stderr, "'%s'\n", currency_symbol);
	assert(!strcmp("€", currency_symbol));
	char *crncystr = nl_langinfo_l(CRNCYSTR, german);
	fprintf(stderr, "'%s'\n", crncystr);
	assert(!strcmp("+€", crncystr));
	char *abday_1 = nl_langinfo_l(ABDAY_1, german);
	fprintf(stderr, "'%s'\n", abday_1);
	assert(!strcmp("So", abday_1));
	char *day_1 = nl_langinfo_l(DAY_1, german);
	fprintf(stderr, "'%s'\n", day_1);
	assert(!strcmp("Sonntag", day_1));

	freelocale(german);

	locale_t posix = newlocale(LC_ALL_MASK, "POSIX", 0);
	assert(posix != 0);

	decimal_point = nl_langinfo_l(DECIMAL_POINT, posix);
	fprintf(stderr, "'%s'\n", decimal_point);
	assert(!strcmp(".", decimal_point));
	tousands_sep = nl_langinfo_l(THOUSEP, posix);
	fprintf(stderr, "'%s'\n", tousands_sep);
	assert(!strcmp("", tousands_sep));
	yesexpr = nl_langinfo_l(YESEXPR, posix);
	fprintf(stderr, "'%s'\n", yesexpr);
	assert(!strcmp("^[yY]", yesexpr));
	noexpr = nl_langinfo_l(NOEXPR, posix);
	fprintf(stderr, "'%s'\n", noexpr);
	assert(!strcmp("^[nN]", noexpr));
	currency_symbol = nl_langinfo_l(CURRENCY_SYMBOL, posix);
	fprintf(stderr, "'%s'\n", currency_symbol);
	assert(!strcmp("", currency_symbol));
	crncystr = nl_langinfo_l(CRNCYSTR, posix);
	fprintf(stderr, "'%s'\n", crncystr);
	assert(!strcmp("-", crncystr));
	abday_1 = nl_langinfo_l(ABDAY_1, posix);
	fprintf(stderr, "'%s'\n", abday_1);
	assert(!strcmp("Sun", abday_1));
	day_1 = nl_langinfo_l(DAY_1, posix);
	fprintf(stderr, "'%s'\n", day_1);
	assert(!strcmp("Sunday", day_1));

	freelocale(posix);

	return 0;
}
