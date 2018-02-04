
#include <wctype.h>
#include <bits/ensure.h>

// [C11/7.30.2.2] Extensible wide character classification functions.

wctype_t wctype(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int iswctype(wint_t, wctype_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// [C11/7.30.3] Wide character case mapping utilities.

wint_t towlower(wint_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

wint_t towupper(wint_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

