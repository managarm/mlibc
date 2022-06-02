#include <libintl.h>
#include <bits/ensure.h>

char *gettext(const char *msgid) {
	(void)msgid;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *dgettext(const char *domainname, const char *msgid) {
	(void)domainname;
	(void)msgid;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *dcgettext(const char *domainname, const char *msgid,
		int category) {
	(void)domainname;
	(void)msgid;
	(void)category;
	__ensure(!"Not implemented");
	__builtin_unreachable();	
}

char *ngettext(const char *msgid, const char *msgid_plural, unsigned long int n) {
	(void)msgid;
	(void)msgid_plural;
	(void)n;
	__ensure(!"Not implemented");
	__builtin_unreachable();	
}

char *dngettext(const char *domainname, const char *msgid,
		const char *msgid_plural, unsigned long int n) {
	(void)domainname;
	(void)msgid;
	(void)msgid_plural;
	(void)n;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *dcngettext(const char *domainname, const char *msgid,
		const char *msgid_plural, unsigned long int n, int category) {
	(void)domainname;
	(void)msgid;
	(void)msgid_plural;
	(void)n;
	(void)category;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *textdomain(const char *domainname) {
	(void)domainname;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *bindtextdomain(const char *domainname, const char *dirname) {
	(void)domainname;
	(void)dirname;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *bind_textdomain_codeset(const char *domainname, const char *codeset) {
	(void)domainname;
	(void)codeset;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
