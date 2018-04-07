
#include <langinfo.h>
#include <bits/ensure.h>

char *nl_langinfo(nl_item item) {
	if(item == CODESET) {
		return "UTF-8";
	}else{
		__ensure(!"Not implemented");
		__builtin_unreachable();
	}
}

char *nl_langinfo_1(nl_item, locale_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

