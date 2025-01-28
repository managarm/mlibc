
#ifndef _LOCALE_H
#define _LOCALE_H

#include <mlibc-config.h>

#include <bits/null.h>

#define LC_ALL 1
#define LC_COLLATE 2
#define LC_CTYPE 3
#define LC_MONETARY 4
#define LC_NUMERIC 5
#define LC_TIME 6
#define LC_MESSAGES 7
#define LC_MEASUREMENT 11

#define LC_GLOBAL_LOCALE ((locale_t) -1L)

#define LC_CTYPE_MASK (1<<LC_CTYPE)
#define LC_NUMERIC_MASK (1<<LC_NUMERIC)
#define LC_TIME_MASK (1<<LC_TIME)
#define LC_COLLATE_MASK (1<<LC_COLLATE)
#define LC_MONETARY_MASK (1<<LC_MONETARY)
#define LC_MESSAGES_MASK (1<<LC_MESSAGES)
#define LC_MEASUREMENT_MASK (1<<LC_MEASUREMENT)
#define LC_ALL_MASK 0x7FFFFFFF

#ifdef __cplusplus
extern "C" {
#endif

struct lconv {
	char *decimal_point;
	char *thousands_sep;
	char *grouping;
	char *mon_decimal_point;
	char *mon_thousands_sep;
	char *mon_grouping;
	char *positive_sign;
	char *negative_sign;
	char *currency_symbol;
	char frac_digits;
	char p_cs_precedes;
	char n_cs_precedes;
	char p_sep_by_space;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	char *int_curr_symbol;
	char int_frac_digits;
	char int_p_cs_precedes;
	char int_n_cs_precedes;
	char int_p_sep_by_space;
	char int_n_sep_by_space;
	char int_p_sign_posn;
	char int_n_sign_posn;
};

#ifndef __MLIBC_ABI_ONLY

/* [C11/7.11.1] setlocale() function */

char *setlocale(int __category, const char *__locale);

/* [C11/7.11.2] Locale inquiry function */

struct lconv *localeconv(void);

#endif /* !__MLIBC_ABI_ONLY */

/* posix extension */

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_locale.h>
#endif /* __MLIBC_POSIX_OPTION */

#ifdef __cplusplus
}
#endif

#endif /* _LOCALE_H */

