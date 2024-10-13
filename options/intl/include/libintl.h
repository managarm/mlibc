
#ifndef _LIBINTL_H
#define _LIBINTL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

char *gettext(const char *__msgid);
char *dgettext(const char *__domainname, const char *__msgid);
char *dcgettext(const char *__domainname, const char *__msgid,
		int __category);

char *ngettext(const char *__msgid, const char *__msgid_plural, unsigned long int __n);
char *dngettext(const char *__domainname, const char *__msgid,
		const char *__msgid_plural, unsigned long int __n);
char *dcngettext(const char *__domainname, const char *__msgid,
		const char *__msgid_plural, unsigned long int __n, int __category);

char *textdomain(const char *__domainname);
char *bindtextdomain(const char *__domainname, const char *__dirname);
char *bind_textdomain_codeset(const char *__domainname, const char *__codeset);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _LIBINTL_H */

