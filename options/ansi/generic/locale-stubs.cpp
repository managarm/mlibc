
#include <locale.h>
#include <string.h>

#include <bits/ensure.h>
#include <mlibc/cxx-support.hpp>

#include <mlibc/debug.hpp>
#include <frg/optional.hpp>

struct __Mlibc_LocaleDesc {
	// identifier of this locale. used in setlocale()
	const char *locale;
};

frg::optional<__Mlibc_LocaleDesc> __mlibc_queryLocale(const char *locale) {
	// our default C locale is the C locale
	if(!strlen(locale))
		return __mlibc_queryLocale("C");

	// for now we only support the C locale
	__ensure(!strcmp(locale, "C"));

	__Mlibc_LocaleDesc desc;
	desc.locale = locale;
	return desc;
}

__Mlibc_LocaleDesc __mlibc_collateDesc;
__Mlibc_LocaleDesc __mlibc_ctypeDesc;
__Mlibc_LocaleDesc __mlibc_monetaryDesc;
__Mlibc_LocaleDesc __mlibc_numericDesc;
__Mlibc_LocaleDesc __mlibc_timeDesc;

void __mlibc_initLocale() {
	__mlibc_collateDesc = *__mlibc_queryLocale("C");
	__mlibc_ctypeDesc = *__mlibc_queryLocale("C");
	__mlibc_monetaryDesc = *__mlibc_queryLocale("C");
	__mlibc_numericDesc = *__mlibc_queryLocale("C");
	__mlibc_timeDesc = *__mlibc_queryLocale("C");
}

__Mlibc_LocaleDesc &__mlibc_currentLocale(int category) {
	switch(category) {
	case LC_COLLATE: return __mlibc_collateDesc;
	case LC_CTYPE: return __mlibc_ctypeDesc;
	case LC_MONETARY: return __mlibc_monetaryDesc;
	case LC_NUMERIC: return __mlibc_numericDesc;
	case LC_TIME: return __mlibc_timeDesc;
	default:
		__ensure(!"__mlibc_currentLocale called with bad category");
		__builtin_unreachable();
	}
}

char *setlocale(int category, const char *locale) {
	if(!(category == LC_ALL || category == LC_COLLATE || category == LC_CTYPE
			|| category == LC_MONETARY || category == LC_NUMERIC || category == LC_TIME)) {
		mlibc::infoLogger() << "mlibc: Unexpected value " << category
				<< " for category in setlocale()" << frg::endlog;
		return nullptr;
	}

	if(category == LC_ALL) {
		// ´TODO: implement correct return value when categories differ
		__Mlibc_LocaleDesc current_desc = __mlibc_collateDesc;
		__ensure(!strcmp(current_desc.locale, __mlibc_ctypeDesc.locale));
		__ensure(!strcmp(current_desc.locale, __mlibc_monetaryDesc.locale));
		__ensure(!strcmp(current_desc.locale, __mlibc_numericDesc.locale));
		__ensure(!strcmp(current_desc.locale, __mlibc_timeDesc.locale));
		if(!locale)
			return const_cast<char *>(current_desc.locale);
		
		frg::optional<__Mlibc_LocaleDesc> new_desc = __mlibc_queryLocale(locale);
		if(!new_desc)
			return nullptr;
		__mlibc_collateDesc = *new_desc;
		__mlibc_ctypeDesc = *new_desc;
		__mlibc_monetaryDesc = *new_desc;
		__mlibc_numericDesc = *new_desc;
		__mlibc_timeDesc = *new_desc;
		return const_cast<char *>(current_desc.locale);
	}

	__Mlibc_LocaleDesc current_desc = __mlibc_currentLocale(category);
	if(!locale)
		return const_cast<char *>(current_desc.locale);
	
	frg::optional<__Mlibc_LocaleDesc> new_desc = __mlibc_queryLocale(locale);
	if(!new_desc)
		return nullptr;
	__mlibc_currentLocale(category) = *new_desc;
	return const_cast<char *>(current_desc.locale);
}

struct lconv *localeconv(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

locale_t newlocale(int category_mask, const char *locale, locale_t base) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void freelocale(locale_t locobj) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

