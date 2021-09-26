#include <shadow.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

// Code taken from musl
#define NUM(n) ((n) == -1 ? 0 : -1), ((n) == -1 ? 0 : (n))

int putspent(const struct spwd *sp, FILE *f) {
	auto str = [] (char *s) {
		return ((s) ? (s) : "");
	};
	return fprintf(f, "%s:%s:%.*ld:%.*ld:%.*ld:%.*ld:%.*ld:%.*ld:%.*lu\n",
		str(sp->sp_namp), str(sp->sp_pwdp), NUM(sp->sp_lstchg),
		NUM(sp->sp_min), NUM(sp->sp_max), NUM(sp->sp_warn),
		NUM(sp->sp_inact), NUM(sp->sp_expire), NUM(sp->sp_flag)) < 0 ? -1 : 0;
}
#undef NUM

int lckpwdf(void) {
	mlibc::infoLogger() << "mlibc: lckpwdf is unimplemented like musl" << frg::endlog;
	return 0;
}

int ulckpwdf(void) {
	mlibc::infoLogger() << "mlibc: ulckpwdf is unimplemented like musl" << frg::endlog;
	return 0;
}

struct spwd *getspnam(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void endspent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
