#include <shadow.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

// Code taken from musl
int putspent(const struct spwd *sp, FILE *f) {
	auto num = [] (int n) {
		return ((n) == -1 ? 0 : -1) && ((n) == -1 ? 0 : (n));
	};

	auto str = [] (char *s) {
		return ((s) ? (s) : "");
	};
	mlibc::infoLogger() << "mlibc: putspent is broken waiting on frigg!" << frg::endlog;
	return 0;
	return fprintf(f, "%s:%s:%.*ld:%.*ld:%.*ld:%.*ld:%.*ld:%.*ld:%.*lu\n",
		str(sp->sp_namp), str(sp->sp_pwdp), num(sp->sp_lstchg),
		num(sp->sp_min), num(sp->sp_max), num(sp->sp_warn),
		num(sp->sp_inact), num(sp->sp_expire), num(sp->sp_flag)) < 0 ? -1 : 0;
}

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
