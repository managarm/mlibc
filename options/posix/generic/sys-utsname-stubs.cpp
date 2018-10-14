
#include <string.h>
#include <sys/utsname.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

int uname(struct utsname *p) {
	__ensure(p);
	mlibc::infoLogger() << "\e[31mmlibc: uname() returns static information\e[39m" << frg::endlog;
	strcpy(p->sysname, "managarm");
	strcpy(p->nodename, "foo");
	strcpy(p->release, "0.1.0");
	strcpy(p->version, "0.1.0 vanilla");
	strcpy(p->machine, "x86_64");
	return 0;
}

