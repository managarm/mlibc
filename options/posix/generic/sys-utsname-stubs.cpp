
#include <sys/utsname.h>
#include <bits/ensure.h>

#include <frigg/debug.hpp>

int uname(struct utsname *p) {
	__ensure(p);
	frigg::infoLogger() << "\e[31mmlibc: uname() returns static information\e[39m" << frigg::endLog;
	strcpy(p->sysname, "managarm");
	strcpy(p->nodename, "foo");
	strcpy(p->release, "0.1.0");
	strcpy(p->version, "0.1.0 vanilla");
	strcpy(p->machine, "x86_64");
	return 0;
}

