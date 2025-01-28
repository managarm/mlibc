
#include <langinfo.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/locale.hpp>

char *nl_langinfo(nl_item item) {
	return mlibc::nl_langinfo(item);
}

char *nl_langinfo_l(nl_item, locale_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

