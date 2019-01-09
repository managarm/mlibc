
#include <string.h>
#include <sys/utsname.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <internal-config.h>

int uname(struct utsname *p) {
	__ensure(p);
	mlibc::infoLogger() << "\e[31mmlibc: uname() returns static information\e[39m" << frg::endlog;
	strcpy(p->sysname, MLIBC_SYSTEM_NAME);
	strcpy(p->nodename, "?");
	strcpy(p->release, "?");
	strcpy(p->version, "?");
	strcpy(p->machine, "x86_64");
	return 0;
}

